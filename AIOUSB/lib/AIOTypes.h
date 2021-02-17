/**
 * @file   AIOTypes.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief
 *
 */

#ifndef _AIOTYPES_H
#define _AIOTYPES_H
#define HAS_PTHREAD 1
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>


typedef int64_t AIORET_TYPE;        /* New return type is signed, negative indicates error */
typedef unsigned long AIORESULT;

typedef unsigned short * COUNTS;
typedef struct ushort_array {
    unsigned short *ary;
    unsigned size;
} Ushort_Array;

typedef  enum {
    AIO_PER_OVERSAMPLE = 1,
    AIO_PER_CHANNEL,
    AIO_PER_SCANS
} AIO_SCAN_TYPE;


#ifndef PUBLIC_EXTERN
#define PUBLIC_EXTERN extern
#endif


#ifndef EXPORTED_FUNCTION
#define EXPORTED_FUNCTION
#endif


#define CREATE_ENUM(name, ...) typedef enum { name ## _begin, __VA_ARGS__, name ## _end } name;
#define CREATE_ENUM_W_START(name,num, ...) typedef enum { name ## _begin = (num-1), __VA_ARGS__, name ## _end } name;
#define LAST_ENUM(name ) (name ## _end-1 )
#define FIRST_ENUM(name ) (name ## _begin+1)
#define MIN_VALUE(name ) (name ## _begin+1)
#define MAX_VALUE(name ) (name ## _end-1)
#define VALID_ENUM(name,value ) ( value >= FIRST_ENUM(name) && value <= LAST_ENUM(name ))
#define ERR_UNLESS_VALID_ENUM(name,value) assert(( value >= FIRST_ENUM(name) && value <= LAST_ENUM(name )))

#define VALID_PRODUCT(product) ( VALID_ENUM(  ProductIDS, product ) )


#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100     \
                     + __GNUC_PATCHLEVEL__)
#ifdef SWIG
#define ACCES_DEPRECATED(FOO) ()
#else

#if GCC_VERSION > 40500
#define ACCES_DEPRECATED(FOO)   __attribute__ ((deprecated(FOO)))
#else
#define ACCES_DEPRECATED(FOO)   __attribute__ ((deprecated))
#endif

#endif

#ifdef __aiousb_cplusplus
namespace AIOUSB {
#endif

#ifndef __cplusplus
#define LAMBDA(return_type, header, function_body)                      \
    ({                                                                  \
        return_type __fn ## __FILE__ ## __LINE__ header function_body   \
            __fn ## __FILE__ ## __LINE__ ;                              \
    })
#else
#define LAMBDA(return_type, header, function_body)      \
    [] header -> return_type function_body
#endif


CREATE_ENUM_W_START( THREAD_STATUS , -1 ,
                     INVALID_OBJECT = -2,
                     NOT_STARTED = 0,
                     RUNNING = 1,
                     WITH_DATA = 2,
                     TERMINATED = 4,
                     RUNNING_OR_WITH_DATA = RUNNING | WITH_DATA,
                     JOINED = 8,
                     TERMINATED_OVERRUN = 16,
                     TERMINATING = 32
                     );

CREATE_ENUM_W_START( AIOContinuousBufMode, 0 ,
                     AIOCONTINUOUS_BUF_ALLORNONE,
                     AIOCONTINUOUS_BUF_NORMAL,
                     AIOCONTINUOUS_BUF_OVERRIDE
                     );

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define AUR_CBUF_SETUP  0x01000007
#define AUR_CBUF_EXIT   0x00020002

#define NUMBER_CHANNELS 16

/**< Simple macro for iterating over objects */
#define foreach_array( i , ary, size )  i = ary[0]; \
                                        for ( int j = 0; j < size ; j ++, i = ary[i] )

typedef uint16_t AIOBufferType;

enum {
    MAX_USB_DEVICES               = 32
};

#define AIO_MAKE_ERROR(N) -1*labs(N)
#define AIOUSB_ERROR_VALUE(N) -1*labs(N)

#define AIO_ASSERT(...) assert( __VA_ARGS__ ); if (!( __VA_ARGS__) ) { errno = -AIOUSB_ERROR_INVALID_PARAMETER; return -AIOUSB_ERROR_INVALID_PARAMETER; }
#define AIO_ASSERT_RET(ret,...) assert( __VA_ARGS__ ); if (!( __VA_ARGS__) ) { return ret; }
#define AIO_ASSERT_AIORET_TYPE(ret,...) assert( __VA_ARGS__ ); if (!( __VA_ARGS__) ) { errno = -labs(ret); return -labs(ret); }
#define AIO_ASSERT_NO_RETURN(...) assert( __VA_ARGS__ ); if (!( __VA_ARGS__) ) { return; }
#define AIO_ASSERT_EXIT(...) assert( __VA_ARGS__ ); if (!( __VA_ARGS__) ) { exit(-AIOUSB_ERROR_INVALID_PARAMETER); }

#define AIO_ASSERT_ERR_NO_RETURN(err,...) assert( __VA_ARGS__ ); if (!( __VA_ARGS__) ) { exit(-err); }
#define AIO_ASSERT_VALID_DATA(err,... ) assert( __VA_ARGS__ ) ; if (!( __VA_ARGS__) ) { return err; }
#define AIO_ASSERT_USB(...) AIO_ASSERT_VALID_DATA(-AIOUSB_ERROR_INVALID_USBDEVICE, __VA_ARGS__ )
#define AIO_ASSERT_DIOBUF(...) AIO_ASSERT_VALID_DATA(-AIOUSB_ERROR_INVALID_DIOBUF, __VA_ARGS__ )
#define AIO_ASSERT_CHANNELMASK(...) AIO_ASSERT_VALID_DATA(-AIOUSB_ERROR_INVALID_AIOCHANNELMASK, __VA_ARGS__ )

#define AIO_ASSERT_CONFIG(...) AIO_ASSERT_VALID_DATA(-AIOUSB_ERROR_INVALID_ADCCONFIG , __VA_ARGS__ )
#define AIO_ASSERT_AIOCONTBUF(...) AIO_ASSERT_VALID_DATA(-AIOUSB_ERROR_INVALID_AIOCONTINUOUS_BUFFER, __VA_ARGS__ );
#define AIO_ASSERT_AIOEITHER(err,msg,...) assert( __VA_ARGS__ ); if ( !(__VA_ARGS__) ) { AIOEither tmp; tmp.left = err; tmp.errmsg=strdup(msg); return tmp; }

/**
 * @brief AIO_ERROR * are just like the regular ASSERTIONS meaning that they
 * argument should evaluate to true otherwise it will fail.
 *
 */

#define AIO_ERROR(X) ( -labs(X) )
#define AIO_ERROR_VALID_DATA(err,...) if ( !(__VA_ARGS__) ) { return err; }
#define AIO_ERROR_VALID_DATA_RETVAL( err, ... ) if ( !(__VA_ARGS__) ) { return -labs(err); }
#define AIO_ERROR_VALID_AIORET_TYPE( err, ... ) if ( !(__VA_ARGS__) ) { return -labs(err); }
#define AIO_ERROR_AIOEITHER_VALID_DATA(err,...) if ( !(__VA_ARGS__) ) { \
        AIOEither tmp; tmp.left = err; tmp.errmsg=NULL; return tmp; }
