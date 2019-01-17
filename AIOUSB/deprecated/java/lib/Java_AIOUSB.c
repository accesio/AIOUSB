/*
 * $RCSfile: Java_AIOUSB.c,v $
 * $Revision: 1.27 $
 * $Date: 2010/01/29 00:01:37 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * ACCES I/O USB API for Java
 */


// {{{ build notes
/*
 * Philosophy:
 *   o This module doesn't perform a thorough checking of the parameters passed to it. The reason is
 *     that we perform all such checking in the Java code, where we have much better exception handling,
 *     before calling these functions. These functions assume that the parameters have been thoroughly
 *     vetted by the Java code. Moreover, the underlying AIOUSB code does considerable checking of its own.
 *
 * Compiling:
 *   gcc -Wall -o libjavaaiousb.so -shared -Wl,-soname,libjavaaiousb.so Java_AIOUSB.c -static -laiousb -lusb-1.0 -lc -lrt
 */
// }}}

// {{{ includes
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <aiousb.h>
#include "../../lib/AIOUSB_Core.h"
#include "com_acces_aiousb_USBDeviceManager.h"
#include "com_acces_aiousb_USBDevice.h"
#include "com_acces_aiousb_AnalogInputSubsystem.h"
#include "com_acces_aiousb_AnalogOutputSubsystem.h"
#include "com_acces_aiousb_DigitalIOSubsystem.h"
#include "com_acces_aiousb_DIOStreamSubsystem.h"
#include "com_acces_aiousb_Counter.h"
#include "com_acces_aiousb_CounterSubsystem.h"
// }}}

// {{{ local variables

/*
 * table of buffers allocated for bulk analog input, one per device; we could return the actual pointer
 * to the buffer as a "handle," but that's not as portable across different platforms as using an
 * integer index into this local table; moreover, it's not possible to perform more than one bulk read
 * per device at a time, so managing the pointers in this manner is not a limitation at all; in the
 * interest of portability and safety, it's better to keep pointers confined to their native environment
 * as much as possible; using the device index makes for a convenient means to access the buffers indirectly
 * and establishes a generalized technique for adding properties to the device table
 */

static unsigned short *readBulkBuffers[ MAX_USB_DEVICES ];

// }}}

// {{{ USBDeviceManager.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDeviceManager_init( JNIEnv *env, jobject obj ) {
	assert( env != 0 );
	memset( readBulkBuffers, 0, sizeof( readBulkBuffers ) );
	return AIOUSB_Init();
}	// Java_com_acces_aiousb_USBDeviceManager_init()


JNIEXPORT void JNICALL Java_com_acces_aiousb_USBDeviceManager_exit( JNIEnv *env, jobject obj ) {
	assert( env != 0 );
	/*
	 * hopefully the user will have properly cleaned up before calling this function,
	 * but just in case ...
	 */
	int index;
	for( index = 0; index < MAX_USB_DEVICES; index++ ) {
		if( readBulkBuffers[ index ] != NULL ) {
			free( readBulkBuffers[ index ] );
			readBulkBuffers[ index ] = NULL;
		}	// if( readBulkBuffers[ ...
	}	// for( index ...
	AIOUSB_Exit();
}	// Java_com_acces_aiousb_USBDeviceManager_exit()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDeviceManager_getDeviceByProductID( JNIEnv *env, jobject obj
		, jint minProductID, jint maxProductID, jintArray devices ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	/*
	 * layout of devices[]:
	 *   [ 0 ]   : number of device index-product ID pairs returned in devices[ 1-n ]
	 *   [ 1 ]   : index of first device found
	 *   [ 2 ]   : product ID of first device found
	 *   ...
	 *   [ n-1 ] : index of last device found
	 *   [ n ]   : product ID of last device found
	 * so the size of the array is 1 + numDevices * 2; the maximum capacity is then,
	 * ( array size - 1 ) / 2
	 */
	const int maxDevices = ( ( *env )->GetArrayLength( env, devices ) - 1 ) / 2;
	jint *const nativeDevices = ( *env )->GetIntArrayElements( env, devices, NULL );
	if( nativeDevices != NULL ) {
		result = AIOUSB_GetDeviceByProductID( minProductID, maxProductID, maxDevices, nativeDevices );
		( *env )->ReleaseIntArrayElements( env, devices, nativeDevices, 0 );
	}	// if( nativeDevices ...
	return result;
}	// Java_com_acces_aiousb_USBDeviceManager_getDeviceByProductID()


