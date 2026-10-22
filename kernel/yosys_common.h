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

#నిర్వచితం_కాకపోతే YOSYS_COMMON_H
#నిర్వచించు YOSYS_COMMON_H

#అన్తర్భావ <array>
#అన్తర్భావ <atomic>
#అన్తర్భావ <map>
#అన్తర్భావ <set>
#అన్తర్భావ <tuple>
#అన్తర్భావ <vector>
#అన్తర్భావ <string>
#అన్తర్భావ <algorithm>
#అన్తర్భావ <functional>
#అన్తర్భావ <unordered_map>
#అన్తర్భావ <unordered_set>
#అన్తర్భావ <initializer_list>
#అన్తర్భావ <variant>
#అన్తర్భావ <optional>
#అన్తర్భావ <stdexcept>
#అన్తర్భావ <memory>
#అన్తర్భావ <cmath>
#అన్తర్భావ <cstddef>

#అన్తర్భావ <sstream>
#అన్తర్భావ <fstream>
#అన్తర్భావ <istream>
#అన్తర్భావ <ostream>
#అన్తర్భావ <iostream>

#అన్తర్భావ <version>
#ఒకవేళ __cpp_lib_source_location >= 201907L
	#అన్తర్భావ <source_location>
	using std::ఆధారం_location;
#లేకపోతే
	#అన్తర్భావ <experimental/source_location>
	using std::experimental::ఆధారం_location;
#అయితే_ముగింపు

#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <limits.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <errno.h>

#నిర్వచితం_కాకపోతే _YOSYS_
#  error It looks like you are trying గమ్యం build Yosys without the config నిర్వచిస్తుంది సమితి. \
         When building Yosys with a custom make system, make sure you సమితి all the \
         నిర్వచిస్తుంది the Yosys Makefile would సమితి కొరకు your build configuration.
#అయితే_ముగింపు

#అన్తర్భావ "kernel/yosys_config.h"

#నిర్వచించు FRIEND_TEST(test_case_name, test_name) \
  మిత్ర class test_case_name##_##test_name##_Test

#నిర్వచితమైతే _WIN32
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
#    define మార్గం_MAX MAX_మార్గం
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే మార్గం_MAX
#  define మార్గం_MAX 4096
#అయితే_ముగింపు


#నిర్వచించు YOSYS_NAMESPACE          Yosys
#నిర్వచించు వ్యక్తిగత_NAMESPACE_BEGIN  namespace {
#నిర్వచించు వ్యక్తిగత_NAMESPACE_చివర    }
#నిర్వచించు YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#నిర్వచించు YOSYS_NAMESPACE_చివర      }
#నిర్వచించు YOSYS_NAMESPACE_ఉపసర్గ   Yosys::
#నిర్వచించు USING_YOSYS_NAMESPACE    using namespace Yosys;

#ఒకవేళ defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#లేదావేళ defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#లేకపోతే
#  define YS_ATTRIBUTE(...)
#అయితే_ముగింపు

#ఒకవేళ defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_వాడని __attribute__((__unused__))
#లేకపోతే
#  define YS_MAYBE_వాడని
#అయితే_ముగింపు

#ఒకవేళ defined(_MSC_VER)
#  define YS_NO_UNIQUE_చిరునామా [[msvc::no_unique_చిరునామా]]
#లేకపోతే
#  define YS_NO_UNIQUE_చిరునామా [[no_unique_చిరునామా]]
#అయితే_ముగింపు

#ఒకవేళ __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#లేకపోతే
#  error "C++20 or later compatible compiler is required"
#అయితే_ముగింపు

#ఒకవేళ defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#లేకపోతే
#  define YS_COLD
#అయితే_ముగింపు

#నిర్వచితమైతే __cpp_consteval
#నిర్వచించు YOSYS_CONSTEVAL consteval
#లేకపోతే
// If we can't use consteval we can at least make it constexpr.
#నిర్వచించు YOSYS_CONSTEVAL constexpr
#అయితే_ముగింపు

#నిర్వచించు YOSYS_ABORT(s) YOSYS_NAMESPACE_ఉపసర్గ log_yosys_abort_సందేశం(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] శూన్య log_yosys_abort_సందేశం(std::string_view file, పూర్ణాంకం పంక్తి, std::string_view func, std::string_view సందేశం_2);

[[noreturn]]
శూన్య log_assert_failure(స్థిరరాశి అక్షరం *సమాసం, ఆధారం_location location);
#నిర్వచితం_కాకపోతే NDEBUG
స్థిర అంతఃవరుస శూన్య log_assert_కార్మికుడు(bool cond, స్థిరరాశి అక్షరం *సమాసం, ఆధారం_location location = ఆధారం_location::current()) {
	ఒకవేళ (!cond) log_assert_failure(సమాసం, location);
}
#  define log_assert(assert_సమాసం) YOSYS_NAMESPACE_ఉపసర్గ log_assert_కార్మికుడు(assert_సమాసం, #_assert_expr_)
#లేకపోతే
#  define log_assert(assert_సమాసం) చేయి { ఒకవేళ (0) { (శూన్య)(assert_సమాసం); } } అయితే(0)
#అయితే_ముగింపు
YOSYS_NAMESPACE_చివర

