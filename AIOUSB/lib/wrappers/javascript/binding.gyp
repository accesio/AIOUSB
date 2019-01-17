
{
"targets": [
           {
           "target_name": "AIOUSB",
           "sources": [ "AIOUSB_wrap.cxx" ],
           "ldflags": [
                   "-L$(AIO_LIB_DIR)","-lusb-1.0","-laiousb"
                   ],
           "include_dirs" : [
           "$(AIO_LIB_DIR)"
           ]
           },
           ]
}