JNIEXPORT jstring JNICALL Java_com_acces_aiousb_USBDeviceManager_getAIOUSBVersion( JNIEnv *env, jobject obj ) {
	assert( env != 0 );
	return ( *env )->NewStringUTF( env, AIOUSB_GetVersion() );
}	// Java_com_acces_aiousb_USBDeviceManager_getAIOUSBVersion()


JNIEXPORT jstring JNICALL Java_com_acces_aiousb_USBDeviceManager_getAIOUSBVersionDate( JNIEnv *env, jobject obj ) {
	assert( env != 0 );
	return ( *env )->NewStringUTF( env, AIOUSB_GetVersionDate() );
}	// Java_com_acces_aiousb_USBDeviceManager_getAIOUSBVersionDate()


JNIEXPORT jstring JNICALL Java_com_acces_aiousb_USBDeviceManager_libProductIDToName( JNIEnv *env, jclass class, jint productID ) {
	assert( env != 0 );
	return ( *env )->NewStringUTF( env, ProductIDToName( productID ) );
}	// Java_com_acces_aiousb_USBDeviceManager_libProductIDToName()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDeviceManager_libProductNameToID( JNIEnv *env, jclass class, jstring name ) {
	assert( env != 0 );
	jint productID = 0;
	const char *nativeName = ( *env )->GetStringUTFChars( env, name, NULL );
	if( nativeName != NULL ) {
		productID = ProductNameToID( nativeName );
		( *env )->ReleaseStringUTFChars( env, name, nativeName );
	}	// if( nativeName ...
	return productID;
}	// Java_com_acces_aiousb_USBDeviceManager_libProductNameToID()


JNIEXPORT void JNICALL Java_com_acces_aiousb_USBDeviceManager_listDevices( JNIEnv *env, jobject obj ) {
	assert( env != 0 );
	AIOUSB_ListDevices();
}	// Java_com_acces_aiousb_USBDeviceManager_listDevices()


JNIEXPORT jstring JNICALL Java_com_acces_aiousb_USBDeviceManager_getResultCodeAsString( JNIEnv *env, jclass class, jint result ) {
	assert( env != 0 );
	return ( *env )->NewStringUTF( env, AIOUSB_GetResultCodeAsString( result ) );
}	// Java_com_acces_aiousb_USBDeviceManager_getResultCodeAsString()

// }}}

// {{{ USBDevice.java

JNIEXPORT jstring JNICALL Java_com_acces_aiousb_USBDevice_getName( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return ( *env )->NewStringUTF( env, GetSafeDeviceName( deviceIndex ) );
}	// Java_com_acces_aiousb_USBDevice_getName()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_getDeviceSerialNumber( JNIEnv *env, jobject obj
		, jint deviceIndex, jlongArray serialNumbers ) {
	assert( env != 0 );
	__uint64_t serialNumber = 0;
	const unsigned long result = GetDeviceSerialNumber( deviceIndex, &serialNumber );
	if( result == AIOUSB_SUCCESS ) {
		jlong nativeSerialNumber[ 1 ] = { serialNumber };
		( *env )->SetLongArrayRegion( env, serialNumbers, 0, 1, nativeSerialNumber );
	}	// if( result ...
	return result;
}	// Java_com_acces_aiousb_USBDevice_getDeviceSerialNumber()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_getCommTimeout( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return AIOUSB_GetCommTimeout( deviceIndex );
}	// Java_com_acces_aiousb_USBDevice_getCommTimeout()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_setCommTimeout( JNIEnv *env, jobject obj, jint deviceIndex, jint timeout ) {
	assert( env != 0 );
	return AIOUSB_SetCommTimeout( deviceIndex, timeout );
}	// Java_com_acces_aiousb_USBDevice_setCommTimeout()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_reset( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return AIOUSB_Reset( deviceIndex );
}	// Java_com_acces_aiousb_USBDevice_reset()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_customEEPROMWrite( JNIEnv *env, jobject obj
		, jint deviceIndex, jint address, jbyteArray data ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeData = ( *env )->GetByteArrayElements( env, data, NULL );
	if( nativeData != NULL ) {
		result = CustomEEPROMWrite( deviceIndex, address, ( *env )->GetArrayLength( env, data ), nativeData );
		( *env )->ReleaseByteArrayElements( env, data, nativeData, 0 );
	}	// if( nativeData ...
	return result;
}	// Java_com_acces_aiousb_USBDevice_customEEPROMWrite()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_customEEPROMRead( JNIEnv *env, jobject obj
		, jint deviceIndex, jint address, jbyteArray data ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeData = ( *env )->GetByteArrayElements( env, data, NULL );
	if( nativeData != NULL ) {
		unsigned long dataSize = ( *env )->GetArrayLength( env, data );
		result = CustomEEPROMRead( deviceIndex, address, &dataSize, nativeData );
		( *env )->ReleaseByteArrayElements( env, data, nativeData, 0 );
	}	// if( nativeData ...
	return result;
}	// Java_com_acces_aiousb_USBDevice_customEEPROMRead()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_clearFIFO( JNIEnv *env, jobject obj, jint deviceIndex, jint method ) {
	assert( env != 0 );
	return AIOUSB_ClearFIFO( deviceIndex, method );
}	// Java_com_acces_aiousb_USBDevice_clearFIFO()


