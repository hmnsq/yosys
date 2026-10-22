#ضمن <string>
#ضمن <stdarg.h>
#ضمن <type_traits>
#ضمن "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

بنية IdString;

مضمن std::string vstringf(ثابت حرف *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ثابت صحيح kBufSize = 128;
	حرف buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	صحيح n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	إذا (n < kBufSize)
		أرجع std::string(buf);

	std::string string;
	حرف *str = عدم;
#إذا defined(_WIN32) || defined(__CYGWIN__)
	صحيح sz = 2 * kBufSize, rc;
	طالما (1) {
		va_copy(apc, ap);
		str = (حرف *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		إذا (rc >= 0 && rc < sz)
			اكسر;
		sz *= 2;
	}
	إذا (str != عدم) {
		string = str;
		حرر(str);
	}
	أرجع string;
#وإلا
	إذا (vasprintf(&str, fmt, ap) < 0)
		str = عدم;
	إذا (str != عدم) {
		string = str;
		حرر(str);
	}
	أرجع string;
#endif
}

تعداد ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(حرف ch, حرف prev_ch)
{
	بدل (ch) {
	حالة 'd':
	حالة 'i':
		أرجع CONVSPEC_SIGNED_INT;
	حالة 'o':
	حالة 'u':
	حالة 'x':
	حالة 'X':
	حالة 'm':
		أرجع CONVSPEC_UNSIGNED_INT;
	حالة 'c':
		إذا (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			أرجع CONVSPEC_ERROR;
		}
		أرجع CONVSPEC_UNSIGNED_INT;
	حالة 'e':
	حالة 'E':
	حالة 'f':
	حالة 'F':
	حالة 'g':
	حالة 'G':
	حالة 'a':
	حالة 'A':
		أرجع CONVSPEC_DOUBLE;
	حالة 's':
		إذا (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			أرجع CONVSPEC_ERROR;
		}
		أرجع CONVSPEC_CHAR_PTR;
	حالة 'p':
		أرجع CONVSPEC_VOID_PTR;
	حالة '$': // positional parameters
	حالة 'n':
	حالة 'S':
		أرجع CONVSPEC_ERROR;
	افتراضي:
		أرجع CONVSPEC_NONE;
	}
}

تعداد class Dynamic_Int_کيراءن : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
بنية FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	صحيح start;
	// The end offset of the conversion spec in the format string.
	صحيح end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_کيراءن اڠک_dynamic_ints;
};

