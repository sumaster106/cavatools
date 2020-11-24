/*
  Copyright (c) 2020 Peter Hsu.  All Rights Reserved.  See LICENCE file for details.
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "caveat.h"
#include "opcodes.h"
#include "insn.h"
#include "shmfifo.h"


#define REPORT_FREQUENCY  1000000000


long report_frequency = REPORT_FREQUENCY;
time_t start_tick;
long insn_count =0;
long segments =0;
long pvr_cycles =0;
long pvr_cutoff =0;

struct fifo_t trace_buffer;
int hart;
uint64_t mem_queue[tr_memq_len];


static inline void status_report()
{
  double elapse = (clock() - start_tick) / CLOCKS_PER_SEC;
  fprintf(stderr, "\r%3.1fB insns (%ld segments) in %3.1f seconds for %3.1f MIPS",
	  insn_count/1e9, segments, elapse, insn_count/1e6/elapse);
}


void do_nothing(long pc)
{
  long next_report =report_frequency;
  int withregs =0;
  for (uint64_t tr=fifo_get(&trace_buffer); tr!=tr_eof; tr=fifo_get(&trace_buffer)) {
    if (is_mem(tr)) {
      continue;
    }
    if (is_bbk(tr)) {
      if (withregs) {
	long epc = pc + tr_number(tr);
	while (pc < epc) {
	  const struct insn_t* p = insn(pc);
	  if (p->op_rd != NOREG) {
	    long val = fifo_get(&trace_buffer);
	  }
	  pc += shortOp(p->op_code) ? 2 : 4;
	}
      }
      else
	pc += tr_number(tr);
      if (is_goto(tr))
	pc = tr_pc(tr);
      continue;
    }
    if (is_frame(tr)) {
      hart = tr_number(tr);
      pc = tr_pc(tr);
      withregs = (tr & tr_has_reg) != 0;
      fprintf(stderr, "Frame(pc=%d, mem=%d, reg=%d), hart#=%d, pc=0x%lx\n", (tr&tr_has_pc)!=0, (tr&tr_has_mem)!=0, withregs, hart, pc);
      ++segments;
      continue;
    }
    if (tr_code(tr) == tr_icount) {
      insn_count = tr_value(tr);
      if (insn_count >= next_report) {
	status_report();
	next_report += report_frequency;
      }
      continue;
    }
    /* ignore other trace record types */
  }
}


void print_timing_trace(long begin, long end)
{
  if (pvr_cycles) {
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    fprintf(stdout, "#Paraver (%02d/%02d/%02d at %02d:%02d):%ld:1(1):1:1(1:1)\n",
	    tm.tm_mday, tm.tm_mon+1, tm.tm_year, tm.tm_hour, tm.tm_min, pvr_cycles);
  }
  else {
    fprintf(stdout, "Timing trace [%lx, %lx]\n", begin, end);
  }
  long previous_time = 0;
  long cache_miss = 0;
  for (uint64_t tr=fifo_get(&trace_buffer); tr_code(tr)!=tr_eof; tr=fifo_get(&trace_buffer)) {
    long pc, now;
    if (tr_code(tr) == tr_issue) {
      pc = tr_pc(tr);
      now = previous_time + tr_number(tr);
      if (begin <= pc && pc <= end) {
	if (pvr_cycles) {
	  if (now-previous_time > pvr_cutoff) {
	    fprintf(stdout, "2:0:1:1:1:%ld:%d:%ld\n", previous_time,  0, pc);
	    fprintf(stdout, "2:0:1:1:1:%ld:%d:%ld\n",           now, 10, pc);
	  }
	}
	else {
	  while (++previous_time < now)
	    fprintf(stdout, "%18s%8ld:\n", "", previous_time);
	  if (cache_miss) {
	    fprintf(stdout, "[%016lx]", cache_miss);
	    cache_miss = 0;
	  }
	  else
	    fprintf(stdout, "%18s", "");
	  fprintf(stdout, "%8ld: ", now);
	  
	  if (pvr_cycles)
	    fprintf(stdout, "2:0:1:1:1:%ld:%ld:%ld\n", now, now-previous_time, pc);
	  print_insn(tr_pc(tr), stdout);
	}
      }
      previous_time = now;
    }
    else if (tr_code(tr) == tr_d1get) {
      cache_miss = tr_value(tr);
    }
    if (pvr_cycles) {
      if (is_dcache(tr))
	fprintf(stdout, "2:0:1:1:1:%ld:%ld:%ld\n", now, tr_clevel(tr),    tr_value(tr));
      else if (is_icache(tr))
	fprintf(stdout, "2:0:1:1:1:%ld:%ld:%ld\n", now, tr_clevel(tr)+10, tr_value(tr));
      if (now >= pvr_cycles)
	return;
    }
  }
}


