#pragma once
#include <string>
#include <openssl/evp.h>

// Calculate SHA256-Hash using OpenSSL.
// See https://www.openssl.org/docs/manmaster/crypto/EVP_DigestInit.html for Details.
class Sha256 {
public:
  static bool selftest();

  Sha256();
  ~Sha256();
  bool init();
  bool reset();
  bool put(const void *pData, int iLength);
  unsigned int getHash(unsigned char *pBuf, unsigned int uMaxSize);
  std::string getHashHex();

protected:
  EVP_MD_CTX *m_mdctx;
};
