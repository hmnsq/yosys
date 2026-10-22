#समावेश <string>
#समावेश <stdarg.h>
#समावेश <type_traits>
#समावेश "kernel/yosys_common.h"

#अगर_अघोषित YOSYS_IO_H
#घोषणा YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

काष्ठा IdString;

अंतरभूत std::string vstringf(स्थिर अक्षर *fmt, बहु_सूची ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	स्थिर पूर्णांक k_Buf_आकार = 128;
	अक्षर buf[k_Buf_आकार];
	buf[0] = '\0';
	बहु_सूची apc;
	va_copy(apc, ap);
	पूर्णांक n = vsnprintf(buf, k_Buf_आकार, fmt, apc);
	बहु_पूर्ण(apc);
	अगर (n < k_Buf_आकार)
		वापस std::string(buf);

	std::string string;
	अक्षर *str = शून्य;
#अगर defined(_WIN32) || defined(__CYGWIN__)
	पूर्णांक sz = 2 * k_Buf_आकार, rc;
	जबतक (1) {
		va_copy(apc, ap);
		str = (अक्षर *)पुनः_स्मृति(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		बहु_पूर्ण(apc);
		अगर (rc >= 0 && rc < sz)
			अवरोध;
		sz *= 2;
	}
	अगर (str != शून्य) {
		string = str;
		मुक्त(str);
	}
	वापस string;
#अन्यथा
	अगर (vasprintf(&str, fmt, ap) < 0)
		str = शून्य;
	अगर (str != शून्य) {
		string = str;
		मुक्त(str);
	}
	वापस string;
#पूर्ण_अगर
}

क्रमागत ConversionSpecifier : uint8_t
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

constexpr ConversionSpecifier parse_conversion_specifier(अक्षर ch, अक्षर पूर्व_ch)
{
	चयन (ch) {
	हाल 'd':
	हाल 'i':
		वापस CONVSPEC_SIGNED_INT;
	हाल 'o':
	हाल 'u':
	हाल 'x':
	हाल 'X':
	हाल 'm':
		वापस CONVSPEC_UNSIGNED_INT;
	हाल 'c':
		अगर (पूर्व_ch == 'l' || पूर्व_ch == 'q' || पूर्व_ch == 'L') {
			// wchar not supported
			वापस CONVSPEC_ERROR;
		}
		वापस CONVSPEC_UNSIGNED_INT;
	हाल 'e':
	हाल 'E':
	हाल 'f':
	हाल 'F':
	हाल 'g':
	हाल 'G':
	हाल 'a':
	हाल 'A':
		वापस CONVSPEC_DOUBLE;
	हाल 's':
		अगर (पूर्व_ch == 'l' || पूर्व_ch == 'q' || पूर्व_ch == 'L') {
			// wchar not supported
			वापस CONVSPEC_ERROR;
		}
		वापस CONVSPEC_CHAR_PTR;
	हाल 'p':
		वापस CONVSPEC_VOID_PTR;
	हाल '$': // positional parameters
	हाल 'n':
	हाल 'S':
		वापस CONVSPEC_ERROR;
	शेष:
		वापस CONVSPEC_NONE;
	}
}

क्रमागत class Dynamic_Int_गिनती : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
काष्ठा FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	पूर्णांक start;
	// The end offset of the conversion spec in the format string.
	पूर्णांक end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_गिनती अंक_dynamic_ints;
};

