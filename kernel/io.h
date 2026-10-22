#অন্তর্ভাব <string>
#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <type_traits>
#অন্তর্ভাব "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

কাঠামো IdString;

অন্তঃরেখ std::string vstringf(ধ্রুবক অক্ষর *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ধ্রুবক পূর্ণসংখ্যা k_Buf_আকার = 128;
	অক্ষর buf[k_Buf_আকার];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	পূর্ণসংখ্যা n = vsnprintf(buf, k_Buf_আকার, fmt, apc);
	va_end(apc);
	যদি (n < k_Buf_আকার)
		ফেরত std::string(buf);

	std::string string;
	অক্ষর *str = ফাঁকা;
#যদি defined(_WIN32) || defined(__CYGWIN__)
	পূর্ণসংখ্যা sz = 2 * k_Buf_আকার, rc;
	যখন (1) {
		va_copy(apc, ap);
		str = (অক্ষর *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		যদি (rc >= 0 && rc < sz)
			বিরতি;
		sz *= 2;
	}
	যদি (str != ফাঁকা) {
		string = str;
		মুক্ত(str);
	}
	ফেরত string;
#অন্যথা
	যদি (vasprintf(&str, fmt, ap) < 0)
		str = ফাঁকা;
	যদি (str != ফাঁকা) {
		string = str;
		মুক্ত(str);
	}
	ফেরত string;
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

constexpr ConversionSpecifier parse_conversion_specifier(অক্ষর ch, অক্ষর পূর্ব_ch)
{
	নির্বাচন (ch) {
	ক্ষেত্র 'd':
	ক্ষেত্র 'i':
		ফেরত CONVSPEC_SIGNED_INT;
	ক্ষেত্র 'o':
	ক্ষেত্র 'u':
	ক্ষেত্র 'x':
	ক্ষেত্র 'X':
	ক্ষেত্র 'm':
		ফেরত CONVSPEC_UNSIGNED_INT;
	ক্ষেত্র 'c':
		যদি (পূর্ব_ch == 'l' || পূর্ব_ch == 'q' || পূর্ব_ch == 'L') {
			// wchar not supported
			ফেরত CONVSPEC_ERROR;
		}
		ফেরত CONVSPEC_UNSIGNED_INT;
	ক্ষেত্র 'e':
	ক্ষেত্র 'E':
	ক্ষেত্র 'f':
	ক্ষেত্র 'F':
	ক্ষেত্র 'g':
	ক্ষেত্র 'G':
	ক্ষেত্র 'a':
	ক্ষেত্র 'A':
		ফেরত CONVSPEC_DOUBLE;
	ক্ষেত্র 's':
		যদি (পূর্ব_ch == 'l' || পূর্ব_ch == 'q' || পূর্ব_ch == 'L') {
			// wchar not supported
			ফেরত CONVSPEC_ERROR;
		}
		ফেরত CONVSPEC_CHAR_PTR;
	ক্ষেত্র 'p':
		ফেরত CONVSPEC_VOID_PTR;
	ক্ষেত্র '$': // positional parameters
	ক্ষেত্র 'n':
	ক্ষেত্র 'S':
		ফেরত CONVSPEC_ERROR;
	পূর্বনির্ধারিত:
		ফেরত CONVSPEC_NONE;
	}
}

গণনা class Dynamic_Int_গুনতি : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
কাঠামো FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	পূর্ণসংখ্যা start;
	// The end offset of the conversion spec in the format string.
	পূর্ণসংখ্যা end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints;
};

// Ensure there is no format spec.
constexpr শূন্য ensure_no_format_spec(std::string_view fmt, পূর্ণসংখ্যা সূচক, bool *has_escapes)
{
	পূর্ণসংখ্যা fmt_আকার = static_cast<পূর্ণসংখ্যা>(fmt.আকার_2());
	// A trailing '%' is not a format spec.
	যখন (সূচক + 1 < fmt_আকার) {
		যদি (fmt[সূচক] != '%') {
			++সূচক;
			চালু;
		}
		যদি (fmt[সূচক + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		সূচক += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_পরবর্তী_format_spec(std::string_view fmt, পূর্ণসংখ্যা fmt_start, bool *has_escapes)
{
	পূর্ণসংখ্যা সূচক = fmt_start;
	পূর্ণসংখ্যা fmt_আকার = static_cast<পূর্ণসংখ্যা>(fmt.আকার_2());
	যখন (সূচক < fmt_আকার) {
		যদি (fmt[সূচক] != '%') {
			++সূচক;
			চালু;
		}
		পূর্ণসংখ্যা p = সূচক + 1;
		uint8_t অঙ্ক_dynamic_ints = 0;
		যখন (true) {
			যদি (p == fmt_আকার) {
				ফেরত {0, 0, CONVSPEC_NONE, Dynamic_Int_গুনতি::NONE};
			}
			যদি (fmt[p] == '%') {
				*has_escapes = true;
				সূচক = p + 1;
				বিরতি;
			}
			যদি (fmt[p] == '*') {
				যদি (অঙ্ক_dynamic_ints >= 2) {
					ফেরত {0, 0, CONVSPEC_ERROR, Dynamic_Int_গুনতি::NONE};
				}
				++অঙ্ক_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			যদি (spec != CONVSPEC_NONE) {
				ফেরত {সূচক, p + 1, spec, static_cast<Dynamic_Int_গুনতি>(অঙ্ক_dynamic_ints)};
			}
			++p;
		}
	}
	ফেরত {0, 0, CONVSPEC_NONE, Dynamic_Int_গুনতি::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<আকার...(Args) == 0>::type
check_format(std::string_view fmt, পূর্ণসংখ্যা fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_গুনতি)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
স্থির auto has_name_member_imp(পূর্ণসংখ্যা)
	-> decltype(static_cast<ধ্রুবক IdString>(std::declval<T>().name), std::true_type{});

template <class T>
স্থির auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
কাঠামো has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
স্থির auto ptr_has_name_member_imp(পূর্ণসংখ্যা)
	-> decltype(static_cast<ধ্রুবক IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
স্থির auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
কাঠামো ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr শূন্য check_format(std::string_view fmt, পূর্ণসংখ্যা fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_গুনতি int_args_consumed)
{
	FoundFormatSpec found = find_পরবর্তী_format_spec(fmt, fmt_start, has_escapes);
	যদি (found.অঙ্ক_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		যদি constexpr (!std::is_convertible_v<Arg, পূর্ণসংখ্যা>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_গুনতি>(static_cast<uint8_t>(int_args_consumed) + 1));
		ফেরত;
	}
	নির্বাচন (found.spec) {
	ক্ষেত্র CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		বিরতি;
	ক্ষেত্র CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		বিরতি;
	ক্ষেত্র CONVSPEC_SIGNED_INT:
		যদি constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		বিরতি;
	ক্ষেত্র CONVSPEC_UNSIGNED_INT:
		যদি constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		বিরতি;
	ক্ষেত্র CONVSPEC_DOUBLE:
		যদি constexpr (!std::is_convertible_v<Arg, দ্বিগুণ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		বিরতি;
	ক্ষেত্র CONVSPEC_CHAR_PTR:
		যদি constexpr (!std::is_convertible_v<Arg, ধ্রুবক অক্ষর *> &&
		        !std::is_convertible_v<Arg, ধ্রুবক std::string &> &&
			!std::is_convertible_v<Arg, ধ্রুবক std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		বিরতি;
	ক্ষেত্র CONVSPEC_VOID_PTR:
		যদি constexpr (!std::is_convertible_v<Arg, ধ্রুবক শূন্য *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		বিরতি;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_গুনতি::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
শূন্য format_emit_long_long(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
শূন্য format_emit_unsigned_long_long(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
শূন্য format_emit_double(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, দ্বিগুণ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
শূন্য format_emit_char_ptr(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুবক অক্ষর *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
শূন্য format_emit_string(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুবক std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
শূন্য format_emit_string_view(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
শূন্য format_emit_void_ptr(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুবক শূন্য *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
অন্তঃরেখ শূন্য format_emit_one(std::string &ফলাফল, std::string_view fmt, ধ্রুবক FoundFormatSpec &ffspec,
	পূর্ণসংখ্যা *dynamic_ints, ধ্রুবক Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints = ffspec.অঙ্ক_dynamic_ints;
	নির্বাচন (ffspec.spec) {
	ক্ষেত্র CONVSPEC_SIGNED_INT:
		যদি constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		বিরতি;
	ক্ষেত্র CONVSPEC_UNSIGNED_INT:
		যদি constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		বিরতি;
	ক্ষেত্র CONVSPEC_DOUBLE:
		যদি constexpr (std::is_convertible_v<Arg, দ্বিগুণ>) {
			দ্বিগুণ s = arg;
			format_emit_double(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		বিরতি;
	ক্ষেত্র CONVSPEC_CHAR_PTR:
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুবক অক্ষর *>) {
			ধ্রুবক অক্ষর *s = arg;
			format_emit_char_ptr(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুবক std::string &>) {
			ধ্রুবক std::string &s = arg;
			format_emit_string(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুবক std::string_view &>) {
			ধ্রুবক std::string_view &s = arg;
			format_emit_string_view(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		যদি constexpr (has_name_member<Arg>()) {
			ধ্রুবক std::string &s = arg.name.unescape();
			format_emit_string(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		যদি constexpr (ptr_has_name_member<Arg>()) {
			ধ্রুবক std::string &s = arg->name.unescape();
			format_emit_string(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		বিরতি;
	ক্ষেত্র CONVSPEC_VOID_PTR:
		যদি constexpr (std::is_convertible_v<Arg, ধ্রুবক শূন্য *>) {
			ধ্রুবক শূন্য *s = arg;
			format_emit_void_ptr(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, s);
			ফেরত;
		}
		বিরতি;
	পূর্বনির্ধারিত:
		বিরতি;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
শূন্য format_emit_unescaped(std::string &ফলাফল, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

অন্তঃরেখ শূন্য format_emit(std::string &ফলাফল, std::string_view fmt, পূর্ণসংখ্যা fmt_start,
	bool has_escapes, ধ্রুবক FoundFormatSpec*, পূর্ণসংখ্যা*, Dynamic_Int_গুনতি)
{
	fmt = fmt.substr(fmt_start);
	যদি (has_escapes) {
		format_emit_unescaped(ফলাফল, fmt);
	} অন্যথা {
		ফলাফল += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
অন্তঃরেখ শূন্য format_emit(std::string &ফলাফল, std::string_view fmt, পূর্ণসংখ্যা fmt_start, bool has_escapes,
	ধ্রুবক FoundFormatSpec* specs, পূর্ণসংখ্যা *dynamic_ints, Dynamic_Int_গুনতি অঙ্ক_dynamic_ints,
	ধ্রুবক Arg &arg, ধ্রুবক Args &... args)
{
	যদি (specs->অঙ্ক_dynamic_ints > অঙ্ক_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		যদি constexpr (std::is_convertible_v<Arg, পূর্ণসংখ্যা>) {
			dynamic_ints[static_cast<uint8_t>(অঙ্ক_dynamic_ints)] = arg;
		} অন্যথা {
			YOSYS_ABORT("Internal error");
		}
		format_emit(ফলাফল, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_গুনতি>(static_cast<uint8_t>(অঙ্ক_dynamic_ints) + 1), args...);
		ফেরত;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	যদি (has_escapes) {
		format_emit_unescaped(ফলাফল, str);
	} অন্যথা {
		ফলাফল += str;
	}
	format_emit_one(ফলাফল, fmt, *specs, dynamic_ints, arg);
	format_emit(ফলাফল, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_গুনতি::NONE, args...);
}

template <typename... Args>
অন্তঃরেখ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ধ্রুবক FoundFormatSpec* specs, ধ্রুবক Args &... args)
{
	std::string ফলাফল;
	পূর্ণসংখ্যা dynamic_ints[2] = { 0, 0 };
	format_emit(ফলাফল, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_গুনতি::NONE, args...);
	ফেরত ফলাফল;
}
template <>
অন্তঃরেখ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ধ্রুবক FoundFormatSpec*)
{
	যদি (!has_escapes) {
		ফেরত std::string(fmt);
	}
	ফেরত unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
সর্বজনীন:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ধ্রুবক অক্ষর *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_গুনতি::NONE);
	}
	std::string format(ধ্রুবক Args &... args)
	{
		ফেরত format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ধ্রুবক { ফেরত fmt; }
ব্যক্তিগত:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[আকার...(Args) ? আকার...(Args) : 1] = {};
};

template <typename T> কাঠামো WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
অন্তঃরেখ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	ফেরত fmt.format(args...);
}

পূর্ণসংখ্যা readsome(std::istream &f, অক্ষর *s, পূর্ণসংখ্যা n);
std::string পরবর্তী_token(std::string &text, ধ্রুবক অক্ষর *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ধ্রুবক std::string &text, ধ্রুবক অক্ষর *sep = " \t\r\n");
bool patmatch(ধ্রুবক অক্ষর *pattern, ধ্রুবক অক্ষর *string);
#যদি !defined(YOSYS_DISABLE_SPAWN)
পূর্ণসংখ্যা run_command(ধ্রুবক std::string &command, std::function<শূন্য(ধ্রুবক std::string&)> process_সারি = std::function<শূন্য(ধ্রুবক std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_অস্থায়ী_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_অস্থায়ী_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ধ্রুবক std::string& filename, bool is_exec = false);
bool check_directory_exists(ধ্রুবক std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
শূন্য remove_directory(std::string dirname);
bool create_directory(ধ্রুবক std::string& dirname);
bool make_private_directory(ধ্রুবক std::string& dirname);
std::string make_user_tmpdir(ধ্রুবক std::string& name);
std::string escape_filename_spaces(ধ্রুবক std::string& filename);
শূন্য append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ধ্রুবক std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
কাঠামো ScopedFileLock {
	ScopedFileLock(ধ্রুবক std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ধ্রুবক ScopedFileLock &) = delete;
	ScopedFileLock &operator=(ধ্রুবক ScopedFileLock &) = delete;
ব্যক্তিগত:
#যদি defined(_WIN32)
	শূন্য *handle = nullptr;
#নতুবা_যদি !defined(__wasi__)
	পূর্ণসংখ্যা fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
