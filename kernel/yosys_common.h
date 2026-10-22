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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_COMMON_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_COMMON_H

#ಅನ್ತರ್ಭಾವ <array>
#ಅನ್ತರ್ಭಾವ <atomic>
#ಅನ್ತರ್ಭಾವ <map>
#ಅನ್ತರ್ಭಾವ <set>
#ಅನ್ತರ್ಭಾವ <tuple>
#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <functional>
#ಅನ್ತರ್ಭಾವ <unordered_map>
#ಅನ್ತರ್ಭಾವ <unordered_set>
#ಅನ್ತರ್ಭಾವ <initializer_list>
#ಅನ್ತರ್ಭಾವ <variant>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <stdexcept>
#ಅನ್ತರ್ಭಾವ <memory>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ <cstddef>

#ಅನ್ತರ್ಭಾವ <sstream>
#ಅನ್ತರ್ಭಾವ <fstream>
#ಅನ್ತರ್ಭಾವ <istream>
#ಅನ್ತರ್ಭಾವ <ostream>
#ಅನ್ತರ್ಭಾವ <iostream>

#ಅನ್ತರ್ಭಾವ <version>
#ಆದರೆ __cpp_lib_source_location >= 201907L
	#ಅನ್ತರ್ಭಾವ <source_location>
	using std::ಮೂಲ_location;
#ಇಲ್ಲದಿದ್ದರೆ
	#ಅನ್ತರ್ಭಾವ <experimental/source_location>
	using std::experimental::ಮೂಲ_location;
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _YOSYS_
#  error It looks like you are trying ಗಮ್ಯ build Yosys without the config ನಿರೂಪಿಸುತ್ತದೆ ಗಣ. \
         When building Yosys with a custom make system, make sure you ಗಣ all the \
         ನಿರೂಪಿಸುತ್ತದೆ the Yosys Makefile would ಗಣ ಗಾಗಿ your build configuration.
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "kernel/yosys_config.h"

#ವ್ಯಾಖ್ಯಾನಿಸು FRIEND_TEST(test_case_name, test_name) \
  ಮಿತ್ರ class test_case_name##_##test_name##_Test

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
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
#    define ಮಾರ್ಗ_MAX MAX_ಮಾರ್ಗ
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಮಾರ್ಗ_MAX
#  define ಮಾರ್ಗ_MAX 4096
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_NAMESPACE          Yosys
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಸಗಿ_NAMESPACE_BEGIN  namespace {
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಸಗಿ_NAMESPACE_ಕೊನೆ    }
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_NAMESPACE_ಕೊನೆ      }
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ   Yosys::
#ವ್ಯಾಖ್ಯಾನಿಸು USING_YOSYS_NAMESPACE    using namespace Yosys;

#ಆದರೆ defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#ಅಥವಾಆದರೆ defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#ಇಲ್ಲದಿದ್ದರೆ
#  define YS_ATTRIBUTE(...)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_ಬಳಸದ __attribute__((__unused__))
#ಇಲ್ಲದಿದ್ದರೆ
#  define YS_MAYBE_ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_MSC_VER)
#  define YS_NO_UNIQUE_ವಿಳಾಸ [[msvc::no_unique_ವಿಳಾಸ]]
#ಇಲ್ಲದಿದ್ದರೆ
#  define YS_NO_UNIQUE_ವಿಳಾಸ [[no_unique_ವಿಳಾಸ]]
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#ಇಲ್ಲದಿದ್ದರೆ
#  error "C++20 or later compatible compiler is required"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#ಇಲ್ಲದಿದ್ದರೆ
#  define YS_COLD
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cpp_consteval
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_CONSTEVAL consteval
#ಇಲ್ಲದಿದ್ದರೆ
// If we can't use consteval we can at least make it constexpr.
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_CONSTEVAL constexpr
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_ABORT(s) YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ log_yosys_abort_ಸಂದೇಶ(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] ಶೂನ್ಯ log_yosys_abort_ಸಂದೇಶ(std::string_view file, ಪೂರ್ಣಾಂಕ ಸಾಲು, std::string_view func, std::string_view ಸಂದೇಶ_2);

[[noreturn]]
ಶೂನ್ಯ log_assert_failure(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಭಿವ್ಯಕ್ತಿ, ಮೂಲ_location location);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_assert_ಕೆಲಸಗಾರ(bool cond, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಭಿವ್ಯಕ್ತಿ, ಮೂಲ_location location = ಮೂಲ_location::current()) {
	ಆದರೆ (!cond) log_assert_failure(ಅಭಿವ್ಯಕ್ತಿ, location);
}
#  define log_assert(assert_ಅಭಿವ್ಯಕ್ತಿ) YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ log_assert_ಕೆಲಸಗಾರ(assert_ಅಭಿವ್ಯಕ್ತಿ, #_assert_expr_)
#ಇಲ್ಲದಿದ್ದರೆ
#  define log_assert(assert_ಅಭಿವ್ಯಕ್ತಿ) ಮಾಡು { ಆದರೆ (0) { (ಶೂನ್ಯ)(assert_ಅಭಿವ್ಯಕ್ತಿ); } } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ
YOSYS_NAMESPACE_ಕೊನೆ