#define AIO_ERROR_VALID_DATA_W_CODE(err, code, ... ) if ( !(__VA_ARGS__) ) { { code; }; return err; }
#define AIO_ERROR_VALID_DATA_WITH_CODE(retval, err, code ) if ( ! (code) ) { errno = -labs(err); return retval; }


#if !(defined (G_STMT_START) && defined (G_STMT_END))
#define G_STMT_START  do
#if defined (_MSC_VER) && (_MSC_VER >= 1500)
#define G_STMT_END \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    while(0) \
    __pragma(warning(pop))
#else
#define G_STMT_END    while (0)
#endif
#endif



#if (defined (__i386__) || defined (__x86_64__)) && defined (__GNUC__) && __GNUC__ >= 2
#  define G_BREAKPOINT()        G_STMT_START{ __asm__ __volatile__ ("int $03"); }G_STMT_END
#elif (defined (_MSC_VER) || defined (__DMC__)) && defined (_M_IX86)
#  define G_BREAKPOINT()        G_STMT_START{ __asm int 3h }G_STMT_END
#elif defined (_MSC_VER)
#  define G_BREAKPOINT()        G_STMT_START{ __debugbreak(); }G_STMT_END
#elif defined (__alpha__) && !defined(__osf__) && defined (__GNUC__) && __GNUC__ >= 2
#  define G_BREAKPOINT()        G_STMT_START{ __asm__ __volatile__ ("bpt"); }G_STMT_END
#else   /* !__i386__ && !__alpha__ */
#  define G_BREAKPOINT()        G_STMT_START{ raise (SIGTRAP); }G_STMT_END
#endif  /* __i386__ */



/**
 * @brief other libraries often declare BOOL, TRUE and FALSE, and worse, they declare these
 * using \#define; so we sidestep that potential conflict by declaring the same types
 * prefixed with AIOUSB_; it's ugly, but if people want to use the shorter names and
 * they are certain they won't conflict with anything else, they can define the
 * ENABLE_BOOL_TYPE macro
 */
#ifdef __cplusplus
typedef bool AIOUSB_BOOL;
const bool AIOUSB_FALSE = false;
const bool AIOUSB_TRUE = true;
#else

enum AIOUSB_BOOL_VAL {
    AIOUSB_FALSE                        = 0,
    AIOUSB_TRUE                         = 1
};

