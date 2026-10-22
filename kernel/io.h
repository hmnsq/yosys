#অন্তর্ভাব <string>
#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <type_traits>
#অন্তর্ভাব "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

সংরচনা IdString;

অন্তঃস্থ std::string vstringf(ধ্রুব বর্ণ *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ধ্রুব পূর্ণাঙ্ক kBufSize = 128;
	বর্ণ buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	পূর্ণাঙ্ক n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	যদি (n < kBufSize)
		প্রত্যাগম std::string(buf);

	std::string string;
	বর্ণ *str = রিক্ত;
#যদি defined(_WIN32) || defined(__CYGWIN__)
	পূর্ণাঙ্ক sz = 2 * kBufSize, rc;
	যাবত্ (1) {
		va_copy(apc, ap);
		str = (বর্ণ *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		যদি (rc >= 0 && rc < sz)
			ভঙ্গ;
		sz *= 2;
	}
	যদি (str != রিক্ত) {
		string = str;
		মুক্ত(str);
	}
	প্রত্যাগম string;
#অন্যথা
	যদি (vasprintf(&str, fmt, ap) < 0)
		str = রিক্ত;
	যদি (str != রিক্ত) {
		string = str;
		মুক্ত(str);
	}
	প্রত্যাগম string;
#endif
}

গণনা ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(বর্ণ ch, বর্ণ prev_ch)
{
	বিকল্প (ch) {
	পক্ষ 'd':
	পক্ষ 'i':
		প্রত্যাগম CONVSPEC_SIGNED_INT;
	পক্ষ 'o':
	পক্ষ 'u':
	পক্ষ 'x':
	পক্ষ 'X':
	পক্ষ 'm':
		প্রত্যাগম CONVSPEC_UNSIGNED_INT;
	পক্ষ 'c':
		যদি (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			প্রত্যাগম CONVSPEC_ERROR;
		}
		প্রত্যাগম CONVSPEC_UNSIGNED_INT;
	পক্ষ 'e':
	পক্ষ 'E':
	পক্ষ 'f':
	পক্ষ 'F':
	পক্ষ 'g':
	পক্ষ 'G':
	পক্ষ 'a':
	পক্ষ 'A':
		প্রত্যাগম CONVSPEC_DOUBLE;
	পক্ষ 's':
		যদি (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			প্রত্যাগম CONVSPEC_ERROR;
		}
		প্রত্যাগম CONVSPEC_CHAR_PTR;
	পক্ষ 'p':
		প্রত্যাগম CONVSPEC_VOID_PTR;
	পক্ষ '$': // positional parameters
	পক্ষ 'n':
	পক্ষ 'S':
		প্রত্যাগম CONVSPEC_ERROR;
	স্বভাব:
		প্রত্যাগম CONVSPEC_NONE;
	}
}

গণনা class Dynamic_Int_গুনতি : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
সংরচনা FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	পূর্ণাঙ্ক start;
	// The end offset of the conversion spec in the format string.
	পূর্ণাঙ্ক end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints;
};

