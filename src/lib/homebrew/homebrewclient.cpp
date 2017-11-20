#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <string>
#include "../platform/platform.h"
#include "../data/sha256.h"
#include "../data/hexdump.h"
#include "homebrewpacket.h"
#include "homebrewclient.h"

void *ReceiveThread(void *pClassInstance) {
  ((HomebrewClient *)pClassInstance)->runReceiveThread();
  pthread_exit(NULL);
}

HomebrewClient::HomebrewClient() {
  pthread_mutex_init(&m_lckRxQueue, NULL);
  m_eReceiveThreadState = TSTATE_IDLE;
  m_ePhase = CP_DISCONNECTED;

  m_iRptId = 1;
  m_sRptCallsign = "NOCALL";
  m_iRxFreq_Hz = m_iTxFreq_Hz = 0;
  m_iTxPower_dBm = 0;
  m_iColorCode = 1;
  m_dLatitude = m_dLongitude = 0;
  m_iHeight_m = 0;
  m_sLocation = "Nowhere";
  m_sDescription = "Test Repeater";

  m_iDialect = PDIALECT_CLASSIC;

  m_iMaxPacketRxQueueSize = 50;

  m_tLastPongTime = 0;

  m_pLogFile = NULL;
  m_iLogLevel = 0;

  assert(Sha256::selftest());
}

HomebrewClient::~HomebrewClient() {
  close();
  pthread_mutex_destroy(&m_lckRxQueue);
}

bool HomebrewClient::open(const char *pszHostName, const char *pszPasswd, uint16_t u16Port, PROTOCOLDIALECT iDialect) {
  if(iDialect < 0 || iDialect >= PDIALECT_INVALID) return false;

  close();

  m_ServerAddr.setbyhostname(pszHostName);
  m_ServerAddr.setport(u16Port);
  m_sServerPasswd = pszPasswd;
  m_iDialect = iDialect;

  if(m_sock.open() == -1) return false;

  m_eReceiveThreadState = TSTATE_RUNNING;
  if(pthread_create(&m_ReceiveThread, NULL, ReceiveThread, this)) {
    m_eReceiveThreadState = TSTATE_IDLE;
    return false;
  }

  return true;
}

void HomebrewClient::close() {
  if(m_eReceiveThreadState != TSTATE_IDLE) {
    m_eReceiveThreadState = TSTATE_REQUESTSTOP;
    pthread_join(m_ReceiveThread, NULL);
  }
  m_eReceiveThreadState = TSTATE_IDLE;
  m_ePhase = CP_DISCONNECTED;
}

