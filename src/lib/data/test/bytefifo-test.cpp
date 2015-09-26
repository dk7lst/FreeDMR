#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "../../platform/platform.h"
#include "../bytefifo.h"

int main() {
  const int TESTBUFFERSIZE = 2*1024*1024, FIFOSIZE = 8*1024;
  BYTE *pInBuf = new BYTE[TESTBUFFERSIZE];
  for(int i = 0; i < TESTBUFFERSIZE; ++i) pInBuf[i] = rand() % 256;
  
  BYTE *pOutBuf = new BYTE[TESTBUFFERSIZE];
  
  ByteFiFo fifo;
  fifo.setSize(FIFOSIZE);
  
  BYTE *pRead = pInBuf, *pWrite = pOutBuf;
  while(pRead < pInBuf + TESTBUFFERSIZE) {
    int iBytes = std::min(rand() % FIFOSIZE + 1, TESTBUFFERSIZE - int(pRead - pInBuf));
#if 1
    fifo.put(pRead, iBytes);
    pRead += iBytes;
    
    pWrite += fifo.get(pWrite, iBytes);
#else
    memcpy(pWrite, pRead, iBytes);
    pRead += iBytes;
    pWrite += iBytes;
#endif
  }
  
  printf("%d Bytes read, %d bytes written: %s\n", int(pRead - pInBuf), int(pWrite - pOutBuf), memcmp(pInBuf, pOutBuf, TESTBUFFERSIZE) ? "different!" : "equal");
  delete[] pOutBuf;
  delete[] pInBuf;
  return 0;
}