JNIEXPORT jdouble JNICALL Java_com_acces_aiousb_USBDevice_getMiscClock( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return AIOUSB_GetMiscClock( deviceIndex );
}	// Java_com_acces_aiousb_USBDevice_getMiscClock()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_setMiscClock( JNIEnv *env, jobject obj, jint deviceIndex, jdouble clockHz ) {
	assert( env != 0 );
	return AIOUSB_SetMiscClock( deviceIndex, clockHz );
}	// Java_com_acces_aiousb_USBDevice_setMiscClock()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_getStreamingBlockSize( JNIEnv *env, jobject obj
		, jint deviceIndex, jintArray blockSize ) {
	assert( env != 0 );
	unsigned long size;
	const unsigned long result = AIOUSB_GetStreamingBlockSize( deviceIndex, &size );
	if( result == AIOUSB_SUCCESS ) {
		jint nativeBlockSize[ 1 ] = { size };
		( *env )->SetIntArrayRegion( env, blockSize, 0, 1, nativeBlockSize );
	}	// if( result ...
	return result;
}	// Java_com_acces_aiousb_USBDevice_getStreamingBlockSize()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_USBDevice_setStreamingBlockSize( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockSize ) {
	assert( env != 0 );
	return AIOUSB_SetStreamingBlockSize( deviceIndex, blockSize );
}	// Java_com_acces_aiousb_USBDevice_setStreamingBlockSize()

// }}}

// {{{ AnalogInputSubsystem.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_getConfigSize( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int configSize = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			configSize = deviceTable[ deviceIndex ].ConfigBytes;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return configSize;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_getConfigSize()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_getNumChannels( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int numChannels = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			numChannels = deviceTable[ deviceIndex ].ADCChannels;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return numChannels;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_getNumChannels()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_getNumMUXChannels( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int numChannels = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			numChannels = deviceTable[ deviceIndex ].ADCMUXChannels;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return numChannels;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_getNumMUXChannels()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_getChannelsPerGroup( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int channelsPerGroup = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			channelsPerGroup = deviceTable[ deviceIndex ].ADCChannelsPerGroup;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return channelsPerGroup;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_getChannelsPerGroup()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_queryCal( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return ADC_QueryCal( deviceIndex );
}	// Java_com_acces_aiousb_AnalogInputSubsystem_queryCal()


