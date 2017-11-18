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

  bool setSeqNum(int iSeqNum) {
    if(iSeqNum < 0 || iSeqNum > 255) return false;
    m_data[4] = iSeqNum;
    return true;
  }

  int getSrcId() const { // source id
    return (m_data[5] << 16) | (m_data[6] << 8) | m_data[7];
  }

  bool setSrcId(int iSrcId) { // source id
    m_data[5] = iSrcId >> 16;
    m_data[6] = iSrcId >> 8;
    m_data[7] = iSrcId;
    // TODO: Headers inside m_data need to be updated too to avoid conflicting headers!
    return true;
  }

  int getDstId() const { // destination id
    return (m_data[8] << 16) | (m_data[9] << 8) | m_data[10];
  }

  bool setDstId(int iDstId) { // destination id
    m_data[8] = iDstId >> 16;
    m_data[9] = iDstId >> 8;
    m_data[10] = iDstId;
    // TODO: Headers inside m_data need to be updated too to avoid conflicting headers!
    return true;
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

  int setSlot(int iSlot) { // time slot
    if(iSlot < 0 || iSlot > 1) return false;
    m_data[15] = iSlot ? m_data[15] | 1 : m_data[15] & ~1;
    return true;
  }

  bool isGroupCall() const { // group call or private call
    return (m_data[15] & 2) == 0;
  }

  int setGroupCall(bool bIsGroupCall) { // group call or private call
    m_data[15] = bIsGroupCall ? m_data[15] & ~2 : m_data[15] | 2;
    return true;
  }

  FRAMETYPE getFrameType() const {
    return FRAMETYPE((m_data[15] >> 2) & 3);
  }

  bool setFrameType(FRAMETYPE eFrameType) {
    m_data[15] = (m_data[15] & 0xC) | ((eFrameType & 3) << 2);
    return true;
  }

  int getVoiceSeqOrDataType() const {
    return m_data[15] >> 4;
  }

  bool setVoiceSeqOrDataType(int iVoiceSeqOrDataType) {
    m_data[15] = (m_data[15] & 0xF) | (iVoiceSeqOrDataType << 4);
    return true;
  }

  unsigned int getStreamId() const {
    return (m_data[16] << 24) | (m_data[17] << 16) | (m_data[18] << 8) | m_data[19];
  }

  bool setStreamId(int iStreamId) {
    m_data[16] = iStreamId >> 24;
    m_data[17] = iStreamId >> 16;
    m_data[18] = iStreamId >> 8;
    m_data[19] = iStreamId;
    return true;
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
