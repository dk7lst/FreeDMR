#pragma once
#include <stdio.h>
#include <string>
#include <pthread.h>
#include "../serial/rs232lib.h"
#include "../data/bytesink.h"

class DV4Mini {
public:
  enum MODE {
    MODE_DMR = 'M',
    MODE_DSTAR = 'D',
    MODE_FUSION = 'F'
  };
  
  enum TXPOWER {
    TXPOWER_MIN = 0,
    TXPOWER_HALF = 4,
    TXPOWER_MAX = 9
  };

  enum THREADSTATE {
    TSTATE_IDLE,
    TSTATE_RUNNING,
    TSTATE_REQUESTSTOP,
    TSTATE_STOPPED
  };

  DV4Mini();
  virtual ~DV4Mini();

  bool open(const char *pzDeviceName);
  void close();
  
  //std::string getVersion() const;
  bool setSeed();
  bool setSeed(uint32_t seed);
  bool setLED(bool bOnOff);
  bool setTxBufferSize(int iLength_ms);
  bool setFrequency(int iHz);
  bool setMode(MODE mode);
  bool setTxPower(TXPOWER level);
  //std::string getDebug();

  bool requestWatchdogMsg();
  bool requestReceiveMsg();

  bool transmit(BYTE *pBuffer, BYTE iLength);
  bool flush();

  void runWatchdogThread();
  void runReceiveThread();

  bool setRxSink(ByteSink *pSink) {
    m_pRxSink = pSink;
    return true;
  }

  void setSimulationMode(bool bOnOff) {
    m_bSimulationMode = bOnOff;
  }

  void setLogFile(FILE *pFile, int iLevel) {
    m_pLogFile = pFile;
    m_iLogLevel = iLevel;
  }

  int getRSSI() const {
    return m_iRSSI;
  }

protected:
  bool sendCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength);
  void receiveCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength);

  pthread_mutex_t m_lckTx;
  pthread_t m_WatchdogThread, m_ReceiveThread;
  THREADSTATE m_WatchdogThreadState, m_ReceiveThreadState;

  RS232Port m_Port;
  
  ByteSink *m_pRxSink;

  FILE *m_pLogFile;
  int m_iLogLevel;

  bool m_bSimulationMode;
  int m_iRSSI;
};
