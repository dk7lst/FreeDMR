#include <netinet/in.h>

// Repräsentation einer IPv4 Unicast bzw. Multicast-Adresse
class IPAddr {
public:
 struct sockaddr_in a;

 IPAddr();
 IPAddr(unsigned long n_addr, unsigned short n_port);
 void clear();
 char *tostring(char *buf);
 char *tostringwop(char *buf);
 int setbydotstring(const char *dotstr);
 int setbyhostname(const char *hostname);
 int setbyipport(const char *hostport);
 uint16_t getport();
 void setport(uint16_t port);
 int isset();
 int ismulticast();
 int islocalif();
};

// Methoden und Attribute, die für alle Sockets gültig sind
class AbstractSocket {
protected:
 int sockfd, socketmode;
 int reuseaddrflag;

public:  
 AbstractSocket();
 virtual ~AbstractSocket();
 
 virtual int open();
 virtual int close();
 virtual int bind(); // Port automatisch wählen
 virtual int bind(uint16_t port); // Port an allen Interfaces binden
 virtual int bind(IPAddr *interface); // Port nur an einem Interface binden
 virtual int bind(IPAddr *bindaddr, uint16_t portoverride);
 virtual int getsockname(IPAddr *bindaddr, unsigned *bindaddrlen);
 virtual void enablereuseaddr(int onoff);
 virtual int enablerttlrecording(int onoff);
 //virtual int enableblockingmode(int onoff); 
 virtual int getttl();
 virtual int setttl(int ttl);
 virtual int getsocket() {return sockfd;}
};

// Schnittstelle zu den Funktionen des Betriebsystems
class UDPSocket : public AbstractSocket {
public:
 UDPSocket();
 virtual int sendto(IPAddr *destination, const void *buf, int bytes);
 virtual int recvfrom(void *buf, int maxbytes, IPAddr *fromaddr, unsigned *fromaddrlen);
 virtual int recvfrom(void *buf, int maxbytes, IPAddr *fromaddr, unsigned *fromaddrlen, int *ttl);
};

class MulticastUDPSocket : public UDPSocket {
public:
 MulticastUDPSocket();
 
 virtual int join(IPAddr *mcgroup);
 virtual int join(IPAddr *mcgroup, IPAddr *interface);
 virtual int leave(IPAddr *mcgroup);
 virtual int getttl();
 virtual int setttl(int ttl);
 virtual int enableloopmode(int flag);
 virtual int setoutgoinginterface(IPAddr *interface);
};

class TCPSocket : public AbstractSocket {
public:
 TCPSocket();

 virtual int connect(IPAddr *destination);
 virtual int send(const void *buf, int bytes);
 virtual int sendline(const char *str);
 virtual int recv(void *buf, int maxbytes);
 virtual int recvline(void *buf, int maxbytes);
};