void print_listing(long pc)
{
  uint64_t* memq = mem_queue;	/* help compiler allocate in register */
  long tail =0;
  int withregs =0;
  for (uint64_t tr=fifo_get(&trace_buffer); tr!=tr_eof; tr=fifo_get(&trace_buffer)) {
    if (is_mem(tr)) {
      memq[tail++] = tr_value(tr);
      continue;
    }
    if (is_bbk(tr)) {
      static char buf[1024];
      char bing = is_goto(tr) ? '@' : '!';
      long epc = pc + tr_number(tr);
      long head = 0;
      while (pc < epc) {
	const struct insn_t* p = insn(pc);
	if (memOp(p->op_code))
	  printf("%c[%016lx]", bing, memq[head++]);
	else if (insnAttr[p->op_code].unit == Unit_b && (pc+(shortOp(p->op_code)?2:4)) == epc)
	  printf("%c<%16lx>", bing, tr_pc(tr));
	else
	  printf("%c %16s ", bing, "");
	if (withregs) {
	  if (p->op_rd == NOREG && p->op_rd != 0)
	    printf("%22s", "");
	  else {
	    long val = fifo_get(&trace_buffer);
	    printf("%4s=%016lx ", regName[p->op_rd], val);
	  }
	}
	print_pc(pc, stdout);
	print_insn(pc, stdout);
	++insn_count;
	pc += shortOp(p->op_code) ? 2 : 4;
	bing = ' ';
      }
      if (is_goto(tr))
	pc = tr_pc(tr);
      tail = 0;
      continue;
    }
    if (is_frame(tr)) {
      hart = tr_number(tr);
      pc = tr_pc(tr);
      withregs = (tr & tr_has_reg) != 0;
      fprintf(stderr, "Frame(pc=%d, mem=%d, reg=%d, timing=%d), hart#=%d, pc=0x%lx\n", (tr&tr_has_pc)!=0, (tr&tr_has_mem)!=0, withregs, (tr&tr_has_timing)!=0, hart, pc);
      continue;
    }
    /* ignore other trace record types */
  }
}


long atohex(const char* p)
{
  for (long n=0; ; p++) {
    long digit;
    if ('0' <= *p && *p <= '9')
      digit = *p - '0';
    else if ('a' <= *p && *p <= 'f')
      digit = 10 + (*p - 'a');
    else if ('A' <= *p && *p <= 'F')
      digit = 10 + (*p - 'F');
    else
      return n;
    n = 16*n + digit;
  }
}


int main(int argc, const char** argv)
{
  static const char* shm_path =0;
  static int list =0;
  static int trace =0;
  static const char* see_range =0;
  
  static const char* report =0;
  static const char* paraver =0;
  static const char* cutoff =0;
  
  static struct options_t flags[] =
    {  { "--in=",	.v = &shm_path		},
       { "--list",	.f = &list		},
       { "--trace",	.f = &trace		},
       { "--see=",	.v = &see_range		},
       { "--report=",	.v = &report		},
       { "--paraver=",	.v = &paraver		},
       { "--cutoff=",	.v = &cutoff		},
       { 0					}
    };
  int numopts = parse_options(flags, argv+1);
  if (!shm_path) {
    fprintf(stderr, "usage: traceinfo --in=shm_path <other options> elf_binary\n");
    exit(0);
  }
  if (report)
    report_frequency = atoi(report);
  long entry = load_elf_binary(argv[1+numopts], 0);
  trace_init(&trace_buffer, shm_path, 1);
  start_tick = clock();
  if (list)
    print_listing(entry);
  else if (trace)
    print_timing_trace(insnSpace.base, insnSpace.bound);
  else if (paraver || see_range) {
    if (paraver) {
      pvr_cycles = atoi(paraver);
      if (cutoff)
	pvr_cutoff = atoi(cutoff);
    }
    long begin = insnSpace.base;
    long end = insnSpace.bound;
    if (see_range) {
      begin = atohex(see_range);
      const char* comma = strchr(see_range, ',');
      end = atohex(comma+1);
    }
    print_timing_trace(begin, end);
  }
  else
    do_nothing(entry);
  trace_fini(&trace_buffer);
  fprintf(stderr, "\n%ld Instructions in trace\n", insn_count);
  return 0;
}