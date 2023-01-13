/*
  Scalar Cache.
*/


#ifndef CACHE_T
#define CACHE_T


// Cache element
struct element {
	bool valid_bit;		// valid bit of the element
	bool dirty_bit;		// dirty bit of the element
	int tag;		// tag of the element
	int dir;		// element keeps track of their address			
};


// Cache descriptor
class cache_t {		       
	
	// Constants
	const int WB_SIZE = 4;							// write buffer size in bytes
	const int ADDRESS_SIZE = 32;				// address size for elements								-- 32!!
	const char* name;										// cache name for printing 
	
	// Variables
	int n_rows;													// number of rows
	int n_ways;													// number of ways
	int n_sets;													// number of sets
	int size;													// size in bytes
	int row_size;											// row size in bytes
	int sc_penalty;										// cache lookup penalty
	int wb_to_dram_penalty;						// moving 1 element from WB to DRAM penalty
	int wb_penalty;										// wb lookup penalty
	
	int n_accesses;											// number of accessed elements in the cache
	int wb_occupation;									// number of elements currently in write buffer
	int n_misses; 											// number of missed accesses in the cache

	// Bits
	int bits_set;												// number of bits in address for set
	int bits_offset;										// number of bits in address for offset
	int bits_tag;												// number of bits in address for tag
	
	// Data structures
	int* write_buffer;									// write buffer (queue) with victimized elements to be written in DRAM write_buffer[WB_SIZE]
	int** lru;													// LRU queue per set lru[n_sets][n_ways]     --- LRU:newset..oldest
	element** s_cache;									// scalar cache itself s_cache[n_sets][n_ways]
		
  // Private functions
  int lru_victimise(int set, int &penalty);
  void lru_update(int set, int line);
  int wb_add(int dir);
  bool wb_lookup(int dir, int &penalty);
  void flush();
  
 public:
 	
	cache_t(const char* nam, int penalty_sc, int penalty_wb, int penalty_wb_to_dram, int ways, int sets, int row_size);
	bool lookup(long addr, int &penalty);
  void print(FILE* f = stderr);
  void show();
  
};

void flush_cache();
void init_cache();
void show_cache();
void print_cache(FILE* f =stderr);

#endif
