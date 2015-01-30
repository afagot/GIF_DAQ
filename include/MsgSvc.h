#ifndef _MSGSVC_H_
#define _MSGSVC_H_
//***************************************************************
// *   MsgSvc.h
// *   Alexis Fagot
// *   20/01/2015
// *   Simplified msg service to allow output supression
// *   Created by Nir Amram
// *   06/21/2010
//***************************************************************

#include <iostream>

#define RED    "\033[1;31m"
#define GREEN  "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE   "\033[1;34m"
#define PINK   "\033[1;35m"
#define CYAN   "\033[1;36m"
#define WHITE  "\033[1;39m"
#define NC     "\033[1;0m"
#define BLANK  ""

#define LVL_ALWAYS  6
#define LVL_FATAL   5
#define LVL_ERROR   4
#define LVL_WARNING 3
#define LVL_INFO    2
#define LVL_DEBUG   1
#define LVL_VERBOSE 0
#define OUTPUTLEVEL LVL_INFO

/** printing macros; private use */
//#define MSG_LVL_NOCHK_(name, ...) { printf("%10s: ",name); printf(__VA_ARGS__);}
#define MSG_LVL_NOCHK_(name, ...) { printf("%s",name); printf(__VA_ARGS__); printf("%s",NC);}
#define MSG_LVL_(lvl, name, ...)            \
do {                                        \
    if ( lvl >= OUTPUTLEVEL ) {             \
        MSG_LVL_NOCHK_(name, __VA_ARGS__);  \
    }                                       \
} while (0)
/** printing macros; public use */
#define MSG_VERBOSE(...) MSG_LVL_(LVL_VERBOSE, CYAN,   __VA_ARGS__)
#define MSG_DEBUG(...)   MSG_LVL_(LVL_DEBUG,   GREEN,  __VA_ARGS__)
#define MSG_INFO(...)    MSG_LVL_(LVL_INFO,    NC,     __VA_ARGS__)
#define MSG_WARNING(...) MSG_LVL_(LVL_WARNING, PINK,   __VA_ARGS__)
#define MSG_ERROR(...)   MSG_LVL_(LVL_ERROR,   RED,    __VA_ARGS__)
#define MSG_FATAL(...)   MSG_LVL_(LVL_FATAL,   YELLOW, __VA_ARGS__)
#define MSG_ALWAYS(...)  MSG_LVL_(LVL_ALWAYS,  WHITE,  __VA_ARGS__)

#define MSG_CYAN(...)    MSG_LVL_(LVL_INFO,    CYAN,   __VA_ARGS__)
#define MSG_GREEN(...)   MSG_LVL_(LVL_INFO,    GREEN,  __VA_ARGS__)
#define MSG_PINK(...)    MSG_LVL_(LVL_INFO,    PINK,   __VA_ARGS__)
#define MSG_RED(...)     MSG_LVL_(LVL_INFO,    RED,    __VA_ARGS__)
#define MSG_YELLOW(...)  MSG_LVL_(LVL_INFO,    YELLOW, __VA_ARGS__)
#define MSG_WHITE(...)   MSG_LVL_(LVL_INFO,    WHITE,  __VA_ARGS__)

#endif

