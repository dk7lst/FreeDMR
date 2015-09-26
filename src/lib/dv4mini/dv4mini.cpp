#include <stdlib.h>
#include <assert.h>
#include "../platform/platform.h"
#include "dv4mini.h"

const BYTE CmdPreamble[] = {0x71, 0xFE, 0x39, 0x1D};

enum {
  SETADFQRG = 1,
  SETADFMODE = 2,
  FLUSHTXBUF = 3,
  ADFWRITE = 4,
  ADFWATCHDOG = 5,
  ADFGETDATA = 7,
  ADFGREENLED = 8,
  ADFSETPOWER = 9,
  ADFDEBUG = 10,
  ADFSETSEED = 17,
  ADFVERSION = 18,
  ADFSETTXBUF = 19
};

bool DV4Mini::open(const char *pzDeviceName) {
  port.setBaud(115200);
  if(port.open(pzDeviceName)) return false;
  setMode(MODE_DMR);
  setTxPower(TXPOWER_MAX);
  return true;
}

void DV4Mini::close() {
  port.close();
}

/*  
std::string DV4Mini::getVersion() const;
void DV4Mini::setSeed();
void DV4Mini::setLED();
void DV4Mini::setBufferSize(int iLength);
*/

bool DV4Mini::setFrequency(int iHz) {
  struct SetFrequencyParameters
  {
    uint32_t rx;
    uint32_t tx;
  } param;
  assert(sizeof param == 8);
  param.rx = param.tx = htole32(iHz);
  return sendCmd(SETADFQRG, (BYTE *)&param, sizeof param);
}

bool DV4Mini::setMode(MODE mode) {
  BYTE m = mode;
  return sendCmd(SETADFMODE, &m, 1);
}

bool DV4Mini::setTxPower(TXPOWER level) {
  if(level < 0 || level > 9) {
    assert(0);
    return false;
  }
  BYTE lev = level;
  return sendCmd(ADFSETPOWER, &lev, 1);
}

/*
std::string DV4Mini::getDebug();
*/

bool DV4Mini::transmit(BYTE *pBuffer, BYTE iLength) {
  if(iLength < 1 || iLength > 245 || !pBuffer) {
    assert(0);
    return false;
  }
  return sendCmd(ADFWRITE, pBuffer, iLength);
}

bool DV4Mini::flush() {
  return sendCmd(FLUSHTXBUF, NULL, 0);
}

/*
void DV4Mini::receive();
*/

bool DV4Mini::sendCmd(BYTE iCmd, const BYTE *pParam, BYTE iLength) {
  if(!pParam && iLength) {
    assert(0);
    return false;
  }

  port.transmit(CmdPreamble, sizeof CmdPreamble);
  port.transmit(&iCmd, 1);
  port.transmit(&iLength, 1);
  if(iLength > 0) port.transmit(pParam, iLength);
  return true;
}
