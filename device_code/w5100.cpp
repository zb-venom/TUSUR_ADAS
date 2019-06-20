/* 
 * File:   w5100.cpp
 * Author: akeela
 */
#include <stdio.h>
#include <string.h>


#include "w5100.h"

W5100Class W5100;

#define TX_RX_MAX_BUF_SIZE 2048
#define TX_BUF 0x1100
#define RX_BUF (TX_BUF + TX_RX_MAX_BUF_SIZE)
//#define RX_BUF 2048
#define TXBUF_BASE 0x4000
#define RXBUF_BASE 0x6000 // (0x8000 end) 

/**
 * @brief	Устанавливает связь с W5100 через SPI
 * 
 */

void W5100Class::init (void) 
{
    SPI_Begin(SPI1, SPI_MODE0, SPI_WL8, 1); //Начинает работу c SPI
    SPI_SetClockDivider(SPI1, 10);
    writeMR(1<<RST);
    writeTMSR(0x55);
    writeRMSR(0x55);

  for (int i=0; i<MAX_SOCK_NUM; i++) {
    SBASE[i] = TXBUF_BASE + SSIZE * i;
    RBASE[i] = RXBUF_BASE + RSIZE * i;
  }
    //W5100_Regset (MR, 0x80);
}

/**
 * @brief Возвращает свободный размер TX буфера 
 * 
 */

uint16_t W5100Class::getTXFreeSize(SOCKET s) 
{
    uint16_t val=0, val1=0;
    do 
    {
        val1 = readSnTX_FSR(s);
        if (val1 != 0)
            val = readSnTX_FSR(s);
    } 
    while (val != val1);
    return val;
}

/**
 * @brief	Возвращает занятую размер RX буфера
 *       
 */

uint16_t W5100Class::getRXReceivedSize(SOCKET s)
{
  uint16_t val=0,val1=0;
  do {
    val1 = readSnRX_RSR(s);
    if (val1 != 0)
      val = readSnRX_RSR(s);
  } 
  while (val != val1);
  
  return val;
}

/**
 * @brief	Процесс отправки данных
 * 
 */

void W5100Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  send_data_processing_offset(s, 0, data, len);
}

/**
 * @brief	Процесс отправки данных со смещением
 * 
 */

void W5100Class::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
  uint16_t ptr = readSnTX_WR(s);
  ptr += data_offset;
  uint16_t offset = ptr & SMASK;
  uint16_t dstAddr = offset + SBASE[s];

  if (offset + len > SSIZE) 
  {
    // Wrap around circular buffer
    uint16_t size = SSIZE - offset;
    write(dstAddr, data, size);
    write(SBASE[s], data + size, len - size);
  } 
  else {
    write(dstAddr, data, len);
  }

  ptr += len;
  writeSnTX_WR(s, ptr);
}

/**
 * @brief	Процесс получения данных
 * 
 */

void W5100Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
  uint16_t ptr;
  ptr = readSnRX_RD(s);
  read_data(s, ptr, data, len);
  if (!peek)
  {
    ptr += len;
    writeSnRX_RD(s, ptr);
  }
}

/**
 * @brief	Чтение данных
 * 
 */

void W5100Class::read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
  uint16_t size;
  uint16_t src_mask;
  uint16_t src_ptr;

  src_mask = src & RMASK;
  src_ptr = RBASE[s] + src_mask;

  if( (src_mask + len) > RSIZE ) 
  {
    size = RSIZE - src_mask;
    read(src_ptr, (uint8_t *)dst, size);
    dst += size;
    read(RBASE[s], (uint8_t *) dst, len - size);
  }
  else
    read(src_ptr, (uint8_t *) dst, len);
}

/**
 * @brief	Запись по SPI
 * 
 */

uint8_t W5100Class::write(uint16_t _addr, uint8_t _data)
{
    SPI_Transfer(SPI1, 0xF0); //Write command
    SPI_Transfer(SPI1, _addr >> 8);
    SPI_Transfer(SPI1, _addr & 0xFF);
    SPI_Transfer(SPI1, _data);
    return 1;
}

/**
 * @brief	Запись по SPI
 * 
 */

uint16_t W5100Class::write(uint16_t _addr, const uint8_t *_buf, uint16_t _len)
{
    for (uint16_t i=0; i<_len ;i++) 
    {
        SPI_Transfer(SPI1, 0xF0); //Write command
        SPI_Transfer(SPI1, _addr >> 8);
        SPI_Transfer(SPI1, _addr & 0xFF);
        printf (">>> SPI trans. %c to %02X\n", _buf[i],_addr);
        //printf (">>> SPI trans. %hu to %02X\n", _buf[i],_addr);
        SPI_Transfer(SPI1, _buf[i]);
        _addr++;
    }
    return _len;
}

/**
 * @brief	Чтение по SPI
 * 
 */

uint8_t W5100Class::read(uint16_t _addr)
{
    SPI_Transfer(SPI1, 0x0F); //Read command
    SPI_Transfer(SPI1, _addr >> 8);
    SPI_Transfer(SPI1, _addr & 0xFF);
    uint8_t _data = SPI_Transfer(SPI1, 0);
    return _data;
}

/**
 * @brief	Чтение по SPI
 * 
 */

uint16_t W5100Class::read(uint16_t _addr, uint8_t *_buf, uint16_t _len)
{
    for (uint16_t i=0; i<_len ;i++) {
        SPI_Transfer(SPI1, 0x0F); //Read command
        SPI_Transfer(SPI1, _addr >> 8);
        SPI_Transfer(SPI1, _addr & 0xFF);
        _buf[i] = SPI_Transfer(SPI1, 0);
        _addr++;
    }
    return _len;
}

/**
 * @brief	Отправка команды в регистр-команд
 * 
 */

void W5100Class::execCmdSn(SOCKET s, SockCMD _cmd) 
{
  // Отправляет команду в сокет
  writeSnCR(s, _cmd);
  // Ждем завершение обработки команды
  while (readSnCR(s))
      ;  
  //printf("cmd: %d, socet: %d\n", _cmd, readSnCR(s));
}
