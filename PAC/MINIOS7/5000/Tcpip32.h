#ifndef _TCPIP_H_
#define _TCPIP_H_

#define NNETS 1
#define NCONFIGS 20

#define NCONNS 32
#define NBUFFS 31
#define FIFO_SIZE 16

/* [06/20/2002]
#define MAXBUF 1592
*/
#define MAXBUF 1536

#define FRAGMENTATION 3
#define IPOPTIONS
#define USS_IP_MC_LEVEL 0
#undef MIB2
#define RELAYING 1
#define chksum_INASM
#define DHCP 2
#undef DNS
#undef PPP

#define USERID "test"
#define PASSWD "usnet"

#define LITTLE /* 10/17/2003 add */

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================================================
   local initialization and termination code for Ninit() and Nterm() */
int Nclkinit(void); /* timer setup routine */
void Nclkterm(void);
#define LOCALSETUP() Nclkinit()
#define LOCALSHUTOFF() Nclkterm()

/* Add by TKT */
void SetLocalHostName(char *hostname); /* net.c [90/04/17] */

/*********** socket.h ***************************************/

// #ifndef errno
// extern int      errno;
// #endif

/* protocol family */

#define PF_UNSPEC 0 /* unspecified protocol family */
#define PF_INET 2   /* TCP/IP and related */
#define AF_UNSPEC 0 /* unspecified address family */
#define AF_INET 2   /* TCP/IP and related */

/* socket types */

#define SOCK_STREAM 1 /* stream socket */
#define SOCK_DGRAM 2  /* datagram socket */
#define SOCK_RAW 3    /* raw-protocol interface */

/* options for getsockopt() and setsockopt() */

#define SOL_SOCKET 0xffff  /* options for socket level */
#define IPPROTO_TCP 0x0001 /* options for TCP level */
#define IPPROTO_IP 0x0002  /* options for IP level */

#define SO_DEBUG 0x0001     /* turn on debugging info recording */
#define SO_REUSEADDR 0x0004 /* allow local address reuse */
#define SO_KEEPALIVE 0x0008 /* keep connections alive */
#define SO_DONTROUTE 0x0010 /* just use interface addresses */
#define SO_BROADCAST 0x0020 /* permit sending of broadcast msgs */
#define SO_LINGER 0x0080    /* linger on close if data present */
#define SO_OOBINLINE 0x0100 /* leave received OOB data in line */
#define SO_SNDBUF 0x1001    /* send buffer size */
#define SO_RCVBUF 0x1002    /* receive buffer size */
#define SO_ERROR 0x1007     /* get error status and clear */
#define SO_TYPE 0x1008      /* get socket type */

#define TCP_MAXSEG 0x2000  /* maximum segment size */
#define TCP_NODELAY 0x2001 /* immediate send() */

#define IP_OPTIONS 0x0001 /* IP header options */

/* options for recv and send */

#define MSG_OOB 0x01       /* send or receive out of band data */
#define MSG_PEEK 0x02      /* take data but leave it */
#define MSG_DONTROUTE 0x04 /* do not route */

/* options for fcntl */

#define O_NDELAY 0x04    /* non-blocking */
#define FNDELAY O_NDELAY /* synonym */
#define F_GETFL 3        /* get flags */
#define F_SETFL 4        /* set flags */

/* options for ioctl */

#define SIOCATMARK 7 /* check for out of bound data */
#define FIONBIO 126  /* set/clear non-blocking I/O */
#define FIONREAD 127 /* number of bytes to read */

/* structures */

