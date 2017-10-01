#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include "../platform/platform.h"
#include "homebrewclient.h"

void *WatchdogThread(void *pClassInstance) {
  ((HomebrewClient *)pClassInstance)->runWatchdogThread();
  pthread_exit(NULL);
}

void *ReceiveThread(void *pClassInstance) {
  ((HomebrewClient *)pClassInstance)->runReceiveThread();
  pthread_exit(NULL);
}

HomebrewClient::HomebrewClient() {
  pthread_mutex_init(&m_lckTx, NULL);
  m_WatchdogThreadState = m_ReceiveThreadState = TSTATE_IDLE;

  m_iRptId = 0;
  m_sRptCallsign = "NOCALL";
  m_iRxFreq_Hz = m_iTxFreq_Hz = 0;
  m_iColorCode = 1;
  m_dLatitude = m_dLongitude = 0;
  m_iHeight_m = 0;
  m_sLocation = "Nowhere";
  m_sDescription = "Test Repeater";

  m_pLogFile = NULL;
  m_iLogLevel = 0;
}

HomebrewClient::~HomebrewClient() {
  close();
  pthread_mutex_destroy(&m_lckTx);
}

bool HomebrewClient::open(const char *pszHostName, const char *pszPasswd, uint16_t u16Port) {
  m_ServerAddr.setbyhostname(pszHostName);
  m_ServerAddr.setport(u16Port);
  m_sServerPasswd = pszPasswd;

  if(m_sock.open() == -1) return false;

  m_WatchdogThreadState = TSTATE_RUNNING;
  if(pthread_create(&m_WatchdogThread, NULL, WatchdogThread, this)) {
    m_WatchdogThreadState = TSTATE_IDLE;
    return false;
  }

  m_ReceiveThreadState = TSTATE_RUNNING;
  if(pthread_create(&m_ReceiveThread, NULL, ReceiveThread, this)) {
    m_ReceiveThreadState = TSTATE_IDLE;
    m_WatchdogThreadState = TSTATE_REQUESTSTOP;
    return false;
  }

  return true;
}

void HomebrewClient::close() {
  if(m_ReceiveThreadState != TSTATE_IDLE) {
    m_ReceiveThreadState = TSTATE_REQUESTSTOP;
    pthread_join(m_ReceiveThread, NULL);
  }
  if(m_WatchdogThreadState != TSTATE_IDLE) {
    pthread_join(m_WatchdogThread, NULL);
  }
}

void HomebrewClient::runWatchdogThread() {
  int iCount = 0;
  while(m_WatchdogThreadState == TSTATE_RUNNING) {
    usleep(100);
    //requestReceiveMsg();
    if(++iCount >= 10) {
      //requestWatchdogMsg();
      iCount = 0;
    }
  }
  m_WatchdogThreadState = TSTATE_STOPPED;
}

void HomebrewClient::runReceiveThread() {
  char txBuffer[512];
  sprintf(txBuffer, "RPTL%08X", m_iRptId);
  assert(strlen(txBuffer) == 12);
  int iBytes = send(txBuffer, strlen(txBuffer));
  if(m_pLogFile && m_iLogLevel >= 5) fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): send greeting: %d\n", iBytes);

  BYTE rxBuffer[512];
  while(m_ReceiveThreadState == TSTATE_RUNNING) {
    IPAddr fromAddr;
    unsigned uFromAddrLength;
    int iBytes = m_sock.recvfrom(rxBuffer, sizeof rxBuffer, &fromAddr, &uFromAddrLength);
    if(m_pLogFile && m_iLogLevel >= 10) {
      fprintf(m_pLogFile, "HomebrewClient::runReceiveThread(): Received %d bytes from %s:", iBytes, fromAddr.tostring(txBuffer));
      for(int i = 0; i < iBytes; ++i) fprintf(m_pLogFile, " %X", rxBuffer[i]);
      fputc('\n', m_pLogFile);
    }
  }
  m_sock.close();
  m_ReceiveThreadState = TSTATE_STOPPED;
  m_WatchdogThreadState = TSTATE_REQUESTSTOP;
}

int HomebrewClient::send(const void *txBuffer, int iBytes) const {
  bool bEnableLogging = m_pLogFile && m_iLogLevel >= 10;
  if(bEnableLogging) {
    char buf[1024];
    fprintf(m_pLogFile, "HomebrewClient::send(%s): \"", m_ServerAddr.tostring(buf));
    fwrite(txBuffer, iBytes, 1, m_pLogFile);
  }
  int iResult = m_bSimulationMode ? iBytes : m_sock.sendto(&m_ServerAddr, txBuffer, iBytes);
  if(bEnableLogging) fprintf(m_pLogFile, "\" (%d of %d bytes %s)\n", iResult, iBytes, m_bSimulationMode ? "simulated" : "sent");
  return iResult;
}
