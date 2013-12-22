#include "config.h"
#include <boost/cstdint.hpp>
#include <boost/type_traits/make_signed.hpp>

#define DECLARE_MACHINE_WORD_helper(x) boost::uint##x##_t
#define DECLARE_MACHINE_WORD(x) typedef DECLARE_MACHINE_WORD_helper(x) mword_t
DECLARE_MACHINE_WORD(ARCH_NATURAL_BITNESS);

typedef boost::make_signed<mword_t>::type smword_t;
