#अन्तर्भाव <string>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <type_traits>
#अन्तर्भाव "kernel/yosys_common.h"

#यदि_अपरिभाषितम् YOSYS_IO_H
#परिभाषय YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

संरचना IdString;

अन्तःस्थ std::string vstringf(ध्रुव वर्ण *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ध्रुव पूर्णाङ्क k_Buf_आकार = 128;
	वर्ण buf[k_Buf_आकार];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	पूर्णाङ्क n = vsnprintf(buf, k_Buf_आकार, fmt, apc);
	va_अन्तः(apc);
	यदि (n < k_Buf_आकार)
		प्रत्यागम std::string(buf);

	std::string string;
	वर्ण *str = रिक्त;
#यदि defined(_WIN32) || defined(__CYGWIN__)
	पूर्णाङ्क sz = 2 * k_Buf_आकार, rc;
	यावत् (1) {
		va_copy(apc, ap);
		str = (वर्ण *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_अन्तः(apc);
		यदि (rc >= 0 && rc < sz)
			भङ्ग;
		sz *= 2;
	}
	यदि (str != रिक्त) {
		string = str;
		मुक्त(str);
	}
	प्रत्यागम string;
#अन्यथा
	यदि (vasprintf(&str, fmt, ap) < 0)
		str = रिक्त;
	यदि (str != रिक्त) {
		string = str;
		मुक्त(str);
	}
	प्रत्यागम string;
#यदि_अन्तः
}

गणना ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(वर्ण ch, वर्ण पूर्व_ch)
{
	विकल्प (ch) {
	पक्ष 'd':
	पक्ष 'i':
		प्रत्यागम CONVSPEC_SIGNED_INT;
	पक्ष 'o':
	पक्ष 'u':
	पक्ष 'x':
	पक्ष 'X':
	पक्ष 'm':
		प्रत्यागम CONVSPEC_UNSIGNED_INT;
	पक्ष 'c':
		यदि (पूर्व_ch == 'l' || पूर्व_ch == 'q' || पूर्व_ch == 'L') {
			// wchar not supported
			प्रत्यागम CONVSPEC_ERROR;
		}
		प्रत्यागम CONVSPEC_UNSIGNED_INT;
	पक्ष 'e':
	पक्ष 'E':
	पक्ष 'f':
	पक्ष 'F':
	पक्ष 'g':
	पक्ष 'G':
	पक्ष 'a':
	पक्ष 'A':
		प्रत्यागम CONVSPEC_DOUBLE;
	पक्ष 's':
		यदि (पूर्व_ch == 'l' || पूर्व_ch == 'q' || पूर्व_ch == 'L') {
			// wchar not supported
			प्रत्यागम CONVSPEC_ERROR;
		}
		प्रत्यागम CONVSPEC_CHAR_PTR;
	पक्ष 'p':
		प्रत्यागम CONVSPEC_VOID_PTR;
	पक्ष '$': // positional parameters
	पक्ष 'n':
	पक्ष 'S':
		प्रत्यागम CONVSPEC_ERROR;
	स्वभाव:
		प्रत्यागम CONVSPEC_NONE;
	}
}

गणना class Dynamic_Int_गणनम् : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
संरचना प्राप्तम्_Format_Spec
{
	// The start offset of the conversion spec in the format string.
	पूर्णाङ्क start;
	// The end offset of the conversion spec in the format string.
	पूर्णाङ्क अन्तः;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints;
};

// Ensure there is no format spec.
constexpr शून्य ensure_no_format_spec(std::string_view fmt, पूर्णाङ्क सूचकांक, bool *has_escapes)
{
	पूर्णाङ्क fmt_आकार = static_cast<पूर्णाङ्क>(fmt.आकार());
	// A trailing '%' is not a format spec.
	यावत् (सूचकांक + 1 < fmt_आकार) {
		यदि (fmt[सूचकांक] != '%') {
			++सूचकांक;
			अनुवर्त;
		}
		यदि (fmt[सूचकांक + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		सूचकांक += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr प्राप्तम्_Format_Spec find_अग्र_format_spec(std::string_view fmt, पूर्णाङ्क fmt_start, bool *has_escapes)
{
	पूर्णाङ्क सूचकांक = fmt_start;
	पूर्णाङ्क fmt_आकार = static_cast<पूर्णाङ्क>(fmt.आकार());
	यावत् (सूचकांक < fmt_आकार) {
		यदि (fmt[सूचकांक] != '%') {
			++सूचकांक;
			अनुवर्त;
		}
		पूर्णाङ्क p = सूचकांक + 1;
		uint8_t अङ्कः_dynamic_ints = 0;
		यावत् (true) {
			यदि (p == fmt_आकार) {
				प्रत्यागम {0, 0, CONVSPEC_NONE, Dynamic_Int_गणनम्::NONE};
			}
			यदि (fmt[p] == '%') {
				*has_escapes = true;
				सूचकांक = p + 1;
				भङ्ग;
			}
			यदि (fmt[p] == '*') {
				यदि (अङ्कः_dynamic_ints >= 2) {
					प्रत्यागम {0, 0, CONVSPEC_ERROR, Dynamic_Int_गणनम्::NONE};
				}
				++अङ्कः_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			यदि (spec != CONVSPEC_NONE) {
				प्रत्यागम {सूचकांक, p + 1, spec, static_cast<Dynamic_Int_गणनम्>(अङ्कः_dynamic_ints)};
			}
			++p;
		}
	}
	प्रत्यागम {0, 0, CONVSPEC_NONE, Dynamic_Int_गणनम्::NONE};
}

प्रतिरूपम् <typename... तर्काः_3>
constexpr typename std::enable_if<परिमाण...(तर्काः_3) == 0>::type
check_format(std::string_view fmt, पूर्णाङ्क fmt_start, bool *has_escapes, प्राप्तम्_Format_Spec*, Dynamic_Int_गणनम्)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

प्रतिरूपम् <class T>
स्थिर स्वचालित has_name_member_imp(पूर्णाङ्क)
	-> decltype(static_cast<ध्रुव IdString>(std::declval<T>().name), std::true_type{});

प्रतिरूपम् <class T>
स्थिर स्वचालित has_name_member_imp(दीर्घ)
	-> std::false_type;

प्रतिरूपम् <class T>
संरचना has_name_member : decltype(has_name_member_imp<T>(0)){};

प्रतिरूपम् <class T>
स्थिर स्वचालित ptr_has_name_member_imp(पूर्णाङ्क)
	-> decltype(static_cast<ध्रुव IdString>(std::declval<T>()->name), std::true_type{});

प्रतिरूपम् <class T>
स्थिर स्वचालित ptr_has_name_member_imp(दीर्घ)
	-> std::false_type;

प्रतिरूपम् <class T>
संरचना ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
प्रतिरूपम् <typename Arg, typename... तर्काः_3>
constexpr शून्य check_format(std::string_view fmt, पूर्णाङ्क fmt_start, bool *has_escapes, प्राप्तम्_Format_Spec* specs,
        Dynamic_Int_गणनम् int_तर्काः_consumed)
{
	प्राप्तम्_Format_Spec प्राप्तम् = find_अग्र_format_spec(fmt, fmt_start, has_escapes);
	यदि (प्राप्तम्.अङ्कः_dynamic_ints > int_तर्काः_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		यदि constexpr (!std::is_convertible_v<Arg, पूर्णाङ्क>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<तर्काः_3...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_गणनम्>(static_cast<uint8_t>(int_तर्काः_consumed) + 1));
		प्रत्यागम;
	}
	विकल्प (प्राप्तम्.spec) {
	पक्ष CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		भङ्ग;
	पक्ष CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		भङ्ग;
	पक्ष CONVSPEC_SIGNED_INT:
		यदि constexpr (!std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = प्राप्तम्;
		भङ्ग;
	पक्ष CONVSPEC_UNSIGNED_INT:
		यदि constexpr (!std::is_convertible_v<Arg, अचिह्नित दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = प्राप्तम्;
		भङ्ग;
	पक्ष CONVSPEC_DOUBLE:
		यदि constexpr (!std::is_convertible_v<Arg, द्विगुण>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = प्राप्तम्;
		भङ्ग;
	पक्ष CONVSPEC_CHAR_PTR:
		यदि constexpr (!std::is_convertible_v<Arg, ध्रुव वर्ण *> &&
		        !std::is_convertible_v<Arg, ध्रुव std::string &> &&
			!std::is_convertible_v<Arg, ध्रुव std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = प्राप्तम्;
		भङ्ग;
	पक्ष CONVSPEC_VOID_PTR:
		यदि constexpr (!std::is_convertible_v<Arg, ध्रुव शून्य *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = प्राप्तम्;
		भङ्ग;
	}
	check_format<तर्काः_3...>(fmt, प्राप्तम्.अन्तः, has_escapes, specs + 1, Dynamic_Int_गणनम्::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
शून्य format_emit_long_long(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
शून्य format_emit_unsigned_long_long(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, अचिह्नित दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
शून्य format_emit_double(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, द्विगुण arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
शून्य format_emit_char_ptr(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव वर्ण *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
शून्य format_emit_string(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
शून्य format_emit_string_view(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
शून्य format_emit_void_ptr(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव शून्य *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
प्रतिरूपम् <typename Arg>
अन्तःस्थ शून्य format_emit_one(std::string &परिणाम, std::string_view fmt, ध्रुव प्राप्तम्_Format_Spec &ffspec,
	पूर्णाङ्क *dynamic_ints, ध्रुव Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.अन्तः - ffspec.start);
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints = ffspec.अङ्कः_dynamic_ints;
	विकल्प (ffspec.spec) {
	पक्ष CONVSPEC_SIGNED_INT:
		यदि constexpr (std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			दीर्घ दीर्घ s = arg;
			format_emit_long_long(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_UNSIGNED_INT:
		यदि constexpr (std::is_convertible_v<Arg, अचिह्नित दीर्घ दीर्घ>) {
			अचिह्नित दीर्घ दीर्घ s = arg;
			format_emit_unsigned_long_long(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_DOUBLE:
		यदि constexpr (std::is_convertible_v<Arg, द्विगुण>) {
			द्विगुण s = arg;
			format_emit_double(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_CHAR_PTR:
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव वर्ण *>) {
			ध्रुव वर्ण *s = arg;
			format_emit_char_ptr(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव std::string &>) {
			ध्रुव std::string &s = arg;
			format_emit_string(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव std::string_view &>) {
			ध्रुव std::string_view &s = arg;
			format_emit_string_view(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (has_name_member<Arg>()) {
			ध्रुव std::string &s = arg.name.unescape();
			format_emit_string(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (ptr_has_name_member<Arg>()) {
			ध्रुव std::string &s = arg->name.unescape();
			format_emit_string(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_VOID_PTR:
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव शून्य *>) {
			ध्रुव शून्य *s = arg;
			format_emit_void_ptr(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
शून्य format_emit_unescaped(std::string &परिणाम, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

अन्तःस्थ शून्य format_emit(std::string &परिणाम, std::string_view fmt, पूर्णाङ्क fmt_start,
	bool has_escapes, ध्रुव प्राप्तम्_Format_Spec*, पूर्णाङ्क*, Dynamic_Int_गणनम्)
{
	fmt = fmt.substr(fmt_start);
	यदि (has_escapes) {
		format_emit_unescaped(परिणाम, fmt);
	} अन्यथा {
		परिणाम += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
प्रतिरूपम् <typename Arg, typename... तर्काः_3>
अन्तःस्थ शून्य format_emit(std::string &परिणाम, std::string_view fmt, पूर्णाङ्क fmt_start, bool has_escapes,
	ध्रुव प्राप्तम्_Format_Spec* specs, पूर्णाङ्क *dynamic_ints, Dynamic_Int_गणनम् अङ्कः_dynamic_ints,
	ध्रुव Arg &arg, ध्रुव तर्काः_3 &... तर्काः_2)
{
	यदि (specs->अङ्कः_dynamic_ints > अङ्कः_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		यदि constexpr (std::is_convertible_v<Arg, पूर्णाङ्क>) {
			dynamic_ints[static_cast<uint8_t>(अङ्कः_dynamic_ints)] = arg;
		} अन्यथा {
			YOSYS_ABORT("Internal error");
		}
		format_emit(परिणाम, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_गणनम्>(static_cast<uint8_t>(अङ्कः_dynamic_ints) + 1), तर्काः_2...);
		प्रत्यागम;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	यदि (has_escapes) {
		format_emit_unescaped(परिणाम, str);
	} अन्यथा {
		परिणाम += str;
	}
	format_emit_one(परिणाम, fmt, *specs, dynamic_ints, arg);
	format_emit(परिणाम, fmt, specs->अन्तः, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_गणनम्::NONE, तर्काः_2...);
}

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ध्रुव प्राप्तम्_Format_Spec* specs, ध्रुव तर्काः_3 &... तर्काः_2)
{
	std::string परिणाम;
	पूर्णाङ्क dynamic_ints[2] = { 0, 0 };
	format_emit(परिणाम, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_गणनम्::NONE, तर्काः_2...);
	प्रत्यागम परिणाम;
}
प्रतिरूपम् <>
अन्तःस्थ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ध्रुव प्राप्तम्_Format_Spec*)
{
	यदि (!has_escapes) {
		प्रत्यागम std::string(fmt);
	}
	प्रत्यागम unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
प्रतिरूपम् <typename... तर्काः_3>
class FmtString
{
सार्वजनिक:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ध्रुव वर्ण *p) : fmt(p)
	{
		check_format<तर्काः_3...>(fmt, 0, &has_escapes, specs, Dynamic_Int_गणनम्::NONE);
	}
	std::string format(ध्रुव तर्काः_3 &... तर्काः_2)
	{
		प्रत्यागम format_emit_toplevel(fmt, has_escapes, specs, तर्काः_2...);
	}
	std::string_view format_string() ध्रुव { प्रत्यागम fmt; }
गुप्त:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	प्राप्तम्_Format_Spec specs[परिमाण...(तर्काः_3) ? परिमाण...(तर्काः_3) : 1] = {};
};

प्रतिरूपम् <typename T> संरचना WrapType { using type = T; };
प्रतिरूपम् <typename T> using Type_तटस्थः = typename WrapType<T>::type;

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ std::string stringf(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	प्रत्यागम fmt.format(तर्काः_2...);
}

पूर्णाङ्क readsome(std::istream &f, वर्ण *s, पूर्णाङ्क n);
std::string अग्र_token(std::string &text, ध्रुव वर्ण *sep = " \t\r\n", bool long_वाक्यानि = false);
std::सदिशम्<std::string> विभज_tokens(ध्रुव std::string &text, ध्रुव वर्ण *sep = " \t\r\n");
bool patmatch(ध्रुव वर्ण *आदर्शः, ध्रुव वर्ण *string);
#यदि !defined(YOSYS_DISABLE_SPAWN)
पूर्णाङ्क सततम्_command(ध्रुव std::string &command, std::function<शून्य(ध्रुव std::string&)> process_पङ्क्तिः = std::function<शून्य(ध्रुव std::string&)>());
#यदि_अन्तः
std::string get_आधारः_tmpdir();
std::string make_अस्थायी_file(std::string template_str = get_आधारः_tmpdir() + "/yosys_XXXXXX");
std::string make_अस्थायी_dir(std::string template_str = get_आधारः_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ध्रुव std::string& filename, bool is_exec = false);
bool check_directory_exists(ध्रुव std::string& dirname, bool is_exec = false);
bool is_absolute_मार्गः(std::string filename);
शून्य remove_directory(std::string dirname);
bool create_directory(ध्रुव std::string& dirname);
bool make_वैयक्तिकम्_directory(ध्रुव std::string& dirname);
std::string make_user_tmpdir(ध्रुव std::string& name);
std::string escape_filename_spaces(ध्रुव std::string& filename);
शून्य append_globbed(std::सदिशम्<std::string>& paths, std::string आदर्शः);
std::string name_from_file_मार्गः(std::string मार्गः_2);
std::string absolute_मार्गः(ध्रुव std::string& मार्गः_2);
std::string parent_from_file_मार्गः(std::string मार्गः_2);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
संरचना Scoped_File_तालकम् {
	Scoped_File_तालकम्(ध्रुव std::string &मार्गः_2);
	~Scoped_File_तालकम्();
	Scoped_File_तालकम्(ध्रुव Scoped_File_तालकम् &) = delete;
	Scoped_File_तालकम् &संक्रियकः=(ध्रुव Scoped_File_तालकम् &) = delete;
गुप्त:
#यदि defined(_WIN32)
	शून्य *handle = nullptr;
#नोचेद्यदि !defined(__wasi__)
	पूर्णाङ्क fd = -1;
#यदि_अन्तः
};

// Current process id (0 on WASI).
अचिह्नित get_process_id();

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः // YOSYS_IO_H
