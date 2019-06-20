
#include "Ethernet.h"
#include "w5100.h"

extern "C" {
  #include "string.h"
}

uint16_t EthernetClient::_srcport = 1024;

EthernetClient::EthernetClient() : _sock(MAX_SOCK_NUM) { }

EthernetClient::EthernetClient(uint8_t sock) : _sock(sock) { }

/**
 * @brief Присоединяется к клиенту по IP и порту
 * 
 * @return 1 - подключен к клиенту
 */
int EthernetClient::connect(uint8_t * ip, uint16_t port) 
{           
    if (_sock != MAX_SOCK_NUM) {
        return 0;}
    for (int i = 0; i < MAX_SOCK_NUM; i++) 
    {
        uint8_t s = W5100.readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) 
        {
            _sock = i;
            break;
        }
    }
    if (_sock == MAX_SOCK_NUM){
        return 0;}
    _srcport++;
    if (_srcport == 0) _srcport = 1024;
    Ethernet.socketBegin(_sock, SnMR::TCP, _srcport,0);
    if (!Ethernet.socketConnect(_sock, ip, port))
        return 0;
    
    while (status() != SnSR::ESTABLISHED) 
    {
        Delay(1);
        if (status() == SnSR::CLOSED) 
        {
            _sock = MAX_SOCK_NUM; 
            return 0;
        }
    }
    return 1;
}

size_t EthernetClient::write(uint8_t b) 
{
  return write(&b, 1);
}

/**
 * @brief Отправка данных клиенту
 * 
 * @return Размер данных, иначе 0
 */
size_t EthernetClient::write(const uint8_t *buf, size_t size) 
{
  if (_sock == MAX_SOCK_NUM) {
    return 0;
  }
  if (Ethernet.socketSend(_sock, buf, size)) 
    return size;
  return 0;
}

/**
 * @brief Проверка соединения с сервером
 */
uint8_t EthernetClient::connected() 
{
  if (_sock == MAX_SOCK_NUM) return 0;
  uint8_t s = status();
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

int EthernetClient::peek() 
{
  uint8_t b;
  if (!available())
    return -1;
  Ethernet.socketPeek(_sock, &b);
  return b;
}

/**
 * @brief Return number of data written to the RX buffer
 */
int EthernetClient::available() 
{
    if (_sock != MAX_SOCK_NUM)
        return W5100.getRXReceivedSize(_sock);
    return 0;   
}

/**
 * @brief Чтение данных от сервера
 * 
 * @return Размер данных, иначе -1
 */
int EthernetClient::read() 
{
  uint8_t b;
  if (Ethernet.socketRecv(_sock, &b, 1) > 0 )
  {
    return b;
  }
  else
  {
    // Данных не поступало
    return -1;
  }
}

int EthernetClient::read(uint8_t *buf, size_t size) {
  return Ethernet.socketRecv(_sock, buf, size);
}

/**
 * @brief Разрыв соединения с сервером
 */
void EthernetClient::stop() {
  if (_sock == MAX_SOCK_NUM)
    return;

  // Попрощаться с сервером по-дружески
  Ethernet.socketDisconnect(_sock);
  unsigned long start = RTC_millis();

  while (status() != SnSR::CLOSED && RTC_millis() - start < 1000)
    Delay(1);

  // Если соединение еще установлено, закрыть соединение не попрощавшись
  if (status() != SnSR::CLOSED)
    Ethernet.socketClose(_sock);

EthernetClass::_server_port[_sock] = 0;
  _sock = MAX_SOCK_NUM;

}

/**
 * @brief Возвращает статус регистра
 */
uint8_t EthernetClient::status() 
{
    uint8_t a;
    if (_sock >= MAX_SOCK_NUM) return SnSR::CLOSED;
    a = W5100.readSnSR(_sock);
    //printf ("status a %02X \n",a);
    return a;
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool() {
  return _sock != MAX_SOCK_NUM;
}

bool EthernetClient::operator==(const EthernetClient& rhs) {
  return _sock == rhs._sock && _sock != MAX_SOCK_NUM && rhs._sock != MAX_SOCK_NUM;
}

size_t EthernetClient::write(const char * str) 
{
    if(str == NULL)
        return 0;
    return write((const uint8_t *) str, strlen(str));
}