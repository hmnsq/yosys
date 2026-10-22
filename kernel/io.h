#અન્તર્ભાવ <string>
#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <type_traits>
#અન્તર્ભાવ "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

સંરચના IdString;

અંતઃરેખ std::string vstringf(અચલ અક્ષર *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	અચલ પૂર્ણાંક kBufSize = 128;
	અક્ષર buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	પૂર્ણાંક n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	જો (n < kBufSize)
		પાછા std::string(buf);

	std::string string;
	અક્ષર *str = ખાલી;
#જો defined(_WIN32) || defined(__CYGWIN__)
	પૂર્ણાંક sz = 2 * kBufSize, rc;
	જ્યારે (1) {
		va_copy(apc, ap);
		str = (અક્ષર *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		જો (rc >= 0 && rc < sz)
			તોડો;
		sz *= 2;
	}
	જો (str != ખાલી) {
		string = str;
		મુક્ત(str);
	}
	પાછા string;
#નહીંતર
	જો (vasprintf(&str, fmt, ap) < 0)
		str = ખાલી;
	જો (str != ખાલી) {
		string = str;
		મુક્ત(str);
	}
	પાછા string;
#endif
}

ગણના ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(અક્ષર ch, અક્ષર prev_ch)
{
	પસંદગી (ch) {
	સ્થિતિ 'd':
	સ્થિતિ 'i':
		પાછા CONVSPEC_SIGNED_INT;
	સ્થિતિ 'o':
	સ્થિતિ 'u':
	સ્થિતિ 'x':
	સ્થિતિ 'X':
	સ્થિતિ 'm':
		પાછા CONVSPEC_UNSIGNED_INT;
	સ્થિતિ 'c':
		જો (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			પાછા CONVSPEC_ERROR;
		}
		પાછા CONVSPEC_UNSIGNED_INT;
	સ્થિતિ 'e':
	સ્થિતિ 'E':
	સ્થિતિ 'f':
	સ્થિતિ 'F':
	સ્થિતિ 'g':
	સ્થિતિ 'G':
	સ્થિતિ 'a':
	સ્થિતિ 'A':
		પાછા CONVSPEC_DOUBLE;
	સ્થિતિ 's':
		જો (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			પાછા CONVSPEC_ERROR;
		}
		પાછા CONVSPEC_CHAR_PTR;
	સ્થિતિ 'p':
		પાછા CONVSPEC_VOID_PTR;
	સ્થિતિ '$': // positional parameters
	સ્થિતિ 'n':
	સ્થિતિ 'S':
		પાછા CONVSPEC_ERROR;
	મૂળભૂત:
		પાછા CONVSPEC_NONE;
	}
}

ગણના class Dynamic_Int_ગણતરી : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
સંરચના FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	પૂર્ણાંક start;
	// The end offset of the conversion spec in the format string.
	પૂર્ણાંક end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_ગણતરી અંક_dynamic_ints;
};

