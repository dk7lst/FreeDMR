#pragma once
#include <pthread.h>
#include "../platform/platform.h"
#include "bytesource.h"
#include "bytesink.h"

class ByteFiFo : public ByteSource, public ByteSink {
public:
  ByteFiFo();
  virtual ~ByteFiFo();

  virtual bool reset();
  virtual bool setSize(int iSize);

  virtual bool put(BYTE byte);
  virtual bool put(const BYTE *pBuffer, int iLength);

  virtual bool get(BYTE *pByte);
  virtual int get(BYTE *pBuffer, int iMaxLength);

protected:
  pthread_mutex_t m_lckRing;
  BYTE *m_pRing;
  int m_iSize, m_iCount;
  int m_iReadIdx, m_iWriteIdx;
};
