#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "../platform/platform.h"
#include "dv4mini.h"

const BYTE CmdPreamble[] = {0x71, 0xFE, 0x39, 0x1D};

enum {
  SETADFQRG = 1,
  SETADFMODE = 2,
  FLUSHTXBUF = 3,
  ADFWRITE = 4,
  ADFWATCHDOG = 5,
  ADFGETDATA = 7,
  ADFGREENLED = 8,
  ADFSETPOWER = 9,
  ADFDEBUG = 10,
  ADFSETSEED = 17,
  ADFVERSION = 18,
  ADFSETTXBUF = 19
};

void *WatchdogThread(void *pClassInstance) {
  ((DV4Mini *)pClassInstance)->runWatchdogThread();
  pthread_exit(NULL);
}

void *ReceiveThread(void *pClassInstance) {
  ((DV4Mini *)pClassInstance)->runReceiveThread();
  pthread_exit(NULL);
}

DV4Mini::DV4Mini() {
}

DV4Mini::~DV4Mini() {
  close();
}

bool DV4Mini::open(const char *pzDeviceName) {
  m_Port.setBaud(115200);
  if(!m_Port.open(pzDeviceName)) return false;

  m_bWatchdogRunning = true;
  if(pthread_create(&m_WatchdogThread, NULL, WatchdogThread, this)) return false;
  if(pthread_create(&m_ReceiveThread, NULL, ReceiveThread, this)) {
    m_bWatchdogRunning = false;
    return false;
  }

  return true;
}

void DV4Mini::close() {
  m_bReceiveThreadRunning = false;
  m_Port.close();
}

bool DV4Mini::setFrequency(int iHz) {
  struct SetFrequencyParameters
  {
    uint32_t rx;
    uint32_t tx;
  } param;
  assert(sizeof param == 8);
  param.rx = param.tx = htole32(iHz);
  return sendCmd(SETADFQRG, (BYTE *)&param, sizeof param);
}

bool DV4Mini::setMode(MODE mode) {
  BYTE m = mode;
  return sendCmd(SETADFMODE, &m, 1);
}

bool DV4Mini::setTxPower(TXPOWER level) {
  if(level < 0 || level > 9) {
    assert(0);
    return false;
  }
  BYTE lev = level;
  return sendCmd(ADFSETPOWER, &lev, 1);
}

bool DV4Mini::requestWatchdogMsg() {
  return sendCmd(ADFWATCHDOG, NULL, 0);
}

bool DV4Mini::requestReceiveMsg() {
  return sendCmd(ADFGETDATA, NULL, 0);
}

bool DV4Mini::transmit(BYTE *pBuffer, BYTE iLength) {
  if(iLength < 1 || iLength > 245 || !pBuffer) {
    assert(0);
    return false;
  }
  return sendCmd(ADFWRITE, pBuffer, iLength);
}

bool DV4Mini::flush() {
  return sendCmd(FLUSHTXBUF, NULL, 0);
}

void DV4Mini::runWatchdogThread() {
  setMode(MODE_DMR);
  setTxPower(TXPOWER_MAX);

  int iCount = 0;
  while(m_bWatchdogRunning) {
    usleep(100);
    requestReceiveMsg();
    if(++iCount >= 10) {
      requestWatchdogMsg();
      iCount = 0;
    }
  }
}

void DV4Mini::runReceiveThread() {
  BYTE rxBuffer[256];
  BYTE iCmd, iLength, paramBuffer[256];
  int iIdx = 0;
  while(m_bReceiveThreadRunning) {
    int iBytes = m_Port.receive(rxBuffer, sizeof rxBuffer);
    for(int iBufPos = 0; iBufPos < iBytes; ++iBufPos) {
      BYTE b = rxBuffer[iBufPos];
      if(iIdx < sizeof CmdPreamble) {
        if(b == CmdPreamble[iIdx]) ++iIdx;
        else iIdx = 0;
      }
      else if(iIdx == sizeof CmdPreamble) iCmd = b;
      else if(iIdx == sizeof CmdPreamble + 1) {
        iLength = b;
        if(iLength == 0) {
          receiveCmd(iCmd, NULL, 0);
          iIdx = 0;
        }
      }
      else if(iIdx >= sizeof CmdPreamble + 2) {
        int iParamIdx = iIdx - sizeof CmdPreamble - 2;
        paramBuffer[iParamIdx] = b;
        if(iParamIdx >= iLength - 1) {
          receiveCmd(iCmd, paramBuffer, iLength);
          iIdx = 0;
        }
      }
    }
  }
  m_bWatchdogRunning = false;
}
  
bool DV4Mini::sendCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength) {
  if(!pParam && iLength) {
    assert(0);
    return false;
  }

  pthread_mutex_lock(&m_lckTx);
  m_Port.transmit(CmdPreamble, sizeof CmdPreamble);
  m_Port.transmit(&iCmd, 1);
  m_Port.transmit(&iLength, 1);
  if(iLength > 0) m_Port.transmit(pParam, iLength);
  pthread_mutex_unlock(&m_lckTx);
  return true;
}

void DV4Mini::receiveCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength) {
  printf("Receive cmd %d with %d bytes\n", iCmd, iLength);  
}
