
#include "Ethernet.h"
#include "w5100.h"

extern "C" {
  #include "string.h"
}

EthernetServer::EthernetServer (uint16_t port) {  _port = port; }

/**
 * @brief Запускает сервер для приема входящих соединений
 */
void EthernetServer::begin()
{
  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client(sock);
    if (client.status() == SnSR::CLOSED) {
      Ethernet.socketBegin(sock, SnMR::TCP, _port, 0);
      Ethernet.socketListen(sock);
      EthernetClass::_server_port[sock] = _port;
      break;
    }
  }  
}

void EthernetServer::accept()
{
  int listening = 0;

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client(sock);

    if (EthernetClass::_server_port[sock] == _port) {
      if (client.status() == SnSR::LISTEN) {
        listening = 1;
      } 
      else if (client.status() == SnSR::CLOSE_WAIT && !client.available()) {
        client.stop();
      }
    } 
  }

  if (!listening) {
    begin();
  }
}

/**
 * @brief Проверка готовности принять клиента
 */
EthernetClient EthernetServer::available()
{
  accept();

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client(sock);
    if (EthernetClass::_server_port[sock] == _port &&
        (client.status() == SnSR::ESTABLISHED ||
         client.status() == SnSR::CLOSE_WAIT)) {
      if (client.available()) {
        return client;
      }
    }
  }

  return EthernetClient(MAX_SOCK_NUM);
}

size_t EthernetServer::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t EthernetServer::write(const uint8_t *buffer, size_t size) 
{
  size_t n = 0;
  
  accept();

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    EthernetClient client(sock);

    if (EthernetClass::_server_port[sock] == _port &&
      client.status() == SnSR::ESTABLISHED) {
      n += client.write(buffer, size);
    }
  }
  
  return n;
}

/**
 * @brief Отправка сообщения
 */
size_t EthernetServer::write(const char * str) 
{
    if(str == NULL)
        return 0;
    return write((const uint8_t *) str, strlen(str));
}