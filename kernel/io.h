#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <type_traits>
#അന്തര്ഭാവ "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ഘടന IdString;

വരിയിൽ std::string vstringf(സ്ഥിരം അക്ഷരം *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	സ്ഥിരം പൂർണ്ണസംഖ്യ kBufSize = 128;
	അക്ഷരം buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	പൂർണ്ണസംഖ്യ n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	എങ്കിൽ (n < kBufSize)
		തിരികെ std::string(buf);

	std::string string;
	അക്ഷരം *str = ശൂന്യ;
#എങ്കിൽ defined(_WIN32) || defined(__CYGWIN__)
	പൂർണ്ണസംഖ്യ sz = 2 * kBufSize, rc;
	ആകുമ്പോൾ (1) {
		va_copy(apc, ap);
		str = (അക്ഷരം *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		എങ്കിൽ (rc >= 0 && rc < sz)
			നിർത്തുക;
		sz *= 2;
	}
	എങ്കിൽ (str != ശൂന്യ) {
		string = str;
		സ്വതന്ത്രം(str);
	}
	തിരികെ string;
#അല്ലെങ്കിൽ
	എങ്കിൽ (vasprintf(&str, fmt, ap) < 0)
		str = ശൂന്യ;
	എങ്കിൽ (str != ശൂന്യ) {
		string = str;
		സ്വതന്ത്രം(str);
	}
	തിരികെ string;
#endif
}

എണ്ണൽ ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(അക്ഷരം ch, അക്ഷരം prev_ch)
{
	തിരഞ്ഞെടുപ്പ് (ch) {
	അവസ്ഥ 'd':
	അവസ്ഥ 'i':
		തിരികെ CONVSPEC_SIGNED_INT;
	അവസ്ഥ 'o':
	അവസ്ഥ 'u':
	അവസ്ഥ 'x':
	അവസ്ഥ 'X':
	അവസ്ഥ 'm':
		തിരികെ CONVSPEC_UNSIGNED_INT;
	അവസ്ഥ 'c':
		എങ്കിൽ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			തിരികെ CONVSPEC_ERROR;
		}
		തിരികെ CONVSPEC_UNSIGNED_INT;
	അവസ്ഥ 'e':
	അവസ്ഥ 'E':
	അവസ്ഥ 'f':
	അവസ്ഥ 'F':
	അവസ്ഥ 'g':
	അവസ്ഥ 'G':
	അവസ്ഥ 'a':
	അവസ്ഥ 'A':
		തിരികെ CONVSPEC_DOUBLE;
	അവസ്ഥ 's':
		എങ്കിൽ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			തിരികെ CONVSPEC_ERROR;
		}
		തിരികെ CONVSPEC_CHAR_PTR;
	അവസ്ഥ 'p':
		തിരികെ CONVSPEC_VOID_PTR;
	അവസ്ഥ '$': // positional parameters
	അവസ്ഥ 'n':
	അവസ്ഥ 'S':
		തിരികെ CONVSPEC_ERROR;
	സ്ഥിരസ്ഥിതി:
		തിരികെ CONVSPEC_NONE;
	}
}

എണ്ണൽ class Dynamic_Int_എണ്ണം : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ഘടന FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	പൂർണ്ണസംഖ്യ start;
	// The end offset of the conversion spec in the format string.
	പൂർണ്ണസംഖ്യ end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints;
};

