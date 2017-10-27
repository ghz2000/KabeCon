#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG
#ifdef DEBUG
    #define BeginDebugPrint()    Serial.begin( 115200 )
#define DebugPrintln( message )\
        {\
            char __buff__[ 512 ];\
            Serial.print( message );\
            sprintf( __buff__\
                   , "(Func:%s, File:%s, Line:%d)"\
                   , __func__\
                   , __FILE__\
                   , __LINE__ );\
            Serial.println( __buff__ );\
            Serial.flush();\
        }
     #define DebugPrint( message )\
        {\
            Serial.print( message );\
            Serial.flush();\
        }
#else
    #define BeginDebugPrint()
    #define DebugPrintln( message )
    #define DebugPrint( message )
#endif // DEBUG
#endif // __DEBUG_H__
