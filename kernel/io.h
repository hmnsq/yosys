#अन्तर्भाव <string>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <type_traits>
#अन्तर्भाव "kernel/yosys_common.h"

#ifndef YOSYS_IO_H
#define YOSYS_IO_H

YOSYS_NAMESPACE_BEGIN

संरचना IdString;

अन्तःस्थ std::string vstringf(ध्रुव वर्ण *fmt, बहु_सूची ap)
{
	// For the common case of strings shorter than 128, save a heap
	// allocation by using a stack allocated buffer.
	ध्रुव पूर्णाङ्क kBufSize = 128;
	वर्ण buf[kBufSize];
	buf[0] = '\0';
	बहु_सूची apc;
	va_copy(apc, ap);
	पूर्णाङ्क n = vsnprintf(buf, kBufSize, fmt, apc);
	बहु_पूर्ण(apc);
	यदि (n < kBufSize)
		प्रत्यागम std::string(buf);

	std::string string;
	वर्ण *str = रिक्त;
#यदि defined(_WIN32) || defined(__CYGWIN__)
	पूर्णाङ्क sz = 2 * kBufSize, rc;
	यावत् (1) {
		va_copy(apc, ap);
		str = (वर्ण *)पुनः_स्मृति(str, sz);
		rc = vsnprintf(str, sz, fmt, apc);
		बहु_पूर्ण(apc);
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

constexpr ConversionSpecifier parse_conversion_specifier(वर्ण ch, वर्ण prev_ch)
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
		यदि (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
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
		यदि (prev_ch == 'l' || prev_ch == 'q' || prev_ch == 'L') {
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

गणना class Dynamic_Int_गिनती : uint8_t {
	NONE = 0,
	ONE = 1,
	TWO = 2,
};

// Describes a printf-style format conversion specifier found in a format string.
संरचना FoundFormatSpec
{
	// The start offset of the conversion spec in the format string.
	पूर्णाङ्क start;
	// The end offset of the conversion spec in the format string.
	पूर्णाङ्क end;
	ConversionSpecifier spec;
	// Number of int args consumed by '*' dynamic width/precision args.
	Dynamic_Int_गिनती अंक_dynamic_ints;
};

// Ensure there is no format spec.
constexpr शून्य ensure_no_format_spec(std::string_view fmt, पूर्णाङ्क index, bool *has_escapes)
{
	पूर्णाङ्क fmt_size = static_cast<पूर्णाङ्क>(fmt.size());
	// A trailing '%' is not a format spec.
	यावत् (index + 1 < fmt_size) {
		यदि (fmt[index] != '%') {
			++index;
			अनुवर्त;
		}
		यदि (fmt[index + 1] != '%') {
			YOSYS_ABORT("More format conversion specifiers than arguments");
		}
		*has_escapes = true;
		index += 2;
	}
}

// Returns the next format conversion specifier (starting with '%').
// Returns CONVSPEC_NONE if there isn't a format conversion specifier.
constexpr FoundFormatSpec find_next_format_spec(std::string_view fmt, पूर्णाङ्क fmt_start, bool *has_escapes)
{
	पूर्णाङ्क index = fmt_start;
	पूर्णाङ्क fmt_size = static_cast<पूर्णाङ्क>(fmt.size());
	यावत् (index < fmt_size) {
		यदि (fmt[index] != '%') {
			++index;
			अनुवर्त;
		}
		पूर्णाङ्क p = index + 1;
		uint8_t अंक_dynamic_ints = 0;
		यावत् (true) {
			यदि (p == fmt_size) {
				प्रत्यागम {0, 0, CONVSPEC_NONE, Dynamic_Int_गिनती::NONE};
			}
			यदि (fmt[p] == '%') {
				*has_escapes = true;
				index = p + 1;
				भङ्ग;
			}
			यदि (fmt[p] == '*') {
				यदि (अंक_dynamic_ints >= 2) {
					प्रत्यागम {0, 0, CONVSPEC_ERROR, Dynamic_Int_गिनती::NONE};
				}
				++अंक_dynamic_ints;
			}
			ConversionSpecifier spec = parse_conversion_specifier(fmt[p], fmt[p - 1]);
			यदि (spec != CONVSPEC_NONE) {
				प्रत्यागम {index, p + 1, spec, static_cast<Dynamic_Int_गिनती>(अंक_dynamic_ints)};
			}
			++p;
		}
	}
	प्रत्यागम {0, 0, CONVSPEC_NONE, Dynamic_Int_गिनती::NONE};
}

ढाँचा <typename... Args>
constexpr typename std::enable_if<परिमाण...(Args) == 0>::type
check_format(std::string_view fmt, पूर्णाङ्क fmt_start, bool *has_escapes, FoundFormatSpec*, Dynamic_Int_गिनती)
{
	ensure_no_format_spec(fmt, fmt_start, has_escapes);
}

ढाँचा <class T>
स्थिर स्वतः has_name_member_imp(पूर्णाङ्क)
	-> decltype(static_cast<ध्रुव IdString>(std::declval<T>().name), std::true_type{});

ढाँचा <class T>
स्थिर स्वतः has_name_member_imp(दीर्घ)
	-> std::false_type;

ढाँचा <class T>
संरचना has_name_member : decltype(has_name_member_imp<T>(0)){};

ढाँचा <class T>
स्थिर स्वतः ptr_has_name_member_imp(पूर्णाङ्क)
	-> decltype(static_cast<ध्रुव IdString>(std::declval<T>()->name), std::true_type{});

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
constexpr शून्य check_format(std::string_view fmt, पूर्णाङ्क fmt_start, bool *has_escapes, FoundFormatSpec* specs,
        Dynamic_Int_गिनती int_args_consumed)
{
	FoundFormatSpec found = find_next_format_spec(fmt, fmt_start, has_escapes);
	यदि (found.अंक_dynamic_ints > int_args_consumed) {
		// We need to consume at least one more int for the dynamic
		// width/precision of this format conversion specifier.
		यदि constexpr (!std::is_convertible_v<Arg, पूर्णाङ्क>) {
			YOSYS_ABORT("Expected dynamic int argument");
		}
		check_format<Args...>(fmt, fmt_start, has_escapes, specs,
			static_cast<Dynamic_Int_गिनती>(static_cast<uint8_t>(int_args_consumed) + 1));
		प्रत्यागम;
	}
	विकल्प (found.spec) {
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
		*specs = found;
		भङ्ग;
	पक्ष CONVSPEC_UNSIGNED_INT:
		यदि constexpr (!std::is_convertible_v<Arg, अचिन्हित दीर्घ दीर्घ>) {
			YOSYS_ABORT("Expected type convertible to unsigned integer");
		}
		*specs = found;
		भङ्ग;
	पक्ष CONVSPEC_DOUBLE:
		यदि constexpr (!std::is_convertible_v<Arg, द्विगुण>) {
			YOSYS_ABORT("Expected type convertible to double");
		}
		*specs = found;
		भङ्ग;
	पक्ष CONVSPEC_CHAR_PTR:
		यदि constexpr (!std::is_convertible_v<Arg, ध्रुव वर्ण *> &&
		        !std::is_convertible_v<Arg, ध्रुव std::string &> &&
			!std::is_convertible_v<Arg, ध्रुव std::string_view &> &&
			!has_name_member<Arg>() &&
			!ptr_has_name_member<Arg>()) {
			YOSYS_ABORT("Expected type convertible to char *");
		}
		*specs = found;
		भङ्ग;
	पक्ष CONVSPEC_VOID_PTR:
		यदि constexpr (!std::is_convertible_v<Arg, ध्रुव शून्य *>) {
			YOSYS_ABORT("Expected pointer type");
		}
		*specs = found;
		भङ्ग;
	}
	check_format<Args...>(fmt, found.end, has_escapes, specs + 1, Dynamic_Int_गिनती::NONE);
}

// Emit the string representation of `arg` that has been converted to a `long long'.
शून्य format_emit_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `unsigned long long'.
शून्य format_emit_unsigned_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, अचिन्हित दीर्घ दीर्घ arg);

// Emit the string representation of `arg` that has been converted to a `double'.
शून्य format_emit_double(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, द्विगुण arg);

// Emit the string representation of `arg` that has been converted to a `const char*'.
शून्य format_emit_char_ptr(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, ध्रुव वर्ण *arg);

// Emit the string representation of `arg` that has been converted to a `std::string'.
शून्य format_emit_string(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, ध्रुव std::string &arg);

// Emit the string representation of `arg` that has been converted to a `std::string_view'.
शून्य format_emit_string_view(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, std::string_view arg);

// Emit the string representation of `arg` that has been converted to a `double'.
शून्य format_emit_void_ptr(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, ध्रुव शून्य *arg);

// Emit the string representation of `arg` according to the given `FoundFormatSpec`,
// appending it to `result`.
ढाँचा <typename Arg>
अन्तःस्थ शून्य format_emit_one(std::string &result, std::string_view fmt, ध्रुव FoundFormatSpec &ffspec,
	पूर्णाङ्क *dynamic_ints, ध्रुव Arg& arg)
{
	std::string_view spec = fmt.substr(ffspec.start, ffspec.end - ffspec.start);
	Dynamic_Int_गिनती अंक_dynamic_ints = ffspec.अंक_dynamic_ints;
	विकल्प (ffspec.spec) {
	पक्ष CONVSPEC_SIGNED_INT:
		यदि constexpr (std::is_convertible_v<Arg, दीर्घ दीर्घ>) {
			दीर्घ दीर्घ s = arg;
			format_emit_long_long(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_UNSIGNED_INT:
		यदि constexpr (std::is_convertible_v<Arg, अचिन्हित दीर्घ दीर्घ>) {
			अचिन्हित दीर्घ दीर्घ s = arg;
			format_emit_unsigned_long_long(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_DOUBLE:
		यदि constexpr (std::is_convertible_v<Arg, द्विगुण>) {
			द्विगुण s = arg;
			format_emit_double(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_CHAR_PTR:
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव वर्ण *>) {
			ध्रुव वर्ण *s = arg;
			format_emit_char_ptr(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव std::string &>) {
			ध्रुव std::string &s = arg;
			format_emit_string(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव std::string_view &>) {
			ध्रुव std::string_view &s = arg;
			format_emit_string_view(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (has_name_member<Arg>()) {
			ध्रुव std::string &s = arg.name.unescape();
			format_emit_string(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		यदि constexpr (ptr_has_name_member<Arg>()) {
			ध्रुव std::string &s = arg->name.unescape();
			format_emit_string(result, spec, dynamic_ints, अंक_dynamic_ints, s);
			प्रत्यागम;
		}
		भङ्ग;
	पक्ष CONVSPEC_VOID_PTR:
		यदि constexpr (std::is_convertible_v<Arg, ध्रुव शून्य *>) {
			ध्रुव शून्य *s = arg;
			format_emit_void_ptr(result, spec, dynamic_ints, अंक_dynamic_ints, s);
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
शून्य format_emit_unescaped(std::string &result, std::string_view fmt);
std::string unescape_format_string(std::string_view fmt);

अन्तःस्थ शून्य format_emit(std::string &result, std::string_view fmt, पूर्णाङ्क fmt_start,
	bool has_escapes, ध्रुव FoundFormatSpec*, पूर्णाङ्क*, Dynamic_Int_गिनती)
{
	fmt = fmt.substr(fmt_start);
	यदि (has_escapes) {
		format_emit_unescaped(result, fmt);
	} अन्यथा {
		result += fmt;
	}
}
// Format `args` according to `fmt` (starting at `fmt_start`) and `specs` and append to `result`.
// `num_dynamic_ints` in `dynamic_ints[]` have already been collected to provide as
// dynamic width/precision args for the next format conversion specifier.
ढाँचा <typename Arg, typename... Args>
अन्तःस्थ शून्य format_emit(std::string &result, std::string_view fmt, पूर्णाङ्क fmt_start, bool has_escapes,
	ध्रुव FoundFormatSpec* specs, पूर्णाङ्क *dynamic_ints, Dynamic_Int_गिनती अंक_dynamic_ints,
	ध्रुव Arg &arg, ध्रुव Args &... args)
{
	यदि (specs->अंक_dynamic_ints > अंक_dynamic_ints) {
		// Collect another int for the dynamic width precision/args
		// for the next format conversion specifier.
		यदि constexpr (std::is_convertible_v<Arg, पूर्णाङ्क>) {
			dynamic_ints[static_cast<uint8_t>(अंक_dynamic_ints)] = arg;
		} अन्यथा {
			YOSYS_ABORT("Internal error");
		}
		format_emit(result, fmt, fmt_start, has_escapes, specs, dynamic_ints,
		            static_cast<Dynamic_Int_गिनती>(static_cast<uint8_t>(अंक_dynamic_ints) + 1), args...);
		प्रत्यागम;
	}
	std::string_view str = fmt.substr(fmt_start, specs->start - fmt_start);
	यदि (has_escapes) {
		format_emit_unescaped(result, str);
	} अन्यथा {
		result += str;
	}
	format_emit_one(result, fmt, *specs, dynamic_ints, arg);
	format_emit(result, fmt, specs->end, has_escapes, specs + 1, dynamic_ints, Dynamic_Int_गिनती::NONE, args...);
}

ढाँचा <typename... Args>
अन्तःस्थ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ध्रुव FoundFormatSpec* specs, ध्रुव Args &... args)
{
	std::string result;
	पूर्णाङ्क dynamic_ints[2] = { 0, 0 };
	format_emit(result, fmt, 0, has_escapes, specs, dynamic_ints, Dynamic_Int_गिनती::NONE, args...);
	प्रत्यागम result;
}
ढाँचा <>
अन्तःस्थ std::string format_emit_toplevel(std::string_view fmt, bool has_escapes, ध्रुव FoundFormatSpec*)
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
ढाँचा <typename... Args>
class FmtString
{
सार्वजनिक:
	// Implicit conversion from const char * means that users can pass
	// C string constants which are automatically parsed.
	YOSYS_CONSTEVAL FmtString(ध्रुव वर्ण *p) : fmt(p)
	{
		check_format<Args...>(fmt, 0, &has_escapes, specs, Dynamic_Int_गिनती::NONE);
	}
	std::string format(ध्रुव Args &... args)
	{
		प्रत्यागम format_emit_toplevel(fmt, has_escapes, specs, args...);
	}
	std::string_view format_string() ध्रुव { प्रत्यागम fmt; }
गुप्त:
	std::string_view fmt;
	bool has_escapes = false;
	// Making array at least size of one to make MSVC happy and strict to standards
	FoundFormatSpec specs[परिमाण...(Args) ? परिमाण...(Args) : 1] = {};
};

ढाँचा <typename T> संरचना WrapType { using type = T; };
ढाँचा <typename T> using TypeIdentity = typename WrapType<T>::type;

ढाँचा <typename... Args>
अन्तःस्थ std::string stringf(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	प्रत्यागम fmt.format(args...);
}

पूर्णाङ्क readsome(std::istream &f, वर्ण *s, पूर्णाङ्क n);
std::string next_token(std::string &text, ध्रुव वर्ण *sep = " \t\r\n", bool long_strings = false);
std::vector<std::string> split_tokens(ध्रुव std::string &text, ध्रुव वर्ण *sep = " \t\r\n");
bool patmatch(ध्रुव वर्ण *pattern, ध्रुव वर्ण *string);
#यदि !defined(YOSYS_DISABLE_SPAWN)
पूर्णाङ्क run_command(ध्रुव std::string &command, std::function<शून्य(ध्रुव std::string&)> process_पंक्ति = std::function<शून्य(ध्रुव std::string&)>());
#endif
std::string get_base_tmpdir();
std::string make_temp_file(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
std::string make_temp_dir(std::string template_str = get_base_tmpdir() + "/yosys_XXXXXX");
bool check_file_exists(ध्रुव std::string& filename, bool is_exec = false);
bool check_directory_exists(ध्रुव std::string& स_नाम, bool is_exec = false);
bool is_absolute_path(std::string filename);
शून्य remove_directory(std::string स_नाम);
bool create_directory(ध्रुव std::string& स_नाम);
bool make_private_directory(ध्रुव std::string& स_नाम);
std::string make_user_tmpdir(ध्रुव std::string& name);
std::string escape_filename_spaces(ध्रुव std::string& filename);
शून्य append_globbed(std::vector<std::string>& paths, std::string pattern);
std::string name_from_file_path(std::string path);
std::string absolute_path(ध्रुव std::string& path);
std::string parent_from_file_path(std::string path);

// Exclusive inter-process file lock (flock/LockFileEx, no-op on WASI),
// released on destruction. Never unlinks the lock file: that would reopen the race.
संरचना ScopedFileLock {
	ScopedFileLock(ध्रुव std::string &path);
	~ScopedFileLock();
	ScopedFileLock(ध्रुव ScopedFileLock &) = delete;
	ScopedFileLock &चालक=(ध्रुव ScopedFileLock &) = delete;
गुप्त:
#यदि defined(_WIN32)
	शून्य *handle = nullptr;
#नोचेद्यदि !defined(__wasi__)
	पूर्णाङ्क fd = -1;
#endif
};

// Current process id (0 on WASI).
अचिन्हित get_process_id();

YOSYS_NAMESPACE_END

#endif // YOSYS_IO_H