typedef enum AIOUSB_BOOL_VAL AIOUSB_BOOL;



#endif

typedef long double AIO_NUMBER;




#define EXIT_FN_IF_NO_VALID_USB( d, r, f, u, g ) do {           \
        if ( !d ) {                                             \
            r = -AIOUSB_ERROR_DEVICE_NOT_FOUND;                 \
            goto g;                                             \
        } else if ( ( r = f ) != AIOUSB_SUCCESS  ) {            \
            goto g;                                             \
        } else if ( !(u = AIOUSBDeviceGetUSBHandle( d )))  {    \
            r = -AIOUSB_ERROR_INVALID_USBDEVICE;                \
            goto g;                                             \
        }                                                       \
    } while (0 )


CREATE_ENUM_W_START(USB_SPEED,0,
                    usUnknown,
                    usLowFull,
                    usHigh);


CREATE_ENUM_W_START(ProductIDS,0,
                     ACCES_VENDOR_ID    = 0x1605,
                     /**
                      * these product IDs are constant
                      *
                      */
                     USB_DA12_8A_REV_A  = 0xC001,
                     USB_DA12_8A        = 0xC002,
                     USB_DA12_8E        = 0xC003,
                     /**
                      * these are the product IDs after firmware is uploaded to the device; prior
                      * to uploading the firmware, the product ID is that shown below minus 0x8000
                      */
                     USB_DIO_32         = 0x8001,
                     USB_DIO_32I        = 0x8004,
                     USB_DIO_24         = 0x8005,
                     USB_DIO_48         = 0x8002,
                     USB_DIO_96         = 0x8003,
                     USB_DIO24_CTR6     = 0x8006,
                     USB_DI16A_REV_A1   = 0x8008,
                     USB_DO16A_REV_A1   = 0x8009,
                     USB_DI16A_REV_A2   = 0x800a,
                     USB_DIO_16H        = 0x800c,
                     USB_DI16A          = 0x800d,
                     USB_DO16A          = 0x800e,
                     USB_DIO_16A        = 0x800f,

                     USB_IIRO_16        = 0x8010,
                     USB_II_16          = 0x8011,
                     USB_RO_16          = 0x8012,
                     USB_IIRO_8         = 0x8014,
                     USB_II_8           = 0x8015,
                     USB_IIRO_4         = 0x8016,
                     USB_IDIO_16        = 0x8018,
                     USB_II_16_OLD      = 0x8019,
                     USB_IDO_16         = 0x801a,
                     USB_IDIO_8         = 0x801c,
                     USB_II_8_OLD       = 0x801d,
                     USB_IDIO_4         = 0x801e,
                     USB_CTR_15         = 0x8020,
                     USB_IIRO4_2SM      = 0x8030,
                     USB_IIRO4_COM      = 0x8031,
                     USB_DIO16RO8       = 0x8032,
                     USB_DIO48DO24      = 0x803C,
                     USB_DIO24DO12      = 0x803D,
                     USB_DO24           = 0x803E,
                     PICO_DIO16RO8      = 0x8033,
                     USBP_II8IDO4A      = 0x8036,
                     USB_AI16_16A       = 0x8040,
                     USB_AI16_16E       = 0x8041,
                     USB_AI12_16A       = 0x8042,
                     USB_AI12_16        = 0x8043,
                     USB_AI12_16E       = 0x8044,
                     USB_AI16_64MA      = 0x8045,
                     USB_AI16_64ME      = 0x8046,
                     USB_AI12_64MA      = 0x8047,
                     USB_AI12_64M       = 0x8048,
                     USB_AI12_64ME      = 0x8049,
                     USB_AI16_32A       = 0x804a,
                     USB_AI16_32E       = 0x804b,
                     USB_AI12_32A       = 0x804c,
                     USB_AI12_32        = 0x804d,
                     USB_AI12_32E       = 0x804e,
                     USB_AI16_64A       = 0x804f,
                     USB_AI16_64E       = 0x8050,
                     USB_AI12_64A       = 0x8051,
                     USB_AI12_64        = 0x8052,
                     USB_AI12_64E       = 0x8053,
                     USB_AI16_96A       = 0x8054,
                     USB_AI16_96E       = 0x8055,
                     USB_AI12_96A       = 0x8056,
                     USB_AI12_96        = 0x8057,
                     USB_AI12_96E       = 0x8058,
                     USB_AI16_128A      = 0x8059,
                     USB_AI16_128E      = 0x805a,
                     USB_AI12_128A      = 0x805b,
                     USB_AI12_128       = 0x805c,
                     USB_AI12_128E      = 0x805d,
                     USB_AI16_16F	    = 0x805f,
                     USB_AO_ARB1        = 0x8068,
                     USB_AO16_16A       = 0x8070,
                     USB_AO16_16        = 0x8071,
                     USB_AO16_12A       = 0x8072,
                     USB_AO16_12        = 0x8073,
                     USB_AO16_8A        = 0x8074,
                     USB_AO16_8         = 0x8075,
                     USB_AO16_4A        = 0x8076,
                     USB_AO16_4         = 0x8077,
                     USB_AO12_16A       = 0x8078,
                     USB_AO12_16        = 0x8079,
                     USB_AO12_12A       = 0x807a,
                     USB_AO12_12        = 0x807b,
                     USB_AO12_8A        = 0x807c,
                     USB_AO12_8         = 0x807d,
                     USB_AO12_4A        = 0x807e,
                     USB_AO12_4         = 0x807f,
                     USB_AIO16_16A      = 0x8140,
                     USB_AIO16_16E      = 0x8141,
                     USB_AIO12_16A      = 0x8142,
                     USB_AIO12_16       = 0x8143,
                     USB_AIO12_16E      = 0x8144,
                     USB_AIO16_64MA     = 0x8145,
                     USB_AIO16_64ME     = 0x8146,
                     USB_AIO12_64MA     = 0x8147,
                     USB_AIO12_64M      = 0x8148,
                     USB_AIO12_64ME     = 0x8149,
                     USB_AIO16_32A      = 0x814a,
                     USB_AIO16_32E      = 0x814b,
                     USB_AIO12_32A      = 0x814c,
                     USB_AIO12_32       = 0x814d,
                     USB_AIO12_32E      = 0x814e,
                     USB_AIO16_64A      = 0x814f,
                     USB_AIO16_64E      = 0x8150,
                     USB_AIO12_64A      = 0x8151,
                     USB_AIO12_64       = 0x8152,
                     USB_AIO12_64E      = 0x8153,
                     USB_AIO16_96A      = 0x8154,
                     USB_AIO16_96E      = 0x8155,
                     USB_AIO12_96A      = 0x8156,
                     USB_AIO12_96       = 0x8157,
                     USB_AIO12_96E      = 0x8158,
                     USB_AIO16_128A     = 0x8159,
                     USB_AIO16_128E     = 0x815a,
                     USB_AIO12_128A     = 0x815b,
                     USB_AIO12_128      = 0x815c,
                     USB_AIO12_128E     = 0x815d,
                     USB_AIO16_16F      = 0x815f
                     ); // keep sorted by productID