// Ensure there is no format spec.
constexpr व्योम ensure_no_format_spec(std::string_view fmt, पूर्णांक सूचकांक, bool *has_escapes)
{
	पूर्णांक fmt_आकार = static_cast<पूर्णांक>(fmt.आकार());
	// A trailing '%' is not a format spec.
	जबतक (सूचकांक + 1 < fmt_आकार) {
		अगर (fmt[सूचकांक] != '%') {
			++सूचकांक;
			जारी;
		}
		अगर (fmt[सूचकांक + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		सूचकांक += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_अग्र_format_spec(std::string_view fmt, पूर्णांक fmt_start, bool *has_escapes)
{
	पूर्णांक सूचकांक = fmt_start;
	पूर्णांक fmt_आकार = static_cast<पूर्णांक>(fmt.आकार());
	जबतक (सूचकांक < fmt_आकार) {
		अगर (fmt[सूचकांक] != '%') {
			++सूचकांक;
			जारी;
		}
		पूर्णांक p = सूचकांक + 1;
		uint8_t अंक_dynamic_ints = 0;
		जबतक (true) {
			अगर (p == fmt_आकार) {
				वापस {0, 0, CONVSPEC_NONE, Dynamic_Int_गिनती::NONE};
			}
			अगर (fmt[p] == '%') {
				*has_escapes = true;
				सूचकांक = p + 1;
				अवरोध;
			}
			अगर (fmt[p] == '*') {
				अगर (अंक_dynamic_ints >= 2) {
					वापस {0, 0, CONVSPEC_ERROR, Dynamic_Int_गिनती::NONE};
				}
				++अंक_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			अगर (spec != CONVSPEC_NONE) {
				वापस {सूचकांक, p + 1, spec, static_cast<Dynamic_Int_गिनती>(अंक_dynamic_ints)};
			}
			++p;
		}
	}
	वापस {0, 0, CONVSPEC_NONE, Dynamic_Int_गिनती::NONE};
}

ढाँचा <typename... Args>
constexpr typename std::enable_if<माप...(Args) == 0>::type
check_format(std::string_view fmt, पूर्णांक fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_गिनती)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

ढाँचा <class T>
अटल स्वतः has_name_member_imp(पूर्णांक)
	-> decltype(static_cast<स्थिर IdString>(std::declval<T>().name), std::true_type{});

ढाँचा <class T>
अटल स्वतः has_name_member_imp(दीर्घ)
	-> std::false_type;

ढाँचा <class T>
काष्ठा has_name_member : decltype(has_name_member_imp<T>(0)){};

ढाँचा <class T>
अटल स्वतः ptr_has_name_member_imp(पूर्णांक)
	-> decltype(static_cast<स्थिर IdString>(std::declval<T>()->name), std::true_type{});

ढाँचा <class T>
अटल स्वतः ptr_has_name_member_imp(दीर्घ)
	-> std::false_type;

ढाँचा <class T>
काष्ठा ptr_has_name_member : decltype(ptr_has_name_member_imp<T>(0)){};

// Check that the format string `fmt.substr(fmt_start)` is valid for the given type arguments.
// Fills `specs` with the FoundFormatSpecs found in the format string.
// `int_args_consumed` is the number of int arguments already consumed to satisfy the
// dynamic width/precision args for the next format conversion specifier.
ढाँचा <typename Arg, typename... Args>
constexpr व्योम check_format(std::string_view fmt, पूर्णांक fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_गिनती int_args_consumed)
{
	FoundFormatSpec found = find_अग्र_format_spec(fmt, fmt_start, has_escapes);
	अगर (found.अंक_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		अगर constexpr (!std::is_convertible_v<Arg, पूर्णांक>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_गिनती>(static_cast<uint8_t>(int_args_consumed) + 1));
		वापस;
	}
	चयन (found.spec) {
	हाल CONVSPEC_NONE:
		YOSYS_ABORT("Expected format conversion specifier for argument");
		अवरोध;
	हाल CONVSPEC_ERROR:
		YOSYS_ABORT("Found unsupported format conversion specifier");
		अवरोध;
	हाल CONVSPEC_SIGNED_INT:
		अगर constexpr (!std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to signed integer");
		}
		*specs = found;
		अवरोध;
	हाल CONVSPEC_UNSIGNED_INT:
		अगर constexpr (!std::is_convertible_v<Arg, अचिन्हित दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		अवरोध;
	हाल CONVSPEC_DOUBLE:
		अगर constexpr (!std::is_convertible_v<Arg, द्विगुन>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		अवरोध;
	हाल CONVSPEC_CHAR_PTR:
		अगर constexpr (!std::is_convertible_v<Arg, स्थिर अक्षर *> &&
		        !std::is_convertible_v<Arg, स्थिर std::string &> &&
			!std::is_convertible_v<Arg, स्थिर std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		अवरोध;
	हाल CONVSPEC_VOID_PTR:
		अगर constexpr (!std::is_convertible_v<Arg, स्थिर व्योम *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		अवरोध;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_गिनती::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
व्योम format_emit_long_long(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
व्योम format_emit_unsigned_long_long(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, अचिन्हित दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
व्योम format_emit_double(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, द्विगुन arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
व्योम format_emit_char_ptr(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, स्थिर अक्षर *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
व्योम format_emit_string(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, स्थिर std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
व्योम format_emit_string_view(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
व्योम format_emit_void_ptr(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, स्थिर व्योम *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
ढाँचा <typename Arg>
अंतरभूत व्योम format_emit_one(std::string &परिणाम, std::string_view fmt, स्थिर FoundFormatSpec &ffspec,
	पूर्णांक *dynamic_ints, स्थिर Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_गिनती अंक_dynamic_ints = ffspec.अंक_dynamic_ints;
	चयन (ffspec.spec) {
	हाल CONVSPEC_SIGNED_INT:
		अगर constexpr (std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			दीर्घ दीर्घ s = arg;
			format_emit_long_long(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अवरोध;
	हाल CONVSPEC_UNSIGNED_INT:
		अगर constexpr (std::is_convertible_v<Arg, अचिन्हित दीर्घ दीर्घ>) {
			अचिन्हित दीर्घ दीर्घ s = arg;
			format_emit_unsigned_long_long(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अवरोध;
	हाल CONVSPEC_DOUBLE:
		अगर constexpr (std::is_convertible_v<Arg, द्विगुन>) {
			द्विगुन s = arg;
			format_emit_double(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अवरोध;
	हाल CONVSPEC_CHAR_PTR:
		अगर constexpr (std::is_convertible_v<Arg, स्थिर अक्षर *>) {
			स्थिर अक्षर *s = arg;
			format_emit_char_ptr(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अगर constexpr (std::is_convertible_v<Arg, स्थिर std::string &>) {
			स्थिर std::string &s = arg;
			format_emit_string(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अगर constexpr (std::is_convertible_v<Arg, स्थिर std::string_view &>) {
			स्थिर std::string_view &s = arg;
			format_emit_string_view(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अगर constexpr (has_name_member<Arg>()) {
			स्थिर std::string &s = arg.name.unescape();
			format_emit_string(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अगर constexpr (ptr_has_name_member<Arg>()) {
			स्थिर std::string &s = arg->name.unescape();
			format_emit_string(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अवरोध;
	हाल CONVSPEC_VOID_PTR:
		अगर constexpr (std::is_convertible_v<Arg, स्थिर व्योम *>) {
			स्थिर व्योम *s = arg;
			format_emit_void_ptr(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, s);
			वापस;
		}
		अवरोध;
	शेष:
		अवरोध;
	}
	YOSYS_ABORT("Internal error");
}

// Append the format string `fmt` to `result`, assuming there are no format conversion
// specifiers other than "%%" and therefore no arguments. Unescape "%%".
व्योम format_emit_unescaped(std::string &परिणाम, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

अंतरभूत व्योम format_emit(std::string &परिणाम, std::string_view fmt, पूर्णांक fmt_start,
	bool has_escapes, स्थिर FoundFormatSpec*, पूर्णांक*, Dynamic_Int_गिनती)
{
	fmt = fmt.substr(fmt_start);
	अगर (has_escapes) {
		format_emit_unescaped(परिणाम, fmt);
	} अन्यथा {
		परिणाम += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
ढाँचा <typename Arg, typename... Args>
अंतरभूत व्योम format_emit(std::string &परिणाम, std::string_view fmt, पूर्णांक fmt_start, bool has_escapes,
	स्थिर FoundFormatSpec* specs, पूर्णांक *dynamic_ints, Dynamic_Int_गिनती अंक_dynamic_ints,
	स्थिर Arg &arg, स्थिर Args &... args)
{
	अगर (specs->अंक_dynamic_ints > अंक_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		अगर constexpr (std::is_convertible_v<Arg, पूर्णांक>) {
			dynamic_ints[static_cast<uint8_t>(अंक_dynamic_ints)] = arg;
		} अन्यथा {
			YOSYS_ABORT("Internal error");
		}
		format_emit(परिणाम, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_गिनती>(static_cast<uint8_t>(अंक_dynamic_ints) + 1), args...);
		वापस;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	अगर (has_escapes) {
		format_emit_unescaped(परिणाम, str);
	} अन्यथा {
		परिणाम += str;
	}
	format_emit_one(परिणाम, fmt, *specs, dynamic_ints, arg);
	format_emit(परिणाम, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_गिनती::NONE, args...);
}

ढाँचा <typename... Args>
अंतरभूत std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, स्थिर FoundFormatSpec* specs, स्थिर Args &... args)
{
	std::string परिणाम;
	पूर्णांक dynamic_ints[2] = { 0, 0 };
	format_emit(परिणाम, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_गिनती::NONE, args...);
	वापस परिणाम;
}
ढाँचा <>
अंतरभूत std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, स्थिर FoundFormatSpec*)
{
	अगर (!has_escapes) {
		वापस std::string(fmt);
	}
	वापस unescape_format_string(fmt);
}

// This class parses format strings to build a list of `FoundFormatSpecs` in `specs`.
// When the compiler supports `consteval` (C++20), this parsing happens at compile time and
// type errors will be reported at compile time. Otherwise the parsing happens at
// runtime and type errors will trigger an `abort()` at runtime.
ढाँचा <typename... Args>
class FmtString
{
खुला:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(स्थिर अक्षर *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_गिनती::NONE);
	}
	std::string format(स्थिर Args &... args)
	{
		वापस format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() स्थिर { वापस fmt; }
निजी:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[माप...(Args) ? माप...(Args) : 1] = {};
};

ढाँचा <typename T> काष्ठा WrapType { using type = T; };
ढाँचा <typename T> using TypeIdentity = typename WrapType<T>::type;

ढाँचा <typename... Args>
अंतरभूत std::string stringf(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	वापस fmt.format(args...);
}

पूर्णांक readsome(std::istream &f, अक्षर *s, पूर्णांक n);
std::string अग्र_token(std::string &text, स्थिर अक्षर *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(स्थिर std::string &text, स्थिर अक्षर *sep = " \t\r\n");
bool patmatch(स्थिर अक्षर *pattern, स्थिर अक्षर *string);
#अगर !defined(YOSYS_DISABLE_SPAWN)
पूर्णांक run_command(स्थिर std::string &command, std::function<व्योम(स्थिर std::string&)> process_पंक्ति = std::function<व्योम(स्थिर std::string&)>());
#पूर्ण_अगर
std::string get_base_tmpdir();
std::string make_अस्थायी_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_अस्थायी_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(स्थिर std::string& filename, bool is_exec = false);
bool check_directory_exists(स्थिर std::string& स_नाम, bool is_exec = false);
bool is_absolute_path(std::string filename);
व्योम remove_directory(std::string स_नाम);
bool create_directory(स्थिर std::string& स_नाम);
bool make_private_directory(स्थिर std::string& स_नाम);
std::string make_user_tmpdir(स्थिर std::string& name);
std::string escape_filename_spaces(स्थिर std::string& filename);
व्योम append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(स्थिर std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
काष्ठा ScopedFileLock {
	ScopedFileLock(स्थिर std::string &path);
	~ScopedFileLock();
	ScopedFileLock(स्थिर ScopedFileLock &) = delete;
	ScopedFileLock &चालक=(स्थिर ScopedFileLock &) = delete;
निजी:
#अगर defined(_WIN32)
	व्योम *handle = nullptr;
#या_अगर !defined(__wasi__)
	पूर्णांक fd = -1;
#पूर्ण_अगर
};

// Current process id (0 on WASI).
अचिन्हित get_process_id();

YOSYS_NAMESPACE_END

#पूर्ण_अगर // YOSYS_IO_H
