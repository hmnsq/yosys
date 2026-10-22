#include <string>
#include <stdarg.h>
#include <type_traits>
#include "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

struct IdString;

inline std::string vstringf(const char *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	const 정수 kBufSize = 128;
	char buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	정수 n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	만약 (n < kBufSize)
		반환 std::string(buf);

	std::string string;
	char *str = NULL;
#만약 defined(_WIN32) || defined(__CYGWIN__)
	정수 sz = 2 * kBufSize, rc;
	동안 (1) {
		va_copy(apc, ap);
		str = (char *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		만약 (rc >= 0 && rc < sz)
			break;
		sz *= 2;
	}
	만약 (str != NULL) {
		string = str;
		free(str);
	}
	반환 string;
#아니면
	만약 (vasprintf(&str, fmt, ap) < 0)
		str = NULL;
	만약 (str != NULL) {
		string = str;
		free(str);
	}
	반환 string;
#endif
}

enum ConversionSpecifier : uint8_t
{
	CONVSPEC_NONE,
	// Specifier not understood/supported
	CONVSPEC_ERROR,
	// Consumes a "long long"
	CONVSPEC_SIGNED_INT,
	// Consumes a "unsigned long long"
	CONVSPEC_UNSIGNED_INT,
	// Consumes a "double"
	CONVSPEC_DOUBLE,
	// Consumes a "const char*" or other string type
	CONVSPEC_CHAR_PTR,
	// Consumes a "void*"
	CONVSPEC_VOID_PTR,
};

constexpr ConversionSpecifier parse_conversion_specifier(char ch, char prev_ch)
{
	switch (ch) {
	case 'd':
	case 'i':
		반환 CONVSPEC_SIGNED_INT;
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	case 'm':
		반환 CONVSPEC_UNSIGNED_INT;
	case 'c':
		만약 (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			반환 CONVSPEC_ERROR;
		}
		반환 CONVSPEC_UNSIGNED_INT;
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
	case 'a':
	case 'A':
		반환 CONVSPEC_DOUBLE;
	case 's':
		만약 (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			반환 CONVSPEC_ERROR;
		}
		반환 CONVSPEC_CHAR_PTR;
	case 'p':
		반환 CONVSPEC_VOID_PTR;
	case '$': // positional parameters
	case 'n':
	case 'S':
		반환 CONVSPEC_ERROR;
	default:
		반환 CONVSPEC_NONE;
	}
}

enum class DynamicIntCount : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
struct FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	정수 start;
	// The end offset of the conversion spec in the format string.
	정수 end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	DynamicIntCount num_dynamic_ints;
};

