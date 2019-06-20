#ifndef SERIAL0_H
#define SERIAL0_H

#include <inttypes.h>
#include <stdio.h> // for size_t
#include <string.h> // for size_t
#include "mstn_types.h"
#include "mstn_usb.h"
#include "Print.h"

class FakeSerial
{
  public:
    inline FakeSerial() { };
    void begin(unsigned long baud) { }
    void begin(unsigned long, uint8_t) { };
    void end() { };
    int available(void) {return USB_GetReceivedLen();};
    int peek(void) { int c = getchar(); 
                     ungetc(c, stdin);
                     return c; };
    int read(void) { return getchar(); };
    void flush(void) { while (USB_CheckReceiveCompleting() != R_SUCCESS); };
    size_t write(uint8_t c) {putchar(c); return 1; };
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    void print(const char *str);
    void println(const char *str);
    void print(const char c);
    void println(const char c);
};

extern FakeSerial Serial;

#endif /* SERIAL0_H */

