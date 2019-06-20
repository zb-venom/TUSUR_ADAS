/* 
 * File:   Ethernet.h
 * Author: akeela
 */

#ifndef ETHERNET_H
#define ETHERNET_H

#include <inttypes.h>
#include <stdio.h>

#include "w5100.h"

#include "mstn_rtc.h" //Intec libs
#include "mstn_clk.h"

#define MAX_SOCK_NUM 4
#define UDP_TX_PACKET_MAX_SIZE 24

class EthernetUDP;
class EthernetClient;
class EthernetServer;
class DhcpClass;

class EthernetClass {
private:
    //static DhcpClass* _dhcp;
public:
    static uint8_t _state[MAX_SOCK_NUM];
    static uint16_t _server_port[MAX_SOCK_NUM];
    
    static int begin(uint8_t * mac_address, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);
    //static int begin (uint8_t * mac_address);
    static int begin(uint8_t * mac_address, uint8_t * local_ip);
    static int begin(uint8_t * mac_address, uint8_t * local_ip, uint8_t * gateway);
    
    virtual uint8_t *localIP();
    virtual uint8_t *localMAC();
    
    friend class EthernetClient;
    friend class EthernetServer;
    friend class EthernetUDP;
    
//private:
    //Открытие сокета (TCP)
    static uint8_t socketBegin (SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);
    static uint8_t socketStatus (SOCKET s);
    //Закрытие сокета
    static void socketClose(SOCKET s);
    //Установка соединения (TCP)
    static uint8_t socketListen(SOCKET s);
    static uint8_t socketConnect(SOCKET s, uint8_t * addr, uint16_t port);
    //Разрыв соединения
    static void socketDisconnect(SOCKET s);
    //Отправка данных (TCP)
    static uint16_t socketSend(SOCKET s, const uint8_t * buf, uint16_t len);
    //Получение данных (TCP)
    static uint16_t socketRecv(SOCKET s, uint8_t *buf, int16_t len);
    static uint16_t socketPeek (SOCKET s, uint8_t *buf);

    static int socketStartUDP (SOCKET s, uint8_t* addr, uint16_t port);
    static int socketSendUDP (SOCKET s);
    static uint16_t socketBufferData (SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len);
private:
    static uint16_t socketSendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port);
    static uint16_t socketRecvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port);

};

extern EthernetClass Ethernet;

class EthernetUDP
{
private:
    uint8_t _sock;
    uint16_t _port; // Порт для прослушки
    uint8_t _remoteIP[4];
    uint16_t _remotePort;
    uint16_t _offset;
    uint16_t _remaining;
public:
    EthernetUDP();
    virtual uint8_t begin (uint16_t);
    virtual void stop();

    virtual int beginPacket(uint8_t * ip, uint16_t port);
    //virtual int beginPacket(const char * host, uint16_t port);  // TODO: DNS
    virtual int endPacket ();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(const char * str);
    
    virtual int parsePacket();
    virtual int available();
    virtual int read();
    virtual int read(unsigned char* buffer, size_t len);

    virtual void flush();

    //TODO;
    //virtual uint8_t * remoteIP() { return *_remoteIP; }
    virtual uint8_t remoteIP() { return _remoteIP[0]; }
    virtual uint16_t remotePort() { return _remotePort; }
    virtual uint16_t localPort() { return _port; }
};

class EthernetClient 
{
public:
    EthernetClient();
    EthernetClient(uint8_t sock);

    uint8_t status();
    virtual int connect (uint8_t * ip, uint16_t port);
    virtual void stop ();
    virtual uint8_t connected();
    virtual int available();
    virtual size_t write(uint8_t b);
    virtual size_t write(const uint8_t *buf, size_t size);
    //better writing
    virtual size_t write(const char * str);
    virtual int read();
    virtual int read(uint8_t *buf, size_t size);
    virtual int peek();
    
    virtual operator bool();
    virtual bool operator==(const EthernetClient&);
    virtual bool operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };
    
    friend class EthernetServer;
    
private:
    uint8_t _sock;
    static uint16_t _srcport;
};

class EthernetServer 
{
private:
    uint16_t _port;
public:    
    EthernetServer(uint16_t);
    void accept();
    EthernetClient available();
    virtual void begin();
    virtual size_t write(uint8_t b);
    virtual size_t write(const uint8_t *buf, size_t size);
    //better writing
    virtual size_t write(const char * str);
};

#endif /* ETHERNET_H */
