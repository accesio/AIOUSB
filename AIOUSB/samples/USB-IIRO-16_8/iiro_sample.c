/**
 * @file   iiro_sample.c
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * 
 * @page usb_iiro_sample iiro_sample.c
 *
 * @par iiro_sample
 *
 * iiro_sample.c is simple program that demonstrates using
 * the IIRO relay based USB product line.
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
#include "aiocommon.h"

#define RATE_LIMIT(product) \
    do {                                                                \
        if( product >= USB_IIRO_16 && product <= USB_IIRO_4 )           \
            sleep(1);                                                   \
    } while( 0 );                                                       \

AIOUSB_BOOL find_idio( AIOUSBDevice *dev ) { 
    if( (dev->ProductID >= USB_IIRO_16 && dev->ProductID <= USB_IIRO_4 ) ||
        (dev->ProductID >= USB_IDIO_16 && dev->ProductID <= USB_IDIO_4 )) {
        /* found a USB-IIRO or USB-IDIO family device
         * as this sample supports both
         */
        return AIOUSB_TRUE;
    } else {
        return AIOUSB_FALSE;
    }
}

int main(int argc, char *argv[] )
{
    AIODeviceQuery *devq;
    int *indices, num_devices = 0, stopval;
    int timeout = 1000;
    int productId;
    struct opts options = AIO_OPTIONS;
    AIORET_TYPE retval;
    unsigned outData = 15;

    retval = AIOUSB_Init(); /**< Call AIOUSB_Init() first */
    AIOUSB_ListDevices();   /**< Quickly list USB devices on the bus */

    AIOUSB_FindDevices( &indices, &num_devices, find_idio );
    if ( num_devices <= 0 ) {
        fprintf(stderr,"Unable to find a USB-IDIO device\n");
        exit(1);
    }

    if ( ( retval = aio_supply_default_command_line_settings(&options)) != AIOUSB_SUCCESS )
        exit(retval);

    devq = NewAIODeviceQuery( options.index );
    if (!devq ) {
        fprintf(stderr,"Unable to query device at index %d \n", options.index );
        exit(1);
    }

    if ( AIODeviceQueryGetProductID( devq ) == USB_IDIO_16 || AIODeviceQueryGetProductID(devq) == USB_IIRO_16  ) { 
        stopval = 16;
    } else { 
        stopval = 8;
    }

    productId = AIODeviceQueryGetProductID( devq );

    AIOUSB_Reset( options.index );
    AIOUSB_SetCommTimeout( options.index, timeout );

    DIO_WriteAll( options.index, &outData );


    if( ( productId >= USB_IIRO_16 && productId <= USB_IIRO_4 )) {
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
                DIO_WriteAll( options.index, &outData );
                RATE_LIMIT( productId );
                tot ++;
            }
  
            for ( outData = 0; outData < stopval; outData ++ , tot ++ ) {
                unsigned output = 1 << outData;
                DIO_WriteAll( options.index, &output );
                RATE_LIMIT( productId );
                tot++;
            }
            gettimeofday( &now, NULL ) ;
            printf("%d: num=%d delta=%ld\n", i, tot, (now.tv_usec - start.tv_usec ) + (now.tv_sec - start.tv_sec)*1000000 );
        }
    }
 
 walkingbit_test:
   outData = 0x0; 
   DIO_WriteAll(options.index, &outData );
   unsigned output = 0;
   for ( outData = 0; outData < stopval; outData ++ ) {
       output |= (unsigned)(1 << outData);
       DIO_WriteAll( options.index, &output );
       RATE_LIMIT( productId );
   }

   DIOBuf *buf= NewDIOBuf(0);
   unsigned char cdat;

   DIO_Read8( options.index, 0, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( options.index, 1, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( options.index, 2, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( options.index, 3, &cdat   );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );

   outData = 43690;
   DIO_WriteAll(options.index, &outData );

   DIO_Read8( options.index, 0, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( options.index, 1, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( options.index, 2, &cdat  );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );
   DIO_Read8( options.index, 3, &cdat   );
   printf("Single data was : hex:%x, int:%d\n", (int)cdat, (int)cdat );

   DIO_ReadAllToDIOBuf( options.index, buf );
   printf("Binary was: %s\n", DIOBufToString( buf ) );
   printf("Hex was: %s\n", DIOBufToHex( buf ) );


   unsigned char val=0;
   for ( int i = 7 ; i >= 0 ; i-- ) {
       DIO_Read1(options.index,i, &val);
       printf("%d", val );
   }
   printf("\n-----\n");
   for ( int i = 15 ; i >= 8 ; i -- ) {
       DIO_Read1(options.index,i, &val);
       printf("%d", val );
   }
   printf("\n");
   AIOUSB_Exit();
   DeleteDIOBuf( buf );
   return (int)retval;
}

