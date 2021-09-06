/*
  Copyright (c) 2021 Peter Hsu.  All Rights Reserved.  See LICENCE file for details.
*/
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#include "options.h"
#include "uspike.h"
#include "cpu.h"

configuration_t conf;
insnSpace_t code;

void* operator new(size_t size)
{
  //  fprintf(stderr, "operator new(%ld)\n", size);
  extern void* malloc(size_t);
  return malloc(size);
}
void operator delete(void*) noexcept
{
}

void exit_func()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "EXIT_FUNC() called\n\n");
  status_report();
  fprintf(stderr, "\n");
}  

extern "C" {
  extern int lastGdbSignal;
  extern jmp_buf mainGdbJmpBuf;
  void signal_handler(int nSIGnum);
  void ProcessGdbCommand();
  void ProcessGdbException();
  void OpenTcpLink(const char* name);
  extern long *gdb_pc;
  extern long *gdb_reg;
  extern long gdbNumContinue;
};
  

int main(int argc, const char* argv[], const char* envp[])
{
  new option<>     (conf.isa,	"isa",		"rv64imafdcv",			"RISC-V ISA string");
  new option<>     (conf.vec,	"vec",		"vlen:128,elen:64,slen:128",	"Vector unit parameters");
  new option<long> (conf.stat,	"stat",		100,				"Status every M instructions");
  new option<bool> (conf.ecall,	"ecall",	false, true,			"Show system calls");
  new option<bool> (conf.quiet,	"quiet",	false, true,			"No status report");
  new option<long> (conf.show,	"show",		0, 				"Trace execution after N gdb continue");
  new option<>     (conf.gdb,	"gdb",		0, "localhost:1234", 		"Remote GDB on socket");
  
  parse_options(argc, argv, "uspike: user-mode RISC-V interpreter derived from Spike");
  if (argc == 0)
    help_exit();
  conf.stat *= 1000000L;	// in millions of instructions
  start_time(1);
  cpu_t* mycpu = new cpu_t(argc, argv, envp);

  //#ifdef DEBUG
#if 0
  static struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  sigemptyset(&action.sa_mask);
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  action.sa_sigaction = signal_handler;
  sigaction(SIGSEGV, &action, NULL);
  sigaction(SIGABRT, &action, NULL);
  sigaction(SIGINT,  &action, NULL);
  //  if (setjmp(return_to_top_level) != 0) {
  //    fprintf(stderr, "SIGSEGV signal was caught\n");
  //    debug.print();
  //    exit(-1);
  //  }
#endif

  dieif(atexit(exit_func), "atexit failed");
  long next_status_report = conf.stat*1000000L;
  //  enum stop_reason reason;
  if (conf.gdb) {
    gdb_pc = mycpu->ptr_pc();
    gdb_reg = mycpu->reg_file();
    OpenTcpLink(conf.gdb);
    signal(SIGABRT, signal_handler);
    signal(SIGFPE,  signal_handler);
    signal(SIGSEGV, signal_handler);
    //    signal(SIGILL,  signal_handler);
    //    signal(SIGINT,  signal_handler);
    //    signal(SIGTERM, signal_handler);
    while (1) {
      if (setjmp(mainGdbJmpBuf))
	ProcessGdbException();
      ProcessGdbCommand();
#if 1
      while (1) {
	long oldpc = mycpu->read_pc();
	if (!mycpu->single_step())
	  break;
	if (gdbNumContinue > conf.show)
	  show(mycpu, oldpc);
      }
#else
      while (mycpu->single_step())
	/* pass */ ;
#endif
      lastGdbSignal = SIGTRAP;
      ProcessGdbException();
    }
  }
  else
    interpreter(mycpu);
  return 0;
}

void insnSpace_t::init(long lo, long hi)
{
  base=lo;
  limit=hi;
  int n = (hi-lo)/2;
  predecoded=new Insn_t[n];
  memset(predecoded, 0, n*sizeof(Insn_t));
  // Predecode instruction code segment
  long pc = lo;
  while (pc < hi) {
    Insn_t i = code.set(pc, decoder(code.image(pc), pc));
    pc += i.compressed() ? 2 : 4;
  }
  substitute_cas(lo, hi);
}

#include "constants.h"

