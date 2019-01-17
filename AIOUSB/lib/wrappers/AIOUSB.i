
%module AIOUSB

%include "cpointer.i"
%include "carrays.i"
%include "typemaps.i"
%include "aioarrays.i"
#if defined(SWIGJAVA)
%include "enums.swg"
#else

#endif

%feature("autodoc", "1");


%pointer_functions( unsigned long,  ulp );
%pointer_functions( long long, ullp );
%pointer_functions( double,  udp );
%pointer_functions( int,  ip );
%pointer_functions( unsigned short, usp );
%pointer_functions( double , dp );
%pointer_functions( char , cp );
%pointer_functions( unsigned char , ucp );
%array_functions( char , cstring );


%apply unsigned long *INOUT { unsigned long *result };
%apply long long { int64_t };
%apply unsigned long long { uint64_t };

%inline %{
    typedef char tmpchar;
%}

%apply AIORESULT *OUTPUT { unsigned long *result };

%{
  extern unsigned long ADC_BulkPoll( unsigned long DeviceIndex, unsigned long *INOUT );

%}

%{

  #include "AIOTypes.h"
  #include "AIOUSB_Core.h"
  #include "AIOCommandLine.h"
  #include "ADCConfigBlock.h"
  #include "AIOContinuousBuffer.h"
  #include "AIOChannelMask.h"
  #include "AIODeviceTable.h"    
  #include "AIODeviceQuery.h"
  #include "AIOProductTypes.h"
  #include "AIOUSBDevice.h"
  #include "AIODeviceInfo.h"
  #include "AIOUSB_Properties.h"
  #include "AIOUSB_DAC.h"
  #include "AIOUSB_ADC.h"
  #include "AIOUSB_CTR.h"
  #include "cJSON.h"
  #include "AIOUSB_DIO.h"
  #include "AIOBuf.h"
  #include "DIOBuf.h"
  #include "AIOList.h"
  #include "USBDevice.h"
  #include "libusb.h"
  #include <pthread.h>
%}

#if defined(SWIGJAVA)
/* ArrayList in of Arguments */
%typemap(in) ( int *argc, char **argv ) {
    jclass cls = (*jenv)->FindClass(jenv, "java/util/ArrayList");
    jmethodID sizeMethod = (*jenv)->GetMethodID(jenv, cls, "size", "()I");
    jmethodID getMethod = (*jenv)->GetMethodID(jenv, cls, "get", "(I)Ljava/lang/Object;");
    int i = 0;
    int tmpval;
    tmpval = (int)(long)(*jenv)->CallObjectMethod(jenv, jarg2, sizeMethod  );
    tmpval ++;
    $1 = &tmpval;
    // Java needs an extra one for
    // the name of the process
    $2 = (char **) malloc((*$1+1)*sizeof(char *));
    $2[0] = "tmp";
    for (i = 1; i<*$1; i++) {
        jstring jstr = (*jenv)->CallObjectMethod(jenv, jarg2, getMethod, i-1 );
        const char *cstr = (const char *)(*jenv)->GetStringUTFChars(jenv, jstr, 0);
        $2[i] = (char *)cstr;
    }
    $2[i+1] = 0;
}

%typemap(freearg) (int *argc, char **argv) {
    free($2);
 }

%typemap(argout) (int *argc, char **argv) {

    jclass cls = (*jenv)->FindClass(jenv, "java/util/ArrayList");
    jclass strCls =  (*jenv)->FindClass(jenv, "java/lang/Integer");
    int i ;
    intlistentry *np;                       
    (*jenv)->FindClass(jenv, "java/util/ArrayList");

    jmethodID clearMethod = (*jenv)->GetMethodID(jenv, cls, "clear", "()V");
    jmethodID addMethod  = (*jenv)->GetMethodID(jenv, cls, "add", "(Ljava/lang/Object;)Z");  
    jmethodID sizeMethod = (*jenv)->GetMethodID(jenv, cls, "size", "()I");

    (*jenv)->CallObjectMethod(jenv, jarg2, clearMethod );
    for ( i = 1; i < *$1; i ++ ) { 
        jstring temp_string = (*jenv)->NewStringUTF(jenv, $2[i] );
        (*jenv)->CallObjectMethod(jenv, jarg2, addMethod, temp_string  ); 
    }
}

%typemap(jni)    (int *argc, char **argv) "jobject"
%typemap(jtype)  (int *argc, char **argv) "java.util.ArrayList"
%typemap(jstype) (int *argc, char **argv) "java.util.ArrayList"
%typemap(javain) (int *argc, char **argv) "$javainput"