struct sockaddr {           /* generic socket address */
  unsigned short sa_family; /* address family */
  char sa_data[14];         /* up to 14 bytes of direct address */
};
struct in_addr { /* Internet address */
  unsigned long s_addr;
};
struct sockaddr_in {       /* Internet socket address */
  short sin_family;        /* should be unsigned but this is BSD */
  unsigned short sin_port; /* network order !!! */
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct iovec {    /* address and length */
  char *iov_base; /* base */
  int iov_len;    /* size */
};
struct msghdr {          /* Message header for recvmsg and sendmsg. */
  char *msg_name;        /* optional address */
  int msg_namelen;       /* size of address */
  struct iovec *msg_iov; /* scatter/gather array */
  int msg_iovlen;        /* # elements in msg_iov */
  char *msg_accrights;   /* access rights sent/received */
  int msg_accrightslen;
};

struct hostent {              /* structure for gethostbyname */
  char *h_name;               /* official name of host */
  char **h_aliases;           /* alias list */
  int h_addrtype;             /* host address type */
  int h_length;               /* length of address */
  char **h_addr_list;         /* list of addresses from name server */
#define h_addr h_addr_list[0] /* address, for backward compatiblity */
};

struct servent {    /* structure for getservbyname */
  char *s_name;     /* official service name */
  char **s_aliases; /* alias list */
  int s_port;       /* port # */
  char *s_proto;    /* protocol to use */
};

struct linger { /* structure for the SO_LINGER option */
  int l_onoff;  /* zero=off, nonzero = on */
  int l_linger; /* linger time, in seconds */
};

#define CHAR_BIT_CNT 8
#define FD_SET(n, p) \
  ((p)->fds_bits[(n) / CHAR_BIT_CNT] |= (1 << ((n) % CHAR_BIT_CNT)))
#define FD_CLR(n, p) \
  ((p)->fds_bits[(n) / CHAR_BIT_CNT] &= ~(1 << ((n) % CHAR_BIT_CNT)))
#define FD_ISSET(n, p) \
  ((p)->fds_bits[(n) / CHAR_BIT_CNT] & (1 << ((n) % CHAR_BIT_CNT)))
#define FD_ZERO(p) memset((void *)(p), 0, sizeof(*(p)))

typedef struct fd_set { /* Bit mask for select() */
  unsigned char fds_bits[(NCONNS + CHAR_BIT_CNT - 1) / CHAR_BIT_CNT];
} fd_set;

struct timeval { /* Timeout format for select() */
  long tv_sec;   /* seconds */
  long tv_usec;  /* microseconds */
};

/* BSD socket error codes */

#define NE_PARAM -10     /* user parameter error */
#define EHOSTUNREACH -11 /* host not reachable */
#define ETIMEDOUT -12    /* timeout */
#define ECONNABORTED -14 /* protocol error */
#define ENOBUFS -15      /* no buffer space */
#define EBADF -16        /* connection block invalid */
#define EFAULT -17       /* invalid pointer argument */
#define EWOULDBLOCK -18  /* operation would block */
#define EMSGSIZE -19     /* message too long */
#define ENOPROTOOPT -20  /* Protocol not available */

#define EDESTADDRREQ -50    /* Destination address required */
#define EPROTOTYPE -52      /* Protocol wrong type for socket */
#define EPROTONOSUPPORT -54 /* Protocol not supported */
#define ESOCKTNOSUPPORT -55 /* Socket type not supported */
#define EOPNOTSUPP -56      /* Operation not supported on socket */
#define EPFNOSUPPORT -57    /* Protocol family not supported */
#define EAFNOSUPPORT -58    /* Address family not supported by */
                            /* protocol family */
#define EADDRINUSE -59      /* Address already in use */
#define EADDRNOTAVAIL -60   /* Can't assign requested address */
#define ENETDOWN -61        /* Network is down */
#define ENETUNREACH -62     /* Network is unreachable */
#define ENETRESET -63       /* Network dropped connection because */
                            /* of reset */
#define ECONNRESET -65      /* Connection reset by peer */
#define EISCONN -67         /* Socket is already connected */
#define ENOTCONN -68        /* Socket is not connected */
#define ESHUTDOWN -69       /* Can't send after socket shutdown */
#define ECONNREFUSED -72    /* Connection refused */
#define EHOSTDOWN -73       /* Host is down */
#define EALREADY -76        /* operation already in progress */
#define EINPROGRESS -77     /* operation now in progress */

/* byte swapping routines */

unsigned short htons(unsigned short);
#define ntohs(val) htons(val)
unsigned long htonl(unsigned long);
#define ntohl(val) htonl(val)

/* function prototypes */

int Ninit(void);
int Nterm(void);
int Portinit(char *port);
int Portterm(char *port);
/*int             Nprintf(char *form,...);*/
/*void            Nputchr(char); */
int Nchkchr(void);
int Ngetchr(void);

int selectsocket(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                 struct timeval *timeout);
int accept(int s, struct sockaddr *name, int *namelen);
int bind(int s, struct sockaddr *name, int namelen);
int connect(int s, struct sockaddr *name, int namelen);
int getsockname(int s, struct sockaddr *name, int *namelen);
int getpeername(int s, struct sockaddr *peer, int *addrlen);
int getsockopt(int s, int level, int optname, char *optval, int *optlen);
int setsockopt(int s, int level, int optname, char *optval, int optlen);
int listen(int s, int backlog);
int recv(int s, char *buf, int len, int flags);
#define readsocket(s, buf, len) recv(s, buf, len, 0)
int recvfrom(int s, char *buf, int len, int flags, struct sockaddr *from,
             int *fromlen);
int recvmsg(int s, struct msghdr *msg, int flags);
int send(int s, char *buf, int len, int flags);
#define writesocket(s, buf, len) send(s, buf, len, 0)
int sendto(int s, char *buf, int len, int flags, struct sockaddr *to,
           int tolen);
int sendmsg(int s, struct msghdr *msg, int flags);
int shutdown(int s, int how);
int socket(int domain, int type, int protocol);
int closesocket(int conno);
struct hostent *gethostbyname_r(char *hnp, struct hostent *result, char *buffer,
                                int buflen, int *h_errnop);
struct hostent *gethostbyname(char *hnp);
struct hostent *gethostbyaddr_r(char *addr, int len, int type,
                                struct hostent *result, char *buffer,
                                int buflen, int *h_errnop);
struct hostent *gethostbyaddr(char *addr, int len, int type);
int fcntlsocket(int fildes, int cmd, int arg);
int ioctlsocket(int fildes, int request, ...);
void sleepsocket(int sec);
unsigned long inet_addr(char *dotted);
char *inet_ntoa(struct in_addr addr);

/* net.h */

/*
 */

#include <string.h> /* needed for size_t type definition */

#ifdef _OLD_PROTOCOL_TABLE_
#define IP 1   /* Internet Protocol: connectionless */
#define ICMP 2 /* Internet Control Message Protocol */
#define ARP 3  /* Address Resolution Protocol */
#define RARP 4 /* Reverse Address Resolution Protocol */
#define UDP 5  /* User Datagram Protocol: simple connections */
#define TCP                                          \
  6              /* Transport Control Protocol: full \
                    connections */
/*#define LQRP*/ /* Link Quality Report Protocol */
#else

#define IP 1   /* Internet Protocol: connectionless */
#define TCP 2  /* Transport Control Protocol: full connections */
#define UDP 3  /* User Datagram Protocol: simple connections */
#define ARP 4  /* Address Resolution Protocol */
#define ICMP 5 /* Internet Control Message Protocol */
#define RARP 6 /* Reverse Address Resolution Protocol */

#endif

/* =========================================================================
   Configuration definitions. */

#define LHDRSZ 14         /* size of link level header */
#define TOUT_READ 20000UL /* default timeout ms for read */
#define REASSTOUT 60000UL /* reassembly timeout ms */
#define ARPTOUT 60000UL   /* ARP address void in this time, 0 = never */
#define MAXWACK 4         /* maximum messages in the wait for ACK queue */
#define MAXWIND 4         /* window size in full buffers */
#define MAXOUTQLEN 10     /* maximum output queue length */

/* =========================================================================
   Macros and definitions to simplify coding. */

#define ASCII 0 /* file mode: text */
#define IMAGE 1 /* binary */

#define QUEUE_EMPTY(ptr, qname) (ptr->qname.mhead == ptr->qname.mtail)
#define QUEUE_FULL(ptr, qname) \
  (ptr->qname.mhead ==         \
   ((ptr->qname.mtail + 1) &   \
    ((sizeof(ptr->qname.mp) / sizeof(ptr->qname.mp[0])) - 1)))
#define QUEUE_IN(ptr, qname, mess)            \
  ptr->qname.mp[ptr->qname.mtail] = mess;     \
  ptr->qname.mtail = (ptr->qname.mtail + 1) & \
                     ((sizeof(ptr->qname.mp) / sizeof(ptr->qname.mp[0])) - 1);
#define QUEUE_OUT(ptr, qname, mess)           \
  mess = ptr->qname.mp[ptr->qname.mhead];     \
  ptr->qname.mhead = (ptr->qname.mhead + 1) & \
                     ((sizeof(ptr->qname.mp) / sizeof(ptr->qname.mp[0])) - 1);
#define MESS_OUT(ptr, mess)                                            \
  mess = ptr->first;                                                   \
  if ((ptr->first = mess->next) == 0) ptr->last = (MESS *)&ptr->first; \
  ptr->ninque--;

#ifndef FARDEF
#define FAR
#define Nfarcpy memcpy
#else
#define FAR FARDEF
void Nfarcpy(char FAR *, char FAR *, int);
#endif

/* =========================================================================
   Structures. */

/* external or Ethernet address format */
#define Eid_SZ 6
struct Eid {
  unsigned char c[Eid_SZ];
};
/* internal or IP address format */
#define Iid_SZ 4
typedef union {
  unsigned char c[Iid_SZ];
  unsigned short s[Iid_SZ / 2];
  unsigned long l;
} Iid;
typedef union {
  unsigned char b[4];
  unsigned long l;
} IPaddr;
#define EQIID(id1, id2) (*(long *)&id1 == *(long *)&id2)
#define Ihdr_SZ 20

/* IP address interpretation function for multicast */
/* note that IP address is stored in network byte order */
#ifdef LITTLE
#define USS_ISCLASSD(iidl) ((iidl & 0xe0UL) == 0xe0UL)
#else
#define USS_ISCLASSD(iidl) ((iidl & 0xe0000000UL) == 0xe0000000UL)
#endif

#define ussBroadcastIndex 255 /* special conf index for broadcast */
#define ussMulticastIndex 254 /* special conf index for multicast */

#define ussDfltMcNetno 0 /* by default, mcast messages sent to this net */

/* Indices for the protoc array in the NET structure */
#define ussLinkIndex 0
#define ussDriverIndex 1
#define ussAdapterIndex 2

/* Commands for ioctl request argument */
enum ioctlreq {
  ussIdentE,
  ussDevBaudSetE,
  ussLinkIsUpE,
  ussLinkIsDownE,
  ussLinkBringUpE,
  ussLinkBringDownE,
  ussInterfaceBringUpE,
  ussInterfaceBringDownE,
  ussPPPUserIdSetE,
  ussPPPUserIdGetE,
  ussPPPPasswordSetE,
  ussPPPPasswordGetE,
  ussPPPDialEnableE,
  ussPPPDialDisableE,
  ussMcastGroupJoinE,
  ussMcastGroupLeaveE
};

struct MESSH {          /* internal message header */
  struct MESSH *next;   /* chain pointer */
  unsigned long timems; /* time stamp */
  unsigned long target; /* IP address of target */
  unsigned short mlen;  /* message length */
  unsigned char netno;  /* network number */
  char offset;          /* offset to current level */
  unsigned char confix; /* network configuration index */
  char conno;           /* connection number */
  short id;             /* message identification */
};
typedef struct MESSH MESS;
#define MESSH_SZ ((sizeof(MESS) + 3) & ~3)
#define bFUTURE 0x7777        /* buffer in future queue */
#define bFREE 0x7676          /* buffer free */
#define bALLOC 0x7575         /* buffer allocated */
#define bWACK 0x7474          /* buffer in wait-for-ack queue */
#define bRELEASE 0x7373       /* buffer to be released */
#define boRELEASED (char)0xff /* offset for released buffer */
#define boTXDONE (char)0xfd   /* offset for transmit done */

#define PTABLE const struct P_tab /* typedef caused trouble */
struct P_tab {                    /* protocol table, at end of each module */
  char name[10];                  /* name of protocol */
  int (*init)(int, char *);       /* initialize */
  void (*shut)(int);              /* shut */
  int (*screen)(MESS *);          /* screen */
  int (*opeN)(int, int);          /* open */
  int (*closE)(int);              /* close */
  MESS *(*reaD)(int);             /* receive */
  int (*writE)(int, MESS *);      /* send */
  int (*ioctl)(void *, enum ioctlreq, void *, size_t);
  /* i/o control */
  int Eprotoc;          /* external protocol num */
  unsigned char hdrsiz; /* header size */
};

/* =========================================================================
   Link level protocols. */

extern PTABLE ussEthernetTable;

/* These #defines provide shorthand names for the above */
#define Ethernet &ussEthernetTable

/* =========================================================================
   Driver level protocols. */

extern PTABLE ussNE2000Table;
extern PTABLE ussWrapTable;

/* More shorthand.  These names are used in netconf.c */
#define NE2000 &ussNE2000Table
#define WRAP &ussWrapTable

/*[06/20/2002]
struct FIFOQ8 {
    MESS           *mp[8];
    int             mhead,
                    mtail;
};
*/

struct FIFOQ16 {
  MESS *mp[FIFO_SIZE];
  int mhead, mtail;
};

struct NETDATA { /* network configuration table - ROM */
  char *name;    /* host name */
  char *pname;   /* port name */
  /* the following are really type Iid but some compilers will not allow
     initial data for unions */
  struct {
    unsigned char c[Iid_SZ];
  } /* address mask, 0 = host part */
  Imask,
      Iaddr;        /* internal (Internet) address */
  struct Eid Eaddr; /* external (Ethernet) address */
  char flags;       /* configuration flags */
  PTABLE *lprotoc;  /* link level protocol */
  PTABLE *dprotoc;  /* driver */
  PTABLE *adapter;  /* adapter */
  char *params;     /* pointer to setup parameters */
};
/* configuration flag bits */
#define NOTUSED 1 /* configured out */
#define BOOTSERVER 2
#define TIMESERVER 4   /* will respond to ICMP time requests */
#define ROUTER 0x08    /* host is a default router or gateway */
#define LOCALHOST 0x10 /* this is me */
#define NODE 0x20      /* host has at least 2 networks */
#define DIAL 0x40      /* dial-up line */
#define DNSVER 0x80    /* host is a DNS server */

/* flags to be used in Nopen */
#define IPADDR 0x0100 /* name is IP address (binary */

/* =========================================================================
   Error codes.  Use values -100, -101 etc. for private codes. */

#define NE_PARAM -10     /* user parameter error */
#define EHOSTUNREACH -11 /* host not reachable */
#define ETIMEDOUT -12    /* timeout */
#define NE_HWERR -13     /* hardware error */
#define ECONNABORTED -14 /* protocol error */
#define ENOBUFS -15      /* no buffer space */
#define EBADF -16        /* connection block invalid */
#define EFAULT -17       /* invalid pointer argument */
#define EWOULDBLOCK -18  /* operation would block */
#define EMSGSIZE -19     /* message too long */
#define ENOPROTOOPT -20  /* Protocol not available */
#define ussErrInval -21  /* this ioctl request not implemented */

/* old codes */
#define NE_NOTCONF EHOSTUNREACH
#define NE_TIMEOUT ETIMEDOUT
#define NE_NOBUFS ENOBUFS
#define NE_PROTO ECONNABORTED

/* =========================================================================
   Function prototypes. */

void Ninitbuf(int size, int count);
MESS *Ngetbuf(void);
MESS *NgetbufIR(void);
void Nrelbuf(MESS *bufptr);
void NrelbufIR(MESS *bufptr);
void Ncntbuf(void);
int BuildRoutes(void);
int GetHostData(unsigned long iid, int addflag);
int Ninit(void);
int Nterm(void);
int Portinit(char *port);
int Portterm(char *port);
int Nopen(char *to, char *protoc, int myport, int herport, int flags);
int Nclose(int conno);
int Nwrite(int conno, char *mess, int len);
int Nread(int conno, char *mess, int len);
int BOOTPnames(char *host, char *file);
void FAR *BOOTP(char FAR *base, char *host, char *file);
void BOOTPserv(void);
void TFTPserv(void);
int TFTPput(char *host, char *file, int mode);
int TFTPget(char *host, char *file, int mode);
void FTPserv(void);
int FTPgetput(char *host, char *file, int mode);
#define FTPget(host, file, mode) FTPgetput(host, file, mode)
#define FTPput(host, file, mode) FTPgetput(host, file, mode + 2)
int getEid(unsigned long iid);
int pppTimeout(int netno);
int ussHostGroupJoin(Iid iid, int netno);
int ussHostGroupLeave(Iid iid, int netno);

/*******************************/
/*
 */

/* event numbers, dependent on the multitasker */
#define SIG_RC(conno) (conno + conno + EVBASE)          /* read connection */
#define SIG_CC(conno) (conno + conno + 1 + EVBASE)      /* control connection */
#define SIG_RN(netno) (2 * netno + 2 * NCONNS + EVBASE) /* read network */
#define SIG_WN(netno) (2 * netno + 2 * NCONNS + 1 + EVBASE) /* write network \
                                                             */
#define SIG_ARP (2 * (NCONNS + NNETS) + EVBASE)             /* ARP */
#define SIG_GEN (2 * (NCONNS + NNETS) + 1 + EVBASE) /* general purpose */

void NetTask(int netno);

/* =========================================================================
   Structures that are affected by LOCAL.H so they can't be in NET.H */

struct NET {             /* structure defining a network */
  int netstat;           /* status bits */
  PTABLE *protoc[3];     /* link, driver, adapter protocol */
  unsigned char confix;  /* number in configuration table */
  char sndoff;           /* offset to sender's address */
  unsigned long tout;    /* basic timeout in milliseconds */
  int maxblo;            /* maximum block size for link */
  int nettasktout;       /* timeout for network task */
  char cflags;           /* configuration flags */
  char worktodo;         /* flag for network task */
  unsigned char none1;   /* alignment */
  unsigned char state;   /* state */
  struct FIFOQ16 arrive; /* circular list of arrivals */
  MESS *fragmq;          /* linked sorted list of fragments */
  MESS *fragmh;          /* reassembled message */
  int irno[4];           /* interrupt numbers */
  int port;              /* I/O port */
  char FAR *base[2];     /* for memory-mapped I/O */
  struct Eid id;         /* board id */
  unsigned char netno;   /* network number */
  char hwflags;          /* hardware level flags */
  MESS *bufbas;          /* input buffer base */
  MESS *bufbaso;         /* output buffer base */
  struct FIFOQ16 depart; /* circular list of departures */
  long bps;              /* bits per second */
#ifdef DHCP
  unsigned long DHCPserver; /* DHCP server IP address */
  unsigned long LeaseTime;  /* DHCP times */
  unsigned long RenewalTime;
  unsigned long RebindingTime;
  unsigned long DHCPsecs;
#endif
  char *ifDescr;
  int ifType;
  int ifAdminStatus;
#ifdef MIB2
  unsigned long ifLastChange;
  unsigned long ifInOctets;
  unsigned long ifInUcastPkts;
  unsigned long ifInNUcastPkts;
#endif
  unsigned long ifInDiscards;
  unsigned long ifInErrors;
#ifdef MIB2
  unsigned long ifInUnknownProtos;
  unsigned long ifOutOctets;
  unsigned long ifOutUcastPkts;
  unsigned long ifOutNUcastPkts;
#endif
  unsigned long ifOutDiscards;
  unsigned long ifOutErrors;
#ifdef LQRP
  unsigned long ownLQRPms;
  unsigned long peerLQRPms;
  char lastins[32];
  unsigned long OutLQRs;
  unsigned long InLQRs;
  unsigned long InGoodOctets;
#endif
  /* all hardware net structures must fit in SERIAL, use filler if necessary */
  struct SERIAL {                     /* hardware net data for serial lines */
    unsigned long ul1, ul2;           /* miscellaneous */
    unsigned long timems;             /* time stamp */
    void (*comec)(int, struct NET *); /* character from driver */
    int (*goingc)(struct NET *);      /* character to driver */
    char *bufin;                      /* input buffer pointer */
    char *buflim;                     /* input buffer upper limit */
    char *bufout;                     /* output buffer pointer */
    char baudctl[2];                  /* baud rate controls */
    int chsout;                       /* output buffer counter */
    unsigned char lastin;             /* last arrived character */
    unsigned char nxtout;             /* next character out */
    char opt1, opt2, opt3, opt4;      /* various internal options */
    char opt5, opt6, opt7, opt8;
#ifdef PPP
    unsigned short locopts, remopts;
    char *userid, *passwd;      /* User ID and password for PPP */
    unsigned char *script, *sp; /* Script and script pointer */
    unsigned short st;          /* State of script */
    unsigned long mnum, idle_time, echo_time;
  } hw;
  struct FIFOQ16 future; /* queued messages from dial on demand */
  Iid ohaddr, oraddr;    /* Old Host/Remote IP for PPP */
  Iid haddr, raddr;      /* New Host/Remote IP for PPP */
#else
  } hw;
#endif
};