/*
 * generic device indexes
 */
enum {
    /* passed to ResolveDeviceIndex() */
    diFirst                       = 0xFFFFFFFEul,      /* -2 */
    diOnly                        = 0xFFFFFFFDul,      /* -3 */

    /* returned by ResolveDeviceIndex() */
    diNone                        = 0xFFFFFFFFul       /* -1 */
};

/**
 * @brief range codes passed to DACSetBoardRange()
 */
CREATE_ENUM_W_START(DACRange,0,
                    DAC_RANGE_0_5V,
                    DAC_RANGE_5V,
                    DAC_RANGE_0_10V,
                    DAC_RANGE_10V
                    );



/**
 * @brief FIFO clearing methods passed to AIOUSB_ClearFIFO()
 */
CREATE_ENUM_W_START(FIFO_Method,0,
                    CLEAR_FIFO_METHOD_IMMEDIATE,
                    CLEAR_FIFO_METHOD_AUTO,
                    CLEAR_FIFO_METHOD_IMMEDIATE_AND_ABORT = 5,
                    CLEAR_FIFO_METHOD_NOW = 0x35,
                    CLEAR_FIFO_METHOD_WAIT = 86
                    )



/**
 * @brief
 * The AIOUSB function result codes are a bit confusing; the result codes used in the Windows
 * implementation of the API are defined in a system file, winerror.h; these result codes
 * are generic and can apply to many applications; the very first result code, ERROR_SUCCESS,
 * sounds like an oxymoron; the result codes used in libusb, on the other hand, are a lot
 * more appealing; the result code for success is LIBUSB_SUCCESS; the result codes for errors
 * are LIBUSB_ERROR_xxx; further complicating matters is that the AIOUSB result codes must be
 * non-negative since all the functions return an unsigned result, whereas the LIBUSB result
 * codes are negative in the case of errors; both schemes use zero to denote success; it would
 * also be nice to return the original libusb result code in cases where a libusb error causes
 * an AIOUSB API function to fail; so to satisfy all these requirements, we've employed the
 * following scheme:
 *
 * - AIOUSB result codes in Linux start with "AIOUSB_"; the result code for success is
 *   AIOUSB_SUCCESS, which has a value of zero; the result codes for errors are AIOUSB_ERROR_xxx,
 *   which have positive values, starting with one (1)
 *
 * - in order to offer users the option of using result codes whose names are similar to those
 *   cited in the AIOUSB API specification, we define a second set of result codes with names
 *   similar to those in API specification but which map to the same values as the AIOUSB_xxx
 *   result codes; these alternate result code names can be enabled by defining the macro
 *   ENABLE_WINDOWS_RESULT_CODES, which is not enabled by default
 *
 * - in order to preserve the original libusb result codes and pass them back from an AIOUSB API
 *   function, we translate the libusb result codes to a format that conforms to the one
 *   AIOUSB employs and provide macros for converting the AIOUSB result code back to a libusb
 *   result code; LIBUSB_RESULT_TO_AIOUSB_RESULT() converts a libusb result code to an AIOUSB
 *   result code; AIOUSB_RESULT_TO_LIBUSB_RESULT() does the reverse; these macros cannot be
 *   used with LIBUSB_SUCCESS
 *
 * - we provide an extended AIOUSB API function named AIOUSB_GetResultCodeAsString() that returns a
 *   string representation of the result code, including those derived from a libusb result code
 */
