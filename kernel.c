#include "kernel.h"
#include "common.h"

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram[], __free_ram_end[];

struct sbiret sbi_putchar(char ch) {
  register long a0 __asm__("a0") = ch;
  register long a7 __asm__("a7") = 1;
  __asm__ __volatile__(
    "ecall"
    : "+r"(a0)
    : "r"(a7)
    : "memory"
  );

  return (struct sbiret){ .error = a0, .value = 0 };
}

void putchar(char ch) { sbi_putchar(ch); }


void sbi_shutdown(void){
  register long a7 __asm__("a7") = 8;
  __asm__ __volatile__(
      "ecall"
      :
      : "r"(a7)
      : "memory"
  );

  for(;;)
    __asm__ __volatile__("wfi");
}

void handle_trap(struct trap_frame *f) {
  uint32_t scause = READ_CSR(scause);
  uint32_t stval = READ_CSR(stval);
  uint32_t user_pc = READ_CSR(sepc);

  (void) f;
  PANIC("unexpected trap scause=%x, stval=%x, sepc=%x", scause, stval, user_pc);
}


__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void){
  __asm__ __volatile__(
      "csrw sscratch, sp\n"
      "addi sp, sp, -4 * 31\n"

      "sw ra,  4 * 0(sp)\n"
      "sw gp,  4 * 1(sp)\n"
      "sw tp,  4 * 2(sp)\n"
      "sw t0,  4 * 3(sp)\n"
      "sw t1,  4 * 4(sp)\n"
      "sw t2,  4 * 5(sp)\n"
      "sw t3,  4 * 6(sp)\n"
      "sw t4,  4 * 7(sp)\n"
      "sw t5,  4 * 8(sp)\n"
      "sw t6,  4 * 9(sp)\n"
      "sw a0,  4 * 10(sp)\n"
      "sw a1,  4 * 11(sp)\n"
      "sw a2,  4 * 12(sp)\n"
      "sw a3,  4 * 13(sp)\n"
      "sw a4,  4 * 14(sp)\n"
      "sw a5,  4 * 15(sp)\n"
      "sw a6,  4 * 16(sp)\n"
      "sw a7,  4 * 17(sp)\n"
      "sw s0,  4 * 18(sp)\n"
      "sw s1,  4 * 19(sp)\n"
      "sw s2,  4 * 20(sp)\n"
      "sw s3,  4 * 21(sp)\n"
      "sw s4,  4 * 22(sp)\n"
      "sw s5,  4 * 23(sp)\n"
      "sw s6,  4 * 24(sp)\n"
      "sw s7,  4 * 25(sp)\n"
      "sw s8,  4 * 26(sp)\n"
      "sw s9,  4 * 27(sp)\n"
      "sw s10, 4 * 28(sp)\n"
      "sw s11, 4 * 29(sp)\n"

      "csrr a0, sscratch\n"
      "sw a0, 4 * 30(sp)\n"

      "mv a0, sp\n"
      "call handle_trap\n"

      "lw ra,  4 * 0(sp)\n"
      "lw gp,  4 * 1(sp)\n"
      "lw tp,  4 * 2(sp)\n"
      "lw t0,  4 * 3(sp)\n"
      "lw t1,  4 * 4(sp)\n"
      "lw t2,  4 * 5(sp)\n"
      "lw t3,  4 * 6(sp)\n"
      "lw t4,  4 * 7(sp)\n"
      "lw t5,  4 * 8(sp)\n"
      "lw t6,  4 * 9(sp)\n"
      "lw a0,  4 * 10(sp)\n"
      "lw a1,  4 * 11(sp)\n"
      "lw a2,  4 * 12(sp)\n"
      "lw a3,  4 * 13(sp)\n"
      "lw a4,  4 * 14(sp)\n"
      "lw a5,  4 * 15(sp)\n"
      "lw a6,  4 * 16(sp)\n"
      "lw a7,  4 * 17(sp)\n"
      "lw s0,  4 * 18(sp)\n"
      "lw s1,  4 * 19(sp)\n"
      "lw s2,  4 * 20(sp)\n"
      "lw s3,  4 * 21(sp)\n"
      "lw s4,  4 * 22(sp)\n"
      "lw s5,  4 * 23(sp)\n"
      "lw s6,  4 * 24(sp)\n"
      "lw s7,  4 * 25(sp)\n"
      "lw s8,  4 * 26(sp)\n"
      "lw s9,  4 * 27(sp)\n"
      "lw s10, 4 * 28(sp)\n"
      "lw s11, 4 * 29(sp)\n"
      "lw sp,  4 * 30(sp)\n"

      "sret\n");
}

