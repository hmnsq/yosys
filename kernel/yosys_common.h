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

#અન્તર્ભાવ <array>
#અન્તર્ભાવ <atomic>
#અન્તર્ભાવ <map>
#અન્તર્ભાવ <set>
#અન્તર્ભાવ <tuple>
#અન્તર્ભાવ <vector>
#અન્તર્ભાવ <string>
#અન્તર્ભાવ <algorithm>
#અન્તર્ભાવ <functional>
#અન્તર્ભાવ <unordered_map>
#અન્તર્ભાવ <unordered_set>
#અન્તર્ભાવ <initializer_list>
#અન્તર્ભાવ <variant>
#અન્તર્ભાવ <optional>
#અન્તર્ભાવ <stdexcept>
#અન્તર્ભાવ <memory>
#અન્તર્ભાવ <cmath>
#અન્તર્ભાવ <cstddef>

#અન્તર્ભાવ <sstream>
#અન્તર્ભાવ <fstream>
#અન્તર્ભાવ <istream>
#અન્તર્ભાવ <ostream>
#અન્તર્ભાવ <iostream>

#અન્તર્ભાવ <version>
#જો __cpp_lib_source_location >= 201907L
	#અન્તર્ભાવ <source_location>
	using std::source_location;
#નહીંતર
	#અન્તર્ભાવ <experimental/source_location>
	using std::experimental::source_location;
#endif

#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set માટે your build configuration.
#endif

#અન્તર્ભાવ "kernel/yosys_config.h"

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

#જો defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#અથવાજો defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#નહીંતર
#  define YS_ATTRIBUTE(...)
#endif

#જો defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#નહીંતર
#  define YS_MAYBE_UNUSED
#endif

#જો defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#નહીંતર
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#જો __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#નહીંતર
#  error "C++20 or later compatible compiler is required"
#endif

#જો defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#નહીંતર
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#નહીંતર
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] શૂન્ય log_yosys_abort_message(std::string_view file, પૂર્ણાંક લીટી, std::string_view func, std::string_view message);

[[noreturn]]
શૂન્ય log_assert_failure(અચલ અક્ષર *પદાવલિ, source_location location);
#ifndef NDEBUG
સ્થિર અંતઃરેખ શૂન્ય log_assert_worker(bool cond, અચલ અક્ષર *પદાવલિ, source_location location = source_location::current()) {
	જો (!cond) log_assert_failure(પદાવલિ, location);
}
#  define log_assert(assert_પદાવલિ) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_પદાવલિ, #_assert_expr_)
#નહીંતર
#  define log_assert(assert_પદાવલિ) કરો { જો (0) { (શૂન્ય)(assert_પદાવલિ); } } જ્યારે(0)
#endif
YOSYS_NAMESPACE_END

#અન્તર્ભાવ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#નહીંતર
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
સંરચના shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(અચલ અક્ષર *s) { content = std::shared_ptr<string>(new string(s)); }
	અચલ અક્ષર *c_str() અચલ { પાછા content->c_str(); }
	અચલ string &str() અચલ { પાછા *content; }
	bool operator==(અચલ shared_str &other) અચલ { પાછા *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
		h.eat(*content);
		પાછા h;
	}
};

namespace RTLIL {
	સંરચના Const;
	સંરચના SigBit;
	સંરચના SigSpec;
	સંરચના Wire;
	સંરચના Cell;
	સંરચના Memory;
	સંરચના Process;
	સંરચના Module;
	સંરચના Design;
	સંરચના Monitor;
	સંરચના Selection;
	સંરચના SigChunk;
	સંરચના PooledName;
	ગણના State : unsigned અક્ષર;

	પ્રકારવ્યાખ્યા std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	સંરચના Ast_ગાંઠ;
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
	template<> સંરચના hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> સંરચના hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> સંરચના hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> સંરચના hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> સંરચના hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> સંરચના hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> સંરચના hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> સંરચના hash_ops<AST::Ast_ગાંઠ*> : hash_obj_ops {};

	template<> સંરચના hash_ops<અચલ RTLIL::Wire*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ RTLIL::Cell*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ RTLIL::Memory*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ RTLIL::Process*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ RTLIL::Module*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ RTLIL::Design*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ RTLIL::Monitor*> : hash_obj_ops {};
	template<> સંરચના hash_ops<અચલ AST::Ast_ગાંઠ*> : hash_obj_ops {};
}

શૂન્ય memhasher_on();
શૂન્ય memhasher_off();
શૂન્ય memhasher_do();

બાહ્ય bool memhasher_active;
અંતઃરેખ શૂન્ય memhasher() { જો (memhasher_active) memhasher_do(); }

શૂન્ય yosys_banner();
પૂર્ણાંક ceil_log2(પૂર્ણાંક x) YS_ATTRIBUTE(અચલ);

template<typename T> પૂર્ણાંક GetSize(અચલ T &obj) { પાછા obj.size(); }
અંતઃરેખ પૂર્ણાંક GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
સંરચના Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	સ્થિર bool active() { પાછા active_; }
ખાનગી:
	સ્થિર bool active_;
};

સંરચના Autoidx {
	Autoidx(પૂર્ણાંક કિંમત) : કિંમત(કિંમત) {}
	operator પૂર્ણાંક() અચલ { પાછા કિંમત; }
	શૂન્ય ensure_at_least(પૂર્ણાંક v);
	પૂર્ણાંક operator++(પૂર્ણાંક);
ખાનગી:
	પૂર્ણાંક કિંમત;
};

બાહ્ય Autoidx autoidx;
બાહ્ય પૂર્ણાંક yosys_xtrace;
બાહ્ય bool yosys_write_versions;

અચલ std::string *create_id_prefix(std::string_view file, પૂર્ણાંક લીટી, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> અચલ std::string * { \
		સ્થિર std::unique_ptr<અચલ std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.લીટી(), func)); \
		પાછા prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> અચલ std::string * { \
		સ્થિર std::unique_ptr<અચલ std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.લીટી(), func)); \
		પાછા prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
