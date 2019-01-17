/** 
 *  $Date $Format: %ad$$
 *  $Author $Format: %an <%ae>$$
 *  $Release $Format: %h$$
 *  @brief Sample program to run the USB-IDIO-16 
 */
#include "aiousb.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[] )
{
    unsigned long productId, nameSize, numDIOBytes, numCounters;
    const int MAX_NAME_SIZE = 20;
    char name[ MAX_NAME_SIZE + 2 ];
    int stopval;

    unsigned long deviceIndex = 0;
    AIOUSB_BOOL deviceFound = AIOUSB_FALSE;
    printf(
           "USB-AI16-16A sample program\n"
           "  AIOUSB version:'%s',date:'%s'\n"
           "  This program demonstrates controlling a USB-AI16-16A device on\n"
           "  the USB bus. For simplicity, it uses the first such device found\n"
           "  on the bus.\n"
           , AIOUSB_GetVersion(), AIOUSB_GetVersionDate()
           );

    /*
     * MUST call AIOUSB_Init() before any meaningful AIOUSB functions;
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
      AIOUSB_ListDevices();				/* print list of all devices found on the bus */

      while( deviceMask != 0 ) {
        if( ( deviceMask & 1 ) != 0 ) {
          /* found a device, but is it the correct type? */
          nameSize = MAX_NAME_SIZE;
          result = QueryDeviceInfo( deviceIndex, &productId, &nameSize, name, &numDIOBytes, &numCounters );
          if( result == AIOUSB_SUCCESS ) {
            if( productId >= 32792 || productId == 32796 ) {
              deviceFound = AIOUSB_TRUE;
              break;
            }
          } else
            printf( "Error '%s' querying device at index %lu\n", 
                    AIOUSB_GetResultCodeAsString( result ), deviceIndex );
        }
        deviceIndex++;
        deviceMask >>= 1;
      }
    }

    if( deviceFound != AIOUSB_TRUE ) { 
      printf("Card with board id '0x%x' is not supported by this sample\n", (int)productId );
      _exit(1);
    }

    if ( productId  == 32792 ) { 
      stopval = 16;
    } else { 
      stopval = 8;
    }

    int timeout = 1000;
    AIOUSB_Reset( deviceIndex );
    AIOUSB_SetCommTimeout( deviceIndex, timeout );
    unsigned outData = 15;

    DIO_WriteAll( deviceIndex, &outData );
    /* gettimeofday(struct timeval *tv, struct timezone *tz) */
    struct timeval start;
    struct timeval now;
    int num_per_bin = 10;


    /* for( int i = 0; i < 50; i ++ ) {  */
    int tot = 0;
    gettimeofday( &start, NULL ) ;
      /* printf("%d\n", i ); */
    int endvalue = (int)pow(2,(double)stopval);
    int *vals = (int *)malloc(sizeof(int)*endvalue );
    int count = 0;
    for ( outData = 0; outData < endvalue; outData ++  ) {
      DIO_WriteAll( deviceIndex, &outData );
      if( outData % num_per_bin == 0  ) {
        gettimeofday( &now, NULL );
        vals[count] =  (now.tv_usec - start.tv_usec ) + (now.tv_sec - start.tv_sec)*1000000;
        count ++;
      }
      tot ++;
    }
   
    FILE *fp = fopen("output.txt","w");
    if( !fp ) {
      fprintf(stderr,"Can't open output.txt for writing\n");
    }
    for( outData = 0; outData < count ; outData ++ ) {
      fprintf(fp,"%d\n", vals[outData] );
    }
    gettimeofday( &now, NULL ) ;
    printf("%d points: delta=%ld\n", endvalue, (long)((now.tv_usec - start.tv_usec ) + (now.tv_sec - start.tv_sec)*1000000 ));

    if( fp ) 
      fclose(fp);
    AIOUSB_Exit();
}

