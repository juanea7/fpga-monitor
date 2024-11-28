/*
 * Monitor APIs - debug configuration
 *
 * Author      : Juan Encinas Anchústegui <juan.encinas@upm.es>
 * Date        : February 2021
 * Description : This file contains definitions to customize the debug
 *               behavior in the Monitor APIs (basically, the amount of
 *               messages and info that is printed to the console.)
 *
 */


#ifndef _MONITOR_DBG_H_
#define _MONITOR_DBG_H_

#include <stdio.h>

// If not defined by command line, disable debug
#ifndef MONITOR_DEBUG
    #define MONITOR_DEBUG 0
#endif

// Debug messages
#if MONITOR_DEBUG
    #define monitor_print_debug(msg, args...) printf(msg, ##args)
    #define monitor_print_info(msg, args...) printf(msg, ##args)
#else
    #define monitor_print_debug(msg, args...)
    #if MONITOR_INFO
        #define monitor_print_info(msg, args...) printf(msg, ##args)
    #else
        #define monitor_print_info(msg, args...)
    #endif
#endif

// Error messages
#define monitor_print_error(msg, args...) printf(msg, ##args)

#endif /* _MONITOR_DBG_H_ */
