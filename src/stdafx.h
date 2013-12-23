#ifdef USE_PRECOMPILED_HEADERS
#ifdef _MSC_VER
#pragma once
#endif

#define PRECOMPILED_HEADERS_ARE_AVAILABLE

#include <memory>
#include <vector>
#include <deque>
#include <list>
#include <SDL.h>

#ifdef USE_BOOST_STUFF
#define USE_BOOST_MUTEX
#define USE_BOOST_COROUTINES
#endif

#ifdef USE_BOOST_MUTEX
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#endif

#ifdef USE_BOOST_COROUTINES
#include <boost/coroutine/coroutine.hpp>
#include <boost/thread.hpp>
#endif

#endif
