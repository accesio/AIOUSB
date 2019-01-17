/*
 * sample program to write out the calibration table and then 
 * reload it again, verify that the data is in fact reversed
 */

#include <aiousb.h>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <string.h>
#include "aiocommon.h"


AIOUSB_BOOL find_ai_board( AIOUSBDevice *dev ) { 
    if ( (dev->ProductID >= USB_DIO_32 && dev->ProductID <= USB_DIO24DO12 ) || dev->ProductID == USB_AO_ARB1 ) { 
        return AIOUSB_TRUE;
    } else {
        return AIOUSB_FALSE;
    }
}


int main( int argc, char **argv ) 
{
    struct opts options = AIO_OPTIONS;
    AIORET_TYPE result = AIOUSB_SUCCESS;
    int *indices, num_devices;

    options.clock_rate = -1;
    process_aio_cmd_line( &options, argc, argv );

    result = AIOUSB_Init();
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error calling AIOUSB_Init(): %d\n", (int)result );
        exit(result );
    }

    AIOUSB_ListDevices();

    process_aio_cmd_line( &options, argc, argv );

    AIOUSB_FindDevices( &indices, &num_devices, find_ai_board );
    
    if( (result = aio_list_devices( &options, indices, num_devices ) != AIOUSB_SUCCESS )) 
        exit(result);

    int buf[256] = {0};
    int outmask = 0xffff;
    result = DIO_Configure( options.index , AIOUSB_FALSE, &outmask, buf );
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running DIO_Configure: %d\n", (int)result );
        exit(result);
    }
    
    double ReadClockHz = 0;
    double WriteClockHz = ( options.clock_rate == -1 ? 1000000 : options.clock_rate );


    result = DIO_StreamSetClocks( options.index, &ReadClockHz, &WriteClockHz );
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running DIO_StreamSetClocks: %d\n", (int)result );
        exit(result);
    }
    
    result = DACSetBoardRange( options.index, 0 );
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running DACSetBoardRange: %d\n", (int)result );
        exit(result);
    }

    result =  DIO_StreamOpen( options.index, AIOUSB_FALSE );
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running DIO_StreamOpen: %d\n", (int)result );
        exit(result);
    }
    unsigned char OutMask = 0x07; //A, B, and C output; D is opposite C, so it's input.
    unsigned long DIOData = 0xFFFFFFFF;
    unsigned char TristateMask = 0x00;
    DIO_ConfigureEx( options.index, &OutMask, &DIOData, &TristateMask);
 
    unsigned long TransferPackets = 65536;
    uint16_t Content[TransferPackets];

    for ( int i = 65535; i >= 0; i -- ) {
        Content[65535-i] = i;
    }
    
    deviceTable[options.index].StreamingBlockSize = 4096;

    for ( int i = 0; i < options.num_scans; i ++ ) { 

        unsigned long BytesTransferred = 0, WordsLeft = TransferPackets, Status;

        if ( i % options.rate_limit == 0 ) 
            fprintf(stderr,"Running option %d\n", i );
        do {
            Status = DIO_StreamFrame(options.index, WordsLeft, Content, &BytesTransferred);
            if ( Status == AIOUSB_ERROR_HANDLE_EOF ) {
                Status = AIOUSB_SUCCESS;
            }
            if ( Status != AIOUSB_SUCCESS ) break;
            //BytesTransferred will always be integral words.
            unsigned long WordsTransferred = BytesTransferred / 2;
            WordsLeft -= WordsTransferred;
        } while ( WordsLeft > 0 );
    }

    result = DIO_StreamClose(options.index);
    if ( result != AIOUSB_SUCCESS ) {
        fprintf(stderr,"Error running DIO_StreamClose: %d\n", (int)result );
        exit(result);
    }


    return result;
}



