#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_IO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ IdString;

ਸਤਰਅੰਦਰ std::string vstringf(ਅਚਲ ਅੱਖਰ *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ਅਚਲ ਪੂਰਨ k_Buf_ਮਾਪ = 128;
	ਅੱਖਰ buf[k_Buf_ਮਾਪ];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	ਪੂਰਨ n = vsnprintf(buf, k_Buf_ਮਾਪ, fmt, apc);
	va_ਸਿਰਾ(apc);
	ਜੇ (n < k_Buf_ਮਾਪ)
		ਵਾਪਸ std::string(buf);

	std::string string;
	ਅੱਖਰ *str = ਨਿਰਾ;
#ਜੇ defined(_WIN32) || defined(__CYGWIN__)
	ਪੂਰਨ sz = 2 * k_Buf_ਮਾਪ, rc;
	ਜਦੋਂ (1) {
		va_copy(apc, ap);
		str = (ਅੱਖਰ *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_ਸਿਰਾ(apc);
		ਜੇ (rc >= 0 && rc < sz)
			ਰੋਕੋ;
		sz *= 2;
	}
	ਜੇ (str != ਨਿਰਾ) {
		string = str;
		ਮੁਕਤ(str);
	}
	ਵਾਪਸ string;
#ਨਹੀਂਤਾਂ
	ਜੇ (vasprintf(&str, fmt, ap) < 0)
		str = ਨਿਰਾ;
	ਜੇ (str != ਨਿਰਾ) {
		string = str;
		ਮੁਕਤ(str);
	}
	ਵਾਪਸ string;
#ਜੇ_ਅੰਤ
}

ਗਿਣਤੀ ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(ਅੱਖਰ ch, ਅੱਖਰ prev_ch)
{
	ਚੋਣ (ch) {
	ਹਾਲਤ 'd':
	ਹਾਲਤ 'i':
		ਵਾਪਸ CONVSPEC_SIGNED_INT;
	ਹਾਲਤ 'o':
	ਹਾਲਤ 'u':
	ਹਾਲਤ 'x':
	ਹਾਲਤ 'X':
	ਹਾਲਤ 'm':
		ਵਾਪਸ CONVSPEC_UNSIGNED_INT;
	ਹਾਲਤ 'c':
		ਜੇ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ਵਾਪਸ CONVSPEC_ERROR;
		}
		ਵਾਪਸ CONVSPEC_UNSIGNED_INT;
	ਹਾਲਤ 'e':
	ਹਾਲਤ 'E':
	ਹਾਲਤ 'f':
	ਹਾਲਤ 'F':
	ਹਾਲਤ 'g':
	ਹਾਲਤ 'G':
	ਹਾਲਤ 'a':
	ਹਾਲਤ 'A':
		ਵਾਪਸ CONVSPEC_DOUBLE;
	ਹਾਲਤ 's':
		ਜੇ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ਵਾਪਸ CONVSPEC_ERROR;
		}
		ਵਾਪਸ CONVSPEC_CHAR_PTR;
	ਹਾਲਤ 'p':
		ਵਾਪਸ CONVSPEC_VOID_PTR;
	ਹਾਲਤ '$': // positional parameters
	ਹਾਲਤ 'n':
	ਹਾਲਤ 'S':
		ਵਾਪਸ CONVSPEC_ERROR;
	ਮੂਲ:
		ਵਾਪਸ CONVSPEC_NONE;
	}
}

ਗਿਣਤੀ class Dynamic_Int_ਗਿਣਤੀ : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ਬਣਤਰ ਮਿਲਿਆ_Format_Spec
{
	// The start offset of the conversion spec in the format string.
	ਪੂਰਨ ਸ਼ੁਰੂ;
	// The end offset of the conversion spec in the format string.
	ਪੂਰਨ ਸਿਰਾ;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints;
};

