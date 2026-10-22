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

#جے_بنا_تعریف YOSYS_COMMON_H
#تعریف_کرو YOSYS_COMMON_H

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
	using std::سروت_location;
#وإلا
	#ضمن <experimental/source_location>
	using std::experimental::سروت_location;
#جے_انت

#ضمن <stdarg.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <stdio.h>
#ضمن <limits.h>
#ضمن <sys/stat.h>
#ضمن <errno.h>

#جے_بنا_تعریف _YOSYS_
#  error It looks like you are trying منزل build Yosys without the config مقرر_کردا سیٹ. \
         When building Yosys with a custom make system, make sure you سیٹ all the \
         مقرر_کردا the Yosys Makefile would سیٹ لكل your build configuration.
#جے_انت

#ضمن "kernel/yosys_config.h"

#تعریف_کرو FRIEND_TEST(test_case_name, test_name) \
  یار class test_case_name##_##test_name##_Test

#جے_تعریف_شدہ _WIN32
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
#    define رستہ_MAX MAX_رستہ
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#جے_انت

#جے_بنا_تعریف رستہ_MAX
#  define رستہ_MAX 4096
#جے_انت


#تعریف_کرو YOSYS_NAMESPACE          Yosys
#تعریف_کرو نجی_NAMESPACE_BEGIN  namespace {
#تعریف_کرو نجی_NAMESPACE_سرا    }
#تعریف_کرو YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#تعریف_کرو YOSYS_NAMESPACE_سرا      }
#تعریف_کرو YOSYS_NAMESPACE_اگیتر   Yosys::
#تعریف_کرو USING_YOSYS_NAMESPACE    using namespace Yosys;

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وإلاإذا defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#وإلا
#  define YS_ATTRIBUTE(...)
#جے_انت

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_بن_ورتیا __attribute__((__unused__))
#وإلا
#  define YS_MAYBE_بن_ورتیا
#جے_انت

#إذا defined(_MSC_VER)
#  define YS_NO_UNIQUE_پتہ [[msvc::no_unique_پتہ]]
#وإلا
#  define YS_NO_UNIQUE_پتہ [[no_unique_پتہ]]
#جے_انت

#إذا __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#وإلا
#  error "C++20 or later compatible compiler is required"
#جے_انت

#إذا defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#وإلا
#  define YS_COLD
#جے_انت

#جے_تعریف_شدہ __cpp_consteval
#تعریف_کرو YOSYS_CONSTEVAL consteval
#وإلا
// If we can't use consteval we can at least make it constexpr.
#تعریف_کرو YOSYS_CONSTEVAL constexpr
#جے_انت

#تعریف_کرو YOSYS_ABORT(s) YOSYS_NAMESPACE_اگیتر log_yosys_abort_سنیہا(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] فراغ log_yosys_abort_سنیہا(std::string_view file, صحيح سطر, std::string_view func, std::string_view سنیہا);

[[noreturn]]
فراغ log_assert_failure(ثابت حرف *اظہاریہ, سروت_location location);
#جے_بنا_تعریف NDEBUG
ساكن مضمن فراغ log_assert_کامہ(bool cond, ثابت حرف *اظہاریہ, سروت_location location = سروت_location::current()) {
	إذا (!cond) log_assert_failure(اظہاریہ, location);
}
#  define log_assert(assert_اظہاریہ) YOSYS_NAMESPACE_اگیتر log_assert_کامہ(assert_اظہاریہ, #_assert_expr_)
#وإلا
#  define log_assert(assert_اظہاریہ) افعل { إذا (0) { (فراغ)(assert_اظہاریہ); } } طالما(0)
#جے_انت
YOSYS_NAMESPACE_سرا

#ضمن "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#جے_تعریف_شدہ HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#وإلا
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#جے_انت


using std::سمتیہ;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::لگاتار_hash;
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
	bool عامل==(ثابت shared_str &other) ثابت { أرجع *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		أرجع h;
	}
};

namespace RTLIL {
	بنية Const;
	بنية Sig_ثنائی_ہندسہ;
	بنية SigSpec;
	بنية Wire;
	بنية Cell;
	بنية حافظہ;
	بنية Process;
	بنية Module;
	بنية Design;
	بنية Monitor;
	بنية Selection;
	بنية SigChunk;
	بنية PooledName;
	تعداد State : بنانشانی حرف;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	بنية Ast_گرہ;
}

using RTLIL::Const;
using RTLIL::Sig_ثنائی_ہندسہ;
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
	سانچا<> بنية hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<RTLIL::حافظہ*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<RTLIL::Process*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<RTLIL::Module*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<RTLIL::Design*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<AST::Ast_گرہ*> : hash_obj_ops {};

	سانچا<> بنية hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت RTLIL::حافظہ*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	سانچا<> بنية hash_ops<ثابت AST::Ast_گرہ*> : hash_obj_ops {};
}

فراغ memhasher_on();
فراغ memhasher_off();
فراغ memhasher_do();

خارجي bool memhasher_سرگرم;
مضمن فراغ memhasher() { إذا (memhasher_سرگرم) memhasher_do(); }

فراغ yosys_banner();
صحيح ceil_log2(صحيح x) YS_ATTRIBUTE(ثابت);

سانچا<typename T> صحيح Get_ناپ(ثابت T &obj) { أرجع obj.ناپ(); }
مضمن صحيح Get_ناپ(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
بنية Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ساكن bool سرگرم() { أرجع سرگرم_2; }
خاص:
	ساكن bool سرگرم_2;
};

بنية Autoidx {
	Autoidx(صحيح قدر) : قدر(قدر) {}
	عامل صحيح() ثابت { أرجع قدر; }
	فراغ ensure_at_گھٹ_توں_گھٹ(صحيح v);
	صحيح عامل++(صحيح);
خاص:
	صحيح قدر;
};

خارجي Autoidx autoidx;
خارجي صحيح yosys_xtrace;
خارجي bool yosys_write_versions;

ثابت std::string *create_id_اگیتر(std::string_view file, صحيح سطر, std::string_view func);

#تعریف_کرو NEW_ID \
	YOSYS_NAMESPACE_اگیتر TwineSpec{YOSYS_NAMESPACE_اگیتر TwineSpec::AutoSuffix{[](سروت_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> اگیتر_2(YOSYS_NAMESPACE_اگیتر create_id_اگیتر(location.file_name(), location.سطر(), func)); \
		أرجع اگیتر_2.get(); \
	}(سروت_location::current(), __FUNCTION__), std::منزل_string(YOSYS_NAMESPACE_اگیتر autoidx++)}}
#تعریف_کرو NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_اگیتر TwineSpec{YOSYS_NAMESPACE_اگیتر TwineSpec::AutoSuffix{[](سروت_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> اگیتر_2(YOSYS_NAMESPACE_اگیتر create_id_اگیتر(location.file_name(), location.سطر(), func)); \
		أرجع اگیتر_2.get(); \
	}(سروت_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::منزل_string(YOSYS_NAMESPACE_اگیتر autoidx++)}}

YOSYS_NAMESPACE_سرا

#جے_انت
