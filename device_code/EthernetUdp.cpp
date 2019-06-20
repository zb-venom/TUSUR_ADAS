                        

#include "Ethernet.h"
#include "w5100.h"

extern "C" {
  #include "string.h"
}

EthernetUDP::EthernetUDP(): _sock(MAX_SOCK_NUM) 
{ }

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t EthernetUDP::begin(uint16_t port) {
  if (_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);
    if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
      _sock = i;
      break;
    }
  }
  if (_sock == MAX_SOCK_NUM)
    return 0;

  _port = port;
  _remaining = 0;
  Ethernet.socketBegin(_sock, SnMR::UDP, _port, 0);

  return 1;
}

int EthernetUDP::available() {
  return _remaining;
}

void EthernetUDP::stop()
{
  if (_sock == MAX_SOCK_NUM)
    return;

  Ethernet.socketClose(_sock);

  EthernetClass::_server_port[_sock] = 0;
  _sock = MAX_SOCK_NUM;
}

int EthernetUDP::beginPacket(uint8_t * ip, uint16_t port)
{
  _offset = 0;
  return Ethernet.socketStartUDP(_sock, ip, port);
}

int EthernetUDP::endPacket()
{
  return Ethernet.socketSendUDP(_sock);
}

size_t EthernetUDP::write(uint8_t byte)
{
  return write(&byte, 1);
}

size_t EthernetUDP::write(const uint8_t *buffer, size_t size)
{

  uint16_t bytes_written = Ethernet.socketBufferData(_sock, _offset, buffer, size);
  _offset += bytes_written;
  return bytes_written;
}

//easy writting
size_t EthernetUDP::write(const char * str) 
{
    if(str == NULL)
        return 0;
    return write((const uint8_t *) str, strlen(str));
}

int EthernetUDP::parsePacket ()
{
	//flush(); // эта функция ломает все

	if (W5100.getRXReceivedSize(_sock) > 0)
	//if (Ethernet.socketRecvAvailable(_sock) > 0)
	{
		//HACK - hand-parse the UDP packet using TCP recv method
		uint8_t tmpBuf[8];
		int ret =0; 
		//read 8 header bytes and get IP and port from it
		ret = Ethernet.socketRecv(_sock,tmpBuf,8);
		if (ret > 0)
		{
			_remoteIP[0] = tmpBuf[0];
			_remoteIP[1] = tmpBuf[1];
			_remoteIP[2] = tmpBuf[2];
			_remoteIP[3] = tmpBuf[3];
			_remotePort = tmpBuf[4];
			_remotePort = (_remotePort << 8) + tmpBuf[5];
			_remaining = tmpBuf[6];
			_remaining = (_remaining << 8) + tmpBuf[7];

			// When we get here, any remaining bytes are the data
			ret = _remaining;
		}
		return ret;
	}
	// There aren't any packets available
	return 0;
}

int EthernetUDP::read()
{
  uint8_t byte;

  if ((_remaining > 0) && (Ethernet.socketRecv(_sock, &byte, 1) > 0))
  {
    // We read things without any problems
    _remaining--;
    return byte;
  }

  // If we get here, there's no data available
  return -1;
}

int EthernetUDP::read(unsigned char* buffer, size_t len)
{

  if (_remaining > 0)
  {

    int got;

    if (_remaining <= len)
    {
      // data should fit in the buffer
      got = Ethernet.socketRecv(_sock, buffer, _remaining);
    }
    else
    {
      // too much data for the buffer, 
      // grab as much as will fit
      got = Ethernet.socketRecv(_sock, buffer, len);
    }

    if (got > 0)
    {
      _remaining -= got;
      return got;
    }

  }

  // If we get here, there's no data available or recv failed
  return -1;

}

// TODO: переписать (некорректно работает на mstn)
void EthernetUDP::flush()
{

	while (_remaining) {
		printf("_remaining = %hu\n", _remaining);
		// could this fail (loop endlessly) if _remaining > 0 and recv in read fails?
		// should only occur if recv fails after telling us the data is there, lets
		// hope the w5100 always behaves :)
		read((uint8_t *)NULL, _remaining);
	}
}