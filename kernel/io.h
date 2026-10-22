#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <type_traits>
#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା IdString;

ଧାଡ଼ିଭିତରେ std::string vstringf(ଅଚଳ ଅକ୍ଷର *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା kBufSize = 128;
	ଅକ୍ଷର buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	ଯଦି (n < kBufSize)
		ଫେରନ୍ତୁ std::string(buf);

	std::string string;
	ଅକ୍ଷର *str = ଖାଲି;
#ଯଦି defined(_WIN32) || defined(__CYGWIN__)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz = 2 * kBufSize, rc;
	ଯେତେବେଳେ (1) {
		va_copy(apc, ap);
		str = (ଅକ୍ଷର *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		ଯଦି (rc >= 0 && rc < sz)
			ଭାଙ୍ଗ;
		sz *= 2;
	}
	ଯଦି (str != ଖାଲି) {
		string = str;
		ମୁକ୍ତ(str);
	}
	ଫେରନ୍ତୁ string;
#ଅନ୍ୟଥା
	ଯଦି (vasprintf(&str, fmt, ap) < 0)
		str = ଖାଲି;
	ଯଦି (str != ଖାଲି) {
		string = str;
		ମୁକ୍ତ(str);
	}
	ଫେରନ୍ତୁ string;
#endif
}

ଗଣନା ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(ଅକ୍ଷର ch, ଅକ୍ଷର prev_ch)
{
	ବାଛ (ch) {
	ସ୍ଥିତି 'd':
	ସ୍ଥିତି 'i':
		ଫେରନ୍ତୁ CONVSPEC_SIGNED_INT;
	ସ୍ଥିତି 'o':
	ସ୍ଥିତି 'u':
	ସ୍ଥିତି 'x':
	ସ୍ଥିତି 'X':
	ସ୍ଥିତି 'm':
		ଫେରନ୍ତୁ CONVSPEC_UNSIGNED_INT;
	ସ୍ଥିତି 'c':
		ଯଦି (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ଫେରନ୍ତୁ CONVSPEC_ERROR;
		}
		ଫେରନ୍ତୁ CONVSPEC_UNSIGNED_INT;
	ସ୍ଥିତି 'e':
	ସ୍ଥିତି 'E':
	ସ୍ଥିତି 'f':
	ସ୍ଥିତି 'F':
	ସ୍ଥିତି 'g':
	ସ୍ଥିତି 'G':
	ସ୍ଥିତି 'a':
	ସ୍ଥିତି 'A':
		ଫେରନ୍ତୁ CONVSPEC_DOUBLE;
	ସ୍ଥିତି 's':
		ଯଦି (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			ଫେରନ୍ତୁ CONVSPEC_ERROR;
		}
		ଫେରନ୍ତୁ CONVSPEC_CHAR_PTR;
	ସ୍ଥିତି 'p':
		ଫେରନ୍ତୁ CONVSPEC_VOID_PTR;
	ସ୍ଥିତି '$': // positional parameters
	ସ୍ଥିତି 'n':
	ସ୍ଥିତି 'S':
		ଫେରନ୍ତୁ CONVSPEC_ERROR;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ CONVSPEC_NONE;
	}
}

ଗଣନା class Dynamic_Int_ଗଣନା : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ସଂରଚନା FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା start;
	// The end offset of the conversion spec in the format string.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints;
};

