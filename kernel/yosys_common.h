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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_COMMON_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_COMMON_H

#ਅਨ੍ਤਰ੍ਭਾਵ <array>
#ਅਨ੍ਤਰ੍ਭਾਵ <atomic>
#ਅਨ੍ਤਰ੍ਭਾਵ <map>
#ਅਨ੍ਤਰ੍ਭਾਵ <set>
#ਅਨ੍ਤਰ੍ਭਾਵ <tuple>
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <functional>
#ਅਨ੍ਤਰ੍ਭਾਵ <unordered_map>
#ਅਨ੍ਤਰ੍ਭਾਵ <unordered_set>
#ਅਨ੍ਤਰ੍ਭਾਵ <initializer_list>
#ਅਨ੍ਤਰ੍ਭਾਵ <variant>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdexcept>
#ਅਨ੍ਤਰ੍ਭਾਵ <memory>
#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>

#ਅਨ੍ਤਰ੍ਭਾਵ <sstream>
#ਅਨ੍ਤਰ੍ਭਾਵ <fstream>
#ਅਨ੍ਤਰ੍ਭਾਵ <istream>
#ਅਨ੍ਤਰ੍ਭਾਵ <ostream>
#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>

#ਅਨ੍ਤਰ੍ਭਾਵ <version>
#ਜੇ __cpp_lib_source_location >= 201907L
	#ਅਨ੍ਤਰ੍ਭਾਵ <source_location>
	using std::ਸਰੋਤ_location;
#ਨਹੀਂਤਾਂ
	#ਅਨ੍ਤਰ੍ਭਾਵ <experimental/source_location>
	using std::experimental::ਸਰੋਤ_location;
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _YOSYS_
#  error It looks like you are trying ਮੰਜ਼ਿਲ build Yosys without the config ਨਿਰਧਾਰਤ_ਕਰਦਾ ਸੈੱਟ. \
         When building Yosys with a custom make system, make sure you ਸੈੱਟ all the \
         ਨਿਰਧਾਰਤ_ਕਰਦਾ the Yosys Makefile would ਸੈੱਟ ਲਈ your build configuration.
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_config.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRIEND_TEST(test_case_name, test_name) \
  ਮਿੱਤਰ class test_case_name##_##test_name##_Test

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
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
#    define ਰਸਤਾ_MAX MAX_ਰਸਤਾ
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਰਸਤਾ_MAX
#  define ਰਸਤਾ_MAX 4096
#ਜੇ_ਅੰਤ


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_NAMESPACE          Yosys
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿੱਜੀ_NAMESPACE_BEGIN  namespace {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿੱਜੀ_NAMESPACE_ਸਿਰਾ    }
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_NAMESPACE_ਸਿਰਾ      }
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_NAMESPACE_ਅਗੇਤਰ   Yosys::
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_YOSYS_NAMESPACE    using namespace Yosys;

#ਜੇ defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#ਜਾਂਜੇ defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#ਨਹੀਂਤਾਂ
#  define YS_ATTRIBUTE(...)
#ਜੇ_ਅੰਤ

#ਜੇ defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_ਅਣਵਰਤਿਆ __attribute__((__unused__))
#ਨਹੀਂਤਾਂ
#  define YS_MAYBE_ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

#ਜੇ defined(_MSC_VER)
#  define YS_NO_UNIQUE_ਪਤਾ [[msvc::no_unique_ਪਤਾ]]
#ਨਹੀਂਤਾਂ
#  define YS_NO_UNIQUE_ਪਤਾ [[no_unique_ਪਤਾ]]
#ਜੇ_ਅੰਤ

#ਜੇ __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#ਨਹੀਂਤਾਂ
#  error "C++20 or later compatible compiler is required"
#ਜੇ_ਅੰਤ

#ਜੇ defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#ਨਹੀਂਤਾਂ
#  define YS_COLD
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cpp_consteval
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_CONSTEVAL consteval
#ਨਹੀਂਤਾਂ
// If we can't use consteval we can at least make it constexpr.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_CONSTEVAL constexpr
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_ABORT(s) YOSYS_NAMESPACE_ਅਗੇਤਰ log_yosys_abort_ਸੁਨੇਹਾ(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] ਖਾਲੀ log_yosys_abort_ਸੁਨੇਹਾ(std::string_view file, ਪੂਰਨ ਸਤਰ, std::string_view func, std::string_view ਸੁਨੇਹਾ_2);

[[noreturn]]
ਖਾਲੀ log_assert_failure(ਅਚਲ ਅੱਖਰ *ਵਿਅੰਜਕ, ਸਰੋਤ_location location);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_assert_ਕਾਮਾ(bool cond, ਅਚਲ ਅੱਖਰ *ਵਿਅੰਜਕ, ਸਰੋਤ_location location = ਸਰੋਤ_location::current()) {
	ਜੇ (!cond) log_assert_failure(ਵਿਅੰਜਕ, location);
}
#  define log_assert(assert_ਵਿਅੰਜਕ) YOSYS_NAMESPACE_ਅਗੇਤਰ log_assert_ਕਾਮਾ(assert_ਵਿਅੰਜਕ, #_assert_expr_)
#ਨਹੀਂਤਾਂ
#  define log_assert(assert_ਵਿਅੰਜਕ) ਕਰੋ { ਜੇ (0) { (ਖਾਲੀ)(assert_ਵਿਅੰਜਕ); } } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ
YOSYS_NAMESPACE_ਸਿਰਾ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#ਨਹੀਂਤਾਂ
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#ਜੇ_ਅੰਤ


