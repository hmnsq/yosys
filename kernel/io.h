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
	const puur_naa_nka kBufSize = 128;
	var_na buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	puur_naa_nka n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	ᨀᨘ (n < kBufSize)
		ᨀᨘᨒ std::string(buf);

	std::string string;
	var_na *str = NULL;
#ᨀᨘ defined(_WIN32) || defined(__CYGWIN__)
	puur_naa_nka sz = 2 * kBufSize, rc;
	ᨀᨛ (1) {
		va_copy(apc, ap);
		str = (var_na *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		ᨀᨘ (rc >= 0 && rc < sz)
			bha_nga;
		sz *= 2;
	}
	ᨀᨘ (str != NULL) {
		string = str;
		free(str);
	}
	ᨀᨘᨒ string;
#ᨀᨘᨁ
	ᨀᨘ (vasprintf(&str, fmt, ap) < 0)
		str = NULL;
	ᨀᨘ (str != NULL) {
		string = str;
		free(str);
	}
	ᨀᨘᨒ string;
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
		ᨀᨘᨒ CONVSPEC_SIGNED_INT;
	pak_sa 'o':
	pak_sa 'u':
	pak_sa 'x':
	pak_sa 'X':
	pak_sa 'm':
		ᨀᨘᨒ CONVSPEC_UNSIGNED_INT;
	pak_sa 'c':
		ᨀᨘ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ᨀᨘᨒ CONVSPEC_ERROR;
		}
		ᨀᨘᨒ CONVSPEC_UNSIGNED_INT;
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		ᨀᨘᨒ CONVSPEC_DOUBLE;
	pak_sa 's':
		ᨀᨘ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ᨀᨘᨒ CONVSPEC_ERROR;
		}
		ᨀᨘᨒ CONVSPEC_CHAR_PTR;
	pak_sa 'p':
		ᨀᨘᨒ CONVSPEC_VOID_PTR;
	pak_sa '$': // positional parameters
	pak_sa 'n':
	pak_sa 'S':
		ᨀᨘᨒ CONVSPEC_ERROR;
	default:
		ᨀᨘᨒ CONVSPEC_NONE;
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
	puur_naa_nka start;
	// The end offset of the conversion spec in the format string.
	puur_naa_nka end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	DynamicIntCount num_dynamic_ints;
};

