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

#अगर_अघोषित YOSYS_COMMON_H
#घोषणा YOSYS_COMMON_H

#समावेश <array>
#समावेश <atomic>
#समावेश <map>
#समावेश <set>
#समावेश <tuple>
#समावेश <vector>
#समावेश <string>
#समावेश <algorithm>
#समावेश <functional>
#समावेश <unordered_map>
#समावेश <unordered_set>
#समावेश <initializer_list>
#समावेश <variant>
#समावेश <optional>
#समावेश <stdexcept>
#समावेश <memory>
#समावेश <cmath>
#समावेश <cstddef>

#समावेश <sstream>
#समावेश <fstream>
#समावेश <istream>
#समावेश <ostream>
#समावेश <iostream>

#समावेश <version>
#अगर __cpp_lib_source_location >= 201907L
	#समावेश <source_location>
	using std::source_location;
#अन्यथा
	#समावेश <experimental/source_location>
	using std::experimental::source_location;
#पूर्ण_अगर

#समावेश <stdarg.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <stdint.h>
#समावेश <stdio.h>
#समावेश <limits.h>
#समावेश <sys/stat.h>
#समावेश <errno.h>

#अगर_अघोषित _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make प्रणाली, make sure you set all the \
         defines the Yosys Makefile would set क्रम your build configuration.
#पूर्ण_अगर

#समावेश "kernel/yosys_config.h"

#घोषणा FRIEND_TEST(test_case_name, test_name) \
  मित्र class test_case_name##_##test_name##_Test

#अगर_घोषित _WIN32
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
#  define सूची_गढ़ो _mkdir
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
#पूर्ण_अगर

#अगर_अघोषित PATH_MAX
#  define PATH_MAX 4096
#पूर्ण_अगर


#घोषणा YOSYS_NAMESPACE          Yosys
#घोषणा PRIVATE_NAMESPACE_BEGIN  namespace {
#घोषणा PRIVATE_NAMESPACE_END    }
#घोषणा YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#घोषणा YOSYS_NAMESPACE_END      }
#घोषणा YOSYS_NAMESPACE_PREFIX   Yosys::
#घोषणा USING_YOSYS_NAMESPACE    using namespace Yosys;

#अगर defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#या_अगर defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#अन्यथा
#  define YS_ATTRIBUTE(...)
#पूर्ण_अगर

#अगर defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#अन्यथा
#  define YS_MAYBE_UNUSED
#पूर्ण_अगर

#अगर defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#अन्यथा
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#पूर्ण_अगर

#अगर __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#अन्यथा
#  error "C++20 or later compatible compiler is required"
#पूर्ण_अगर

#अगर defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#अन्यथा
#  define YS_COLD
#पूर्ण_अगर

#अगर_घोषित __cpp_consteval
#घोषणा YOSYS_CONSTEVAL consteval
#अन्यथा
// If we can't use consteval we can at least make it constexpr.
#घोषणा YOSYS_CONSTEVAL constexpr
#पूर्ण_अगर

#घोषणा YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__खाता__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] व्योम log_yosys_abort_message(std::string_view file, पूर्णांक पंक्ति, std::string_view func, std::string_view message);

[[noreturn]]
व्योम log_assert_failure(स्थिर अक्षर *व्यंजक, source_location location);
#अगर_अघोषित न_संशोधन
अटल अंतरभूत व्योम log_assert_worker(bool cond, स्थिर अक्षर *व्यंजक, source_location location = source_location::current()) {
	अगर (!cond) log_assert_failure(व्यंजक, location);
}
#  define log_assert(assert_व्यंजक) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_व्यंजक, #_assert_expr_)
#अन्यथा
#  define log_assert(assert_व्यंजक) करो { अगर (0) { (व्योम)(assert_व्यंजक); } } जबतक(0)
#पूर्ण_अगर
YOSYS_NAMESPACE_END

#समावेश "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#अगर_घोषित HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#अन्यथा
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#पूर्ण_अगर


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
काष्ठा shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(स्थिर अक्षर *s) { content = std::shared_ptr<string>(new string(s)); }
	स्थिर अक्षर *c_str() स्थिर { वापस content->c_str(); }
	स्थिर string &str() स्थिर { वापस *content; }
	bool चालक==(स्थिर shared_str &other) स्थिर { वापस *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
		h.eat(*content);
		वापस h;
	}
};

namespace RTLIL {
	काष्ठा Const;
	काष्ठा SigBit;
	काष्ठा SigSpec;
	काष्ठा Wire;
	काष्ठा Cell;
	काष्ठा Memory;
	काष्ठा Process;
	काष्ठा Module;
	काष्ठा Design;
	काष्ठा Monitor;
	काष्ठा Selection;
	काष्ठा SigChunk;
	काष्ठा PooledName;
	क्रमागत State : अचिन्हित अक्षर;

	प्रकार std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	काष्ठा Ast_गाँठ;
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
	ढाँचा<> काष्ठा hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::Process*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::Module*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::Design*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<AST::Ast_गाँठ*> : hash_obj_ops {};

	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Wire*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Cell*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Memory*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Process*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Module*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Design*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर RTLIL::Monitor*> : hash_obj_ops {};
	ढाँचा<> काष्ठा hash_ops<स्थिर AST::Ast_गाँठ*> : hash_obj_ops {};
}

व्योम memhasher_on();
व्योम memhasher_off();
व्योम memhasher_do();

बाह्य bool memhasher_active;
अंतरभूत व्योम memhasher() { अगर (memhasher_active) memhasher_do(); }

व्योम yosys_banner();
पूर्णांक ceil_log2(पूर्णांक x) YS_ATTRIBUTE(स्थिर);

ढाँचा<typename T> पूर्णांक Get_आकार(स्थिर T &obj) { वापस obj.आकार(); }
अंतरभूत पूर्णांक Get_आकार(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
काष्ठा Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	अटल bool active() { वापस active_; }
निजी:
	अटल bool active_;
};

काष्ठा Autoidx {
	Autoidx(पूर्णांक मूल्य) : मूल्य(मूल्य) {}
	चालक पूर्णांक() स्थिर { वापस मूल्य; }
	व्योम ensure_at_least(पूर्णांक v);
	पूर्णांक चालक++(पूर्णांक);
निजी:
	पूर्णांक मूल्य;
};

बाह्य Autoidx autoidx;
बाह्य पूर्णांक yosys_xtrace;
बाह्य bool yosys_write_versions;

स्थिर std::string *create_id_prefix(std::string_view file, पूर्णांक पंक्ति, std::string_view func);

#घोषणा NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> स्थिर std::string * { \
		अटल std::unique_ptr<स्थिर std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.पंक्ति(), func)); \
		वापस prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#घोषणा NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> स्थिर std::string * { \
		अटल std::unique_ptr<स्थिर std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.पंक्ति(), func)); \
		वापस prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#पूर्ण_अगर