/*---------------- intlist * ----------------  */
%typemap(in) (intlist *indices ) {
    intlist *tmp = NewTailQListint();
    jclass cls = (*jenv)->FindClass(jenv, "java/util/ArrayList");
    jclass intcls = (*jenv)->FindClass(jenv, "java/lang/Integer");
    jmethodID intgetVal = (*jenv)->GetMethodID(jenv,intcls, "intValue", "()I");

    int thissize;
    jmethodID sizeMethod = (*jenv)->GetMethodID(jenv, cls, "size", "()I");
    jmethodID getMethod = (*jenv)->GetMethodID(jenv, cls, "get", "(I)Ljava/lang/Object;");

    int i;
    thissize = (int)(long)(*jenv)->CallObjectMethod(jenv, $input, sizeMethod );

    for ( i = 0; i < thissize; i ++ ) {
        jobject tmpobj = (*jenv)->CallObjectMethod(jenv, $input, getMethod, i );
        int val = (*jenv)->CallIntMethod(jenv,tmpobj, intgetVal );
        intlistInsert(tmp, val );
    }

    $1 = tmp;
}

%typemap(argout) (intlist *indices ) {
    jclass cls = (*jenv)->FindClass(jenv, "java/util/ArrayList");
    jclass intCls =  (*jenv)->FindClass(jenv, "java/lang/Integer");
    long mid;
    intlistentry *np;                       
    (*jenv)->FindClass(jenv, "java/util/ArrayList");
    jmethodID clearMethod = (*jenv)->GetMethodID(jenv, cls, "clear", "()V");
    jmethodID addMethod  = (*jenv)->GetMethodID(jenv, cls, "add", "(Ljava/lang/Object;)Z");  
    jmethodID intInit = (*jenv)->GetMethodID(jenv, intCls, "<init>", "(I)V" );
    jclass clazz = (*jenv)->GetObjectClass(jenv, $input );
    (*jenv)->CallObjectMethod(jenv, $input, clearMethod );
    for (np = $1->head.tqh_first; np != NULL; np = np->entries.tqe_next) {                    
        jobject tmpobj =  (*jenv)->NewObject(jenv, intCls  ,intInit , np->_value );
        (*jenv)->CallObjectMethod(jenv, $input, addMethod, tmpobj  ); 
    }                                                                                           
}

%typemap(freearg) (intlist *indices) {
    if ($1) 
        DeleteTailQListint( $1 );
}

%typemap(jni)    intlist *indices  "jobject"
%typemap(jtype)  intlist *indices  "java.util.ArrayList"
%typemap(jstype) intlist *indices  "java.util.ArrayList"
%typemap(javain) intlist *indices  "$javainput"



#endif

#if defined(SWIGPYTHON)
%typemap(in) unsigned char *pGainCodes {
    int i;
    static unsigned char temp[16];
    if (!PySequence_Check($input)) {
        PyErr_SetString(PyExc_ValueError,"Expected a sequence");
        return NULL;
    }
    if (PySequence_Length($input) != 16 ) {
        PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected 16 elements");
        return NULL;
    }
    for (i = 0; i < 16; i++) {
        PyObject *o = PySequence_GetItem($input,i);
        if (PyNumber_Check(o)) {
            temp[i] = (unsigned char) PyFloat_AsDouble(o);
        } else {
            PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");
            return NULL;
        }
    }
    $1 = temp;
}

%typemap(in) (int *argc, char **argv) {
    int i;
    $2 = NULL;
    if (!PyList_Check($input)) {
        PyErr_SetString(PyExc_ValueError, "Expecting a list");
        return NULL;
    }
    $1 = (int *)malloc(sizeof(int));
    if (!$1 ) 
        return NULL;
    *$1 = PyList_Size($input);
    $2 = (char **) malloc((*$1+1)*sizeof(char *));
    for (i = 0; i < *$1; i++) {
        PyObject *s = PyList_GetItem($input,i);
        if (!PyString_Check(s)) {
            free($2);
            PyErr_SetString(PyExc_ValueError, "List items must be strings");
            return NULL;
        }
        $2[i] = PyString_AsString(s);
    }
    $2[i] = 0;
}
/* After arguments have been processed....
 * we set the argv to be this new value */
