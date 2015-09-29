#pragma once
#include <pthread.h>
#include "bytesource.h"
#include "bytesink.h"

class ByteFiFo : public ByteSource, public ByteSink {
public:
  ByteFiFo();
  virtual ~ByteFiFo();

  bool reset();
  bool setSize(int iSize);

  bool put(BYTE byte);
  bool put(const BYTE *pBuffer, int iLength);

  bool get(BYTE *pByte);
  int get(BYTE *pBuffer, int iMaxLength);

protected:
  pthread_mutex_t m_lckRing;
  BYTE *m_pRing;
  int m_iSize, m_iCount;
  int m_iReadIdx, m_iWriteIdx;
};
