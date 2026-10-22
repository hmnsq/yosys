#அந்தர்பாவ <string>
#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <type_traits>
#அந்தர்பாவ "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு IdString;

உள்வரி std::string vstringf(மாறிலி எழுத்து *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	மாறிலி முழு kBufSize = 128;
	எழுத்து buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	முழு n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	எனில் (n < kBufSize)
		திரும்பு std::string(buf);

	std::string string;
	எழுத்து *str = வெற்று;
#எனில் defined(_WIN32) || defined(__CYGWIN__)
	முழு sz = 2 * kBufSize, rc;
	போது (1) {
		va_copy(apc, ap);
		str = (எழுத்து *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		எனில் (rc >= 0 && rc < sz)
			நிறுத்து;
		sz *= 2;
	}
	எனில் (str != வெற்று) {
		string = str;
		விடுவி(str);
	}
	திரும்பு string;
#இல்லையெனில்
	எனில் (vasprintf(&str, fmt, ap) < 0)
		str = வெற்று;
	எனில் (str != வெற்று) {
		string = str;
		விடுவி(str);
	}
	திரும்பு string;
#endif
}

எண்ணிக்கை ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(எழுத்து ch, எழுத்து prev_ch)
{
	தேர்வு (ch) {
	நிலை 'd':
	நிலை 'i':
		திரும்பு CONVSPEC_SIGNED_INT;
	நிலை 'o':
	நிலை 'u':
	நிலை 'x':
	நிலை 'X':
	நிலை 'm':
		திரும்பு CONVSPEC_UNSIGNED_INT;
	நிலை 'c':
		எனில் (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			திரும்பு CONVSPEC_ERROR;
		}
		திரும்பு CONVSPEC_UNSIGNED_INT;
	நிலை 'e':
	நிலை 'E':
	நிலை 'f':
	நிலை 'F':
	நிலை 'g':
	நிலை 'G':
	நிலை 'a':
	நிலை 'A':
		திரும்பு CONVSPEC_DOUBLE;
	நிலை 's':
		எனில் (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			திரும்பு CONVSPEC_ERROR;
		}
		திரும்பு CONVSPEC_CHAR_PTR;
	நிலை 'p':
		திரும்பு CONVSPEC_VOID_PTR;
	நிலை '$': // positional parameters
	நிலை 'n':
	நிலை 'S':
		திரும்பு CONVSPEC_ERROR;
	இயல்பு:
		திரும்பு CONVSPEC_NONE;
	}
}

எண்ணிக்கை class Dynamic_Int_மொத்தம் : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
கட்டமைப்பு FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	முழு start;
	// The end offset of the conversion spec in the format string.
	முழு end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints;
};

