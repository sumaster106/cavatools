#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <math.h>		// logarithms

#include "cache.h"
#include "VCache.h"	// VC lookup after SC lookup (for mutual exclusivity) 
#include "DRAM.h"		// DRAM lookup

cache_t::cache_t(const char* nam, int penalty_sc, int penalty_wb, int penalty_wb_to_dram, int ways, int sets, int row_size) {
  // Setting parameters
  name = nam;
  sc_penalty = penalty_sc;
  wb_penalty = penalty_wb;
 	wb_to_dram_penalty = penalty_wb_to_dram;
  n_ways = ways;
  n_sets = sets;
  row_size = row_size; 
    
  // Compute rest of parmeters
  n_rows = n_ways * n_sets;
  size = n_rows * row_size;
  
  // Bits
  bits_set = log2(n_sets);
  bits_offset = log2(row_size);
  bits_tag = ADDRESS_SIZE - bits_set - bits_offset;


  // Data Structures
	write_buffer = new int[WB_SIZE];
  lru = new int*[n_sets];
  s_cache = new element*[n_sets];
  
  for (int i = 0; i < n_sets; i++) {  
    s_cache[i] = new element[n_ways];
    lru[i] = new int[n_ways];
  }
  
  // Initialise cache
  flush();
}


/**
* Searches for address in SC.
* If not in SC, searches for it VC, then in WB, and finally in DRAM. 
* Updates penalty to the sum of all penalties until hit.
* Returns true if SC hit; false otherwise.
*/
bool cache_t::lookup(long addr, int &penalty) {
	// Get set and tag from address (UNSIGNED VALUES!!)
	// Convert to 32 bits address
	unsigned int dir = (int) addr;
	
	// Get tag
	unsigned int tag = dir >> (bits_set + bits_offset);
	
	// Get set
	unsigned int set = (dir << bits_tag) >> (bits_tag + bits_offset);

	// Search for tag in all set lines
	penalty = sc_penalty;
	n_accesses++;
	int line = -1;
	for (int i  = 0; i < n_ways; i++) {
		if (s_cache[set][i].tag == tag) {
			// Hit in line
			line = i;
			break;
		}
	}
	// Hit in SC (valid line)
	if (line != -1 and s_cache[set][line].valid_bit == true) {
		// Update line as MRU in LRU list
		lru_update(set, line);
		return true;
	}
		
	// Not in SC (invalid or non-present line in SC)
	n_misses++;
	
	// Search for element in Vector Cache
	bool vc_hit = false;
	// TODO vc_hit = VCACHE.vc_lookup_after_sc(dir, penalty);
	
	// Hit in VC (valid line in VC)
	if (vc_hit) {
		return false;
	}

	// Line not in SC nor in VC
	bool wb_hit = false;
	
	// Hit in WB: restored line
	wb_hit = wb_lookup(dir, penalty);
	
	// Line not in SC, nor in VC, nor in WB
	if (!wb_hit) {
	// TODO	dram_lookup(dir, penalty);
	}
	
	// Line was not in SC
	if (line == -1) {
		// Get victim line from that set
		line = lru_victimise(set, penalty);	
		// Set tag and address to new line
		s_cache[set][line].tag = tag;
		s_cache[set][line].dir = dir;
	}
	
	// Validate new line in SC
	s_cache[set][line].valid_bit = true;
	
	// Line could have been restored from WB or taken fresh from DRAM
	s_cache[set][line].dirty_bit = wb_hit;
	
	// Update line as MRU
	lru_update(set, line);
	return false;
}
	
	
// -- LRU functions (LRU: newest..oldest)

/**
* Gets victim line from set. If valid and dirty, moves line to wb. Increases penalty with wb_add()'s penalty.
* (LRU: newest..oldest)
* Returns Id of the victim line.
*/
int cache_t::lru_victimise(int set, int &penalty) {
	// Get LRU from set
	int victim = lru[set][-1];
	
	// Move every line in lru list to next position
	for (int i = n_ways-1; i > 1; i--) {
		lru[set][i] = lru[set][i-1];
	}
	
	// Move line to WB if dirty and valid
	if (s_cache[set][victim].valid_bit and s_cache[set][victim].dirty_bit) {
		penalty += wb_add(s_cache[set][victim].dir);
	}
		
	return victim;
}
	
/**
* Updates line to Most Recently Used line in the set.
*/
void cache_t::lru_update(int set, int line) {
	int old = lru[set][0];
	// Accessed line was already the newest line in the set
	if (old == line){
		return;
	}
	// Accessed line was not the newest one
	// Set line as the newest one
	lru[set][0] = line;
	
	// Move LRU lines to next position until old line's position
	int aux;
	for (int i  = 1; i < n_ways; i++) {
		aux = lru[set][i];
		lru[set][i] = old;
		old = aux;
		// Rest of the lines in LRU are already correctly positioned
		if (aux == line) {
			return;
		}
	}
}