%typemap(argout) (int *argc, char **argv) {
    PyList_SetSlice($input, 0, PyList_Size($input), NULL);
    {
        int i; 
        for ( i = 0; i < *$1 ; i ++ ) { 
            PyObject *ofmt = SWIG_Python_str_FromChar( $2[i] );
            PyList_Append( $input, ofmt );
        }
    } 
}

%typemap(in) (int **where, int *length) {
    int *tmp = 0;
    $1 = &tmp;
    $2 = (int *)malloc(sizeof(int));
    *$2 = 0;
}
/* After arguments have been processed....
 * we set the argv to be this new value */
%typemap(argout) (int **where, int *length) {
    PyList_SetSlice($input, 0, (Py_ssize_t)*$2, NULL);
    {
        int i; 
        /* printf("After: %d\n", *$1 ); */
        for ( i = 0; i < *$2 ; i ++ ) { 
            PyObject *ofmt = PyInt_FromLong( *$1[i] );
            PyList_Append( $input, ofmt );
        }
    } 
}

/*---------------- intlist * ----------------  */
%typemap(in) (intlist *indices ) {
    intlist *tmp = Newintlist();
    $1 = tmp;
}

%typemap(argout) (intlist *indices) {
    PyList_SetSlice($input, 0, (Py_ssize_t)intlistSize($1), NULL);
    {
       intlistentry *np;                                                           
       for (np = $1->head.tqh_first; np != NULL; np = np->entries.tqe_next) {                    
           PyObject *ofmt = PyInt_FromLong( np->_value );
           PyList_Append( $input, ofmt );
       }                                                                                           
    } 
}

%typemap(freearg) (intlist *indices) {
    if ($1) 
        Deleteintlist( $1 );
}


#endif

#if defined(SWIGJAVA) && defined(SWIGANDROID)
#else
/* Needed to allow inclusion into Scala */
%pragma(java) moduleimports=%{
     import cz.adamh.utils.NativeUtils;
     import java.io.IOException;
%}

%pragma(java) modulecode=%{
    static {
        try {    
            NativeUtils.loadLibraryFromJar("/libaiousbdbg.so");
            NativeUtils.loadLibraryFromJar("/libAIOUSB.so");
        } catch (IOException e) {    
            e.printStackTrace();  
        }    
    }
%}
#endif

%newobject CreateSmartBuffer;
%newobject NewAIOBuf;
%newobject NewAIODeviceQuery;
%delobject AIOBuf::DeleteAIOBuf;


AIOUSBDevice *AIODeviceTableGetDeviceAtIndex( unsigned long DeviceIndex , unsigned long *OUTPUT );



#if defined(SWIGPYTHON)

%typemap(in)  double *pBuf
{
    double temp[256];
    $1 = temp;
}

%typemap(in)  double *ctrClockHz {
    double tmp = PyFloat_AsDouble($input);
    $1 = &tmp;
}

#elif defined(SWIGPERL)

%typemap(in) unsigned char *pGainCodes {
    AV *tempav;
    I32 len;
    int i;
    SV **tv;

    static unsigned char temp[16];
    if (!SvROK($input))
        croak("Argument $argnum is not a reference.");
    if (SvTYPE(SvRV($input)) != SVt_PVAV)
        croak("Argument $argnum is not an array.");

    tempav = (AV*)SvRV($input);
    len = av_len(tempav);
    if ( (int)len != 16-1 )  {
        croak("Bad stuff: length was %d\n", (int)len);
    }
    for (i = 0; i <= len; i++) {
        tv = av_fetch(tempav, i, 0);
        temp[i] = (unsigned char) SvNV(*tv );
        // printf("Setting value %d\n", (int)SvNV(*tv ));
    }
    $1 = temp;
}

%typemap(in) (int *argc, char **argv) {
    AV *tempav;
    I32 len;
    int i;
    SV **tv;
    if (!SvROK($input))
        croak("Argument $argnum is not a reference.");
    if (SvTYPE(SvRV($input)) != SVt_PVAV)
        croak("Argument $argnum is not an array.");

    $2 = NULL;

    $1 = (int *)malloc(sizeof(int));
    if (!$1 )
        return;

    tempav = (AV*)SvRV($input);
    *$1 = av_len(tempav);
    printf("Original value: %d\n", av_len(tempav) );
    $2 = (char **)malloc((*$1+1)*sizeof(char *));
    for (i = 0; i < *$1; i++) {
        tv = av_fetch(tempav, i, 0);
        $2[i] = (char *) SvPV(*tv,PL_na);
        printf("\targv[%d] = %s\n", i, $2[i] );
    }
    $2[i+1] = 0;
}

