#include "AIOTypes.h"
#include <sys/queue.h>
#include <stdio.h>
#include <string.h>
#include "AIOList.h"

#ifdef __cplusplus
namespace AIOUSB {
#endif

char *intToString( int val )
{
    char *tmp = 0;

    if ( asprintf(&tmp,"%d",val ) < 0 ) 
        return NULL;
    
    return tmp;
}
/**
 * Dummy function to make tail q list work
 */
AIORET_TYPE Deleteint( int val ) {
    return AIOUSB_SUCCESS;
}
 
TAIL_Q_LIST_IMPLEMENTATION( CStringArray *, CStringArray_p );
TAIL_Q_LIST_IMPLEMENTATION( int, int );

int newone;

int intlistFirst( intlist *list ) { 
    TailQListEntryint *ent = TailQListintFirst( list );
    assert(ent);
    return ent->_value;
}

intlist *Newintlist() { return NewTailQListint(); }
AIORET_TYPE Deleteintlist(intlist *list) { return DeleteTailQListint(list); }
char *intlistToString( intlist *list) { return TailQListintToString( list ); };
int intlistSize(intlist *list) { return TailQListintSize( list ); };
AIORET_TYPE intlistInsert( intlist *list, int tmpval ) { intlistentry *val = NewTailQListEntryint( tmpval ); if (!val ) { return AIOUSB_ERROR_VALUE(AIOUSB_ERROR_NOT_ENOUGH_MEMORY); } return TailQListintInsert( list, val ); }


#ifdef __cplusplus
}
#endif




#ifdef SELF_TEST

#include "gtest/gtest.h"
#include "AIOList.h"
#include <stdio.h>
#include <iostream>
using namespace AIOUSB;


TEST(QList,test)
{
    TailQListint *tmp = NewTailQListint();
    char *hold;

    TailQListintInsert( tmp, NewTailQListEntryint( 42 )  );
    TailQListintInsert( tmp, NewTailQListEntryint( 43 )  );
    TailQListintInsert( tmp, NewTailQListEntryint( 44 )  );
    ASSERT_STREQ((hold=TailQListintToString( tmp )), "42,43,44" ); 
    ASSERT_EQ( TailQListintSize(tmp), 3 );
    free(hold);

    DeleteTailQListint( tmp );
}

TEST(Qlist,CStringArray)
{
    TailQListCStringArray_p *tmp = NewTailQListCStringArray_p();
    char *hold;

    TailQListCStringArray_pInsert(tmp,NewTailQListEntryCStringArray_p(NewCStringArrayWithStrings(3,(char *)"This",(char *)"is",(char *)"a string")));
    TailQListCStringArray_pInsert(tmp,NewTailQListEntryCStringArray_p(NewCStringArrayWithStrings(3,(char *)"and", (char *)"another",(char *)"string")));
    ASSERT_STREQ((hold=TailQListCStringArray_pToString( tmp )), "This is a string,and another string" ); 
    ASSERT_EQ( TailQListCStringArray_pSize(tmp), 2 );


    free(hold);

    DeleteTailQListCStringArray_p( tmp );
}



TEST(intlist,BasicTest) 
{
   intlist *tmp = Newintlist();
   char *hold = 0;
   int expected[] = {23, 48, 56};
   intlistInsert( tmp, 23 );
   intlistInsert( tmp, 48 );
   intlistInsert( tmp, 56 );
   ASSERT_EQ( TailQListintSize(tmp), 3 );
   ASSERT_STREQ((hold=TailQListintToString( tmp )), (char *)"23,48,56" ); 
   int i = 0;
   int j;
   foreach_int( j,  tmp ) { 
       ASSERT_EQ( expected[i], j );
       i ++;
   }

   free(hold);
   DeleteTailQListint( tmp );
}

TEST(intlist,Iterate) {
   intlist *indices = Newintlist();
   char *hold = 0;
   int expected[] = {23, 48, 56, 64};
   for( int i = 0; i < sizeof(expected)/sizeof(int); i ++ ) { 
       intlistInsert( indices, expected[i] );
   }

   ASSERT_EQ( TailQListintSize(indices), sizeof(expected)/sizeof(int) );

   int i = 0;
   int k;
   foreach_int( k, indices ) {
       if ( i >= TailQListintSize( indices ) - 1 )
           break;
       ASSERT_EQ( expected[i], k );
       i ++;
   }
   ASSERT_EQ( i, TailQListintSize( indices ) - 1);
   
   ASSERT_EQ( TailQListEntryintToint(TailQListintLast( indices )), expected[sizeof(expected)/sizeof(int)-1] );

   free(hold);
   DeleteTailQListint( indices );
}

int 
main(int argc, char *argv[] ) 
{
  testing::InitGoogleTest(&argc, argv);
  testing::TestEventListeners & listeners = testing::UnitTest::GetInstance()->listeners();
  return RUN_ALL_TESTS();
}



#endif