struct NETCONF {         /* network configuration table - RAM */
  char name[9];          /* host name */
  char pname[9];         /* port name */
  Iid Imask;             /* address mask, 0 = host part */
  Iid Iaddr;             /* internal (Internet) address */
  struct Eid Eaddr;      /* external (Ethernet) address */
  char flags;            /* configuration flags */
  char ncstat;           /* block status */
  char cqnext, cqprev;   /* next, previous in cache queue */
  unsigned char netno;   /* network number */
  unsigned char hops;    /* number of needed hops */
  unsigned char nexthix; /* next host index */
  unsigned char gnid;    /* for routing logic */
  char hwaddyes;         /* flag for hardware address present */
#ifdef ARP
  char ARPwait;    /* flag for message waiting for ARP */
  MESS *ARPwaitmp; /* 1 message waiting for ARP */
#endif
  unsigned long timems; /* time stamp */
};

struct CONNECT {  /* logical connection structure */
  char blockstat; /* connection block status: */
  /* 0 = free */
  /* 1 = allocated */
  /* 2 = close initiated */
  /* 4 = close in final timeout */
  char txstat;            /* main level status flags, see below */
  char rxstat;            /* nettask level status flags. see below */
  char state;             /* state machine state */
  struct CONNECT *next;   /* link for incoming connection queue */
  int backlog;            /* maximum size of backlog */
  int icqcur;             /* num in icq, or idx of listening socket */
  unsigned char sendack;  /* flag to send ACK */
  unsigned char netno;    /* net number */
  unsigned char confix;   /* target configuration index */
  unsigned char routeidx; /* route table index */
  unsigned char arpidx;   /* arp table index */
  char tcpflags;          /* TCP read places flags here */
  unsigned long rxtout;   /* timeout value for receive */
  unsigned long txtout;   /* transmission timeout */
  long txvar, txave;      /* values needed to calculate txtout */
  unsigned long txseq;    /* my sequence number */
  unsigned long rxseq;    /* the other sequence number */
  unsigned long ackno;    /* acknowledged up to this */
  unsigned long seqtoack; /* next we'll ACK this number */
  unsigned long ackdseq;  /* last ACK'd number */
  unsigned long acktime;  /* time from which ACK delayed */
  MESS *first, *last;     /* linked list of arrived messages */
  MESS *wackf, *wackl;    /* linked list of messages waiting for ACK */
  unsigned char ninque;   /* number of messages in input queue */
  unsigned char nwacks;   /* number of entries in the WACK queue */
  unsigned char wackmax;  /* maximum WACK length */
  unsigned char wackslow; /* WACK length where we slow down */
  MESS *future;           /* linked sorted list of future messages */
  PTABLE *protoc[5];      /* protocol path */
  int myport;             /* my port number, network order */
  int herport;            /* the other port number, network order */
  Iid heriid;             /* requested remote Internet address */
  Iid realiid;            /* received remote Internet address */
#ifdef MIB2
  Iid offerediid; /* offered local Internet address */
#endif
  unsigned int window;      /* how much we can send */
  unsigned int mywindow;    /* size of our window */
  unsigned int prevwindow;  /* last advertized window */
  unsigned int maxdat;      /* maximum data size for application */
  unsigned int doffset;     /* user data offset in buffer */
  unsigned long lastrxtime; /* time of last RX */
  char urgflag;             /* urgent flag: 1 = data present */
  char urgdata;             /* urgent data */
  unsigned short pseudosum; /* fixed part of pseudo-header checksum */
  unsigned short frid;      /* fragment id for the IP header */
#ifdef IPOPTIONS
  char IPOtxlen;     /* IP options for TX: length in bytes */
  char IPOrxlen;     /* IP options for RX: length in bytes */
  char IPOtxopt[40]; /* IP level options for write */
  char IPOrxopt[40]; /* IP level options for read */
#endif
#if USS_IP_MC_LEVEL > 0
  unsigned char mcnetno; /* mcast network + 1, 0 means deflt */
  unsigned char mcttl;   /* mcast ttl, 0 means default */
#endif
  int urgcnt;           /* amount of urgent data */
  unsigned long urgseq; /* starting point of urgent data */
  MESS *ostreamb;       /* output stream I/O buffer base */
};
#define S_FIN 0x01   /* main level txstat: write end of file */
#define S_MON 0x02   /* monitor mode */
#define S_KEEPA 0x04 /* keepalive mode */
#define S_PSH 0x08   /* push */
#define S_NOCON 0x10 /* no connection, for UDP */
#define S_URG 0x20   /* urgent, out of band */
#define S_STRM 0x40  /* stream */
#define S_NOWA 0x80  /* non-blocking */