// Ensure there is no format spec.
constexpr வெறுமை ensure_no_format_spec(std::string_view fmt, முழு index, bool *has_escapes)
{
	முழு fmt_size = static_cast<முழு>(fmt.size());
	// A trailing '%' is not a format spec.
	போது (index + 1 < fmt_size) {
		எனில் (fmt[index] != '%') {
			++index;
			தொடர்;
		}
		எனில் (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, முழு fmt_start, bool *has_escapes)
{
	முழு index = fmt_start;
	முழு fmt_size = static_cast<முழு>(fmt.size());
	போது (index < fmt_size) {
		எனில் (fmt[index] != '%') {
			++index;
			தொடர்;
		}
		முழு p = index + 1;
		uint8_t இலக்கம்_dynamic_ints = 0;
		போது (true) {
			எனில் (p == fmt_size) {
				திரும்பு {0, 0, CONVSPEC_NONE, Dynamic_Int_மொத்தம்::NONE};
			}
			எனில் (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				நிறுத்து;
			}
			எனில் (fmt[p] == '*') {
				எனில் (இலக்கம்_dynamic_ints >= 2) {
					திரும்பு {0, 0, CONVSPEC_ERROR, Dynamic_Int_மொத்தம்::NONE};
				}
				++இலக்கம்_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			எனில் (spec != CONVSPEC_NONE) {
				திரும்பு {index, p + 1, spec, static_cast<Dynamic_Int_மொத்தம்>(இலக்கம்_dynamic_ints)};
			}
			++p;
		}
	}
	திரும்பு {0, 0, CONVSPEC_NONE, Dynamic_Int_மொத்தம்::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<அளவு...(Args) == 0>::type
check_format(std::string_view fmt, முழு fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_மொத்தம்)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
நிலையான auto has_name_member_imp(முழு)
	-> decltype(static_cast<மாறிலி IdString>(std::declval<T>().name), std::true_type{});

template <class T>
நிலையான auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
கட்டமைப்பு has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
நிலையான auto ptr_has_name_member_imp(முழு)
	-> decltype(static_cast<மாறிலி IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
நிலையான auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
கட்டமைப்பு ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr வெறுமை check_format(std::string_view fmt, முழு fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_மொத்தம் int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	எனில் (found.இலக்கம்_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		எனில் constexpr (!std::is_convertible_v<Arg, முழு>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_மொத்தம்>(static_cast<uint8_t>(int_args_consumed) + 1));
		திரும்பு;
	}
	தேர்வு (found.spec) {
	நிலை CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		நிறுத்து;
	நிலை CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		நிறுத்து;
	நிலை CONVSPEC_SIGNED_INT:
		எனில் constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		நிறுத்து;
	நிலை CONVSPEC_UNSIGNED_INT:
		எனில் constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		நிறுத்து;
	நிலை CONVSPEC_DOUBLE:
		எனில் constexpr (!std::is_convertible_v<Arg, இரட்டை>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		நிறுத்து;
	நிலை CONVSPEC_CHAR_PTR:
		எனில் constexpr (!std::is_convertible_v<Arg, மாறிலி எழுத்து *> &&
		        !std::is_convertible_v<Arg, மாறிலி std::string &> &&
			!std::is_convertible_v<Arg, மாறிலி std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		நிறுத்து;
	நிலை CONVSPEC_VOID_PTR:
		எனில் constexpr (!std::is_convertible_v<Arg, மாறிலி வெறுமை *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		நிறுத்து;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_மொத்தம்::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
வெறுமை format_emit_long_long(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
வெறுமை format_emit_unsigned_long_long(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
வெறுமை format_emit_double(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, இரட்டை arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
வெறுமை format_emit_char_ptr(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, மாறிலி எழுத்து *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
வெறுமை format_emit_string(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, மாறிலி std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
வெறுமை format_emit_string_view(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
வெறுமை format_emit_void_ptr(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, மாறிலி வெறுமை *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
உள்வரி வெறுமை format_emit_one(std::string &result, std::string_view fmt, மாறிலி FoundFormatSpec &ffspec,
	முழு *dynamic_ints, மாறிலி Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints = ffspec.இலக்கம்_dynamic_ints;
	தேர்வு (ffspec.spec) {
	நிலை CONVSPEC_SIGNED_INT:
		எனில் constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		நிறுத்து;
	நிலை CONVSPEC_UNSIGNED_INT:
		எனில் constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		நிறுத்து;
	நிலை CONVSPEC_DOUBLE:
		எனில் constexpr (std::is_convertible_v<Arg, இரட்டை>) {
			இரட்டை s = arg;
			format_emit_double(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		நிறுத்து;
	நிலை CONVSPEC_CHAR_PTR:
		எனில் constexpr (std::is_convertible_v<Arg, மாறிலி எழுத்து *>) {
			மாறிலி எழுத்து *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		எனில் constexpr (std::is_convertible_v<Arg, மாறிலி std::string &>) {
			மாறிலி std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		எனில் constexpr (std::is_convertible_v<Arg, மாறிலி std::string_view &>) {
			மாறிலி std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		எனில் constexpr (has_name_member<Arg>()) {
			மாறிலி std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		எனில் constexpr (ptr_has_name_member<Arg>()) {
			மாறிலி std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		நிறுத்து;
	நிலை CONVSPEC_VOID_PTR:
		எனில் constexpr (std::is_convertible_v<Arg, மாறிலி வெறுமை *>) {
			மாறிலி வெறுமை *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, s);
			திரும்பு;
		}
		நிறுத்து;
	இயல்பு:
		நிறுத்து;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
வெறுமை format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

உள்வரி வெறுமை format_emit(std::string &result, std::string_view fmt, முழு fmt_start,
	bool has_escapes, மாறிலி FoundFormatSpec*, முழு*, Dynamic_Int_மொத்தம்)
{
	fmt = fmt.substr(fmt_start);
	எனில் (has_escapes) {
		format_emit_unescaped(result, fmt);
	} இல்லையெனில் {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
உள்வரி வெறுமை format_emit(std::string &result, std::string_view fmt, முழு fmt_start, bool has_escapes,
	மாறிலி FoundFormatSpec* specs, முழு *dynamic_ints, Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints,
	மாறிலி Arg &arg, மாறிலி Args &... args)
{
	எனில் (specs->இலக்கம்_dynamic_ints > இலக்கம்_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		எனில் constexpr (std::is_convertible_v<Arg, முழு>) {
			dynamic_ints[static_cast<uint8_t>(இலக்கம்_dynamic_ints)] = arg;
		} இல்லையெனில் {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_மொத்தம்>(static_cast<uint8_t>(இலக்கம்_dynamic_ints) + 1), args...);
		திரும்பு;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	எனில் (has_escapes) {
		format_emit_unescaped(result, str);
	} இல்லையெனில் {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_மொத்தம்::NONE, args...);
}

template <typename... Args>
உள்வரி std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, மாறிலி FoundFormatSpec* specs, மாறிலி Args &... args)
{
	std::string result;
	முழு dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_மொத்தம்::NONE, args...);
	திரும்பு result;
}
template <>
உள்வரி std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, மாறிலி FoundFormatSpec*)
{
	எனில் (!has_escapes) {
		திரும்பு std::string(fmt);
	}
	திரும்பு unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
பொது:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(மாறிலி எழுத்து *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_மொத்தம்::NONE);
	}
	std::string format(மாறிலி Args &... args)
	{
		திரும்பு format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() மாறிலி { திரும்பு fmt; }
தனிப்பட்ட:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[அளவு...(Args) ? அளவு...(Args) : 1] = {};
};

template <typename T> கட்டமைப்பு WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
உள்வரி std::string stringf(FmtString<TypeIdentity<Args>...> fmt, மாறிலி Args &... args)
{
	திரும்பு fmt.format(args...);
}

முழு readsome(std::istream &f, எழுத்து *s, முழு n);
std::string next_token(std::string &text, மாறிலி எழுத்து *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(மாறிலி std::string &text, மாறிலி எழுத்து *sep = " \t\r\n");
bool patmatch(மாறிலி எழுத்து *pattern, மாறிலி எழுத்து *string);
#எனில் !defined(YOSYS_DISABLE_SPAWN)
முழு run_command(மாறிலி std::string &command, std::function<வெறுமை(மாறிலி std::string&)> process_வரி = std::function<வெறுமை(மாறிலி std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(மாறிலி std::string& filename, bool is_exec = false);
bool check_directory_exists(மாறிலி std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
வெறுமை remove_directory(std::string dirname);
bool create_directory(மாறிலி std::string& dirname);
bool make_private_directory(மாறிலி std::string& dirname);
std::string make_user_tmpdir(மாறிலி std::string& name);
std::string escape_filename_spaces(மாறிலி std::string& filename);
வெறுமை append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(மாறிலி std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
கட்டமைப்பு ScopedFileLock {
	ScopedFileLock(மாறிலி std::string &path);
	~ScopedFileLock();
	ScopedFileLock(மாறிலி ScopedFileLock &) = delete;
	ScopedFileLock &operator=(மாறிலி ScopedFileLock &) = delete;
தனிப்பட்ட:
#எனில் defined(_WIN32)
	வெறுமை *handle = nullptr;
#அல்லதெனில் !defined(__wasi__)
	முழு fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