// Ensure there is no format spec.
constexpr ଶୂନ୍ୟ ensure_no_format_spec(std::string_view fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, bool *has_escapes)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_size = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(fmt.size());
	// A trailing '%' is not a format spec.
	ଯେତେବେଳେ (index + 1 < fmt_size) {
		ଯଦି (fmt[index] != '%') {
			++index;
			ଜାରି;
		}
		ଯଦି (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_start, bool *has_escapes)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = fmt_start;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_size = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(fmt.size());
	ଯେତେବେଳେ (index < fmt_size) {
		ଯଦି (fmt[index] != '%') {
			++index;
			ଜାରି;
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା p = index + 1;
		uint8_t ଅଙ୍କ_dynamic_ints = 0;
		ଯେତେବେଳେ (true) {
			ଯଦି (p == fmt_size) {
				ଫେରନ୍ତୁ {0, 0, CONVSPEC_NONE, Dynamic_Int_ଗଣନା::NONE};
			}
			ଯଦି (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				ଭାଙ୍ଗ;
			}
			ଯଦି (fmt[p] == '*') {
				ଯଦି (ଅଙ୍କ_dynamic_ints >= 2) {
					ଫେରନ୍ତୁ {0, 0, CONVSPEC_ERROR, Dynamic_Int_ଗଣନା::NONE};
				}
				++ଅଙ୍କ_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			ଯଦି (spec != CONVSPEC_NONE) {
				ଫେରନ୍ତୁ {index, p + 1, spec, static_cast<Dynamic_Int_ଗଣନା>(ଅଙ୍କ_dynamic_ints)};
			}
			++p;
		}
	}
	ଫେରନ୍ତୁ {0, 0, CONVSPEC_NONE, Dynamic_Int_ଗଣନା::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<ଆକାର...(Args) == 0>::type
check_format(std::string_view fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_ଗଣନା)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
ସ୍ଥିର auto has_name_member_imp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
	-> decltype(static_cast<ଅଚଳ IdString>(std::declval<T>().name), std::true_type{});

template <class T>
ସ୍ଥିର auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
ସଂରଚନା has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
ସ୍ଥିର auto ptr_has_name_member_imp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
	-> decltype(static_cast<ଅଚଳ IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
ସ୍ଥିର auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
ସଂରଚନା ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr ଶୂନ୍ୟ check_format(std::string_view fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_ଗଣନା int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	ଯଦି (found.ଅଙ୍କ_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		ଯଦି constexpr (!std::is_convertible_v<Arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_ଗଣନା>(static_cast<uint8_t>(int_args_consumed) + 1));
		ଫେରନ୍ତୁ;
	}
	ବାଛ (found.spec) {
	ସ୍ଥିତି CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_SIGNED_INT:
		ଯଦି constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_UNSIGNED_INT:
		ଯଦି constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_DOUBLE:
		ଯଦି constexpr (!std::is_convertible_v<Arg, ଦ୍ୱିଗୁଣ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_CHAR_PTR:
		ଯଦି constexpr (!std::is_convertible_v<Arg, ଅଚଳ ଅକ୍ଷର *> &&
		        !std::is_convertible_v<Arg, ଅଚଳ std::string &> &&
			!std::is_convertible_v<Arg, ଅଚଳ std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_VOID_PTR:
		ଯଦି constexpr (!std::is_convertible_v<Arg, ଅଚଳ ଶୂନ୍ୟ *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		ଭାଙ୍ଗ;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_ଗଣନା::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
ଶୂନ୍ୟ format_emit_long_long(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
ଶୂନ୍ୟ format_emit_unsigned_long_long(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ଶୂନ୍ୟ format_emit_double(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଦ୍ୱିଗୁଣ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
ଶୂନ୍ୟ format_emit_char_ptr(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଅଚଳ ଅକ୍ଷର *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
ଶୂନ୍ୟ format_emit_string(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଅଚଳ std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
ଶୂନ୍ୟ format_emit_string_view(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
ଶୂନ୍ୟ format_emit_void_ptr(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଅଚଳ ଶୂନ୍ୟ *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ format_emit_one(std::string &result, std::string_view fmt, ଅଚଳ FoundFormatSpec &ffspec,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints, ଅଚଳ Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints = ffspec.ଅଙ୍କ_dynamic_ints;
	ବାଛ (ffspec.spec) {
	ସ୍ଥିତି CONVSPEC_SIGNED_INT:
		ଯଦି constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_UNSIGNED_INT:
		ଯଦି constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_DOUBLE:
		ଯଦି constexpr (std::is_convertible_v<Arg, ଦ୍ୱିଗୁଣ>) {
			ଦ୍ୱିଗୁଣ s = arg;
			format_emit_double(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_CHAR_PTR:
		ଯଦି constexpr (std::is_convertible_v<Arg, ଅଚଳ ଅକ୍ଷର *>) {
			ଅଚଳ ଅକ୍ଷର *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଯଦି constexpr (std::is_convertible_v<Arg, ଅଚଳ std::string &>) {
			ଅଚଳ std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଯଦି constexpr (std::is_convertible_v<Arg, ଅଚଳ std::string_view &>) {
			ଅଚଳ std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଯଦି constexpr (has_name_member<Arg>()) {
			ଅଚଳ std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଯଦି constexpr (ptr_has_name_member<Arg>()) {
			ଅଚଳ std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CONVSPEC_VOID_PTR:
		ଯଦି constexpr (std::is_convertible_v<Arg, ଅଚଳ ଶୂନ୍ୟ *>) {
			ଅଚଳ ଶୂନ୍ୟ *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, s);
			ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
ଶୂନ୍ୟ format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ format_emit(std::string &result, std::string_view fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_start,
	bool has_escapes, ଅଚଳ FoundFormatSpec*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା*, Dynamic_Int_ଗଣନା)
{
	fmt = fmt.substr(fmt_start);
	ଯଦି (has_escapes) {
		format_emit_unescaped(result, fmt);
	} ଅନ୍ୟଥା {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ format_emit(std::string &result, std::string_view fmt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmt_start, bool has_escapes,
	ଅଚଳ FoundFormatSpec* specs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints, Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints,
	ଅଚଳ Arg &arg, ଅଚଳ Args &... args)
{
	ଯଦି (specs->ଅଙ୍କ_dynamic_ints > ଅଙ୍କ_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		ଯଦି constexpr (std::is_convertible_v<Arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>) {
			dynamic_ints[static_cast<uint8_t>(ଅଙ୍କ_dynamic_ints)] = arg;
		} ଅନ୍ୟଥା {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_ଗଣନା>(static_cast<uint8_t>(ଅଙ୍କ_dynamic_ints) + 1), args...);
		ଫେରନ୍ତୁ;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	ଯଦି (has_escapes) {
		format_emit_unescaped(result, str);
	} ଅନ୍ୟଥା {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_ଗଣନା::NONE, args...);
}

template <typename... Args>
ଧାଡ଼ିଭିତରେ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ଅଚଳ FoundFormatSpec* specs, ଅଚଳ Args &... args)
{
	std::string result;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_ଗଣନା::NONE, args...);
	ଫେରନ୍ତୁ result;
}
template <>
ଧାଡ଼ିଭିତରେ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ଅଚଳ FoundFormatSpec*)
{
	ଯଦି (!has_escapes) {
		ଫେରନ୍ତୁ std::string(fmt);
	}
	ଫେରନ୍ତୁ unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
ସର୍ବସାଧାରଣ:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ଅଚଳ ଅକ୍ଷର *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_ଗଣନା::NONE);
	}
	std::string format(ଅଚଳ Args &... args)
	{
		ଫେରନ୍ତୁ format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ଅଚଳ { ଫେରନ୍ତୁ fmt; }
ବ୍ୟକ୍ତିଗତ:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[ଆକାର...(Args) ? ଆକାର...(Args) : 1] = {};
};

template <typename T> ସଂରଚନା WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
ଧାଡ଼ିଭିତରେ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	ଫେରନ୍ତୁ fmt.format(args...);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readsome(std::istream &f, ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
std::string next_token(std::string &text, ଅଚଳ ଅକ୍ଷର *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ଅଚଳ std::string &text, ଅଚଳ ଅକ୍ଷର *sep = " \t\r\n");
bool patmatch(ଅଚଳ ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *string);
#ଯଦି !defined(YOSYS_DISABLE_SPAWN)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା run_command(ଅଚଳ std::string &command, std::function<ଶୂନ୍ୟ(ଅଚଳ std::string&)> process_ଧାଡ଼ି = std::function<ଶୂନ୍ୟ(ଅଚଳ std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ଅଚଳ std::string& filename, bool is_exec = false);
bool check_directory_exists(ଅଚଳ std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
ଶୂନ୍ୟ remove_directory(std::string dirname);
bool create_directory(ଅଚଳ std::string& dirname);
bool make_private_directory(ଅଚଳ std::string& dirname);
std::string make_user_tmpdir(ଅଚଳ std::string& name);
std::string escape_filename_spaces(ଅଚଳ std::string& filename);
ଶୂନ୍ୟ append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ଅଚଳ std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ସଂରଚନା ScopedFileLock {
	ScopedFileLock(ଅଚଳ std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ଅଚଳ ScopedFileLock &) = delete;
	ScopedFileLock &operator=(ଅଚଳ ScopedFileLock &) = delete;
ବ୍ୟକ୍ତିଗତ:
#ଯଦି defined(_WIN32)
	ଶୂନ୍ୟ *handle = nullptr;
#ନଚେତଯଦି !defined(__wasi__)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
