.section .text.boot
.globl boot

boot:
  la a1, message

next:
  lbu a0, 0(a1)
  beqz a0, done
  li a7, 1  // print char in console sbi_console_putchar (1). Serail is connected to stdio
  ecall
  addi a1, a1, 1
  j next

done:
  li a7, 8  // shutdown the machine sbi_shutdown (8)
  li a6, 0
  ecall

halt:
  wfi
  j halt

.section .rodata
message:
  .string "Hello from RISC-V\n"
