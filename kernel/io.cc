#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/log.h"
#അന്തര്ഭാവ <iostream>
#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <filesystem>
#അന്തര്ഭാവ <system_error>

#എങ്കിൽ !defined(WIN32)
#അന്തര്ഭാവ <dirent.h>
#അന്തര്ഭാവ <unistd.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <io.h>
#endif

#എങ്കിൽ !defined(_WIN32) && !defined(__wasi__)
#അന്തര്ഭാവ <sys/file.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

പൂർണ്ണസംഖ്യ readsome(std::istream &f, അക്ഷരം *s, പൂർണ്ണസംഖ്യ n)
{
	പൂർണ്ണസംഖ്യ rc = പൂർണ്ണസംഖ്യ(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	എങ്കിൽ (rc == 0) {
		പൂർണ്ണസംഖ്യ c = f.get();
		എങ്കിൽ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	തിരികെ rc;
}

std::string next_token(std::string &text, സ്ഥിരം അക്ഷരം *sep, തർക്കം long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	എങ്കിൽ (pos_begin == std::string::npos)
		pos_begin = text.size();

	എങ്കിൽ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		വേണ്ടി (size_t i = pos_begin+1; i < text.size(); i++) {
			എങ്കിൽ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				തിരികെ token;
			}
			എങ്കിൽ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				തിരികെ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	എങ്കിൽ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	തിരികെ token;
}

std::vector<std::string> split_tokens(സ്ഥിരം std::string &text, സ്ഥിരം അക്ഷരം *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	വേണ്ടി (അക്ഷരം c : text) {
		എങ്കിൽ (strchr(sep, c)) {
			എങ്കിൽ (!current_token.empty()) {
				tokens.തള്ളുക_back(current_token);
				current_token.clear();
			}
		} അല്ലെങ്കിൽ
			current_token += c;
	}
	എങ്കിൽ (!current_token.empty()) {
		tokens.തള്ളുക_back(current_token);
		current_token.clear();
	}
	തിരികെ tokens;
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
തർക്കം patmatch(സ്ഥിരം അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *string)
{
	എങ്കിൽ (*pattern == 0)
		തിരികെ *string == 0;

	എങ്കിൽ (*pattern == '\\') {
		എങ്കിൽ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			തിരികെ സത്യം;
	}

	എങ്കിൽ (*pattern == '?') {
		എങ്കിൽ (*string == 0)
			തിരികെ അസത്യം;
		തിരികെ patmatch(pattern+1, string+1);
	}

	എങ്കിൽ (*pattern == '*') {
		ആകുമ്പോൾ (*string) {
			എങ്കിൽ (patmatch(pattern+1, string++))
				തിരികെ സത്യം;
		}
		തിരികെ pattern[1] == 0;
	}

	എങ്കിൽ (*pattern == '[') {
		തർക്കം found_match = അസത്യം;
		തർക്കം inverted_list = pattern[1] == '!';
		സ്ഥിരം അക്ഷരം *p = pattern + (inverted_list ? 1 : 0);

		ആകുമ്പോൾ (*++p) {
			എങ്കിൽ (*p == ']') {
				എങ്കിൽ (found_match != inverted_list && patmatch(p+1, string+1))
					തിരികെ സത്യം;
				നിർത്തുക;
			}

			എങ്കിൽ (*p == '\\') {
				എങ്കിൽ (*++p == *string)
					found_match = സത്യം;
			} അല്ലെങ്കിൽ
			എങ്കിൽ (*p == *string)
				found_match = സത്യം;
		}
	}

	എങ്കിൽ (*pattern == *string)
		തിരികെ patmatch(pattern+1, string+1);

	തിരികെ അസത്യം;
}

std::string get_base_tmpdir()
{
	സ്ഥിതം std::string tmpdir;

	എങ്കിൽ (!tmpdir.empty()) {
		തിരികെ tmpdir;
	}

#എങ്കിൽ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	എങ്കിൽ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), ശൂന്യ, 0, ശൂന്യ, ശൂന്യ));
	എങ്കിൽ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), ശൂന്യ, ശൂന്യ) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#അല്ലെങ്കിൽ
	അക്ഷരം * var = std::getenv("TMPDIR");
	എങ്കിൽ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ആകുമ്പോൾ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.എടുക്കുക_back();
		}
	} അല്ലെങ്കിൽ {
		tmpdir.assign("/tmp");
	}
