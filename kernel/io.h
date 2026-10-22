#شامل <string>
#شامل <stdarg.h>
#شامل <type_traits>
#شامل "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ساختار IdString;

درونخطی std::string vstringf(ثابت نویسه *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ثابت صحیح kBufSize = 128;
	نویسه buf[kBufSize];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	صحیح n = vsnprintf(buf, kBufSize, fmt, apc);
	va_end(apc);
	اگر (n < kBufSize)
		بازگشت std::string(buf);

	std::string string;
	نویسه *str = پوچ;
#اگر defined(_WIN32) || defined(__CYGWIN__)
	صحیح sz = 2 * kBufSize, rc;
	وقتی (1) {
		va_copy(apc, ap);
		str = (نویسه *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_end(apc);
		اگر (rc >= 0 && rc < sz)
			بشکن;
		sz *= 2;
	}
	اگر (str != پوچ) {
		string = str;
		آزاد(str);
	}
	بازگشت string;
#وگرنه
	اگر (vasprintf(&str, fmt, ap) < 0)
		str = پوچ;
	اگر (str != پوچ) {
		string = str;
		آزاد(str);
	}
	بازگشت string;
#endif
}

شمارش ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(نویسه ch, نویسه prev_ch)
{
	انتخاب (ch) {
	حالت 'd':
	حالت 'i':
		بازگشت CONVSPEC_SIGNED_INT;
	حالت 'o':
	حالت 'u':
	حالت 'x':
	حالت 'X':
	حالت 'm':
		بازگشت CONVSPEC_UNSIGNED_INT;
	حالت 'c':
		اگر (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			بازگشت CONVSPEC_ERROR;
		}
		بازگشت CONVSPEC_UNSIGNED_INT;
	حالت 'e':
	حالت 'E':
	حالت 'f':
	حالت 'F':
	حالت 'g':
	حالت 'G':
	حالت 'a':
	حالت 'A':
		بازگشت CONVSPEC_DOUBLE;
	حالت 's':
		اگر (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			بازگشت CONVSPEC_ERROR;
		}
		بازگشت CONVSPEC_CHAR_PTR;
	حالت 'p':
		بازگشت CONVSPEC_VOID_PTR;
	حالت '$': // positional parameters
	حالت 'n':
	حالت 'S':
		بازگشت CONVSPEC_ERROR;
	پیشفرض:
		بازگشت CONVSPEC_NONE;
	}
}

شمارش class Dynamic_Int_شمار : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ساختار FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	صحیح start;
	// The end offset of the conversion spec in the format string.
	صحیح end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_شمار رقم_dynamic_ints;
};