#ಅನ್ತರ್ಭಾವ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#ಇಲ್ಲದಿದ್ದರೆ
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#ಆದರೆ_ಅಂತ್ಯ


using std::ಸದಿಶ;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::ಸತತ_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
ರಚನೆ shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) { content = std::shared_ptr<string>(new string(s)); }
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c_str() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ content->c_str(); }
	ಸ್ಥಿರಾಂಕ string &str() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *content; }
	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ shared_str &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
		h.eat(*content);
		ಹಿಂತಿರುಗಿ h;
	}
};

namespace RTLIL {
	ರಚನೆ Const;
	ರಚನೆ Sig_ಬಿಟ್;
	ರಚನೆ SigSpec;
	ರಚನೆ Wire;
	ರಚನೆ Cell;
	ರಚನೆ ಸ್ಮರಣೆ;
	ರಚನೆ Process;
	ರಚನೆ Module;
	ರಚನೆ Design;
	ರಚನೆ Monitor;
	ರಚನೆ Selection;
	ರಚನೆ SigChunk;
	ರಚನೆ PooledName;
	ಎಣಿಕೆ State : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ;

	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ರಚನೆ Ast_ಗಂಟು;
}

using RTLIL::Const;
using RTLIL::Sig_ಬಿಟ್;
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
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::ಸ್ಮರಣೆ*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::Process*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::Module*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::Design*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<AST::Ast_ಗಂಟು*> : hash_obj_ops {};

	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::Wire*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::Cell*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::ಸ್ಮರಣೆ*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::Process*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::Module*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::Design*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ RTLIL::Monitor*> : hash_obj_ops {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<ಸ್ಥಿರಾಂಕ AST::Ast_ಗಂಟು*> : hash_obj_ops {};
}

ಶೂನ್ಯ memhasher_on();
ಶೂನ್ಯ memhasher_off();
ಶೂನ್ಯ memhasher_do();

ಬಾಹ್ಯ bool memhasher_ಸಕ್ರಿಯ;
ಸಾಲೊಳಗೆ ಶೂನ್ಯ memhasher() { ಆದರೆ (memhasher_ಸಕ್ರಿಯ) memhasher_do(); }

ಶೂನ್ಯ yosys_banner();
ಪೂರ್ಣಾಂಕ ceil_log2(ಪೂರ್ಣಾಂಕ x) YS_ATTRIBUTE(ಸ್ಥಿರಾಂಕ);

ಮಾದರಿ<typename T> ಪೂರ್ಣಾಂಕ Get_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ T &obj) { ಹಿಂತಿರುಗಿ obj.ಗಾತ್ರ_2(); }
ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ Get_ಗಾತ್ರ(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ರಚನೆ Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ಸ್ಥಿರ bool ಸಕ್ರಿಯ() { ಹಿಂತಿರುಗಿ ಸಕ್ರಿಯ_2; }
ಖಾಸಗಿ:
	ಸ್ಥಿರ bool ಸಕ್ರಿಯ_2;
};

ರಚನೆ Autoidx {
	Autoidx(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ) : ಮೌಲ್ಯ(ಮೌಲ್ಯ) {}
	ಕ್ರಿಯಾಕಾರಕ ಪೂರ್ಣಾಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ; }
	ಶೂನ್ಯ ensure_at_ಕನಿಷ್ಠ(ಪೂರ್ಣಾಂಕ v);
	ಪೂರ್ಣಾಂಕ ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ);
ಖಾಸಗಿ:
	ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;
};

ಬಾಹ್ಯ Autoidx autoidx;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ yosys_xtrace;
ಬಾಹ್ಯ bool yosys_write_versions;

ಸ್ಥಿರಾಂಕ std::string *create_id_ಪೂರ್ವಪ್ರತ್ಯಯ(std::string_view file, ಪೂರ್ಣಾಂಕ ಸಾಲು, std::string_view func);

#ವ್ಯಾಖ್ಯಾನಿಸು NEW_ID \
	YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ TwineSpec{YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ TwineSpec::AutoSuffix{[](ಮೂಲ_location location, std::string_view func) -> ಸ್ಥಿರಾಂಕ std::string * { \
		ಸ್ಥಿರ std::unique_ptr<ಸ್ಥಿರಾಂಕ std::string> ಪೂರ್ವಪ್ರತ್ಯಯ_2(YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ create_id_ಪೂರ್ವಪ್ರತ್ಯಯ(location.file_name(), location.ಸಾಲು(), func)); \
		ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_2.get(); \
	}(ಮೂಲ_location::current(), __FUNCTION__), std::ಗಮ್ಯ_string(YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ autoidx++)}}
#ವ್ಯಾಖ್ಯಾನಿಸು NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ TwineSpec{YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ TwineSpec::AutoSuffix{[](ಮೂಲ_location location, std::string_view func) -> ಸ್ಥಿರಾಂಕ std::string * { \
		ಸ್ಥಿರ std::unique_ptr<ಸ್ಥಿರಾಂಕ std::string> ಪೂರ್ವಪ್ರತ್ಯಯ_2(YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ create_id_ಪೂರ್ವಪ್ರತ್ಯಯ(location.file_name(), location.ಸಾಲು(), func)); \
		ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_2.get(); \
	}(ಮೂಲ_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::ಗಮ್ಯ_string(YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ autoidx++)}}

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
