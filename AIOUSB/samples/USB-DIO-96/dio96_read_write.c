/**
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  
 * @see Compilation 
 * @see CmakeCompilation
 */

#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

AIOUSB_BOOL find_dio_96( AIOUSBDevice *dev ) 
{ 
    if ( dev->ProductID == USB_DIO_96 )
        return AIOUSB_TRUE;
    else
        return AIOUSB_FALSE;
}

#undef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#define DEVICES_REQUIRED 1
#define MAX_DIO_BYTES 12
#define MASK_BYTES  (( MAX_DIO_BYTES + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE)
#define MAX_NAME_SIZE 20

#define PORT_C  1<<2
#define PORT_B  1<<1
#define PORT_A  1

#define MAKE_MASK( GROUP, PORT ) ((PORT << (GROUP * 3)))

char *show_byte( unsigned char );

int main( int argc, char **argv ) {
    AIODeviceQuery *devq;
    int *indices, num_devices = 0;
    AIORET_TYPE result;
    int i;
    int mask = 0;
    unsigned char Data[12];
    unsigned char DataRead[12];
    AIOCommandLineOptions options = AIO_DEFAULT_CMDLINE_OPTIONS;

    result = AIOUSB_Init();
    AIOUSB_ListDevices();
    AIOUSB_FindDevices( &indices, &num_devices, find_dio_96 );
    if ( num_devices <= 0 ) {
        fprintf(stderr,"Unable to find a USB-IDIO device\n");
        exit(1);
    }

    result = AIOProcessCmdline( &options, argc, argv );
    if ( result < AIOUSB_SUCCESS ) {
        exit(1);
    }
    
    if ( options.index >= 0 ) { 
        for ( i = 0 ; i < num_devices ; i ++ ) {
            if ( options.index == indices[i] ) 
                goto found_device;
        }
        fprintf(stderr,"Error: Your index of '%d' does not match any of the found USB-DIO-96 devices\n", 
                options.index );
        exit(1);
    } else {
        options.index = indices[0];
    }
 found_device:

    devq = NewAIODeviceQuery( options.index );
    if (!devq ) {
        fprintf(stderr,"Unable to query device at index %d \n", options.index );
        exit(1);
    }

    printf("Using Device '%s' for sample\n", AIODeviceQueryToRepr( devq ) );

    for ( i = 0; i < 12; i ++ )
        Data[i] = 0xff;

    
    /**
     * @brief Configure Groups 1 and 3 and all ports A-C
     *        for being an output
     *
     **/
    for ( int group = 1; group <= 3 ; group += 2 ) { 
        for ( int port = PORT_A; port <= PORT_C ; port <<= 1 ) {
            mask |= MAKE_MASK( group , port );
        }
    }

    DIO_Configure( options.index, 0, &mask, Data );

    /**
     * @brief Due to the nature of DIO_Configure using a Mask
     * as the third argument, this logic is a bit inverted.
     * Using DIO_Configure, you must specify a '1' in the mask
     * where you want a Low voltage to occur, and you must specify
     * a '0' in the mask where you want a High Voltage to occur.
     *
     *
     * @verbatim
     * Write the following port patterns with 1 
     * indicating Off ( or low ) voltage. 
     *
     * DIO_Configure Mask               | Output Signal (1=High,0=Low)
     * ================================================================
     *
     * Group 1 Port A   00100100  corresponds to   11011011 Volts
     *         Port B   10101010        "          01010101 Volts
     *         Port C   11110000        "          00001111 Volts
     *
     * Group 3 Port A   00001111        "          11110000 Volts
     *         Port B   00111100        "          11000011 Volts
     *         Port C   11000011        "          00111100 Volts
     * 
     * @endverbatim
     */ 

    Data[3]  = (unsigned char)(0xdb);          /**< Data[3] is the start of Group 1, Port A */
    Data[4]  = (unsigned char)(0x55);          /**< Group 1 Port B */
    Data[5]  = (unsigned char)(0x0f);          /**< Group 1 Port C */

    Data[9]  = (unsigned char)(0xf0);
    Data[10] = (unsigned char)(0xc3);
    Data[11] = (unsigned char)(0x3c);

    DIO_Configure( options.index, 0 , &mask, Data );

    
    DIO_ReadAll( options.index, DataRead );

    for ( i = 0 ; i < 12 ; i ++ ) { 
        printf("Group %d Port %c:  Value: %s\n", 
               i / 3, 
               ( i % 3 == 0 ? 'A' : ( i % 3 == 1 ? 'B' : 'C' )),
               show_byte(DataRead[i])
               );
    }
    printf("\n");

    AIOUSB_Exit();

    return 0;
} 


char *show_byte( unsigned char val )
{
    static char retstring[9];
    for( int i = 7 ; i >= 0 ; i -- ) { 
        retstring[7-i] = ( (1 << i) & val ? '1' : '0' );
    }

    return retstring;
}
