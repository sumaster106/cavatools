/*
  Scalar Cache.
*/


#ifndef CACHE_T
#define CACHE_T


// Cache element
struct element {
	bool valid_bit;		// valid bit of the element
	bool dirty_bit;		// dirty bit of the element
	long tag;			// tag of the element
}


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
	long size;													// size in bytes
	long row_size												// row size in bytes
	float penalty;											// cache lookup penalty
	
	int n_accesses;											// number of accessed elements in the cache
	int wb_occupation;									// number of elements currently in write buffer

	// Bits
	int bits_set;												// number of bits in address for set
	int bits_offset;										// number of bits in address for offset
	int bits_tag;												// number of bits in address for tag
	
	// Data structures
	//tag_t** tags;												// cache tag array [ways][rows]
	long write_buffer[WB_SIZE];					// write buffer (queue) with victimized elements to be written in DRAM write_buffer[WB_SIZE]
	long** lru;													// LRU queue per set lru[n_sets][n_ways]
	element** s_cache;									// scalar cache itself s_cache[n_sets][n_ways]
		
	
  
  
 public:
 	
	cache_t(const char* nam, float penalty, int ways, int sets, int row_size); 
	bool SC_lookup(long addr, bool write =false);
  
  void print(FILE* f = stderr);
  void show();
  void flush();
  
  /*
  long refs() { return _refs; }
  long misses() { return _misses; }
  long updates() { return _updates; }
  long evictions() { return _evictions; }
  long penalty() { return _penalty; }
  
  void flush();
  void show();
  void print(FILE* f =stderr);
  */
};

inline bool cache_t::lookup(long addr, bool write)
{
  _refs++;
  addr >>= lg_line;		// make proper tag (ok to include index) 
  int index = addr & row_mask;
  unsigned short* state = states + index;
  struct lru_fsm_t* p = fsm + *state; // recall fsm points to [-1] 
  struct lru_fsm_t* end = p + ways;	 // hence +ways = last entry 
  struct tag_t* tag;
  bool hit = true;
  do {
    p++;
    tag = tags[p->way] + index;
    //    tag = tags + index*ways + p->way;
    if (addr == tag->addr)
      goto cache_hit;
  } while (p < end);
  hit = false;
  tag->addr = addr;
  _misses++;
  if (tag->dirty) {
    *evicted = tag->addr;	// will SEGV if not cache not writable 
    _evictions++;		// can conveniently point to your location 
    tag->dirty = 0;
  }
  else if (evicted)
    *evicted = 0;
  tag->addr = addr;
  
 cache_hit:
  *state = p->next_state;	// already multiplied by ways
  if (write) {
    tag->dirty = 1;
    _updates++;
  }
  return hit;
}


void flush_cache();
void init_cache();
void show_cache();
void print_cache(FILE* f =stderr);


#endif
