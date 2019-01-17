#ifndef _CSTRINGARRAY_H
#define _CSTRINGARRAY_H

#include "AIOTypes.h"

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

typedef struct CStringArray {
    int _size;
    char ** _strings;
#ifdef __cplusplus
    CStringArray(int size, ... );
    ~CStringArray();
    CStringArray(const CStringArray &ref);
#endif
} CStringArray;

#ifdef __cplusplus
#define STRING_ARRAY(N, ... ) 
#else
#define STRING_ARRAY(N, ... ) 
#endif

/* BEGIN AIOUSB_API */
PUBLIC_EXTERN CStringArray *NewCStringArray(size_t numstrings );
PUBLIC_EXTERN CStringArray *NewCStringArrayWithStrings(size_t numstrings, ... );
PUBLIC_EXTERN CStringArray *NewCStringArrayFromCArgs( int argc, char *argv[] );
PUBLIC_EXTERN AIORET_TYPE DeleteCStringArray(CStringArray *str);
PUBLIC_EXTERN CStringArray *CopyCStringArray( CStringArray *str );
PUBLIC_EXTERN char *CStringArrayToString( CStringArray *str );
PUBLIC_EXTERN char *CStringArrayToStringWithDelimeter( CStringArray *str, const char *delim);
static inline char *CStringArray_pToString( CStringArray *sa) { return CStringArrayToString(sa); };
static inline AIORET_TYPE DeleteCStringArray_p(CStringArray *str) { return DeleteCStringArray(str); };

/* END AIOUSB_API */

#ifdef __aiousb_cplusplus
}
#endif


#endif
