#شامل <string>
#شامل <stdarg.h>
#شامل <type_traits>
#شامل "kernel/yosys_common.h"

#اگر_غیر_تعریف_شدہ YOSYS_IO_H
#تعریف_کرو YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

ساخت IdString;

اندرونی std::string vstringf(ثابت حرف *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ثابت صحیح k_Buf_سائز = 128;
	حرف buf[k_Buf_سائز];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	صحیح n = vsnprintf(buf, k_Buf_سائز, fmt, apc);
	va_انتہا(apc);
	اگر (n < k_Buf_سائز)
		واپس std::string(buf);

	std::string string;
	حرف *str = تہی;
#اگر defined(_WIN32) || defined(__CYGWIN__)
	صحیح sz = 2 * k_Buf_سائز, rc;
	جب (1) {
		va_copy(apc, ap);
		str = (حرف *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_انتہا(apc);
		اگر (rc >= 0 && rc < sz)
			توڑو;
		sz *= 2;
	}
	اگر (str != تہی) {
		string = str;
		آزاد(str);
	}
	واپس string;
#ورنہ
	اگر (vasprintf(&str, fmt, ap) < 0)
		str = تہی;
	اگر (str != تہی) {
		string = str;
		آزاد(str);
	}
	واپس string;
#اگر_ختم
}

شمار ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(حرف ch, حرف پچھلا_ch)
{
	انتخاب (ch) {
	حالت 'd':
	حالت 'i':
		واپس CONVSPEC_SIGNED_INT;
	حالت 'o':
	حالت 'u':
	حالت 'x':
	حالت 'X':
	حالت 'm':
		واپس CONVSPEC_UNSIGNED_INT;
	حالت 'c':
		اگر (پچھلا_ch == 'l' || پچھلا_ch == 'q' || پچھلا_ch == 'L') {
			// wchar not supported
			واپس CONVSPEC_ERROR;
		}
		واپس CONVSPEC_UNSIGNED_INT;
	حالت 'e':
	حالت 'E':
	حالت 'f':
	حالت 'F':
	حالت 'g':
	حالت 'G':
	حالت 'a':
	حالت 'A':
		واپس CONVSPEC_DOUBLE;
	حالت 's':
		اگر (پچھلا_ch == 'l' || پچھلا_ch == 'q' || پچھلا_ch == 'L') {
			// wchar not supported
			واپس CONVSPEC_ERROR;
		}
		واپس CONVSPEC_CHAR_PTR;
	حالت 'p':
		واپس CONVSPEC_VOID_PTR;
	حالت '$': // positional parameters
	حالت 'n':
	حالت 'S':
		واپس CONVSPEC_ERROR;
	طےشدہ:
		واپس CONVSPEC_NONE;
	}
}

شمار class Dynamic_Int_گنتی : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
ساخت ملا_Format_Spec
{
	// The start offset of the conversion spec in the format string.
	صحیح آغاز;
	// The end offset of the conversion spec in the format string.
	صحیح انتہا;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_گنتی عدد_dynamic_ints;
};

// Ensure there is no format spec.
constexpr خالی ensure_no_format_spec(std::string_view fmt, صحیح اشاریہ, bool *has_escapes)
{
	صحیح fmt_سائز = static_cast<صحیح>(fmt.سائز());
	// A trailing '%' is not a format spec.
	جب (اشاریہ + 1 < fmt_سائز) {
		اگر (fmt[اشاریہ] != '%') {
			++اشاریہ;
			جاری;
		}
		اگر (fmt[اشاریہ + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		اشاریہ += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr ملا_Format_Spec ڈھونڈو_اگلا_format_spec(std::string_view fmt, صحیح fmt_آغاز, bool *has_escapes)
{
	صحیح اشاریہ = fmt_آغاز;
	صحیح fmt_سائز = static_cast<صحیح>(fmt.سائز());
	جب (اشاریہ < fmt_سائز) {
		اگر (fmt[اشاریہ] != '%') {
			++اشاریہ;
			جاری;
		}
		صحیح p = اشاریہ + 1;
		uint8_t عدد_dynamic_ints = 0;
		جب (true) {
			اگر (p == fmt_سائز) {
				واپس {0, 0, CONVSPEC_NONE, Dynamic_Int_گنتی::NONE};
			}
			اگر (fmt[p] == '%') {
				*has_escapes = true;
				اشاریہ = p + 1;
				توڑو;
			}
			اگر (fmt[p] == '*') {
				اگر (عدد_dynamic_ints >= 2) {
					واپس {0, 0, CONVSPEC_ERROR, Dynamic_Int_گنتی::NONE};
				}
				++عدد_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			اگر (spec != CONVSPEC_NONE) {
				واپس {اشاریہ, p + 1, spec, static_cast<Dynamic_Int_گنتی>(عدد_dynamic_ints)};
			}
			++p;
		}
	}
	واپس {0, 0, CONVSPEC_NONE, Dynamic_Int_گنتی::NONE};
}

سانچہ <typename... دلائل_3>
constexpr typename std::enable_if<حجم...(دلائل_3) == 0>::type
check_format(std::string_view fmt, صحیح fmt_آغاز, bool *has_escapes, ملا_Format_Spec*, Dynamic_Int_گنتی)
{
	ensure_no_format_spec(fmt, fmt_آغاز, has_escapes);
}

سانچہ <class T>
ساکن خودکار has_name_member_imp(صحیح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>().name), std::true_type{});

سانچہ <class T>
ساکن خودکار has_name_member_imp(طویل)
	-> std::false_type;

سانچہ <class T>
ساخت has_name_member : decltype(has_name_member_imp<T>(0)){};

سانچہ <class T>
ساکن خودکار ptr_has_name_member_imp(صحیح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>()->name), std::true_type{});

سانچہ <class T>
ساکن خودکار ptr_has_name_member_imp(طویل)
	-> std::false_type;

سانچہ <class T>
ساخت ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
سانچہ <typename Arg, typename... دلائل_3>
constexpr خالی check_format(std::string_view fmt, صحیح fmt_آغاز, bool *has_escapes, ملا_Format_Spec* specs,
        Dynamic_Int_گنتی int_دلائل_consumed)
{
	ملا_Format_Spec ملا = ڈھونڈو_اگلا_format_spec(fmt, fmt_آغاز, has_escapes);
	اگر (ملا.عدد_dynamic_ints > int_دلائل_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		اگر constexpr (!std::is_convertible_v<Arg, صحیح>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<دلائل_3...>(fmt, fmt_آغاز, has_escapes, specs,
			static_cast<Dynamic_Int_گنتی>(static_cast<uint8_t>(int_دلائل_consumed) + 1));
		واپس;
	}
	انتخاب (ملا.spec) {
	حالت CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		توڑو;
	حالت CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		توڑو;
	حالت CONVSPEC_SIGNED_INT:
		اگر constexpr (!std::is_convertible_v<Arg, طویل طویل>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = ملا;
		توڑو;
	حالت CONVSPEC_UNSIGNED_INT:
		اگر constexpr (!std::is_convertible_v<Arg, غیرعلامتی طویل طویل>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = ملا;
		توڑو;
	حالت CONVSPEC_DOUBLE:
		اگر constexpr (!std::is_convertible_v<Arg, دوہرا>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = ملا;
		توڑو;
	حالت CONVSPEC_CHAR_PTR:
		اگر constexpr (!std::is_convertible_v<Arg, ثابت حرف *> &&
		        !std::is_convertible_v<Arg, ثابت std::string &> &&
			!std::is_convertible_v<Arg, ثابت std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = ملا;
		توڑو;
	حالت CONVSPEC_VOID_PTR:
		اگر constexpr (!std::is_convertible_v<Arg, ثابت خالی *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = ملا;
		توڑو;
	}
	check_format<دلائل_3...>(fmt, ملا.انتہا, has_escapes, specs + 1, Dynamic_Int_گنتی::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
خالی format_emit_long_long(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, طویل طویل arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
خالی format_emit_unsigned_long_long(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, غیرعلامتی طویل طویل arg);

// Emit the string representation of `arg` that has been converted to a `double'.
خالی format_emit_double(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, دوہرا arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
خالی format_emit_char_ptr(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت حرف *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
خالی format_emit_string(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
خالی format_emit_string_view(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
خالی format_emit_void_ptr(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت خالی *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
سانچہ <typename Arg>
اندرونی خالی format_emit_one(std::string &نتیجہ, std::string_view fmt, ثابت ملا_Format_Spec &ffspec,
	صحیح *dynamic_ints, ثابت Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.آغاز, ffspec.انتہا - ffspec.آغاز);
	Dynamic_Int_گنتی عدد_dynamic_ints = ffspec.عدد_dynamic_ints;
	انتخاب (ffspec.spec) {
	حالت CONVSPEC_SIGNED_INT:
		اگر constexpr (std::is_convertible_v<Arg, طویل طویل>) {
			طویل طویل s = arg;
			format_emit_long_long(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		توڑو;
	حالت CONVSPEC_UNSIGNED_INT:
		اگر constexpr (std::is_convertible_v<Arg, غیرعلامتی طویل طویل>) {
			غیرعلامتی طویل طویل s = arg;
			format_emit_unsigned_long_long(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		توڑو;
	حالت CONVSPEC_DOUBLE:
		اگر constexpr (std::is_convertible_v<Arg, دوہرا>) {
			دوہرا s = arg;
			format_emit_double(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		توڑو;
	حالت CONVSPEC_CHAR_PTR:
		اگر constexpr (std::is_convertible_v<Arg, ثابت حرف *>) {
			ثابت حرف *s = arg;
			format_emit_char_ptr(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		اگر constexpr (std::is_convertible_v<Arg, ثابت std::string &>) {
			ثابت std::string &s = arg;
			format_emit_string(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		اگر constexpr (std::is_convertible_v<Arg, ثابت std::string_view &>) {
			ثابت std::string_view &s = arg;
			format_emit_string_view(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		اگر constexpr (has_name_member<Arg>()) {
			ثابت std::string &s = arg.name.unescape();
			format_emit_string(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		اگر constexpr (ptr_has_name_member<Arg>()) {
			ثابت std::string &s = arg->name.unescape();
			format_emit_string(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		توڑو;
	حالت CONVSPEC_VOID_PTR:
		اگر constexpr (std::is_convertible_v<Arg, ثابت خالی *>) {
			ثابت خالی *s = arg;
			format_emit_void_ptr(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			واپس;
		}
		توڑو;
	طےشدہ:
		توڑو;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
خالی format_emit_unescaped(std::string &نتیجہ, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

اندرونی خالی format_emit(std::string &نتیجہ, std::string_view fmt, صحیح fmt_آغاز,
	bool has_escapes, ثابت ملا_Format_Spec*, صحیح*, Dynamic_Int_گنتی)
{
	fmt = fmt.substr(fmt_آغاز);
	اگر (has_escapes) {
		format_emit_unescaped(نتیجہ, fmt);
	} ورنہ {
		نتیجہ += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
سانچہ <typename Arg, typename... دلائل_3>
اندرونی خالی format_emit(std::string &نتیجہ, std::string_view fmt, صحیح fmt_آغاز, bool has_escapes,
	ثابت ملا_Format_Spec* specs, صحیح *dynamic_ints, Dynamic_Int_گنتی عدد_dynamic_ints,
	ثابت Arg &arg, ثابت دلائل_3 &... دلائل_2)
{
	اگر (specs->عدد_dynamic_ints > عدد_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		اگر constexpr (std::is_convertible_v<Arg, صحیح>) {
			dynamic_ints[static_cast<uint8_t>(عدد_dynamic_ints)] = arg;
		} ورنہ {
			YOSYS_ABORT("Internal error");
		}
		format_emit(نتیجہ, fmt, fmt_آغاز, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_گنتی>(static_cast<uint8_t>(عدد_dynamic_ints) + 1), دلائل_2...);
		واپس;
	}
	std::string_view str = fmt.substr(fmt_آغاز, specs->آغاز - fmt_آغاز);
	اگر (has_escapes) {
		format_emit_unescaped(نتیجہ, str);
	} ورنہ {
		نتیجہ += str;
	}
	format_emit_one(نتیجہ, fmt, *specs, dynamic_ints, arg);
	format_emit(نتیجہ, fmt, specs->انتہا, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_گنتی::NONE, دلائل_2...);
}

سانچہ <typename... دلائل_3>
اندرونی std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت ملا_Format_Spec* specs, ثابت دلائل_3 &... دلائل_2)
{
	std::string نتیجہ;
	صحیح dynamic_ints[2] = { 0, 0 };
	format_emit(نتیجہ, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_گنتی::NONE, دلائل_2...);
	واپس نتیجہ;
}
سانچہ <>
اندرونی std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت ملا_Format_Spec*)
{
	اگر (!has_escapes) {
		واپس std::string(fmt);
	}
	واپس unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
سانچہ <typename... دلائل_3>
class FmtString
{
عوامی:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ثابت حرف *p) : fmt(p)
	{
		check_format<دلائل_3...>(fmt, 0, &has_escapes, specs, Dynamic_Int_گنتی::NONE);
	}
	std::string format(ثابت دلائل_3 &... دلائل_2)
	{
		واپس format_emit_toplevel(fmt, has_escapes, specs, دلائل_2...);
	}
	std::string_view format_string() ثابت { واپس fmt; }
نجی:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	ملا_Format_Spec specs[حجم...(دلائل_3) ? حجم...(دلائل_3) : 1] = {};
};

سانچہ <typename T> ساخت WrapType { using type = T; };
سانچہ <typename T> using Type_شناختی_عنصر = typename WrapType<T>::type;

سانچہ <typename... دلائل_3>
اندرونی std::string stringf(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	واپس fmt.format(دلائل_2...);
}

صحیح readsome(std::istream &f, حرف *s, صحیح n);
std::string اگلا_token(std::string &متن, ثابت حرف *sep = " \t\r\n", bool long_لڑیاں = false);
std::سمتیہ<std::string> بانٹو_علامات(ثابت std::string &متن, ثابت حرف *sep = " \t\r\n");
bool patmatch(ثابت حرف *نمونہ, ثابت حرف *string);
#اگر !defined(YOSYS_DISABLE_SPAWN)
صحیح تسلسل_command(ثابت std::string &command, std::function<خالی(ثابت std::string&)> process_سطر = std::function<خالی(ثابت std::string&)>());
#اگر_ختم
std::string get_بنیاد_tmpdir();
std::string make_عارضی_file(std::string template_str = get_بنیاد_tmpdir() + "/yosys_XXXXXX");
std::string make_عارضی_dir(std::string template_str = get_بنیاد_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ثابت std::string& filename, bool is_exec = false);
bool check_directory_exists(ثابت std::string& dirname, bool is_exec = false);
bool is_absolute_راستہ(std::string filename);
خالی remove_directory(std::string dirname);
bool create_directory(ثابت std::string& dirname);
bool make_نجی_directory(ثابت std::string& dirname);
std::string make_user_tmpdir(ثابت std::string& name);
std::string escape_filename_spaces(ثابت std::string& filename);
خالی append_globbed(std::سمتیہ<std::string>& paths, std::string نمونہ);
std::string name_from_file_راستہ(std::string راستہ_2);
std::string absolute_راستہ(ثابت std::string& راستہ_2);
std::string والد_from_file_راستہ(std::string راستہ_2);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
ساخت Scoped_File_تالا {
	Scoped_File_تالا(ثابت std::string &راستہ_2);
	~Scoped_File_تالا();
	Scoped_File_تالا(ثابت Scoped_File_تالا &) = delete;
	Scoped_File_تالا &عامل=(ثابت Scoped_File_تالا &) = delete;
نجی:
#اگر defined(_WIN32)
	خالی *handle = nullptr;
#وگرنہ_اگر !defined(__wasi__)
	صحیح fd = -1;
#اگر_ختم
};

// Current process id (0 on WASI).
غیرعلامتی get_process_id();

YOSYS_NAMESPACE_انتہا

#اگر_ختم // YOSYS_IO_H