// Ensure there is no format spec.
constexpr ਖਾਲੀ ensure_no_format_spec(std::string_view fmt, ਪੂਰਨ ਸੂਚਕਾਂਕ, bool *has_escapes)
{
	ਪੂਰਨ fmt_ਮਾਪ = static_cast<ਪੂਰਨ>(fmt.ਮਾਪ());
	// A trailing '%' is not a format spec.
	ਜਦੋਂ (ਸੂਚਕਾਂਕ + 1 < fmt_ਮਾਪ) {
		ਜੇ (fmt[ਸੂਚਕਾਂਕ] != '%') {
			++ਸੂਚਕਾਂਕ;
			ਜਾਰੀ;
		}
		ਜੇ (fmt[ਸੂਚਕਾਂਕ + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		ਸੂਚਕਾਂਕ += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr ਮਿਲਿਆ_Format_Spec ਲੱਭੋ_ਅਗਲਾ_format_spec(std::string_view fmt, ਪੂਰਨ fmt_ਸ਼ੁਰੂ, bool *has_escapes)
{
	ਪੂਰਨ ਸੂਚਕਾਂਕ = fmt_ਸ਼ੁਰੂ;
	ਪੂਰਨ fmt_ਮਾਪ = static_cast<ਪੂਰਨ>(fmt.ਮਾਪ());
	ਜਦੋਂ (ਸੂਚਕਾਂਕ < fmt_ਮਾਪ) {
		ਜੇ (fmt[ਸੂਚਕਾਂਕ] != '%') {
			++ਸੂਚਕਾਂਕ;
			ਜਾਰੀ;
		}
		ਪੂਰਨ p = ਸੂਚਕਾਂਕ + 1;
		uint8_t ਅੰਕ_dynamic_ints = 0;
		ਜਦੋਂ (true) {
			ਜੇ (p == fmt_ਮਾਪ) {
				ਵਾਪਸ {0, 0, CONVSPEC_NONE, Dynamic_Int_ਗਿਣਤੀ::NONE};
			}
			ਜੇ (fmt[p] == '%') {
				*has_escapes = true;
				ਸੂਚਕਾਂਕ = p + 1;
				ਰੋਕੋ;
			}
			ਜੇ (fmt[p] == '*') {
				ਜੇ (ਅੰਕ_dynamic_ints >= 2) {
					ਵਾਪਸ {0, 0, CONVSPEC_ERROR, Dynamic_Int_ਗਿਣਤੀ::NONE};
				}
				++ਅੰਕ_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			ਜੇ (spec != CONVSPEC_NONE) {
				ਵਾਪਸ {ਸੂਚਕਾਂਕ, p + 1, spec, static_cast<Dynamic_Int_ਗਿਣਤੀ>(ਅੰਕ_dynamic_ints)};
			}
			++p;
		}
	}
	ਵਾਪਸ {0, 0, CONVSPEC_NONE, Dynamic_Int_ਗਿਣਤੀ::NONE};
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
constexpr typename std::enable_if<ਆਕਾਰ...(ਦਲੀਲਾਂ_3) == 0>::type
check_format(std::string_view fmt, ਪੂਰਨ fmt_ਸ਼ੁਰੂ, bool *has_escapes, ਮਿਲਿਆ_Format_Spec*, Dynamic_Int_ਗਿਣਤੀ)
{
	ensure_no_format_spec(fmt, fmt_ਸ਼ੁਰੂ, has_escapes);
}

ਸਾਂਚਾ <class T>
ਸਥਿਰ ਸਵੈਚਾਲਿਤ has_name_member_imp(ਪੂਰਨ)
	-> decltype(static_cast<ਅਚਲ IdString>(std::declval<T>().name), std::true_type{});

ਸਾਂਚਾ <class T>
ਸਥਿਰ ਸਵੈਚਾਲਿਤ has_name_member_imp(ਲੰਮਾ)
	-> std::false_type;

ਸਾਂਚਾ <class T>
ਬਣਤਰ has_name_member : decltype(has_name_member_imp<T>(0)){};

ਸਾਂਚਾ <class T>
ਸਥਿਰ ਸਵੈਚਾਲਿਤ ptr_has_name_member_imp(ਪੂਰਨ)
	-> decltype(static_cast<ਅਚਲ IdString>(std::declval<T>()->name), std::true_type{});

ਸਾਂਚਾ <class T>
ਸਥਿਰ ਸਵੈਚਾਲਿਤ ptr_has_name_member_imp(ਲੰਮਾ)
	-> std::false_type;

ਸਾਂਚਾ <class T>
ਬਣਤਰ ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
ਸਾਂਚਾ <typename Arg, typename... ਦਲੀਲਾਂ_3>
constexpr ਖਾਲੀ check_format(std::string_view fmt, ਪੂਰਨ fmt_ਸ਼ੁਰੂ, bool *has_escapes, ਮਿਲਿਆ_Format_Spec* specs,
        Dynamic_Int_ਗਿਣਤੀ int_ਦਲੀਲਾਂ_consumed)
{
	ਮਿਲਿਆ_Format_Spec ਮਿਲਿਆ = ਲੱਭੋ_ਅਗਲਾ_format_spec(fmt, fmt_ਸ਼ੁਰੂ, has_escapes);
	ਜੇ (ਮਿਲਿਆ.ਅੰਕ_dynamic_ints > int_ਦਲੀਲਾਂ_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		ਜੇ constexpr (!std::is_convertible_v<Arg, ਪੂਰਨ>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<ਦਲੀਲਾਂ_3...>(fmt, fmt_ਸ਼ੁਰੂ, has_escapes, specs,
			static_cast<Dynamic_Int_ਗਿਣਤੀ>(static_cast<uint8_t>(int_ਦਲੀਲਾਂ_consumed) + 1));
		ਵਾਪਸ;
	}
	ਚੋਣ (ਮਿਲਿਆ.spec) {
	ਹਾਲਤ CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_SIGNED_INT:
		ਜੇ constexpr (!std::is_convertible_v<Arg, ਲੰਮਾ ਲੰਮਾ>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = ਮਿਲਿਆ;
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_UNSIGNED_INT:
		ਜੇ constexpr (!std::is_convertible_v<Arg, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = ਮਿਲਿਆ;
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_DOUBLE:
		ਜੇ constexpr (!std::is_convertible_v<Arg, ਦੁੱਗਣਾ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = ਮਿਲਿਆ;
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_CHAR_PTR:
		ਜੇ constexpr (!std::is_convertible_v<Arg, ਅਚਲ ਅੱਖਰ *> &&
		        !std::is_convertible_v<Arg, ਅਚਲ std::string &> &&
			!std::is_convertible_v<Arg, ਅਚਲ std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = ਮਿਲਿਆ;
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_VOID_PTR:
		ਜੇ constexpr (!std::is_convertible_v<Arg, ਅਚਲ ਖਾਲੀ *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = ਮਿਲਿਆ;
		ਰੋਕੋ;
	}
	check_format<ਦਲੀਲਾਂ_3...>(fmt, ਮਿਲਿਆ.ਸਿਰਾ, has_escapes, specs + 1, Dynamic_Int_ਗਿਣਤੀ::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
ਖਾਲੀ format_emit_long_long(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਲੰਮਾ ਲੰਮਾ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
ਖਾਲੀ format_emit_unsigned_long_long(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ਖਾਲੀ format_emit_double(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਦੁੱਗਣਾ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
ਖਾਲੀ format_emit_char_ptr(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਅਚਲ ਅੱਖਰ *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
ਖਾਲੀ format_emit_string(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਅਚਲ std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
ਖਾਲੀ format_emit_string_view(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ਖਾਲੀ format_emit_void_ptr(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਅਚਲ ਖਾਲੀ *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
ਸਾਂਚਾ <typename Arg>
ਸਤਰਅੰਦਰ ਖਾਲੀ format_emit_one(std::string &ਨਤੀਜਾ, std::string_view fmt, ਅਚਲ ਮਿਲਿਆ_Format_Spec &ffspec,
	ਪੂਰਨ *dynamic_ints, ਅਚਲ Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.ਸ਼ੁਰੂ, ffspec.ਸਿਰਾ - ffspec.ਸ਼ੁਰੂ);
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints = ffspec.ਅੰਕ_dynamic_ints;
	ਚੋਣ (ffspec.spec) {
	ਹਾਲਤ CONVSPEC_SIGNED_INT:
		ਜੇ constexpr (std::is_convertible_v<Arg, ਲੰਮਾ ਲੰਮਾ>) {
			ਲੰਮਾ ਲੰਮਾ s = arg;
			format_emit_long_long(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_UNSIGNED_INT:
		ਜੇ constexpr (std::is_convertible_v<Arg, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ>) {
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ s = arg;
			format_emit_unsigned_long_long(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_DOUBLE:
		ਜੇ constexpr (std::is_convertible_v<Arg, ਦੁੱਗਣਾ>) {
			ਦੁੱਗਣਾ s = arg;
			format_emit_double(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_CHAR_PTR:
		ਜੇ constexpr (std::is_convertible_v<Arg, ਅਚਲ ਅੱਖਰ *>) {
			ਅਚਲ ਅੱਖਰ *s = arg;
			format_emit_char_ptr(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਜੇ constexpr (std::is_convertible_v<Arg, ਅਚਲ std::string &>) {
			ਅਚਲ std::string &s = arg;
			format_emit_string(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਜੇ constexpr (std::is_convertible_v<Arg, ਅਚਲ std::string_view &>) {
			ਅਚਲ std::string_view &s = arg;
			format_emit_string_view(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਜੇ constexpr (has_name_member<Arg>()) {
			ਅਚਲ std::string &s = arg.name.unescape();
			format_emit_string(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਜੇ constexpr (ptr_has_name_member<Arg>()) {
			ਅਚਲ std::string &s = arg->name.unescape();
			format_emit_string(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਰੋਕੋ;
	ਹਾਲਤ CONVSPEC_VOID_PTR:
		ਜੇ constexpr (std::is_convertible_v<Arg, ਅਚਲ ਖਾਲੀ *>) {
			ਅਚਲ ਖਾਲੀ *s = arg;
			format_emit_void_ptr(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, s);
			ਵਾਪਸ;
		}
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
ਖਾਲੀ format_emit_unescaped(std::string &ਨਤੀਜਾ, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

ਸਤਰਅੰਦਰ ਖਾਲੀ format_emit(std::string &ਨਤੀਜਾ, std::string_view fmt, ਪੂਰਨ fmt_ਸ਼ੁਰੂ,
	bool has_escapes, ਅਚਲ ਮਿਲਿਆ_Format_Spec*, ਪੂਰਨ*, Dynamic_Int_ਗਿਣਤੀ)
{
	fmt = fmt.substr(fmt_ਸ਼ੁਰੂ);
	ਜੇ (has_escapes) {
		format_emit_unescaped(ਨਤੀਜਾ, fmt);
	} ਨਹੀਂਤਾਂ {
		ਨਤੀਜਾ += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
ਸਾਂਚਾ <typename Arg, typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ format_emit(std::string &ਨਤੀਜਾ, std::string_view fmt, ਪੂਰਨ fmt_ਸ਼ੁਰੂ, bool has_escapes,
	ਅਚਲ ਮਿਲਿਆ_Format_Spec* specs, ਪੂਰਨ *dynamic_ints, Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints,
	ਅਚਲ Arg &arg, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	ਜੇ (specs->ਅੰਕ_dynamic_ints > ਅੰਕ_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		ਜੇ constexpr (std::is_convertible_v<Arg, ਪੂਰਨ>) {
			dynamic_ints[static_cast<uint8_t>(ਅੰਕ_dynamic_ints)] = arg;
		} ਨਹੀਂਤਾਂ {
			YOSYS_ABORT("Internal error");
		}
		format_emit(ਨਤੀਜਾ, fmt, fmt_ਸ਼ੁਰੂ, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_ਗਿਣਤੀ>(static_cast<uint8_t>(ਅੰਕ_dynamic_ints) + 1), ਦਲੀਲਾਂ_2...);
		ਵਾਪਸ;
	}
	std::string_view str = fmt.substr(fmt_ਸ਼ੁਰੂ, specs->ਸ਼ੁਰੂ - fmt_ਸ਼ੁਰੂ);
	ਜੇ (has_escapes) {
		format_emit_unescaped(ਨਤੀਜਾ, str);
	} ਨਹੀਂਤਾਂ {
		ਨਤੀਜਾ += str;
	}
	format_emit_one(ਨਤੀਜਾ, fmt, *specs, dynamic_ints, arg);
	format_emit(ਨਤੀਜਾ, fmt, specs->ਸਿਰਾ, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_ਗਿਣਤੀ::NONE, ਦਲੀਲਾਂ_2...);
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ਅਚਲ ਮਿਲਿਆ_Format_Spec* specs, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	std::string ਨਤੀਜਾ;
	ਪੂਰਨ dynamic_ints[2] = { 0, 0 };
	format_emit(ਨਤੀਜਾ, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_ਗਿਣਤੀ::NONE, ਦਲੀਲਾਂ_2...);
	ਵਾਪਸ ਨਤੀਜਾ;
}
ਸਾਂਚਾ <>
ਸਤਰਅੰਦਰ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ਅਚਲ ਮਿਲਿਆ_Format_Spec*)
{
	ਜੇ (!has_escapes) {
		ਵਾਪਸ std::string(fmt);
	}
	ਵਾਪਸ unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
class FmtString
{
ਜਨਤਕ:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ਅਚਲ ਅੱਖਰ *p) : fmt(p)
	{
		check_format<ਦਲੀਲਾਂ_3...>(fmt, 0, &has_escapes, specs, Dynamic_Int_ਗਿਣਤੀ::NONE);
	}
	std::string format(ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
	{
		ਵਾਪਸ format_emit_toplevel(fmt, has_escapes, specs, ਦਲੀਲਾਂ_2...);
	}
	std::string_view format_string() ਅਚਲ { ਵਾਪਸ fmt; }
ਨਿਜੀ:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	ਮਿਲਿਆ_Format_Spec specs[ਆਕਾਰ...(ਦਲੀਲਾਂ_3) ? ਆਕਾਰ...(ਦਲੀਲਾਂ_3) : 1] = {};
};

ਸਾਂਚਾ <typename T> ਬਣਤਰ WrapType { using type = T; };
ਸਾਂਚਾ <typename T> using Type_ਪਛਾਣ_ਤੱਤ = typename WrapType<T>::type;

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ std::string stringf(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	ਵਾਪਸ fmt.format(ਦਲੀਲਾਂ_2...);
}

ਪੂਰਨ readsome(std::istream &f, ਅੱਖਰ *s, ਪੂਰਨ n);
std::string ਅਗਲਾ_token(std::string &ਪਾਠ, ਅਚਲ ਅੱਖਰ *sep = " \t\r\n", bool long_ਲੜੀਆਂ = false);
std::ਸਦਿਸ਼<std::string> ਵੰਡੋ_ਟੋਕਨ(ਅਚਲ std::string &ਪਾਠ, ਅਚਲ ਅੱਖਰ *sep = " \t\r\n");
bool patmatch(ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *string);
#ਜੇ !defined(YOSYS_DISABLE_SPAWN)
ਪੂਰਨ ਲੜੀਵਾਰ_command(ਅਚਲ std::string &command, std::function<ਖਾਲੀ(ਅਚਲ std::string&)> process_ਸਤਰ = std::function<ਖਾਲੀ(ਅਚਲ std::string&)>());
#ਜੇ_ਅੰਤ
std::string get_ਅਧਾਰ_tmpdir();
std::string make_ਆਰਜ਼ੀ_file(std::string template_str = get_ਅਧਾਰ_tmpdir() + "/yosys_XXXXXX");
std::string make_ਆਰਜ਼ੀ_dir(std::string template_str = get_ਅਧਾਰ_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ਅਚਲ std::string& filename, bool is_exec = false);
bool check_directory_exists(ਅਚਲ std::string& dirname, bool is_exec = false);
bool is_absolute_ਰਸਤਾ(std::string filename);
ਖਾਲੀ remove_directory(std::string dirname);
bool create_directory(ਅਚਲ std::string& dirname);
bool make_ਨਿੱਜੀ_directory(ਅਚਲ std::string& dirname);
std::string make_user_tmpdir(ਅਚਲ std::string& name);
std::string escape_filename_spaces(ਅਚਲ std::string& filename);
ਖਾਲੀ append_globbed(std::ਸਦਿਸ਼<std::string>& paths, std::string ਨਮੂਨਾ);
std::string name_from_file_ਰਸਤਾ(std::string ਰਸਤਾ_2);
std::string absolute_ਰਸਤਾ(ਅਚਲ std::string& ਰਸਤਾ_2);
std::string ਮਾਪਾ_from_file_ਰਸਤਾ(std::string ਰਸਤਾ_2);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ਬਣਤਰ Scoped_File_ਤਾਲਾ {
	Scoped_File_ਤਾਲਾ(ਅਚਲ std::string &ਰਸਤਾ_2);
	~Scoped_File_ਤਾਲਾ();
	Scoped_File_ਤਾਲਾ(ਅਚਲ Scoped_File_ਤਾਲਾ &) = delete;
	Scoped_File_ਤਾਲਾ &ਸੰਚਾਲਕ=(ਅਚਲ Scoped_File_ਤਾਲਾ &) = delete;
ਨਿਜੀ:
#ਜੇ defined(_WIN32)
	ਖਾਲੀ *handle = nullptr;
#ਜਾਂਜੇ !defined(__wasi__)
	ਪੂਰਨ fd = -1;
#ਜੇ_ਅੰਤ
};

// Current process id (0 on WASI).
ਬਿਨਾਨਿਸ਼ਾਨੀ get_process_id();

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // YOSYS_IO_H