JNIEXPORT jboolean JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_isDiscardFirstSample( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return AIOUSB_IsDiscardFirstSample( deviceIndex ) != AIOUSB_FALSE;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_isDiscardFirstSample()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_setDiscardFirstSample( JNIEnv *env, jobject obj
		, jint deviceIndex, jboolean discard ) {
	assert( env != 0 );
	return AIOUSB_SetDiscardFirstSample( deviceIndex, discard ? AIOUSB_TRUE : AIOUSB_FALSE );
}	// Java_com_acces_aiousb_AnalogInputSubsystem_setDiscardFirstSample()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_loadCalTable( JNIEnv *env, jobject obj
		, jint deviceIndex, jstring fileName ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	const char *nativeFileName = ( *env )->GetStringUTFChars( env, fileName, NULL );
	if( nativeFileName != NULL ) {
		result = AIOUSB_ADC_LoadCalTable( deviceIndex, nativeFileName );
		( *env )->ReleaseStringUTFChars( env, fileName, nativeFileName );
	}	// if( nativeFileName ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_loadCalTable()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_setCalTable( JNIEnv *env, jobject obj
		, jint deviceIndex, jcharArray calTable ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jchar *const nativeCalTable = ( *env )->GetCharArrayElements( env, calTable, NULL );
	if( nativeCalTable != NULL ) {
		result = AIOUSB_ADC_SetCalTable( deviceIndex, nativeCalTable );
		( *env )->ReleaseCharArrayElements( env, calTable, nativeCalTable, 0 );
	}	// if( nativeCalTable ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_setCalTable()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_getConfig( JNIEnv *env, jobject obj
		, jint deviceIndex, jbyteArray configBlock ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeConfigBlock = ( *env )->GetByteArrayElements( env, configBlock, NULL );
	if( nativeConfigBlock != NULL ) {
		unsigned long size = ( *env )->GetArrayLength( env, configBlock );
		result = ADC_GetConfig( deviceIndex, ( unsigned char * ) nativeConfigBlock, &size );
		( *env )->ReleaseByteArrayElements( env, configBlock, nativeConfigBlock, 0 );
	}	// if( nativeConfigBlock ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_getConfig()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_setConfig( JNIEnv *env, jobject obj
		, jint deviceIndex, jbyteArray configBlock ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeConfigBlock = ( *env )->GetByteArrayElements( env, configBlock, NULL );
	if( nativeConfigBlock != NULL ) {
		unsigned long size = ( *env )->GetArrayLength( env, configBlock );
		result = ADC_SetConfig( deviceIndex, ( unsigned char * ) nativeConfigBlock, &size );
		( *env )->ReleaseByteArrayElements( env, configBlock, nativeConfigBlock, 0 );
	}	// if( nativeConfigBlock ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_setConfig()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_internalCal( JNIEnv *env, jobject obj
		, jint deviceIndex, jboolean autoCal, jcharArray returnCalTable, jstring saveFileName ) {
	assert( env != 0 );

	/*
	 * returnCalTable is optional; only return table if returnCalTable != NULL
	 */
	jchar *nativeCalTable = NULL;
	if(
		returnCalTable != NULL
		&& ( *env )->GetArrayLength( env, returnCalTable ) == CAL_TABLE_WORDS
	)
		nativeCalTable = ( *env )->GetCharArrayElements( env, returnCalTable, NULL );

	/*
	 * saveFileName is optional; only save table if saveFileName != NULL and not empty
	 */
	const char *nativeFileName = NULL;
	if(
		saveFileName != NULL
		&& ( *env )->GetStringLength( env, saveFileName ) > 0
	)
		nativeFileName = ( *env )->GetStringUTFChars( env, saveFileName, NULL );

	const unsigned long result = AIOUSB_ADC_InternalCal( deviceIndex
		, autoCal ? AIOUSB_TRUE : AIOUSB_FALSE, ( unsigned short * ) nativeCalTable, nativeFileName );

	/*
	 * clean up
	 */
	if( nativeFileName != NULL )
		( *env )->ReleaseStringUTFChars( env, saveFileName, nativeFileName );
	if( nativeCalTable != NULL )
		( *env )->ReleaseCharArrayElements( env, returnCalTable, nativeCalTable, 0 );

	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_internalCal()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_externalCal( JNIEnv *env, jobject obj
		, jint deviceIndex, jdoubleArray points, jcharArray returnCalTable, jstring saveFileName ) {
	assert( env != 0 );

	/*
	 * returnCalTable is optional; only return table if returnCalTable != NULL
	 */
	jchar *nativeCalTable = NULL;
	if(
		returnCalTable != NULL
		&& ( *env )->GetArrayLength( env, returnCalTable ) == CAL_TABLE_WORDS
	)
		nativeCalTable = ( *env )->GetCharArrayElements( env, returnCalTable, NULL );

	/*
	 * saveFileName is optional; only save table if saveFileName != NULL and not empty
	 */
	const char *nativeFileName = NULL;
	if(
		saveFileName != NULL
		&& ( *env )->GetStringLength( env, saveFileName ) > 0
	)
		nativeFileName = ( *env )->GetStringUTFChars( env, saveFileName, NULL );

	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jdouble *const nativePoints = ( *env )->GetDoubleArrayElements( env, points, NULL );
	if( nativePoints != NULL ) {
		result = AIOUSB_ADC_ExternalCal( deviceIndex, nativePoints, ( *env )->GetArrayLength( env, points ) / 2
			, ( unsigned short * ) nativeCalTable, nativeFileName );
		( *env )->ReleaseDoubleArrayElements( env, points, nativePoints, 0 );
	}	// if( nativePoints ...

	/*
	 * clean up
	 */
	if( nativeFileName != NULL )
		( *env )->ReleaseStringUTFChars( env, saveFileName, nativeFileName );
	if( nativeCalTable != NULL )
		( *env )->ReleaseCharArrayElements( env, returnCalTable, nativeCalTable, 0 );

	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_externalCal()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_getScan( JNIEnv *env, jobject obj
		, jint deviceIndex, jcharArray counts ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jchar *const nativeCounts = ( *env )->GetCharArrayElements( env, counts, NULL );
	if( nativeCounts != NULL ) {
		result = AIOUSB_GetScan( deviceIndex, nativeCounts );
		( *env )->ReleaseCharArrayElements( env, counts, nativeCounts, 0 );
	}	// if( nativeCounts ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_getScan()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_readBulkStart( JNIEnv *env, jobject obj
		, jint deviceIndex, jint numSamples ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	unsigned long result;
	if( readBulkBuffers[ deviceIndex ] == NULL ) {
		const int numBytes = numSamples * sizeof( unsigned short );
		unsigned short *const counts = calloc( numSamples, sizeof( unsigned short ) );
		if( counts != NULL ) {
			result = ADC_BulkAcquire( deviceIndex, numBytes, counts );
			if( result == AIOUSB_SUCCESS )
				readBulkBuffers[ deviceIndex ] = counts;
			else
				free( counts );
		} else
			result = AIOUSB_ERROR_NOT_ENOUGH_MEMORY;
	} else
		result = AIOUSB_ERROR_OPEN_FAILED;
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_readBulkStart()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_readBulkSamplesRemaining( JNIEnv *env, jobject obj
		, jint deviceIndex, jintArray samplesRemaining ) {
	assert( env != 0 );
	unsigned long bytesRemaining = 0;
	const unsigned long result = ADC_BulkPoll( deviceIndex, &bytesRemaining );
	if( result == AIOUSB_SUCCESS ) {
		jint nativeSamplesRemaining[ 1 ] = { bytesRemaining / sizeof( unsigned short ) };
		( *env )->SetIntArrayRegion( env, samplesRemaining, 0, 1, nativeSamplesRemaining );
	}	// if( result ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_readBulkSamplesRemaining()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_readBulkNext( JNIEnv *env, jobject obj
		, jint deviceIndex, jint offset, jcharArray counts ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	if( readBulkBuffers[ deviceIndex ] != NULL ) {
		jchar *const nativeCounts = ( *env )->GetCharArrayElements( env, counts, NULL );
		if( nativeCounts != NULL ) {
			memcpy( nativeCounts, readBulkBuffers[ deviceIndex ] + offset, ( *env )->GetArrayLength( env, counts ) * sizeof( unsigned short ) );
			( *env )->ReleaseCharArrayElements( env, counts, nativeCounts, 0 );
			result = AIOUSB_SUCCESS;
		}	// if( nativeCounts ...
	}	// if( readBulkBuffers[ ...
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_readBulkNext()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogInputSubsystem_readBulkFinish( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	unsigned long result;
	if( readBulkBuffers[ deviceIndex ] != NULL ) {
		free( readBulkBuffers[ deviceIndex ] );
		readBulkBuffers[ deviceIndex ] = NULL;
		result = AIOUSB_SUCCESS;
	} else
		result = AIOUSB_ERROR_INVALID_DATA;
	return result;
}	// Java_com_acces_aiousb_AnalogInputSubsystem_readBulkFinish()

// }}}

// {{{ AnalogOutputSubsystem.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogOutputSubsystem_getNumChannels( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int numChannels = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			numChannels = deviceTable[ deviceIndex ].ImmDACs;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return numChannels;
}	// Java_com_acces_aiousb_AnalogOutputSubsystem_getNumChannels()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogOutputSubsystem_writeCounts__IIC( JNIEnv *env, jobject obj
		, jint deviceIndex, jint channel, jchar counts ) {
	assert( env != 0 );
	return DACDirect( deviceIndex, channel, counts );
}	// Java_com_acces_aiousb_AnalogOutputSubsystem_writeCounts__IIC()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_AnalogOutputSubsystem_writeCounts__I_3C( JNIEnv *env, jobject obj, jint deviceIndex, jcharArray points ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	const jsize numPoints = ( *env )->GetArrayLength( env, points ) / 2;
	if( numPoints > 0 ) {
		jchar *const nativePoints = ( *env )->GetCharArrayElements( env, points, NULL );
		if( nativePoints != NULL ) {
			result = DACMultiDirect( deviceIndex, nativePoints, numPoints );
			( *env )->ReleaseCharArrayElements( env, points, nativePoints, 0 );
		}	// if( nativePoints ...
	}	// if( numPoints ...
	return result;
}	// Java_com_acces_aiousb_AnalogOutputSubsystem_writeCounts__I_3C()

// }}}

// {{{ AO16_AnalogOutputSubsystem.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_AO16_1AnalogOutputSubsystem_setBoardRange( JNIEnv *env, jobject obj, jint deviceIndex, jint range ) {
	assert( env != 0 );
	return DACSetBoardRange( deviceIndex, range );
}	// Java_com_acces_aiousb_AO16_1AnalogOutputSubsystem_setBoardRange()

