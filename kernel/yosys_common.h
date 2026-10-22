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

#اگر_غیر_تعریف_شدہ YOSYS_COMMON_H
#تعریف_کرو YOSYS_COMMON_H

#شامل <array>
#شامل <atomic>
#شامل <map>
#شامل <set>
#شامل <tuple>
#شامل <vector>
#شامل <string>
#شامل <algorithm>
#شامل <functional>
#شامل <unordered_map>
#شامل <unordered_set>
#شامل <initializer_list>
#شامل <variant>
#شامل <optional>
#شامل <stdexcept>
#شامل <memory>
#شامل <cmath>
#شامل <cstddef>

#شامل <sstream>
#شامل <fstream>
#شامل <istream>
#شامل <ostream>
#شامل <iostream>

#شامل <version>
#اگر __cpp_lib_source_location >= 201907L
	#شامل <source_location>
	using std::ماخذ_location;
#ورنہ
	#شامل <experimental/source_location>
	using std::experimental::ماخذ_location;
#اگر_ختم

#شامل <stdarg.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <stdint.h>
#شامل <stdio.h>
#شامل <limits.h>
#شامل <sys/stat.h>
#شامل <errno.h>

#اگر_غیر_تعریف_شدہ _YOSYS_
#  error It looks like you are trying مقصد build Yosys without the config متعین_کرتا سیٹ. \
         When building Yosys with a custom make system, make sure you سیٹ all the \
         متعین_کرتا the Yosys Makefile would سیٹ برائے your build configuration.
#اگر_ختم

#شامل "kernel/yosys_config.h"

#تعریف_کرو FRIEND_TEST(test_case_name, test_name) \
  دوست class test_case_name##_##test_name##_Test

#اگر_تعریف_شدہ _WIN32
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
#    define راستہ_MAX MAX_راستہ
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#اگر_ختم

#اگر_غیر_تعریف_شدہ راستہ_MAX
#  define راستہ_MAX 4096
#اگر_ختم


#تعریف_کرو YOSYS_NAMESPACE          Yosys
#تعریف_کرو نجی_NAMESPACE_BEGIN  namespace {
#تعریف_کرو نجی_NAMESPACE_انتہا    }
#تعریف_کرو YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#تعریف_کرو YOSYS_NAMESPACE_انتہا      }
#تعریف_کرو YOSYS_NAMESPACE_سابقہ   Yosys::
#تعریف_کرو USING_YOSYS_NAMESPACE    using namespace Yosys;

#اگر defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وگرنہ_اگر defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#ورنہ
#  define YS_ATTRIBUTE(...)
#اگر_ختم

#اگر defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_غیر_مستعمل __attribute__((__unused__))
#ورنہ
#  define YS_MAYBE_غیر_مستعمل
#اگر_ختم

#اگر defined(_MSC_VER)
#  define YS_NO_UNIQUE_پتہ [[msvc::no_unique_پتہ]]
#ورنہ
#  define YS_NO_UNIQUE_پتہ [[no_unique_پتہ]]
#اگر_ختم

#اگر __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#ورنہ
#  error "C++20 or later compatible compiler is required"
#اگر_ختم

#اگر defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#ورنہ
#  define YS_COLD
#اگر_ختم

#اگر_تعریف_شدہ __cpp_consteval
#تعریف_کرو YOSYS_CONSTEVAL consteval
#ورنہ
// If we can't use consteval we can at least make it constexpr.
#تعریف_کرو YOSYS_CONSTEVAL constexpr
#اگر_ختم

#تعریف_کرو YOSYS_ABORT(s) YOSYS_NAMESPACE_سابقہ log_yosys_abort_پیغام(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] خالی log_yosys_abort_پیغام(std::string_view file, صحیح سطر, std::string_view func, std::string_view پیغام_2);

[[noreturn]]
خالی log_assert_failure(ثابت حرف *اظہاریہ, ماخذ_location location);
#اگر_غیر_تعریف_شدہ NDEBUG
ساکن اندرونی خالی log_assert_کارکن(bool cond, ثابت حرف *اظہاریہ, ماخذ_location location = ماخذ_location::current()) {
	اگر (!cond) log_assert_failure(اظہاریہ, location);
}
#  define log_assert(assert_اظہاریہ) YOSYS_NAMESPACE_سابقہ log_assert_کارکن(assert_اظہاریہ, #_assert_expr_)
#ورنہ
#  define log_assert(assert_اظہاریہ) کرو { اگر (0) { (خالی)(assert_اظہاریہ); } } جب(0)
#اگر_ختم
YOSYS_NAMESPACE_انتہا

