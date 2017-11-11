#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sha256.h"

#if OPENSSL_VERSION_NUMBER < 0x010100000 // enable for OpenSSL below v1.1
  #define EVP_MD_CTX_new() EVP_MD_CTX_create()
  #define EVP_MD_CTX_free(mdctx) EVP_MD_CTX_destroy(mdctx)
#endif

bool Sha256::selftest() {
  Sha256 h;
  return h.init() && h.put("Hallo", 5) && !strcmp(h.getHashHex().c_str(), "753692EC36ADB4C794C973945EB2A99C1649703EA6F76BF259ABB4FB838E013E");
}

Sha256::Sha256() {
  m_mdctx = NULL;
}

Sha256::~Sha256() {
  reset();
}

bool Sha256::init() {
  reset();
  m_mdctx = EVP_MD_CTX_new();
  return m_mdctx && EVP_DigestInit_ex(m_mdctx, EVP_sha256(), NULL) == 1;
}

bool Sha256::reset() {
  if(m_mdctx) {
    EVP_MD_CTX_free(m_mdctx);
    m_mdctx = NULL;
  }
  return true;
}

bool Sha256::put(const void *pData, int iLength) {
  return m_mdctx && EVP_DigestUpdate(m_mdctx, pData, iLength) == 1;
}

unsigned int Sha256::getHash(unsigned char *pBuf, unsigned int uMaxSize) {
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len = 0, uBytes = 0;
  if(m_mdctx && EVP_DigestFinal_ex(m_mdctx, md_value, &md_len) == 1) {
    if(md_len <= uMaxSize) {
      memcpy(pBuf, md_value, md_len);
      uBytes = md_len;
    }
  }
  reset();
  return uBytes;
}

std::string Sha256::getHashHex() {
  std::string sHexStr;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len = getHash(md_value, EVP_MAX_MD_SIZE);
  char buf[8];
  for(unsigned i = 0; i < md_len; ++i) {
    sprintf(buf, "%02X", md_value[i]);
    sHexStr += buf;
  }
  return sHexStr;
}
