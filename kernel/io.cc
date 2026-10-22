#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <filesystem>
#अन्तर्भाव <system_error>

#जइ !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अण्णहा
#अन्तर्भाव <io.h>
#endif

#जइ !defined(_WIN32) && !defined(__wasi__)
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
	जइ (rc == 0) {
		पूर्णाङ्क c = f.get();
		जइ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	प्रत्यागम rc;
}

std::string next_token(std::string &text, ध्रुव वर्ण *sep, तार्किक long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	जइ (pos_begin == std::string::npos)
		pos_begin = text.size();

	जइ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		प्रति (size_t i = pos_begin+1; i < text.size(); i++) {
			जइ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				प्रत्यागम token;
			}
			जइ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				प्रत्यागम token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	जइ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	प्रत्यागम token;
}

std::vector<std::string> split_tokens(ध्रुव std::string &text, ध्रुव वर्ण *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	प्रति (वर्ण c : text) {
		जइ (strchr(sep, c)) {
			जइ (!current_token.empty()) {
				tokens.प्रक्षिप_back(current_token);
				current_token.clear();
			}
		} अण्णहा
			current_token += c;
	}
	जइ (!current_token.empty()) {
		tokens.प्रक्षिप_back(current_token);
		current_token.clear();
	}
	प्रत्यागम tokens;
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
	जइ (*pattern == 0)
		प्रत्यागम *string == 0;

	जइ (*pattern == '\\') {
		जइ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			प्रत्यागम सच्च;
	}

	जइ (*pattern == '?') {
		जइ (*string == 0)
			प्रत्यागम मिच्छ;
		प्रत्यागम patmatch(pattern+1, string+1);
	}

	जइ (*pattern == '*') {
		जाव (*string) {
			जइ (patmatch(pattern+1, string++))
				प्रत्यागम सच्च;
		}
		प्रत्यागम pattern[1] == 0;
	}

	जइ (*pattern == '[') {
		तार्किक found_match = मिच्छ;
		तार्किक inverted_list = pattern[1] == '!';
		ध्रुव वर्ण *p = pattern + (inverted_list ? 1 : 0);

		जाव (*++p) {
			जइ (*p == ']') {
				जइ (found_match != inverted_list && patmatch(p+1, string+1))
					प्रत्यागम सच्च;
				भङ्ग;
			}

			जइ (*p == '\\') {
				जइ (*++p == *string)
					found_match = सच्च;
			} अण्णहा
			जइ (*p == *string)
				found_match = सच्च;
		}
	}

	जइ (*pattern == *string)
		प्रत्यागम patmatch(pattern+1, string+1);

	प्रत्यागम मिच्छ;
}

std::string get_base_tmpdir()
{
	स्थिर std::string tmpdir;

	जइ (!tmpdir.empty()) {
		प्रत्यागम tmpdir;
	}

#जइ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	जइ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), रिक्त, 0, रिक्त, रिक्त));
	जइ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), रिक्त, रिक्त) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#अण्णहा
	वर्ण * var = std::getenv("TMPDIR");
	जइ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		जाव (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.निष्कासय_back();
		}
	} अण्णहा {
		tmpdir.assign("/tmp");
	}
#endif
	प्रत्यागम tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#जइ defined(__wasm)
	स्थिर size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#नोचेद्यदि defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	जाव (1) {
		प्रति (पूर्णाङ्क i = 0; i < 6; i++) {
			स्थिर std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			स्थिर uint32_t x = 314159265 ^ uint32_t(time(रिक्त));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		जइ (_access(template_str.c_str(), 0) != 0)
			भङ्ग;
	}
#अण्णहा
	पूर्णाङ्क suffixlen = template_str.size() - pos - 6;

	वर्ण *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	मुक्त(p);
#endif

	प्रत्यागम template_str;
}

std::string make_temp_dir(std::string template_str)
{
#जइ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	प्रत्यागम template_str;
#नोचेद्यदि defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	प्रत्यागम template_str;
#अण्णहा
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	पूर्णाङ्क suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	वर्ण *p = strdup(template_str.c_str());
	log_assert(p);
	वर्ण *res = mkdtemp(p);
	जइ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	मुक्त(p);

	प्रत्यागम template_str;
#endif
}

