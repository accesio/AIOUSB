#ifndef _DIO_BUF_H
#define _DIO_BUF_H

#include "AIOTypes.h"
#include "AIOChannelMask.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef __aiousb_cplusplus
namespace AIOUSB {
#endif

/**
 * @brief DIOBuf: A Smart structure for maintaining bit vectors and
 * for providing human-readable functionality to make it easy to
 * operate on said bit vectors. The functionality provided by this
 * structure makes it easy for a user to work with a binary
 * string data type convert it between raw bytes and hexidecimal
 * representations as well as use it for generating 
 * digital intput, output and tristate bits with the corresponding
 * [ACCES I/O Products](http://accesio.com/) USB Digital input and output boards.
 * 
 * There are methods to work with DIOBuf that will convert this
 * structure to a character string of 1's and 0's, to a hexadecimal
 * representation and to raw bytes that can be used in the
 * transmission across a number of media. This later functionality
 * would be useful in case you are working with a network server that
 * would need to write an incoming byte stream to a digital buffer.
 *
 * @todo Provide Binary operators such as AND, OR, And Not between two
 * different DIOBuf's
 */

typedef struct {
    unsigned size;              /**< Size of the buffer */
    unsigned char *buffer;      /**< Raw buffer data  */
    char *strbuf;               /**< String representation in terms of 1's and 0's */
    int strbuf_size;            /**< Strlen of the 1's and 0's version including some padding
                                   room */
} DIOBuf;


typedef unsigned char DIOBufferType ;

/* BEGIN AIOUSB_API */

PUBLIC_EXTERN DIOBuf *NewDIOBuf ( unsigned size );
PUBLIC_EXTERN DIOBuf *NewDIOBufFromChar( const char *ary , int size_array );
PUBLIC_EXTERN DIOBuf *NewDIOBufFromBinStr( const char *ary );
PUBLIC_EXTERN void DeleteDIOBuf ( DIOBuf  *buf );
PUBLIC_EXTERN DIOBuf *DIOBufReplaceString( DIOBuf *buf, char *ary, int size_array );
PUBLIC_EXTERN DIOBuf *DIOBufReplaceBinString( DIOBuf *buf, char *bitstr );
PUBLIC_EXTERN char *DIOBufToHex( DIOBuf *buf );
PUBLIC_EXTERN char *DIOBufToBinary( DIOBuf *buf );
PUBLIC_EXTERN char *DIOBufToInvertedBinary( DIOBuf *buf );
PUBLIC_EXTERN DIOBuf  *DIOBufResize( DIOBuf  *buf , unsigned size );
PUBLIC_EXTERN unsigned DIOBufSize( DIOBuf  *buf );
PUBLIC_EXTERN unsigned DIOBufByteSize( DIOBuf *buf );
PUBLIC_EXTERN char *DIOBufToString( DIOBuf  *buf );
PUBLIC_EXTERN AIORET_TYPE DIOBufSetIndex( DIOBuf *buf, int index, unsigned value );
PUBLIC_EXTERN AIORET_TYPE DIOBufGetIndex( DIOBuf *buf, int index );
PUBLIC_EXTERN AIORET_TYPE DIOBufGetByteAtIndex( DIOBuf *buf, unsigned index, char *value);
PUBLIC_EXTERN AIORET_TYPE DIOBufSetByteAtIndex( DIOBuf *buf, unsigned index, char  value );

/* END AIOUSB_API */

#ifdef __aiousb_cplusplus
}
#endif

#endif


