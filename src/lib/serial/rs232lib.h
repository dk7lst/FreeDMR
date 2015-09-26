#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

class RS232Port {
public:
  // Konstruktoren:
  RS232Port();
  ~RS232Port();

  // Parameter/Attribute setzen:
  bool setBaud(int iBaud);
  //bool setDataBits(int iBits);
  //bool setStopBits(int iBits);
  bool setBytesForBlockingMode(int iBytes);

  // Port-Operationen:
  bool open(const char *pzDeviceName);
  void close();

  // Datentransfer-Operationen:
  int transmit(unsigned const char *pcBuf, int iBytes);
  int receive(unsigned char *pcBuf, int iMaxBytes);
  
protected:
  // Struktur fuer Terminal-Attribute:
  struct termios m_OrgTIO;
  
  // Datei-Handle des Schnittstellen-Devices, -1 wenn keine Schnnittstelle offen:
  int m_iFD;
  
  // Flags zum Setzen der richtigen Baudrate:
  int m_iBaudFlags;
};
