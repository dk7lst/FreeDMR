#pragma once
#include <stdio.h>
#include <string>

class HomebrewPacket {
public:
  enum FRAMETYPE {
    FT_Voice,
    FT_VoiceSync,
    FT_DataSync,
    FT_Invalid
  };

  HomebrewPacket();
  HomebrewPacket(const void *pRawData, int iBytes);
  virtual ~HomebrewPacket();

  bool importRaw(const void *pRawData, int iBytes);
  bool exportRaw(void *pRawData, int iBytes) const;

  std::string toString() const;

  int getSeqNum() const {
    return m_data[4];
  }

  int getSrcId() const { // source id
    return (m_data[5] << 16) | (m_data[6] << 8) | m_data[7];
  }

  int getDstId() const { // destination id
    return (m_data[8] << 16) | (m_data[9] << 8) | m_data[10];
  }

  int getRptId() const { // repeater id
    return (m_data[11] << 24) | (m_data[12] << 16) | (m_data[13] << 8) | m_data[14];
  }
  
  bool setRptId(int iRptId) {
    m_data[11] = iRptId >> 24;
    m_data[12] = iRptId >> 16;
    m_data[13] = iRptId >> 8;
    m_data[14] = iRptId;
    return true;
  }

  int getSlot() const { // time slot
    return m_data[15] & 1;
  }

  bool isGroupCall() const { // group call or private call
    return (m_data[15] & 2) == 0;
  }

  FRAMETYPE getFrameType() const {
    return FRAMETYPE((m_data[15] >> 2) & 3);
  }

  int getVoiceSeqOrDataType() const {
    return m_data[15] >> 4;
  }

  unsigned int getStreamId() const {
    return (m_data[16] << 24) | (m_data[17] << 16) | (m_data[18] << 8) | m_data[19];
  }

  const void *getData() const {
    return m_data + 20;
  }

  const void *getRawDataPtr() const {
    return m_data;
  }

  int getRawDataSize() const {
    return sizeof m_data;
  }

protected:
  BYTE m_data[53];
};
