#ifndef __DEBUG_MACRO_HPP__
#define __DEBUG_MACRO_HPP__


#ifdef WITH_DEBUG_INFO
#define DEBUG_LINE(...) __VA_ARGS__ ;
#else
#endif // WITH_DEBUG_INFO


# endif // __DEBUG_MACRO_HPP__