#include "common.h"


void *memset(void *buf, char c, size_t n){
  uint8_t *p = (uint8_t*) buf;
  for(size_t i = 0; i < n; i++){
    p[i] = c;
  }
  return buf;
}


void *memcpy(void *dst, const void *src, size_t n){
  uint8_t *d = (uint8_t*) dst;
  const uint8_t *s = (const uint8_t*) src;
  
  for(size_t i = 0; i < n; i++){
    d[i] = s[i];
  }
  return dst;
}


void *strcpy(char *dst, const char *src){
  size_t i = 0;
  while(src[i] != '\0'){
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2){
  while(*s1 && *s1 == *s2){
    s1++;
    s2++;
  }

  return *(const unsigned char*)s1 - *(const unsigned char*)s2; 
}


size_t strlen(const char *s){
  size_t l = 0;
  while(s[l] != '\0'){
    l++;
  }
  return l;
}


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
          // Convert to unsigned int so that negative values (including INT_MIN,
          // whose negation overflows int) are handled without undefined behavior.
          // The `power` variable is then used to emit the decimal digits from
          // most significant to least significant.
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
