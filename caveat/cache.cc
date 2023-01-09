#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <math.h>		// for logarithms

#include "cache.h"
#include "lru_fsm_1way.h"
#include "lru_fsm_2way.h"
#include "lru_fsm_3way.h"
#include "lru_fsm_4way.h"

cache_t::cache_t(const char* nam, float sc_penalty, int ways, int sets, long row_size) 
{
  name = nam;
  penalty = sc_penalty;
  n_ways = ways;
  n_sets = sets;
  
  // Calculate rest of variables
  row_size = row_size; 
  n_rows = n_ways * n_sets;
  size = n_rows * row_size;
  
  bits_set = log2(n_sets);
  bits_offset = log2(row_size);
  bits_tag = ADDRESS_SIZE - bits_set - bits_offset;
  
  // Initialize variables
  n_accesses = 0;
  wb_occupation = 0;
  
  // Data Structures
  lru = new long*[n_sets][n_ways];
  s_cache = new element*[n_sets][n_ways];
  
  
  /*
  switch (ways) {
  case 1:  fsm = cache_fsm_1way;  break;
  case 2:  fsm = cache_fsm_2way;  break;
  case 3:  fsm = cache_fsm_3way;  break;
  case 4:  fsm = cache_fsm_4way;  break;
  default:
    fprintf(stderr, "ways=%ld only 1..4 ways implemented\n", ways);
    syscall(SYS_exit_group, -1);
  } // note fsm purposely point to [-1] 
  lg_line = lin;
  lg_rows = row;
  line = 1 << lg_line;
  rows = 1 << lg_rows;
  tag_mask = ~(line-1);
  //  row_mask =  (rows-1) << lg_line;
  row_mask =  (rows-1);
  tags = new tag_t*[ways];
  for (int k=0; k<ways; k++)
    tags[k] = new tag_t[rows];
  states = new unsigned short[rows];
  flush();
  static long place =0;
  evicted = writeable ? &place : 0;
  _refs = _misses = 0;
  _updates = _evictions = 0;
  */
}


void cache_t::flush()
{
  for (int k=0; k<ways; k++)
    memset((char*)tags[k], 0, rows*sizeof(long));
  memset((char*)states, 0, rows*sizeof(unsigned short));
}

void cache_t::show()
{
  fprintf(stderr, "bits_offset=%ld bits_rows=%ld row_size(B)=%ld rows=%ld ways=%ld sets=%d\n",
	  bits_offset, bits_rows, row_size_, n_rows, n_ways, n_sets);
}

void cache_t::print(FILE* f)
{
  fprintf(f, "%s cache\n", name);
  //long size = line * rows * ways;
  if      (size >= 1024*1024)  fprintf(f, "  %3.1f MB capacity\n", size/1024.0/1024);
  else if (size >=      1024)  fprintf(f, "  %3.1f KB capacity\n", size/1024.0);
  else                         fprintf(f, "  %ld B capacity\n", size);
  fprintf(f, "  %ld bytes line size\n", row_size);
  fprintf(f, "  %ld ways set associativity\n", n_ways);
  fprintf(f, "  %ld cycles miss penalty\n", penalty);
  fprintf(f, "  %ld references\n", n_accesses);
  //fprintf(f, "  %ld misses (%5.3f%%)\n", _misses, 100.0*_misses/_refs);

}
  
