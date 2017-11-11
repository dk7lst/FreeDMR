#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../platform/platform.h"
#include "homebrewpacket.h"

HomebrewPacket::HomebrewPacket() {
  memset(m_data, 0, sizeof m_data);
  strcpy((char *)m_data, "DMRD");
}

HomebrewPacket::HomebrewPacket(const void *pRawData, int iBytes) {
  assert(pRawData && iBytes == sizeof m_data);
  importRaw(pRawData, iBytes);
}

HomebrewPacket::~HomebrewPacket() {
}

bool HomebrewPacket::importRaw(const void *pRawData, int iBytes) {
  if(!pRawData || iBytes != sizeof m_data) return false;
  memcpy(m_data, pRawData, iBytes);
  return true;
}

bool HomebrewPacket::exportRaw(void *pRawData, int iBytes) const {
  if(!pRawData || iBytes != sizeof m_data) return false;
  memcpy(pRawData, m_data, iBytes);
  return true;
}

std::string HomebrewPacket::toString() const {
  char buf[512];
  sprintf(buf, "DMRD %d->%d via rpt %d TS%d %s SID=%u", getSrcId(), getDstId(), getRptId(), getSlot(), isGroupCall() ? "GRP" : "PRV", getStreamId());
  return std::string(buf);
}
