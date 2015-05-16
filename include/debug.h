#ifndef DEBUG_H
#define DEBUG_H
    #ifdef DEBUG
        #include <stdio.h>
        
        #define debug(format, ...)     printf(format,##__VA_ARGS__)
        
        /*
         * Disable STDOUT buffering to enable printing before a newline 
         * character or buffer flush.
         */
    #else
        #define debug(format, ...)
    #endif /* DEBUG */
#endif /* DEBUG_H */