// -- WB functions (WB: newest..oldest)

/**
* Adds replaced valid and dirty line from SC to WB.
*/
int cache_t::wb_add(int dir) {
	
	int wb_penalty = 0;
	
	// Penalty of moving 1 element to DRAM
	if (wb_occupation == WB_SIZE) {
		wb_penalty = wb_to_dram_penalty;
	}
	
	// Wait until WB stops being full (until mem. controller moves at least 1 line to DRAM)
	while (wb_occupation == WB_SIZE);
	
	// WB has room for new line
	int pos = WB_SIZE - wb_occupation - 1;
	// Add line in WB's first empty position
	write_buffer[pos] = dir;
	wb_occupation++;
	
	return wb_penalty;
}	

/**
* Searches for line in WB. If there, restores it to SC and returns true. Otherwise, returns false.
*/
bool cache_t::wb_lookup(int dir, int &penalty) {
	
	// Checks if WB was empty
	if (wb_occupation == 0) {
		return false;
	}
	
	// Search for line in WB
	int pos = -1;
	for (int i = WB_SIZE - 1; i > WB_SIZE - wb_occupation - 1; i--) {
		if (write_buffer[i] == dir) {
		 pos = i;
		 break;
		}
	}
	
	// Increase penalty due to lookup
	penalty += wb_penalty;
		
	// If line was in WB
	if (pos != -1) {
		// Restore line to SC
		wb_occupation--;
		
		// If there were more lines in WB and the restored line was not the newest one
		if (wb_occupation > 0 and  pos != WB_SIZE - wb_occupation - 1) {
			// Move newer lines to next position until the now empty pos
			int aux;
			int old = write_buffer[WB_SIZE - wb_occupation - 1];
			for (int i = WB_SIZE - wb_occupation; i <= pos; i++) {
				aux = write_buffer[i];
				write_buffer[i] = old;
				old = aux;
			}
		}	
		return true;			
	}
	
	// Line was not in WB
	return false;
}

// TODO: SC aux lookup function
/*
// Invocado desde VC para recuperar bloque vectorial de SC
bool cache_t::sc_lookup_after_vc_miss(int dir, float& penalty, bool& dirty) {
	obtiene tag y conjunto de dir
	busca tag en el conjunto
	si está y es válido:
		invalida linea					// retira bloque de SC
		return (está=true, dirty?, penalty(SC))		// envía bloque a VC

	// No está en SC
	busca dir en WB de SC:
		si está:						// línea válida y dirty en WB de SC
			WB_restore(dir, *penalty)			// elimina línea de WB
			return (está=true, dirty, penalty(SC+WB))	// lleva línea dirty a VC
			
	// No está en SC ni en WB de SC
	return (está=false, null, penalty(SC+WB))
}
*/

/**
* Initialises SC (addresses get value -1) and LRU list in every set to line IDs.
*/
void cache_t::flush() {
	// Initialize variables
  n_accesses = 0;
  n_misses = 0;
  wb_occupation = 0;
  
  // Initialise LRU list and SC stored addresses
	for (int i = 0; i < n_sets; i++) {
		for (int j = 0; j < n_ways; j++) {
			lru[i][j] = j;
			s_cache[i][j].dir = -1;
			s_cache[i][j].tag = -1;
		}
	}
	
	// Memory allocation for data sctructures
	for (int i = 0; i < n_sets; i++) {
		memset((char*)s_cache[i], 0, n_ways*sizeof(int));
		memset((char*)lru[i], 0, n_ways*sizeof(int));
	}
	memset((char*)write_buffer, 0, WB_SIZE*sizeof(int));
}


void cache_t::show() {
  fprintf(stderr, "bits_offset=%d bits_tag=%d row_size(B)=%d rows=%d ways=%d sets=%d\n",
	  bits_offset, bits_tag, row_size, n_rows, n_ways, n_sets);
}

void cache_t::print(FILE* f) {
  fprintf(f, "\n%s cache\n", name);
  if      (size >= 1024*1024)  fprintf(f, "  %3.1f MB capacity\n", size/1024.0/1024);
  else if (size >=      1024)  fprintf(f, "  %3.1f KB capacity\n", size/1024.0);
  else                         fprintf(f, "  %d B capacity\n", size);
  fprintf(f, "  %d bytes line size\n", row_size);
  fprintf(f, "  %d ways set associativity\n", n_ways);
  fprintf(f, "  %d cycles miss penalty\n", sc_penalty);
  fprintf(f, "  %d references\n", n_accesses);
  fprintf(f, "  %d misses (%5.3f%% miss rate)\n", n_misses, 100.0*n_misses/n_accesses);

}
  