using std::ਸਦਿਸ਼;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::ਲੜੀਵਾਰ_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
ਬਣਤਰ shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ਅਚਲ ਅੱਖਰ *s) { content = std::shared_ptr<string>(new string(s)); }
	ਅਚਲ ਅੱਖਰ *c_str() ਅਚਲ { ਵਾਪਸ content->c_str(); }
	ਅਚਲ string &str() ਅਚਲ { ਵਾਪਸ *content; }
	bool ਸੰਚਾਲਕ==(ਅਚਲ shared_str &other) ਅਚਲ { ਵਾਪਸ *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
		h.eat(*content);
		ਵਾਪਸ h;
	}
};

namespace RTLIL {
	ਬਣਤਰ Const;
	ਬਣਤਰ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਬਣਤਰ SigSpec;
	ਬਣਤਰ Wire;
	ਬਣਤਰ Cell;
	ਬਣਤਰ ਸਮ੍ਰਿਤੀ;
	ਬਣਤਰ Process;
	ਬਣਤਰ Module;
	ਬਣਤਰ Design;
	ਬਣਤਰ Monitor;
	ਬਣਤਰ Selection;
	ਬਣਤਰ SigChunk;
	ਬਣਤਰ PooledName;
	ਗਿਣਤੀ State : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ;

	ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ਬਣਤਰ Ast_ਗੰਢ;
}

using RTLIL::Const;
using RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ;
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
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::ਸਮ੍ਰਿਤੀ*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::Process*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::Module*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::Design*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<AST::Ast_ਗੰਢ*> : hash_obj_ops {};

	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::Wire*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::Cell*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::ਸਮ੍ਰਿਤੀ*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::Process*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::Module*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::Design*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ RTLIL::Monitor*> : hash_obj_ops {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<ਅਚਲ AST::Ast_ਗੰਢ*> : hash_obj_ops {};
}

ਖਾਲੀ memhasher_on();
ਖਾਲੀ memhasher_off();
ਖਾਲੀ memhasher_do();

ਬਾਹਰੀ bool memhasher_ਸਰਗਰਮ;
ਸਤਰਅੰਦਰ ਖਾਲੀ memhasher() { ਜੇ (memhasher_ਸਰਗਰਮ) memhasher_do(); }

ਖਾਲੀ yosys_banner();
ਪੂਰਨ ceil_log2(ਪੂਰਨ x) YS_ATTRIBUTE(ਅਚਲ);

ਸਾਂਚਾ<typename T> ਪੂਰਨ Get_ਮਾਪ(ਅਚਲ T &obj) { ਵਾਪਸ obj.ਮਾਪ(); }
ਸਤਰਅੰਦਰ ਪੂਰਨ Get_ਮਾਪ(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ਬਣਤਰ Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ਸਥਿਰ bool ਸਰਗਰਮ() { ਵਾਪਸ ਸਰਗਰਮ_2; }
ਨਿਜੀ:
	ਸਥਿਰ bool ਸਰਗਰਮ_2;
};

ਬਣਤਰ Autoidx {
	Autoidx(ਪੂਰਨ ਮੁੱਲ) : ਮੁੱਲ(ਮੁੱਲ) {}
	ਸੰਚਾਲਕ ਪੂਰਨ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ; }
	ਖਾਲੀ ensure_at_ਘੱਟ_ਤੋਂ_ਘੱਟ(ਪੂਰਨ v);
	ਪੂਰਨ ਸੰਚਾਲਕ++(ਪੂਰਨ);
ਨਿਜੀ:
	ਪੂਰਨ ਮੁੱਲ;
};

ਬਾਹਰੀ Autoidx autoidx;
ਬਾਹਰੀ ਪੂਰਨ yosys_xtrace;
ਬਾਹਰੀ bool yosys_write_versions;

ਅਚਲ std::string *create_id_ਅਗੇਤਰ(std::string_view file, ਪੂਰਨ ਸਤਰ, std::string_view func);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_ID \
	YOSYS_NAMESPACE_ਅਗੇਤਰ TwineSpec{YOSYS_NAMESPACE_ਅਗੇਤਰ TwineSpec::AutoSuffix{[](ਸਰੋਤ_location location, std::string_view func) -> ਅਚਲ std::string * { \
		ਸਥਿਰ std::unique_ptr<ਅਚਲ std::string> ਅਗੇਤਰ_2(YOSYS_NAMESPACE_ਅਗੇਤਰ create_id_ਅਗੇਤਰ(location.file_name(), location.ਸਤਰ(), func)); \
		ਵਾਪਸ ਅਗੇਤਰ_2.get(); \
	}(ਸਰੋਤ_location::current(), __FUNCTION__), std::ਮੰਜ਼ਿਲ_string(YOSYS_NAMESPACE_ਅਗੇਤਰ autoidx++)}}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_ਅਗੇਤਰ TwineSpec{YOSYS_NAMESPACE_ਅਗੇਤਰ TwineSpec::AutoSuffix{[](ਸਰੋਤ_location location, std::string_view func) -> ਅਚਲ std::string * { \
		ਸਥਿਰ std::unique_ptr<ਅਚਲ std::string> ਅਗੇਤਰ_2(YOSYS_NAMESPACE_ਅਗੇਤਰ create_id_ਅਗੇਤਰ(location.file_name(), location.ਸਤਰ(), func)); \
		ਵਾਪਸ ਅਗੇਤਰ_2.get(); \
	}(ਸਰੋਤ_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::ਮੰਜ਼ਿਲ_string(YOSYS_NAMESPACE_ਅਗੇਤਰ autoidx++)}}

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