%typemap(argout) (int *argc, char **argv) {
    printf("After: %d\n", *$1 );
    SV **svs;
    AV *tempav = (AV*)SvRV($input);
    /* av_clear( tempav  ); */
    {
        int i;
        svs = (SV **) malloc(*$1*sizeof(SV *));
        for ( i = 0; i < *$1 ; i ++ ) { 
            printf("Values are %d\n", i );
        }
        /* for (i = 0; i < *$1 ; i++) { */
        /*     printf("Pushing value %s\n", $2[i] ); */
        /*     svs[i] = sv_newmortal(); */
        /*     /\* sv_setpv((SV*)svs[i],$2[i]); *\/ */
        /*     av_push( tempav, svs[i] ); */
        /* }; */
        /* myav = av_make(len,svs); */
        free(svs);
    } 
}


%typemap(freearg) (int *argc, char **argv) {
    if ($1) free($1);
    if ($2) free($2);
}

/*---------------- intlist * ----------------  */
%typemap(in) (intlist *indices ) {
    intlist *tmp = Newintlist();
    $1 = tmp;
    /* printf("BAAAAR\n"); */
}

%typemap(argout) (intlist *indices) {
    intlistentry *np;
    SV *tmp = $input;
    AV *ary = (AV*)SvRV( tmp );
    /* printf("Value is %d\n", (int)av_top_index(ary)); */
    av_clear(ary);
    for (np = $1->head.tqh_first; np != NULL; np = np->entries.tqe_next) {                    
        av_push( ary, newSViv( np->_value ));
    }                                                                                           
}

%typemap(freearg) (intlist *indices) {
    if ($1) 
        Deleteintlist( $1 );
}

%typemap(in)  double *ctrClockHz {
    double tmp = SvIV($input);
    $1 = &tmp;
}

#elif defined(SWIGRUBY)

/*---------------- intlist * ----------------  */
%typemap(in) (intlist *indices ) {
    intlist *tmp = Newintlist();
    $1 = tmp;
}

%typemap(argout) (intlist *indices) {
    VALUE ptr;
    intlistentry *np;
    ptr = (VALUE)RARRAY($input);
    rb_ary_clear(ptr);
    for (np = $1->head.tqh_first; np != NULL; np = np->entries.tqe_next) {                    
        rb_ary_push( ptr, INT2FIX( np->_value ));
    }                                                                                           

}

%typemap(freearg) (intlist *indices) {
    if ($1) 
        Deleteintlist( $1 );
}

/*--------------- done intlist * ---------------*/


%typemap(in)  double *ctrClockHz {
    double tmp = NUM2DBL($input);
    // printf("Type was %d\n", (int)tmp );
    $1 = &tmp;
}

%typemap(in) unsigned char *gainCodes {
    int i;
    static unsigned char temp[16];

    if ( RARRAY_LEN($input) != 16 ) {
        rb_raise(rb_eIndexError, "Length is not valid ");
    }
    for (i = 0; i < 16; i++) {
        temp[i] = (unsigned char)NUM2INT(rb_ary_entry($input, i)); 
        /* printf("Setting temp[%d] to %d\n", i, (int)temp[i] ); */
    }
    $1 = temp;
}

%typemap(in) (int *argc, char **argv) {
    int i;
    VALUE *ptr;
    $2 = NULL;
    $1 = (int *)malloc(sizeof(int));
    if (!$1 ) 
        return NULL;
    *$1 = RARRAY_LEN($input);
    $2 = (char **) malloc((*$1+1)*sizeof(char *));
    ptr = RARRAY_PTR($input);
    for (i = 0; i < *$1; i++, ptr++) {
        $2[i] = StringValuePtr(*ptr);
    }
    $2[i] = 0;
}

%typemap(argout) (int *argc, char **argv) {
    {
        int i; 
        VALUE ptr;
        /* printf("After: %d\n", *$1 ); */
        ptr = (VALUE)RARRAY($input);
        rb_ary_clear(ptr);
        for ( i = 0; i < *$1 ; i ++ ) { 
            rb_ary_push( ptr, rb_str_new2( $2[i] ));
            /* printf("Values are %s\n", $2[i] ); */
        }
    } 
}


%typemap(freearg) (int *argc, char **argv) {
    if ($1) free($1);
    if ($2) free($2);
}


#endif