#endif
	തിരികെ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#എങ്കിൽ defined(__wasm)
	സ്ഥിതം size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ആകുമ്പോൾ (1) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 6; i++) {
			സ്ഥിതം std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			സ്ഥിതം uint32_t x = 314159265 ^ uint32_t(time(ശൂന്യ));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		എങ്കിൽ (_access(template_str.c_str(), 0) != 0)
			നിർത്തുക;
	}
#അല്ലെങ്കിൽ
	പൂർണ്ണസംഖ്യ suffixlen = template_str.size() - pos - 6;

	അക്ഷരം *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	സ്വതന്ത്രം(p);
#endif

	തിരികെ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#എങ്കിൽ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	തിരികെ template_str;
#അല്ലെങ്കിലെങ്കിൽ defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	തിരികെ template_str;
#അല്ലെങ്കിൽ
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	പൂർണ്ണസംഖ്യ suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	അക്ഷരം *p = strdup(template_str.c_str());
	log_assert(p);
	അക്ഷരം *res = mkdtemp(p);
	എങ്കിൽ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	സ്വതന്ത്രം(p);

	തിരികെ template_str;
#endif
}

തർക്കം check_is_directory(സ്ഥിരം std::string& dirname)
{
#എങ്കിൽ defined(_WIN32)
	ഘടന _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	എങ്കിൽ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	എങ്കിൽ (_stat(dirname_.c_str(), &info) != 0)
	{
		തിരികെ അസത്യം;
	}
	തിരികെ (info.st_mode & _S_IFDIR) != 0;
#അല്ലെങ്കിൽ
	ഘടന stat info;
	എങ്കിൽ (stat(dirname.c_str(), &info) != 0)
	{
		തിരികെ അസത്യം;
	}
	തിരികെ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
തർക്കം check_accessible(സ്ഥിരം std::string& filename, തർക്കം)
{
	തിരികെ _access(filename.c_str(), 0) == 0;
}
#അല്ലെങ്കിൽ
തർക്കം check_accessible(സ്ഥിരം std::string& filename, തർക്കം is_exec)
{
	തിരികെ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

തർക്കം check_file_exists(സ്ഥിരം std::string& filename, തർക്കം is_exec)
{
	തിരികെ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
തർക്കം check_directory_exists(സ്ഥിരം std::string& filename, തർക്കം is_exec)
{
	തിരികെ check_accessible(filename, is_exec) && check_is_directory(filename);
}

തർക്കം is_absolute_path(std::string filename)
{
#ifdef _WIN32
	തിരികെ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#അല്ലെങ്കിൽ
	തിരികെ filename[0] == '/';
#endif
}

ശൂന്യം remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#അല്ലെങ്കിൽ
	ഘടന stat stbuf;
	ഘടന dirent **namelist;
	പൂർണ്ണസംഖ്യ n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n; i++) {
		എങ്കിൽ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			എങ്കിൽ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} അല്ലെങ്കിൽ
				remove_directory(buffer);
		}
		സ്വതന്ത്രം(namelist[i]);
	}
	സ്വതന്ത്രം(namelist);
	rmdir(dirname.c_str());
#endif
}

തർക്കം create_directory(സ്ഥിരം std::string& dirname)
{
#എങ്കിൽ defined(_WIN32)
	പൂർണ്ണസംഖ്യ ret = _mkdir(dirname.c_str());
#അല്ലെങ്കിൽ
	mode_t mode = 0755;
	പൂർണ്ണസംഖ്യ ret = mkdir(dirname.c_str(), mode);
#endif
	എങ്കിൽ (ret == 0)
		തിരികെ സത്യം;

	തിരഞ്ഞെടുപ്പ് (errno)
	{
	അവസ്ഥ ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			എങ്കിൽ (pos == std::string::npos)
#എങ്കിൽ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			എങ്കിൽ (pos == std::string::npos)
#endif
				തിരികെ അസത്യം;
			എങ്കിൽ (!create_directory( dirname.substr(0, pos) ))
				തിരികെ അസത്യം;
		}
		// now, try to create again
#എങ്കിൽ defined(_WIN32)
		തിരികെ 0 == _mkdir(dirname.c_str());
#അല്ലെങ്കിൽ
		തിരികെ 0 == mkdir(dirname.c_str(), mode);
#endif

	അവസ്ഥ EEXIST:
		// done!
		തിരികെ check_directory_exists(dirname);

	സ്ഥിരസ്ഥിതി:
		തിരികെ അസത്യം;
	}
}