// }}}

// {{{ DigitalIOSubsystem.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_getNumPorts( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int numPorts = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			numPorts = deviceTable[ deviceIndex ].DIOBytes;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return numPorts;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_getNumPorts()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_getNumTristateGroups( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int numGroups = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			numGroups = deviceTable[ deviceIndex ].Tristates;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return numGroups;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_getNumTristateGroups()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_configure__IZ_3B_3B( JNIEnv *env, jobject obj
		, jint deviceIndex, jboolean tristate, jbyteArray outputMask, jbyteArray values ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeOutputMask = ( *env )->GetByteArrayElements( env, outputMask, NULL );
	if( nativeOutputMask != NULL ) {
		jbyte *const nativeValues = ( *env )->GetByteArrayElements( env, values, NULL );
		if( nativeValues != NULL ) {
			result = DIO_Configure( deviceIndex, ( unsigned char ) ( tristate ? AIOUSB_TRUE : AIOUSB_FALSE ), nativeOutputMask, nativeValues );
			( *env )->ReleaseByteArrayElements( env, values, nativeValues, 0 );
		}	// if( nativeValues ...
		( *env )->ReleaseByteArrayElements( env, outputMask, nativeOutputMask, 0 );
	}	// if( nativeOutputMask ...
	return result;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_configure__IZ_3B_3B()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_configure__I_3B_3B_3B( JNIEnv *env, jobject obj
		, jint deviceIndex, jbyteArray tristateMask, jbyteArray outputMask, jbyteArray values ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeTristateMask = ( *env )->GetByteArrayElements( env, tristateMask, NULL );
	if( nativeTristateMask != NULL ) {
		jbyte *const nativeOutputMask = ( *env )->GetByteArrayElements( env, outputMask, NULL );
		if( nativeOutputMask != NULL ) {
			jbyte *const nativeValues = ( *env )->GetByteArrayElements( env, values, NULL );
			if( nativeValues != NULL ) {
				result = DIO_ConfigureEx( deviceIndex, nativeOutputMask, nativeValues, nativeTristateMask );
				( *env )->ReleaseByteArrayElements( env, values, nativeValues, 0 );
			}	// if( nativeValues ...
			( *env )->ReleaseByteArrayElements( env, outputMask, nativeOutputMask, 0 );
		}	// if( nativeOutputMask ...
		( *env )->ReleaseByteArrayElements( env, tristateMask, nativeTristateMask, 0 );
	}	// if( nativeTristateMask ...
	return result;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_configure__I_3B_3B_3B()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_getConfiguration( JNIEnv *env, jobject obj
		, jint deviceIndex, jbyteArray tristateMask, jbyteArray outputMask ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeTristateMask = ( *env )->GetByteArrayElements( env, tristateMask, NULL );
	if( nativeTristateMask != NULL ) {
		jbyte *const nativeOutputMask = ( *env )->GetByteArrayElements( env, outputMask, NULL );
		if( nativeOutputMask != NULL ) {
			result = DIO_ConfigurationQuery( deviceIndex, nativeOutputMask, nativeTristateMask );
			( *env )->ReleaseByteArrayElements( env, outputMask, nativeOutputMask, 0 );
		}	// if( nativeOutputMask ...
		( *env )->ReleaseByteArrayElements( env, tristateMask, nativeTristateMask, 0 );
	}	// if( nativeTristateMask ...
	return result;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_getConfiguration()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_read1( JNIEnv *env, jobject obj
		, jint deviceIndex, jint channel, jbooleanArray values ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jboolean *const nativeValues = ( *env )->GetBooleanArrayElements( env, values, NULL );
	if( nativeValues != NULL ) {
		unsigned char value;
		result = DIO_Read1( deviceIndex, channel, &value );
		if( result == AIOUSB_SUCCESS )
			nativeValues[ 0 ] = ( value != 0 );
		( *env )->ReleaseBooleanArrayElements( env, values, nativeValues, 0 );
	}	// if( nativeValues ...
	return result;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_read1()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_readAll( JNIEnv *env, jobject obj
		, jint deviceIndex, jbyteArray values ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeValues = ( *env )->GetByteArrayElements( env, values, NULL );
	if( nativeValues != NULL ) {
		result = DIO_ReadAll( deviceIndex, nativeValues );
		( *env )->ReleaseByteArrayElements( env, values, nativeValues, 0 );
	}	// if( nativeValues ...
	return result;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_readAll()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DigitalIOSubsystem_writeAll( JNIEnv *env, jobject obj
		, jint deviceIndex, jbyteArray values ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jbyte *const nativeValues = ( *env )->GetByteArrayElements( env, values, NULL );
	if( nativeValues != NULL ) {
		result = DIO_WriteAll( deviceIndex, nativeValues );
		( *env )->ReleaseByteArrayElements( env, values, nativeValues, 0 );
	}	// if( nativeValues ...
	return result;
}	// Java_com_acces_aiousb_DigitalIOSubsystem_writeAll()

// }}}

// {{{ DIOStreamSubsystem.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_DIOStreamSubsystem_setClock( JNIEnv *env, jobject obj
		, jint deviceIndex, jboolean directionRead, jdoubleArray clock ) {
	assert( env != 0 );
	double unusedClock = 0;
	jdouble nativeClock[ 1 ];
	( *env )->GetDoubleArrayRegion( env, clock, 0, 1, nativeClock );
	const int result = directionRead
		? DIO_StreamSetClocks( deviceIndex, &nativeClock[ 0 ], &unusedClock )
		: DIO_StreamSetClocks( deviceIndex, &unusedClock, &nativeClock[ 0 ] );
	if( result == AIOUSB_SUCCESS )
		( *env )->SetDoubleArrayRegion( env, clock, 0, 1, nativeClock );
	return result;
}	// Java_com_acces_aiousb_DIOStreamSubsystem_setClock()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DIOStreamSubsystem_streamOpen( JNIEnv *env, jobject obj
		, jint deviceIndex, jboolean directionRead ) {
	assert( env != 0 );
	return DIO_StreamOpen( deviceIndex, directionRead ? AIOUSB_TRUE : AIOUSB_FALSE );
}	// Java_com_acces_aiousb_DIOStreamSubsystem_streamOpen()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DIOStreamSubsystem_streamClose( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	return DIO_StreamClose( deviceIndex );
}	// Java_com_acces_aiousb_DIOStreamSubsystem_streamClose()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_DIOStreamSubsystem_streamFrame( JNIEnv *env, jobject obj
		, jint deviceIndex, jcharArray values, jintArray samplesTransferred ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	const int numSamples = ( *env )->GetArrayLength( env, values );
	jchar *const nativeValues = ( *env )->GetCharArrayElements( env, values, NULL );
	if( nativeValues != NULL ) {
		unsigned long bytesTransferred;
		result = DIO_StreamFrame( deviceIndex, numSamples, nativeValues, &bytesTransferred );
		( *env )->ReleaseCharArrayElements( env, values, nativeValues, 0 );
		if( result == AIOUSB_SUCCESS ) {
			jint nativeSamples[ 1 ] = { bytesTransferred / sizeof( unsigned short ) };
			( *env )->SetIntArrayRegion( env, samplesTransferred, 0, 1, nativeSamples );
		}	// if( result ...
	}	// if( nativeValues ...
	return result;
}	// Java_com_acces_aiousb_DIOStreamSubsystem_streamFrame()