// Ensure there is no format spec.
constexpr تهی ensure_no_format_spec(std::string_view fmt, صحیح index, bool *has_escapes)
{
	صحیح fmt_size = static_cast<صحیح>(fmt.size());
	// A trailing '%' is not a format spec.
	وقتی (index + 1 < fmt_size) {
		اگر (fmt[index] != '%') {
			++index;
			ادامه;
		}
		اگر (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, صحیح fmt_start, bool *has_escapes)
{
	صحیح index = fmt_start;
	صحیح fmt_size = static_cast<صحیح>(fmt.size());
	وقتی (index < fmt_size) {
		اگر (fmt[index] != '%') {
			++index;
			ادامه;
		}
		صحیح p = index + 1;
		uint8_t رقم_dynamic_ints = 0;
		وقتی (true) {
			اگر (p == fmt_size) {
				بازگشت {0, 0, CONVSPEC_NONE, Dynamic_Int_شمار::NONE};
			}
			اگر (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				بشکن;
			}
			اگر (fmt[p] == '*') {
				اگر (رقم_dynamic_ints >= 2) {
					بازگشت {0, 0, CONVSPEC_ERROR, Dynamic_Int_شمار::NONE};
				}
				++رقم_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			اگر (spec != CONVSPEC_NONE) {
				بازگشت {index, p + 1, spec, static_cast<Dynamic_Int_شمار>(رقم_dynamic_ints)};
			}
			++p;
		}
	}
	بازگشت {0, 0, CONVSPEC_NONE, Dynamic_Int_شمار::NONE};
}

template <typename... Args>
constexpr typename std::enable_if<اندازه...(Args) == 0>::type
check_format(std::string_view fmt, صحیح fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_شمار)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

template <class T>
ایستا auto has_name_member_imp(صحیح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>().name), std::true_type{});

template <class T>
ایستا auto has_name_member_imp(long)
	-> std::false_type;

template <class T>
ساختار has_name_member : decltype(has_name_member_imp<T>(0)){};

template <class T>
ایستا auto ptr_has_name_member_imp(صحیح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>()->name), std::true_type{});

template <class T>
ایستا auto ptr_has_name_member_imp(long)
	-> std::false_type;

template <class T>
ساختار ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
constexpr تهی check_format(std::string_view fmt, صحیح fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_شمار int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	اگر (found.رقم_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		اگر constexpr (!std::is_convertible_v<Arg, صحیح>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_شمار>(static_cast<uint8_t>(int_args_consumed) + 1));
		بازگشت;
	}
	انتخاب (found.spec) {
	حالت CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		بشکن;
	حالت CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		بشکن;
	حالت CONVSPEC_SIGNED_INT:
		اگر constexpr (!std::is_convertible_v<Arg, long long>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		بشکن;
	حالت CONVSPEC_UNSIGNED_INT:
		اگر constexpr (!std::is_convertible_v<Arg, unsigned long long>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		بشکن;
	حالت CONVSPEC_DOUBLE:
		اگر constexpr (!std::is_convertible_v<Arg, دوگانه>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		بشکن;
	حالت CONVSPEC_CHAR_PTR:
		اگر constexpr (!std::is_convertible_v<Arg, ثابت نویسه *> &&
		        !std::is_convertible_v<Arg, ثابت std::string &> &&
			!std::is_convertible_v<Arg, ثابت std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		بشکن;
	حالت CONVSPEC_VOID_PTR:
		اگر constexpr (!std::is_convertible_v<Arg, ثابت تهی *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		بشکن;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_شمار::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
تهی format_emit_long_long(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, long long arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
تهی format_emit_unsigned_long_long(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, unsigned long long arg);

// Emit the string representation of `arg` that has been converted to a `double'.
تهی format_emit_double(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, دوگانه arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
تهی format_emit_char_ptr(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت نویسه *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
تهی format_emit_string(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
تهی format_emit_string_view(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
تهی format_emit_void_ptr(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت تهی *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
template <typename Arg>
درونخطی تهی format_emit_one(std::string &result, std::string_view fmt, ثابت FoundFormatSpec &ffspec,
	صحیح *dynamic_ints, ثابت Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_شمار رقم_dynamic_ints = ffspec.رقم_dynamic_ints;
	انتخاب (ffspec.spec) {
	حالت CONVSPEC_SIGNED_INT:
		اگر constexpr (std::is_convertible_v<Arg, long long>) {
			long long s = arg;
			format_emit_long_long(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		بشکن;
	حالت CONVSPEC_UNSIGNED_INT:
		اگر constexpr (std::is_convertible_v<Arg, unsigned long long>) {
			unsigned long long s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		بشکن;
	حالت CONVSPEC_DOUBLE:
		اگر constexpr (std::is_convertible_v<Arg, دوگانه>) {
			دوگانه s = arg;
			format_emit_double(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		بشکن;
	حالت CONVSPEC_CHAR_PTR:
		اگر constexpr (std::is_convertible_v<Arg, ثابت نویسه *>) {
			ثابت نویسه *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		اگر constexpr (std::is_convertible_v<Arg, ثابت std::string &>) {
			ثابت std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		اگر constexpr (std::is_convertible_v<Arg, ثابت std::string_view &>) {
			ثابت std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		اگر constexpr (has_name_member<Arg>()) {
			ثابت std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		اگر constexpr (ptr_has_name_member<Arg>()) {
			ثابت std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		بشکن;
	حالت CONVSPEC_VOID_PTR:
		اگر constexpr (std::is_convertible_v<Arg, ثابت تهی *>) {
			ثابت تهی *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, رقم_dynamic_ints, s);
			بازگشت;
		}
		بشکن;
	پیشفرض:
		بشکن;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
تهی format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

درونخطی تهی format_emit(std::string &result, std::string_view fmt, صحیح fmt_start,
	bool has_escapes, ثابت FoundFormatSpec*, صحیح*, Dynamic_Int_شمار)
{
	fmt = fmt.substr(fmt_start);
	اگر (has_escapes) {
		format_emit_unescaped(result, fmt);
	} وگرنه {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
template <typename Arg, typename... Args>
درونخطی تهی format_emit(std::string &result, std::string_view fmt, صحیح fmt_start, bool has_escapes,
	ثابت FoundFormatSpec* specs, صحیح *dynamic_ints, Dynamic_Int_شمار رقم_dynamic_ints,
	ثابت Arg &arg, ثابت Args &... args)
{
	اگر (specs->رقم_dynamic_ints > رقم_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		اگر constexpr (std::is_convertible_v<Arg, صحیح>) {
			dynamic_ints[static_cast<uint8_t>(رقم_dynamic_ints)] = arg;
		} وگرنه {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_شمار>(static_cast<uint8_t>(رقم_dynamic_ints) + 1), args...);
		بازگشت;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	اگر (has_escapes) {
		format_emit_unescaped(result, str);
	} وگرنه {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_شمار::NONE, args...);
}

template <typename... Args>
درونخطی std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت FoundFormatSpec* specs, ثابت Args &... args)
{
	std::string result;
	صحیح dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_شمار::NONE, args...);
	بازگشت result;
}
template <>
درونخطی std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت FoundFormatSpec*)
{
	اگر (!has_escapes) {
		بازگشت std::string(fmt);
	}
	بازگشت unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
template <typename... Args>
class FmtString
{
عمومی:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ثابت نویسه *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_شمار::NONE);
	}
	std::string format(ثابت Args &... args)
	{
		بازگشت format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ثابت { بازگشت fmt; }
خصوصی:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[اندازه...(Args) ? اندازه...(Args) : 1] = {};
};

template <typename T> ساختار WrapType { using type = T; };
template <typename T> using TypeIdentity = typename WrapType<T>::type;

template <typename... Args>
درونخطی std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	بازگشت fmt.format(args...);
}

صحیح readsome(std::istream &f, نویسه *s, صحیح n);
std::string next_token(std::string &text, ثابت نویسه *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ثابت std::string &text, ثابت نویسه *sep = " \t\r\n");
bool patmatch(ثابت نویسه *pattern, ثابت نویسه *string);
#اگر !defined(YOSYS_DISABLE_SPAWN)
صحیح run_command(ثابت std::string &command, std::function<تهی(ثابت std::string&)> process_سطر = std::function<تهی(ثابت std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ثابت std::string& filename, bool is_exec = false);
bool check_directory_exists(ثابت std::string& dirname, bool is_exec = false);
bool is_absolute_path(std::string filename);
تهی remove_directory(std::string dirname);
bool create_directory(ثابت std::string& dirname);
bool make_private_directory(ثابت std::string& dirname);
std::string make_user_tmpdir(ثابت std::string& name);
std::string escape_filename_spaces(ثابت std::string& filename);
تهی append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ثابت std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ساختار ScopedFileLock {
	ScopedFileLock(ثابت std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ثابت ScopedFileLock &) = delete;
	ScopedFileLock &operator=(ثابت ScopedFileLock &) = delete;
خصوصی:
#اگر defined(_WIN32)
	تهی *handle = nullptr;
#وگرنهاگر !defined(__wasi__)
	صحیح fd = -1;
#endif
};

// Current process id (0 on WASI).
unsigned get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
