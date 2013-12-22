#ifndef SYSTEM_ID
#define SYSTEM_WINDOWS    1
#define SYSTEM_LINUX      2
#define SYSTEM_BSD        3
#define SYSTEM_UNIX_OTHER 4
#define SYSTEM_PSP        5

#if defined _WIN32 || defined _WIN64
#define SYSTEM_ID SYSTEM_WINDOWS
#elif defined linux || defined __linux
#define SYSTEM_ID SYSTEM_LINUX
#elif defined __bsdi__
#define SYSTEM_ID SYSTEM_BSD
#elif defined __unix__ || defined __unix || (__APPLE__ && __MACH__)
#define SYSTEM_ID SYSTEM_UNIX_OTHER
#elif defined PSP
#define SYSTEM_ID SYSTEM_PSP
#endif
#endif //SYSTEM_ID

#ifndef COMPILER_ID
#define COMPILER_MSVCPP 1
#define COMPILER_GCC    2
#define COMPILER_CLANG  3

#if defined _MSC_VER
#define COMPILER_ID COMPILER_MSVCPP
#elif defined __GNUC__
#define COMPILER_ID COMPILER_GCC
#elif __clang__
#define COMPILER_ID COMPILER_CLANG
#else
#error This toolchain is not supported. You can remove this error and try to \
	   proceed anyway, but the compiled program may not work properly.
#endif
#endif //COMPILER_ID

#ifndef ARCH_ID
#define ARCH_x86_BEGIN       0
#define ARCH_x86             1
#define ARCH_x86_64          2
#define ARCH_x86_16          3
#define ARCH_x86_END         4
#define ARCH_ARM_BEGIN       5
#define ARCH_ARM_UNSPECIFIED 6
#define ARCH_ARM_END         7

#if COMPILER_ID == COMPILER_MSVCPP
	#if defined _M_IX86
		#define ARCH_ID ARCH_x86
		#define ARCH_NATURAL_BITNESS 32
	#elif defined _M_X64
		#define ARCH_ID ARCH_x86_64
		#define ARCH_NATURAL_BITNESS 64
	#elif defined _M_I86
		#define ARCH_ID ARCH_x86_16
		#define ARCH_NATURAL_BITNESS 16
	#elif defined _M_ARM
		#define ARCH_ID ARCH_ARM_UNSPECIFIED
	#endif
#elif COMPILER_ID == COMPILER_GCC
	#if defined i386 || defined __i386 || defined __i386__
		#define ARCH_ID ARCH_x86
		#define ARCH_NATURAL_BITNESS 32
	#elif defined __amd64__ || defined __amd64 || defined __x86_64__ || defined __x86_64
		#define ARCH_ID ARCH_x86_64
		#define ARCH_NATURAL_BITNESS 64
	#elif defined __arm__
		#define ARCH_ID ARCH_ARM_UNSPECIFIED
	#endif
#elif COMPILER_ID == COMPILER_CLANG
//TODO
#endif

#ifndef ARCH_ID
	#error Unknown configuration for this toolchain.
#endif

#ifndef ARCH_NATURAL_BITNESS
#error Architecture not supported.
#endif

#define ARCH_IS_LIKE_x86 (ARCH_ID > ARCH_x86_BEGIN && ARCH_ID < ARCH_x86_END)
#define ARCH_IS_ARM      (ARCH_ID > ARCH_ARM_BEGIN && ARCH_ID < ARCH_ARM_END)

#endif //ARCH_ID