// Ensure there is no format spec.
constexpr ശൂന്യം ensure_no_format_spec(std::string_view fmt, പൂർണ്ണസംഖ്യ index, bool *has_escapes)
{
	പൂർണ്ണസംഖ്യ fmt_size = static_cast<പൂർണ്ണസംഖ്യ>(fmt.size());
	// A trailing '%' is not a format spec.
	ആകുമ്പോൾ (index + 1 < fmt_size) {
		എങ്കിൽ (fmt[index] != '%') {
			++index;
			തുടരുക;
		}
		എങ്കിൽ (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, പൂർണ്ണസംഖ്യ fmt_start, bool *has_escapes)
{
	പൂർണ്ണസംഖ്യ index = fmt_start;
	പൂർണ്ണസംഖ്യ fmt_size = static_cast<പൂർണ്ണസംഖ്യ>(fmt.size());
	ആകുമ്പോൾ (index < fmt_size) {
		എങ്കിൽ (fmt[index] != '%') {
			++index;
			തുടരുക;
		}
		പൂർണ്ണസംഖ്യ p = index + 1;
		uint8_t അക്കം_dynamic_ints = 0;
		ആകുമ്പോൾ (true) {
			എങ്കിൽ (p == fmt_size) {
				തിരികെ {0, 0, CONVSPEC_NONE, Dynamic_Int_എണ്ണം::NONE};
			}
			എങ്കിൽ (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				നിർത്തുക;
			}
			എങ്കിൽ (fmt[p] == '*') {
				എങ്കിൽ (അക്കം_dynamic_ints >= 2) {
					തിരികെ {0, 0, CONVSPEC_ERROR, Dynamic_Int_എണ്ണം::NONE};
				}
				++അക്കം_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			എങ്കിൽ (spec != CONVSPEC_NONE) {
				തിരികെ {index, p + 1, spec, static_cast<Dynamic_Int_എണ്ണം>(അക്കം_dynamic_ints)};
			}
			++p;
		}
	}
	തിരികെ {0, 0, CONVSPEC_NONE, Dynamic_Int_എണ്ണം::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<വലുപ്പം...(Args) == 0>::type
check_format(std::string_view fmt, പൂർണ്ണസംഖ്യ fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_എണ്ണം)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
സ്ഥിതം auto has_name_member_imp(പൂർണ്ണസംഖ്യ)
	-> decltype(static_cast<സ്ഥിരം IdString>(std::declval<T>().name), std::true_type{});

template <class T>
സ്ഥിതം auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
ഘടന has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
സ്ഥിതം auto ptr_has_name_member_imp(പൂർണ്ണസംഖ്യ)
	-> decltype(static_cast<സ്ഥിരം IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
സ്ഥിതം auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
ഘടന ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr ശൂന്യം check_format(std::string_view fmt, പൂർണ്ണസംഖ്യ fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_എണ്ണം int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	എങ്കിൽ (found.അക്കം_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		എങ്കിൽ constexpr (!std::is_convertible_v<Arg, പൂർണ്ണസംഖ്യ>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_എണ്ണം>(static_cast<uint8_t>(int_args_consumed) + 1));
		തിരികെ;
	}
	തിരഞ്ഞെടുപ്പ് (found.spec) {
	അവസ്ഥ CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_SIGNED_INT:
		എങ്കിൽ constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_UNSIGNED_INT:
		എങ്കിൽ constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_DOUBLE:
		എങ്കിൽ constexpr (!std::is_convertible_v<Arg, ഇരട്ട>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_CHAR_PTR:
		എങ്കിൽ constexpr (!std::is_convertible_v<Arg, സ്ഥിരം അക്ഷരം *> &&
		        !std::is_convertible_v<Arg, സ്ഥിരം std::string &> &&
			!std::is_convertible_v<Arg, സ്ഥിരം std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_VOID_PTR:
		എങ്കിൽ constexpr (!std::is_convertible_v<Arg, സ്ഥിരം ശൂന്യം *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		നിർത്തുക;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_എണ്ണം::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
ശൂന്യം format_emit_long_long(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
ശൂന്യം format_emit_unsigned_long_long(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ശൂന്യം format_emit_double(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, ഇരട്ട arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
ശൂന്യം format_emit_char_ptr(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, സ്ഥിരം അക്ഷരം *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
ശൂന്യം format_emit_string(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, സ്ഥിരം std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
ശൂന്യം format_emit_string_view(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ശൂന്യം format_emit_void_ptr(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, സ്ഥിരം ശൂന്യം *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
വരിയിൽ ശൂന്യം format_emit_one(std::string &result, std::string_view fmt, സ്ഥിരം FoundFormatSpec &ffspec,
	പൂർണ്ണസംഖ്യ *dynamic_ints, സ്ഥിരം Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints = ffspec.അക്കം_dynamic_ints;
	തിരഞ്ഞെടുപ്പ് (ffspec.spec) {
	അവസ്ഥ CONVSPEC_SIGNED_INT:
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_UNSIGNED_INT:
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_DOUBLE:
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, ഇരട്ട>) {
			ഇരട്ട s = arg;
			format_emit_double(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_CHAR_PTR:
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, സ്ഥിരം അക്ഷരം *>) {
			സ്ഥിരം അക്ഷരം *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, സ്ഥിരം std::string &>) {
			സ്ഥിരം std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, സ്ഥിരം std::string_view &>) {
			സ്ഥിരം std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		എങ്കിൽ constexpr (has_name_member<Arg>()) {
			സ്ഥിരം std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		എങ്കിൽ constexpr (ptr_has_name_member<Arg>()) {
			സ്ഥിരം std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		നിർത്തുക;
	അവസ്ഥ CONVSPEC_VOID_PTR:
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, സ്ഥിരം ശൂന്യം *>) {
			സ്ഥിരം ശൂന്യം *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, അക്കം_dynamic_ints, s);
			തിരികെ;
		}
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
ശൂന്യം format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

വരിയിൽ ശൂന്യം format_emit(std::string &result, std::string_view fmt, പൂർണ്ണസംഖ്യ fmt_start,
	bool has_escapes, സ്ഥിരം FoundFormatSpec*, പൂർണ്ണസംഖ്യ*, Dynamic_Int_എണ്ണം)
{
	fmt = fmt.substr(fmt_start);
	എങ്കിൽ (has_escapes) {
		format_emit_unescaped(result, fmt);
	} അല്ലെങ്കിൽ {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
വരിയിൽ ശൂന്യം format_emit(std::string &result, std::string_view fmt, പൂർണ്ണസംഖ്യ fmt_start, bool has_escapes,
	സ്ഥിരം FoundFormatSpec* specs, പൂർണ്ണസംഖ്യ *dynamic_ints, Dynamic_Int_എണ്ണം അക്കം_dynamic_ints,
	സ്ഥിരം Arg &arg, സ്ഥിരം Args &... args)
{
	എങ്കിൽ (specs->അക്കം_dynamic_ints > അക്കം_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		എങ്കിൽ constexpr (std::is_convertible_v<Arg, പൂർണ്ണസംഖ്യ>) {
			dynamic_ints[static_cast<uint8_t>(അക്കം_dynamic_ints)] = arg;
		} അല്ലെങ്കിൽ {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_എണ്ണം>(static_cast<uint8_t>(അക്കം_dynamic_ints) + 1), args...);
		തിരികെ;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	എങ്കിൽ (has_escapes) {
		format_emit_unescaped(result, str);
	} അല്ലെങ്കിൽ {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_എണ്ണം::NONE, args...);
}

template <typename... Args>
വരിയിൽ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, സ്ഥിരം FoundFormatSpec* specs, സ്ഥിരം Args &... args)
{
	std::string result;
	പൂർണ്ണസംഖ്യ dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_എണ്ണം::NONE, args...);
	തിരികെ result;
}
template <>
വരിയിൽ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, സ്ഥിരം FoundFormatSpec*)
{
	എങ്കിൽ (!has_escapes) {
		തിരികെ std::string(fmt);
	}
	തിരികെ unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
പൊതു:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(സ്ഥിരം അക്ഷരം *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_എണ്ണം::NONE);
	}
	std::string format(സ്ഥിരം Args &... args)
	{
		തിരികെ format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() സ്ഥിരം { തിരികെ fmt; }
സ്വകാര്യ:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[വലുപ്പം...(Args) ? വലുപ്പം...(Args) : 1] = {};
};

template <typename T> ഘടന WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
വരിയിൽ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	തിരികെ fmt.format(args...);
}

പൂർണ്ണസംഖ്യ readsome(std::istream &f, അക്ഷരം *s, പൂർണ്ണസംഖ്യ n);
std::string next_token(std::string &text, സ്ഥിരം അക്ഷരം *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(സ്ഥിരം std::string &text, സ്ഥിരം അക്ഷരം *sep = " \t\r\n");
bool patmatch(സ്ഥിരം അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *string);
#എങ്കിൽ !defined(YOSYS_DISABLE_SPAWN)
പൂർണ്ണസംഖ്യ run_command(സ്ഥിരം std::string &command, std::function<ശൂന്യം(സ്ഥിരം std::string&)> process_വരി = std::function<ശൂന്യം(സ്ഥിരം std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(സ്ഥിരം std::string& filename, bool is_exec = false);
bool check_directory_exists(സ്ഥിരം std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
ശൂന്യം remove_directory(std::string dirname);
bool create_directory(സ്ഥിരം std::string& dirname);
bool make_private_directory(സ്ഥിരം std::string& dirname);
std::string make_user_tmpdir(സ്ഥിരം std::string& name);
std::string escape_filename_spaces(സ്ഥിരം std::string& filename);
ശൂന്യം append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(സ്ഥിരം std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ഘടന ScopedFileLock {
	ScopedFileLock(സ്ഥിരം std::string &path);
	~ScopedFileLock();
	ScopedFileLock(സ്ഥിരം ScopedFileLock &) = delete;
	ScopedFileLock &operator=(സ്ഥിരം ScopedFileLock &) = delete;
സ്വകാര്യ:
#എങ്കിൽ defined(_WIN32)
	ശൂന്യം *handle = nullptr;
#അല്ലെങ്കിലെങ്കിൽ !defined(__wasi__)
	പൂർണ്ണസംഖ്യ fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
