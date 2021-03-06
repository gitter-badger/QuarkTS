/*This file is part of the QuarkTS distribution.*/
#ifndef QRESPONSE_H
    #define QRESPONSE_H

    #include "qtypes.h"
    #include "qstimers.h"

    #include <string.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    typedef struct{
        /*This data should be handled only using the provided API*/
        struct _qResponse_Private_s{
            char *Pattern2Match;                /*< Points to the storage area provided by the user to hold the match requested response. */
            qSTimer_t timeout;                  /*< The timeout used to wait the requested response. */
            size_t MaxStrLength;                /*< The size of the storage area. */
            size_t PatternLength;               /*< The length of the current response request.*/
            volatile size_t MatchedCount;       /*< To hold the current number of response matches. */
            volatile qBool_t ResponseReceived;  /*< A flag that indicates when the response matches the request. */
        }qPrivate;
    }qResponse_t;

    void qResponse_Setup( qResponse_t * const obj, char *xLocBuff, size_t nMax ); 
    void qResponse_Reset( qResponse_t * const obj );
    qBool_t qResponse_Received( qResponse_t * const obj, const char *Pattern, size_t n );
    qBool_t qResponse_ReceivedWithTimeout( qResponse_t * const obj, const char *Pattern, size_t n, qTime_t t );
    qBool_t qResponse_ISRHandler( qResponse_t * const obj, const char rxchar );

    #ifdef __cplusplus
    }
    #endif

#endif
