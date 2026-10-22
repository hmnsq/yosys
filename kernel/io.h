#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_IO_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ರಚನೆ IdString;

ಸಾಲೊಳಗೆ std::string vstringf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ k_Buf_ಗಾತ್ರ = 128;
	ಅಕ್ಷರ buf[k_Buf_ಗಾತ್ರ];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	ಪೂರ್ಣಾಂಕ n = vsnprintf(buf, k_Buf_ಗಾತ್ರ, fmt, apc);
	va_ಕೊನೆ(apc);
	ಆದರೆ (n < k_Buf_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ std::string(buf);

	std::string string;
	ಅಕ್ಷರ *str = ಖಾಲಿ;
#ಆದರೆ defined(_WIN32) || defined(__CYGWIN__)
	ಪೂರ್ಣಾಂಕ sz = 2 * k_Buf_ಗಾತ್ರ, rc;
	ಆಗ (1) {
		va_copy(apc, ap);
		str = (ಅಕ್ಷರ *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_ಕೊನೆ(apc);
		ಆದರೆ (rc >= 0 && rc < sz)
			ನಿಲ್ಲಿಸು;
		sz *= 2;
	}
	ಆದರೆ (str != ಖಾಲಿ) {
		string = str;
		ಬಿಡುಗಡೆ(str);
	}
	ಹಿಂತಿರುಗಿ string;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (vasprintf(&str, fmt, ap) < 0)
		str = ಖಾಲಿ;
	ಆದರೆ (str != ಖಾಲಿ) {
		string = str;
		ಬಿಡುಗಡೆ(str);
	}
	ಹಿಂತಿರುಗಿ string;
#ಆದರೆ_ಅಂತ್ಯ
}

ಎಣಿಕೆ ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(ಅಕ್ಷರ ch, ಅಕ್ಷರ prev_ch)
{
	ಆಯ್ಕೆ (ch) {
	ಸ್ಥಿತಿ 'd':
	ಸ್ಥಿತಿ 'i':
		ಹಿಂತಿರುಗಿ CONVSPEC_SIGNED_INT;
	ಸ್ಥಿತಿ 'o':
	ಸ್ಥಿತಿ 'u':
	ಸ್ಥಿತಿ 'x':
	ಸ್ಥಿತಿ 'X':
	ಸ್ಥಿತಿ 'm':
		ಹಿಂತಿರುಗಿ CONVSPEC_UNSIGNED_INT;
	ಸ್ಥಿತಿ 'c':
		ಆದರೆ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ಹಿಂತಿರುಗಿ CONVSPEC_ERROR;
		}
		ಹಿಂತಿರುಗಿ CONVSPEC_UNSIGNED_INT;
	ಸ್ಥಿತಿ 'e':
	ಸ್ಥಿತಿ 'E':
	ಸ್ಥಿತಿ 'f':
	ಸ್ಥಿತಿ 'F':
	ಸ್ಥಿತಿ 'g':
	ಸ್ಥಿತಿ 'G':
	ಸ್ಥಿತಿ 'a':
	ಸ್ಥಿತಿ 'A':
		ಹಿಂತಿರುಗಿ CONVSPEC_DOUBLE;
	ಸ್ಥಿತಿ 's':
		ಆದರೆ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ಹಿಂತಿರುಗಿ CONVSPEC_ERROR;
		}
		ಹಿಂತಿರುಗಿ CONVSPEC_CHAR_PTR;
	ಸ್ಥಿತಿ 'p':
		ಹಿಂತಿರುಗಿ CONVSPEC_VOID_PTR;
	ಸ್ಥಿತಿ '$': // positional parameters
	ಸ್ಥಿತಿ 'n':
	ಸ್ಥಿತಿ 'S':
		ಹಿಂತಿರುಗಿ CONVSPEC_ERROR;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ CONVSPEC_NONE;
	}
}

ಎಣಿಕೆ class Dynamic_Int_ಎಣಿಕೆ : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ರಚನೆ ಸಿಕ್ಕಿತು_Format_Spec
{
	// The start offset of the conversion spec in the format string.
	ಪೂರ್ಣಾಂಕ ಆರಂಭ;
	// The end offset of the conversion spec in the format string.
	ಪೂರ್ಣಾಂಕ ಕೊನೆ;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints;
};

