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

#include <array>
#include <atomic>
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <variant>
#include <optional>
#include <stdexcept>
#include <memory>
#include <cmath>
#include <cstddef>

#include <sstream>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <version>
#ᯀᯮᯝ __cpp_lib_source_location >= 201907L
	#include <source_location>
	using std::source_location;
#ᯀᯮᯝᯂᯪ
	#include <experimental/source_location>
	using std::experimental::source_location;
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set for your build configuration.
#endif

#include "kernel/yosys_config.h"

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

#ᯀᯮᯝ defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#elif defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#ᯀᯮᯝᯂᯪ
#  define YS_ATTRIBUTE(...)
#endif

#ᯀᯮᯝ defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#ᯀᯮᯝᯂᯪ
#  define YS_MAYBE_UNUSED
#endif

#ᯀᯮᯝ defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#ᯀᯮᯝᯂᯪ
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#ᯀᯮᯝ __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#ᯀᯮᯝᯂᯪ
#  error "C++20 or later compatible compiler is required"
#endif

#ᯀᯮᯝ defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#ᯀᯮᯝᯂᯪ
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#ᯀᯮᯝᯂᯪ
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] void log_yosys_abort_message(std::string_view file, puur_naa_nka line, std::string_view func, std::string_view message);

[[noreturn]]
void log_assert_failure(const var_na *expr, source_location location);
#ifndef NDEBUG
static anta_hstha void log_assert_worker(bool cond, const var_na *expr, source_location location = source_location::current()) {
	ᯀᯮᯝ (!cond) log_assert_failure(expr, location);
}
#  define log_assert(_assert_expr_) YOSYS_NAMESPACE_PREFIX log_assert_worker(_assert_expr_, #_assert_expr_)
#ᯀᯮᯝᯂᯪ
#  define log_assert(_assert_expr_) do { ᯀᯮᯝ (0) { (void)(_assert_expr_); } } ᯀᯬᯝ(0)
#endif
YOSYS_NAMESPACE_END

#include "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#ᯀᯮᯝᯂᯪ
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
sa_nracanaa shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(const var_na *s) { content = std::shared_ptr<string>(new string(s)); }
	const var_na *c_str() const { ᯀᯮᯞ content->c_str(); }
	const string &str() const { ᯀᯮᯞ *content; }
	bool operator==(const shared_str &other) const { ᯀᯮᯞ *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) const {
		h.eat(*content);
		ᯀᯮᯞ h;
	}
};

namespace RTLIL {
	sa_nracanaa Const;
	sa_nracanaa SigBit;
	sa_nracanaa SigSpec;
	sa_nracanaa Wire;
	sa_nracanaa Cell;
	sa_nracanaa Memory;
	sa_nracanaa Process;
	sa_nracanaa Module;
	sa_nracanaa Design;
	sa_nracanaa Monitor;
	sa_nracanaa Selection;
	sa_nracanaa SigChunk;
	sa_nracanaa PooledName;
	ga_nanaa State : unsigned var_na;

	typedef std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	sa_nracanaa AstNode;
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
	template<> sa_nracanaa hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<AST::AstNode*> : hash_obj_ops {};

	template<> sa_nracanaa hash_ops<const RTLIL::Wire*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const RTLIL::Cell*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const RTLIL::Memory*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const RTLIL::Process*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const RTLIL::Module*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const RTLIL::Design*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const RTLIL::Monitor*> : hash_obj_ops {};
	template<> sa_nracanaa hash_ops<const AST::AstNode*> : hash_obj_ops {};
}

void memhasher_on();
void memhasher_off();
void memhasher_do();

extern bool memhasher_active;
anta_hstha void memhasher() { ᯀᯮᯝ (memhasher_active) memhasher_do(); }

void yosys_banner();
puur_naa_nka ceil_log2(puur_naa_nka x) YS_ATTRIBUTE(const);

template<typename T> puur_naa_nka GetSize(const T &obj) { ᯀᯮᯞ obj.size(); }
anta_hstha puur_naa_nka GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
sa_nracanaa Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	static bool active() { ᯀᯮᯞ active_; }
private:
	static bool active_;
};

sa_nracanaa Autoidx {
	Autoidx(puur_naa_nka value) : value(value) {}
	operator puur_naa_nka() const { ᯀᯮᯞ value; }
	void ensure_at_least(puur_naa_nka v);
	puur_naa_nka operator++(puur_naa_nka);
private:
	puur_naa_nka value;
};

extern Autoidx autoidx;
extern puur_naa_nka yosys_xtrace;
extern bool yosys_write_versions;

const std::string *create_id_prefix(std::string_view file, puur_naa_nka line, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> const std::string * { \
		static std::unique_ptr<const std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.line(), func)); \
		ᯀᯮᯞ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> const std::string * { \
		static std::unique_ptr<const std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.line(), func)); \
		ᯀᯮᯞ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
