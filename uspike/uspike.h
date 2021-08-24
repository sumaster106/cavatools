/*
  Copyright (c) 2021 Peter Hsu.  All Rights Reserved.  See LICENCE file for details.
*/

#include <cassert>
#include <cstdint>
#include <stdio.h>
#include <string.h>

using namespace std;
void* operator new(size_t size);
void operator delete(void*) noexcept;

//#define DEBUG

/*
  Utility stuff.
*/
#define die(fmt, ...)                  { fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n\n"); abort(); }
#define dieif(bad, fmt, ...)  if (bad) { fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n\n"); abort(); }
#define quitif(bad, fmt, ...) if (bad) { fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n\n"); exit(0); }

#define diesegv()  (*(char*)0=1)
#define checkif(bad) if (!(bad)) diesegv()

extern "C" {
  long load_elf_binary(const char* file_name, int include_data);
  int elf_find_symbol(const char* name, long* begin, long* end);
  const char* elf_find_pc(long pc, long* offset);
  long initialize_stack(int argc, const char** argv, const char** envp, long entry);
  extern unsigned long low_bound, high_bound;
  void start_time(int mhz);
  double elapse_time();
  double simulated_time(long cycles);
  long emulate_brk(long addr);
  long proxy_syscall(long sysnum, long cycles, const char* name, long a0, long a1, long a2, long a3, long a4, long a5);
  int proxy_clone(int (*fn)(void*), void *interp_stack, int flags, void *arg, void *parent_tidptr, void *child_tidptr);

  struct configuration_t {
    const char* isa;
    const char* vec;
    int mhz;
    int stat;
    bool show;
    const char* gdb;
    int ecall;
  };
  extern configuration_t conf;
};

enum stop_reason { stop_normal, stop_exited, stop_breakpoint };

enum stop_reason interpreter(class cpu_t* mycpu, long number);
void status_report();
class cpu_t* initial_cpu(long entry, long sp);
class cpu_t* find_cpu(int tid);
void show_insn(long pc, int tid);

static inline bool find_symbol(const char* name, long &begin, long &end) { return elf_find_symbol(name, &begin, &end) != 0; }
static inline const char* find_pc(long pc, long &offset) { return elf_find_pc(pc, &offset); }

#include "opcodes.h"

struct Insn_t {
  unsigned op_4B	:  1;
  unsigned op_vm	:  1;
  unsigned op_longimmed	:  1;
  enum Opcode_t op_code	: 13;
  uint8_t op_rd;		// note unsigned byte
  uint8_t op_rs1;		// so NOREG==0xFF
  union {
    struct {
      uint8_t rs2;
      uint8_t rs3;
      int16_t imm;
    } op;
    int32_t op_immed;
  };
  Insn_t() { *((int64_t*)this) = -1; } // all registers become NOREG
  Insn_t(enum Opcode_t code, int big, int bigimm) { *((int64_t*)this)=-1; op_code=code; op_4B=big; op_longimmed=bigimm; }
};
static_assert(sizeof(Insn_t) == 8);

#define GPREG	0
#define FPREG	GPREG+32
#define VPREG	FPREG+32
#define VMREG	VPREG+32
#define NOREG	0xFF

Insn_t decoder(int b, long pc);	// given bitpattern image of in struction

class insnSpace_t {
  long base;
  long limit;
  Insn_t* predecoded;
public:  
  insnSpace_t() { base=limit=0; predecoded=0; }
  void init(long lo, long hi);
  bool valid(long pc) { return base<=pc && pc<limit; }
  long index(long pc) { checkif(valid(pc)); return (pc-base)/2; }
  Insn_t at(long pc) { return predecoded[index(pc)]; }
  uint32_t image(long pc) { checkif(valid(pc)); return *(uint32_t*)(pc); }
  Insn_t set(long pc, Insn_t i) { predecoded[index(pc)] = i; return i; }
};

extern insnSpace_t code;
extern const char* op_name[];
extern const char* reg_name[];

void labelpc(long pc, FILE* f =stderr);
void disasm(long pc, const char* end, FILE* f =stderr);
inline void disasm(long pc, FILE* f =stderr) { disasm(pc, "\n", f); }

void OpenTcpLink(const char* name);
void ProcessGdbCommand(void* spike_state =0);
void HandleException(int signum);
