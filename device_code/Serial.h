#ifndef SERIAL_H
#define SERIAL_H

#include <inttypes.h>
#include <stdio.h> // for size_t
#include <string.h> // for size_t
#include "mstn_uart.h"

class HardwareSerial
{
  public:
    inline HardwareSerial(_UART_InterfaceNum inum) { iNum = inum; };
    inline void init(_UART_InterfaceNum inum) { iNum = inum; };
    void begin(unsigned long baud) {UART_Begin(iNum, baud); }
    void end() {UART_End(this->iNum); };
    int available(void) {return UART_Available(iNum);};
    int peek(void) { return UART_Peek(iNum); };
    int read(void) { return UART_Read(iNum); };
    void flush(void) { UART_Flush(iNum); };
    size_t write(uint8_t c) {UART_Write(iNum, (char)c); return (size_t)1; };
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    void print(const char *str);
    void println(const char *str);
    void print(const char c);
private:
    _UART_InterfaceNum iNum;
};

extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;
extern HardwareSerial Serial4;

#endif /* SERIAL_H */

