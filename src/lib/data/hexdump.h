#pragma once
#include <stdio.h>
#include <string>
#include "../platform/platform.h"

class HexDump {
public:
  static bool hexDump(FILE *pFile, const BYTE *pData, int iDataLength, int iDataBytesPerLine = 16, bool bFillLine = true);
  static std::string hexDump(const BYTE *pData, int iDataLength, int iDataBytesPerLine = 16, bool bFillLine = true);
};
