#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
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
  pthread_mutex_init(&m_lckTx, NULL);
  m_pRxSink = NULL;
  m_pLogFile = NULL;
  m_iLogLevel = 0;
  m_iRSSI = 0;
}

DV4Mini::~DV4Mini() {
  close();
  pthread_mutex_destroy(&m_lckTx);
}

bool DV4Mini::open(const char *pzDeviceName) {
  m_Port.setBaud(115200);
  if(!m_Port.open(pzDeviceName)) return false;

  m_bWatchdogRunning = true;
  if(pthread_create(&m_WatchdogThread, NULL, WatchdogThread, this)) return false;
  
  m_bReceiveThreadRunning = true;
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

bool DV4Mini::setSeed() {
  return setSeed(time(NULL));
}

bool DV4Mini::setSeed(uint32_t seed) {
  return sendCmd(ADFSETSEED, (BYTE *)&seed, sizeof seed);
}

bool DV4Mini::setLED(bool bOnOff) {
  BYTE b = bOnOff ? 1 : 0;
  return sendCmd(ADFGREENLED, &b, sizeof b);
}

bool DV4Mini::setTxBufferSize(int iLength_ms) {
  BYTE b = std::min(15, std::max(1, iLength_ms / 100));
  return sendCmd(ADFSETTXBUF, &b, sizeof b);
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
  setSeed();

  int iCount = 0;
  while(m_bWatchdogRunning) {
    usleep(100);
    requestReceiveMsg();
    if(++iCount >= 10) {
      requestWatchdogMsg();
      iCount = 0;
    }
  }
  
  setLED(false);
}

void DV4Mini::runReceiveThread() {
  BYTE rxBuffer[256];
  BYTE iCmd, iLength, paramBuffer[256];
  unsigned uIdx = 0;
  while(m_bReceiveThreadRunning) {
    int iBytes = 0;
    if(m_bSimulationMode) {
      static const BYTE testData[] = {0x71, 0xFE, 0x39, 0x1D, 0x5, 0x28, 0xFF,
        0x9C, 0x0, 0x1, 0x61, 0x8D, 0x76, 0xBD, 0xD4, 0xD5, 0x80, 0x6F, 0xAE,
        0x23,  0xA8, 0x85, 0x88, 0x7B, 0xAE, 0x99, 0x4, 0x29, 0xFB, 0xE0, 0x90,
        0xE2, 0xE1, 0xB7, 0x26, 0x9D, 0xE7, 0x1D, 0x2D, 0xA6, 0x4F, 0xD4, 0x16,
        0x9A, 0x7C, 0xE3, 0x19};
      iBytes = sizeof testData;
      assert(iBytes == 47);
      memcpy(rxBuffer, testData, iBytes);
    }
    else iBytes = m_Port.receive(rxBuffer, sizeof rxBuffer);

    if(m_pLogFile && m_iLogLevel >= 10) {
      fprintf(m_pLogFile, "DV4Mini::runReceiveThread(): Received %d bytes:", iBytes);
      for(int i = 0; i < iBytes; ++i) fprintf(m_pLogFile, " %X", rxBuffer[i]);
      fputc('\n', m_pLogFile);
    }

    for(int iBufPos = 0; iBufPos < iBytes; ++iBufPos) {
      BYTE b = rxBuffer[iBufPos];
      if(uIdx < sizeof CmdPreamble) {
        if(b == CmdPreamble[uIdx]) ++uIdx;
        else uIdx = 0;
      }
      else if(uIdx == sizeof CmdPreamble) {
        iCmd = b;
        ++uIdx;
      }
      else if(uIdx == sizeof CmdPreamble + 1) {
        iLength = b;
        ++uIdx;
        if(iLength == 0) {
          receiveCmd(iCmd, NULL, 0);
          uIdx = 0;
        }
      }
      else if(uIdx >= sizeof CmdPreamble + 2) {
        int iParamIdx = uIdx - sizeof CmdPreamble - 2;
        paramBuffer[iParamIdx] = b;
        if(iParamIdx >= iLength - 1) {
          receiveCmd(iCmd, paramBuffer, iLength);
          uIdx = 0;
        }
        else ++uIdx;
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

  if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "DV4Mini::sendCmd(): Sending cmd %d with %d bytes.\n", iCmd, iLength);
  pthread_mutex_lock(&m_lckTx);
  m_Port.transmit(CmdPreamble, sizeof CmdPreamble);
  m_Port.transmit(&iCmd, 1);
  m_Port.transmit(&iLength, 1);
  if(iLength > 0) m_Port.transmit(pParam, iLength);
  pthread_mutex_unlock(&m_lckTx);
  return true;
}

void DV4Mini::receiveCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength) {
  if(m_pLogFile && m_iLogLevel >= 3) fprintf(m_pLogFile, "DV4Mini::receiveCmd(): Received cmd %d with %d bytes\n", iCmd, iLength);
  switch(iCmd) {
    case ADFWATCHDOG:
      assert(pParam && iLength >= 2);
      m_iRSSI = be16toh(*(uint16_t *)pParam);
      break;
    case ADFGETDATA:
      if(m_pRxSink) m_pRxSink->put(pParam, iLength);
      break;
  }
}