// }}}

// {{{ Counter.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_Counter_setMode( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockIndex, jint counterIndex, jint mode ) {
	assert( env != 0 );
	return CTR_8254Mode( deviceIndex, blockIndex, counterIndex, mode );
}	// Java_com_acces_aiousb_Counter_setMode()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_Counter_setCount( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockIndex, jint counterIndex, jchar count ) {
	assert( env != 0 );
	return CTR_8254Load( deviceIndex, blockIndex, counterIndex, count );
}	// Java_com_acces_aiousb_Counter_setCount()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_Counter_setModeAndCount( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockIndex, jint counterIndex, jint mode, jchar count ) {
	assert( env != 0 );
	return CTR_8254ModeLoad( deviceIndex, blockIndex, counterIndex, mode, count );
}	// Java_com_acces_aiousb_Counter_setModeAndCount()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_Counter_readCount( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockIndex, jint counterIndex, jcharArray count ) {
	assert( env != 0 );
	unsigned short readValue;
	const unsigned long result = CTR_8254Read( deviceIndex, blockIndex, counterIndex, &readValue );
	if( result == AIOUSB_SUCCESS ) {
		jchar nativeCount[ 1 ] = { readValue };
		( *env )->SetCharArrayRegion( env, count, 0, 1, nativeCount );
	}	// if( result ...
	return result;
}	// Java_com_acces_aiousb_Counter_readCount()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_Counter_readCountAndStatus( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockIndex, jint counterIndex, jcharArray countStatus ) {
	assert( env != 0 );
	unsigned short readValue;
	unsigned char status;
	const unsigned long result = CTR_8254ReadStatus( deviceIndex, blockIndex, counterIndex, &readValue, &status );
	if( result == AIOUSB_SUCCESS ) {
		jchar nativeCountStatus[ 2 ] = { readValue, status };
		( *env )->SetCharArrayRegion( env, countStatus, 0, 2, nativeCountStatus );
	}	// if( result ...
	return result;
}	// Java_com_acces_aiousb_Counter_readCountAndStatus()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_Counter_readCountAndSetModeAndCount( JNIEnv *env, jobject obj
		, jint deviceIndex, jint blockIndex, jint counterIndex, jint mode, jchar count, jcharArray prevCount ) {
	assert( env != 0 );
	unsigned short readValue;
	const unsigned long result = CTR_8254ReadModeLoad( deviceIndex, blockIndex, counterIndex, mode, count, &readValue );
	if( result == AIOUSB_SUCCESS ) {
		jchar nativePrevCount[ 1 ] = { readValue };
		( *env )->SetCharArrayRegion( env, prevCount, 0, 1, nativePrevCount );
	}	// if( result ...
	return result;
}	// Java_com_acces_aiousb_Counter_readCountAndSetModeAndCount()

