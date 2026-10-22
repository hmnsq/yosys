#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <filesystem>
#अन्तर्भाव <system_error>

#सचे !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अञ्ञथा
#अन्तर्भाव <io.h>
#endif

#सचे !defined(_WIN32) && !defined(__wasi__)
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

पूर्णाङ्क readsome(std::istream &f, वर्ण *s, पूर्णाङ्क n)
{
	पूर्णाङ्क rc = पूर्णाङ्क(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	सचे (rc == 0) {
		पूर्णाङ्क c = f.get();
		सचे (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	पटिनिवत्त rc;
}

std::string next_token(std::string &text, ध्रुव वर्ण *sep, तार्किक long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	सचे (pos_begin == std::string::npos)
		pos_begin = text.size();

	सचे (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		अत्थाय (size_t i = pos_begin+1; i < text.size(); i++) {
			सचे (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				पटिनिवत्त token;
			}
			सचे (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				पटिनिवत्त token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	सचे (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	पटिनिवत्त token;
}

std::vector<std::string> split_tokens(ध्रुव std::string &text, ध्रुव वर्ण *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	अत्थाय (वर्ण c : text) {
		सचे (strchr(sep, c)) {
			सचे (!current_token.empty()) {
				tokens.प्रक्षिप_back(current_token);
				current_token.clear();
			}
		} अञ्ञथा
			current_token += c;
	}
	सचे (!current_token.empty()) {
		tokens.प्रक्षिप_back(current_token);
		current_token.clear();
	}
	पटिनिवत्त tokens;
}

// this is very similar to fnmatch(). the exact rules used by this
// function are:
//
//    ?        matches any character except
//    *        matches any sequence of characters
//    [...]    matches any of the characters in the list
//    [!..]    matches any of the characters not in the list
//
// a backslash may be used to escape the next characters in the
// pattern. each special character can also simply match itself.
//
तार्किक patmatch(ध्रुव वर्ण *pattern, ध्रुव वर्ण *string)
{
	सचे (*pattern == 0)
		पटिनिवत्त *string == 0;

	सचे (*pattern == '\\') {
		सचे (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			पटिनिवत्त सच्च;
	}

	सचे (*pattern == '?') {
		सचे (*string == 0)
			पटिनिवत्त मिच्छा;
		पटिनिवत्त patmatch(pattern+1, string+1);
	}

	सचे (*pattern == '*') {
		याव (*string) {
			सचे (patmatch(pattern+1, string++))
				पटिनिवत्त सच्च;
		}
		पटिनिवत्त pattern[1] == 0;
	}

	सचे (*pattern == '[') {
		तार्किक found_match = मिच्छा;
		तार्किक inverted_list = pattern[1] == '!';
		ध्रुव वर्ण *p = pattern + (inverted_list ? 1 : 0);

		याव (*++p) {
			सचे (*p == ']') {
				सचे (found_match != inverted_list && patmatch(p+1, string+1))
					पटिनिवत्त सच्च;
				भङ्ग;
			}

			सचे (*p == '\\') {
				सचे (*++p == *string)
					found_match = सच्च;
			} अञ्ञथा
			सचे (*p == *string)
				found_match = सच्च;
		}
	}

	सचे (*pattern == *string)
		पटिनिवत्त patmatch(pattern+1, string+1);

	पटिनिवत्त मिच्छा;
}

std::string get_base_tmpdir()
{
	स्थिर std::string tmpdir;

	सचे (!tmpdir.empty()) {
		पटिनिवत्त tmpdir;
	}

#सचे defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	सचे (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), रिक्त, 0, रिक्त, रिक्त));
	सचे (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), रिक्त, रिक्त) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#अञ्ञथा
	वर्ण * var = std::getenv("TMPDIR");
	सचे (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		याव (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.निष्कासय_back();
		}
	} अञ्ञथा {
		tmpdir.assign("/tmp");
	}
#endif
	पटिनिवत्त tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#सचे defined(__wasm)
	स्थिर size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#नोचेद्यदि defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	याव (1) {
		अत्थाय (पूर्णाङ्क i = 0; i < 6; i++) {
			स्थिर std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			स्थिर uint32_t x = 314159265 ^ uint32_t(time(रिक्त));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		सचे (_access(template_str.c_str(), 0) != 0)
			भङ्ग;
	}
#अञ्ञथा
	पूर्णाङ्क suffixlen = template_str.size() - pos - 6;

	वर्ण *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	मुक्त(p);
#endif

	पटिनिवत्त template_str;
}

std::string make_temp_dir(std::string template_str)
{
#सचे defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	पटिनिवत्त template_str;
#नोचेद्यदि defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	पटिनिवत्त template_str;
#अञ्ञथा
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	पूर्णाङ्क suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	वर्ण *p = strdup(template_str.c_str());
	log_assert(p);
	वर्ण *res = mkdtemp(p);
	सचे (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	मुक्त(p);

	पटिनिवत्त template_str;
#endif
}

तार्किक check_is_directory(ध्रुव std::string& dirname)
{
#सचे defined(_WIN32)
	संरचना _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	सचे (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	सचे (_stat(dirname_.c_str(), &info) != 0)
	{
		पटिनिवत्त मिच्छा;
	}
	पटिनिवत्त (info.st_mode & _S_IFDIR) != 0;
#अञ्ञथा
	संरचना stat info;
	सचे (stat(dirname.c_str(), &info) != 0)
	{
		पटिनिवत्त मिच्छा;
	}
	पटिनिवत्त (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
तार्किक check_accessible(ध्रुव std::string& filename, तार्किक)
{
	पटिनिवत्त _access(filename.c_str(), 0) == 0;
}
#अञ्ञथा
तार्किक check_accessible(ध्रुव std::string& filename, तार्किक is_exec)
{
	पटिनिवत्त access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

तार्किक check_file_exists(ध्रुव std::string& filename, तार्किक is_exec)
{
	पटिनिवत्त check_accessible(filename, is_exec) && !check_is_directory(filename);
}
तार्किक check_directory_exists(ध्रुव std::string& filename, तार्किक is_exec)
{
	पटिनिवत्त check_accessible(filename, is_exec) && check_is_directory(filename);
}

तार्किक is_absolute_path(std::string filename)
{
#ifdef _WIN32
	पटिनिवत्त filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#अञ्ञथा
	पटिनिवत्त filename[0] == '/';
#endif
}

शून्य remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#अञ्ञथा
	संरचना stat stbuf;
	संरचना dirent **namelist;
	पूर्णाङ्क n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	अत्थाय (पूर्णाङ्क i = 0; i < n; i++) {
		सचे (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			सचे (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} अञ्ञथा
				remove_directory(buffer);
		}
		मुक्त(namelist[i]);
	}
	मुक्त(namelist);
	rmdir(dirname.c_str());
#endif
}

तार्किक create_directory(ध्रुव std::string& dirname)
{
#सचे defined(_WIN32)
	पूर्णाङ्क ret = _mkdir(dirname.c_str());
#अञ्ञथा
	mode_t mode = 0755;
	पूर्णाङ्क ret = mkdir(dirname.c_str(), mode);
#endif
	सचे (ret == 0)
		पटिनिवत्त सच्च;

	विकल्प (errno)
	{
	पक्ष ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			सचे (pos == std::string::npos)
#सचे defined(_WIN32)
				pos = dirname.find_last_of('\\');
			सचे (pos == std::string::npos)
#endif
				पटिनिवत्त मिच्छा;
			सचे (!create_directory( dirname.substr(0, pos) ))
				पटिनिवत्त मिच्छा;
		}
		// now, try to create again
#सचे defined(_WIN32)
		पटिनिवत्त 0 == _mkdir(dirname.c_str());
#अञ्ञथा
		पटिनिवत्त 0 == mkdir(dirname.c_str(), mode);
#endif

	पक्ष EEXIST:
		// done!
		पटिनिवत्त check_directory_exists(dirname);

	स्वभाव:
		पटिनिवत्त मिच्छा;
	}
}

तार्किक make_private_directory(ध्रुव std::string& dirname)
{
#सचे defined(_WIN32) || defined(__wasi__)
	सचे (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		पटिनिवत्त मिच्छा;
	}
	पटिनिवत्त सच्च;
#अञ्ञथा
	सचे (mkdir(dirname.c_str(), 0700) == 0)
		पटिनिवत्त सच्च;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	पूर्णाङ्क err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	सचे (err == ENOENT && !parent.empty() && create_directory(parent)) {
		सचे (mkdir(dirname.c_str(), 0700) == 0)
			पटिनिवत्त सच्च;
		err = errno;
	}

	सचे (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		पटिनिवत्त मिच्छा;
	}

	// Do not use a leftover that isn't ours
	संरचना stat st;
	सचे (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		पटिनिवत्त मिच्छा;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	सचे (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		पटिनिवत्त मिच्छा;
	}

	पटिनिवत्त सच्च;
#endif
}

std::string make_user_tmpdir(ध्रुव std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#सचे defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#अञ्ञथा
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	पटिनिवत्त make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ध्रुव std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	अत्थाय (auto c : filename)
	{
		सचे (c == ' ')
			out += "\\ ";
		अञ्ञथा
			out.प्रक्षिप_back(c);
	}
	पटिनिवत्त out;
}

शून्य append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	पटिनिवत्त std::filesystem::path(path).filename().string();
}

std::string absolute_path(ध्रुव std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	सचे (ec)
		पटिनिवत्त "";

	result = std::filesystem::weakly_canonical(result, ec);

	सचे (ec)
		पटिनिवत्त "";

	पटिनिवत्त result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	सचे (parent.empty()) {
		पटिनिवत्त "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	सचे (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	पटिनिवत्त result;
}

शून्य format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	अत्थाय (size_t i = 0; i < fmt.size(); ++i) {
		वर्ण ch = fmt[i];
		result.प्रक्षिप_back(ch);
		सचे (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	पटिनिवत्त result;
}

स्थिर std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	पटिनिवत्त result;
}

स्थिर पूर्णाङ्क spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	विकल्प (spec[spec.size() - 1]) {
	पक्ष 'd':
	पक्ष 'i':
	पक्ष 'o':
	पक्ष 'u':
	पक्ष 'x':
	पक्ष 'X':
		विकल्प (spec[spec.size() - 2]) {
		पक्ष 'h':
			सचे (spec[spec.size() - 3] == 'h')
				पटिनिवत्त परिमाण(वर्ण);
			पटिनिवत्त परिमाण(short);
		पक्ष 'l':
			सचे (spec[spec.size() - 3] == 'l')
				पटिनिवत्त परिमाण(long long);
			पटिनिवत्त परिमाण(long);
		पक्ष 'L':
		पक्ष 'q':
			पटिनिवत्त परिमाण(long long);
		पक्ष 'j':
			पटिनिवत्त परिमाण(intmax_t);
		पक्ष 'z':
		पक्ष 'Z':
			पटिनिवत्त परिमाण(size_t);
		पक्ष 't':
			पटिनिवत्त परिमाण(ptrdiff_t);
		}
		पटिनिवत्त परिमाण(पूर्णाङ्क);
	पक्ष 'e':
	पक्ष 'E':
	पक्ष 'f':
	पक्ष 'F':
	पक्ष 'g':
	पक्ष 'G':
	पक्ष 'a':
	पक्ष 'A':
		सचे (spec[spec.size() - 2] == 'L')
			पटिनिवत्त परिमाण(long द्विगुण);
		सचे (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			पटिनिवत्त परिमाण(long द्विगुण);
		पटिनिवत्त परिमाण(द्विगुण);
	पक्ष 'c':
		सचे (spec[spec.size() - 2] == 'l') {
			पटिनिवत्त परिमाण(wchar_t);
		}
		पटिनिवत्त परिमाण(unsigned वर्ण);
	पक्ष 'C':
		पटिनिवत्त परिमाण(wchar_t);
	पक्ष 's':
	पक्ष 'p':
	पक्ष 'S':
	पक्ष 'n':
		पटिनिवत्त परिमाण(शून्य *);
	पक्ष 'm':
		पटिनिवत्त परिमाण(पूर्णाङ्क);
	स्वभाव:
		पटिनिवत्त -1;
	}
}

template <typename Arg>
स्थिर शून्य format_emit_stringf(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	विकल्प (अङ्कः_dynamic_ints) {
	पक्ष Dynamic_Int_गणनम्::NONE:
		result += string_view_stringf(spec, arg);
		पटिनिवत्त;
	पक्ष Dynamic_Int_गणनम्::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		पटिनिवत्त;
	पक्ष Dynamic_Int_गणनम्::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		पटिनिवत्त;
	}
	YOSYS_ABORT("Internal error");
}

शून्य format_emit_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, long long arg)
{
	सचे (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<पूर्णाङ्क>(arg));
		पटिनिवत्त;
	}
	सचे (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, (पूर्णाङ्क)arg);
	} अञ्ञथा {
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
	}
}

शून्य format_emit_unsigned_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, unsigned long long arg)
{
	सचे (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned पूर्णाङ्क>(arg));
		पटिनिवत्त;
	}
	सचे (spec == "%c") {
		result += static_cast<वर्ण>(arg);
		पटिनिवत्त;
	}
	सचे (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, (unsigned पूर्णाङ्क)arg);
	} अञ्ञथा {
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
	}
}

शून्य format_emit_double(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, द्विगुण arg)
{
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

शून्य format_emit_char_ptr(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव वर्ण *arg)
{
	सचे (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		पटिनिवत्त;
	}
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

शून्य format_emit_string(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव std::string &arg)
{
	सचे (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		पटिनिवत्त;
	}
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg.c_str());
}

शून्य format_emit_string_view(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, std::string_view arg)
{
	सचे (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		पटिनिवत्त;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, std::string(arg).c_str());
}

शून्य format_emit_void_ptr(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव शून्य *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

#सचे defined(_WIN32)
ScopedFileLock::ScopedFileLock(ध्रुव std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			रिक्त, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, रिक्त);
	सचे (h == INVALID_HANDLE_मानम्) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		पटिनिवत्त;
	}
	OVERLAPPED ov = {};
	सचे (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		पटिनिवत्त;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	सचे (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#नोचेद्यदि !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ध्रुव std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	सचे (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	सचे (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		पटिनिवत्त;
	}
	पूर्णाङ्क ret;
	याव ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	सचे (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	सचे (fd >= 0)
		close(fd); // releases the flock
}
#अञ्ञथा
ScopedFileLock::ScopedFileLock(ध्रुव std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#सचे defined(_WIN32)
	पटिनिवत्त GetCurrentProcessId();
#नोचेद्यदि defined(__wasi__)
	पटिनिवत्त 0;
#अञ्ञथा
	पटिनिवत्त getpid();
#endif
}

YOSYS_NAMESPACE_END
