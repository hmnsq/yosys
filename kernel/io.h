#include <string>
#include <stdarg.h>
#include <type_traits>
#include "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

sa_nracanaa IdString;

anta_hstha std::string vstringf(const var_na *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	const පූර්ණ_සංඛ්‍යා kBufSize = 128;
	var_na buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	පූර්ණ_සංඛ්‍යා n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	නම් (n < kBufSize)
		ආපසු std::string(buf);

	std::string string;
	var_na *str = NULL;
#නම් defined(_WIN32) || defined(__CYGWIN__)
	පූර්ණ_සංඛ්‍යා sz = 2 * kBufSize, rc;
	අතරතුර (1) {
		va_copy(apc, ap);
		str = (var_na *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		නම් (rc >= 0 && rc < sz)
			bha_nga;
		sz *= 2;
	}
	නම් (str != NULL) {
		string = str;
		free(str);
	}
	ආපසු string;
#නැතහොත්
	නම් (vasprintf(&str, fmt, ap) < 0)
		str = NULL;
	නම් (str != NULL) {
		string = str;
		free(str);
	}
	ආපසු string;
#endif
}

ga_nanaa ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(var_na ch, var_na prev_ch)
{
	switch (ch) {
	pak_sa 'd':
	pak_sa 'i':
		ආපසු CONVSPEC_SIGNED_INT;
	pak_sa 'o':
	pak_sa 'u':
	pak_sa 'x':
	pak_sa 'X':
	pak_sa 'm':
		ආපසු CONVSPEC_UNSIGNED_INT;
	pak_sa 'c':
		නම් (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ආපසු CONVSPEC_ERROR;
		}
		ආපසු CONVSPEC_UNSIGNED_INT;
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		ආපසු CONVSPEC_DOUBLE;
	pak_sa 's':
		නම් (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ආපසු CONVSPEC_ERROR;
		}
		ආපසු CONVSPEC_CHAR_PTR;
	pak_sa 'p':
		ආපසු CONVSPEC_VOID_PTR;
	pak_sa '$': // positional parameters
	pak_sa 'n':
	pak_sa 'S':
		ආපසු CONVSPEC_ERROR;
	default:
		ආපසු CONVSPEC_NONE;
	}
}

ga_nanaa class DynamicIntCount : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
sa_nracanaa FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	පූර්ණ_සංඛ්‍යා start;
	// The end offset of the conversion spec in the format string.
	පූර්ණ_සංඛ්‍යා end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	DynamicIntCount num_dynamic_ints;
};