#define S_EOF 0x01   /* rx level stat: nothing more to read */
#define S_NOACK 0x02 /* we failed to get an ack */
#define S_UNREA 0x04 /* destination unreachable ICMP */
#define S_TIMEX 0x08 /* time exceeded ICMP */
#define S_QUENC 0x10 /* source quench ICMP */
#define S_FATAL 0x20 /* fatal error */
#define S_RST 0x40   /* restart message received */

/* =========================================================================
   Macros to simplify coding. */

/* Check/take a packet from network (physical connection) netno.
   These would be redifined in an environment that does not use USNET drivers.
*/
#define CHECK_DRIVER_PACKET(netno) QUEUE_EMPTY(netp, arrive)
#define TAKE_DRIVER_PACKET(netno, mp) QUEUE_OUT(netp, arrive, mp)

#define NLONG char
#define NC2(val) (((val & 0xff) << 8) | ((unsigned short)val >> 8))
#define GETLONG(w, net) \
  w.c[0] = net[3], w.c[1] = net[2], w.c[2] = net[1], w.c[3] = net[0]
#define PUTLONG(w, net) \
  net[0] = w.c[3], net[1] = w.c[2], net[2] = w.c[1], net[3] = w.c[0]

#ifdef __cplusplus
#define GLOBALCONST extern
#else
#define GLOBALCONST
#endif

