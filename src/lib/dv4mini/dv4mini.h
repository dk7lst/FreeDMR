#include <string>
#include <pthread.h>
#include "../serial/rs232lib.h"

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

  DV4Mini();
  virtual ~DV4Mini();

  bool open(const char *pzDeviceName);
  void close();
  
  //std::string getVersion() const;
  //bool setSeed();
  //bool setLED();
  //bool setBufferSize(int iLength);
  bool setFrequency(int iHz);
  bool setMode(MODE mode);
  bool setTxPower(TXPOWER level);
  //std::string getDebug();

  bool requestWatchdogMsg();
  bool requestReceiveMsg();

  bool transmit(BYTE *pBuffer, BYTE iLength);
  bool flush();
  int receive();

  void runWatchdogThread();
  void runReceiveThread();

protected:
  bool sendCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength);
  void receiveCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength);
  
  pthread_mutex_t m_lckTx;
  pthread_t m_WatchdogThread, m_ReceiveThread;
  RS232Port m_Port;
  bool m_bWatchdogRunning, m_bReceiveThreadRunning;
};