// Ensure there is no format spec.
constexpr void ensure_no_format_spec(std::string_view fmt, 정수 index, bool *has_escapes)
{
	정수 fmt_size = static_cast<정수>(fmt.size());
	// A trailing '%' is not a format spec.
	동안 (index + 1 < fmt_size) {
		만약 (fmt[index] != '%') {
			++index;
			continue;
		}
		만약 (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, 정수 fmt_start, bool *has_escapes)
{
	정수 index = fmt_start;
	정수 fmt_size = static_cast<정수>(fmt.size());
	동안 (index < fmt_size) {
		만약 (fmt[index] != '%') {
			++index;
			continue;
		}
		정수 p = index + 1;
		uint8_t num_dynamic_ints = 0;
		동안 (true) {
			만약 (p == fmt_size) {
				반환 {0, 0, CONVSPEC_NONE, DynamicIntCount::NONE};
			}
			만약 (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				break;
			}
			만약 (fmt[p] == '*') {
				만약 (num_dynamic_ints >= 2) {
					반환 {0, 0, CONVSPEC_ERROR, DynamicIntCount::NONE};
				}
				++num_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			만약 (spec != CONVSPEC_NONE) {
				반환 {index, p + 1, spec, static_cast<DynamicIntCount>(num_dynamic_ints)};
			}
			++p;
		}
	}
	반환 {0, 0, CONVSPEC_NONE, DynamicIntCount::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<sizeof...(Args) == 0>::type
check_format(std::string_view fmt, 정수 fmt_start, bool *has_escapes, FoundFormatSpec*, DynamicIntCount)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
static auto has_name_member_imp(정수)
	-> decltype(static_cast<const IdString>(std::declval<T>().name), std::true_type{});

template <class T>
static auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
struct has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
static auto ptr_has_name_member_imp(정수)
	-> decltype(static_cast<const IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
static auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
struct ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr void check_format(std::string_view fmt, 정수 fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        DynamicIntCount int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	만약 (found.num_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		만약 constexpr (!std::is_convertible_v<Arg, 정수>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<DynamicIntCount>(static_cast<uint8_t>(int_args_consumed) + 1));
		반환;
	}
	switch (found.spec) {
	case CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		break;
	case CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		break;
	case CONVSPEC_SIGNED_INT:
		만약 constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		break;
	case CONVSPEC_UNSIGNED_INT:
		만약 constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		break;
	case CONVSPEC_DOUBLE:
		만약 constexpr (!std::is_convertible_v<Arg, double>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		break;
	case CONVSPEC_CHAR_PTR:
		만약 constexpr (!std::is_convertible_v<Arg, const char *> &&
		        !std::is_convertible_v<Arg, const std::string &> &&
			!std::is_convertible_v<Arg, const std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		break;
	case CONVSPEC_VOID_PTR:
		만약 constexpr (!std::is_convertible_v<Arg, const void *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		break;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, DynamicIntCount::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
void format_emit_long_long(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
void format_emit_unsigned_long_long(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
void format_emit_double(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, double arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
void format_emit_char_ptr(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const char *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
void format_emit_string(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
void format_emit_string_view(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
void format_emit_void_ptr(std::string &result, std::string_view spec, 정수 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
inline void format_emit_one(std::string &result, std::string_view fmt, const FoundFormatSpec &ffspec,
	정수 *dynamic_ints, const Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	DynamicIntCount num_dynamic_ints = ffspec.num_dynamic_ints;
	switch (ffspec.spec) {
	case CONVSPEC_SIGNED_INT:
		만약 constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		break;
	case CONVSPEC_UNSIGNED_INT:
		만약 constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		break;
	case CONVSPEC_DOUBLE:
		만약 constexpr (std::is_convertible_v<Arg, double>) {
			double s = arg;
			format_emit_double(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		break;
	case CONVSPEC_CHAR_PTR:
		만약 constexpr (std::is_convertible_v<Arg, const char *>) {
			const char *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		만약 constexpr (std::is_convertible_v<Arg, const std::string &>) {
			const std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		만약 constexpr (std::is_convertible_v<Arg, const std::string_view &>) {
			const std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		만약 constexpr (has_name_member<Arg>()) {
			const std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		만약 constexpr (ptr_has_name_member<Arg>()) {
			const std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		break;
	case CONVSPEC_VOID_PTR:
		만약 constexpr (std::is_convertible_v<Arg, const void *>) {
			const void *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, num_dynamic_ints, s);
			반환;
		}
		break;
	default:
		break;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
void format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

inline void format_emit(std::string &result, std::string_view fmt, 정수 fmt_start,
	bool has_escapes, const FoundFormatSpec*, 정수*, DynamicIntCount)
{
	fmt = fmt.substr(fmt_start);
	만약 (has_escapes) {
		format_emit_unescaped(result, fmt);
	} 아니면 {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
inline void format_emit(std::string &result, std::string_view fmt, 정수 fmt_start, bool has_escapes,
	const FoundFormatSpec* specs, 정수 *dynamic_ints, DynamicIntCount num_dynamic_ints,
	const Arg &arg, const Args &... args)
{
	만약 (specs->num_dynamic_ints > num_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		만약 constexpr (std::is_convertible_v<Arg, 정수>) {
			dynamic_ints[static_cast<uint8_t>(num_dynamic_ints)] = arg;
		} 아니면 {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<DynamicIntCount>(static_cast<uint8_t>(num_dynamic_ints) + 1), args...);
		반환;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	만약 (has_escapes) {
		format_emit_unescaped(result, str);
	} 아니면 {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, DynamicIntCount::NONE, args...);
}

template <typename... Args>
inline std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, const FoundFormatSpec* specs, const Args &... args)
{
	std::string result;
	정수 dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, DynamicIntCount::NONE, args...);
	반환 result;
}
template <>
inline std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, const FoundFormatSpec*)
{
	만약 (!has_escapes) {
		반환 std::string(fmt);
	}
	반환 unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
public:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(const char *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, DynamicIntCount::NONE);
	}
	std::string format(const Args &... args)
	{
		반환 format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() const { 반환 fmt; }
private:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[sizeof...(Args) ? sizeof...(Args) : 1] = {};
};

template <typename T> struct WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
inline std::string stringf(FmtString<TypeIdentity<Args>...> fmt, const Args &... args)
{
	반환 fmt.format(args...);
}

정수 readsome(std::istream &f, char *s, 정수 n);
std::string next_token(std::string &text, const char *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(const std::string &text, const char *sep = " \t\r\n");
bool patmatch(const char *pattern, const char *string);
#만약 !defined(YOSYS_DISABLE_SPAWN)
정수 run_command(const std::string &command, std::function<void(const std::string&)> process_line = std::function<void(const std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(const std::string& filename, bool is_exec = false);
bool check_directory_exists(const std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
void remove_directory(std::string dirname);
bool create_directory(const std::string& dirname);
bool make_private_directory(const std::string& dirname);
std::string make_user_tmpdir(const std::string& name);
std::string escape_filename_spaces(const std::string& filename);
void append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(const std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
struct ScopedFileLock {
	ScopedFileLock(const std::string &path);
	~ScopedFileLock();
	ScopedFileLock(const ScopedFileLock &) = delete;
	ScopedFileLock &operator=(const ScopedFileLock &) = delete;
private:
#만약 defined(_WIN32)
	void *handle = nullptr;
#elif !defined(__wasi__)
	정수 fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