/* Turn on and off the non-blocking mode.  This mode is not a good idea in
   multi-tasking applications. */
#define SOCKET_CLEAR(s) connblo[s].rxstat = 0
#define SOCKET_NOBLOCK(s) connblo[s].txstat |= S_NOWA, connblo[s].rxtout = 0
#define SOCKET_BLOCK(s) \
  connblo[s].txstat &= ~S_NOWA, connblo[s].rxtout = TOUT_READ
#define SOCKET_IPADDR(s) connblo[s].realiid
#define SOCKET_OWNIPADDR(s) netconf[nets[connblo[s].netno].confix].Iaddr
#define SOCKET_PUSH(s) connblo[s].txstat |= S_PSH
#define SOCKET_FIN(s) connblo[s].txstat |= S_FIN
#define SOCKET_TESTFIN(s) (connblo[s].rxstat & S_EOF)
#define SOCKET_ISOPEN(s) (connblo[s].state == 1)
#define SOCKET_HASDATA(s) (connblo[s].first != 0)
#define SOCKET_CANSEND(s, len) socket_cansend(s, len)
int socket_cansend(int, unsigned int);
#define SOCKET_MAXDAT(s) connblo[s].maxdat
/* Set read timeout to milliseconds. */
#define SOCKET_RXTOUT(s, val) connblo[s].rxtout = val

