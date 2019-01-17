#ifndef _AIOLIST_H
#define _AIOLIST_H

#include "AIOTypes.h"
#include "CStringArray.h"
#include <sys/queue.h>

#ifdef __aiousb_cplusplus
namespace AIOUSB
{
#endif

#define TAIL_Q_LIST_TYPE( PRETTYNAME )   TailQList ## PRETTYNAME
#define TAIL_Q_LIST_ENTRY_TYPE( PRETTYNAME ) TailQListEntry ## PRETTYNAME
#define TAIL_Q_LIST( TYPE , PRETTYNAME )                                                                 \
    typedef struct TailQListEntry ## PRETTYNAME {                                                        \
        TYPE _value;                                                                                     \
        TAILQ_ENTRY(TailQListEntry ## PRETTYNAME) entries;                                               \
    } TailQListEntry ##PRETTYNAME;                                                                       \
    struct tailhead ## PRETTYNAME { struct TailQListEntry ## PRETTYNAME *tqh_first;                      \
                                         struct TailQListEntry ## PRETTYNAME **tqh_last; };              \
                                                                                                         \
    typedef struct TailQList ## PRETTYNAME {                                                             \
        struct TailQListEntry ## PRETTYNAME _list;                                                       \
        int _size;                                                                                       \
        /* New stuff */                                                                                  \
        struct tailhead ## PRETTYNAME head;                                                              \
        struct tailhead ## PRETTYNAME *headp;                                                            \
    } TailQList ##PRETTYNAME;                                                                            \
    PUBLIC_EXTERN TailQList ## PRETTYNAME  *NewTailQList ## PRETTYNAME();                                \
    TailQListEntry ## PRETTYNAME *NewTailQListEntry ## PRETTYNAME( TYPE value );                         \
    AIORET_TYPE DeleteTailQListEntry ## PRETTYNAME( TailQListEntry ## PRETTYNAME *entry );               \
    int TailQList ## PRETTYNAME ## Size( TailQList ## PRETTYNAME  *list );                               \
    TailQListEntry ## PRETTYNAME * TailQList ## PRETTYNAME ## First( TailQList ## PRETTYNAME *list );    \
    TailQListEntry ## PRETTYNAME * TailQList ## PRETTYNAME ## Last( TailQList ## PRETTYNAME *list );     \
    TYPE TailQList ## PRETTYNAME ## LastValue( TailQList ## PRETTYNAME *list );                          \
    TYPE TailQListEntry ## PRETTYNAME ## To ##PRETTYNAME( TailQListEntry ## PRETTYNAME *entry );         \
    char *TailQListEntry ## PRETTYNAME ## ToString( TailQListEntry ## PRETTYNAME *entry );               \
    char *TailQList ## PRETTYNAME ## ToString( TailQList ## PRETTYNAME  *list );                         \
    AIORET_TYPE DeleteTailQList ## PRETTYNAME ( TailQList ## PRETTYNAME  *list );                        \
    AIORET_TYPE TailQList ## PRETTYNAME ## Insert( TailQList ## PRETTYNAME  *list,                       \
                                                   TailQListEntry ## PRETTYNAME *nnode );
#ifdef DEBUG_NL
#define __NL__ __NL__
#else
#define __NL__
#endif