// Ensure there is no format spec.
constexpr শূন্য ensure_no_format_spec(std::string_view fmt, পূর্ণাঙ্ক index, bool *has_escapes)
{
	পূর্ণাঙ্ক fmt_size = static_cast<পূর্ণাঙ্ক>(fmt.size());
	// A trailing '%' is not a format spec.
	যাবত্ (index + 1 < fmt_size) {
		যদি (fmt[index] != '%') {
			++index;
			অনুবর্ত;
		}
		যদি (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, পূর্ণাঙ্ক fmt_start, bool *has_escapes)
{
	পূর্ণাঙ্ক index = fmt_start;
	পূর্ণাঙ্ক fmt_size = static_cast<পূর্ণাঙ্ক>(fmt.size());
	যাবত্ (index < fmt_size) {
		যদি (fmt[index] != '%') {
			++index;
			অনুবর্ত;
		}
		পূর্ণাঙ্ক p = index + 1;
		uint8_t অঙ্ক_dynamic_ints = 0;
		যাবত্ (true) {
			যদি (p == fmt_size) {
				প্রত্যাগম {0, 0, CONVSPEC_NONE, Dynamic_Int_গুনতি::NONE};
			}
			যদি (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				ভঙ্গ;
			}
			যদি (fmt[p] == '*') {
				যদি (অঙ্ক_dynamic_ints >= 2) {
					প্রত্যাগম {0, 0, CONVSPEC_ERROR, Dynamic_Int_গুনতি::NONE};
				}
				++অঙ্ক_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			যদি (spec != CONVSPEC_NONE) {
				প্রত্যাগম {index, p + 1, spec, static_cast<Dynamic_Int_গুনতি>(অঙ্ক_dynamic_ints)};
			}
			++p;
		}
	}
	প্রত্যাগম {0, 0, CONVSPEC_NONE, Dynamic_Int_গুনতি::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<পরিমাণ...(Args) == 0>::type
check_format(std::string_view fmt, পূর্ণাঙ্ক fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_গুনতি)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
স্থির auto has_name_member_imp(পূর্ণাঙ্ক)
	-> decltype(static_cast<ধ্রুব IdString>(std::declval<T>().name), std::true_type{});

template <class T>
স্থির auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
সংরচনা has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
স্থির auto ptr_has_name_member_imp(পূর্ণাঙ্ক)
	-> decltype(static_cast<ধ্রুব IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
স্থির auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
সংরচনা ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr শূন্য check_format(std::string_view fmt, পূর্ণাঙ্ক fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_গুনতি int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	যদি (found.অঙ্ক_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		যদি constexpr (!std::is_convertible_v<Arg, পূর্ণাঙ্ক>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_গুনতি>(static_cast<uint8_t>(int_args_consumed) + 1));
		প্রত্যাগম;
	}
	বিকল্প (found.spec) {
	পক্ষ CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		ভঙ্গ;
	পক্ষ CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		ভঙ্গ;
	পক্ষ CONVSPEC_SIGNED_INT:
		যদি constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		ভঙ্গ;
	পক্ষ CONVSPEC_UNSIGNED_INT:
		যদি constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		ভঙ্গ;
	পক্ষ CONVSPEC_DOUBLE:
		যদি constexpr (!std::is_convertible_v<Arg, দ্বিগুণ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		ভঙ্গ;
	পক্ষ CONVSPEC_CHAR_PTR:
		যদি constexpr (!std::is_convertible_v<Arg, ধ্রুব বর্ণ *> &&
		        !std::is_convertible_v<Arg, ধ্রুব std::string &> &&
			!std::is_convertible_v<Arg, ধ্রুব std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		ভঙ্গ;
	পক্ষ CONVSPEC_VOID_PTR:
		যদি constexpr (!std::is_convertible_v<Arg, ধ্রুব শূন্য *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		ভঙ্গ;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_গুনতি::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
শূন্য format_emit_long_long(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
শূন্য format_emit_unsigned_long_long(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
শূন্য format_emit_double(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, দ্বিগুণ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
শূন্য format_emit_char_ptr(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুব বর্ণ *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
শূন্য format_emit_string(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুব std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
শূন্য format_emit_string_view(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
শূন্য format_emit_void_ptr(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুব শূন্য *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
অন্তঃস্থ শূন্য format_emit_one(std::string &result, std::string_view fmt, ধ্রুব FoundFormatSpec &ffspec,
	পূর্ণাঙ্ক *dynamic_ints, ধ্রুব Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints = ffspec.অঙ্ক_dynamic_ints;
	বিকল্প (ffspec.spec) {
	পক্ষ CONVSPEC_SIGNED_INT:
		যদি constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		ভঙ্গ;
	পক্ষ CONVSPEC_UNSIGNED_INT:
		যদি constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		ভঙ্গ;
	পক্ষ CONVSPEC_DOUBLE:
		যদি constexpr (std::is_convertible_v<Arg, দ্বিগুণ>) {
			দ্বিগুণ s = arg;
			format_emit_double(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		ভঙ্গ;
	পক্ষ CONVSPEC_CHAR_PTR:
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুব বর্ণ *>) {
			ধ্রুব বর্ণ *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুব std::string &>) {
			ধ্রুব std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুব std::string_view &>) {
			ধ্রুব std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		যদি constexpr (has_name_member<Arg>()) {
			ধ্রুব std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		যদি constexpr (ptr_has_name_member<Arg>()) {
			ধ্রুব std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		ভঙ্গ;
	পক্ষ CONVSPEC_VOID_PTR:
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুব শূন্য *>) {
			ধ্রুব শূন্য *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			প্রত্যাগম;
		}
		ভঙ্গ;
	স্বভাব:
		ভঙ্গ;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
শূন্য format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

অন্তঃস্থ শূন্য format_emit(std::string &result, std::string_view fmt, পূর্ণাঙ্ক fmt_start,
	bool has_escapes, ধ্রুব FoundFormatSpec*, পূর্ণাঙ্ক*, Dynamic_Int_গুনতি)
{
	fmt = fmt.substr(fmt_start);
	যদি (has_escapes) {
		format_emit_unescaped(result, fmt);
	} অন্যথা {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
অন্তঃস্থ শূন্য format_emit(std::string &result, std::string_view fmt, পূর্ণাঙ্ক fmt_start, bool has_escapes,
	ধ্রুব FoundFormatSpec* specs, পূর্ণাঙ্ক *dynamic_ints, Dynamic_Int_গুনতি অঙ্ক_dynamic_ints,
	ধ্রুব Arg &arg, ধ্রুব Args &... args)
{
	যদি (specs->অঙ্ক_dynamic_ints > অঙ্ক_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		যদি constexpr (std::is_convertible_v<Arg, পূর্ণাঙ্ক>) {
			dynamic_ints[static_cast<uint8_t>(অঙ্ক_dynamic_ints)] = arg;
		} অন্যথা {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_গুনতি>(static_cast<uint8_t>(অঙ্ক_dynamic_ints) + 1), args...);
		প্রত্যাগম;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	যদি (has_escapes) {
		format_emit_unescaped(result, str);
	} অন্যথা {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_গুনতি::NONE, args...);
}

template <typename... Args>
অন্তঃস্থ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ধ্রুব FoundFormatSpec* specs, ধ্রুব Args &... args)
{
	std::string result;
	পূর্ণাঙ্ক dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_গুনতি::NONE, args...);
	প্রত্যাগম result;
}
template <>
অন্তঃস্থ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ধ্রুব FoundFormatSpec*)
{
	যদি (!has_escapes) {
		প্রত্যাগম std::string(fmt);
	}
	প্রত্যাগম unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
সার্বজনিক:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ধ্রুব বর্ণ *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_গুনতি::NONE);
	}
	std::string format(ধ্রুব Args &... args)
	{
		প্রত্যাগম format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ধ্রুব { প্রত্যাগম fmt; }
গুপ্ত:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[পরিমাণ...(Args) ? পরিমাণ...(Args) : 1] = {};
};

template <typename T> সংরচনা WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
অন্তঃস্থ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	প্রত্যাগম fmt.format(args...);
}

পূর্ণাঙ্ক readsome(std::istream &f, বর্ণ *s, পূর্ণাঙ্ক n);
std::string next_token(std::string &text, ধ্রুব বর্ণ *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ধ্রুব std::string &text, ধ্রুব বর্ণ *sep = " \t\r\n");
bool patmatch(ধ্রুব বর্ণ *pattern, ধ্রুব বর্ণ *string);
#যদি !defined(YOSYS_DISABLE_SPAWN)
পূর্ণাঙ্ক run_command(ধ্রুব std::string &command, std::function<শূন্য(ধ্রুব std::string&)> process_সারি = std::function<শূন্য(ধ্রুব std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ধ্রুব std::string& filename, bool is_exec = false);
bool check_directory_exists(ধ্রুব std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
শূন্য remove_directory(std::string dirname);
bool create_directory(ধ্রুব std::string& dirname);
bool make_private_directory(ধ্রুব std::string& dirname);
std::string make_user_tmpdir(ধ্রুব std::string& name);
std::string escape_filename_spaces(ধ্রুব std::string& filename);
শূন্য append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ধ্রুব std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
সংরচনা ScopedFileLock {
	ScopedFileLock(ধ্রুব std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ধ্রুব ScopedFileLock &) = delete;
	ScopedFileLock &operator=(ধ্রুব ScopedFileLock &) = delete;
গুপ্ত:
#যদি defined(_WIN32)
	শূন্য *handle = nullptr;
#নোচেদ্যদি !defined(__wasi__)
	পূর্ণাঙ্ক fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
