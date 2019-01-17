/**
 * @file   AIOCmd.h
 * @author $Format: %an <%ae>$
 * @date   $Format: %ad$
 * @version $Format: %h$
 * @brief  General structure for processing AIOUSB commands
 */


#include "AIOTypes.h"
#include "AIOCmd.h"
#include "AIOEither.h"


#ifdef __cplusplus
namespace AIOUSB 
{
#endif

/*----------------------------------------------------------------------------*/
AIOCmd *NewAIOCmdFromJSON( const char *str )
{
    AIO_ASSERT_RET( NULL, str );
    AIOCmd *cmd = (AIOCmd*)calloc(1,sizeof(AIOCmd));

    return cmd;
}

/*----------------------------------------------------------------------------*/
AIOCmd *NewAIOCmd()
{
    AIOCmd *cmd = (AIOCmd*)calloc(1,sizeof(AIOCmd));

    return cmd;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE DeleteAIOCmd( AIOCmd *cmd )
{
    AIO_ASSERT( cmd );
    free(cmd);
    return AIOUSB_SUCCESS;
}




#ifdef __cplusplus
}
#endif
