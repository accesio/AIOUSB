/**
 * @file   idio_sample2.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * 
 * @page usb_idio_sample2 idio_sample2.c
 *
 * @par Sample2
 *
 * sample2.c is simple program that demonstrates using
 * the AIOUSB C library's Continuous mode acquisition API.
 *
 *
 * @todo Complete this example
 */

#ifndef IGNORE_DOXYGEN
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#endif
#include "aiousb.h"
#define RATE_LIMIT(product) \
    do {                                                                \
        if( product >= USB_IIRO_16 && product <= USB_IIRO_4 )           \
            sleep(1);                                                   \
    } while( 0 );                                                       \

int main(int argc, char *argv[] )
{
    unsigned long productId, nameSize, numDIOBytes, numCounters;
    const int MAX_NAME_SIZE = 20;
    char name[ MAX_NAME_SIZE + 2 ];
    int stopval;

    unsigned long deviceIndex = 0;
    AIOUSB_BOOL deviceFound = AIOUSB_FALSE;
    printf(
           "  AIOUSB version:'%s',date:'%s'\n"
           "  This program demonstrates controlling a USB-IIRO or USB-IDIO device on\n"
           "  the USB bus. For simplicity, it uses the first such device found\n"
           "  on the bus.\n"
           , AIOUSB_GetVersion(), AIOUSB_GetVersionDate()
           );

    /*
     * Must call AIOUSB_Init() before any meaningful AIOUSB functions;
     * AIOUSB_GetVersion() above is an exception
     */
    unsigned long result = AIOUSB_Init();

    /*
     * call GetDevices() to obtain "list" of devices found on the bus
     */
    unsigned long deviceMask = GetDevices();
    if( deviceMask != 0 ) {
        /*
         * at least one ACCES device detected, but we want one of a specific type
         */
        AIOUSB_ListDevices(); /* print list of all devices found on the bus */

        while( deviceMask != 0 ) {
            if( ( deviceMask & 1 ) != 0 ) {
                // found a device, but is it the correct type?
                nameSize = MAX_NAME_SIZE;
                result = QueryDeviceInfo( deviceIndex, &productId, &nameSize, name, &numDIOBytes, &numCounters );
                if( result == AIOUSB_SUCCESS ) {
                    if( (productId >= USB_IIRO_16 && productId <= USB_IIRO_4 ) || 
                        (productId >= USB_IDIO_16 && productId <= USB_IDIO_4 )) {
                        // found a USB-AI16-16A family device
                        deviceFound = AIOUSB_TRUE;
                        break;				// from while()
                    }
                } else
                    printf( "Error '%s' querying device at index %lu\n"
                            , AIOUSB_GetResultCodeAsString( result ), deviceIndex );
            }
            deviceIndex++;
            deviceMask >>= 1;
        }
    }

    if( deviceFound != AIOUSB_TRUE ) { 
        printf("Card with board id '0x%x' is not supported by this sample\n", (int)productId );
        _exit(1);
    }

    if ( productId  == 0x8018 || productId == USB_IIRO_16  ) { 
        stopval = 16;
    } else { 
        stopval = 8;
    }

    int timeout = 1000;
    AIOUSB_Reset( deviceIndex );
    AIOUSB_SetCommTimeout( deviceIndex, timeout );
    unsigned outData = 15;

    DIO_WriteAll( deviceIndex, &outData );


    if( (productId >= USB_IIRO_16 && productId <= USB_IIRO_4 )) {
        goto walkingbit_test;
    }
    /* Speed test for IDIOs not for mechanical switching !! */
    struct timeval start;
    struct timeval now;
    for( int i = 0; i < 50; i ++ ) { 
        int tot = 0;
        gettimeofday( &start, NULL ) ;
        if( 1 ) { 
            for ( outData = 0; outData < stopval ; outData ++  ) {
                DIO_WriteAll( deviceIndex, &outData );
                RATE_LIMIT( productId );
                tot ++;
            }
  
            for ( outData = 0; outData < stopval; outData ++ , tot ++ ) {
                unsigned output = 1 << outData;
                DIO_WriteAll( deviceIndex, &output );
                RATE_LIMIT( productId );
                tot++;
            }
            gettimeofday( &now, NULL ) ;
            printf("%d: num=%d delta=%ld\n", i, tot, (now.tv_usec - start.tv_usec ) + (now.tv_sec - start.tv_sec)*1000000 );
        }
    }
 
 walkingbit_test:
   outData = 0x0; 
   DIO_WriteAll(deviceIndex, &outData );
   unsigned output = 0;
   for ( outData = 0; outData < stopval; outData ++ ) {
       output |= (unsigned)(1 << outData);
       DIO_WriteAll( deviceIndex, &output );
       RATE_LIMIT( productId );
   }

   DIOBuf *buf= NewDIOBuf(0);
   unsigned char cdat;
   DIO_ReadAllToDIOBuf( deviceIndex, buf );
   printf("Binary was: %s\n", DIOBufToString( buf ) );
   printf("Hex was: %s\n", DIOBufToHex( buf ) );
   DIO_Read8( deviceIndex, 0, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( deviceIndex, 1, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( deviceIndex, 2, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( deviceIndex, 3, &cdat   );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );

   unsigned char val=0;
   for ( int i = 7 ; i >= 0 ; i-- ) {
       DIO_Read1(deviceIndex,i, &val);
       printf("%d", val );
   }
   printf("\n-----\n");
   for ( int i = 15 ; i >= 8 ; i -- ) {
       DIO_Read1(deviceIndex,i, &val);
       printf("%d", val );
   }
   printf("\n");
   AIOUSB_Exit();
   DeleteDIOBuf( buf );
}

