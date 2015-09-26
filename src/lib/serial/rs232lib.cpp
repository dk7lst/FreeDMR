//#include <fcntl.h>
//#include <unistd.h>
//#include <termios.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include "rs232lib.h"

// http://tldp.org/HOWTO/Serial-Programming-HOWTO/index.html
// http://tldp.org/HOWTO/Serial-HOWTO.html

RS232Port::RS232Port() {
  m_iFD = -1;

  setBaud(19200);
}

RS232Port::~RS232Port() {
  close();
}

bool RS232Port::setBaud(int iBaud) {
  switch(iBaud) {
    case 1200: m_iBaudFlags = B1200; break;
    case 2400: m_iBaudFlags = B2400; break;
    case 4800: m_iBaudFlags = B4800; break;
    case 9600: m_iBaudFlags = B9600; break;
    case 19200: m_iBaudFlags = B19200; break;
    case 38400: m_iBaudFlags = B38400; break;
    case 57600: m_iBaudFlags = B57600; break;
    case 115200: m_iBaudFlags = B115200; break;
    default: return false;
  }
  return true;
}

/*
bool RS232Port::setDataBits(int iBits) {
}

bool RS232Port::setStopBits(int iBits) {
}
*/

bool RS232Port::open(const char *pzDeviceName) {
  m_iFD = ::open(pzDeviceName, O_RDWR | O_NOCTTY);
  if(m_iFD < 0) {
    return false;
  }

  // siehe http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html :
  struct termios tio;
  tcgetattr(m_iFD, &m_OrgTIO);
  bzero(&tio, sizeof(tio));
  tio.c_cflag = m_iBaudFlags | CS8 | CLOCAL | CREAD;
  tio.c_iflag = IGNPAR;
  tio.c_oflag = 0;
  tio.c_lflag = 0;
  tio.c_cc[VTIME] = 0; // inter-character timer unused
  tio.c_cc[VMIN] = 4; // blocking read until 4 chars received
  tcflush(m_iFD, TCIFLUSH);
  tcsetattr(m_iFD, TCSANOW, &tio);
  return true;
}

void RS232Port::close() {
  if(m_iFD >= 0) {
    tcsetattr(m_iFD, TCSANOW, &m_OrgTIO);
    ::close(m_iFD);
    m_iFD = -1;
  }
}

int RS232Port::transmit(unsigned const char *pcBuf, int iBytes) {
  return write(m_iFD, pcBuf, iBytes);
}

int RS232Port::receive(unsigned char *pcBuf, int iMaxBytes) {
  return read(m_iFD, pcBuf, iMaxBytes);
}
