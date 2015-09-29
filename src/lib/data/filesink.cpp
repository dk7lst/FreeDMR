#include <stdlib.h>
#include "../platform/platform.h"
#include "filesink.h"

FileSink::FileSink() {
  m_pFile = NULL;
}

FileSink::~FileSink() {
  close();
}

bool FileSink::open(const char *pszFileName) {
  close();
  m_pFile = fopen(pszFileName, "w");
  return m_pFile != NULL;
}

bool FileSink::close() {
  if(m_pFile) {
    fclose(m_pFile);
    m_pFile = NULL;
  }
  return true;
}

bool FileSink::put(BYTE byte) {
  if(!m_pFile) return false;
  return fputc(byte, m_pFile) != EOF;
}

bool FileSink::put(const BYTE *pBuffer, int iLength) {
  if(!m_pFile) return false;
  return fwrite(pBuffer, 1, iLength, m_pFile) == size_t(iLength);
}
