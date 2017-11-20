#pragma once
#include <stdio.h>
#include "../platform/platform.h"
#include "bytesink.h"

class FileSink : public ByteSink {
public:
  FileSink();
  virtual ~FileSink();

  bool open(const char *pszFileName);
  bool close();

  bool put(BYTE byte);
  bool put(const BYTE *pBuffer, int iLength);
  
  bool isOpen() const {
    return m_pFile != NULL;
  }

protected:
  FILE *m_pFile;
};
