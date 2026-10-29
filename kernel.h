#ifndef KERNEL_H
#define KERNEL_H

struct sbiret {
  long error;
  long value;
};

#define PANIC(fmt, ...)                                                 \
  do{                                                                   \
    printf("PANIC: %s:%d" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    while(1){                                                           \
      __asm__ __volatile__("wfi");                                      \
    }                                                                   \
  } while(0);                                                           \

#endif /* KERNEL_H */