// }}}

// {{{ CounterSubsystem.java

JNIEXPORT jint JNICALL Java_com_acces_aiousb_CounterSubsystem_getNumCounterBlocks( JNIEnv *env, jobject obj, jint deviceIndex ) {
	assert( env != 0 );
	assert( deviceIndex >= 0
		&& deviceIndex < MAX_USB_DEVICES );
	int numCounterBlocks = 0;
	if( AIOUSB_Lock() ) {
		if(
			deviceIndex >= 0
			&& deviceIndex < MAX_USB_DEVICES
		)
			numCounterBlocks = deviceTable[ deviceIndex ].Counters;
		AIOUSB_UnLock();
	}	// if( AIOUSB_Lock() )
	return numCounterBlocks;
}	// Java_com_acces_aiousb_CounterSubsystem_getNumCounterBlocks()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_CounterSubsystem_readLatched( JNIEnv *env, jobject obj
		, jint deviceIndex, jcharArray counts ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jchar *const nativeCounts = ( *env )->GetCharArrayElements( env, counts, NULL );
	if( nativeCounts != NULL ) {
		result = CTR_8254ReadLatched( deviceIndex, nativeCounts );
		( *env )->ReleaseCharArrayElements( env, counts, nativeCounts, 0 );
	}	// if( nativeCounts ...
	return result;
}	// Java_com_acces_aiousb_CounterSubsystem_readLatched()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_CounterSubsystem_readAll( JNIEnv *env, jobject obj
		, jint deviceIndex, jcharArray counts ) {
	assert( env != 0 );
	unsigned long result = AIOUSB_ERROR_INVALID_DATA;
	jchar *const nativeCounts = ( *env )->GetCharArrayElements( env, counts, NULL );
	if( nativeCounts != NULL ) {
		result = CTR_8254ReadAll( deviceIndex, nativeCounts );
		( *env )->ReleaseCharArrayElements( env, counts, nativeCounts, 0 );
	}	// if( nativeCounts ...
	return result;
}	// Java_com_acces_aiousb_CounterSubsystem_readAll()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_CounterSubsystem_selectGate( JNIEnv *env, jobject obj
		, jint deviceIndex, jint counter ) {
	assert( env != 0 );
	return CTR_8254SelectGate( deviceIndex, counter );
}	// Java_com_acces_aiousb_CounterSubsystem_selectGate()


JNIEXPORT jint JNICALL Java_com_acces_aiousb_CounterSubsystem_startClock( JNIEnv *env, jobject obj
		, jint deviceIndex, jint counterBlock, jdoubleArray clock ) {
	assert( env != 0 );
	jdouble nativeClock[ 1 ];
	( *env )->GetDoubleArrayRegion( env, clock, 0, 1, nativeClock );
	const unsigned long result = CTR_StartOutputFreq( deviceIndex, counterBlock, &nativeClock[ 0 ] );
	if( result == AIOUSB_SUCCESS )
		( *env )->SetDoubleArrayRegion( env, clock, 0, 1, nativeClock );
	return result;
}	// Java_com_acces_aiousb_CounterSubsystem_startClock()

// }}}


/* end of file */
