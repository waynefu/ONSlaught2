#ifdef USE_PRECOMPILED_HEADERS
#ifdef _MSC_VER
#pragma once
#endif

#define PRECOMPILED_HEADERS_ARE_AVAILABLE

#include <memory>
#include <vector>
#include <SDL.h>
#ifdef USE_BOOST_MUTEX
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#endif

#endif
