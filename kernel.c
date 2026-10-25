#include "kernel.h"
#include "common.h"

extern char __bss[], __bss_end[], __stack_top[];

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


void kernelMain(void){
  memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

  printf("\nHello World!\n");
  printf("1 + 2 = %d, %x\n", 3, 0xdeadbeef);
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