/* Typemaps should remain before the includes */
%include "AIOTypes.h"
%include "AIOUSB_Core.h"
%include "ADCConfigBlock.h"
%include "AIOCommandLine.h"
%include "AIOContinuousBuffer.h"
%include "AIOUSB_Properties.h"
%include "AIOChannelMask.h"
%include "AIODeviceTable.h"    
%include "AIODeviceQuery.h"
%include "AIOUSB_ADC.h"
%include "AIOUSB_DAC.h"
%include "AIOUSB_CTR.h"
%include "AIOUSBDevice.h"
%include "AIODeviceInfo.h"
%include "AIOProductTypes.h"
%include "AIOUSB_DIO.h"
%include "cJSON.h"
%include "AIOBuf.h"
%include "DIOBuf.h"
%include "USBDevice.h"


%aioarray_class(unsigned short,ushortarray)
%aioarray_class(double,doublearray)


%inline %{
AIORET_TYPE ADC_GetScanVToDoubleArray( unsigned long DeviceIndex, doublearray *ary ) {
    return ADC_GetScanV( DeviceIndex, ary->el );
}
%}

%inline %{
AIORET_TYPE DIO_StreamFrameTmp( unsigned long DeviceIndex, unsigned long FramePoints, ushortarray *ary, unsigned long *bytesTransferred ) {
    return DIO_StreamFrame( DeviceIndex, FramePoints,  ary->el , bytesTransferred );
}
%}


%extend AIOCommandLineOptions {
    AIOCommandLineOptions() { 
        return NewAIOCommandLineOptionsFromDefaultOptions( AIO_SCRIPTING_OPTIONS() );
    }

    ~AIOCommandLineOptions() {
        DeleteAIOCommandLineOptions( $self );        
    }

    AIORET_TYPE ProcessCommandLine( int *argc, char **argv ) {
        return AIOProcessCommandLine( $self, argc, argv );
    }
}

%extend AIOChannelMask { 
    AIOChannelMask( unsigned size ) { 
        return (AIOChannelMask *)NewAIOChannelMask( size );
    }
    ~AIOChannelMask() { 
        DeleteAIOChannelMask($self);
    }

    const char *__str__() {
        return AIOChannelMaskToString( $self );
    }

    const char *__repr__() {
        return AIOChannelMaskToString( $self );
    }
 }

%extend AIODeviceQuery { 

    AIODeviceQuery( unsigned long DeviceIndex ) {
        return (AIODeviceQuery*)NewAIODeviceQuery( DeviceIndex );
    }

    ~AIODeviceQuery() { 
        DeleteAIODeviceQuery( $self );
    }

    char *__str__() { 
        return AIODeviceQueryToStr( $self );
    }    

    char *__repr__() { 
        return AIODeviceQueryToRepr( $self );
    }    

}

%extend AIOContinuousBuf {

    AIOContinuousBuf( unsigned long deviceIndex, unsigned numScans, unsigned numChannels ) {
        return (AIOContinuousBuf *)NewAIOContinuousBufForCounts( deviceIndex, numScans, numChannels );
    }

    ~AIOContinuousBuf() {
        DeleteAIOContinuousBuf($self);
    }

}


%extend AIOBuf {
   AIOBuf(int bufsize, int refsize)  {
       return (AIOBuf *)NewAIOBuf( (AIOBufType)bufsize, refsize );
   }  
  ~AIOBuf()  {
       DeleteAIOBuf($self);
  }
}


%extend DIOBuf {

  DIOBuf( int size ) {
    return (DIOBuf *)NewDIOBuf( size );
  }

  DIOBuf( char *ary, int size_array ) {
    return (DIOBuf *)NewDIOBufFromChar(ary, size_array );
  } 

  DIOBuf( char *ary ) {
    return (DIOBuf*)NewDIOBufFromBinStr( ary );
  }

  ~DIOBuf() {
    DeleteDIOBuf( $self );
  }
  
  int get(int index) { 
    return DIOBufGetIndex( $self, index );
  }

  int set(int index, int value ) {
    return DIOBufSetIndex( $self, index , value );
  }

  char *hex() {
    return DIOBufToHex($self);
  }

  char *to_hex() {
    return DIOBufToHex($self);
  }

  DIOBuf *resize( unsigned size ) {
    return DIOBufResize( $self, size );
  } 

  const char *__str__() {
    return DIOBufToString( $self );
  }
  
  const char *__repr__() { 
    return DIOBufToString( $self );
  }
 

}
#ifdef __cplusplus
   %extend DIOBuf {
     bool operator==( DIOBuf *b ) {
       int equiv = 1;
       if ( b->size != $self->size )
         return 0;
       for ( unsigned i = 0; i < b->size; i ++ )
           equiv &= ( $self->buffer[i] == b->buffer[i] );
       
       return equiv == 1;
     }
     
     bool operator!=( DIOBuf *b ) {
    return !($self == b);
     }
   }
