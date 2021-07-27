#ifndef TYPE_H
#define TYPE_H

#ifndef null
#define null		((void*)0)
#endif

#ifndef NULL
#define NULL		((void*)0)
#endif

#ifndef false
#define false		(0)
#endif

#ifndef FALSE
#define FALSE		(0)
#endif

#ifndef true
#define true		(1)	
#endif

#ifndef TRUE
#define TRUE		(1)	
#endif

#ifndef INVALID
#define INVALID		0
#endif

/*bool*/
typedef unsigned char       BOOL;
typedef unsigned char       bool;

/*Data*/
typedef unsigned char       		BYTE;
typedef unsigned short      		WORD;
typedef unsigned int        		DWORD;

typedef unsigned char       		byte;
typedef unsigned short      		word;
typedef unsigned int        		dword;

/*Unsigned numbers*/
typedef unsigned char       		UINT8;
typedef unsigned short      		UINT16;
typedef unsigned int        		UINT32;
typedef unsigned long long	UINT64;			

typedef unsigned char       		uint8;
typedef unsigned short      		uint16;
typedef unsigned int        		uint32;
typedef unsigned long long	uint64;

/*Signed numbers*/
typedef signed char         		INT8;
typedef signed short        		INT16;
typedef signed int          			INT32;
typedef signed long long		INT64;

typedef signed char         		int8;
typedef signed short        		int16;
typedef signed int          			int32;
typedef signed long long 		int64;

/* FLOAT */
typedef float          				FP32;                     
typedef double         				FP64;                    

/* NET */
typedef int SOCKET;

/* FILE */
typedef int FILE_DESC;

typedef union
{
	float fbuf;
	uint8 ubuf[4];
}HEXTOFLOAT;

#define  AHM_BCDTOHEX(b)  ((((b) & 0xF0) >> 4) * 10 + ((b) & 0x0F))

#endif 
