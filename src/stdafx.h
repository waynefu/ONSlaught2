#ifdef USE_PRECOMPILED_HEADERS

#include "config.h"

#ifdef _MSC_VER
#pragma once
#endif

#define PRECOMPILED_HEADERS_ARE_AVAILABLE

#if SYSTEM_ID == SYSTEM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <deque>
#include <set>
#include <list>
#include <algorithm>
#include <utility>
#include <fstream>
#include <SDL.h>
#include "Eigen\Dense"

#ifdef USE_BOOST_STUFF
#define USE_BOOST_ATOMIC
#define USE_BOOST_COROUTINES
#define USE_BOOST_MUTEX
#define USE_BOOST_THREAD
#endif

#ifdef USE_BOOST_ATOMIC
#include <boost/atomic.hpp>
#endif

#ifdef USE_BOOST_COROUTINES
#include <boost/coroutine/coroutine.hpp>
#include <boost/thread.hpp>
#endif

#ifdef USE_BOOST_MUTEX
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#endif

#ifdef USE_BOOST_THREAD
#include <boost/thread.hpp>
#endif

#ifdef USE_OPENGL
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#endif
