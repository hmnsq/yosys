#అన్తర్భావ <string>
#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <type_traits>
#అన్తర్భావ "kernel/yosys_common.h"

#నిర్వచితం_కాకపోతే YOSYS_IO_H
#నిర్వచించు YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

నిర్మాణం IdString;

అంతఃవరుస std::string vstringf(స్థిరరాశి అక్షరం *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	స్థిరరాశి పూర్ణాంకం k_Buf_కొలత = 128;
	అక్షరం buf[k_Buf_కొలత];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	పూర్ణాంకం n = vsnprintf(buf, k_Buf_కొలత, fmt, apc);
	va_చివర(apc);
	ఒకవేళ (n < k_Buf_కొలత)
		తిరిగి std::string(buf);

	std::string string;
	అక్షరం *str = ఖాళీ;
#ఒకవేళ defined(_WIN32) || defined(__CYGWIN__)
	పూర్ణాంకం sz = 2 * k_Buf_కొలత, rc;
	అయితే (1) {
		va_copy(apc, ap);
		str = (అక్షరం *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_చివర(apc);
		ఒకవేళ (rc >= 0 && rc < sz)
			ఆపు;
		sz *= 2;
	}
	ఒకవేళ (str != ఖాళీ) {
		string = str;
		విడుదల(str);
	}
	తిరిగి string;
#లేకపోతే
	ఒకవేళ (vasprintf(&str, fmt, ap) < 0)
		str = ఖాళీ;
	ఒకవేళ (str != ఖాళీ) {
		string = str;
		విడుదల(str);
	}
	తిరిగి string;
#అయితే_ముగింపు
}

గణన ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(అక్షరం ch, అక్షరం prev_ch)
{
	ఎంపిక (ch) {
	స్థితి 'd':
	స్థితి 'i':
		తిరిగి CONVSPEC_SIGNED_INT;
	స్థితి 'o':
	స్థితి 'u':
	స్థితి 'x':
	స్థితి 'X':
	స్థితి 'm':
		తిరిగి CONVSPEC_UNSIGNED_INT;
	స్థితి 'c':
		ఒకవేళ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			తిరిగి CONVSPEC_ERROR;
		}
		తిరిగి CONVSPEC_UNSIGNED_INT;
	స్థితి 'e':
	స్థితి 'E':
	స్థితి 'f':
	స్థితి 'F':
	స్థితి 'g':
	స్థితి 'G':
	స్థితి 'a':
	స్థితి 'A':
		తిరిగి CONVSPEC_DOUBLE;
	స్థితి 's':
		ఒకవేళ (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			తిరిగి CONVSPEC_ERROR;
		}
		తిరిగి CONVSPEC_CHAR_PTR;
	స్థితి 'p':
		తిరిగి CONVSPEC_VOID_PTR;
	స్థితి '$': // positional parameters
	స్థితి 'n':
	స్థితి 'S':
		తిరిగి CONVSPEC_ERROR;
	అప్రమేయ:
		తిరిగి CONVSPEC_NONE;
	}
}

గణన class Dynamic_Int_లెక్క : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
నిర్మాణం దొరికింది_Format_Spec
{
	// The start offset of the conversion spec in the format string.
	పూర్ణాంకం ప్రారంభం;
	// The end offset of the conversion spec in the format string.
	పూర్ణాంకం చివర;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_లెక్క అంకె_dynamic_ints;
};

