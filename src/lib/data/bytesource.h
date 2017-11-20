#pragma once
#include "../platform/platform.h"

class ByteSource {
public:
  ByteSource() {}
  virtual ~ByteSource() {}

  virtual bool get(BYTE *pByte) = 0;
  virtual int get(BYTE *pBuffer, int iMaxLength) = 0;
};
