#include "common.h"

void printf(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);

  while(*fmt != '\0'){
    if(*fmt != '%'){
      putchar(*fmt);
    } else {
      fmt++;
      switch(*fmt){
        case '\0': {
          putchar('%');
          goto end;
        } break;
        case '%': {
          putchar('%');
        } break;
        case 's': {
          const char *val = va_arg(args, const char*);
          while(*val != '\0'){
            putchar(*val);
            val++;
          }
        } break;
        case 'd': {
          int val = va_arg(args, int);
          unsigned int magnitude = (unsigned int) val;
          if(val < 0){
            putchar('-');
            magnitude = -magnitude;
          }

          unsigned int power = 1;
          while(magnitude/power > 9){
            power *= 10;
          }

          while(power > 0){
            putchar('0' + magnitude/power);
            magnitude = magnitude % power;
            power = power/10;
          } 

        } break;
        case 'x': {
          unsigned int val = va_arg(args, unsigned int);
          for(int i = 7; i >= 0; i--){
            unsigned int nibble = (val >> (4 * i)) & 0xF;
            putchar("0123456789abcdef"[nibble]);
          } 
        } break;
        default : {
          // Implement
        }
      }
    }
    fmt++;
  }
  end:
  va_end(args);
}