CREATE_ENUM_W_START( ResultCode, 0,
                     AIOUSB_SUCCESS,
                     AIOUSB_ERROR_DEVICE_NOT_CONNECTED,
                     AIOUSB_ERROR_INTERNAL_ERROR,
                     AIOUSB_ERROR_DUP_NAME,
                     AIOUSB_ERROR_NOT_INIT,
                     AIOUSB_ERROR_FILE_NOT_FOUND,
                     AIOUSB_ERROR_INVALID_DATA,
                     AIOUSB_ERROR_INVALID_INDEX,
                     AIOUSB_ERROR_INVALID_MUTEX,
                     AIOUSB_ERROR_INVALID_PARAMETER,
                     AIOUSB_ERROR_INVALID_THREAD,
                     AIOUSB_ERROR_INVALID_ADDRESS,
                     AIOUSB_ERROR_NOT_ENOUGH_MEMORY,
                     AIOUSB_ERROR_INVALID_MEMORY,
                     AIOUSB_ERROR_NOT_SUPPORTED,
                     AIOUSB_ERROR_OPEN_FAILED,
                     AIOUSB_ERROR_BAD_TOKEN_TYPE,
                     AIOUSB_ERROR_TIMEOUT,
                     AIOUSB_ERROR_DIVIDE_BY_ZERO,
                     AIOUSB_ERROR_HANDLE_EOF,
                     AIOUSB_ERROR_DEVICE_NOT_FOUND,
                     AIOUSB_ERROR_USBDEVICE_NOT_FOUND,
                     AIOUSB_ERROR_USB_INIT,
                     AIOUSB_ERROR_INVALID_TIMEOUT,
                     AIOUSB_ERROR_INVALID_AIOEITHER_ALLOCATION,
                     AIOUSB_ERROR_INVALID_USBDEVICE,
                     AIOUSB_ERROR_INVALID_VOLTAGES,
                     AIOUSB_ERROR_INVALID_AIOCMD,
                     AIOUSB_ERROR_INVALID_CALLBACK,
                     AIOUSB_ERROR_INVALID_COUNTS,
                     AIOUSB_ERROR_INVALID_COUNTS_CONVERTER,
                     AIOUSB_ERROR_INVALID_DEVICE,
                     AIOUSB_ERROR_INVALID_DEVICE_SETTING,
                     AIOUSB_ERROR_INVALID_DEVICE_FUNCTIONAL_PARAMETER,
                     AIOUSB_ERROR_INVALID_DEVICE_STREAM_SETTING,
                     AIOUSB_ERROR_INVALID_DEVICE_CHANNEL_SETTING,
                     AIOUSB_ERROR_INVALID_DEVICE_MUX_CHANNEL_SETTING,
                     AIOUSB_ERROR_INVALID_CHANNELS_PER_GROUP_SETTING,
                     AIOUSB_ERROR_INVALID_AIOCHANNELMASK,
                     AIOUSB_ERROR_INVALID_CONFIG,
                     AIOUSB_ERROR_INVALID_DIOBUF,
                     AIOUSB_ERROR_INVALID_GAINCODE,
                     AIOUSB_ERROR_INVALID_CALMODE,
                     AIOUSB_ERROR_INVALID_CHANNEL_NUMBER,
                     AIOUSB_ERROR_INVALID_AIOCONFIGURATION,
                     AIOUSB_ERROR_INVALID_AIOARGUMENT,
                     AIOUSB_ERROR_INVALID_AIODEVICE_QUERY,
                     AIOUSB_ERROR_INVALID_AIOEITHER,
                     AIOUSB_ERROR_INVALID_AIOFIFO,
                     AIOUSB_ERROR_INVALID_ADCCONFIG,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_SIZE,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_TRIGGER_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_CAL_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_CHANNEL_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_OVERSAMPLE_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_REGISTER_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_MUX_SETTING,
                     AIOUSB_ERROR_INVALID_ADCCONFIG_DEVICE,
                     AIOUSB_ERROR_INVALID_AIOCONTINUOUS_BUFFER,
                     AIOUSB_ERROR_INVALID_AIOCONTINUOUS_BUFFER_NUM_CHANNELS,
                     AIOUSB_ERROR_INVALID_AIOBUFTYPE,
                     AIOUSB_ERROR_AIOCOMMANDLINE_INVALID_CHANNEL_RANGE,
                     AIOUSB_ERROR_AIOCOMMANDLINE_INVALID_NUM_CHANNELS,
                     AIOUSB_ERROR_AIOCOMMANDLINE_INVALID_INDEX_NUM,
                     AIOUSB_ERROR_AIOCOMMANDLINE_INVALID_START_END_CHANNEL,
                     AIOUSB_ERROR_AIOCOMMANDLINE_HELP,
                     AIOUSB_ERROR_INVALID_LIBUSB_DEVICE_HANDLE,
                     AIOUSB_FIFO_COPY_ERROR,
                     AIOUSB_ERROR_LIBUSB /* Always make the LIBUSB the last element */
                     );