// Ensure there is no format spec.
constexpr void ensure_no_format_spec(std::string_view fmt, පූර්ණ_සංඛ්‍යා index, bool *has_escapes)
{
	පූර්ණ_සංඛ්‍යා fmt_size = static_cast<පූර්ණ_සංඛ්‍යා>(fmt.size());
	// A trailing '%' is not a format spec.
	අතරතුර (index + 1 < fmt_size) {
		නම් (fmt[index] != '%') {
			++index;
			continue;
		}
		නම් (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, පූර්ණ_සංඛ්‍යා fmt_start, bool *has_escapes)
{
	පූර්ණ_සංඛ්‍යා index = fmt_start;
	පූර්ණ_සංඛ්‍යා fmt_size = static_cast<පූර්ණ_සංඛ්‍යා>(fmt.size());
	අතරතුර (index < fmt_size) {
		නම් (fmt[index] != '%') {
			++index;
			continue;
		}
		පූර්ණ_සංඛ්‍යා p = index + 1;
		uint8_t num_dynamic_ints = 0;
		අතරතුර (true) {
			නම් (p == fmt_size) {
				ආපසු {0, 0, CONVSPEC_NONE, DynamicIntCount::NONE};
			}
			නම් (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				bha_nga;
			}
			නම් (fmt[p] == '*') {
				නම් (num_dynamic_ints >= 2) {
					ආපසු {0, 0, CONVSPEC_ERROR, DynamicIntCount::NONE};
				}
				++num_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			නම් (spec != CONVSPEC_NONE) {
				ආපසු {index, p + 1, spec, static_cast<DynamicIntCount>(num_dynamic_ints)};
			}
			++p;
		}
	}
	ආපසු {0, 0, CONVSPEC_NONE, DynamicIntCount::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<parimaa_na...(Args) == 0>::type
check_format(std::string_view fmt, පූර්ණ_සංඛ්‍යා fmt_start, bool *has_escapes, FoundFormatSpec*, DynamicIntCount)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
static auto has_name_member_imp(පූර්ණ_සංඛ්‍යා)
	-> decltype(static_cast<const IdString>(std::declval<T>().name), std::true_type{});

template <class T>
static auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
sa_nracanaa has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
static auto ptr_has_name_member_imp(පූර්ණ_සංඛ්‍යා)
	-> decltype(static_cast<const IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
static auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
sa_nracanaa ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr void check_format(std::string_view fmt, පූර්ණ_සංඛ්‍යා fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        DynamicIntCount int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	නම් (found.num_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		නම් constexpr (!std::is_convertible_v<Arg, පූර්ණ_සංඛ්‍යා>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<DynamicIntCount>(static_cast<uint8_t>(int_args_consumed) + 1));
		ආපසු;
	}
	switch (found.spec) {
	pak_sa CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		bha_nga;
	pak_sa CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		bha_nga;
	pak_sa CONVSPEC_SIGNED_INT:
		නම් constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_UNSIGNED_INT:
		නම් constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_DOUBLE:
		නම් constexpr (!std::is_convertible_v<Arg, dviguu_na>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_CHAR_PTR:
		නම් constexpr (!std::is_convertible_v<Arg, const var_na *> &&
		        !std::is_convertible_v<Arg, const std::string &> &&
			!std::is_convertible_v<Arg, const std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_VOID_PTR:
		නම් constexpr (!std::is_convertible_v<Arg, const void *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		bha_nga;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, DynamicIntCount::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
void format_emit_long_long(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
void format_emit_unsigned_long_long(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
void format_emit_double(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
void format_emit_char_ptr(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
void format_emit_string(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
void format_emit_string_view(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
void format_emit_void_ptr(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
anta_hstha void format_emit_one(std::string &result, std::string_view fmt, const FoundFormatSpec &ffspec,
	පූර්ණ_සංඛ්‍යා *dynamic_ints, const Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	DynamicIntCount num_dynamic_ints = ffspec.num_dynamic_ints;
	switch (ffspec.spec) {
	pak_sa CONVSPEC_SIGNED_INT:
		නම් constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		bha_nga;
	pak_sa CONVSPEC_UNSIGNED_INT:
		නම් constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		bha_nga;
	pak_sa CONVSPEC_DOUBLE:
		නම් constexpr (std::is_convertible_v<Arg, dviguu_na>) {
			dviguu_na s = arg;
			format_emit_double(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		bha_nga;
	pak_sa CONVSPEC_CHAR_PTR:
		නම් constexpr (std::is_convertible_v<Arg, const var_na *>) {
			const var_na *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		නම් constexpr (std::is_convertible_v<Arg, const std::string &>) {
			const std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		නම් constexpr (std::is_convertible_v<Arg, const std::string_view &>) {
			const std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		නම් constexpr (has_name_member<Arg>()) {
			const std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		නම් constexpr (ptr_has_name_member<Arg>()) {
			const std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		bha_nga;
	pak_sa CONVSPEC_VOID_PTR:
		නම් constexpr (std::is_convertible_v<Arg, const void *>) {
			const void *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, num_dynamic_ints, s);
			ආපසු;
		}
		bha_nga;
	default:
		bha_nga;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
void format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

anta_hstha void format_emit(std::string &result, std::string_view fmt, පූර්ණ_සංඛ්‍යා fmt_start,
	bool has_escapes, const FoundFormatSpec*, පූර්ණ_සංඛ්‍යා*, DynamicIntCount)
{
	fmt = fmt.substr(fmt_start);
	නම් (has_escapes) {
		format_emit_unescaped(result, fmt);
	} නැතහොත් {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
anta_hstha void format_emit(std::string &result, std::string_view fmt, පූර්ණ_සංඛ්‍යා fmt_start, bool has_escapes,
	const FoundFormatSpec* specs, පූර්ණ_සංඛ්‍යා *dynamic_ints, DynamicIntCount num_dynamic_ints,
	const Arg &arg, const Args &... args)
{
	නම් (specs->num_dynamic_ints > num_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		නම් constexpr (std::is_convertible_v<Arg, පූර්ණ_සංඛ්‍යා>) {
			dynamic_ints[static_cast<uint8_t>(num_dynamic_ints)] = arg;
		} නැතහොත් {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<DynamicIntCount>(static_cast<uint8_t>(num_dynamic_ints) + 1), args...);
		ආපසු;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	නම් (has_escapes) {
		format_emit_unescaped(result, str);
	} නැතහොත් {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, DynamicIntCount::NONE, args...);
}

template <typename... Args>
anta_hstha std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, const FoundFormatSpec* specs, const Args &... args)
{
	std::string result;
	පූර්ණ_සංඛ්‍යා dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, DynamicIntCount::NONE, args...);
	ආපසු result;
}
template <>
anta_hstha std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, const FoundFormatSpec*)
{
	නම් (!has_escapes) {
		ආපසු std::string(fmt);
	}
	ආපසු unescape_format_string(fmt);
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
	YOSYS_CONSTEVAL FmtString(const var_na *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, DynamicIntCount::NONE);
	}
	std::string format(const Args &... args)
	{
		ආපසු format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() const { ආපසු fmt; }
private:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[parimaa_na...(Args) ? parimaa_na...(Args) : 1] = {};
};

template <typename T> sa_nracanaa WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
anta_hstha std::string stringf(FmtString<TypeIdentity<Args>...> fmt, const Args &... args)
{
	ආපසු fmt.format(args...);
}

පූර්ණ_සංඛ්‍යා readsome(std::istream &f, var_na *s, පූර්ණ_සංඛ්‍යා n);
std::string next_token(std::string &text, const var_na *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(const std::string &text, const var_na *sep = " \t\r\n");
bool patmatch(const var_na *pattern, const var_na *string);
#නම් !defined(YOSYS_DISABLE_SPAWN)
පූර්ණ_සංඛ්‍යා run_command(const std::string &command, std::function<void(const std::string&)> process_line = std::function<void(const std::string&)>());
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
sa_nracanaa ScopedFileLock {
	ScopedFileLock(const std::string &path);
	~ScopedFileLock();
	ScopedFileLock(const ScopedFileLock &) = delete;
	ScopedFileLock &operator=(const ScopedFileLock &) = delete;
private:
#නම් defined(_WIN32)
	void *handle = nullptr;
#elif !defined(__wasi__)
	පූර්ණ_සංඛ්‍යා fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