paddr_t alloc_pages(uint32_t n) {
  static paddr_t next_paddr = (paddr_t) __free_ram;
  paddr_t paddr = next_paddr;

  next_paddr += n * PAGE_SIZE;
  if (next_paddr > (paddr_t) __free_ram_end)
    PANIC("out of memory");

  memset((void *) paddr, 0, n * PAGE_SIZE);
  return paddr;
}

struct process processTable[MAX_PROCESS];

struct process *createProcess(uint32_t pc){
  struct process *proc = NULL;
  int i = 0;
  for(;i < MAX_PROCESS; i++){
    if(processTable[i].state == PROC_UNUSED){
      proc = &processTable[i];
      break;
    }
  };

  if(proc == NULL) PANIC("processTable full\n");

  uint32_t *sp = (uint32_t *) &proc->stack[sizeof(proc->stack)];

  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;
  *--sp = 0;

  *--sp = pc;

  proc->pid = i + 1;
  proc->state = PROC_RUNNABLE;
  proc->sp = (vaddr_t) sp;
  return proc;
}

struct process *proc_a;
struct process *proc_b;

void delay(void) {
  for (int i = 0; i < 3000000; i++)
    __asm__ __volatile__("nop");
}


__attribute__((naked))
void switchContext(uint32_t *old, uint32_t *new){
    __asm__ __volatile__(
        "addi sp, sp, -64\n" // 16-byte preserve align (risc-v specs)
        "sw ra, 0(sp)\n"
        "sw s0, 4(sp)\n"
        "sw s1, 8(sp)\n"
        "sw s2, 12(sp)\n"
        "sw s3, 16(sp)\n"
        "sw s4, 20(sp)\n"
        "sw s5, 24(sp)\n"
        "sw s6, 28(sp)\n"
        "sw s7, 32(sp)\n"
        "sw s8, 36(sp)\n"
        "sw s9, 40(sp)\n"
        "sw s10, 44(sp)\n"
        "sw s11, 48(sp)\n"

        "sw sp, (a0)\n" // Store old stack pointer
        "lw sp, (a1)\n" // Load new stack pointer

        "lw ra, 0(sp)\n"
        "lw s0, 4(sp)\n"
        "lw s1, 8(sp)\n"
        "lw s2, 12(sp)\n"
        "lw s3, 16(sp)\n"
        "lw s4, 20(sp)\n"
        "lw s5, 24(sp)\n"
        "lw s6, 28(sp)\n"
        "lw s7, 32(sp)\n"
        "lw s8, 36(sp)\n"
        "lw s9, 40(sp)\n"
        "lw s10, 44(sp)\n"
        "lw s11, 48(sp)\n"
        "addi sp, sp, 64\n"

        "ret\n"
    );
}


void proc_a_entry(void) {
  printf("starting process A\n");
  while (1) {
    putchar('A');
    switchContext(&proc_a->sp, &proc_b->sp);
    delay();
  }
}

void proc_b_entry(void) {
  printf("starting process B\n");
  while (1) {
    putchar('B');
    switchContext(&proc_b->sp, &proc_a->sp);
    delay();
  }
}


void kernelMain(void){
  memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

  WRITE_CSR(stvec, (uint32_t) kernel_entry);

  proc_a = createProcess((uint32_t) proc_a_entry);
  proc_b = createProcess((uint32_t) proc_b_entry);

  proc_a_entry();

  sbi_shutdown();
}

__attribute__((section(".text.boot")))
__attribute__((naked))

void boot(void){
  __asm__ __volatile__(
    "mv sp, %[stack_top]\n"
    "j kernelMain\n"
    :
    : [stack_top] "r"(__stack_top)
  );
}