// Ensure there is no format spec.
constexpr ಶೂನ್ಯ ensure_no_format_spec(std::string_view fmt, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, bool *has_escapes)
{
	ಪೂರ್ಣಾಂಕ fmt_ಗಾತ್ರ = static_cast<ಪೂರ್ಣಾಂಕ>(fmt.ಗಾತ್ರ_2());
	// A trailing '%' is not a format spec.
	ಆಗ (ಸೂಚ್ಯಂಕ + 1 < fmt_ಗಾತ್ರ) {
		ಆದರೆ (fmt[ಸೂಚ್ಯಂಕ] != '%') {
			++ಸೂಚ್ಯಂಕ;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (fmt[ಸೂಚ್ಯಂಕ + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		ಸೂಚ್ಯಂಕ += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr ಸಿಕ್ಕಿತು_Format_Spec ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_format_spec(std::string_view fmt, ಪೂರ್ಣಾಂಕ fmt_ಆರಂಭ, bool *has_escapes)
{
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = fmt_ಆರಂಭ;
	ಪೂರ್ಣಾಂಕ fmt_ಗಾತ್ರ = static_cast<ಪೂರ್ಣಾಂಕ>(fmt.ಗಾತ್ರ_2());
	ಆಗ (ಸೂಚ್ಯಂಕ < fmt_ಗಾತ್ರ) {
		ಆದರೆ (fmt[ಸೂಚ್ಯಂಕ] != '%') {
			++ಸೂಚ್ಯಂಕ;
			ಮುಂದುವರಿ;
		}
		ಪೂರ್ಣಾಂಕ p = ಸೂಚ್ಯಂಕ + 1;
		uint8_t ಅಂಕೆ_dynamic_ints = 0;
		ಆಗ (true) {
			ಆದರೆ (p == fmt_ಗಾತ್ರ) {
				ಹಿಂತಿರುಗಿ {0, 0, CONVSPEC_NONE, Dynamic_Int_ಎಣಿಕೆ::NONE};
			}
			ಆದರೆ (fmt[p] == '%') {
				*has_escapes = true;
				ಸೂಚ್ಯಂಕ = p + 1;
				ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (fmt[p] == '*') {
				ಆದರೆ (ಅಂಕೆ_dynamic_ints >= 2) {
					ಹಿಂತಿರುಗಿ {0, 0, CONVSPEC_ERROR, Dynamic_Int_ಎಣಿಕೆ::NONE};
				}
				++ಅಂಕೆ_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			ಆದರೆ (spec != CONVSPEC_NONE) {
				ಹಿಂತಿರುಗಿ {ಸೂಚ್ಯಂಕ, p + 1, spec, static_cast<Dynamic_Int_ಎಣಿಕೆ>(ಅಂಕೆ_dynamic_ints)};
			}
			++p;
		}
	}
	ಹಿಂತಿರುಗಿ {0, 0, CONVSPEC_NONE, Dynamic_Int_ಎಣಿಕೆ::NONE};
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
constexpr typename std::enable_if<ಗಾತ್ರ...(ವಾದಗಳು_3) == 0>::type
check_format(std::string_view fmt, ಪೂರ್ಣಾಂಕ fmt_ಆರಂಭ, bool *has_escapes, ಸಿಕ್ಕಿತು_Format_Spec*, Dynamic_Int_ಎಣಿಕೆ)
{
	ensure_no_format_spec(fmt, fmt_ಆರಂಭ, has_escapes);
}

ಮಾದರಿ <class T>
ಸ್ಥಿರ ಸ್ವಯಂಚಾಲಿತ has_name_member_imp(ಪೂರ್ಣಾಂಕ)
	-> decltype(static_cast<ಸ್ಥಿರಾಂಕ IdString>(std::declval<T>().name), std::true_type{});

ಮಾದರಿ <class T>
ಸ್ಥಿರ ಸ್ವಯಂಚಾಲಿತ has_name_member_imp(ದೀರ್ಘ)
	-> std::false_type;

ಮಾದರಿ <class T>
ರಚನೆ has_name_member : decltype(has_name_member_imp<T>(0)){};

ಮಾದರಿ <class T>
ಸ್ಥಿರ ಸ್ವಯಂಚಾಲಿತ ptr_has_name_member_imp(ಪೂರ್ಣಾಂಕ)
	-> decltype(static_cast<ಸ್ಥಿರಾಂಕ IdString>(std::declval<T>()->name), std::true_type{});

ಮಾದರಿ <class T>
ಸ್ಥಿರ ಸ್ವಯಂಚಾಲಿತ ptr_has_name_member_imp(ದೀರ್ಘ)
	-> std::false_type;

ಮಾದರಿ <class T>
ರಚನೆ ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
ಮಾದರಿ <typename Arg, typename... ವಾದಗಳು_3>
constexpr ಶೂನ್ಯ check_format(std::string_view fmt, ಪೂರ್ಣಾಂಕ fmt_ಆರಂಭ, bool *has_escapes, ಸಿಕ್ಕಿತು_Format_Spec* specs,
        Dynamic_Int_ಎಣಿಕೆ int_ವಾದಗಳು_consumed)
{
	ಸಿಕ್ಕಿತು_Format_Spec ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_format_spec(fmt, fmt_ಆರಂಭ, has_escapes);
	ಆದರೆ (ಸಿಕ್ಕಿತು.ಅಂಕೆ_dynamic_ints > int_ವಾದಗಳು_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		ಆದರೆ constexpr (!std::is_convertible_v<Arg, ಪೂರ್ಣಾಂಕ>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<ವಾದಗಳು_3...>(fmt, fmt_ಆರಂಭ, has_escapes, specs,
			static_cast<Dynamic_Int_ಎಣಿಕೆ>(static_cast<uint8_t>(int_ವಾದಗಳು_consumed) + 1));
		ಹಿಂತಿರುಗಿ;
	}
	ಆಯ್ಕೆ (ಸಿಕ್ಕಿತು.spec) {
	ಸ್ಥಿತಿ CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_SIGNED_INT:
		ಆದರೆ constexpr (!std::is_convertible_v<Arg, ದೀರ್ಘ ದೀರ್ಘ>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = ಸಿಕ್ಕಿತು;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_UNSIGNED_INT:
		ಆದರೆ constexpr (!std::is_convertible_v<Arg, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = ಸಿಕ್ಕಿತು;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_DOUBLE:
		ಆದರೆ constexpr (!std::is_convertible_v<Arg, ದ್ವಿಗುಣ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = ಸಿಕ್ಕಿತು;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_CHAR_PTR:
		ಆದರೆ constexpr (!std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *> &&
		        !std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ std::string &> &&
			!std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = ಸಿಕ್ಕಿತು;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_VOID_PTR:
		ಆದರೆ constexpr (!std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = ಸಿಕ್ಕಿತು;
		ನಿಲ್ಲಿಸು;
	}
	check_format<ವಾದಗಳು_3...>(fmt, ಸಿಕ್ಕಿತು.ಕೊನೆ, has_escapes, specs + 1, Dynamic_Int_ಎಣಿಕೆ::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
ಶೂನ್ಯ format_emit_long_long(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ದೀರ್ಘ ದೀರ್ಘ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
ಶೂನ್ಯ format_emit_unsigned_long_long(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ಶೂನ್ಯ format_emit_double(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ದ್ವಿಗುಣ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
ಶೂನ್ಯ format_emit_char_ptr(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
ಶೂನ್ಯ format_emit_string(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಸ್ಥಿರಾಂಕ std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
ಶೂನ್ಯ format_emit_string_view(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ಶೂನ್ಯ format_emit_void_ptr(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
ಮಾದರಿ <typename Arg>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ format_emit_one(std::string &ಫಲಿತಾಂಶ, std::string_view fmt, ಸ್ಥಿರಾಂಕ ಸಿಕ್ಕಿತು_Format_Spec &ffspec,
	ಪೂರ್ಣಾಂಕ *dynamic_ints, ಸ್ಥಿರಾಂಕ Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.ಆರಂಭ, ffspec.ಕೊನೆ - ffspec.ಆರಂಭ);
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints = ffspec.ಅಂಕೆ_dynamic_ints;
	ಆಯ್ಕೆ (ffspec.spec) {
	ಸ್ಥಿತಿ CONVSPEC_SIGNED_INT:
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ದೀರ್ಘ ದೀರ್ಘ>) {
			ದೀರ್ಘ ದೀರ್ಘ s = arg;
			format_emit_long_long(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_UNSIGNED_INT:
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ>) {
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ s = arg;
			format_emit_unsigned_long_long(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_DOUBLE:
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ದ್ವಿಗುಣ>) {
			ದ್ವಿಗುಣ s = arg;
			format_emit_double(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_CHAR_PTR:
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>) {
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = arg;
			format_emit_char_ptr(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ std::string &>) {
			ಸ್ಥಿರಾಂಕ std::string &s = arg;
			format_emit_string(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ std::string_view &>) {
			ಸ್ಥಿರಾಂಕ std::string_view &s = arg;
			format_emit_string_view(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ constexpr (has_name_member<Arg>()) {
			ಸ್ಥಿರಾಂಕ std::string &s = arg.name.unescape();
			format_emit_string(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ constexpr (ptr_has_name_member<Arg>()) {
			ಸ್ಥಿರಾಂಕ std::string &s = arg->name.unescape();
			format_emit_string(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CONVSPEC_VOID_PTR:
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *>) {
			ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s = arg;
			format_emit_void_ptr(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, s);
			ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
ಶೂನ್ಯ format_emit_unescaped(std::string &ಫಲಿತಾಂಶ, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

ಸಾಲೊಳಗೆ ಶೂನ್ಯ format_emit(std::string &ಫಲಿತಾಂಶ, std::string_view fmt, ಪೂರ್ಣಾಂಕ fmt_ಆರಂಭ,
	bool has_escapes, ಸ್ಥಿರಾಂಕ ಸಿಕ್ಕಿತು_Format_Spec*, ಪೂರ್ಣಾಂಕ*, Dynamic_Int_ಎಣಿಕೆ)
{
	fmt = fmt.substr(fmt_ಆರಂಭ);
	ಆದರೆ (has_escapes) {
		format_emit_unescaped(ಫಲಿತಾಂಶ, fmt);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಫಲಿತಾಂಶ += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
ಮಾದರಿ <typename Arg, typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ format_emit(std::string &ಫಲಿತಾಂಶ, std::string_view fmt, ಪೂರ್ಣಾಂಕ fmt_ಆರಂಭ, bool has_escapes,
	ಸ್ಥಿರಾಂಕ ಸಿಕ್ಕಿತು_Format_Spec* specs, ಪೂರ್ಣಾಂಕ *dynamic_ints, Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints,
	ಸ್ಥಿರಾಂಕ Arg &arg, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	ಆದರೆ (specs->ಅಂಕೆ_dynamic_ints > ಅಂಕೆ_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		ಆದರೆ constexpr (std::is_convertible_v<Arg, ಪೂರ್ಣಾಂಕ>) {
			dynamic_ints[static_cast<uint8_t>(ಅಂಕೆ_dynamic_ints)] = arg;
		} ಇಲ್ಲದಿದ್ದರೆ {
			YOSYS_ABORT("Internal error");
		}
		format_emit(ಫಲಿತಾಂಶ, fmt, fmt_ಆರಂಭ, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_ಎಣಿಕೆ>(static_cast<uint8_t>(ಅಂಕೆ_dynamic_ints) + 1), ವಾದಗಳು_2...);
		ಹಿಂತಿರುಗಿ;
	}
	std::string_view str = fmt.substr(fmt_ಆರಂಭ, specs->ಆರಂಭ - fmt_ಆರಂಭ);
	ಆದರೆ (has_escapes) {
		format_emit_unescaped(ಫಲಿತಾಂಶ, str);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಫಲಿತಾಂಶ += str;
	}
	format_emit_one(ಫಲಿತಾಂಶ, fmt, *specs, dynamic_ints, arg);
	format_emit(ಫಲಿತಾಂಶ, fmt, specs->ಕೊನೆ, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_ಎಣಿಕೆ::NONE, ವಾದಗಳು_2...);
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ಸ್ಥಿರಾಂಕ ಸಿಕ್ಕಿತು_Format_Spec* specs, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	std::string ಫಲಿತಾಂಶ;
	ಪೂರ್ಣಾಂಕ dynamic_ints[2] = { 0, 0 };
	format_emit(ಫಲಿತಾಂಶ, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_ಎಣಿಕೆ::NONE, ವಾದಗಳು_2...);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
ಮಾದರಿ <>
ಸಾಲೊಳಗೆ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ಸ್ಥಿರಾಂಕ ಸಿಕ್ಕಿತು_Format_Spec*)
{
	ಆದರೆ (!has_escapes) {
		ಹಿಂತಿರುಗಿ std::string(fmt);
	}
	ಹಿಂತಿರುಗಿ unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
ಮಾದರಿ <typename... ವಾದಗಳು_3>
class FmtString
{
ಸಾರ್ವಜನಿಕ:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) : fmt(p)
	{
		check_format<ವಾದಗಳು_3...>(fmt, 0, &has_escapes, specs, Dynamic_Int_ಎಣಿಕೆ::NONE);
	}
	std::string format(ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
	{
		ಹಿಂತಿರುಗಿ format_emit_toplevel(fmt, has_escapes, specs, ವಾದಗಳು_2...);
	}
	std::string_view format_string() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ fmt; }
ಖಾಸಗಿ:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	ಸಿಕ್ಕಿತು_Format_Spec specs[ಗಾತ್ರ...(ವಾದಗಳು_3) ? ಗಾತ್ರ...(ವಾದಗಳು_3) : 1] = {};
};

ಮಾದರಿ <typename T> ರಚನೆ WrapType { using type = T; };
ಮಾದರಿ <typename T> using Type_ತಟಸ್ಥ = typename WrapType<T>::type;

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ std::string stringf(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	ಹಿಂತಿರುಗಿ fmt.format(ವಾದಗಳು_2...);
}

ಪೂರ್ಣಾಂಕ readsome(std::istream &f, ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ n);
std::string ಮುಂದಿನ_token(std::string &ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = " \t\r\n", bool long_ಅಕ್ಷರಸರಣಿಗಳು = false);
std::ಸದಿಶ<std::string> ವಿಭಜಿಸು_ಟೋಕನುಗಳು(ಸ್ಥಿರಾಂಕ std::string &ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = " \t\r\n");
bool patmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string);
#ಆದರೆ !defined(YOSYS_DISABLE_SPAWN)
ಪೂರ್ಣಾಂಕ ಸತತ_command(ಸ್ಥಿರಾಂಕ std::string &command, std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ std::string&)> process_ಸಾಲು = std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ std::string&)>());
#ಆದರೆ_ಅಂತ್ಯ
std::string get_ತಳ_tmpdir();
std::string make_ತಾತ್ಕಾಲಿಕ_file(std::string template_str = get_ತಳ_tmpdir() + "/yosys_XXXXXX");
std::string make_ತಾತ್ಕಾಲಿಕ_dir(std::string template_str = get_ತಳ_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ಸ್ಥಿರಾಂಕ std::string& filename, bool is_exec = false);
bool check_directory_exists(ಸ್ಥಿರಾಂಕ std::string& dirname, bool is_exec = false);
bool is_absolute_ಮಾರ್ಗ(std::string filename);
ಶೂನ್ಯ remove_directory(std::string dirname);
bool create_directory(ಸ್ಥಿರಾಂಕ std::string& dirname);
bool make_ಖಾಸಗಿ_directory(ಸ್ಥಿರಾಂಕ std::string& dirname);
std::string make_user_tmpdir(ಸ್ಥಿರಾಂಕ std::string& name);
std::string escape_filename_spaces(ಸ್ಥಿರಾಂಕ std::string& filename);
ಶೂನ್ಯ append_globbed(std::ಸದಿಶ<std::string>& paths, std::string ನಮೂನೆ);
std::string name_from_file_ಮಾರ್ಗ(std::string ಮಾರ್ಗ_2);
std::string absolute_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ std::string& ಮಾರ್ಗ_2);
std::string ಪೋಷಕ_from_file_ಮಾರ್ಗ(std::string ಮಾರ್ಗ_2);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ರಚನೆ Scoped_File_ಬೀಗ {
	Scoped_File_ಬೀಗ(ಸ್ಥಿರಾಂಕ std::string &ಮಾರ್ಗ_2);
	~Scoped_File_ಬೀಗ();
	Scoped_File_ಬೀಗ(ಸ್ಥಿರಾಂಕ Scoped_File_ಬೀಗ &) = delete;
	Scoped_File_ಬೀಗ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Scoped_File_ಬೀಗ &) = delete;
ಖಾಸಗಿ:
#ಆದರೆ defined(_WIN32)
	ಶೂನ್ಯ *handle = nullptr;
#ಅಥವಾಆದರೆ !defined(__wasi__)
	ಪೂರ್ಣಾಂಕ fd = -1;
#ಆದರೆ_ಅಂತ್ಯ
};

// Current process id (0 on WASI).
ಚಿಹ್ನೆರಹಿತ get_process_id();

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // YOSYS_IO_H
