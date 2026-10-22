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

#که_نه_تعریف_شوی YOSYS_COMMON_H
#تعریف_کړه YOSYS_COMMON_H

#ضمن <array>
#ضمن <atomic>
#ضمن <map>
#ضمن <set>
#ضمن <tuple>
#ضمن <vector>
#ضمن <string>
#ضمن <algorithm>
#ضمن <functional>
#ضمن <unordered_map>
#ضمن <unordered_set>
#ضمن <initializer_list>
#ضمن <variant>
#ضمن <optional>
#ضمن <stdexcept>
#ضمن <memory>
#ضمن <cmath>
#ضمن <cstddef>

#ضمن <sstream>
#ضمن <fstream>
#ضمن <istream>
#ضمن <ostream>
#ضمن <iostream>

#ضمن <version>
#إذا __cpp_lib_source_location >= 201907L
	#ضمن <source_location>
	using std::سرچینه_location;
#وإلا
	#ضمن <experimental/source_location>
	using std::experimental::سرچینه_location;
#د_که_پای

#ضمن <stdarg.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <stdio.h>
#ضمن <limits.h>
#ضمن <sys/stat.h>
#ضمن <errno.h>

#که_نه_تعریف_شوی _YOSYS_
#  error It looks like you are trying منزل build Yosys without the config تعریفوي سېټ. \
         When building Yosys with a custom make system, make sure you سېټ all the \
         تعریفوي the Yosys Makefile would سېټ لكل your build configuration.
#د_که_پای

#ضمن "kernel/yosys_config.h"

#تعریف_کړه FRIEND_TEST(test_case_name, test_name) \
  ملګری class test_case_name##_##test_name##_Test

#که_تعریف_شوی _WIN32
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
#    define لار_MAX MAX_لار
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#د_که_پای

#که_نه_تعریف_شوی لار_MAX
#  define لار_MAX 4096
#د_که_پای


#تعریف_کړه YOSYS_NAMESPACE          Yosys
#تعریف_کړه شخصي_NAMESPACE_BEGIN  namespace {
#تعریف_کړه شخصي_NAMESPACE_پای_ټکی    }
#تعریف_کړه YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#تعریف_کړه YOSYS_NAMESPACE_پای_ټکی      }
#تعریف_کړه YOSYS_NAMESPACE_مختاړی   Yosys::
#تعریف_کړه USING_YOSYS_NAMESPACE    using namespace Yosys;

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وإلاإذا defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#وإلا
#  define YS_ATTRIBUTE(...)
#د_که_پای

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_ناکارول_شوی __attribute__((__unused__))
#وإلا
#  define YS_MAYBE_ناکارول_شوی
#د_که_پای

#إذا defined(_MSC_VER)
#  define YS_NO_UNIQUE_پته [[msvc::no_unique_پته]]
#وإلا
#  define YS_NO_UNIQUE_پته [[no_unique_پته]]
#د_که_پای

#إذا __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#وإلا
#  error "C++20 or later compatible compiler is required"
#د_که_پای

#إذا defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#وإلا
#  define YS_COLD
#د_که_پای

#که_تعریف_شوی __cpp_consteval
#تعریف_کړه YOSYS_CONSTEVAL consteval
#وإلا
// If we can't use consteval we can at least make it constexpr.
#تعریف_کړه YOSYS_CONSTEVAL constexpr
#د_که_پای

#تعریف_کړه YOSYS_ABORT(s) YOSYS_NAMESPACE_مختاړی log_yosys_abort_پیغام(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] فراغ log_yosys_abort_پیغام(std::string_view file, صحيح کرښه, std::string_view func, std::string_view پیغام_2);

