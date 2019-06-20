#include "Ethernet.h"
#include "Dhcp.h"

uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 0, };

DhcpClass _dhcp;

/**
 * @brief	This Ethernet initialize
 * 
 * @return      Initializate soket
 */
int EthernetClass::begin(uint8_t * mac_address, unsigned long timeout, unsigned long responseTimeout) 
{

    W5100.init();
    uint8_t defaultaddr [] = {0,0,0,0}; //ip addr to connect
    //uint8_t defaultmac [] = {0x2B,0x1A,0x2B,0x3C,0x4D,0x5E};
    //uint8_t mac_address [] = {0x00,0x00,0x00,0x00,0x00,0x00};
    //uint8_t mac_address[6] ={0,};
    W5100.setIPAddress(defaultaddr);
    W5100.setMACAddress(mac_address);
    
    int ret  =_dhcp.beginWithDHCP(mac_address, timeout , responseTimeout);
    if (ret  == 1) 
    {
        //uint8_t *asd = _dhcp.getLocalIp();
        //printf("< %02X\n",_dhcp.getLocalIp());
        //printf("asd %hu\n",asd[0]);
        W5100.setIPAddress(_dhcp.getLocalIp());
        W5100.setGatewayIp(_dhcp.getGatewayIp());
        W5100.setSubnetMask(_dhcp.getSubnetMask());
        //_dnsServerAddress = _dhcp.getDnsServerIp();
    }
    
    return ret ;
}

/**
 * @brief	The same, but you can init your ip
 * 
 * @return      Initializate soket
 */

int EthernetClass::begin(uint8_t * mac_address, uint8_t * local_ip) 
{
    W5100.init();
    //uint8_t defaultmac [] = {0x2B,0x1A,0x2B,0x3C,0x4D,0x5E};
    //W5100.setGatewayIp(gateway);
    W5100.setMACAddress(mac_address);
    W5100.setIPAddress(local_ip);
    return 1;
}

/**
 * @brief	The same, but you can init your ip, mac, gateway
 * 
 * @return      Initializate soket
 */

int EthernetClass::begin(uint8_t *mac_address, uint8_t * local_ip, uint8_t *gateway) 
{
    W5100.init();
    //uint8_t defaultmac [] = {0x2B,0x1A,0x2B,0x3C,0x4D,0x5E};
    W5100.setGatewayIp(gateway);
    //printf("%hu\n", gateway);
    W5100.setMACAddress(mac_address);
    //printf("%hu\n", mac_address);
    W5100.setIPAddress(local_ip);
    //printf("%hu\n", local_ip);
    return 1;
}

uint8_t * EthernetClass::localMAC()
{
  static uint8_t ret[6];
  ret [0] = W5100.read(0x0009);
  ret [1] = W5100.read(0x000A);
  ret [2] = W5100.read(0x000B);
  ret [3] = W5100.read(0x000C);
  ret [4] = W5100.read(0x000D);
  ret [5] = W5100.read(0x000E);
  
  return ret;
}

uint8_t * EthernetClass::localIP()
{
  static uint8_t ret[4];
  ret [0] = W5100.read(0x000F);
  ret [1] = W5100.read(0x0010);
  ret [2] = W5100.read(0x0011);
  ret [3] = W5100.read(0x0012);
  return ret;
}

EthernetClass Ethernet;