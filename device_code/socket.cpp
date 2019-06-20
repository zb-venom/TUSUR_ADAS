/* 
 * File:   socket.cpp
 * 
 */

#include "Ethernet.h"
#include "w5100.h"


/**
 * @brief	      Начало работы сокета.
 * 
 * @return      1 - Сокет успешно открыт.
 */

uint8_t EthernetClass::socketBegin (SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag) {
        if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP)) 
        {
                Ethernet.socketClose(s);
                W5100.writeSnMR(s,protocol | flag);
                if (port != 0) 
                {
                    W5100.writeSnPORT(s, port);
                } 
                else 
                {
                    //if port not set, set default http
                    W5100.writeSnPORT(s, 80);
                }
                W5100.execCmdSn(s, Sock_OPEN);
                return 1;
        }
        return 0;
}

/**
 * @brief	Возвращает состояние сокета статус регистра.
 * 
 * @return  Состояние сокета статус регистра.
 */

uint8_t EthernetClass::socketStatus(SOCKET s)
{
    return W5100.readSnSR(s);
}

/**
 * @brief	Закрывает сокет.
 * 
 */

void EthernetClass::socketClose(SOCKET s)
{
  W5100.execCmdSn(s, Sock_CLOSE);
  W5100.writeSnIR(s, 0xFF);
}

/**
 * @brief	Переводит сокет в режим прослушки, ожидания новых клиентов.
 * 
 * @return  1 - Сокет прослушивает.
 */

uint8_t EthernetClass::socketListen(SOCKET s)
{
  if (W5100.readSnSR(s) != SnSR::INIT)
    return 0;
  W5100.execCmdSn(s, Sock_LISTEN);
  return 1;
}

/**
 * @brief	Устанавливает соединение.
 * 
 * @return  1 - соединение установлено.
 */