/* =========================================================================
   Function prototypes for support functions. */

/*[91/04/16]
#ifndef Ndisable
void            Ndisable(void);
#endif
#ifndef Nenable
void            Nenable(void);
#endif
#ifndef Nputchr
void            Nputchr(char);
#endif
*/
MESS *reaDD(int conno);
unsigned short Nchksum(unsigned short *sp, int cnt);
unsigned short Nportno(void);
unsigned long Nclock(void);
int Nchkchr(void);
int Ngetchr(void);
int Ninitsupp(void);
unsigned long TimeMS(void);
void SetTimeMS(unsigned long);
int Nsprintf(char *buffer, char *format, ...);
int Nsscanf(char *buffer, char *format, ...);
void Npanic(char *);
void ConfLock(void);
void ConfFree(void);
int ConfFind(int startix, struct NETCONF *argp);
int ConfDel(int argix);
int ConfAdd(struct NETCONF *argp);
int ConfRename(int argix, Iid iaddr);
void ConfDisplay(void);
int RARPget(int netno);
int Ndial(int netno, char *phonenumber);
#ifdef ICMP
void ICMPreply(MESS *mess, int type, int scode);
#else
#define ICMPreply(mess, type, scode)
#endif

int DHCPget(int netno, unsigned long lease);
int DHCPrelease(int netno);

