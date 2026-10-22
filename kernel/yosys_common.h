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
#만약 __cpp_lib_source_location >= 201907L
	#include <source_location>
	using std::source_location;
#아니면
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
         defines the Yosys Makefile would set 위해 your build configuration.
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

#만약 defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#elif defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#아니면
#  define YS_ATTRIBUTE(...)
#endif

#만약 defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#아니면
#  define YS_MAYBE_UNUSED
#endif

#만약 defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#아니면
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#만약 __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#아니면
#  error "C++20 or later compatible compiler is required"
#endif

#만약 defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#아니면
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#아니면
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] void log_yosys_abort_message(std::string_view file, 정수 line, std::string_view func, std::string_view message);

[[noreturn]]
void log_assert_failure(const char *expr, source_location location);
#ifndef NDEBUG
static inline void log_assert_worker(bool cond, const char *expr, source_location location = source_location::current()) {
	만약 (!cond) log_assert_failure(expr, location);
}
#  define log_assert(_assert_expr_) YOSYS_NAMESPACE_PREFIX log_assert_worker(_assert_expr_, #_assert_expr_)
#아니면
#  define log_assert(_assert_expr_) do { 만약 (0) { (void)(_assert_expr_); } } 동안(0)
#endif
YOSYS_NAMESPACE_END

#include "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#아니면
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
struct shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(const char *s) { content = std::shared_ptr<string>(new string(s)); }
	const char *c_str() const { 반환 content->c_str(); }
	const string &str() const { 반환 *content; }
	bool operator==(const shared_str &other) const { 반환 *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) const {
		h.eat(*content);
		반환 h;
	}
};

namespace RTLIL {
	struct Const;
	struct SigBit;
	struct SigSpec;
	struct Wire;
	struct Cell;
	struct Memory;
	struct Process;
	struct Module;
	struct Design;
	struct Monitor;
	struct Selection;
	struct SigChunk;
	struct PooledName;
	enum State : unsigned char;

	typedef std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	struct AstNode;
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
	template<> struct hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> struct hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> struct hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> struct hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> struct hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> struct hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> struct hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> struct hash_ops<AST::AstNode*> : hash_obj_ops {};

	template<> struct hash_ops<const RTLIL::Wire*> : hash_obj_ops {};
	template<> struct hash_ops<const RTLIL::Cell*> : hash_obj_ops {};
	template<> struct hash_ops<const RTLIL::Memory*> : hash_obj_ops {};
	template<> struct hash_ops<const RTLIL::Process*> : hash_obj_ops {};
	template<> struct hash_ops<const RTLIL::Module*> : hash_obj_ops {};
	template<> struct hash_ops<const RTLIL::Design*> : hash_obj_ops {};
	template<> struct hash_ops<const RTLIL::Monitor*> : hash_obj_ops {};
	template<> struct hash_ops<const AST::AstNode*> : hash_obj_ops {};
}

void memhasher_on();
void memhasher_off();
void memhasher_do();

extern bool memhasher_active;
inline void memhasher() { 만약 (memhasher_active) memhasher_do(); }

void yosys_banner();
정수 ceil_log2(정수 x) YS_ATTRIBUTE(const);

template<typename T> 정수 GetSize(const T &obj) { 반환 obj.size(); }
inline 정수 GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
struct Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	static bool active() { 반환 active_; }
private:
	static bool active_;
};

struct Autoidx {
	Autoidx(정수 value) : value(value) {}
	operator 정수() const { 반환 value; }
	void ensure_at_least(정수 v);
	정수 operator++(정수);
private:
	정수 value;
};

extern Autoidx autoidx;
extern 정수 yosys_xtrace;
extern bool yosys_write_versions;

const std::string *create_id_prefix(std::string_view file, 정수 line, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> const std::string * { \
		static std::unique_ptr<const std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.line(), func)); \
		반환 prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> const std::string * { \
		static std::unique_ptr<const std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.line(), func)); \
		반환 prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