तार्किक check_is_directory(ध्रुव std::string& dirname)
{
#जइ defined(_WIN32)
	संरचना _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	जइ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	जइ (_stat(dirname_.c_str(), &info) != 0)
	{
		प्रत्यागम मिच्छ;
	}
	प्रत्यागम (info.st_mode & _S_IFDIR) != 0;
#अण्णहा
	संरचना stat info;
	जइ (stat(dirname.c_str(), &info) != 0)
	{
		प्रत्यागम मिच्छ;
	}
	प्रत्यागम (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
तार्किक check_accessible(ध्रुव std::string& filename, तार्किक)
{
	प्रत्यागम _access(filename.c_str(), 0) == 0;
}
#अण्णहा
तार्किक check_accessible(ध्रुव std::string& filename, तार्किक is_exec)
{
	प्रत्यागम access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

तार्किक check_file_exists(ध्रुव std::string& filename, तार्किक is_exec)
{
	प्रत्यागम check_accessible(filename, is_exec) && !check_is_directory(filename);
}
तार्किक check_directory_exists(ध्रुव std::string& filename, तार्किक is_exec)
{
	प्रत्यागम check_accessible(filename, is_exec) && check_is_directory(filename);
}

तार्किक is_absolute_path(std::string filename)
{
#ifdef _WIN32
	प्रत्यागम filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#अण्णहा
	प्रत्यागम filename[0] == '/';
#endif
}

शून्य remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#अण्णहा
	संरचना stat stbuf;
	संरचना dirent **namelist;
	पूर्णाङ्क n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	प्रति (पूर्णाङ्क i = 0; i < n; i++) {
		जइ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			जइ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} अण्णहा
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
#जइ defined(_WIN32)
	पूर्णाङ्क ret = _mkdir(dirname.c_str());
#अण्णहा
	mode_t mode = 0755;
	पूर्णाङ्क ret = mkdir(dirname.c_str(), mode);
#endif
	जइ (ret == 0)
		प्रत्यागम सच्च;

	विकल्प (errno)
	{
	पक्ष ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			जइ (pos == std::string::npos)
#जइ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			जइ (pos == std::string::npos)
#endif
				प्रत्यागम मिच्छ;
			जइ (!create_directory( dirname.substr(0, pos) ))
				प्रत्यागम मिच्छ;
		}
		// now, try to create again
#जइ defined(_WIN32)
		प्रत्यागम 0 == _mkdir(dirname.c_str());
#अण्णहा
		प्रत्यागम 0 == mkdir(dirname.c_str(), mode);
#endif

	पक्ष EEXIST:
		// done!
		प्रत्यागम check_directory_exists(dirname);

	स्वभाव:
		प्रत्यागम मिच्छ;
	}
}

तार्किक make_private_directory(ध्रुव std::string& dirname)
{
#जइ defined(_WIN32) || defined(__wasi__)
	जइ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		प्रत्यागम मिच्छ;
	}
	प्रत्यागम सच्च;
#अण्णहा
	जइ (mkdir(dirname.c_str(), 0700) == 0)
		प्रत्यागम सच्च;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	पूर्णाङ्क err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	जइ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		जइ (mkdir(dirname.c_str(), 0700) == 0)
			प्रत्यागम सच्च;
		err = errno;
	}

	जइ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		प्रत्यागम मिच्छ;
	}

	// Do not use a leftover that isn't ours
	संरचना stat st;
	जइ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		प्रत्यागम मिच्छ;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	जइ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		प्रत्यागम मिच्छ;
	}

	प्रत्यागम सच्च;
#endif
}

std::string make_user_tmpdir(ध्रुव std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#जइ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#अण्णहा
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	प्रत्यागम make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ध्रुव std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	प्रति (auto c : filename)
	{
		जइ (c == ' ')
			out += "\\ ";
		अण्णहा
			out.प्रक्षिप_back(c);
	}
	प्रत्यागम out;
}

शून्य append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	प्रत्यागम std::filesystem::path(path).filename().string();
}