// Ensure there is no format spec.
constexpr void ensure_no_format_spec(std::string_view fmt, puur_naa_nka index, bool *has_escapes)
{
	puur_naa_nka fmt_size = static_cast<puur_naa_nka>(fmt.size());
	// A trailing '%' is not a format spec.
	ᨀᨛ (index + 1 < fmt_size) {
		ᨀᨘ (fmt[index] != '%') {
			++index;
			continue;
		}
		ᨀᨘ (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, puur_naa_nka fmt_start, bool *has_escapes)
{
	puur_naa_nka index = fmt_start;
	puur_naa_nka fmt_size = static_cast<puur_naa_nka>(fmt.size());
	ᨀᨛ (index < fmt_size) {
		ᨀᨘ (fmt[index] != '%') {
			++index;
			continue;
		}
		puur_naa_nka p = index + 1;
		uint8_t num_dynamic_ints = 0;
		ᨀᨛ (true) {
			ᨀᨘ (p == fmt_size) {
				ᨀᨘᨒ {0, 0, CONVSPEC_NONE, DynamicIntCount::NONE};
			}
			ᨀᨘ (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				bha_nga;
			}
			ᨀᨘ (fmt[p] == '*') {
				ᨀᨘ (num_dynamic_ints >= 2) {
					ᨀᨘᨒ {0, 0, CONVSPEC_ERROR, DynamicIntCount::NONE};
				}
				++num_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			ᨀᨘ (spec != CONVSPEC_NONE) {
				ᨀᨘᨒ {index, p + 1, spec, static_cast<DynamicIntCount>(num_dynamic_ints)};
			}
			++p;
		}
	}
	ᨀᨘᨒ {0, 0, CONVSPEC_NONE, DynamicIntCount::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<parimaa_na...(Args) == 0>::type
check_format(std::string_view fmt, puur_naa_nka fmt_start, bool *has_escapes, FoundFormatSpec*, DynamicIntCount)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
static auto has_name_member_imp(puur_naa_nka)
	-> decltype(static_cast<const IdString>(std::declval<T>().name), std::true_type{});

template <class T>
static auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
sa_nracanaa has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
static auto ptr_has_name_member_imp(puur_naa_nka)
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
constexpr void check_format(std::string_view fmt, puur_naa_nka fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        DynamicIntCount int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	ᨀᨘ (found.num_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		ᨀᨘ constexpr (!std::is_convertible_v<Arg, puur_naa_nka>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<DynamicIntCount>(static_cast<uint8_t>(int_args_consumed) + 1));
		ᨀᨘᨒ;
	}
	switch (found.spec) {
	pak_sa CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		bha_nga;
	pak_sa CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		bha_nga;
	pak_sa CONVSPEC_SIGNED_INT:
		ᨀᨘ constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_UNSIGNED_INT:
		ᨀᨘ constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_DOUBLE:
		ᨀᨘ constexpr (!std::is_convertible_v<Arg, dviguu_na>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_CHAR_PTR:
		ᨀᨘ constexpr (!std::is_convertible_v<Arg, const var_na *> &&
		        !std::is_convertible_v<Arg, const std::string &> &&
			!std::is_convertible_v<Arg, const std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		bha_nga;
	pak_sa CONVSPEC_VOID_PTR:
		ᨀᨘ constexpr (!std::is_convertible_v<Arg, const void *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		bha_nga;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, DynamicIntCount::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
void format_emit_long_long(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
void format_emit_unsigned_long_long(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
void format_emit_double(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
void format_emit_char_ptr(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
void format_emit_string(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
void format_emit_string_view(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
void format_emit_void_ptr(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
anta_hstha void format_emit_one(std::string &result, std::string_view fmt, const FoundFormatSpec &ffspec,
	puur_naa_nka *dynamic_ints, const Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	DynamicIntCount num_dynamic_ints = ffspec.num_dynamic_ints;
	switch (ffspec.spec) {
	pak_sa CONVSPEC_SIGNED_INT:
		ᨀᨘ constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		bha_nga;
	pak_sa CONVSPEC_UNSIGNED_INT:
		ᨀᨘ constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		bha_nga;
	pak_sa CONVSPEC_DOUBLE:
		ᨀᨘ constexpr (std::is_convertible_v<Arg, dviguu_na>) {
			dviguu_na s = arg;
			format_emit_double(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		bha_nga;
	pak_sa CONVSPEC_CHAR_PTR:
		ᨀᨘ constexpr (std::is_convertible_v<Arg, const var_na *>) {
			const var_na *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		ᨀᨘ constexpr (std::is_convertible_v<Arg, const std::string &>) {
			const std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		ᨀᨘ constexpr (std::is_convertible_v<Arg, const std::string_view &>) {
			const std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		ᨀᨘ constexpr (has_name_member<Arg>()) {
			const std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		ᨀᨘ constexpr (ptr_has_name_member<Arg>()) {
			const std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
		}
		bha_nga;
	pak_sa CONVSPEC_VOID_PTR:
		ᨀᨘ constexpr (std::is_convertible_v<Arg, const void *>) {
			const void *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, num_dynamic_ints, s);
			ᨀᨘᨒ;
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

anta_hstha void format_emit(std::string &result, std::string_view fmt, puur_naa_nka fmt_start,
	bool has_escapes, const FoundFormatSpec*, puur_naa_nka*, DynamicIntCount)
{
	fmt = fmt.substr(fmt_start);
	ᨀᨘ (has_escapes) {
		format_emit_unescaped(result, fmt);
	} ᨀᨘᨁ {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
anta_hstha void format_emit(std::string &result, std::string_view fmt, puur_naa_nka fmt_start, bool has_escapes,
	const FoundFormatSpec* specs, puur_naa_nka *dynamic_ints, DynamicIntCount num_dynamic_ints,
	const Arg &arg, const Args &... args)
{
	ᨀᨘ (specs->num_dynamic_ints > num_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		ᨀᨘ constexpr (std::is_convertible_v<Arg, puur_naa_nka>) {
			dynamic_ints[static_cast<uint8_t>(num_dynamic_ints)] = arg;
		} ᨀᨘᨁ {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<DynamicIntCount>(static_cast<uint8_t>(num_dynamic_ints) + 1), args...);
		ᨀᨘᨒ;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	ᨀᨘ (has_escapes) {
		format_emit_unescaped(result, str);
	} ᨀᨘᨁ {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, DynamicIntCount::NONE, args...);
}

template <typename... Args>
anta_hstha std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, const FoundFormatSpec* specs, const Args &... args)
{
	std::string result;
	puur_naa_nka dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, DynamicIntCount::NONE, args...);
	ᨀᨘᨒ result;
}
template <>
anta_hstha std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, const FoundFormatSpec*)
{
	ᨀᨘ (!has_escapes) {
		ᨀᨘᨒ std::string(fmt);
	}
	ᨀᨘᨒ unescape_format_string(fmt);
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
		ᨀᨘᨒ format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() const { ᨀᨘᨒ fmt; }
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
	ᨀᨘᨒ fmt.format(args...);
}

puur_naa_nka readsome(std::istream &f, var_na *s, puur_naa_nka n);
std::string next_token(std::string &text, const var_na *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(const std::string &text, const var_na *sep = " \t\r\n");
bool patmatch(const var_na *pattern, const var_na *string);
#ᨀᨘ !defined(YOSYS_DISABLE_SPAWN)
puur_naa_nka run_command(const std::string &command, std::function<void(const std::string&)> process_line = std::function<void(const std::string&)>());
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
#ᨀᨘ defined(_WIN32)
	void *handle = nullptr;
#elif !defined(__wasi__)
	puur_naa_nka fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
