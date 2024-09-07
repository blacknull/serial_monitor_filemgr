#ifndef SM_FILEMGR
#define SM_FILEMGR

/* flash file system ---------------------------------------------------------*/
#include "FS.h"
#if defined(FS_SPIFFS)
    #ifdef ESP32
        #include "SPIFFS.h"
    #else
    #endif

    #define MyFs SPIFFS
    const String FS_PREFIX = "/spiffs/";
#elif defined(FS_LITTLEFS)
    #include <LittleFS.h>

    #define MyFs LittleFS
    const String FS_PREFIX = "/littlefs/";
#else    
#endif

extern void sm_filemgr_init();
extern void sm_filemgr_process();

#endif