std::string absolute_path(ध्रुव std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	जइ (ec)
		प्रत्यागम "";

	result = std::filesystem::weakly_canonical(result, ec);

	जइ (ec)
		प्रत्यागम "";

	प्रत्यागम result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	जइ (parent.empty()) {
		प्रत्यागम "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	जइ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	प्रत्यागम result;
}

शून्य format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	प्रति (size_t i = 0; i < fmt.size(); ++i) {
		वर्ण ch = fmt[i];
		result.प्रक्षिप_back(ch);
		जइ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	प्रत्यागम result;
}

स्थिर std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	प्रत्यागम result;
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
			जइ (spec[spec.size() - 3] == 'h')
				प्रत्यागम परिमाण(वर्ण);
			प्रत्यागम परिमाण(short);
		पक्ष 'l':
			जइ (spec[spec.size() - 3] == 'l')
				प्रत्यागम परिमाण(long long);
			प्रत्यागम परिमाण(long);
		पक्ष 'L':
		पक्ष 'q':
			प्रत्यागम परिमाण(long long);
		पक्ष 'j':
			प्रत्यागम परिमाण(intmax_t);
		पक्ष 'z':
		पक्ष 'Z':
			प्रत्यागम परिमाण(size_t);
		पक्ष 't':
			प्रत्यागम परिमाण(ptrdiff_t);
		}
		प्रत्यागम परिमाण(पूर्णाङ्क);
	पक्ष 'e':
	पक्ष 'E':
	पक्ष 'f':
	पक्ष 'F':
	पक्ष 'g':
	पक्ष 'G':
	पक्ष 'a':
	पक्ष 'A':
		जइ (spec[spec.size() - 2] == 'L')
			प्रत्यागम परिमाण(long द्विगुण);
		जइ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			प्रत्यागम परिमाण(long द्विगुण);
		प्रत्यागम परिमाण(द्विगुण);
	पक्ष 'c':
		जइ (spec[spec.size() - 2] == 'l') {
			प्रत्यागम परिमाण(wchar_t);
		}
		प्रत्यागम परिमाण(unsigned वर्ण);
	पक्ष 'C':
		प्रत्यागम परिमाण(wchar_t);
	पक्ष 's':
	पक्ष 'p':
	पक्ष 'S':
	पक्ष 'n':
		प्रत्यागम परिमाण(शून्य *);
	पक्ष 'm':
		प्रत्यागम परिमाण(पूर्णाङ्क);
	स्वभाव:
		प्रत्यागम -1;
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
		प्रत्यागम;
	पक्ष Dynamic_Int_गणनम्::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		प्रत्यागम;
	पक्ष Dynamic_Int_गणनम्::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		प्रत्यागम;
	}
	YOSYS_ABORT("Internal error");
}

शून्य format_emit_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, long long arg)
{
	जइ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<पूर्णाङ्क>(arg));
		प्रत्यागम;
	}
	जइ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, (पूर्णाङ्क)arg);
	} अण्णहा {
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
	}
}

शून्य format_emit_unsigned_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, unsigned long long arg)
{
	जइ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned पूर्णाङ्क>(arg));
		प्रत्यागम;
	}
	जइ (spec == "%c") {
		result += static_cast<वर्ण>(arg);
		प्रत्यागम;
	}
	जइ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, (unsigned पूर्णाङ्क)arg);
	} अण्णहा {
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
	जइ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		प्रत्यागम;
	}
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

शून्य format_emit_string(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव std::string &arg)
{
	जइ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		प्रत्यागम;
	}
	format_emit_stringf(result, spec, dynamic_ints, अङ्कः_dynamic_ints, arg.c_str());
}

शून्य format_emit_string_view(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, std::string_view arg)
{
	जइ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		प्रत्यागम;
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

#जइ defined(_WIN32)
ScopedFileLock::ScopedFileLock(ध्रुव std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			रिक्त, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, रिक्त);
	जइ (h == INVALID_HANDLE_मानम्) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		प्रत्यागम;
	}
	OVERLAPPED ov = {};
	जइ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		प्रत्यागम;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	जइ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#नोचेद्यदि !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ध्रुव std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	जइ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	जइ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		प्रत्यागम;
	}
	पूर्णाङ्क ret;
	जाव ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	जइ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	जइ (fd >= 0)
		close(fd); // releases the flock
}
#अण्णहा
ScopedFileLock::ScopedFileLock(ध्रुव std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#जइ defined(_WIN32)
	प्रत्यागम GetCurrentProcessId();
#नोचेद्यदि defined(__wasi__)
	प्रत्यागम 0;
#अण्णहा
	प्रत्यागम getpid();
#endif
}

YOSYS_NAMESPACE_END