[[noreturn]]
فراغ log_assert_failure(ثابت حرف *بیان, سرچینه_location location);
#که_نه_تعریف_شوی NDEBUG
ساكن مضمن فراغ log_assert_کارګر(bool cond, ثابت حرف *بیان, سرچینه_location location = سرچینه_location::current()) {
	إذا (!cond) log_assert_failure(بیان, location);
}
#  define log_assert(assert_بیان) YOSYS_NAMESPACE_مختاړی log_assert_کارګر(assert_بیان, #_assert_expr_)
#وإلا
#  define log_assert(assert_بیان) افعل { إذا (0) { (فراغ)(assert_بیان); } } طالما(0)
#د_که_پای
YOSYS_NAMESPACE_پای_ټکی

#ضمن "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#که_تعریف_شوی HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#وإلا
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#د_که_پای


using std::ویکټور;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::پرلپسې_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
بنية shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ثابت حرف *s) { content = std::shared_ptr<string>(new string(s)); }
	ثابت حرف *c_str() ثابت { أرجع content->c_str(); }
	ثابت string &str() ثابت { أرجع *content; }
	bool چلوونکی==(ثابت shared_str &other) ثابت { أرجع *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		أرجع h;
	}
};

namespace RTLIL {
	بنية Const;
	بنية Sig_بټ;
	بنية SigSpec;
	بنية Wire;
	بنية Cell;
	بنية حافظه;
	بنية Process;
	بنية Module;
	بنية Design;
	بنية Monitor;
	بنية Selection;
	بنية SigChunk;
	بنية PooledName;
	تعداد State : بینښه حرف;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	بنية Ast_غوټه;
}

using RTLIL::Const;
using RTLIL::Sig_بټ;
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
	کينډۍ<> بنية hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<RTLIL::حافظه*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<RTLIL::Process*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<RTLIL::Module*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<RTLIL::Design*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<AST::Ast_غوټه*> : hash_obj_ops {};

	کينډۍ<> بنية hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت RTLIL::حافظه*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	کينډۍ<> بنية hash_ops<ثابت AST::Ast_غوټه*> : hash_obj_ops {};
}

فراغ memhasher_on();
فراغ memhasher_off();
فراغ memhasher_do();

خارجي bool memhasher_فعال;
مضمن فراغ memhasher() { إذا (memhasher_فعال) memhasher_do(); }

فراغ yosys_banner();
صحيح ceil_log2(صحيح x) YS_ATTRIBUTE(ثابت);

کينډۍ<typename T> صحيح Get_اندازه(ثابت T &obj) { أرجع obj.اندازه(); }
مضمن صحيح Get_اندازه(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
بنية Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ساكن bool فعال() { أرجع فعال_2; }
خاص:
	ساكن bool فعال_2;
};

بنية Autoidx {
	Autoidx(صحيح ارزښت) : ارزښت(ارزښت) {}
	چلوونکی صحيح() ثابت { أرجع ارزښت; }
	فراغ ensure_at_لږترلږه(صحيح v);
	صحيح چلوونکی++(صحيح);
خاص:
	صحيح ارزښت;
};

خارجي Autoidx autoidx;
خارجي صحيح yosys_xtrace;
خارجي bool yosys_write_versions;

ثابت std::string *create_id_مختاړی(std::string_view file, صحيح کرښه, std::string_view func);

#تعریف_کړه NEW_ID \
	YOSYS_NAMESPACE_مختاړی TwineSpec{YOSYS_NAMESPACE_مختاړی TwineSpec::AutoSuffix{[](سرچینه_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> مختاړی_2(YOSYS_NAMESPACE_مختاړی create_id_مختاړی(location.file_name(), location.کرښه(), func)); \
		أرجع مختاړی_2.get(); \
	}(سرچینه_location::current(), __FUNCTION__), std::منزل_string(YOSYS_NAMESPACE_مختاړی autoidx++)}}
#تعریف_کړه NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_مختاړی TwineSpec{YOSYS_NAMESPACE_مختاړی TwineSpec::AutoSuffix{[](سرچینه_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> مختاړی_2(YOSYS_NAMESPACE_مختاړی create_id_مختاړی(location.file_name(), location.کرښه(), func)); \
		أرجع مختاړی_2.get(); \
	}(سرچینه_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::منزل_string(YOSYS_NAMESPACE_مختاړی autoidx++)}}

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