തർക്കം make_private_directory(സ്ഥിരം std::string& dirname)
{
#എങ്കിൽ defined(_WIN32) || defined(__wasi__)
	എങ്കിൽ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		തിരികെ അസത്യം;
	}
	തിരികെ സത്യം;
#അല്ലെങ്കിൽ
	എങ്കിൽ (mkdir(dirname.c_str(), 0700) == 0)
		തിരികെ സത്യം;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	പൂർണ്ണസംഖ്യ err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	എങ്കിൽ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		എങ്കിൽ (mkdir(dirname.c_str(), 0700) == 0)
			തിരികെ സത്യം;
		err = errno;
	}

	എങ്കിൽ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		തിരികെ അസത്യം;
	}

	// Do not use a leftover that isn't ours
	ഘടന stat st;
	എങ്കിൽ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		തിരികെ അസത്യം;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	എങ്കിൽ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		തിരികെ അസത്യം;
	}

	തിരികെ സത്യം;
#endif
}

std::string make_user_tmpdir(സ്ഥിരം std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#എങ്കിൽ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#അല്ലെങ്കിൽ
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	തിരികെ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(സ്ഥിരം std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	വേണ്ടി (auto c : filename)
	{
		എങ്കിൽ (c == ' ')
			out += "\\ ";
		അല്ലെങ്കിൽ
			out.തള്ളുക_back(c);
	}
	തിരികെ out;
}

ശൂന്യം append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	തിരികെ std::filesystem::path(path).filename().string();
}

std::string absolute_path(സ്ഥിരം std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	എങ്കിൽ (ec)
		തിരികെ "";

	result = std::filesystem::weakly_canonical(result, ec);

	എങ്കിൽ (ec)
		തിരികെ "";

	തിരികെ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	എങ്കിൽ (parent.empty()) {
		തിരികെ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	എങ്കിൽ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	തിരികെ result;
}

ശൂന്യം format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	വേണ്ടി (size_t i = 0; i < fmt.size(); ++i) {
		അക്ഷരം ch = fmt[i];
		result.തള്ളുക_back(ch);
		എങ്കിൽ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	തിരികെ result;
}

സ്ഥിതം std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	തിരഞ്ഞെടുപ്പ് (spec[spec.size() - 1]) {
	അവസ്ഥ 'd':
	അവസ്ഥ 'i':
	അവസ്ഥ 'o':
	അവസ്ഥ 'u':
	അവസ്ഥ 'x':
	അവസ്ഥ 'X':
		തിരഞ്ഞെടുപ്പ് (spec[spec.size() - 2]) {
		അവസ്ഥ 'h':
			എങ്കിൽ (spec[spec.size() - 3] == 'h')
				തിരികെ വലുപ്പം(അക്ഷരം);
			തിരികെ വലുപ്പം(short);
		അവസ്ഥ 'l':
			എങ്കിൽ (spec[spec.size() - 3] == 'l')
				തിരികെ വലുപ്പം(long long);
			തിരികെ വലുപ്പം(long);
		അവസ്ഥ 'L':
		അവസ്ഥ 'q':
			തിരികെ വലുപ്പം(long long);
		അവസ്ഥ 'j':
			തിരികെ വലുപ്പം(intmax_t);
		അവസ്ഥ 'z':
		അവസ്ഥ 'Z':
			തിരികെ വലുപ്പം(size_t);
		അവസ്ഥ 't':
			തിരികെ വലുപ്പം(ptrdiff_t);
		}
		തിരികെ വലുപ്പം(പൂർണ്ണസംഖ്യ);
	അവസ്ഥ 'e':
	അവസ്ഥ 'E':
	അവസ്ഥ 'f':
	അവസ്ഥ 'F':
	അവസ്ഥ 'g':
	അവസ്ഥ 'G':
	അവസ്ഥ 'a':
	അവസ്ഥ 'A':
		എങ്കിൽ (spec[spec.size() - 2] == 'L')
			തിരികെ വലുപ്പം(long ഇരട്ട);
		എങ്കിൽ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			തിരികെ വലുപ്പം(long ഇരട്ട);
		തിരികെ വലുപ്പം(ഇരട്ട);
	അവസ്ഥ 'c':
		എങ്കിൽ (spec[spec.size() - 2] == 'l') {
			തിരികെ വലുപ്പം(wchar_t);
		}
		തിരികെ വലുപ്പം(unsigned അക്ഷരം);
	അവസ്ഥ 'C':
		തിരികെ വലുപ്പം(wchar_t);
	അവസ്ഥ 's':
	അവസ്ഥ 'p':
	അവസ്ഥ 'S':
	അവസ്ഥ 'n':
		തിരികെ വലുപ്പം(ശൂന്യം *);
	അവസ്ഥ 'm':
		തിരികെ വലുപ്പം(പൂർണ്ണസംഖ്യ);
	സ്ഥിരസ്ഥിതി:
		തിരികെ -1;
	}
}