uint8_t EthernetClass::socketConnect(SOCKET s, uint8_t * addr, uint16_t port)
{
  if (((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
    ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    (port == 0x00) ) 
    return 0;
  W5100.writeSnDIPR(s, addr);
  W5100.writeSnDPORT(s, port);
  W5100.execCmdSn(s, Sock_CONNECT);

  return 1;
}

/**
 * @brief	Разрыв соединения сокета 's'.
 */
void EthernetClass::socketDisconnect(SOCKET s)
{
  W5100.execCmdSn(s, Sock_DISCON);
}


/**
 * @brief	Отправка данных по TCP.
 * @return	1 for success else 0.
 */
uint16_t EthernetClass::socketSend(SOCKET s, const uint8_t * buf, uint16_t len)
{
  uint8_t status=0;
  uint16_t ret=0;
  uint16_t freesize=0;
  if (len > W5100.SSIZE) 
    ret = W5100.SSIZE; 
  else 
    ret = len;


  do 
  {
    freesize = W5100.getTXFreeSize(s);
    status = W5100.readSnSR(s);    
    printf("free %d\n", freesize);
    printf("free %d\n", ret);
    if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
    {        
      ret = 0; 
      break;
    }
  } 
  while (freesize < ret);

  // Передача данных
  W5100.send_data_processing(s, (uint8_t *)buf, ret);
  W5100.execCmdSn(s, Sock_SEND);


  while ( (W5100.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  {
 
    if ( W5100.readSnSR(s) == SnSR::CLOSED )
    {
      socketClose(s);
      return 0;
    }
  }
  W5100.writeSnIR(s, SnIR::SEND_OK);
  return ret;
}


/**
 * @brief	Функция према данных в TCP.
 * 		
 * @return	Размер переданных данных, иначе -1.
 */
uint16_t EthernetClass::socketRecv(SOCKET s, uint8_t *buf, int16_t len)
{
  // Сколько данных доступно
  int16_t ret = W5100.getRXReceivedSize(s);
  if ( ret == 0 )
  {
    // Данные недоступны
    uint8_t status = W5100.readSnSR(s);
    if ( status == SnSR::LISTEN || status == SnSR::CLOSED || status == SnSR::CLOSE_WAIT )
    {
      // Клиент прервал соединение
      ret = 0;
    }
    else
    {
      // Клиент поддерживает соединение, но данные не поступают.
      ret = -1;
    }
  }
  else if (ret > len)
  {
    ret = len;
  }

  if ( ret > 0 )
  {
    W5100.recv_data_processing(s, buf, ret);
    W5100.execCmdSn(s, Sock_RECV);
  }
  return ret;
}

/**
 * @brief	Возвращает первый байт полученных данных
 * 		Не проверяет, были ли полученны данные.
 * @return 1.
 */
uint16_t EthernetClass::socketPeek(SOCKET s, uint8_t *buf)
{
  W5100.recv_data_processing(s, buf, 1, 1);

  return 1;
}

uint16_t EthernetClass::socketSendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port)
{
  uint16_t ret=0;

  if (len > W5100.SSIZE) ret = W5100.SSIZE;
  else ret = len;

  if
    (
  ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    ((port == 0x00)) ||(ret == 0)
    ) 
  {
    ret = 0;
  }
  else
  {
    W5100.writeSnDIPR(s, addr);
    W5100.writeSnDPORT(s, port);
  
    W5100.send_data_processing(s, (uint8_t *)buf, ret);
    W5100.execCmdSn(s, Sock_SEND);

    while ( (W5100.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
    {
      if (W5100.readSnIR(s) & SnIR::TIMEOUT)
      {
        W5100.writeSnIR(s, (SnIR::SEND_OK | SnIR::TIMEOUT));
        return 0;
      }
    }

    W5100.writeSnIR(s, SnIR::SEND_OK);
  }
  return ret;
}

uint16_t EthernetClass::socketRecvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port)
{
  uint8_t head[8];
  uint16_t data_len=0;
  uint16_t ptr=0;

  if ( len > 0 )
  {
    ptr = W5100.readSnRX_RD(s);
    switch (W5100.readSnMR(s) & 0x07)
    {
    case SnMR::UDP :
      W5100.read_data(s, ptr, head, 0x08);
      ptr += 8;
      // IP и порт клиента
      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      *port = head[4];
      *port = (*port << 8) + head[5];
      data_len = head[6];
      data_len = (data_len << 8) + head[7];

      W5100.read_data(s, ptr, buf, data_len); // передача данных
      ptr += data_len;

      W5100.writeSnRX_RD(s, ptr);
      break;

    case SnMR::IPRAW :
      W5100.read_data(s, ptr, head, 0x06);
      ptr += 6;

      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      data_len = head[4];
      data_len = (data_len << 8) + head[5];

      W5100.read_data(s, ptr, buf, data_len);
      ptr += data_len;

      W5100.writeSnRX_RD(s, ptr);
      break;

    case SnMR::MACRAW:
      W5100.read_data(s, ptr, head, 2);
      ptr+=2;
      data_len = head[0];
      data_len = (data_len<<8) + head[1] - 2;

      W5100.read_data(s, ptr, buf, data_len);
      ptr += data_len;
      W5100.writeSnRX_RD(s, ptr);
      break;

    default :
      break;
    }
    W5100.execCmdSn(s, Sock_RECV);
  }
  return data_len;
}

uint16_t EthernetClass::socketBufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
  uint16_t ret =0;
  if (len > W5100.getTXFreeSize(s))
  {
    ret = W5100.getTXFreeSize(s); // check size not to exceed MAX size.
  }
  else
  {
    ret = len;
  }
  W5100.send_data_processing_offset(s, offset, buf, ret);
  return ret;
}

int EthernetClass::socketStartUDP(SOCKET s, uint8_t* addr, uint16_t port)
{
  if
  (
    ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    ((port == 0x00))
  ) 
  {
    return 0;
  }
  else
  {
    W5100.writeSnDIPR(s, addr);
    W5100.writeSnDPORT(s, port);
    return 1;
  }
}

int EthernetClass::socketSendUDP(SOCKET s)
{
  W5100.execCmdSn(s, Sock_SEND);
  while ( (W5100.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  {
      //printf ("in while \n");
    if (W5100.readSnIR(s) & SnIR::TIMEOUT)
    {
      W5100.writeSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
      return 0;
    }
  }

  W5100.writeSnIR(s, SnIR::SEND_OK);
  return 1;
}
