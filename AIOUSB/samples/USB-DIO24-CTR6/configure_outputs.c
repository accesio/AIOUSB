/**
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief Sample program showing the fucntionality of the USB-DIO24-CTR6
 * 
 *
 * @see Compilation 
 * @see CmakeCompilation
 */

#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT_C  1<<2
#define PORT_B  1<<1
#define PORT_A  1

#define MAKE_MASK( GROUP, PORT ) ((PORT << (GROUP * 3)))


char *show_byte( unsigned char val )
{
    static char retstring[9];
    for( int i = 7 ; i >= 0 ; i -- ) { 
        retstring[7-i] = ( (1 << i) & val ? '1' : '0' );
    }

    return retstring;
}


AIOUSB_BOOL find_dio24_ctr6( AIOUSBDevice *dev ) 
{ 
    if ( dev->ProductID == USB_DIO24_CTR6 )
        return AIOUSB_TRUE;
    else
        return AIOUSB_FALSE;
}

int main( int argc, char **argv ) {
    int num_devices = 0;
    AIORET_TYPE retval;
    AIOCommandLineOptions *options = NewDefaultAIOCommandLineOptions();
    int *indices = 0;
    int i;
    int outmask = 0;
    int group = 0;
    unsigned char Data[12];
    unsigned char DataRead[12] = {0};
    /**
     * Instead of 
     * AIOUSB_Init();
     * AIOUSB_ListDevices();
     *
     * use AIOUSB_FindDevices(...) 
     */
    AIOUSB_FindDevices( &indices, &num_devices, find_dio24_ctr6 );
    
    if ( num_devices <= 0 ) {
        fprintf(stderr,"Unable to find a USB-DIO24-CTR6 device\n");
        exit(1);
    }

    retval = AIOProcessCmdline( options, argc, argv );
    if ( retval < AIOUSB_SUCCESS ) {
        exit(1);
    }
    AIOCommandLineListDevices( options, indices, num_devices );
    printf("Using device %d\n", indices[0] );


    for ( i = 0; i < 12; i ++ )
        Data[i] = 0xff;

    
    /**
     * @brief Configure Groups 1 and 3 and all ports A-C
     *        for being an output
     *
     **/

    for ( int port = PORT_A; port <= PORT_C ; port <<= 1 ) {
        outmask |= MAKE_MASK( group , port );
    }

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
     * 
     * @endverbatim
     */ 
    outmask = 0xf;              /* All 4 Groups are output */
    memset(&Data, 0 , 3 );
    Data[0]  = (unsigned char)(0xdb);          /**< Data[3] is the start of Group 1, Port A */
    Data[1]  = (unsigned char)(0x55);          /**< Group 0 Port B */
    Data[2]  = (unsigned char)(0x3f);          /**< Group 0 Port C */
    Data[3]  = 0x0;
    DIO_Configure( options->index, 0 , &outmask, Data );

    
    DIO_ReadAll( options->index, DataRead );

    for ( i = 0 ; i < 3 ; i ++ ) { 
        printf("Group %d Port %c:  Value: %s  Hex: %#x\n", 
               i / 3, 
               ( i % 3 == 0 ? 'A' : ( i % 3 == 1 ? 'B' : 'C' )),
               show_byte(DataRead[i]),
               DataRead[i]
               );
    }
    printf("\n");

    AIOUSB_Exit();

}
