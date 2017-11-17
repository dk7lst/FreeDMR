#pragma once
#include <stdio.h>
#include <string>
#include <pthread.h>
#include <time.h>
#include <vector>
#include "../socket/socketlib.h"

class HomebrewPacket;

// Implementation of the Homebrew Repeater Protocol for connecting DMR amateur
// radio networks like BrandMeister.
// See https://bm.pd0zry.nl/index.php/Homebrew_repeater_protocol for details.
class HomebrewClient {
public:
  enum PROTOCOLDIALECT {
    PDIALECT_CLASSIC, // Classic Homebrew Protocol
    PDIALECT_MMDVM, // MMDVM variant
    PDIALECT_INVALID // must be last entry!
  };

  enum THREADSTATE {
    TSTATE_IDLE,
    TSTATE_RUNNING,
    TSTATE_REQUESTSTOP,
    TSTATE_STOPPED
  };

  enum CONNECTIONPHASE {
    CP_DISCONNECTED,
    CP_LOGIN,
    CP_AUTH,
    CP_CONFIG,
    CP_DATA
  };

  HomebrewClient();
  virtual ~HomebrewClient();

  bool open(const char *pszHostName, const char *pszPasswd, uint16_t u16Port = 62030, PROTOCOLDIALECT iDialect = PDIALECT_CLASSIC);
  void close();

  void runReceiveThread();

  int getRptId() const {
    return m_iRptId;
  }

  void setRptId(int iRptId) {
    m_iRptId = iRptId;
  }

  std::string getRptCallsign() const {
    return m_sRptCallsign;
  }

  void setRptCallsign(const std::string &sRptCallsign) {
    m_sRptCallsign = sRptCallsign;
  }

  int getRxFrequency_Hz() const {
    return m_iRxFreq_Hz;
  }

  void setRxFrequency_Hz(int iFreq_Hz) {
    m_iRxFreq_Hz = iFreq_Hz;
  }

  int getTxFrequency_Hz() const {
    return m_iTxFreq_Hz;
  }

  void setTxFrequency_Hz(int iFreq_Hz) {
    m_iTxFreq_Hz = iFreq_Hz;
  }
  
  int getTxPower_dBm() const {
    return m_iTxPower_dBm;
  }

  void setTxPower_dBm(int iTxPower_dBm) {
    m_iTxPower_dBm = iTxPower_dBm;
  }

  int getColorCode() const {
    return m_iColorCode;
  }

  void setColorCode(int iColorCode) {
    m_iColorCode = iColorCode;
  }

  double getLatitude() const {
    return m_dLatitude;
  }

  void setLatitude(double dLatitude) {
    m_dLatitude = dLatitude;
  }

  double getLongitude() const {
    return m_dLongitude;
  }

  void setLongitude(double dLongitude) {
    m_dLongitude = dLongitude;
  }

  int getHeight_m() const {
    return m_iHeight_m;
  }

  void setHeight_m(int iHeight_m) {
    m_iHeight_m = iHeight_m;
  }

  std::string getLocation() const {
    return m_sLocation;
  }

  void setLocation(const std::string &sLocation) {
    m_sLocation = sLocation;
  }

  std::string getDescription() const {
    return m_sDescription;
  }

  void setDescription(const std::string &sDescription) {
    m_sDescription = sDescription;
  }

  std::string getHomepageURL() const {
    return m_sHomepageURL;
  }

  void setHomepageURL(const std::string &sHomepageURL) {
    m_sHomepageURL = sHomepageURL;
  }

  const char *getSoftwareId(const std::string &sSoftwareId) const {
    return m_sSoftwareId.c_str();
  }

  void setSoftwareId(const std::string &sSoftwareId) {
    m_sSoftwareId = sSoftwareId;
  }

  std::string getPackageId() const {
    return m_sPackageId;
  }

  void setPackageId(const std::string &sPackageId) {
    m_sPackageId = sPackageId;
  }

  int getMaxPacketRxQueueSize() const {
    return m_iMaxPacketRxQueueSize;
  }

  void setMaxPacketRxQueueSize(int iMaxPacketRxQueueSize) {
    m_iMaxPacketRxQueueSize = iMaxPacketRxQueueSize;
  }

  HomebrewPacket *getRxPacket();
  bool sendTxPacket(const HomebrewPacket *pPacket);
  
  time_t getLastPongTime() const {
    return m_tLastPongTime;
  }

  time_t getLastPongAge_s() const {
    return time(NULL) - m_tLastPongTime;
  }

  void setSimulationMode(bool bOnOff) {
    m_bSimulationMode = bOnOff;
  }

  void setLogFile(FILE *pFile, int iLevel) {
    m_pLogFile = pFile;
    m_iLogLevel = iLevel;
  }

protected:
  int send(const void *txBuffer, int iBytes) const;

  pthread_mutex_t m_lckRxQueue;
  pthread_t m_ReceiveThread;
  THREADSTATE m_eReceiveThreadState;
  CONNECTIONPHASE m_ePhase;

  IPAddr m_ServerAddr;
  UDPSocket m_sock;
  std::string m_sServerPasswd;
  PROTOCOLDIALECT m_iDialect;

  int m_iRptId;
  std::string m_sRptCallsign;
  int m_iRxFreq_Hz, m_iTxFreq_Hz, m_iTxPower_dBm;
  int m_iColorCode;
  double m_dLatitude, m_dLongitude;
  int m_iHeight_m;
  std::string m_sLocation;
  std::string m_sDescription;
  std::string m_sHomepageURL;
  std::string m_sSoftwareId, m_sPackageId;

  std::vector<HomebrewPacket *> m_PacketRxQueue;
  int m_iMaxPacketRxQueueSize;

  time_t m_tLastPongTime;

  FILE *m_pLogFile;
  int m_iLogLevel;

  bool m_bSimulationMode;
};
