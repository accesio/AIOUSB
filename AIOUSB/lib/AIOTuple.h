#ifndef _AIOTUPLE_H
#define _AIOTUPLE_H

#include "AIOTypes.h"
#include "CStringArray.h"
#include <stdio.h>

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

#ifdef __cplusplus
#define AIOTUPLE2_TYPE( NAME, T1, T2)                                         \
    typedef struct NAME {                                                     \
        T1 _1;                                                                \
        T2 _2;                                                                \
    NAME(T1 _t1, T2 _t2 )  : _1(_t1), _2(_t2) {};                             \
        T1 get_1( NAME *obj) { return obj->_1 ; };                            \
        T2 get_2( NAME *obj) { return obj->_2 ; };                            \
    } NAME;                                                                   \
    inline NAME * New ## NAME( T1 t1, T2 t2 ) { return new NAME( t1, t2 ); };

#define AIO_CHAR_ARRAY(N , ... ) new char *[N]{ __VA_ARGS__ }
#else
#define AIOTUPLE2_TYPE( NAME, T1, T2)                        \
    typedef struct NAME {                                    \
        T1 _1;                                               \
        T2 _2;                                               \
    } NAME;                                                  \
    T2 NAME ## get_2( NAME *obj ) { return obj->_2 ; };      \
    T1 NAME ## get_1( NAME *obj ) { return obj->_1 ; };
    

#define AIO_CHAR_ARRAY(N , ... ) (char **)&(char *[N]){ __VA_ARGS__ }
#endif

#define AIOTUPLE2_PTR( NAME, T1, T2 )  NAME *
#define AIOTUPLE2(NAME, T1, T2 ) NAME 


/* AIOTUPLE2_TYPE(AIOTuple2_AIORET_TYPE__char_p_p, AIORET_TYPE, char ** ); */
AIOTUPLE2_TYPE(AIOTuple2_AIORET_TYPE__CStringArray  , AIORET_TYPE, CStringArray );
AIOTUPLE2_TYPE(AIOTuple2_AIORET_TYPE__CStringArray_p, AIORET_TYPE, CStringArray * );


inline char *AIOTuple2_AIORET_TYPE__CStringArray_pToString( AIOTuple2_AIORET_TYPE__CStringArray_p * type ) { 
    char *tmp;
    char *t2 = CStringArrayToString(type->_2);
    asprintf(&tmp, "(%d,%s)", (int)type->_1, t2);
    free(t2);
    return tmp;
}
inline AIORET_TYPE DeleteAIOTuple2_AIORET_TYPE__CStringArray_p( AIOTuple2_AIORET_TYPE__CStringArray_p * type ) { 
    AIO_ASSERT( type );
    DeleteCStringArray( type->_2 );
#ifdef __cplusplus
    delete type;
#else
    free(type);
#endif
    return AIOUSB_SUCCESS;
}

#define AIOTUPLE2_TO_STR( TYPE , T ) TYPE ##ToString( T )


#ifdef __aiousb_cplusplus
}
#endif



#endif