#شامل "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#اگر_تعریف_شدہ HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#ورنہ
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#اگر_ختم


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
using hashlib::تسلسل_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
ساخت shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ثابت حرف *s) { content = std::shared_ptr<string>(new string(s)); }
	ثابت حرف *c_str() ثابت { واپس content->c_str(); }
	ثابت string &str() ثابت { واپس *content; }
	bool عامل==(ثابت shared_str &other) ثابت { واپس *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		واپس h;
	}
};

namespace RTLIL {
	ساخت Const;
	ساخت Sig_ثنائی_ہندسہ;
	ساخت SigSpec;
	ساخت Wire;
	ساخت Cell;
	ساخت حافظہ;
	ساخت Process;
	ساخت Module;
	ساخت Design;
	ساخت Monitor;
	ساخت Selection;
	ساخت SigChunk;
	ساخت PooledName;
	شمار State : غیرعلامتی حرف;

	قسمتعریف std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ساخت Ast_گرہ;
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
	سانچہ<> ساخت hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<RTLIL::حافظہ*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<RTLIL::Process*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<RTLIL::Module*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<RTLIL::Design*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<AST::Ast_گرہ*> : hash_obj_ops {};

	سانچہ<> ساخت hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت RTLIL::حافظہ*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	سانچہ<> ساخت hash_ops<ثابت AST::Ast_گرہ*> : hash_obj_ops {};
}

خالی memhasher_on();
خالی memhasher_off();
خالی memhasher_do();

خارجی bool memhasher_فعال;
اندرونی خالی memhasher() { اگر (memhasher_فعال) memhasher_do(); }

خالی yosys_banner();
صحیح ceil_log2(صحیح x) YS_ATTRIBUTE(ثابت);

سانچہ<typename T> صحیح Get_سائز(ثابت T &obj) { واپس obj.سائز(); }
اندرونی صحیح Get_سائز(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ساخت Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ساکن bool فعال() { واپس فعال_2; }
نجی:
	ساکن bool فعال_2;
};

ساخت Autoidx {
	Autoidx(صحیح قدر) : قدر(قدر) {}
	عامل صحیح() ثابت { واپس قدر; }
	خالی ensure_at_کم_سے_کم(صحیح v);
	صحیح عامل++(صحیح);
نجی:
	صحیح قدر;
};

خارجی Autoidx autoidx;
خارجی صحیح yosys_xtrace;
خارجی bool yosys_write_versions;

ثابت std::string *create_id_سابقہ(std::string_view file, صحیح سطر, std::string_view func);

#تعریف_کرو NEW_ID \
	YOSYS_NAMESPACE_سابقہ TwineSpec{YOSYS_NAMESPACE_سابقہ TwineSpec::AutoSuffix{[](ماخذ_location location, std::string_view func) -> ثابت std::string * { \
		ساکن std::unique_ptr<ثابت std::string> سابقہ_2(YOSYS_NAMESPACE_سابقہ create_id_سابقہ(location.file_name(), location.سطر(), func)); \
		واپس سابقہ_2.get(); \
	}(ماخذ_location::current(), __FUNCTION__), std::مقصد_string(YOSYS_NAMESPACE_سابقہ autoidx++)}}
#تعریف_کرو NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_سابقہ TwineSpec{YOSYS_NAMESPACE_سابقہ TwineSpec::AutoSuffix{[](ماخذ_location location, std::string_view func) -> ثابت std::string * { \
		ساکن std::unique_ptr<ثابت std::string> سابقہ_2(YOSYS_NAMESPACE_سابقہ create_id_سابقہ(location.file_name(), location.سطر(), func)); \
		واپس سابقہ_2.get(); \
	}(ماخذ_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::مقصد_string(YOSYS_NAMESPACE_سابقہ autoidx++)}}

YOSYS_NAMESPACE_انتہا

#اگر_ختم
