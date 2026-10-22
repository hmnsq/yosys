#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ IdString;

ಅನ್ತಃಸ್ಥ std::string vstringf(ಧ್ರುವ ವರ್ಣ *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ kBufSize = 128;
	ವರ್ಣ buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	ಪೂರ್ಣಾಙ್ಕ n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	ಯದಿ (n < kBufSize)
		ಪ್ರತ್ಯಾಗಮ std::string(buf);

	std::string string;
	ವರ್ಣ *str = ರಿಕ್ತ;
#ಯದಿ defined(_WIN32) || defined(__CYGWIN__)
	ಪೂರ್ಣಾಙ್ಕ sz = 2 * kBufSize, rc;
	ಯಾವತ್ (1) {
		va_copy(apc, ap);
		str = (ವರ್ಣ *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		ಯದಿ (rc >= 0 && rc < sz)
			ಭಙ್ಗ;
		sz *= 2;
	}
	ಯದಿ (str != ರಿಕ್ತ) {
		string = str;
		ಮುಕ್ತ(str);
	}
	ಪ್ರತ್ಯಾಗಮ string;
#ಅನ್ಯಥಾ
	ಯದಿ (vasprintf(&str, fmt, ap) < 0)
		str = ರಿಕ್ತ;
	ಯದಿ (str != ರಿಕ್ತ) {
		string = str;
		ಮುಕ್ತ(str);
	}
	ಪ್ರತ್ಯಾಗಮ string;
#endif
}

ಗಣನಾ ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(ವರ್ಣ ch, ವರ್ಣ prev_ch)
{
	ವಿಕಲ್ಪ (ch) {
	ಪಕ್ಷ 'd':
	ಪಕ್ಷ 'i':
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_SIGNED_INT;
	ಪಕ್ಷ 'o':
	ಪಕ್ಷ 'u':
	ಪಕ್ಷ 'x':
	ಪಕ್ಷ 'X':
	ಪಕ್ಷ 'm':
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_UNSIGNED_INT;
	ಪಕ್ಷ 'c':
		ಯದಿ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ಪ್ರತ್ಯಾಗಮ CONVSPEC_ERROR;
		}
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_UNSIGNED_INT;
	ಪಕ್ಷ 'e':
	ಪಕ್ಷ 'E':
	ಪಕ್ಷ 'f':
	ಪಕ್ಷ 'F':
	ಪಕ್ಷ 'g':
	ಪಕ್ಷ 'G':
	ಪಕ್ಷ 'a':
	ಪಕ್ಷ 'A':
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_DOUBLE;
	ಪಕ್ಷ 's':
		ಯದಿ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ಪ್ರತ್ಯಾಗಮ CONVSPEC_ERROR;
		}
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_CHAR_PTR;
	ಪಕ್ಷ 'p':
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_VOID_PTR;
	ಪಕ್ಷ '$': // positional parameters
	ಪಕ್ಷ 'n':
	ಪಕ್ಷ 'S':
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_ERROR;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ CONVSPEC_NONE;
	}
}

ಗಣನಾ class Dynamic_Int_ಎಣಿಕೆ : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ಸಂರಚನಾ FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	ಪೂರ್ಣಾಙ್ಕ start;
	// The end offset of the conversion spec in the format string.
	ಪೂರ್ಣಾಙ್ಕ end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints;
};