#endif



#if defined(SWIGPYTHON)
%pythoncode %{
from AIOUSB import doublearray as volts
from AIOUSB import ushortarray as counts

def new_ushortarray(n):
    """Creates a new ushortarray of size n"""
    import AIOUSB
    return AIOUSB.ushortarray(n)

def delete_ushortarray(n):
    del n

def new_doublearray(n):
    """Creates a new double array of size n"""
    import AIOUSB
    return AIOUSB.doublearray(n)

def delete_doublearray(n):
    """Creates a new double array of size n"""
    del n

def AIOUSB_FindDevices(fn):
    import AIOUSB
    devices = [];
    index = 0;
    deviceMask = AIOUSB.GetDevices();

    while deviceMask > 0:
        if (deviceMask & 1 ) != 0:
            obj = AIOUSB.AIODeviceInfoGet( index );
            if fn( obj ):
                devices.append( index )

        index += 1
        deviceMask >>= 1;

    return devices;

%}


/* Special conversion so that binary string with multiple zeros aren't truncated */
%extend DIOBuf {
    
    %typemap(out) tmpchar * {
        printf("Strlen: %d ,  bytesize: %d\n", (int)strlen($1), (int)DIOBufByteSize( arg1 ));
        $result = PyUnicode_FromStringAndSize( $1, MAX(strlen($1),DIOBufByteSize( arg1 )) );
    }

    tmpchar *to_bin() {
        return DIOBufToBinary($self);
    }

}


%exception DIOBuf::__getitem__ { 
    $action 
    if ( result < 0 ) {
          PyErr_SetString(PyExc_IndexError,"Index out of range");
          return NULL;
    }
}

%exception DIOBuf::__setitem__ { 
    $action 
    if ( result < 0 ) { 
        if( result == -AIOUSB_ERROR_INVALID_INDEX ) {
          PyErr_SetString(PyExc_IndexError,"Index out of range");
          return NULL;
        } else if ( result == -AIOUSB_ERROR_INVALID_PARAMETER ) { 
          PyErr_SetString(PyExc_TypeError,"Invalid value");
          return NULL;
        }
    }
}




%extend DIOBuf {
  int __getitem__( int index ) {
    return DIOBufGetIndex( $self, index );
  }
  int __setitem__(int index, int value ) {
    return DIOBufSetIndex( $self, index, value );
  }
 }
#elif defined(SWIGOCTAVE)
%extend DIOBuf {

  int __brace__( unsigned index ) {
    return DIOBufGetIndex( $self, index );
  }
  int __brace_asgn__( unsigned index , int value ) {
    return DIOBufSetIndex( $self, index, value );
  }
  int __paren__( unsigned index ) {
    return DIOBufGetIndex( $self, index );
  }
  int __paren_asgn__( unsigned index , int value) {
    return DIOBufSetIndex( $self, index , value );
  }

 }
#elif defined(SWIGJAVA)


%extend DIOBuf {
    const char *toString() {
        return DIOBufToString( $self );
    }
} 

#elif defined(SWIGRUBY)
%extend DIOBuf {
int at( unsigned index ) {
return DIOBufGetIndex( $self, index );
}
}
#elif defined(SWIGPERL)
%perlcode %{
{
package AIOUSB;
sub AIOUSB_FindDevices
{
    my ($fn) = @_;
    my $devices = [];
    my $index = 0;
    my $deviceMask = AIOUSB::GetDevices();

    while ( $deviceMask > 0 ) {
        if ( ($deviceMask & 1 ) != 0 ) {
            $obj = AIOUSB::AIODeviceInfoGet( $index );
            if ( $fn->( $obj ) ) {
                push( @{$devices}, $index );
            }
        }
        $index ++;
        $deviceMask >>= 1;
    }
    return $devices;
}
}

{
package  AIOUSB::DIOBuf;
sub newDESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    #my $tmp = $self;
    my $tmp = "" . $self;
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        AIOUSBc::delete_DIOBuf($self);
        delete $OWNER{$tmp};
    }
}
 }
*AIOUSB::DIOBuf::DESTROY = *AIOUSB::DIOBuf::newDESTROY;
%}
#endif




