#pragma once
#include "../platform/platform.h"

class ByteSink {
public:
  ByteSink() {}
  virtual ~ByteSink() {}

  virtual bool put(BYTE byte) = 0;
  virtual bool put(const BYTE *pBuffer, int iLength) = 0;
};
