#ifndef MEMORY_MAP
#define MEMORY_MAP

#define TRASH   (0x00)
#define PCLO    (0x01)
#define PCHI    (0x02)
#define PCTEMP  (0x03)
#define DRAMLO  (0x04)
#define DRAMHI  (0x05)
#define LOAD    (0x07)
#define PTRDAT	(0x09)
#define SRC	    (0x0A)
#define DST	    (0x0B)

#define DIP	    (0x10)
#define LED	    (0x11)
#define UDAT	(0x12)
#define UTXBSY	(0x13)
#define UTXRDY	(0x14)
#define URXEMP	(0x15)
#define URXRDY	(0x16)
	
#define PTRADR	(0x20)
	
#define AEB	    (0x3B)
#define NAEB	(0x3C)
#define CARRY	(0x3D)
#define NCARRY	(0x3E)
#define ALUA	(0x3F)
	
#define APLUS1	(0x40)
#define MINUS1	(0x43)
#define SUB	    (0x46)
#define GTA	    (0x46)
#define SHL1	(0x4C)

#define LTA	    (0x56)
#define EQUAL	(0x56)
#define ADD	    (0x59)
#define SHL0	(0x5C)
#define AMINUS1	(0x5F)
	
#define NOTA	(0x60)
#define NORA	(0x61)
#define ZERO	(0x63)
#define NANDA	(0x64)
#define NOTB	(0x65)
#define XORA	(0x66)
#define XNORA	(0x69)
#define STORE	(0x6A)
#define ANDA	(0x6B)
#define ONE	    (0x6C)
#define ORA	    (0x6E)
#define COPYA	(0x6F)

#define RAMLO   (0x80)
#define RAMHI   (0xFF)

#endif