#ifdef DNS
int DNSresolve(char *fullname, IPaddr *iidp);
#endif
#ifdef PPP
int MPjoin(int netno, int oldnetno);
int MPleave(int netno);
#endif
void MD5digest(unsigned char *data, int len, unsigned char *digest);

/* =========================================================================
   Public variables to be used in applications. */

#ifndef NETMODULE
extern char localhostname[];
extern struct CONNECT connblo[];
extern struct NET nets[];
extern struct NETCONF netconf[];
#endif

/* =========================================================================
   Optional SNMP MIB II definitions. */
struct ifgroup {
  int ifNumber;
};
#ifdef MIB2
struct sysgroup {
  const char *sysDescr;
  char *sysContact;
  char *sysName;
  char *sysLocation;
};
struct IPgroup {
  int ipForwarding;
  int ipDefaultTTL;
  unsigned long ipInReceives;
  unsigned long ipInHdrErrors;
  unsigned long ipInAddrErrors;
  unsigned long ipForwDatagrams;
  unsigned long ipInUnknownProtos;
  unsigned long ipInDiscards;
  unsigned long ipInDelivers;
  unsigned long ipOutRequests;
  unsigned long ipOutDiscards;
  unsigned long ipOutNoRoutes;
  unsigned long ipRoutingDiscards;
  int ipReasmTimeout;
  unsigned long ipReasmReqds;
  unsigned long ipReasmOKs;
  unsigned long ipReasmFails;
  unsigned long ipFragOKs;
  unsigned long ipFragFails;
  unsigned long ipFragCreates;
};
struct ICMPgroup {
  unsigned long icmpInMsgs;
  unsigned long icmpInErrors;
  unsigned long icmprx[19];
  unsigned long icmpOutMsgs;
  unsigned long icmpOutErrors;
  unsigned long icmptx[19];
};
struct TCPgroup {
  int tcpRtoMax;
  unsigned long tcpActiveOpens;
  unsigned long tcpPassiveOpens;
  unsigned long tcpAttemptFails;
  unsigned long tcpEstabResets;
  unsigned long tcpCurrEstab;
  unsigned long tcpInSegs;
  unsigned long tcpOutSegs;
  unsigned long tcpRetransSegs;
  unsigned long tcpInErrs;
  unsigned long tcpOutRsts;
};
struct UDPgroup {
  unsigned long udpInDatagrams;
  unsigned long udpNoPorts;
  unsigned long udpInErrors;
  unsigned long udpOutDatagrams;
};
#endif

/******************************/

#define MT 0 /* 0 = no multitasking */
             /* 1 = co-operative multitasking */
             /* 2 = pre-emptive multitasking */

#define BLOCKPREE()
#define RESUMEPREE()