// Ensure there is no format spec.
constexpr ಶೂನ್ಯ ensure_no_format_spec(std::string_view fmt, ಪೂರ್ಣಾಙ್ಕ index, bool *has_escapes)
{
	ಪೂರ್ಣಾಙ್ಕ fmt_size = static_cast<ಪೂರ್ಣಾಙ್ಕ>(fmt.size());
	// A trailing '%' is not a format spec.
	ಯಾವತ್ (index + 1 < fmt_size) {
		ಯದಿ (fmt[index] != '%') {
			++index;
			ಅನುವರ್ತ;
		}
		ಯದಿ (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, ಪೂರ್ಣಾಙ್ಕ fmt_start, bool *has_escapes)
{
	ಪೂರ್ಣಾಙ್ಕ index = fmt_start;
	ಪೂರ್ಣಾಙ್ಕ fmt_size = static_cast<ಪೂರ್ಣಾಙ್ಕ>(fmt.size());
	ಯಾವತ್ (index < fmt_size) {
		ಯದಿ (fmt[index] != '%') {
			++index;
			ಅನುವರ್ತ;
		}
		ಪೂರ್ಣಾಙ್ಕ p = index + 1;
		uint8_t ಅಂಕೆ_dynamic_ints = 0;
		ಯಾವತ್ (true) {
			ಯದಿ (p == fmt_size) {
				ಪ್ರತ್ಯಾಗಮ {0, 0, CONVSPEC_NONE, Dynamic_Int_ಎಣಿಕೆ::NONE};
			}
			ಯದಿ (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				ಭಙ್ಗ;
			}
			ಯದಿ (fmt[p] == '*') {
				ಯದಿ (ಅಂಕೆ_dynamic_ints >= 2) {
					ಪ್ರತ್ಯಾಗಮ {0, 0, CONVSPEC_ERROR, Dynamic_Int_ಎಣಿಕೆ::NONE};
				}
				++ಅಂಕೆ_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			ಯದಿ (spec != CONVSPEC_NONE) {
				ಪ್ರತ್ಯಾಗಮ {index, p + 1, spec, static_cast<Dynamic_Int_ಎಣಿಕೆ>(ಅಂಕೆ_dynamic_ints)};
			}
			++p;
		}
	}
	ಪ್ರತ್ಯಾಗಮ {0, 0, CONVSPEC_NONE, Dynamic_Int_ಎಣಿಕೆ::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<ಪರಿಮಾಣ...(Args) == 0>::type
check_format(std::string_view fmt, ಪೂರ್ಣಾಙ್ಕ fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_ಎಣಿಕೆ)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
ಸ್ಥಿರ auto has_name_member_imp(ಪೂರ್ಣಾಙ್ಕ)
	-> decltype(static_cast<ಧ್ರುವ IdString>(std::declval<T>().name), std::true_type{});

template <class T>
ಸ್ಥಿರ auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
ಸಂರಚನಾ has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
ಸ್ಥಿರ auto ptr_has_name_member_imp(ಪೂರ್ಣಾಙ್ಕ)
	-> decltype(static_cast<ಧ್ರುವ IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
ಸ್ಥಿರ auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
ಸಂರಚನಾ ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr ಶೂನ್ಯ check_format(std::string_view fmt, ಪೂರ್ಣಾಙ್ಕ fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_ಎಣಿಕೆ int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	ಯದಿ (found.ಅಂಕೆ_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		ಯದಿ constexpr (!std::is_convertible_v<Arg, ಪೂರ್ಣಾಙ್ಕ>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_ಎಣಿಕೆ>(static_cast<uint8_t>(int_args_consumed) + 1));
		ಪ್ರತ್ಯಾಗಮ;
	}
	ವಿಕಲ್ಪ (found.spec) {
	ಪಕ್ಷ CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_SIGNED_INT:
		ಯದಿ constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_UNSIGNED_INT:
		ಯದಿ constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_DOUBLE:
		ಯದಿ constexpr (!std::is_convertible_v<Arg, ದ್ವಿಗುಣ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_CHAR_PTR:
		ಯದಿ constexpr (!std::is_convertible_v<Arg, ಧ್ರುವ ವರ್ಣ *> &&
		        !std::is_convertible_v<Arg, ಧ್ರುವ std::string &> &&
			!std::is_convertible_v<Arg, ಧ್ರುವ std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_VOID_PTR:
		ಯದಿ constexpr (!std::is_convertible_v<Arg, ಧ್ರುವ ಶೂನ್ಯ *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		ಭಙ್ಗ;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_ಎಣಿಕೆ::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
ಶೂನ್ಯ format_emit_long_long(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
ಶೂನ್ಯ format_emit_unsigned_long_long(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ಶೂನ್ಯ format_emit_double(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ದ್ವಿಗುಣ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
ಶೂನ್ಯ format_emit_char_ptr(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಧ್ರುವ ವರ್ಣ *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
ಶೂನ್ಯ format_emit_string(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಧ್ರುವ std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
ಶೂನ್ಯ format_emit_string_view(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ಶೂನ್ಯ format_emit_void_ptr(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಧ್ರುವ ಶೂನ್ಯ *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ format_emit_one(std::string &result, std::string_view fmt, ಧ್ರುವ FoundFormatSpec &ffspec,
	ಪೂರ್ಣಾಙ್ಕ *dynamic_ints, ಧ್ರುವ Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints = ffspec.ಅಂಕೆ_dynamic_ints;
	ವಿಕಲ್ಪ (ffspec.spec) {
	ಪಕ್ಷ CONVSPEC_SIGNED_INT:
		ಯದಿ constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_UNSIGNED_INT:
		ಯದಿ constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_DOUBLE:
		ಯದಿ constexpr (std::is_convertible_v<Arg, ದ್ವಿಗುಣ>) {
			ದ್ವಿಗುಣ s = arg;
			format_emit_double(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_CHAR_PTR:
		ಯದಿ constexpr (std::is_convertible_v<Arg, ಧ್ರುವ ವರ್ಣ *>) {
			ಧ್ರುವ ವರ್ಣ *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ constexpr (std::is_convertible_v<Arg, ಧ್ರುವ std::string &>) {
			ಧ್ರುವ std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ constexpr (std::is_convertible_v<Arg, ಧ್ರುವ std::string_view &>) {
			ಧ್ರುವ std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ constexpr (has_name_member<Arg>()) {
			ಧ್ರುವ std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ constexpr (ptr_has_name_member<Arg>()) {
			ಧ್ರುವ std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ CONVSPEC_VOID_PTR:
		ಯದಿ constexpr (std::is_convertible_v<Arg, ಧ್ರುವ ಶೂನ್ಯ *>) {
			ಧ್ರುವ ಶೂನ್ಯ *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
ಶೂನ್ಯ format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ format_emit(std::string &result, std::string_view fmt, ಪೂರ್ಣಾಙ್ಕ fmt_start,
	bool has_escapes, ಧ್ರುವ FoundFormatSpec*, ಪೂರ್ಣಾಙ್ಕ*, Dynamic_Int_ಎಣಿಕೆ)
{
	fmt = fmt.substr(fmt_start);
	ಯದಿ (has_escapes) {
		format_emit_unescaped(result, fmt);
	} ಅನ್ಯಥಾ {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ format_emit(std::string &result, std::string_view fmt, ಪೂರ್ಣಾಙ್ಕ fmt_start, bool has_escapes,
	ಧ್ರುವ FoundFormatSpec* specs, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints, Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints,
	ಧ್ರುವ Arg &arg, ಧ್ರುವ Args &... args)
{
	ಯದಿ (specs->ಅಂಕೆ_dynamic_ints > ಅಂಕೆ_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		ಯದಿ constexpr (std::is_convertible_v<Arg, ಪೂರ್ಣಾಙ್ಕ>) {
			dynamic_ints[static_cast<uint8_t>(ಅಂಕೆ_dynamic_ints)] = arg;
		} ಅನ್ಯಥಾ {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_ಎಣಿಕೆ>(static_cast<uint8_t>(ಅಂಕೆ_dynamic_ints) + 1), args...);
		ಪ್ರತ್ಯಾಗಮ;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	ಯದಿ (has_escapes) {
		format_emit_unescaped(result, str);
	} ಅನ್ಯಥಾ {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_ಎಣಿಕೆ::NONE, args...);
}

template <typename... Args>
ಅನ್ತಃಸ್ಥ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ಧ್ರುವ FoundFormatSpec* specs, ಧ್ರುವ Args &... args)
{
	std::string result;
	ಪೂರ್ಣಾಙ್ಕ dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_ಎಣಿಕೆ::NONE, args...);
	ಪ್ರತ್ಯಾಗಮ result;
}
template <>
ಅನ್ತಃಸ್ಥ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ಧ್ರುವ FoundFormatSpec*)
{
	ಯದಿ (!has_escapes) {
		ಪ್ರತ್ಯಾಗಮ std::string(fmt);
	}
	ಪ್ರತ್ಯಾಗಮ unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
ಸಾರ್ವಜನಿಕ:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ಧ್ರುವ ವರ್ಣ *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_ಎಣಿಕೆ::NONE);
	}
	std::string format(ಧ್ರುವ Args &... args)
	{
		ಪ್ರತ್ಯಾಗಮ format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ fmt; }
ಗುಪ್ತ:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[ಪರಿಮಾಣ...(Args) ? ಪರಿಮಾಣ...(Args) : 1] = {};
};

template <typename T> ಸಂರಚನಾ WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
ಅನ್ತಃಸ್ಥ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ಧ್ರುವ Args &... args)
{
	ಪ್ರತ್ಯಾಗಮ fmt.format(args...);
}

ಪೂರ್ಣಾಙ್ಕ readsome(std::istream &f, ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ n);
std::string next_token(std::string &text, ಧ್ರುವ ವರ್ಣ *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ಧ್ರುವ std::string &text, ಧ್ರುವ ವರ್ಣ *sep = " \t\r\n");
bool patmatch(ಧ್ರುವ ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *string);
#ಯದಿ !defined(YOSYS_DISABLE_SPAWN)
ಪೂರ್ಣಾಙ್ಕ run_command(ಧ್ರುವ std::string &command, std::function<ಶೂನ್ಯ(ಧ್ರುವ std::string&)> process_ಸಾಲು = std::function<ಶೂನ್ಯ(ಧ್ರುವ std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ಧ್ರುವ std::string& filename, bool is_exec = false);
bool check_directory_exists(ಧ್ರುವ std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
ಶೂನ್ಯ remove_directory(std::string dirname);
bool create_directory(ಧ್ರುವ std::string& dirname);
bool make_private_directory(ಧ್ರುವ std::string& dirname);
std::string make_user_tmpdir(ಧ್ರುವ std::string& name);
std::string escape_filename_spaces(ಧ್ರುವ std::string& filename);
ಶೂನ್ಯ append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ಧ್ರುವ std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ಸಂರಚನಾ ScopedFileLock {
	ScopedFileLock(ಧ್ರುವ std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ಧ್ರುವ ScopedFileLock &) = delete;
	ScopedFileLock &operator=(ಧ್ರುವ ScopedFileLock &) = delete;
ಗುಪ್ತ:
#ಯದಿ defined(_WIN32)
	ಶೂನ್ಯ *handle = nullptr;
#ನೋಚೇದ್ಯದಿ !defined(__wasi__)
	ಪೂರ್ಣಾಙ್ಕ fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