// Ensure there is no format spec.
constexpr శూన్య ensure_no_format_spec(std::string_view fmt, పూర్ణాంకం సూచీ, bool *has_escapes)
{
	పూర్ణాంకం fmt_కొలత = static_cast<పూర్ణాంకం>(fmt.కొలత());
	// A trailing '%' is not a format spec.
	అయితే (సూచీ + 1 < fmt_కొలత) {
		ఒకవేళ (fmt[సూచీ] != '%') {
			++సూచీ;
			కొనసాగు;
		}
		ఒకవేళ (fmt[సూచీ + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		సూచీ += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr దొరికింది_Format_Spec కనుగొను_తదుపరి_format_spec(std::string_view fmt, పూర్ణాంకం fmt_ప్రారంభం, bool *has_escapes)
{
	పూర్ణాంకం సూచీ = fmt_ప్రారంభం;
	పూర్ణాంకం fmt_కొలత = static_cast<పూర్ణాంకం>(fmt.కొలత());
	అయితే (సూచీ < fmt_కొలత) {
		ఒకవేళ (fmt[సూచీ] != '%') {
			++సూచీ;
			కొనసాగు;
		}
		పూర్ణాంకం p = సూచీ + 1;
		uint8_t అంకె_dynamic_ints = 0;
		అయితే (true) {
			ఒకవేళ (p == fmt_కొలత) {
				తిరిగి {0, 0, CONVSPEC_NONE, Dynamic_Int_లెక్క::NONE};
			}
			ఒకవేళ (fmt[p] == '%') {
				*has_escapes = true;
				సూచీ = p + 1;
				ఆపు;
			}
			ఒకవేళ (fmt[p] == '*') {
				ఒకవేళ (అంకె_dynamic_ints >= 2) {
					తిరిగి {0, 0, CONVSPEC_ERROR, Dynamic_Int_లెక్క::NONE};
				}
				++అంకె_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			ఒకవేళ (spec != CONVSPEC_NONE) {
				తిరిగి {సూచీ, p + 1, spec, static_cast<Dynamic_Int_లెక్క>(అంకె_dynamic_ints)};
			}
			++p;
		}
	}
	తిరిగి {0, 0, CONVSPEC_NONE, Dynamic_Int_లెక్క::NONE};
}

మూస <typename... వాదనలు_3>
constexpr typename std::enable_if<పరిమాణం...(వాదనలు_3) == 0>::type
check_format(std::string_view fmt, పూర్ణాంకం fmt_ప్రారంభం, bool *has_escapes, దొరికింది_Format_Spec*, Dynamic_Int_లెక్క)
{
	ensure_no_format_spec(fmt, fmt_ప్రారంభం, has_escapes);
}

మూస <class T>
స్థిర స్వయంచాలక has_name_member_imp(పూర్ణాంకం)
	-> decltype(static_cast<స్థిరరాశి IdString>(std::declval<T>().name), std::true_type{});

మూస <class T>
స్థిర స్వయంచాలక has_name_member_imp(దీర్ఘ)
	-> std::false_type;

మూస <class T>
నిర్మాణం has_name_member : decltype(has_name_member_imp<T>(0)){};

మూస <class T>
స్థిర స్వయంచాలక ptr_has_name_member_imp(పూర్ణాంకం)
	-> decltype(static_cast<స్థిరరాశి IdString>(std::declval<T>()->name), std::true_type{});

మూస <class T>
స్థిర స్వయంచాలక ptr_has_name_member_imp(దీర్ఘ)
	-> std::false_type;

మూస <class T>
నిర్మాణం ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
మూస <typename Arg, typename... వాదనలు_3>
constexpr శూన్య check_format(std::string_view fmt, పూర్ణాంకం fmt_ప్రారంభం, bool *has_escapes, దొరికింది_Format_Spec* specs,
        Dynamic_Int_లెక్క int_వాదనలు_consumed)
{
	దొరికింది_Format_Spec దొరికింది = కనుగొను_తదుపరి_format_spec(fmt, fmt_ప్రారంభం, has_escapes);
	ఒకవేళ (దొరికింది.అంకె_dynamic_ints > int_వాదనలు_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		ఒకవేళ constexpr (!std::is_convertible_v<Arg, పూర్ణాంకం>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<వాదనలు_3...>(fmt, fmt_ప్రారంభం, has_escapes, specs,
			static_cast<Dynamic_Int_లెక్క>(static_cast<uint8_t>(int_వాదనలు_consumed) + 1));
		తిరిగి;
	}
	ఎంపిక (దొరికింది.spec) {
	స్థితి CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		ఆపు;
	స్థితి CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		ఆపు;
	స్థితి CONVSPEC_SIGNED_INT:
		ఒకవేళ constexpr (!std::is_convertible_v<Arg, దీర్ఘ దీర్ఘ>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = దొరికింది;
		ఆపు;
	స్థితి CONVSPEC_UNSIGNED_INT:
		ఒకవేళ constexpr (!std::is_convertible_v<Arg, గుర్తులేని దీర్ఘ దీర్ఘ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = దొరికింది;
		ఆపు;
	స్థితి CONVSPEC_DOUBLE:
		ఒకవేళ constexpr (!std::is_convertible_v<Arg, ద్విగుణ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = దొరికింది;
		ఆపు;
	స్థితి CONVSPEC_CHAR_PTR:
		ఒకవేళ constexpr (!std::is_convertible_v<Arg, స్థిరరాశి అక్షరం *> &&
		        !std::is_convertible_v<Arg, స్థిరరాశి std::string &> &&
			!std::is_convertible_v<Arg, స్థిరరాశి std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = దొరికింది;
		ఆపు;
	స్థితి CONVSPEC_VOID_PTR:
		ఒకవేళ constexpr (!std::is_convertible_v<Arg, స్థిరరాశి శూన్య *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = దొరికింది;
		ఆపు;
	}
	check_format<వాదనలు_3...>(fmt, దొరికింది.చివర, has_escapes, specs + 1, Dynamic_Int_లెక్క::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
శూన్య format_emit_long_long(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, దీర్ఘ దీర్ఘ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
శూన్య format_emit_unsigned_long_long(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, గుర్తులేని దీర్ఘ దీర్ఘ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
శూన్య format_emit_double(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, ద్విగుణ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
శూన్య format_emit_char_ptr(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, స్థిరరాశి అక్షరం *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
శూన్య format_emit_string(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, స్థిరరాశి std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
శూన్య format_emit_string_view(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
శూన్య format_emit_void_ptr(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, స్థిరరాశి శూన్య *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
మూస <typename Arg>
అంతఃవరుస శూన్య format_emit_one(std::string &ఫలితం, std::string_view fmt, స్థిరరాశి దొరికింది_Format_Spec &ffspec,
	పూర్ణాంకం *dynamic_ints, స్థిరరాశి Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.ప్రారంభం, ffspec.చివర - ffspec.ప్రారంభం);
	Dynamic_Int_లెక్క అంకె_dynamic_ints = ffspec.అంకె_dynamic_ints;
	ఎంపిక (ffspec.spec) {
	స్థితి CONVSPEC_SIGNED_INT:
		ఒకవేళ constexpr (std::is_convertible_v<Arg, దీర్ఘ దీర్ఘ>) {
			దీర్ఘ దీర్ఘ s = arg;
			format_emit_long_long(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఆపు;
	స్థితి CONVSPEC_UNSIGNED_INT:
		ఒకవేళ constexpr (std::is_convertible_v<Arg, గుర్తులేని దీర్ఘ దీర్ఘ>) {
			గుర్తులేని దీర్ఘ దీర్ఘ s = arg;
			format_emit_unsigned_long_long(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఆపు;
	స్థితి CONVSPEC_DOUBLE:
		ఒకవేళ constexpr (std::is_convertible_v<Arg, ద్విగుణ>) {
			ద్విగుణ s = arg;
			format_emit_double(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఆపు;
	స్థితి CONVSPEC_CHAR_PTR:
		ఒకవేళ constexpr (std::is_convertible_v<Arg, స్థిరరాశి అక్షరం *>) {
			స్థిరరాశి అక్షరం *s = arg;
			format_emit_char_ptr(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఒకవేళ constexpr (std::is_convertible_v<Arg, స్థిరరాశి std::string &>) {
			స్థిరరాశి std::string &s = arg;
			format_emit_string(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఒకవేళ constexpr (std::is_convertible_v<Arg, స్థిరరాశి std::string_view &>) {
			స్థిరరాశి std::string_view &s = arg;
			format_emit_string_view(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఒకవేళ constexpr (has_name_member<Arg>()) {
			స్థిరరాశి std::string &s = arg.name.unescape();
			format_emit_string(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఒకవేళ constexpr (ptr_has_name_member<Arg>()) {
			స్థిరరాశి std::string &s = arg->name.unescape();
			format_emit_string(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఆపు;
	స్థితి CONVSPEC_VOID_PTR:
		ఒకవేళ constexpr (std::is_convertible_v<Arg, స్థిరరాశి శూన్య *>) {
			స్థిరరాశి శూన్య *s = arg;
			format_emit_void_ptr(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, s);
			తిరిగి;
		}
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
శూన్య format_emit_unescaped(std::string &ఫలితం, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

అంతఃవరుస శూన్య format_emit(std::string &ఫలితం, std::string_view fmt, పూర్ణాంకం fmt_ప్రారంభం,
	bool has_escapes, స్థిరరాశి దొరికింది_Format_Spec*, పూర్ణాంకం*, Dynamic_Int_లెక్క)
{
	fmt = fmt.substr(fmt_ప్రారంభం);
	ఒకవేళ (has_escapes) {
		format_emit_unescaped(ఫలితం, fmt);
	} లేకపోతే {
		ఫలితం += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
మూస <typename Arg, typename... వాదనలు_3>
అంతఃవరుస శూన్య format_emit(std::string &ఫలితం, std::string_view fmt, పూర్ణాంకం fmt_ప్రారంభం, bool has_escapes,
	స్థిరరాశి దొరికింది_Format_Spec* specs, పూర్ణాంకం *dynamic_ints, Dynamic_Int_లెక్క అంకె_dynamic_ints,
	స్థిరరాశి Arg &arg, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	ఒకవేళ (specs->అంకె_dynamic_ints > అంకె_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		ఒకవేళ constexpr (std::is_convertible_v<Arg, పూర్ణాంకం>) {
			dynamic_ints[static_cast<uint8_t>(అంకె_dynamic_ints)] = arg;
		} లేకపోతే {
			YOSYS_ABORT("Internal error");
		}
		format_emit(ఫలితం, fmt, fmt_ప్రారంభం, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_లెక్క>(static_cast<uint8_t>(అంకె_dynamic_ints) + 1), వాదనలు_2...);
		తిరిగి;
	}
	std::string_view str = fmt.substr(fmt_ప్రారంభం, specs->ప్రారంభం - fmt_ప్రారంభం);
	ఒకవేళ (has_escapes) {
		format_emit_unescaped(ఫలితం, str);
	} లేకపోతే {
		ఫలితం += str;
	}
	format_emit_one(ఫలితం, fmt, *specs, dynamic_ints, arg);
	format_emit(ఫలితం, fmt, specs->చివర, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_లెక్క::NONE, వాదనలు_2...);
}

మూస <typename... వాదనలు_3>
అంతఃవరుస std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, స్థిరరాశి దొరికింది_Format_Spec* specs, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	std::string ఫలితం;
	పూర్ణాంకం dynamic_ints[2] = { 0, 0 };
	format_emit(ఫలితం, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_లెక్క::NONE, వాదనలు_2...);
	తిరిగి ఫలితం;
}
మూస <>
అంతఃవరుస std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, స్థిరరాశి దొరికింది_Format_Spec*)
{
	ఒకవేళ (!has_escapes) {
		తిరిగి std::string(fmt);
	}
	తిరిగి unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
మూస <typename... వాదనలు_3>
class FmtString
{
బహిరంగ:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(స్థిరరాశి అక్షరం *p) : fmt(p)
	{
		check_format<వాదనలు_3...>(fmt, 0, &has_escapes, specs, Dynamic_Int_లెక్క::NONE);
	}
	std::string format(స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
	{
		తిరిగి format_emit_toplevel(fmt, has_escapes, specs, వాదనలు_2...);
	}
	std::string_view format_string() స్థిరరాశి { తిరిగి fmt; }
అంతరంగ:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	దొరికింది_Format_Spec specs[పరిమాణం...(వాదనలు_3) ? పరిమాణం...(వాదనలు_3) : 1] = {};
};

మూస <typename T> నిర్మాణం WrapType { using type = T; };
మూస <typename T> using Type_తటస్థం = typename WrapType<T>::type;

మూస <typename... వాదనలు_3>
అంతఃవరుస std::string stringf(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	తిరిగి fmt.format(వాదనలు_2...);
}

పూర్ణాంకం readsome(std::istream &f, అక్షరం *s, పూర్ణాంకం n);
std::string తదుపరి_token(std::string &పాఠ్యం, స్థిరరాశి అక్షరం *sep = " \t\r\n", bool long_అక్షరశ్రేణులు = false);
std::సదిశ<std::string> విభజించు_టోకెన్లు(స్థిరరాశి std::string &పాఠ్యం, స్థిరరాశి అక్షరం *sep = " \t\r\n");
bool patmatch(స్థిరరాశి అక్షరం *నమూనా, స్థిరరాశి అక్షరం *string);
#ఒకవేళ !defined(YOSYS_DISABLE_SPAWN)
పూర్ణాంకం నిరంతరం_command(స్థిరరాశి std::string &command, std::function<శూన్య(స్థిరరాశి std::string&)> process_పంక్తి = std::function<శూన్య(స్థిరరాశి std::string&)>());
#అయితే_ముగింపు
std::string get_పునాది_tmpdir();
std::string make_తాత్కాలిక_file(std::string template_str = get_పునాది_tmpdir() + "/yosys_XXXXXX");
std::string make_తాత్కాలిక_dir(std::string template_str = get_పునాది_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(స్థిరరాశి std::string& filename, bool is_exec = false);
bool check_directory_exists(స్థిరరాశి std::string& dirname, bool is_exec = false);
bool is_absolute_మార్గం(std::string filename);
శూన్య remove_directory(std::string dirname);
bool create_directory(స్థిరరాశి std::string& dirname);
bool make_వ్యక్తిగత_directory(స్థిరరాశి std::string& dirname);
std::string make_user_tmpdir(స్థిరరాశి std::string& name);
std::string escape_filename_spaces(స్థిరరాశి std::string& filename);
శూన్య append_globbed(std::సదిశ<std::string>& paths, std::string నమూనా);
std::string name_from_file_మార్గం(std::string మార్గం_2);
std::string absolute_మార్గం(స్థిరరాశి std::string& మార్గం_2);
std::string మాతృ_from_file_మార్గం(std::string మార్గం_2);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
నిర్మాణం Scoped_File_తాళం {
	Scoped_File_తాళం(స్థిరరాశి std::string &మార్గం_2);
	~Scoped_File_తాళం();
	Scoped_File_తాళం(స్థిరరాశి Scoped_File_తాళం &) = delete;
	Scoped_File_తాళం &క్రియాకారకం=(స్థిరరాశి Scoped_File_తాళం &) = delete;
అంతరంగ:
#ఒకవేళ defined(_WIN32)
	శూన్య *handle = nullptr;
#లేదావేళ !defined(__wasi__)
	పూర్ణాంకం fd = -1;
#అయితే_ముగింపు
};

// Current process id (0 on WASI).
గుర్తులేని get_process_id();

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు // YOSYS_IO_H
