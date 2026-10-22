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

#അന്തര്ഭാവ <array>
#അന്തര്ഭാവ <atomic>
#അന്തര്ഭാവ <map>
#അന്തര്ഭാവ <set>
#അന്തര്ഭാവ <tuple>
#അന്തര്ഭാവ <vector>
#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <algorithm>
#അന്തര്ഭാവ <functional>
#അന്തര്ഭാവ <unordered_map>
#അന്തര്ഭാവ <unordered_set>
#അന്തര്ഭാവ <initializer_list>
#അന്തര്ഭാവ <variant>
#അന്തര്ഭാവ <optional>
#അന്തര്ഭാവ <stdexcept>
#അന്തര്ഭാവ <memory>
#അന്തര്ഭാവ <cmath>
#അന്തര്ഭാവ <cstddef>

#അന്തര്ഭാവ <sstream>
#അന്തര്ഭാവ <fstream>
#അന്തര്ഭാവ <istream>
#അന്തര്ഭാവ <ostream>
#അന്തര്ഭാവ <iostream>

#അന്തര്ഭാവ <version>
#എങ്കിൽ __cpp_lib_source_location >= 201907L
	#അന്തര്ഭാവ <source_location>
	using std::source_location;
#അല്ലെങ്കിൽ
	#അന്തര്ഭാവ <experimental/source_location>
	using std::experimental::source_location;
#endif

#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set വേണ്ടി your build configuration.
#endif

#അന്തര്ഭാവ "kernel/yosys_config.h"

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

#എങ്കിൽ defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#അല്ലെങ്കിലെങ്കിൽ defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#അല്ലെങ്കിൽ
#  define YS_ATTRIBUTE(...)
#endif

#എങ്കിൽ defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#അല്ലെങ്കിൽ
#  define YS_MAYBE_UNUSED
#endif

#എങ്കിൽ defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#അല്ലെങ്കിൽ
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#എങ്കിൽ __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#അല്ലെങ്കിൽ
#  error "C++20 or later compatible compiler is required"
#endif

#എങ്കിൽ defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#അല്ലെങ്കിൽ
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#അല്ലെങ്കിൽ
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] ശൂന്യം log_yosys_abort_message(std::string_view file, പൂർണ്ണസംഖ്യ വരി, std::string_view func, std::string_view message);

[[noreturn]]
ശൂന്യം log_assert_failure(സ്ഥിരം അക്ഷരം *വ്യഞ്ജകം, source_location location);
#ifndef NDEBUG
സ്ഥിതം വരിയിൽ ശൂന്യം log_assert_worker(bool cond, സ്ഥിരം അക്ഷരം *വ്യഞ്ജകം, source_location location = source_location::current()) {
	എങ്കിൽ (!cond) log_assert_failure(വ്യഞ്ജകം, location);
}
#  define log_assert(assert_വ്യഞ്ജകം) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_വ്യഞ്ജകം, #_assert_expr_)
#അല്ലെങ്കിൽ
#  define log_assert(assert_വ്യഞ്ജകം) ചെയ്യുക { എങ്കിൽ (0) { (ശൂന്യം)(assert_വ്യഞ്ജകം); } } ആകുമ്പോൾ(0)
#endif
YOSYS_NAMESPACE_END

#അന്തര്ഭാവ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#അല്ലെങ്കിൽ
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
ഘടന shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(സ്ഥിരം അക്ഷരം *s) { content = std::shared_ptr<string>(new string(s)); }
	സ്ഥിരം അക്ഷരം *c_str() സ്ഥിരം { തിരികെ content->c_str(); }
	സ്ഥിരം string &str() സ്ഥിരം { തിരികെ *content; }
	bool operator==(സ്ഥിരം shared_str &other) സ്ഥിരം { തിരികെ *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
		h.eat(*content);
		തിരികെ h;
	}
};

namespace RTLIL {
	ഘടന Const;
	ഘടന SigBit;
	ഘടന SigSpec;
	ഘടന Wire;
	ഘടന Cell;
	ഘടന Memory;
	ഘടന Process;
	ഘടന Module;
	ഘടന Design;
	ഘടന Monitor;
	ഘടന Selection;
	ഘടന SigChunk;
	ഘടന PooledName;
	എണ്ണൽ State : unsigned അക്ഷരം;

	തരനിർവചനം std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ഘടന Ast_കണ്ണി;
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
	template<> ഘടന hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> ഘടന hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> ഘടന hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> ഘടന hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> ഘടന hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> ഘടന hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> ഘടന hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> ഘടന hash_ops<AST::Ast_കണ്ണി*> : hash_obj_ops {};

	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Wire*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Cell*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Memory*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Process*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Module*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Design*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം RTLIL::Monitor*> : hash_obj_ops {};
	template<> ഘടന hash_ops<സ്ഥിരം AST::Ast_കണ്ണി*> : hash_obj_ops {};
}

ശൂന്യം memhasher_on();
ശൂന്യം memhasher_off();
ശൂന്യം memhasher_do();

ബാഹ്യ bool memhasher_active;
വരിയിൽ ശൂന്യം memhasher() { എങ്കിൽ (memhasher_active) memhasher_do(); }

ശൂന്യം yosys_banner();
പൂർണ്ണസംഖ്യ ceil_log2(പൂർണ്ണസംഖ്യ x) YS_ATTRIBUTE(സ്ഥിരം);

template<typename T> പൂർണ്ണസംഖ്യ GetSize(സ്ഥിരം T &obj) { തിരികെ obj.size(); }
വരിയിൽ പൂർണ്ണസംഖ്യ GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ഘടന Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	സ്ഥിതം bool active() { തിരികെ active_; }
സ്വകാര്യ:
	സ്ഥിതം bool active_;
};

ഘടന Autoidx {
	Autoidx(പൂർണ്ണസംഖ്യ മൂല്യം) : മൂല്യം(മൂല്യം) {}
	operator പൂർണ്ണസംഖ്യ() സ്ഥിരം { തിരികെ മൂല്യം; }
	ശൂന്യം ensure_at_least(പൂർണ്ണസംഖ്യ v);
	പൂർണ്ണസംഖ്യ operator++(പൂർണ്ണസംഖ്യ);
സ്വകാര്യ:
	പൂർണ്ണസംഖ്യ മൂല്യം;
};

ബാഹ്യ Autoidx autoidx;
ബാഹ്യ പൂർണ്ണസംഖ്യ yosys_xtrace;
ബാഹ്യ bool yosys_write_versions;

സ്ഥിരം std::string *create_id_prefix(std::string_view file, പൂർണ്ണസംഖ്യ വരി, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> സ്ഥിരം std::string * { \
		സ്ഥിതം std::unique_ptr<സ്ഥിരം std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.വരി(), func)); \
		തിരികെ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> സ്ഥിരം std::string * { \
		സ്ഥിതം std::unique_ptr<സ്ഥിരം std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.വരി(), func)); \
		തിരികെ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
