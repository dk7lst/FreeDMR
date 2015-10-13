#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <assert.h>
#include "../platform/platform.h"
#include "bitfifo.h"

BitFiFo::BitFiFo() {
  reset();
}

BitFiFo::~BitFiFo() {
}

bool BitFiFo::reset() {
  m_iBitIdx = 0;
  return ByteFiFo::reset();
}

//bool get(bool *pBit) {
//}