void HomebrewClient::runReceiveThread() {
  char txBuffer[512];
  int iTxBytes = 0;

  assert(m_ePhase == CP_DISCONNECTED);
  m_ePhase = CP_LOGIN;
  switch(m_iDialect) {
    case PDIALECT_CLASSIC:
      iTxBytes = sprintf(txBuffer, "RPTL%08X", m_iRptId);
      assert(iTxBytes == 12);
      break;
    case PDIALECT_MMDVM:
      strcpy(txBuffer, "RPTL");
      txBuffer[4] = m_iRptId >> 24;
      txBuffer[5] = m_iRptId >> 16;
      txBuffer[6] = m_iRptId >> 8;
      txBuffer[7] = m_iRptId;
      iTxBytes = 8;
      break;
    default:
      assert(false);
      break;
  }
  int iBytes = send(txBuffer, iTxBytes);
  if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): send greeting (DMR ID: %d): %d of %d bytes sent.\n", m_iRptId, iBytes, iTxBytes);

  time_t tLastPingTime = 0;
  m_tLastPongTime = time(NULL);

  BYTE rxBuffer[512];
  while(m_eReceiveThreadState == TSTATE_RUNNING) {
    IPAddr fromAddr;
    unsigned uFromAddrLength;
    int iRxBytes = m_sock.recvfrom(rxBuffer, sizeof rxBuffer, &fromAddr, &uFromAddrLength, 500000); // Timeout after 500ms to allow proper shutdown
    
    const time_t tNow = time(NULL);
    if(tNow != tLastPingTime && m_ePhase == CP_DATA) {
      iTxBytes = sprintf(txBuffer, "RPTP___%c%c%c%c", m_iRptId >> 24, m_iRptId >> 16, m_iRptId >> 8, m_iRptId); // Ping server - why 3 byte padding needed?
      iBytes = send(txBuffer, iTxBytes);
      if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): send ping to server: %d of %d bytes sent.\n", iBytes, iTxBytes);
      tLastPingTime = tNow;
    }

    if(iRxBytes <= 0) continue; // Ignore timeout

    if(m_pLogFile && m_iLogLevel >= 10) {
      fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): Phase: %d Received %d bytes from %s:\n", m_ePhase, iRxBytes, fromAddr.tostring(txBuffer));
      HexDump::hexDump(m_pLogFile, rxBuffer, iRxBytes);
    }

    if(fromAddr != m_ServerAddr) {
      if(m_pLogFile && m_iLogLevel >= 5) fputs("HomebrewClient::runReceiveThread(): Discarding packet from unknown sender!\n", m_pLogFile);
      continue;
    }

    switch(m_iDialect) {
      case PDIALECT_CLASSIC:
        assert(false); // Not yet implemented!
        break;
      case PDIALECT_MMDVM:
        if(iRxBytes == 10 && !memcmp(rxBuffer, "RPTACK", 6)) { // ACK received
          if(m_ePhase == CP_LOGIN) {
            m_ePhase = CP_AUTH;
            if(m_pLogFile && m_iLogLevel >= 5) fputs("HomebrewClient::runReceiveThread(): received ack with challenge!\n", m_pLogFile);
            Sha256 hash;
            hash.init();
            hash.put(rxBuffer + 6, 4);
            hash.put(m_sServerPasswd.c_str(), m_sServerPasswd.length());
            iTxBytes = sprintf(txBuffer, "RPTK%c%c%c%c", m_iRptId >> 24, m_iRptId >> 16, m_iRptId >> 8, m_iRptId);
            iTxBytes += hash.getHash((BYTE *)txBuffer + 8, 32);
            assert(iTxBytes == 40);
            iBytes = send(txBuffer, iTxBytes);
            if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): send challenge reply: %d of %d bytes sent.\n", iBytes, iTxBytes);
          }
          else if(m_ePhase == CP_AUTH) {
            m_ePhase = CP_CONFIG;
            iTxBytes = sprintf(txBuffer, "RPTC%c%c%c%c%-8s%09d%09d%02d%02d%+08.4f%+09.4f%03d%-20s%-20s%-124s%-40s%-40s", m_iRptId >> 24, m_iRptId >> 16, m_iRptId >> 8, m_iRptId, m_sRptCallsign.c_str(),
              m_iRxFreq_Hz, m_iTxFreq_Hz, m_iTxPower_dBm, m_iColorCode, m_dLatitude, m_dLongitude, m_iHeight_m, m_sLocation.c_str(), m_sDescription.c_str(), m_sHomepageURL.c_str(),
              m_sSoftwareId.c_str(), m_sPackageId.c_str());
            assert(iTxBytes == 302);
            iBytes = send(txBuffer, iTxBytes);
            if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): Auth successful, sending config: %d of %d bytes sent.\n", iBytes, iTxBytes);
          }
          else if(m_ePhase == CP_CONFIG) {
            m_ePhase = CP_DATA;
            if(m_pLogFile && m_iLogLevel >= 5) fputs("HomebrewClient::runReceiveThread(): received ack for config. Connection ready for use!\n", m_pLogFile);
          }
          else {
            if(m_pLogFile && m_iLogLevel >= 5) fputs("HomebrewClient::runReceiveThread(): received unexpected ACK!\n", m_pLogFile);
          }
        }
        else if(iRxBytes == 9 && !memcmp(rxBuffer, "MSTCL", 5)) { // Master is going to shutdown
          if(m_pLogFile && m_iLogLevel >= 5) fputs("HomebrewClient::runReceiveThread(): received master shutdown notice!\n", m_pLogFile);
          m_eReceiveThreadState = TSTATE_REQUESTSTOP;
        }
        else if(iRxBytes == 11 && !memcmp(rxBuffer, "MSTPONG", 7)) { // pong received
          if(m_pLogFile && m_iLogLevel >= 5) fputs("HomebrewClient::runReceiveThread(): received pong from master!\n", m_pLogFile);
          m_tLastPongTime = tNow;
        }
        else if(iRxBytes >= 53 && !memcmp(rxBuffer, "DMRD", 4)) { // DMR data received
          HomebrewPacket *p = new HomebrewPacket(rxBuffer, 53);
          if(p) {
            if(m_pLogFile && m_iLogLevel >= 50) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): received DMR data packet: %s\n", p->toString().c_str());
            if(!pthread_mutex_lock(&m_lckRxQueue)) {
              if(m_PacketRxQueue.size() > (unsigned)m_iMaxPacketRxQueueSize) delete getRxPacket();
              m_PacketRxQueue.push_back(p);
              pthread_mutex_unlock(&m_lckRxQueue);
            }
          }
        }
        break;
      default:
        assert(false);
        break;
    }
  }

  iTxBytes = sprintf(txBuffer, "RPTCL%c%c%c%c", m_iRptId >> 24, m_iRptId >> 16, m_iRptId >> 8, m_iRptId); // Logout
  iBytes = send(txBuffer, iTxBytes);
  if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): send logout to server: %d of %d bytes sent.\n", iBytes, iTxBytes);

  m_sock.close();
  m_ePhase = CP_DISCONNECTED;
  m_eReceiveThreadState = TSTATE_STOPPED;
}

HomebrewPacket *HomebrewClient::getRxPacket() {
  HomebrewPacket *p = NULL;
  if(!pthread_mutex_lock(&m_lckRxQueue)) {
    if(!m_PacketRxQueue.empty()) {
      p = m_PacketRxQueue.front();
      m_PacketRxQueue.erase(m_PacketRxQueue.begin());
    }
    pthread_mutex_unlock(&m_lckRxQueue);
  }
  return p;
}

bool HomebrewClient::sendTxPacket(const HomebrewPacket *pPacket) {
  if(!pPacket) return false;
  return send(pPacket->getRawDataPtr(), pPacket->getRawDataSize());
}

int HomebrewClient::send(const void *txBuffer, int iBytes) const {
  bool bEnableLogging = m_pLogFile && m_iLogLevel >= 10;
  if(bEnableLogging) {
    char buf[1024];
    fprintf(m_pLogFile, "HomebrewClient::send(%s):\n", m_ServerAddr.tostring(buf));
    HexDump::hexDump(m_pLogFile, (const BYTE *)txBuffer, iBytes);
  }
  int iResult = m_bSimulationMode ? iBytes : m_sock.sendto(&m_ServerAddr, txBuffer, iBytes);
  if(bEnableLogging) fprintf(m_pLogFile, " (%d of %d bytes %s)\n", iResult, iBytes, m_bSimulationMode ? "simulated" : "sent");
  return iResult;
}