// Ensure there is no format spec.
constexpr فراغ ensure_no_format_spec(std::string_view fmt, صحيح index, bool *has_escapes)
{
	صحيح fmt_size = static_cast<صحيح>(fmt.size());
	// A trailing '%' is not a format spec.
	طالما (index + 1 < fmt_size) {
		إذا (fmt[index] != '%') {
			++index;
			استمر;
		}
		إذا (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, صحيح fmt_start, bool *has_escapes)
{
	صحيح index = fmt_start;
	صحيح fmt_size = static_cast<صحيح>(fmt.size());
	طالما (index < fmt_size) {
		إذا (fmt[index] != '%') {
			++index;
			استمر;
		}
		صحيح p = index + 1;
		uint8_t اڠک_dynamic_ints = 0;
		طالما (true) {
			إذا (p == fmt_size) {
				أرجع {0, 0, CONVSPEC_NONE, Dynamic_Int_کيراءن::NONE};
			}
			إذا (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				اكسر;
			}
			إذا (fmt[p] == '*') {
				إذا (اڠک_dynamic_ints >= 2) {
					أرجع {0, 0, CONVSPEC_ERROR, Dynamic_Int_کيراءن::NONE};
				}
				++اڠک_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			إذا (spec != CONVSPEC_NONE) {
				أرجع {index, p + 1, spec, static_cast<Dynamic_Int_کيراءن>(اڠک_dynamic_ints)};
			}
			++p;
		}
	}
	أرجع {0, 0, CONVSPEC_NONE, Dynamic_Int_کيراءن::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<حجم...(Args) == 0>::type
check_format(std::string_view fmt, صحيح fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_کيراءن)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
ساكن auto has_name_member_imp(صحيح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>().name), std::true_type{});

template <class T>
ساكن auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
بنية has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
ساكن auto ptr_has_name_member_imp(صحيح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
ساكن auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
بنية ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr فراغ check_format(std::string_view fmt, صحيح fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_کيراءن int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	إذا (found.اڠک_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		إذا constexpr (!std::is_convertible_v<Arg, صحيح>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_کيراءن>(static_cast<uint8_t>(int_args_consumed) + 1));
		أرجع;
	}
	بدل (found.spec) {
	حالة CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		اكسر;
	حالة CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		اكسر;
	حالة CONVSPEC_SIGNED_INT:
		إذا constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		اكسر;
	حالة CONVSPEC_UNSIGNED_INT:
		إذا constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		اكسر;
	حالة CONVSPEC_DOUBLE:
		إذا constexpr (!std::is_convertible_v<Arg, مزدوج>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		اكسر;
	حالة CONVSPEC_CHAR_PTR:
		إذا constexpr (!std::is_convertible_v<Arg, ثابت حرف *> &&
		        !std::is_convertible_v<Arg, ثابت std::string &> &&
			!std::is_convertible_v<Arg, ثابت std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		اكسر;
	حالة CONVSPEC_VOID_PTR:
		إذا constexpr (!std::is_convertible_v<Arg, ثابت فراغ *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		اكسر;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_کيراءن::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
فراغ format_emit_long_long(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
فراغ format_emit_unsigned_long_long(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
فراغ format_emit_double(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, مزدوج arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
فراغ format_emit_char_ptr(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, ثابت حرف *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
فراغ format_emit_string(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, ثابت std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
فراغ format_emit_string_view(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
فراغ format_emit_void_ptr(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_کيراءن اڠک_dynamic_ints, ثابت فراغ *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
مضمن فراغ format_emit_one(std::string &result, std::string_view fmt, ثابت FoundFormatSpec &ffspec,
	صحيح *dynamic_ints, ثابت Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_کيراءن اڠک_dynamic_ints = ffspec.اڠک_dynamic_ints;
	بدل (ffspec.spec) {
	حالة CONVSPEC_SIGNED_INT:
		إذا constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_UNSIGNED_INT:
		إذا constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_DOUBLE:
		إذا constexpr (std::is_convertible_v<Arg, مزدوج>) {
			مزدوج s = arg;
			format_emit_double(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_CHAR_PTR:
		إذا constexpr (std::is_convertible_v<Arg, ثابت حرف *>) {
			ثابت حرف *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (std::is_convertible_v<Arg, ثابت std::string &>) {
			ثابت std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (std::is_convertible_v<Arg, ثابت std::string_view &>) {
			ثابت std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (has_name_member<Arg>()) {
			ثابت std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (ptr_has_name_member<Arg>()) {
			ثابت std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_VOID_PTR:
		إذا constexpr (std::is_convertible_v<Arg, ثابت فراغ *>) {
			ثابت فراغ *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, اڠک_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	افتراضي:
		اكسر;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
فراغ format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

مضمن فراغ format_emit(std::string &result, std::string_view fmt, صحيح fmt_start,
	bool has_escapes, ثابت FoundFormatSpec*, صحيح*, Dynamic_Int_کيراءن)
{
	fmt = fmt.substr(fmt_start);
	إذا (has_escapes) {
		format_emit_unescaped(result, fmt);
	} وإلا {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
مضمن فراغ format_emit(std::string &result, std::string_view fmt, صحيح fmt_start, bool has_escapes,
	ثابت FoundFormatSpec* specs, صحيح *dynamic_ints, Dynamic_Int_کيراءن اڠک_dynamic_ints,
	ثابت Arg &arg, ثابت Args &... args)
{
	إذا (specs->اڠک_dynamic_ints > اڠک_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		إذا constexpr (std::is_convertible_v<Arg, صحيح>) {
			dynamic_ints[static_cast<uint8_t>(اڠک_dynamic_ints)] = arg;
		} وإلا {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_کيراءن>(static_cast<uint8_t>(اڠک_dynamic_ints) + 1), args...);
		أرجع;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	إذا (has_escapes) {
		format_emit_unescaped(result, str);
	} وإلا {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_کيراءن::NONE, args...);
}

template <typename... Args>
مضمن std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت FoundFormatSpec* specs, ثابت Args &... args)
{
	std::string result;
	صحيح dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_کيراءن::NONE, args...);
	أرجع result;
}
template <>
مضمن std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت FoundFormatSpec*)
{
	إذا (!has_escapes) {
		أرجع std::string(fmt);
	}
	أرجع unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
عام:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ثابت حرف *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_کيراءن::NONE);
	}
	std::string format(ثابت Args &... args)
	{
		أرجع format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ثابت { أرجع fmt; }
خاص:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[حجم...(Args) ? حجم...(Args) : 1] = {};
};

template <typename T> بنية WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
مضمن std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	أرجع fmt.format(args...);
}

صحيح readsome(std::istream &f, حرف *s, صحيح n);
std::string next_token(std::string &text, ثابت حرف *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ثابت std::string &text, ثابت حرف *sep = " \t\r\n");
bool patmatch(ثابت حرف *pattern, ثابت حرف *string);
#إذا !defined(YOSYS_DISABLE_SPAWN)
صحيح run_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_باريس = std::function<فراغ(ثابت std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ثابت std::string& filename, bool is_exec = false);
bool check_directory_exists(ثابت std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
فراغ remove_directory(std::string dirname);
bool create_directory(ثابت std::string& dirname);
bool make_private_directory(ثابت std::string& dirname);
std::string make_user_tmpdir(ثابت std::string& name);
std::string escape_filename_spaces(ثابت std::string& filename);
فراغ append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ثابت std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
بنية ScopedFileLock {
	ScopedFileLock(ثابت std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ثابت ScopedFileLock &) = delete;
	ScopedFileLock &operator=(ثابت ScopedFileLock &) = delete;
خاص:
#إذا defined(_WIN32)
	فراغ *handle = nullptr;
#وإلاإذا !defined(__wasi__)
	صحيح fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
