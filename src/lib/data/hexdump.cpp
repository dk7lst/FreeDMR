#include <stdlib.h>
#include <ctype.h>
#include "hexdump.h"

bool HexDump::hexDump(FILE *pFile, const BYTE *pData, int iDataLength, int iDataBytesPerLine, bool bFillLine) {
  if(!pFile) return false;
  fputs(hexDump(pData, iDataLength, iDataBytesPerLine, bFillLine).c_str(), pFile);
  return true;
}

std::string HexDump::hexDump(const BYTE *pData, int iDataLength, int iDataBytesPerLine, bool bFillLine) {
  std::string sResult;
  unsigned int uOffset = 0;
  while(pData && iDataLength > 0) {
    char buf[32];
    sprintf(buf, "| %08X ", uOffset);
    std::string sHex(buf), sChar;
    int iDataBytes = std::min(iDataLength, iDataBytesPerLine);
    int iFill = bFillLine ? iDataBytesPerLine - iDataBytes : 0;
    uOffset += iDataBytes;
    iDataLength -= iDataBytes;
    while(iDataBytes--) {
      sprintf(buf, "%02X ", *pData);
      sHex += buf;
      sChar += isprint(*pData) ? *pData : '.';
      ++pData;
    }
    sHex.append(iFill * 3, ' ');
    sChar.append(iFill, ' ');
    sResult += sHex;
    sResult += "| ";
    sResult += sChar;
    sResult += " |";
    if(bFillLine) sResult += '\n';
  }
  return sResult;
}