enum {
  AD_MAX_CHANNELS    = 128,    /* maximum number of channels supported by this driver */
  AD_GAIN_CODE_MASK  = 7
};

#define AIOUSB_ERROR_OFFSET 100


#define LIBUSB_RESULT_TO_AIOUSB_RESULT( code )  ( unsigned long )( AIOUSB_ERROR_OFFSET + -( int )( code ) )
#define AIOUSB_RESULT_TO_LIBUSB_RESULT( code )  ( -( ( int )( code ) - AIOUSB_ERROR_OFFSET ) )


#if defined( ENABLE_WINDOWS_RESULT_CODES )
typedef enum {
    ERROR_SUCCESS = AIOUSB_SUCCESS,
    ERROR_DEVICE_NOT_CONNECTED = AIOUSB_ERROR_DEVICE_NOT_CONNECTED,
    ERROR_DUP_NAME = AIOUSB_ERROR_DUP_NAME,
    ERROR_FILE_NOT_FOUND = AIOUSB_ERROR_FILE_NOT_FOUND,
    ERROR_INVALID_DATA = AIOUSB_ERROR_INVALID_DATA,
    ERROR_INVALID_INDEX = AIOUSB_ERROR_INVALID_INDEX,
    ERROR_INVALID_MUTEX = AIOUSB_ERROR_INVALID_MUTEX,
    ERROR_INVALID_PARAMETER = AIOUSB_ERROR_INVALID_PARAMETER,
    ERROR_INVALID_THREAD = AIOUSB_ERROR_INVALID_THREAD,
    ERROR_NOT_ENOUGH_MEMORY = AIOUSB_ERROR_NOT_ENOUGH_MEMORY,
    ERROR_NOT_SUPPORTED = AIOUSB_ERROR_NOT_SUPPORTED,
    ERROR_OPEN_FAILED = AIOUSB_ERROR_OPEN_FAILED,
    ERROR_TIMEOUT = AIOUSB_ERROR_TIMEOUT
}  WindowsResultCode;
#endif


CREATE_ENUM_W_START( ADRegister, 16,
                     AD_REGISTER_CAL_MODE,
                     AD_REGISTER_TRIG_COUNT,
                     AD_REGISTER_START_END,
                     AD_REGISTER_OVERSAMPLE,
                     AD_REGISTER_MUX_START_END
                     );


/*
 * general constants
 */
enum {
    AD_MAX_CONFIG_REGISTERS             = 21, /* maximum number of "registers" (bytes) in A/D config. block */
    AD_MIN_CONFIG_REGISTERS             = 20,
    AD_MAX_TIMEOUT                      = 8000,
    AD_MIN_TIMEOUT                      = 500,

    AD_NUM_GAIN_CODE_REGISTERS          = 16, /* number of gain code registers in A/D config. block */

    /* A/D configuration block indexes */
    AD_CONFIG_GAIN_CODE                 = 0,  /* gain code for A/D channel 0 (one of GAIN_CODE_* settings below) */
                                              /* gain codes for channels 1-15 occupy configuration block indexes 1-15 */
    AD_REGISTER_GAIN_CODE               = 0,

    /*Codes defined in the USB Software Reference Manual */
    AD_CONFIG_CAL_MODE                  = 0x10, /* calibration mode (one of AD_CAL_MODE_* settings below) */
    AD_CONFIG_TRIG_COUNT                = 0x11, /* trigger and counter clock (one of AD_TRIG_* settings below) */
    AD_CONFIG_START_END                 = 0x12, /* start and end channels for scan (bits 7-4 contain end channel, bits 3-0 contain start channel) */
    AD_CONFIG_OVERSAMPLE                = 0x13, /* oversample setting (0-255 samples in addition to single sample) */
    AD_CONFIG_MUX_START_END             = 0x14, /* MUX start and end channels for scan (bits 7-4 contain end channel MS-nibble, bits 3-0 contain start channel MS-nibble) */
    AD_CONFIG_START_STOP_CHANNEL_EX     = 21,