#అన్తర్భావ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#నిర్వచితమైతే HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#లేకపోతే
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#అయితే_ముగింపు


using std::సదిశ;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::నిరంతరం_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
నిర్మాణం shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(స్థిరరాశి అక్షరం *s) { content = std::shared_ptr<string>(new string(s)); }
	స్థిరరాశి అక్షరం *c_str() స్థిరరాశి { తిరిగి content->c_str(); }
	స్థిరరాశి string &str() స్థిరరాశి { తిరిగి *content; }
	bool క్రియాకారకం==(స్థిరరాశి shared_str &other) స్థిరరాశి { తిరిగి *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
		h.eat(*content);
		తిరిగి h;
	}
};

namespace RTLIL {
	నిర్మాణం Const;
	నిర్మాణం Sig_బిట్టు;
	నిర్మాణం SigSpec;
	నిర్మాణం Wire;
	నిర్మాణం Cell;
	నిర్మాణం జ్ఞాపకం;
	నిర్మాణం Process;
	నిర్మాణం Module;
	నిర్మాణం Design;
	నిర్మాణం Monitor;
	నిర్మాణం Selection;
	నిర్మాణం SigChunk;
	నిర్మాణం PooledName;
	గణన State : గుర్తులేని అక్షరం;

	రకనిర్వచనం std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	నిర్మాణం Ast_కణుపు;
}

using RTLIL::Const;
using RTLIL::Sig_బిట్టు;
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
	మూస<> నిర్మాణం hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<RTLIL::జ్ఞాపకం*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<RTLIL::Process*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<RTLIL::Module*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<RTLIL::Design*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<AST::Ast_కణుపు*> : hash_obj_ops {};

	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::Wire*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::Cell*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::జ్ఞాపకం*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::Process*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::Module*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::Design*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి RTLIL::Monitor*> : hash_obj_ops {};
	మూస<> నిర్మాణం hash_ops<స్థిరరాశి AST::Ast_కణుపు*> : hash_obj_ops {};
}

శూన్య memhasher_on();
శూన్య memhasher_off();
శూన్య memhasher_do();

బాహ్య bool memhasher_చురుకైన;
అంతఃవరుస శూన్య memhasher() { ఒకవేళ (memhasher_చురుకైన) memhasher_do(); }

శూన్య yosys_banner();
పూర్ణాంకం ceil_log2(పూర్ణాంకం x) YS_ATTRIBUTE(స్థిరరాశి);

మూస<typename T> పూర్ణాంకం Get_కొలత(స్థిరరాశి T &obj) { తిరిగి obj.కొలత(); }
అంతఃవరుస పూర్ణాంకం Get_కొలత(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
నిర్మాణం Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	స్థిర bool చురుకైన() { తిరిగి చురుకైన_2; }
అంతరంగ:
	స్థిర bool చురుకైన_2;
};

నిర్మాణం Autoidx {
	Autoidx(పూర్ణాంకం విలువ) : విలువ(విలువ) {}
	క్రియాకారకం పూర్ణాంకం() స్థిరరాశి { తిరిగి విలువ; }
	శూన్య ensure_at_కనిష్ఠ(పూర్ణాంకం v);
	పూర్ణాంకం క్రియాకారకం++(పూర్ణాంకం);
అంతరంగ:
	పూర్ణాంకం విలువ;
};

బాహ్య Autoidx autoidx;
బాహ్య పూర్ణాంకం yosys_xtrace;
బాహ్య bool yosys_write_versions;

స్థిరరాశి std::string *create_id_ఉపసర్గ(std::string_view file, పూర్ణాంకం పంక్తి, std::string_view func);

#నిర్వచించు NEW_ID \
	YOSYS_NAMESPACE_ఉపసర్గ TwineSpec{YOSYS_NAMESPACE_ఉపసర్గ TwineSpec::AutoSuffix{[](ఆధారం_location location, std::string_view func) -> స్థిరరాశి std::string * { \
		స్థిర std::unique_ptr<స్థిరరాశి std::string> ఉపసర్గ_2(YOSYS_NAMESPACE_ఉపసర్గ create_id_ఉపసర్గ(location.file_name(), location.పంక్తి(), func)); \
		తిరిగి ఉపసర్గ_2.get(); \
	}(ఆధారం_location::current(), __FUNCTION__), std::గమ్యం_string(YOSYS_NAMESPACE_ఉపసర్గ autoidx++)}}
#నిర్వచించు NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_ఉపసర్గ TwineSpec{YOSYS_NAMESPACE_ఉపసర్గ TwineSpec::AutoSuffix{[](ఆధారం_location location, std::string_view func) -> స్థిరరాశి std::string * { \
		స్థిర std::unique_ptr<స్థిరరాశి std::string> ఉపసర్గ_2(YOSYS_NAMESPACE_ఉపసర్గ create_id_ఉపసర్గ(location.file_name(), location.పంక్తి(), func)); \
		తిరిగి ఉపసర్గ_2.get(); \
	}(ఆధారం_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::గమ్యం_string(YOSYS_NAMESPACE_ఉపసర్గ autoidx++)}}

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
