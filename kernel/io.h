#अन्तर्भाव <string>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <type_traits>
#अन्तर्भाव "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

संरचना IdString;

ओळीत std::string vstringf(स्थिरांक अक्षर *fmt, बहु_सूची ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	स्थिरांक पूर्णांक kBufSize = 128;
	अक्षर buf[kBufSize];
	buf[0] = '\0';
	बहु_सूची apc;
	va_copy(apc, ap);
	पूर्णांक n = vsnprintf(buf, kBufSize, fmt, apc);
	बहु_पूर्ण(apc);
	जर (n < kBufSize)
		परत std::string(buf);

	std::string string;
	अक्षर *str = रिक्त;
#जर defined(_WIN32) || defined(__CYGWIN__)
	पूर्णांक sz = 2 * kBufSize, rc;
	पर्यंत (1) {
		va_copy(apc, ap);
		str = (अक्षर *)पुनः_स्मृति(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		बहु_पूर्ण(apc);
		जर (rc >= 0 && rc < sz)
			थांबा;
		sz *= 2;
	}
	जर (str != रिक्त) {
		string = str;
		मुक्त(str);
	}
	परत string;
#अन्यथा
	जर (vasprintf(&str, fmt, ap) < 0)
		str = रिक्त;
	जर (str != रिक्त) {
		string = str;
		मुक्त(str);
	}
	परत string;
#endif
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

constexpr ConversionSpecifier parse_conversion_specifier(अक्षर ch, अक्षर prev_ch)
{
	निवड (ch) {
	स्थिती 'd':
	स्थिती 'i':
		परत CONVSPEC_SIGNED_INT;
	स्थिती 'o':
	स्थिती 'u':
	स्थिती 'x':
	स्थिती 'X':
	स्थिती 'm':
		परत CONVSPEC_UNSIGNED_INT;
	स्थिती 'c':
		जर (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			परत CONVSPEC_ERROR;
		}
		परत CONVSPEC_UNSIGNED_INT;
	स्थिती 'e':
	स्थिती 'E':
	स्थिती 'f':
	स्थिती 'F':
	स्थिती 'g':
	स्थिती 'G':
	स्थिती 'a':
	स्थिती 'A':
		परत CONVSPEC_DOUBLE;
	स्थिती 's':
		जर (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
			// wchar not supported
			परत CONVSPEC_ERROR;
		}
		परत CONVSPEC_CHAR_PTR;
	स्थिती 'p':
		परत CONVSPEC_VOID_PTR;
	स्थिती '$': // positional parameters
	स्थिती 'n':
	स्थिती 'S':
		परत CONVSPEC_ERROR;
	पूर्वनिर्धारित:
		परत CONVSPEC_NONE;
	}
}

गणना class Dynamic_Int_मोजणी : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
संरचना FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	पूर्णांक start;
	// The end offset of the conversion spec in the format string.
	पूर्णांक end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_मोजणी अंक_dynamic_ints;
};

// Ensure there is no format spec.
constexpr शून्य ensure_no_format_spec(std::string_view fmt, पूर्णांक index, bool *has_escapes)
{
	पूर्णांक fmt_size = static_cast<पूर्णांक>(fmt.size());
	// A trailing '%' is not a format spec.
	पर्यंत (index + 1 < fmt_size) {
		जर (fmt[index] != '%') {
			++index;
			सुरूठेवा;
		}
		जर (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, पूर्णांक fmt_start, bool *has_escapes)
{
	पूर्णांक index = fmt_start;
	पूर्णांक fmt_size = static_cast<पूर्णांक>(fmt.size());
	पर्यंत (index < fmt_size) {
		जर (fmt[index] != '%') {
			++index;
			सुरूठेवा;
		}
		पूर्णांक p = index + 1;
		uint8_t अंक_dynamic_ints = 0;
		पर्यंत (true) {
			जर (p == fmt_size) {
				परत {0, 0, CONVSPEC_NONE, Dynamic_Int_मोजणी::NONE};
			}
			जर (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				थांबा;
			}
			जर (fmt[p] == '*') {
				जर (अंक_dynamic_ints >= 2) {
					परत {0, 0, CONVSPEC_ERROR, Dynamic_Int_मोजणी::NONE};
				}
				++अंक_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			जर (spec != CONVSPEC_NONE) {
				परत {index, p + 1, spec, static_cast<Dynamic_Int_मोजणी>(अंक_dynamic_ints)};
			}
			++p;
		}
	}
	परत {0, 0, CONVSPEC_NONE, Dynamic_Int_मोजणी::NONE};
}

ढाँचा <typename... Args>
constexpr typename std::enable_if<आकार...(Args) == 0>::type
check_format(std::string_view fmt, पूर्णांक fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_मोजणी)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

ढाँचा <class T>
स्थिर स्वतः has_name_member_imp(पूर्णांक)
	-> decltype(static_cast<स्थिरांक IdString>(std::declval<T>().name), std::true_type{});

ढाँचा <class T>
स्थिर स्वतः has_name_member_imp(दीर्घ)
	-> std::false_type;

ढाँचा <class T>
संरचना has_name_member : decltype(has_name_member_imp<T>(0)){};

ढाँचा <class T>
स्थिर स्वतः ptr_has_name_member_imp(पूर्णांक)
	-> decltype(static_cast<स्थिरांक IdString>(std::declval<T>()->name), std::true_type{});

ढाँचा <class T>
स्थिर स्वतः ptr_has_name_member_imp(दीर्घ)
	-> std::false_type;

ढाँचा <class T>
संरचना ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
ढाँचा <typename Arg, typename... Args>
constexpr शून्य check_format(std::string_view fmt, पूर्णांक fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_मोजणी int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	जर (found.अंक_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		जर constexpr (!std::is_convertible_v<Arg, पूर्णांक>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_मोजणी>(static_cast<uint8_t>(int_args_consumed) + 1));
		परत;
	}
	निवड (found.spec) {
	स्थिती CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		थांबा;
	स्थिती CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		थांबा;
	स्थिती CONVSPEC_SIGNED_INT:
		जर constexpr (!std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		थांबा;
	स्थिती CONVSPEC_UNSIGNED_INT:
		जर constexpr (!std::is_convertible_v<Arg, अचिन्हित दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		थांबा;
	स्थिती CONVSPEC_DOUBLE:
		जर constexpr (!std::is_convertible_v<Arg, द्विगुण>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		थांबा;
	स्थिती CONVSPEC_CHAR_PTR:
		जर constexpr (!std::is_convertible_v<Arg, स्थिरांक अक्षर *> &&
		        !std::is_convertible_v<Arg, स्थिरांक std::string &> &&
			!std::is_convertible_v<Arg, स्थिरांक std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		थांबा;
	स्थिती CONVSPEC_VOID_PTR:
		जर constexpr (!std::is_convertible_v<Arg, स्थिरांक शून्य *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		थांबा;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_मोजणी::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
शून्य format_emit_long_long(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
शून्य format_emit_unsigned_long_long(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, अचिन्हित दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
शून्य format_emit_double(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, द्विगुण arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
शून्य format_emit_char_ptr(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, स्थिरांक अक्षर *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
शून्य format_emit_string(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, स्थिरांक std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
शून्य format_emit_string_view(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
शून्य format_emit_void_ptr(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, स्थिरांक शून्य *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
ढाँचा <typename Arg>
ओळीत शून्य format_emit_one(std::string &result, std::string_view fmt, स्थिरांक FoundFormatSpec &ffspec,
	पूर्णांक *dynamic_ints, स्थिरांक Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_मोजणी अंक_dynamic_ints = ffspec.अंक_dynamic_ints;
	निवड (ffspec.spec) {
	स्थिती CONVSPEC_SIGNED_INT:
		जर constexpr (std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			दीर्घ दीर्घ s = arg;
			format_emit_long_long(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		थांबा;
	स्थिती CONVSPEC_UNSIGNED_INT:
		जर constexpr (std::is_convertible_v<Arg, अचिन्हित दीर्घ दीर्घ>) {
			अचिन्हित दीर्घ दीर्घ s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		थांबा;
	स्थिती CONVSPEC_DOUBLE:
		जर constexpr (std::is_convertible_v<Arg, द्विगुण>) {
			द्विगुण s = arg;
			format_emit_double(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		थांबा;
	स्थिती CONVSPEC_CHAR_PTR:
		जर constexpr (std::is_convertible_v<Arg, स्थिरांक अक्षर *>) {
			स्थिरांक अक्षर *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		जर constexpr (std::is_convertible_v<Arg, स्थिरांक std::string &>) {
			स्थिरांक std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		जर constexpr (std::is_convertible_v<Arg, स्थिरांक std::string_view &>) {
			स्थिरांक std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		जर constexpr (has_name_member<Arg>()) {
			स्थिरांक std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		जर constexpr (ptr_has_name_member<Arg>()) {
			स्थिरांक std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		थांबा;
	स्थिती CONVSPEC_VOID_PTR:
		जर constexpr (std::is_convertible_v<Arg, स्थिरांक शून्य *>) {
			स्थिरांक शून्य *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			परत;
		}
		थांबा;
	पूर्वनिर्धारित:
		थांबा;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
शून्य format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

ओळीत शून्य format_emit(std::string &result, std::string_view fmt, पूर्णांक fmt_start,
	bool has_escapes, स्थिरांक FoundFormatSpec*, पूर्णांक*, Dynamic_Int_मोजणी)
{
	fmt = fmt.substr(fmt_start);
	जर (has_escapes) {
		format_emit_unescaped(result, fmt);
	} अन्यथा {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
ढाँचा <typename Arg, typename... Args>
ओळीत शून्य format_emit(std::string &result, std::string_view fmt, पूर्णांक fmt_start, bool has_escapes,
	स्थिरांक FoundFormatSpec* specs, पूर्णांक *dynamic_ints, Dynamic_Int_मोजणी अंक_dynamic_ints,
	स्थिरांक Arg &arg, स्थिरांक Args &... args)
{
	जर (specs->अंक_dynamic_ints > अंक_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		जर constexpr (std::is_convertible_v<Arg, पूर्णांक>) {
			dynamic_ints[static_cast<uint8_t>(अंक_dynamic_ints)] = arg;
		} अन्यथा {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_मोजणी>(static_cast<uint8_t>(अंक_dynamic_ints) + 1), args...);
		परत;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	जर (has_escapes) {
		format_emit_unescaped(result, str);
	} अन्यथा {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_मोजणी::NONE, args...);
}

ढाँचा <typename... Args>
ओळीत std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, स्थिरांक FoundFormatSpec* specs, स्थिरांक Args &... args)
{
	std::string result;
	पूर्णांक dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_मोजणी::NONE, args...);
	परत result;
}
ढाँचा <>
ओळीत std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, स्थिरांक FoundFormatSpec*)
{
	जर (!has_escapes) {
		परत std::string(fmt);
	}
	परत unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
ढाँचा <typename... Args>
class FmtString
{
सार्वजनिक:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(स्थिरांक अक्षर *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_मोजणी::NONE);
	}
	std::string format(स्थिरांक Args &... args)
	{
		परत format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() स्थिरांक { परत fmt; }
खाजगी:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[आकार...(Args) ? आकार...(Args) : 1] = {};
};

ढाँचा <typename T> संरचना WrapType { using type = T; };
ढाँचा <typename T> using TypeIdentity = typename WrapType<T>::type;

ढाँचा <typename... Args>
ओळीत std::string stringf(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	परत fmt.format(args...);
}

पूर्णांक readsome(std::istream &f, अक्षर *s, पूर्णांक n);
std::string next_token(std::string &text, स्थिरांक अक्षर *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(स्थिरांक std::string &text, स्थिरांक अक्षर *sep = " \t\r\n");
bool patmatch(स्थिरांक अक्षर *pattern, स्थिरांक अक्षर *string);
#जर !defined(YOSYS_DISABLE_SPAWN)
पूर्णांक run_command(स्थिरांक std::string &command, std::function<शून्य(स्थिरांक std::string&)> process_ओळ = std::function<शून्य(स्थिरांक std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(स्थिरांक std::string& filename, bool is_exec = false);
bool check_directory_exists(स्थिरांक std::string& स_नाम, bool is_exec = false);
bool is_absolute_path(std::string filename);
शून्य remove_directory(std::string स_नाम);
bool create_directory(स्थिरांक std::string& स_नाम);
bool make_private_directory(स्थिरांक std::string& स_नाम);
std::string make_user_tmpdir(स्थिरांक std::string& name);
std::string escape_filename_spaces(स्थिरांक std::string& filename);
शून्य append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(स्थिरांक std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
संरचना ScopedFileLock {
	ScopedFileLock(स्थिरांक std::string &path);
	~ScopedFileLock();
	ScopedFileLock(स्थिरांक ScopedFileLock &) = delete;
	ScopedFileLock &चालक=(स्थिरांक ScopedFileLock &) = delete;
खाजगी:
#जर defined(_WIN32)
	शून्य *handle = nullptr;
#किंवाजर !defined(__wasi__)
	पूर्णांक fd = -1;
#endif
};

// Current process id (0 on WASI).
अचिन्हित get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
