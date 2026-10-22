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

#ଅନ୍ତର୍ଭାଵ <array>
#ଅନ୍ତର୍ଭାଵ <atomic>
#ଅନ୍ତର୍ଭାଵ <map>
#ଅନ୍ତର୍ଭାଵ <set>
#ଅନ୍ତର୍ଭାଵ <tuple>
#ଅନ୍ତର୍ଭାଵ <vector>
#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <algorithm>
#ଅନ୍ତର୍ଭାଵ <functional>
#ଅନ୍ତର୍ଭାଵ <unordered_map>
#ଅନ୍ତର୍ଭାଵ <unordered_set>
#ଅନ୍ତର୍ଭାଵ <initializer_list>
#ଅନ୍ତର୍ଭାଵ <variant>
#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <stdexcept>
#ଅନ୍ତର୍ଭାଵ <memory>
#ଅନ୍ତର୍ଭାଵ <cmath>
#ଅନ୍ତର୍ଭାଵ <cstddef>

#ଅନ୍ତର୍ଭାଵ <sstream>
#ଅନ୍ତର୍ଭାଵ <fstream>
#ଅନ୍ତର୍ଭାଵ <istream>
#ଅନ୍ତର୍ଭାଵ <ostream>
#ଅନ୍ତର୍ଭାଵ <iostream>

#ଅନ୍ତର୍ଭାଵ <version>
#ଯଦି __cpp_lib_source_location >= 201907L
	#ଅନ୍ତର୍ଭାଵ <source_location>
	using std::source_location;
#ଅନ୍ୟଥା
	#ଅନ୍ତର୍ଭାଵ <experimental/source_location>
	using std::experimental::source_location;
#endif

#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set ପାଇଁ your build configuration.
#endif

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_config.h"

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

#ଯଦି defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#ନଚେତଯଦି defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#ଅନ୍ୟଥା
#  define YS_ATTRIBUTE(...)
#endif

#ଯଦି defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#ଅନ୍ୟଥା
#  define YS_MAYBE_UNUSED
#endif

#ଯଦି defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#ଅନ୍ୟଥା
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#ଯଦି __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#ଅନ୍ୟଥା
#  error "C++20 or later compatible compiler is required"
#endif

#ଯଦି defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#ଅନ୍ୟଥା
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#ଅନ୍ୟଥା
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] ଶୂନ୍ୟ log_yosys_abort_message(std::string_view file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, std::string_view func, std::string_view message);

[[noreturn]]
ଶୂନ୍ୟ log_assert_failure(ଅଚଳ ଅକ୍ଷର *ପଦସମଷ୍ଟି, source_location location);
#ifndef NDEBUG
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_assert_worker(bool cond, ଅଚଳ ଅକ୍ଷର *ପଦସମଷ୍ଟି, source_location location = source_location::current()) {
	ଯଦି (!cond) log_assert_failure(ପଦସମଷ୍ଟି, location);
}
#  define log_assert(assert_ପଦସମଷ୍ଟି) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_ପଦସମଷ୍ଟି, #_assert_expr_)
#ଅନ୍ୟଥା
#  define log_assert(assert_ପଦସମଷ୍ଟି) କର { ଯଦି (0) { (ଶୂନ୍ୟ)(assert_ପଦସମଷ୍ଟି); } } ଯେତେବେଳେ(0)
#endif
YOSYS_NAMESPACE_END

#ଅନ୍ତର୍ଭାଵ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#ଅନ୍ୟଥା
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
ସଂରଚନା shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ଅଚଳ ଅକ୍ଷର *s) { content = std::shared_ptr<string>(new string(s)); }
	ଅଚଳ ଅକ୍ଷର *c_str() ଅଚଳ { ଫେରନ୍ତୁ content->c_str(); }
	ଅଚଳ string &str() ଅଚଳ { ଫେରନ୍ତୁ *content; }
	bool operator==(ଅଚଳ shared_str &other) ଅଚଳ { ଫେରନ୍ତୁ *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
		h.eat(*content);
		ଫେରନ୍ତୁ h;
	}
};

namespace RTLIL {
	ସଂରଚନା Const;
	ସଂରଚନା SigBit;
	ସଂରଚନା SigSpec;
	ସଂରଚନା Wire;
	ସଂରଚନା Cell;
	ସଂରଚନା Memory;
	ସଂରଚନା Process;
	ସଂରଚନା Module;
	ସଂରଚନା Design;
	ସଂରଚନା Monitor;
	ସଂରଚନା Selection;
	ସଂରଚନା SigChunk;
	ସଂରଚନା PooledName;
	ଗଣନା State : unsigned ଅକ୍ଷର;

	ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ସଂରଚନା Ast_ଗଣ୍ଠି;
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
	template<> ସଂରଚନା hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<AST::Ast_ଗଣ୍ଠି*> : hash_obj_ops {};

	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Wire*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Cell*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Memory*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Process*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Module*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Design*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ RTLIL::Monitor*> : hash_obj_ops {};
	template<> ସଂରଚନା hash_ops<ଅଚଳ AST::Ast_ଗଣ୍ଠି*> : hash_obj_ops {};
}

ଶୂନ୍ୟ memhasher_on();
ଶୂନ୍ୟ memhasher_off();
ଶୂନ୍ୟ memhasher_do();

ବାହ୍ୟ bool memhasher_active;
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ memhasher() { ଯଦି (memhasher_active) memhasher_do(); }

ଶୂନ୍ୟ yosys_banner();
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ceil_log2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) YS_ATTRIBUTE(ଅଚଳ);

template<typename T> ପୂର୍ଣ୍ଣସଂଖ୍ୟା GetSize(ଅଚଳ T &obj) { ଫେରନ୍ତୁ obj.size(); }
ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ସଂରଚନା Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ସ୍ଥିର bool active() { ଫେରନ୍ତୁ active_; }
ବ୍ୟକ୍ତିଗତ:
	ସ୍ଥିର bool active_;
};

ସଂରଚନା Autoidx {
	Autoidx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ) : ମୂଲ୍ୟ(ମୂଲ୍ୟ) {}
	operator ପୂର୍ଣ୍ଣସଂଖ୍ୟା() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ; }
	ଶୂନ୍ୟ ensure_at_least(ପୂର୍ଣ୍ଣସଂଖ୍ୟା v);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା operator++(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ବ୍ୟକ୍ତିଗତ:
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;
};

ବାହ୍ୟ Autoidx autoidx;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା yosys_xtrace;
ବାହ୍ୟ bool yosys_write_versions;

ଅଚଳ std::string *create_id_prefix(std::string_view file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ଅଚଳ std::string * { \
		ସ୍ଥିର std::unique_ptr<ଅଚଳ std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.ଧାଡ଼ି(), func)); \
		ଫେରନ୍ତୁ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ଅଚଳ std::string * { \
		ସ୍ଥିର std::unique_ptr<ଅଚଳ std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.ଧାଡ଼ି(), func)); \
		ଫେରନ୍ତୁ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
