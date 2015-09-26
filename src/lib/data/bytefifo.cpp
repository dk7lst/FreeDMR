#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "../platform/platform.h"
#include "bytefifo.h"

ByteFiFo::ByteFiFo() {
  m_pRing = NULL;
  m_iSize = 0;
  pthread_mutex_init(&m_lckRing, NULL);
  reset();
}

ByteFiFo::~ByteFiFo() {
  pthread_mutex_destroy(&m_lckRing);
  delete m_pRing;
}

bool ByteFiFo::reset() {
  pthread_mutex_lock(&m_lckRing);
  m_iCount = m_iReadIdx = m_iWriteIdx = 0;
  pthread_mutex_unlock(&m_lckRing);
  return true;
}

bool ByteFiFo::setSize(int iSize) {
  pthread_mutex_lock(&m_lckRing);
  reset();
  delete[] m_pRing;
  m_pRing = new BYTE[iSize];
  m_iSize = m_pRing ? iSize : 0;
  pthread_mutex_unlock(&m_lckRing);
  return m_pRing != NULL;
}

bool ByteFiFo::put(BYTE byte) {
  bool bSuccess = false;
  pthread_mutex_lock(&m_lckRing);
  if(m_iCount < m_iSize) {
    m_pRing[m_iWriteIdx] = byte;
    if(++m_iWriteIdx >= m_iSize) m_iWriteIdx = 0;
    ++m_iCount;
    bSuccess = true;
  }
  pthread_mutex_unlock(&m_lckRing);
  return bSuccess;
}

bool ByteFiFo::put(BYTE *pBuffer, int iLength) {
  bool bSuccess = false;
  pthread_mutex_lock(&m_lckRing);
  if(iLength <= m_iSize - m_iCount) {
    int iBytesBeforeWrap = m_iSize - m_iWriteIdx;
    if(iBytesBeforeWrap > iLength) {
      memcpy(m_pRing + m_iWriteIdx, pBuffer, iBytesBeforeWrap);
      memcpy(m_pRing, pBuffer + iBytesBeforeWrap, iLength - iBytesBeforeWrap);
    }
    else memcpy(m_pRing + m_iWriteIdx, pBuffer, iLength);
    m_iWriteIdx += iLength;
    if(m_iWriteIdx >= m_iSize) m_iWriteIdx -= m_iSize;
    m_iCount += iLength;
    bSuccess = true;
  }
  pthread_mutex_unlock(&m_lckRing);
  return bSuccess;
}

bool ByteFiFo::get(BYTE *pByte) {
  bool bSuccess = false;
  pthread_mutex_lock(&m_lckRing);
  if(m_iCount > 0) {
    *pByte = m_pRing[m_iReadIdx];
    if(++m_iReadIdx >= m_iSize) m_iReadIdx = 0;
    --m_iCount;
    bSuccess = true;
  }
  pthread_mutex_unlock(&m_lckRing);
  return bSuccess;
}

int ByteFiFo::get(BYTE *pByte, int iMaxLength) {
  int iBytesRead = 0;
  pthread_mutex_lock(&m_lckRing);
  while(m_iCount > 0 && iBytesRead < iMaxLength) {
    int iBytes = std::min(m_iSize - m_iReadIdx, iMaxLength - iBytesRead);
    memcpy(pByte, m_pRing + m_iReadIdx, iBytes);
    pByte += iBytes;
    iBytesRead += iBytes;
    m_iCount -= iBytes;
  }
  pthread_mutex_unlock(&m_lckRing);
  return iBytesRead;
}
