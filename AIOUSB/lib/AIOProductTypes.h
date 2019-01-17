
#ifndef _AIOPRODUCT_TYPES_H
#define _AIOPRODUCT_TYPES_H

#include "AIOTypes.h"
#ifdef __cplusplus
#include <algorithm>
#endif

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif


/**
 * @brief A simplified range of Products based off of device ids
 *
 */
typedef struct AIOProductRange  {
    unsigned long _start;
    unsigned long _end;
#ifndef SWIG
#ifdef __cplusplus
    AIOProductRange( unsigned long start, unsigned long end ) : _start(std::min(start,end)), _end(std::max(start,end)) {};
#endif
#endif
} AIOProductRange;


/**
 * @brief A smart product group that marks a range of ACCES I/O Products
 *
 */
typedef struct AIOProductGroup {
#ifndef SWIG
#ifdef __cplusplus
    AIOProductGroup( size_t numgroups, AIOProductRange **groups ) : _num_groups(numgroups), _groups(groups) {};
    AIOProductGroup( size_t numgroups, ... );
    ~AIOProductGroup();
#endif
#endif
    size_t _num_groups;
    AIOProductRange **_groups;
} AIOProductGroup;

/* BEGIN AIOUSB_API */
PUBLIC_EXTERN AIOProductRange *NewAIOProductRange( unsigned long start, unsigned long end);
PUBLIC_EXTERN AIORET_TYPE DeleteAIOProductRange( AIOProductRange *pr );
PUBLIC_EXTERN AIORET_TYPE AIOProductRangeStart( const AIOProductRange *pr );
PUBLIC_EXTERN AIORET_TYPE AIOProductRangeEnd( const AIOProductRange *pr );

PUBLIC_EXTERN AIOProductGroup *NewAIOProductGroup(size_t numgroups, ...  );
PUBLIC_EXTERN AIORET_TYPE DeleteAIOProductGroup(AIOProductGroup *);
PUBLIC_EXTERN AIORET_TYPE AIOProductGroupContains( const AIOProductGroup *g, unsigned long val );
PUBLIC_EXTERN AIOProductGroup *groupcpy ( const AIOProductGroup *g);

#define NUMARGS(...)  (sizeof((void*[]){__VA_ARGS__})/sizeof(void*))

#ifdef __cplusplus
#define AIO_RANGE(start,stop) NewAIOProductRange(start,stop)
#define AIO_PRODUCT_GROUP(NAME, N , ... ) const AIOProductGroup NAME( N, __VA_ARGS__ )
#define AIO_PRODUCT_CONSTANT(NAME, NAMEPTR, NAMEFN, N, ... )   const AIOProductGroup NAME( N, __VA_ARGS__ ); \
                                                               const AIOProductGroup *NAMEPTR = &NAME; \
                                                               AIOProductGroup *NAMEFN() { return groupcpy(NAMEPTR);};

#else
/* C definitions */
#define AIO_RANGE(start,stop) (&(AIOProductRange){ ._start=start, ._end =stop })
#define AIO_PRODUCT_GROUP(NAME, N , ... ) const AIOProductGroup NAME = { ._num_groups =N, ._groups = (AIOProductRange **)&(AIOProductRange *[N]){ __VA_ARGS__ } } ;
#define AIO_PRODUCT_CONSTANT(NAME, NAMEPTR,NAMEFN, N, ... )   AIO_PRODUCT_GROUP(NAME,N, __VA_ARGS__); \
                                                              const AIOProductGroup *NAMEPTR = &NAME; \
                                                              AIOProductGroup *NAMEFN() { return groupcpy( NAMEPTR );}
#endif

#define AIO_PRODUCT_EXTERN(NAME,NAMEPTR,NAMEFN)        extern const AIOProductGroup NAME; \
                                                       extern const AIOProductGroup *NAMEPTR;\
                                                       extern AIOProductGroup *NAMEFN();

/* Global product definitions */
AIO_PRODUCT_EXTERN(AIO_ANALOG_OUTPUT_OBJ, AIO_ANALOG_OUTPUT_GROUP, AIO_ANALOG_OUTPUT );
AIO_PRODUCT_EXTERN(AIO_ANALOG_INPUT_OBJ , AIO_ANALOG_INPUT_GROUP, AIO_ANALOG_INPUT  );
AIO_PRODUCT_EXTERN( AIO_DIGITAL_HIGHSPEED_OBJ, AIO_DIGITAL_HIGHSPEED_GROUP, AIO_DIGITAL_HIGHSPEED );
AIO_PRODUCT_EXTERN( AIO_ANALOG_CLOCK_100KHZ_OBJ, AIO_ANALOG_CLOCK_100KHZ_GROUP, AIO_ANALOG_CLOCK_100KHZ );
AIO_PRODUCT_EXTERN( AIO_ANALOG_CLOCK_250KHZ_OBJ, AIO_ANALOG_CLOCK_250KHZ_GROUP, AIO_ANALOG_CLOCK_250KHZ );
AIO_PRODUCT_EXTERN( AIO_ANALOG_CLOCK_500KHZ_OBJ, AIO_ANALOG_CLOCK_500KHZ_GROUP, AIO_ANALOG_CLOCK_500KHZ );
AIO_PRODUCT_EXTERN( AIO_ANALOG_12BIT_A2D_OBJ, AIO_ANALOG_12BIT_A2D_GROUP, AIO_AIO_ANALOG_12BIT_A2D );
AIO_PRODUCT_EXTERN( AIO_ANALOG_16BIT_A2D_OBJ, AIO_ANALOG_16BIT_A2D_GROUP, AIO_AIO_ANALOG_16BIT_A2D );


/* END AIOUSB_API */

#ifdef __aiousb_cplusplus
}
#endif


#endif
