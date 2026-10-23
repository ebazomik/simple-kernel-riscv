#include "kernel.h"

extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_putchar(char ch) {
  register long a0 __asm__("a0") = ch;
  register long a7 __asm__("a7") = 1;
  __asm__ __volatile__(
    "ecall"
    : "+r"(a0), "+r"(a1)
    : "r"(a7)
    : "memory"
  );

  return (struct sbiret){ .error = a0, .value = 0 };
}

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
  const char *s = "\nHellow Word!\n";
  for(int i = 0; s[i] != '\0'; i++)
    sbi_putchar(s[i]);

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