#define TAIL_Q_LIST_IMPLEMENTATION( TYPE, PRETTYNAME )                                                        \
__NL__        TailQList ## PRETTYNAME  *NewTailQList ## PRETTYNAME() {                                        \
__NL__            TailQList ## PRETTYNAME  *tmp=                                                              \
__NL__                (TailQList ## PRETTYNAME *)calloc(1,sizeof(TailQList ## PRETTYNAME ));                  \
__NL__            TAILQ_INIT( &(tmp->head) );                                                                 \
__NL__            return tmp;                                                                                 \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        TailQListEntry ## PRETTYNAME *NewTailQListEntry ## PRETTYNAME( TYPE value )                     \
__NL__        {                                                                                               \
__NL__            TailQListEntry ## PRETTYNAME *tmp =                                                         \
__NL__                (TailQListEntry ## PRETTYNAME *)calloc(1,sizeof(TailQListEntry ## PRETTYNAME));         \
__NL__            if ( !tmp ) return tmp;                                                                     \
__NL__            tmp->_value = value;                                                                        \
__NL__            return tmp;                                                                                 \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        AIORET_TYPE DeleteTailQListEntry ## PRETTYNAME( TailQListEntry ## PRETTYNAME *entry )           \
__NL__        {                                                                                               \
__NL__            AIO_ASSERT( entry );                                                                        \
__NL__            Delete ## PRETTYNAME (entry->_value);                                                       \
__NL__            free(entry);                                                                                \
__NL__            return AIOUSB_SUCCESS;                                                                      \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        char *TailQListEntry ## PRETTYNAME ## ToString( TailQListEntry ## PRETTYNAME *entry )           \
__NL__        {                                                                                               \
__NL__            char *tmpstr = 0;                                                                           \
__NL__            char *tmp;                                                                                  \
__NL__            if(asprintf(&tmpstr, "%s", (tmp= PRETTYNAME ## ToString(entry->_value)) )<0) return NULL;   \
__NL__            free(tmp);                                                                                  \
__NL__            return tmpstr;                                                                              \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        char *TailQList ## PRETTYNAME ## ToString( TailQList ## PRETTYNAME  *list ) {                   \
__NL__            TailQListEntry ## PRETTYNAME *np;                                                           \
__NL__            char start[] = "";                                                                          \
__NL__            char *tmp = start;                                                                          \
__NL__            int started = 1;                                                                            \
__NL__            char *keep = (char *)0;                                                                     \
__NL__            for (np = list->head.tqh_first; np != NULL; np = np->entries.tqe_next) {                    \
__NL__                char *_t1;                                                                              \
__NL__                if(asprintf(&keep, "%s%s", tmp, (_t1 = TailQListEntry ## PRETTYNAME ## ToString( np )))<0) return NULL; \
__NL__                free(_t1);                                                                              \
__NL__                if ( started != 1 )                                                                     \
__NL__                    free(tmp);                                                                          \
__NL__                tmp = strdup(keep);                                                                     \
__NL__                free(keep);                                                                             \
__NL__                if ( np->entries.tqe_next ) {                                                           \
__NL__                    if(asprintf(&keep, "%s,", tmp )<0)return NULL;                                      \
__NL__                    free(tmp);                                                                          \
__NL__                    tmp = strdup(keep);                                                                 \
__NL__                    free(keep);                                                                         \
__NL__                }                                                                                       \
__NL__                started = 0;                                                                            \
__NL__            }                                                                                           \
__NL__            return tmp;                                                                                 \
__NL__        }                                                                                               \
__NL__        int TailQList ## PRETTYNAME ## Size( TailQList ## PRETTYNAME  *list ) {return list->_size;}     \
__NL__                                                                                                        \
__NL__        TailQListEntry ## PRETTYNAME * TailQList ## PRETTYNAME ## First( TailQList ## PRETTYNAME *list )\
__NL__        {                                                                                               \
__NL__            TailQListEntry ## PRETTYNAME *tmp = list->head.tqh_first;                                   \
__NL__            return tmp;                                                                                 \
__NL__        }                                                                                               \
__NL__        TailQListEntry ## PRETTYNAME * TailQList ## PRETTYNAME ## Last( TailQList ## PRETTYNAME *list ) \
__NL__        {                                                                                               \
__NL__            TailQListEntry ## PRETTYNAME *tmp = (TailQListEntry ## PRETTYNAME *)(*(((struct tailhead ## PRETTYNAME *)((list->head).tqh_last))->tqh_last)); \
__NL__            return tmp;                                                                                 \
__NL__        }                                                                                               \
__NL__        TYPE TailQListEntry ## PRETTYNAME ## To ##PRETTYNAME( TailQListEntry ## PRETTYNAME *entry )  {  \
__NL__             return entry->_value;                                                                      \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        TYPE TailQList ## PRETTYNAME ## LastValue( TailQList ## PRETTYNAME *list )                      \
__NL__        {                                                                                               \
__NL__            return (TailQList ## PRETTYNAME ## Last( list ))->_value;                                   \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        AIORET_TYPE DeleteTailQList ## PRETTYNAME ( TailQList ## PRETTYNAME  *list ) {                  \
__NL__            AIO_ASSERT(list);                                                                           \
__NL__            TailQListEntry ## PRETTYNAME *tmp;                                                          \
__NL__            while ( list->head.tqh_first != NULL ) {                                                    \
__NL__                tmp = list->head.tqh_first;                                                             \
__NL__                TAILQ_REMOVE( &list->head, list->head.tqh_first, entries );                             \
__NL__                DeleteTailQListEntry ## PRETTYNAME ( tmp );                                             \
__NL__            }                                                                                           \
__NL__            free(list);                                                                                 \
__NL__            return AIOUSB_SUCCESS;                                                                      \
__NL__        }                                                                                               \
__NL__                                                                                                        \
__NL__        AIORET_TYPE TailQList ## PRETTYNAME ## Insert( TailQList ## PRETTYNAME  *list,                  \
__NL__                                                    TailQListEntry ## PRETTYNAME *nnode ) {             \
__NL__            AIO_ASSERT( list ); AIO_ASSERT( nnode );                                                    \
__NL__            TAILQ_INSERT_TAIL( &list->head, nnode, entries );                                           \
__NL__            list->_size ++;                                                                             \
__NL__            return AIOUSB_SUCCESS;                                                                      \
__NL__        }

TAIL_Q_LIST(int, int );
TAIL_Q_LIST(CStringArray *, CStringArray_p );
typedef TAIL_Q_LIST_TYPE( int ) intlist;
typedef TAIL_Q_LIST_ENTRY_TYPE( int ) intlistentry;

intlist *Newintlist();
AIORET_TYPE Deleteintlist(intlist *list);
char *intlistToString( intlist *list);
int intlistSize(intlist *list);

int intlistFirst( intlist *list );
AIORET_TYPE intlistInsert( intlist *list, int tmpval );


#define foreach_int( J, ILIST ) for ( intlistentry *_ ## IVAL = TailQListintFirst( ILIST) ; _ ## IVAL && (J = _##IVAL->_value); _ ## IVAL = _ ## IVAL->entries.tqe_next )

#define foreach_CStringArray_p( J, ILIST ) for ( CStringArray_plistentry *_ ## IVAL = TailQListCStringArray_pFirst( ILIST) ; _ ## IVAL && (J = _##IVAL->_value); _ ## IVAL = _ ## IVAL->entries.tqe_next )

#ifdef __aiousb_cplusplus
}
#endif

#endif