    /* A/D gain codes */
    AD_NUM_GAIN_CODES                   = 8,
    AD_DIFFERENTIAL_MODE                = 8, /* OR with gain code to enable differential mode for channels 0-7 */

    /* A/D trigger and counter bits */
    AD_TRIGGER_CTR0_EXT                 = 0x10, /* 1==counter 0 externally triggered */
    AD_TRIGGER_FALLING_EDGE             = 0x08, /* 1==triggered by falling edge */
    AD_TRIGGER_SCAN                     = 0x04, /* 1==scan all channels, 0==single channel */

    /* mutually exclusive for now */
    AD_TRIGGER_EXTERNAL                 = 0x02,              /* 1==triggered by external pin on board */
    AD_TRIGGER_TIMER                    = 0x01,              /* 1==triggered by counter 2 */

    AD_TRIGGER_VALID_MASK               = ( AD_TRIGGER_CTR0_EXT
                                            | AD_TRIGGER_FALLING_EDGE
                                            | AD_TRIGGER_SCAN
                                            | AD_TRIGGER_EXTERNAL
                                            | AD_TRIGGER_TIMER )
};

/*
 * A/D gain codes
 */
CREATE_ENUM_W_START( ADGainCode, 0,
                     AD_GAIN_CODE_0_10V,    /* 0-10V */
                     AD_GAIN_CODE_10V,      /* +-10V */
                     AD_GAIN_CODE_0_5V,     /* 0-5V */
                     AD_GAIN_CODE_5V,       /* +-5V */
                     AD_GAIN_CODE_0_2V,     /* 0-2V */
                     AD_GAIN_CODE_2V,       /* +-2V */
                     AD_GAIN_CODE_0_1V,     /* 0-1V */
                     AD_GAIN_CODE_1V        /* +-1V */
                     );



/* partially defined in "Low-Level Vendor Request Reference.pdf" */

 CREATE_ENUM_W_START(VENDOR_REQUEST,0,
                     AUR_DIO_WRITE                           = 0x10,
                     AUR_DIO_READ                            = 0x11,
                     AUR_DIO_CONFIG                          = 0x12,
                     AUR_DIO_CONFIG_QUERY                    = 0x13,
                     AUR_CTR_READ                            = 0x20,
                     AUR_CTR_MODE                            = 0x21,
                     AUR_CTR_LOAD                            = 0x22,
                     AUR_CTR_MODELOAD                        = 0x23,
                     AUR_CTR_SELGATE                         = 0x24,
                     AUR_CTR_READALL                         = 0x25,
                     AUR_CTR_READLATCHED                     = 0x26,
                     AUR_CTR_COS_BULK_GATE2                  = 0x27,
                     AUR_CTR_PUR_FIRST                       = 0x28, // 0x28 is correct; not used with device, for index offsetting
                     AUR_CTR_PUR_OFRQ                        = 0x28, // 0x28 is correct
                     AUR_CTR_COS_BULK_ABORT                  = 0x29,
                     AUR_CTR_PUR_MFRQ                        = 0x2C,
                     AUR_CTR_PUR_EVCT                        = 0x2D,
                     AUR_CTR_PUR_MPUL                        = 0x2E,
                     AUR_WDG_STATUS                          = 0x2E,
                     AUR_DIO_WDG16_DEPREC                    = 0x2F,
                     AUR_READBACK_GLOBAL_STATE               = 0x30,
                     AUR_SAVE_GLOBAL_STATE                   = 0x31,
                     AUR_GEN_CLEAR_FIFO_NEXT                 = 0x34,
                     AUR_GEN_CLEAR_FIFO                      = 0x35,
                     AUR_GEN_CLEAR_FIFO_WAIT                 = 0x36,
                     AUR_GEN_ABORT_AND_CLEAR                 = 0x38,
                     AUR_WDG                                 = 0x44,
                     AUR_OFFLINE_READWRITE                   = 0x50,
                     AUR_SELF_TEST_1                         = 0x91,
                     AUR_EEPROM_READ                         = 0xA2,
                     AUR_EEPROM_WRITE                        = 0XA2,
                     AUR_DAC_CONTROL                         = 0xB0,
                     AUR_DAC_DATAPTR                         = 0xB1,
                     AUR_DAC_DIVISOR                         = 0xB2,
                     AUR_DAC_IMMEDIATE                       = 0xB3,
                     AUR_GEN_STREAM_STATUS                   = 0xB4,
                     AUR_FLASH_READWRITE                     = 0xB5,
                     AUR_DAC_RANGE                           = 0xB7,
                     AUR_PROBE_CALFEATURE                    = 0xBA,


                     AUR_LOAD_BULK_CALIBRATION_BLOCK         = 0xBB,
                     AUR_DIO_STREAM_OPEN_OUTPUT              = 0xBB,
                     AUR_START_ACQUIRING_BLOCK               = 0xBC,
                     AUR_DIO_STREAM_OPEN_INPUT               = 0xBC,

                     AUR_DIO_SETCLOCKS                       = 0xBD,
                     AUR_ADC_SET_CONFIG                      = 0xBE,
                     AUR_ADC_IMMEDIATE                       = 0xBF,
                     AUR_DIO_SPI_WRITE                       = 0xC0,
                     AUR_DIO_SPI_READ                        = 0xC1,
                     AUR_SET_CUSTOM_CLOCKS                   = 0xC4,
                     AUR_ADC_GET_CONFIG                      = 0xD2,
                     AUR_DEBUG_DEBUG                         = 0xD4,
                     AUR_DEBUG_FLASH_1TO1                    = 0xF0,
                     AUR_DEBUG_FLASH_ERASE                   = 0xFC,

                     CUR_RAM_READ                            = 0xA3,
                     CYPRESS_GET_DESC                        = 0x06
                     );