// Ensure there is no format spec.
constexpr શૂન્ય ensure_no_format_spec(std::string_view fmt, પૂર્ણાંક index, bool *has_escapes)
{
	પૂર્ણાંક fmt_size = static_cast<પૂર્ણાંક>(fmt.size());
	// A trailing '%' is not a format spec.
	જ્યારે (index + 1 < fmt_size) {
		જો (fmt[index] != '%') {
			++index;
			ચાલુ;
		}
		જો (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, પૂર્ણાંક fmt_start, bool *has_escapes)
{
	પૂર્ણાંક index = fmt_start;
	પૂર્ણાંક fmt_size = static_cast<પૂર્ણાંક>(fmt.size());
	જ્યારે (index < fmt_size) {
		જો (fmt[index] != '%') {
			++index;
			ચાલુ;
		}
		પૂર્ણાંક p = index + 1;
		uint8_t અંક_dynamic_ints = 0;
		જ્યારે (true) {
			જો (p == fmt_size) {
				પાછા {0, 0, CONVSPEC_NONE, Dynamic_Int_ગણતરી::NONE};
			}
			જો (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				તોડો;
			}
			જો (fmt[p] == '*') {
				જો (અંક_dynamic_ints >= 2) {
					પાછા {0, 0, CONVSPEC_ERROR, Dynamic_Int_ગણતરી::NONE};
				}
				++અંક_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			જો (spec != CONVSPEC_NONE) {
				પાછા {index, p + 1, spec, static_cast<Dynamic_Int_ગણતરી>(અંક_dynamic_ints)};
			}
			++p;
		}
	}
	પાછા {0, 0, CONVSPEC_NONE, Dynamic_Int_ગણતરી::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<કદ...(Args) == 0>::type
check_format(std::string_view fmt, પૂર્ણાંક fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_ગણતરી)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
સ્થિર auto has_name_member_imp(પૂર્ણાંક)
	-> decltype(static_cast<અચલ IdString>(std::declval<T>().name), std::true_type{});

template <class T>
સ્થિર auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
સંરચના has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
સ્થિર auto ptr_has_name_member_imp(પૂર્ણાંક)
	-> decltype(static_cast<અચલ IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
સ્થિર auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
સંરચના ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr શૂન્ય check_format(std::string_view fmt, પૂર્ણાંક fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_ગણતરી int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	જો (found.અંક_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		જો constexpr (!std::is_convertible_v<Arg, પૂર્ણાંક>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_ગણતરી>(static_cast<uint8_t>(int_args_consumed) + 1));
		પાછા;
	}
	પસંદગી (found.spec) {
	સ્થિતિ CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		તોડો;
	સ્થિતિ CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		તોડો;
	સ્થિતિ CONVSPEC_SIGNED_INT:
		જો constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		તોડો;
	સ્થિતિ CONVSPEC_UNSIGNED_INT:
		જો constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		તોડો;
	સ્થિતિ CONVSPEC_DOUBLE:
		જો constexpr (!std::is_convertible_v<Arg, દ્વિગુણ>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		તોડો;
	સ્થિતિ CONVSPEC_CHAR_PTR:
		જો constexpr (!std::is_convertible_v<Arg, અચલ અક્ષર *> &&
		        !std::is_convertible_v<Arg, અચલ std::string &> &&
			!std::is_convertible_v<Arg, અચલ std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		તોડો;
	સ્થિતિ CONVSPEC_VOID_PTR:
		જો constexpr (!std::is_convertible_v<Arg, અચલ શૂન્ય *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		તોડો;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_ગણતરી::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
શૂન્ય format_emit_long_long(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
શૂન્ય format_emit_unsigned_long_long(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
શૂન્ય format_emit_double(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, દ્વિગુણ arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
શૂન્ય format_emit_char_ptr(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, અચલ અક્ષર *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
શૂન્ય format_emit_string(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, અચલ std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
શૂન્ય format_emit_string_view(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
શૂન્ય format_emit_void_ptr(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, અચલ શૂન્ય *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
અંતઃરેખ શૂન્ય format_emit_one(std::string &result, std::string_view fmt, અચલ FoundFormatSpec &ffspec,
	પૂર્ણાંક *dynamic_ints, અચલ Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_ગણતરી અંક_dynamic_ints = ffspec.અંક_dynamic_ints;
	પસંદગી (ffspec.spec) {
	સ્થિતિ CONVSPEC_SIGNED_INT:
		જો constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		તોડો;
	સ્થિતિ CONVSPEC_UNSIGNED_INT:
		જો constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		તોડો;
	સ્થિતિ CONVSPEC_DOUBLE:
		જો constexpr (std::is_convertible_v<Arg, દ્વિગુણ>) {
			દ્વિગુણ s = arg;
			format_emit_double(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		તોડો;
	સ્થિતિ CONVSPEC_CHAR_PTR:
		જો constexpr (std::is_convertible_v<Arg, અચલ અક્ષર *>) {
			અચલ અક્ષર *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		જો constexpr (std::is_convertible_v<Arg, અચલ std::string &>) {
			અચલ std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		જો constexpr (std::is_convertible_v<Arg, અચલ std::string_view &>) {
			અચલ std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		જો constexpr (has_name_member<Arg>()) {
			અચલ std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		જો constexpr (ptr_has_name_member<Arg>()) {
			અચલ std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		તોડો;
	સ્થિતિ CONVSPEC_VOID_PTR:
		જો constexpr (std::is_convertible_v<Arg, અચલ શૂન્ય *>) {
			અચલ શૂન્ય *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, અંક_dynamic_ints, s);
			પાછા;
		}
		તોડો;
	મૂળભૂત:
		તોડો;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
શૂન્ય format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

અંતઃરેખ શૂન્ય format_emit(std::string &result, std::string_view fmt, પૂર્ણાંક fmt_start,
	bool has_escapes, અચલ FoundFormatSpec*, પૂર્ણાંક*, Dynamic_Int_ગણતરી)
{
	fmt = fmt.substr(fmt_start);
	જો (has_escapes) {
		format_emit_unescaped(result, fmt);
	} નહીંતર {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
અંતઃરેખ શૂન્ય format_emit(std::string &result, std::string_view fmt, પૂર્ણાંક fmt_start, bool has_escapes,
	અચલ FoundFormatSpec* specs, પૂર્ણાંક *dynamic_ints, Dynamic_Int_ગણતરી અંક_dynamic_ints,
	અચલ Arg &arg, અચલ Args &... args)
{
	જો (specs->અંક_dynamic_ints > અંક_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		જો constexpr (std::is_convertible_v<Arg, પૂર્ણાંક>) {
			dynamic_ints[static_cast<uint8_t>(અંક_dynamic_ints)] = arg;
		} નહીંતર {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_ગણતરી>(static_cast<uint8_t>(અંક_dynamic_ints) + 1), args...);
		પાછા;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	જો (has_escapes) {
		format_emit_unescaped(result, str);
	} નહીંતર {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_ગણતરી::NONE, args...);
}

template <typename... Args>
અંતઃરેખ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, અચલ FoundFormatSpec* specs, અચલ Args &... args)
{
	std::string result;
	પૂર્ણાંક dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_ગણતરી::NONE, args...);
	પાછા result;
}
template <>
અંતઃરેખ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, અચલ FoundFormatSpec*)
{
	જો (!has_escapes) {
		પાછા std::string(fmt);
	}
	પાછા unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
જાહેર:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(અચલ અક્ષર *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_ગણતરી::NONE);
	}
	std::string format(અચલ Args &... args)
	{
		પાછા format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() અચલ { પાછા fmt; }
ખાનગી:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[કદ...(Args) ? કદ...(Args) : 1] = {};
};

template <typename T> સંરચના WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
અંતઃરેખ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	પાછા fmt.format(args...);
}

પૂર્ણાંક readsome(std::istream &f, અક્ષર *s, પૂર્ણાંક n);
std::string next_token(std::string &text, અચલ અક્ષર *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(અચલ std::string &text, અચલ અક્ષર *sep = " \t\r\n");
bool patmatch(અચલ અક્ષર *pattern, અચલ અક્ષર *string);
#જો !defined(YOSYS_DISABLE_SPAWN)
પૂર્ણાંક run_command(અચલ std::string &command, std::function<શૂન્ય(અચલ std::string&)> process_લીટી = std::function<શૂન્ય(અચલ std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(અચલ std::string& filename, bool is_exec = false);
bool check_directory_exists(અચલ std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
શૂન્ય remove_directory(std::string dirname);
bool create_directory(અચલ std::string& dirname);
bool make_private_directory(અચલ std::string& dirname);
std::string make_user_tmpdir(અચલ std::string& name);
std::string escape_filename_spaces(અચલ std::string& filename);
શૂન્ય append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(અચલ std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
સંરચના ScopedFileLock {
	ScopedFileLock(અચલ std::string &path);
	~ScopedFileLock();
	ScopedFileLock(અચલ ScopedFileLock &) = delete;
	ScopedFileLock &operator=(અચલ ScopedFileLock &) = delete;
ખાનગી:
#જો defined(_WIN32)
	શૂન્ય *handle = nullptr;
#અથવાજો !defined(__wasi__)
	પૂર્ણાંક fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
