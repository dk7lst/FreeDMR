#pragma once
#include "bytefifo.h"

class BitFiFo : public ByteFiFo {
public:
  BitFiFo();
  virtual ~BitFiFo();

  virtual bool reset();

  virtual bool get(bool *pBit);

protected:
  int m_iBitIdx;
};
