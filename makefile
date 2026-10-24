CC     = clang
CFLAGS = --target=riscv32-unknown-elf -march=rv32imac_zicsr -mabi=ilp32 \
         -std=c11 -O2 -g3 -Wall -Wextra -ffreestanding -fno-stack-protector \
         -nostdlib -fuse-ld=lld

KERNEL_SRC = kernel.c common.c
HEADERS = $(wildcard *.h)

QEMU = qemu-system-riscv32
QEMUFLAGS = -machine virt -bios default -nographic -serial mon:stdio -no-reboot

.PHONY: all run clean

all: kernel.elf

kernel.elf: $(KERNEL_SRC) $(HEADERS) kernel.ld
	$(CC) $(CFLAGS) -Wl,-Tkernel.ld -Wl,-Map=kernel.map $(KERNEL_SRC) -o $@

run: all
	$(QEMU) $(QEMUFLAGS) -kernel kernel.elf

clean:
	rm -f kernel.elf kernel.map