template <typename Arg>
സ്ഥിതം ശൂന്യം format_emit_stringf(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	തിരഞ്ഞെടുപ്പ് (അക്കം_dynamic_ints) {
	അവസ്ഥ Dynamic_Int_എണ്ണം::NONE:
		result += string_view_stringf(spec, arg);
		തിരികെ;
	അവസ്ഥ Dynamic_Int_എണ്ണം::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		തിരികെ;
	അവസ്ഥ Dynamic_Int_എണ്ണം::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		തിരികെ;
	}
	YOSYS_ABORT("Internal error");
}

ശൂന്യം format_emit_long_long(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, long long arg)
{
	എങ്കിൽ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<പൂർണ്ണസംഖ്യ>(arg));
		തിരികെ;
	}
	എങ്കിൽ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, (പൂർണ്ണസംഖ്യ)arg);
	} അല്ലെങ്കിൽ {
		format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, arg);
	}
}

ശൂന്യം format_emit_unsigned_long_long(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, unsigned long long arg)
{
	എങ്കിൽ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned പൂർണ്ണസംഖ്യ>(arg));
		തിരികെ;
	}
	എങ്കിൽ (spec == "%c") {
		result += static_cast<അക്ഷരം>(arg);
		തിരികെ;
	}
	എങ്കിൽ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, (unsigned പൂർണ്ണസംഖ്യ)arg);
	} അല്ലെങ്കിൽ {
		format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, arg);
	}
}

ശൂന്യം format_emit_double(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, ഇരട്ട arg)
{
	format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, arg);
}

ശൂന്യം format_emit_char_ptr(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, സ്ഥിരം അക്ഷരം *arg)
{
	എങ്കിൽ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		തിരികെ;
	}
	format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, arg);
}

ശൂന്യം format_emit_string(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, സ്ഥിരം std::string &arg)
{
	എങ്കിൽ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		തിരികെ;
	}
	format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, arg.c_str());
}

ശൂന്യം format_emit_string_view(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, std::string_view arg)
{
	എങ്കിൽ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		തിരികെ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, std::string(arg).c_str());
}

ശൂന്യം format_emit_void_ptr(std::string &result, std::string_view spec, പൂർണ്ണസംഖ്യ *dynamic_ints,
	Dynamic_Int_എണ്ണം അക്കം_dynamic_ints, സ്ഥിരം ശൂന്യം *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, അക്കം_dynamic_ints, arg);
}

#എങ്കിൽ defined(_WIN32)
ScopedFileLock::ScopedFileLock(സ്ഥിരം std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ശൂന്യ, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, ശൂന്യ);
	എങ്കിൽ (h == INVALID_HANDLE_മൂല്യം) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		തിരികെ;
	}
	OVERLAPPED ov = {};
	എങ്കിൽ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		തിരികെ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	എങ്കിൽ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#അല്ലെങ്കിലെങ്കിൽ !defined(__wasi__)
ScopedFileLock::ScopedFileLock(സ്ഥിരം std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	എങ്കിൽ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	എങ്കിൽ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		തിരികെ;
	}
	പൂർണ്ണസംഖ്യ ret;
	ആകുമ്പോൾ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	എങ്കിൽ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	എങ്കിൽ (fd >= 0)
		close(fd); // releases the flock
}
#അല്ലെങ്കിൽ
ScopedFileLock::ScopedFileLock(സ്ഥിരം std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#എങ്കിൽ defined(_WIN32)
	തിരികെ GetCurrentProcessId();
#അല്ലെങ്കിലെങ്കിൽ defined(__wasi__)
	തിരികെ 0;
#അല്ലെങ്കിൽ
	തിരികെ getpid();
#endif
}

YOSYS_NAMESPACE_END
