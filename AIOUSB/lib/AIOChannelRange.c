#include "AIOChannelRange.h"

#ifdef __cplusplus
namespace AIOUSB
{
#endif

typedef enum  { BEGIN, START_CHANNEL, END_CHANNEL, GAIN, END } STATE;

int aio_channel_range_error = 0;
/*----------------------------------------------------------------------------*/
AIOChannelRange *NewAIOChannelRangeFromStr( const char *str )
{
    AIOChannelRange *obj = (AIOChannelRange *)calloc( sizeof(AIOChannelRange), 1 );
    aio_channel_range_error = 0;
    const char *pos = str;
    STATE state = BEGIN;
    while ( *pos != '\0' ) {
        if ( state == BEGIN && isdigit( *pos ) ) {
            state = START_CHANNEL;
            obj->start = atoi( pos );
            for( ; *pos != '-' && *pos != '\0' ; pos = pos + 1);

            state = END_CHANNEL;
            
        } else if ( state == END_CHANNEL && isdigit(*pos ) ) {
            obj->end = atoi( pos );
            state = GAIN;
            for( ; *pos != '=' && *pos != '\0' ; pos = pos + 1);
        } else if ( state == GAIN && ( isdigit(*pos) || *pos == '+' ) ) {
            int found = 0;
            for ( int i = 0; i < LENGTH_AD_GAIN_CODE_STRINGS; i ++ ) {
                if( strncmp(pos,AD_GAIN_CODE_STRINGS[i].name, strlen(AD_GAIN_CODE_STRINGS[i].name)  ) == 0 ) {
                    obj->gain = AD_GAIN_CODE_STRINGS[i].gain;
                    found = 1;
                    state = END;
                    break;
                }
            }
            if ( !found ) {
                /* In valid gain */
                free(obj);
                obj = NULL;
            }
            break;
        } else {
            aio_channel_range_error = AIOUSB_ERROR_INVALID_PARAMETER;
            free(obj);
            return NULL;
        }
        pos = pos+1;
    }
    
    return obj;
}

/*----------------------------------------------------------------------------*/
void DeleteAIOChannelRange( AIOChannelRange *range )
{
    if ( range )
        free(range);
}

/*----------------------------------------------------------------------------*/
const char *lookup_voltage_range( ADGainCode code  ) 
{
    return AD_GAIN_CODE_STRINGS[code].name;
}

/*----------------------------------------------------------------------------*/
char *AIOChannelRangeToStr( AIOChannelRange *range )
{
    char *tmp;
    int retcode;
    retcode = asprintf(&tmp, "%d-%d=%sV", range->start,range->end, lookup_voltage_range( range->gain ) );
    if ( retcode < 0 ) 
        return NULL;
    else
        return tmp;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOChannelRangeGetStart( AIOChannelRange *range )
{
    if ( range ) 
        return range->start;
    return -AIOUSB_ERROR_INVALID_INDEX;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOChannelRangeGetEnd( AIOChannelRange *range )
{
    if ( range ) 
        return range->end;
    return -AIOUSB_ERROR_INVALID_DATA;
}

/*----------------------------------------------------------------------------*/
AIORET_TYPE AIOChannelRangeGetGain( AIOChannelRange *range )
{
    if ( range )
        return range->gain;
    return -AIOUSB_ERROR_INVALID_DATA;
}

    
#ifdef __cplusplus
}
#endif

#ifdef SELF_TEST
#include "gtest/gtest.h"

using namespace AIOUSB;

TEST(AIOChannelRange, Lookup ) 
{
    EXPECT_STREQ("0-10", lookup_voltage_range(AD_GAIN_CODE_0_10V) );
    EXPECT_STREQ("+-1" , lookup_voltage_range(AD_GAIN_CODE_1V) );
}

TEST(AIOChannelRange, Construction ) 
{
    AIOChannelRange *cr = NewAIOChannelRangeFromStr("2-15=+-10V");
    
    EXPECT_EQ( 2, AIOChannelRangeGetStart(cr) );
    EXPECT_EQ( 15, AIOChannelRangeGetEnd(cr) );
    EXPECT_EQ( AD_GAIN_CODE_10V, AIOChannelRangeGetGain(cr) );

    DeleteAIOChannelRange( cr );
    cr =  NewAIOChannelRangeFromStr("6-122=+-2V");
    EXPECT_EQ( 6, AIOChannelRangeGetStart(cr) );
    EXPECT_EQ( 122, AIOChannelRangeGetEnd(cr) );
    EXPECT_EQ( AD_GAIN_CODE_2V, AIOChannelRangeGetGain(cr) );

    DeleteAIOChannelRange( cr );
}

TEST(AIOChannelRange, BadInput )
{
    AIOChannelRange *cr = NewAIOChannelRangeFromStr("2-15=+-3");
    EXPECT_FALSE( cr );
    cr = NewAIOChannelRangeFromStr("-2-15=+-10");
    EXPECT_FALSE( cr );
    cr = NewAIOChannelRangeFromStr("2--15=+-10");    
    EXPECT_FALSE( cr );
}



int main(int argc, char *argv[] )
{
  AIORET_TYPE retval;
  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
#ifdef GTEST_TAP_PRINT_TO_STDOUT
  delete listeners.Release(listeners.default_result_printer());
#endif

  return RUN_ALL_TESTS();  

}

#endif