#undef BITS_PER_BYTE
enum {
    BITS_PER_BYTE                 = 8,
    AI_16_MAX_COUNTS              = 65535,
    MAX_IMM_ADCS                  = 2, /*< maximum number of "immediate" A/Ds in any device */
    CAL_TABLE_WORDS               = ( 64 * 1024 *2), /* 64K 2-byte words, twice: unipolar then bipolar */
    COUNTERS_PER_BLOCK            = 3,
    COUNTER_NUM_MODES             = 6,
    DAC_RESET                     = 0x80,
    CYPRESS_DESC_PARAMS           = 0x0302,  /* 03 = decscriptor type, 02 = index */
    CYPRESS_MAX_DESC_SIZE         = 256,
    AIOUSB_MAX_NAME_SIZE          = 100,

    EEPROM_SERIAL_NUMBER_ADDRESS  = 0x1DF8,
    EEPROM_CUSTOM_BASE_ADDRESS    = 0x1E00,
    EEPROM_CUSTOM_MIN_ADDRESS     = 0,
    EEPROM_CUSTOM_MAX_ADDRESS     = 0x1FF,

    AD_CONFIG_REGISTERS           = 20,             // number of "registers" (bytes) in A/D config. block of boards without MUX
    AD_MUX_CONFIG_REGISTERS       = 21,             // number of "registers" (bytes) in A/D config. block of boards with MUX

    USB_WRITE_TO_DEVICE           = 0x40,
    USB_READ_FROM_DEVICE          = 0xC0,
    USB_BULK_WRITE_ENDPOINT       = 2,
    USB_BULK_READ_ENDPOINT        = 6
};

#define BYTE_INDEX( bitnumber ) ( bitnumber / BITS_PER_BYTE )


/*
 * A/D calibration modes; if ground or reference mode is selected, only one A/D
 * sample may be taken at a time; that means, one channel and no oversampling
 * attempting to read more than one channel or use an oversample setting of more
 * than zero will result in a timeout error
 */
typedef enum  {
  ADCalMode_begin         = -1,
  AD_CAL_MODE_NORMAL      = 0, /* normal measurement */
  AD_CAL_MODE_GROUND      = 1, /* measure ground */
  AD_CAL_MODE_REFERENCE   = 3, /* measure reference */
  AD_CAL_MODE_BIP_GROUND  = 5,
  AD_CAL_MODE_HIGH_REF    = 7,
  ADCalMode_end           = 8
} ADCalMode;


/* For JSON parsing */
typedef struct lookup {
    int value;
    char *str;
    char *strvalue;
} EnumStringLookup;


#define ROOTCLOCK 10000000

/**
 * @brief Allows us to keep track of streaming (bulk) acquires
 * without making the user keep track of the memory management
 */

typedef struct  {
    char *Name;                      /**< null-terminated device name or 0 */
    uint64_t SerialNumber;           /**< 64-bit serial number or 0 */
    unsigned ProductID;              /**< 16-bit product ID */
    unsigned DIOPorts;               /**< number of digital I/O ports (bytes) */
    unsigned Counters;               /**< number of 8254 counter blocks */
    unsigned Tristates;              /**< number of tristates */
    long RootClock;                  /**< base clock frequency */
    unsigned DACChannels;            /**< number of D/A channels */
    unsigned ADCChannels;            /**< number of A/D channels */
    unsigned ADCMUXChannels;         /**< number of MUXed A/D channels */
    unsigned ADCChannelsPerGroup;    /**< number of A/D channels in each config. group */
} DeviceProperties;

/**
 * @brief Enums that govern how commands are performed and
 * operated
 */
CREATE_ENUM_W_START( AIOCommandCode, 0 ,
                     GENERIC_DOSOMETHING_PLACEHOLDER,
                     AIO_CONTINUE_RUNNING,
                     AIO_TERMINATE_CALLBACK
                     );



#ifdef __aiousb_cplusplus
}
#endif

#endif
