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

#اگر_تعریف_نشده YOSYS_COMMON_H
#تعریف_کن YOSYS_COMMON_H

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
	using std::منبع_location;
#وإلا
	#ضمن <experimental/source_location>
	using std::experimental::منبع_location;
#پایان_اگر

#ضمن <stdarg.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <stdio.h>
#ضمن <limits.h>
#ضمن <sys/stat.h>
#ضمن <errno.h>

#اگر_تعریف_نشده _YOSYS_
#  error It looks like you are trying مقصد build Yosys without the config تعریف_میکند مجموعه. \
         When building Yosys with a custom make system, make sure you مجموعه all the \
         تعریف_میکند the Yosys Makefile would مجموعه لكل your build configuration.
#پایان_اگر

#ضمن "kernel/yosys_config.h"

#تعریف_کن FRIEND_TEST(test_case_name, test_name) \
  دوست class test_case_name##_##test_name##_Test

#اگر_تعریف_شده _WIN32
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
#    define مسیر_MAX MAX_مسیر
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#پایان_اگر

#اگر_تعریف_نشده مسیر_MAX
#  define مسیر_MAX 4096
#پایان_اگر


#تعریف_کن YOSYS_NAMESPACE          Yosys
#تعریف_کن خصوصی_NAMESPACE_BEGIN  namespace {
#تعریف_کن خصوصی_NAMESPACE_انتها    }
#تعریف_کن YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#تعریف_کن YOSYS_NAMESPACE_انتها      }
#تعریف_کن YOSYS_NAMESPACE_پیشوند   Yosys::
#تعریف_کن USING_YOSYS_NAMESPACE    using namespace Yosys;

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وإلاإذا defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#وإلا
#  define YS_ATTRIBUTE(...)
#پایان_اگر

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_استفادهنشده __attribute__((__unused__))
#وإلا
#  define YS_MAYBE_استفادهنشده
#پایان_اگر

#إذا defined(_MSC_VER)
#  define YS_NO_UNIQUE_آدرس [[msvc::no_unique_آدرس]]
#وإلا
#  define YS_NO_UNIQUE_آدرس [[no_unique_آدرس]]
#پایان_اگر

#إذا __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#وإلا
#  error "C++20 or later compatible compiler is required"
#پایان_اگر

#إذا defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#وإلا
#  define YS_COLD
#پایان_اگر

#اگر_تعریف_شده __cpp_consteval
#تعریف_کن YOSYS_CONSTEVAL consteval
#وإلا
// If we can't use consteval we can at least make it constexpr.
#تعریف_کن YOSYS_CONSTEVAL constexpr
#پایان_اگر

#تعریف_کن YOSYS_ABORT(s) YOSYS_NAMESPACE_پیشوند log_yosys_abort_پیام(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] فراغ log_yosys_abort_پیام(std::string_view file, صحيح سطر, std::string_view func, std::string_view پیام_2);

[[noreturn]]
فراغ log_assert_failure(ثابت حرف *عبارت, منبع_location location);
#اگر_تعریف_نشده NDEBUG
ساكن مضمن فراغ log_assert_کارگر(bool cond, ثابت حرف *عبارت, منبع_location location = منبع_location::current()) {
	إذا (!cond) log_assert_failure(عبارت, location);
}
#  define log_assert(assert_عبارت) YOSYS_NAMESPACE_پیشوند log_assert_کارگر(assert_عبارت, #_assert_expr_)
#وإلا
#  define log_assert(assert_عبارت) افعل { إذا (0) { (فراغ)(assert_عبارت); } } طالما(0)
#پایان_اگر
YOSYS_NAMESPACE_انتها

#ضمن "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#اگر_تعریف_شده HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#وإلا
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#پایان_اگر


using std::وکتور;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::پیاپی_hash;
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
	bool عملگر==(ثابت shared_str &other) ثابت { أرجع *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		أرجع h;
	}
};

namespace RTLIL {
	بنية Const;
	بنية Sig_بیت;
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
	تعداد State : بیعلامت حرف;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	بنية Ast_گره;
}

using RTLIL::Const;
using RTLIL::Sig_بیت;
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
	قالب<> بنية hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	قالب<> بنية hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	قالب<> بنية hash_ops<RTLIL::حافظه*> : hash_obj_ops {};
	قالب<> بنية hash_ops<RTLIL::Process*> : hash_obj_ops {};
	قالب<> بنية hash_ops<RTLIL::Module*> : hash_obj_ops {};
	قالب<> بنية hash_ops<RTLIL::Design*> : hash_obj_ops {};
	قالب<> بنية hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	قالب<> بنية hash_ops<AST::Ast_گره*> : hash_obj_ops {};

	قالب<> بنية hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت RTLIL::حافظه*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	قالب<> بنية hash_ops<ثابت AST::Ast_گره*> : hash_obj_ops {};
}

فراغ memhasher_on();
فراغ memhasher_off();
فراغ memhasher_do();

خارجي bool memhasher_فعال;
مضمن فراغ memhasher() { إذا (memhasher_فعال) memhasher_do(); }

فراغ yosys_banner();
صحيح ceil_log2(صحيح x) YS_ATTRIBUTE(ثابت);

قالب<typename T> صحيح Get_اندازه(ثابت T &obj) { أرجع obj.اندازه(); }
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
	Autoidx(صحيح مقدار) : مقدار(مقدار) {}
	عملگر صحيح() ثابت { أرجع مقدار; }
	فراغ ensure_at_کمترین(صحيح v);
	صحيح عملگر++(صحيح);
خاص:
	صحيح مقدار;
};

خارجي Autoidx autoidx;
خارجي صحيح yosys_xtrace;
خارجي bool yosys_write_versions;

ثابت std::string *create_id_پیشوند(std::string_view file, صحيح سطر, std::string_view func);

#تعریف_کن NEW_ID \
	YOSYS_NAMESPACE_پیشوند TwineSpec{YOSYS_NAMESPACE_پیشوند TwineSpec::AutoSuffix{[](منبع_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> پیشوند_2(YOSYS_NAMESPACE_پیشوند create_id_پیشوند(location.file_name(), location.سطر(), func)); \
		أرجع پیشوند_2.get(); \
	}(منبع_location::current(), __FUNCTION__), std::مقصد_string(YOSYS_NAMESPACE_پیشوند autoidx++)}}
#تعریف_کن NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_پیشوند TwineSpec{YOSYS_NAMESPACE_پیشوند TwineSpec::AutoSuffix{[](منبع_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> پیشوند_2(YOSYS_NAMESPACE_پیشوند create_id_پیشوند(location.file_name(), location.سطر(), func)); \
		أرجع پیشوند_2.get(); \
	}(منبع_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::مقصد_string(YOSYS_NAMESPACE_پیشوند autoidx++)}}

YOSYS_NAMESPACE_انتها

#پایان_اگر
