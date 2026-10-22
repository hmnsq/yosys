#ضمن <string>
#ضمن <stdarg.h>
#ضمن <type_traits>
#ضمن "kernel/yosys_common.h"

#جے_بنا_تعریف YOSYS_IO_H
#تعریف_کرو YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

بنية IdString;

مضمن std::string vstringf(ثابت حرف *fmt, va_list ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ثابت صحيح k_Buf_ناپ = 128;
	حرف buf[k_Buf_ناپ];
	buf[0] = '\0';
	va_list apc;
	va_copy(apc, ap);
	صحيح n = vsnprintf(buf, k_Buf_ناپ, fmt, apc);
	va_سرا(apc);
	إذا (n < k_Buf_ناپ)
		أرجع std::string(buf);

	std::string string;
	حرف *str = عدم;
#إذا defined(_WIN32) || defined(__CYGWIN__)
	صحيح sz = 2 * k_Buf_ناپ, rc;
	طالما (1) {
		va_copy(apc, ap);
		str = (حرف *)realloc(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		va_سرا(apc);
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
#جے_انت
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

تعداد class Dynamic_Int_گنتی : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
بنية لبھیا_Format_Spec
{
	// The start offset of the conversion spec in the format string.
	صحيح شروع;
	// The end offset of the conversion spec in the format string.
	صحيح سرا;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_گنتی عدد_dynamic_ints;
};

// Ensure there is no format spec.
constexpr فراغ ensure_no_format_spec(std::string_view fmt, صحيح اشاریہ, bool *has_escapes)
{
	صحيح fmt_ناپ = static_cast<صحيح>(fmt.ناپ());
	// A trailing '%' is not a format spec.
	طالما (اشاریہ + 1 < fmt_ناپ) {
		إذا (fmt[اشاریہ] != '%') {
			++اشاریہ;
			استمر;
		}
		إذا (fmt[اشاریہ + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		اشاریہ += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr لبھیا_Format_Spec لبھو_اگلا_format_spec(std::string_view fmt, صحيح fmt_شروع, bool *has_escapes)
{
	صحيح اشاریہ = fmt_شروع;
	صحيح fmt_ناپ = static_cast<صحيح>(fmt.ناپ());
	طالما (اشاریہ < fmt_ناپ) {
		إذا (fmt[اشاریہ] != '%') {
			++اشاریہ;
			استمر;
		}
		صحيح p = اشاریہ + 1;
		uint8_t عدد_dynamic_ints = 0;
		طالما (true) {
			إذا (p == fmt_ناپ) {
				أرجع {0, 0, CONVSPEC_NONE, Dynamic_Int_گنتی::NONE};
			}
			إذا (fmt[p] == '%') {
				*has_escapes = true;
				اشاریہ = p + 1;
				اكسر;
			}
			إذا (fmt[p] == '*') {
				إذا (عدد_dynamic_ints >= 2) {
					أرجع {0, 0, CONVSPEC_ERROR, Dynamic_Int_گنتی::NONE};
				}
				++عدد_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			إذا (spec != CONVSPEC_NONE) {
				أرجع {اشاریہ, p + 1, spec, static_cast<Dynamic_Int_گنتی>(عدد_dynamic_ints)};
			}
			++p;
		}
	}
	أرجع {0, 0, CONVSPEC_NONE, Dynamic_Int_گنتی::NONE};
}

سانچا <typename... دلیلاں_3>
constexpr typename std::enable_if<حجم...(دلیلاں_3) == 0>::type
check_format(std::string_view fmt, صحيح fmt_شروع, bool *has_escapes, لبھیا_Format_Spec*, Dynamic_Int_گنتی)
{
	ensure_no_format_spec(fmt, fmt_شروع, has_escapes);
}

سانچا <class T>
ساكن خودکار has_name_member_imp(صحيح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>().name), std::true_type{});

سانچا <class T>
ساكن خودکار has_name_member_imp(لما)
	-> std::false_type;

سانچا <class T>
بنية has_name_member : decltype(has_name_member_imp<T>(0)){};

سانچا <class T>
ساكن خودکار ptr_has_name_member_imp(صحيح)
	-> decltype(static_cast<ثابت IdString>(std::declval<T>()->name), std::true_type{});

سانچا <class T>
ساكن خودکار ptr_has_name_member_imp(لما)
	-> std::false_type;

سانچا <class T>
بنية ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
سانچا <typename Arg, typename... دلیلاں_3>
constexpr فراغ check_format(std::string_view fmt, صحيح fmt_شروع, bool *has_escapes, لبھیا_Format_Spec* specs,
        Dynamic_Int_گنتی int_دلیلاں_consumed)
{
	لبھیا_Format_Spec لبھیا = لبھو_اگلا_format_spec(fmt, fmt_شروع, has_escapes);
	إذا (لبھیا.عدد_dynamic_ints > int_دلیلاں_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		إذا constexpr (!std::is_convertible_v<Arg, صحيح>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<دلیلاں_3...>(fmt, fmt_شروع, has_escapes, specs,
			static_cast<Dynamic_Int_گنتی>(static_cast<uint8_t>(int_دلیلاں_consumed) + 1));
		أرجع;
	}
	بدل (لبھیا.spec) {
	حالة CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		اكسر;
	حالة CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		اكسر;
	حالة CONVSPEC_SIGNED_INT:
		إذا constexpr (!std::is_convertible_v<Arg, لما لما>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = لبھیا;
		اكسر;
	حالة CONVSPEC_UNSIGNED_INT:
		إذا constexpr (!std::is_convertible_v<Arg, بنانشانی لما لما>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = لبھیا;
		اكسر;
	حالة CONVSPEC_DOUBLE:
		إذا constexpr (!std::is_convertible_v<Arg, مزدوج>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = لبھیا;
		اكسر;
	حالة CONVSPEC_CHAR_PTR:
		إذا constexpr (!std::is_convertible_v<Arg, ثابت حرف *> &&
		        !std::is_convertible_v<Arg, ثابت std::string &> &&
			!std::is_convertible_v<Arg, ثابت std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = لبھیا;
		اكسر;
	حالة CONVSPEC_VOID_PTR:
		إذا constexpr (!std::is_convertible_v<Arg, ثابت فراغ *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = لبھیا;
		اكسر;
	}
	check_format<دلیلاں_3...>(fmt, لبھیا.سرا, has_escapes, specs + 1, Dynamic_Int_گنتی::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
فراغ format_emit_long_long(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, لما لما arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
فراغ format_emit_unsigned_long_long(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, بنانشانی لما لما arg);

// Emit the string representation of `arg` that has been converted to a `double'.
فراغ format_emit_double(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, مزدوج arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
فراغ format_emit_char_ptr(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت حرف *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
فراغ format_emit_string(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
فراغ format_emit_string_view(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
فراغ format_emit_void_ptr(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت فراغ *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
سانچا <typename Arg>
مضمن فراغ format_emit_one(std::string &نتیجہ, std::string_view fmt, ثابت لبھیا_Format_Spec &ffspec,
	صحيح *dynamic_ints, ثابت Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.شروع, ffspec.سرا - ffspec.شروع);
	Dynamic_Int_گنتی عدد_dynamic_ints = ffspec.عدد_dynamic_ints;
	بدل (ffspec.spec) {
	حالة CONVSPEC_SIGNED_INT:
		إذا constexpr (std::is_convertible_v<Arg, لما لما>) {
			لما لما s = arg;
			format_emit_long_long(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_UNSIGNED_INT:
		إذا constexpr (std::is_convertible_v<Arg, بنانشانی لما لما>) {
			بنانشانی لما لما s = arg;
			format_emit_unsigned_long_long(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_DOUBLE:
		إذا constexpr (std::is_convertible_v<Arg, مزدوج>) {
			مزدوج s = arg;
			format_emit_double(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_CHAR_PTR:
		إذا constexpr (std::is_convertible_v<Arg, ثابت حرف *>) {
			ثابت حرف *s = arg;
			format_emit_char_ptr(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (std::is_convertible_v<Arg, ثابت std::string &>) {
			ثابت std::string &s = arg;
			format_emit_string(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (std::is_convertible_v<Arg, ثابت std::string_view &>) {
			ثابت std::string_view &s = arg;
			format_emit_string_view(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (has_name_member<Arg>()) {
			ثابت std::string &s = arg.name.unescape();
			format_emit_string(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		إذا constexpr (ptr_has_name_member<Arg>()) {
			ثابت std::string &s = arg->name.unescape();
			format_emit_string(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
			أرجع;
		}
		اكسر;
	حالة CONVSPEC_VOID_PTR:
		إذا constexpr (std::is_convertible_v<Arg, ثابت فراغ *>) {
			ثابت فراغ *s = arg;
			format_emit_void_ptr(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, s);
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
فراغ format_emit_unescaped(std::string &نتیجہ, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

مضمن فراغ format_emit(std::string &نتیجہ, std::string_view fmt, صحيح fmt_شروع,
	bool has_escapes, ثابت لبھیا_Format_Spec*, صحيح*, Dynamic_Int_گنتی)
{
	fmt = fmt.substr(fmt_شروع);
	إذا (has_escapes) {
		format_emit_unescaped(نتیجہ, fmt);
	} وإلا {
		نتیجہ += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
سانچا <typename Arg, typename... دلیلاں_3>
مضمن فراغ format_emit(std::string &نتیجہ, std::string_view fmt, صحيح fmt_شروع, bool has_escapes,
	ثابت لبھیا_Format_Spec* specs, صحيح *dynamic_ints, Dynamic_Int_گنتی عدد_dynamic_ints,
	ثابت Arg &arg, ثابت دلیلاں_3 &... دلیلاں_2)
{
	إذا (specs->عدد_dynamic_ints > عدد_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		إذا constexpr (std::is_convertible_v<Arg, صحيح>) {
			dynamic_ints[static_cast<uint8_t>(عدد_dynamic_ints)] = arg;
		} وإلا {
			YOSYS_ABORT("Internal error");
		}
		format_emit(نتیجہ, fmt, fmt_شروع, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_گنتی>(static_cast<uint8_t>(عدد_dynamic_ints) + 1), دلیلاں_2...);
		أرجع;
	}
	std::string_view str = fmt.substr(fmt_شروع, specs->شروع - fmt_شروع);
	إذا (has_escapes) {
		format_emit_unescaped(نتیجہ, str);
	} وإلا {
		نتیجہ += str;
	}
	format_emit_one(نتیجہ, fmt, *specs, dynamic_ints, arg);
	format_emit(نتیجہ, fmt, specs->سرا, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_گنتی::NONE, دلیلاں_2...);
}

سانچا <typename... دلیلاں_3>
مضمن std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت لبھیا_Format_Spec* specs, ثابت دلیلاں_3 &... دلیلاں_2)
{
	std::string نتیجہ;
	صحيح dynamic_ints[2] = { 0, 0 };
	format_emit(نتیجہ, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_گنتی::NONE, دلیلاں_2...);
	أرجع نتیجہ;
}
سانچا <>
مضمن std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ثابت لبھیا_Format_Spec*)
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
سانچا <typename... دلیلاں_3>
class FmtString
{
عام:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ثابت حرف *p) : fmt(p)
	{
		check_format<دلیلاں_3...>(fmt, 0, &has_escapes, specs, Dynamic_Int_گنتی::NONE);
	}
	std::string format(ثابت دلیلاں_3 &... دلیلاں_2)
	{
		أرجع format_emit_toplevel(fmt, has_escapes, specs, دلیلاں_2...);
	}
	std::string_view format_string() ثابت { أرجع fmt; }
خاص:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	لبھیا_Format_Spec specs[حجم...(دلیلاں_3) ? حجم...(دلیلاں_3) : 1] = {};
};

سانچا <typename T> بنية WrapType { using type = T; };
سانچا <typename T> using Type_پچھان_تت = typename WrapType<T>::type;

سانچا <typename... دلیلاں_3>
مضمن std::string stringf(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	أرجع fmt.format(دلیلاں_2...);
}

صحيح readsome(std::istream &f, حرف *s, صحيح n);
std::string اگلا_token(std::string &متن, ثابت حرف *sep = " \t\r\n", bool long_لڑیاں = false);
std::سمتیہ<std::string> ونڈو_ٹوکن(ثابت std::string &متن, ثابت حرف *sep = " \t\r\n");
bool patmatch(ثابت حرف *نمونہ, ثابت حرف *string);
#إذا !defined(YOSYS_DISABLE_SPAWN)
صحيح لگاتار_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_سطر = std::function<فراغ(ثابت std::string&)>());
#جے_انت
std::string get_ادھار_tmpdir();
std::string make_عارضی_file(std::string template_str = get_ادھار_tmpdir() + "/yosys_XXXXXX");
std::string make_عارضی_dir(std::string template_str = get_ادھار_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ثابت std::string& filename, bool is_exec = false);
bool check_directory_exists(ثابت std::string& dirname, bool is_exec = false);
bool is_absolute_رستہ(std::string filename);
فراغ remove_directory(std::string dirname);
bool create_directory(ثابت std::string& dirname);
bool make_نجی_directory(ثابت std::string& dirname);
std::string make_user_tmpdir(ثابت std::string& name);
std::string escape_filename_spaces(ثابت std::string& filename);
فراغ append_globbed(std::سمتیہ<std::string>& paths, std::string نمونہ);
std::string name_from_file_رستہ(std::string رستہ_2);
std::string absolute_رستہ(ثابت std::string& رستہ_2);
std::string ماپا_from_file_رستہ(std::string رستہ_2);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
بنية Scoped_File_جندرا {
	Scoped_File_جندرا(ثابت std::string &رستہ_2);
	~Scoped_File_جندرا();
	Scoped_File_جندرا(ثابت Scoped_File_جندرا &) = delete;
	Scoped_File_جندرا &عامل=(ثابت Scoped_File_جندرا &) = delete;
خاص:
#إذا defined(_WIN32)
	فراغ *handle = nullptr;
#وإلاإذا !defined(__wasi__)
	صحيح fd = -1;
#جے_انت
};

// Current process id (0 on WASI).
بنانشانی get_process_id();

YOSYS_NAMESPACE_سرا

#جے_انت // YOSYS_IO_H
