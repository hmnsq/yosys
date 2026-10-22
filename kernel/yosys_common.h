/* -*- c++ -*-
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef YOSYS_COMMON_H
#define YOSYS_COMMON_H

#अन्तर्भाव <array>
#अन्तर्भाव <atomic>
#अन्तर्भाव <map>
#अन्तर्भाव <set>
#अन्तर्भाव <tuple>
#अन्तर्भाव <vector>
#अन्तर्भाव <string>
#अन्तर्भाव <algorithm>
#अन्तर्भाव <functional>
#अन्तर्भाव <unordered_map>
#अन्तर्भाव <unordered_set>
#अन्तर्भाव <initializer_list>
#अन्तर्भाव <variant>
#अन्तर्भाव <optional>
#अन्तर्भाव <stdexcept>
#अन्तर्भाव <memory>
#अन्तर्भाव <cmath>
#अन्तर्भाव <cstddef>

#अन्तर्भाव <sstream>
#अन्तर्भाव <fstream>
#अन्तर्भाव <istream>
#अन्तर्भाव <ostream>
#अन्तर्भाव <iostream>

#अन्तर्भाव <version>
#यदि __cpp_lib_source_location >= 201907L
	#अन्तर्भाव <source_location>
	using std::source_location;
#अन्यथा
	#अन्तर्भाव <experimental/source_location>
	using std::experimental::source_location;
#endif

#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set प्रति your build configuration.
#endif

#अन्तर्भाव "kernel/yosys_config.h"

#define FRIEND_TEST(test_case_name, test_name) \
  friend class test_case_name##_##test_name##_Test

#ifdef _WIN32
#  undef NOMINMAX
#  define NOMINMAX 1
#  undef YY_NO_UNISTD_H
#  define YY_NO_UNISTD_H 1

#  include <windows.h>
#  include <io.h>
#  include <direct.h>

#  define strtok_r strtok_s
#  define strdup _strdup
#  define getcwd _getcwd
#  define mkdir _mkdir
#  define popen _popen
#  define pclose _pclose

#  ifndef __MINGW32__
#    define PATH_MAX MAX_PATH
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#endif

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif


#define YOSYS_NAMESPACE          Yosys
#define PRIVATE_NAMESPACE_BEGIN  namespace {
#define PRIVATE_NAMESPACE_END    }
#define YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#define YOSYS_NAMESPACE_END      }
#define YOSYS_NAMESPACE_PREFIX   Yosys::
#define USING_YOSYS_NAMESPACE    using namespace Yosys;

#यदि defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#नोचेद्यदि defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#अन्यथा
#  define YS_ATTRIBUTE(...)
#endif

#यदि defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#अन्यथा
#  define YS_MAYBE_UNUSED
#endif

#यदि defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#अन्यथा
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#यदि __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#अन्यथा
#  error "C++20 or later compatible compiler is required"
#endif

#यदि defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#अन्यथा
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#अन्यथा
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] शून्य log_yosys_abort_message(std::string_view file, पूर्णाङ्क पंक्ति, std::string_view func, std::string_view message);

[[noreturn]]
शून्य log_assert_failure(ध्रुव वर्ण *व्यंजक, source_location location);
#ifndef NDEBUG
स्थिर अन्तःस्थ शून्य log_assert_worker(bool cond, ध्रुव वर्ण *व्यंजक, source_location location = source_location::current()) {
	यदि (!cond) log_assert_failure(व्यंजक, location);
}
#  define log_assert(assert_व्यंजक) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_व्यंजक, #_assert_expr_)
#अन्यथा
#  define log_assert(assert_व्यंजक) कुरु { यदि (0) { (शून्य)(assert_व्यंजक); } } यावत्(0)
#endif
YOSYS_NAMESPACE_END

#अन्तर्भाव "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#अन्यथा
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#endif


using std::vector;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::run_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
संरचना shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ध्रुव वर्ण *s) { content = std::shared_ptr<string>(new string(s)); }
	ध्रुव वर्ण *c_str() ध्रुव { प्रत्यागम content->c_str(); }
	ध्रुव string &str() ध्रुव { प्रत्यागम *content; }
	bool operator==(ध्रुव shared_str &other) ध्रुव { प्रत्यागम *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		h.eat(*content);
		प्रत्यागम h;
	}
};

namespace RTLIL {
	संरचना Const;
	संरचना SigBit;
	संरचना SigSpec;
	संरचना Wire;
	संरचना Cell;
	संरचना Memory;
	संरचना Process;
	संरचना Module;
	संरचना Design;
	संरचना Monitor;
	संरचना Selection;
	संरचना SigChunk;
	संरचना PooledName;
	गणना State : unsigned वर्ण;

	प्रकारनिर्देश std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	संरचना Ast_गाँठ;
}

using RTLIL::Const;
using RTLIL::SigBit;
using RTLIL::SigSpec;
using RTLIL::Wire;
using RTLIL::Cell;
using RTLIL::Module;
using RTLIL::Design;

using RTLIL::State;
using RTLIL::SigChunk;
using RTLIL::SigSig;
using RTLIL::PooledName;

namespace hashlib {
	template<> संरचना hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> संरचना hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> संरचना hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> संरचना hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> संरचना hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> संरचना hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> संरचना hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> संरचना hash_ops<AST::Ast_गाँठ*> : hash_obj_ops {};

	template<> संरचना hash_ops<ध्रुव RTLIL::Wire*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव RTLIL::Cell*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव RTLIL::Memory*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव RTLIL::Process*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव RTLIL::Module*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव RTLIL::Design*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव RTLIL::Monitor*> : hash_obj_ops {};
	template<> संरचना hash_ops<ध्रुव AST::Ast_गाँठ*> : hash_obj_ops {};
}

शून्य memhasher_on();
शून्य memhasher_off();
शून्य memhasher_do();

बाह्य bool memhasher_active;
अन्तःस्थ शून्य memhasher() { यदि (memhasher_active) memhasher_do(); }

शून्य yosys_banner();
पूर्णाङ्क ceil_log2(पूर्णाङ्क x) YS_ATTRIBUTE(ध्रुव);

template<typename T> पूर्णाङ्क GetSize(ध्रुव T &obj) { प्रत्यागम obj.size(); }
अन्तःस्थ पूर्णाङ्क GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
संरचना Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	स्थिर bool active() { प्रत्यागम active_; }
गुप्त:
	स्थिर bool active_;
};

संरचना Autoidx {
	Autoidx(पूर्णाङ्क मूल्य) : मूल्य(मूल्य) {}
	operator पूर्णाङ्क() ध्रुव { प्रत्यागम मूल्य; }
	शून्य ensure_at_least(पूर्णाङ्क v);
	पूर्णाङ्क operator++(पूर्णाङ्क);
गुप्त:
	पूर्णाङ्क मूल्य;
};

बाह्य Autoidx autoidx;
बाह्य पूर्णाङ्क yosys_xtrace;
बाह्य bool yosys_write_versions;

ध्रुव std::string *create_id_prefix(std::string_view file, पूर्णाङ्क पंक्ति, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ध्रुव std::string * { \
		स्थिर std::unique_ptr<ध्रुव std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.पंक्ति(), func)); \
		प्रत्यागम prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ध्रुव std::string * { \
		स्थिर std::unique_ptr<ध्रुव std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.पंक्ति(), func)); \
		प्रत्यागम prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
