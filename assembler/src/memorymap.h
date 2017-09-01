#ifndef MEMORY_MAP
#define MEMORY_MAP

#define CONSTANT_BANK (0x01)

#define TRASH   (0x0000)
#define PCLO    (0x0001)
#define PCHI    (0x0002)
#define PCTEMP  (0x0003)
#define PTRALO	(0x0009)
#define PTRAHI  (0x000A)

#define DIP	    (0x0010)
#define LED	    (0x0011)
#define UDAT		(0x0012)
#define UTXBSY	(0x0013)
//#define UTXRDY	(0x0014)
//#define URXEMP	(0x0015)
#define URXRDY	(0x0016)

#define PTRDAT	(0x0020)

#define AEB	    (0x003B)
#define NAEB		(0x003C)
#define CARRY		(0x003D)
#define NCARRY	(0x003E)
#define ALUA		(0x003F)

#define APLUS1	(0x0040)
#define MINUS1	(0x0043)
#define SUB	    (0x0046)
#define GTA	    (0x0046)
#define SHL1		(0x004C)

#define LTA	    (0x0056)
#define EQUAL		(0x0056)
#define ADD	    (0x0059)
#define SHL0		(0x005C)
#define AMINUS1	(0x005F)

#define NOTA		(0x0060)
#define NORA		(0x0061)
#define ZERO		(0x0063)
#define NANDA		(0x0064)
#define NOTB		(0x0065)
#define XORA		(0x0066)
#define XNORA		(0x0069)
#define STORE		(0x006A)
#define ANDA		(0x006B)
#define ONE	    (0x006C)
#define ORA	    (0x006E)
#define COPYA		(0x006F)

#endif