#define YIELD()                                                 \
  {                                                             \
    int wnetno;                                                 \
    for (wnetno = 0; wnetno < NNETS; wnetno++) NetTask(wnetno); \
  }

#define SERV_PRIOR 100
#define CLIENT_PRIOR 100
#define NET_PRIOR 110

#define TASKFUNCTION void
#define RUNTASK(func, prior) 0, func() /* create and run a task */

#define EVBASE 0 /* don't used first N */

#define WAITNOMORE(signo)    /* signal */
#define WAITNOMORE_IR(signo) /* signal in interrupt */

/* wait with timeout */
#define WAITFOR(condition, signo, msec, flag)         \
  {                                                   \
    unsigned long wwul1;                              \
    for (flag = 0, wwul1 = TimeMS(); !(condition);) { \
      YIELD();                                        \
      if (TimeMS() - wwul1 >= msec) {                 \
        flag = ETIMEDOUT;                             \
        break;                                        \
      }                                               \
    }                                                 \
  }

/*** icmp ********************************/

struct ICMPhdr {                              /* ICMP header format */
  char type;                                  /* message type */
  char scode;                                 /* status code */
  unsigned short chksum;                      /* checksum */
  unsigned NLONG param[6][4 / sizeof(NLONG)]; /* parameters */
};
#define ICMPhdr_SZ 8 /* size of header */
#define IC_EREP 0    /* echo reply */
#define IC_UNRE 3    /* destination unreachable */
#define IC_QUEN 4    /* source quench */
#define IC_REDI 5    /* redirect */
#define IC_ECHO 8    /* echo request */
#define IC_TIMEX 11  /* time exceeded */
#define IC_PARAM 12  /* paramameter unintelligible */
#define IC_TIMEQ 13  /* time query */
#define IC_TIMER 14  /* time reply */
#define IC_INREQ 15  /* information request */
#define IC_INREP 16  /* information reply */

/******************************************************/
#ifndef LHOST_H
#define LHOST_H

#ifndef FAILURE
#define FAILURE -1
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

struct ip {
  struct {
    unsigned char c[Iid_SZ];
  } addr, mask;
};

int lhip(char *, struct ip *);
void GetSavedIp(struct ip *);
/*
  GetSavedIp() will read the saved IP address & mask from EEPROM.
*/
#endif /* LHOST_H */

/***************** [08/09/2002]*******************/
extern int bAcceptBroadcast;
/*
  bAcceptBroadcast=1 --> accept the UDP broadcast packet
  bAcceptBroadcast=0 --> reject the UDP broadcast packet
*/

/***************** [09/24/2002]*******************/
unsigned GetTcpipLibVer(void);
/*
  return the version number of tcpipL.lib
  high byte is the major version number
  low byte is the minor version number
  for example: version 1.02 will return 0x0102;
*/

/***************** [10/02/2002]*******************/
void GetTcpipLibDate(char *date);
/*
  return the library release date(in string mode).
*/
extern long
    MINTXTOUT; /* For test, about TCP wait ACK timeout & retransmission*/

/* [2004/08/13] Add */
#define MAX_DNS_NUMBER 2
extern char DnsIp[MAX_DNS_NUMBER][16];
extern int DnsIpNo;
void ResetDomainNameServer(void);
int AddDomainNameServer(char *ip);
/*
 ip must use "xxx.xxx.xxx.xxx"
*/

int GetMaxSocketNo(void);
/*
  for 48 sockets version lib will return 48,
  for 32 sockets version lib will return 32,
  for 16 sockets version lib will return 16,
  for  8 sockets version lib will return  8.
*/
extern long DhcpLeaseTime; /*[2004/11/10] add*/
extern int bUseDhcp;       /*[2004/11/10] add*/
void Install_DHCP(void);
/*
  [2004/12/10] add
  If the program will use the DHCP client function, call the function
  "Install_DHCP()". By default if the program without call "DHCPget()", the code
  for DHCP will not be linked into the program.
*/

/* [2006/08/31] add for Link status & Link speed */
extern unsigned bEthernetLinkOk;
extern int Ethernet_LinkSpeed; /* 10 or 100 */

/* [2006/08/31] add for IP filter */
void IpFilter_ClearAll(void);
int IpFilter_Add(unsigned char *ip1, unsigned char *ip2);
int IpFilter_Delete(unsigned char *ip1, unsigned char *ip2);
int IpFilter_DeleteByIdx(int idx);

typedef struct {
  int type;
  unsigned long ip1, ip2;
} IP_FILTER;

int IpFilter_GetFilterNumber(void);
/*
  return the defined IP filter number.
*/
int IpFilter_GetFilter(int no, IP_FILTER *ipf);
/*
  on success return 0, and the filter value is assigned to the buffer pointed by
  ipf. on fail return -1.
*/

/*
[2007/07/18] Add function for UDP port filter.(only for UDP broadcast packet)
can set at max. 10 UDP ports to be acceptted.
*/
int AddUdpPortFilter(unsigned port);
/*
return value:
0: success.
1: the port has already in the filter list.
-1:the filter list is full.
*/

int DeleteUdpPortFilter(unsigned port);
/*
return value:
0: success.
-1: the port is not in the filter list.
*/

#ifdef __cplusplus
}
#endif

#endif
/************************************************/
