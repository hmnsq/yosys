#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <filesystem>
#ಅನ್ತರ್ಭಾವ <system_error>

#ಯದಿ !defined(WIN32)
#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <io.h>
#endif

#ಯದಿ !defined(_WIN32) && !defined(__wasi__)
#ಅನ್ತರ್ಭಾವ <sys/file.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ಪೂರ್ಣಾಙ್ಕ readsome(std::istream &f, ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ n)
{
	ಪೂರ್ಣಾಙ್ಕ rc = ಪೂರ್ಣಾಙ್ಕ(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ಯದಿ (rc == 0) {
		ಪೂರ್ಣಾಙ್ಕ c = f.get();
		ಯದಿ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}

std::string next_token(std::string &text, ಧ್ರುವ ವರ್ಣ *sep, ತಾರ್ಕಿಕ long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ಯದಿ (pos_begin == std::string::npos)
		pos_begin = text.size();

	ಯದಿ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		ಪ್ರತಿ (size_t i = pos_begin+1; i < text.size(); i++) {
			ಯದಿ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ಪ್ರತ್ಯಾಗಮ token;
			}
			ಯದಿ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ಪ್ರತ್ಯಾಗಮ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ಯದಿ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ಪ್ರತ್ಯಾಗಮ token;
}

std::vector<std::string> split_tokens(ಧ್ರುವ std::string &text, ಧ್ರುವ ವರ್ಣ *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	ಪ್ರತಿ (ವರ್ಣ c : text) {
		ಯದಿ (strchr(sep, c)) {
			ಯದಿ (!current_token.empty()) {
				tokens.ತಳ್ಳು_back(current_token);
				current_token.clear();
			}
		} ಅನ್ಯಥಾ
			current_token += c;
	}
	ಯದಿ (!current_token.empty()) {
		tokens.ತಳ್ಳು_back(current_token);
		current_token.clear();
	}
	ಪ್ರತ್ಯಾಗಮ tokens;
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
ತಾರ್ಕಿಕ patmatch(ಧ್ರುವ ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *string)
{
	ಯದಿ (*pattern == 0)
		ಪ್ರತ್ಯಾಗಮ *string == 0;

	ಯದಿ (*pattern == '\\') {
		ಯದಿ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (*pattern == '?') {
		ಯದಿ (*string == 0)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ patmatch(pattern+1, string+1);
	}

	ಯದಿ (*pattern == '*') {
		ಯಾವತ್ (*string) {
			ಯದಿ (patmatch(pattern+1, string++))
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}
		ಪ್ರತ್ಯಾಗಮ pattern[1] == 0;
	}

	ಯದಿ (*pattern == '[') {
		ತಾರ್ಕಿಕ found_match = ಅಸತ್ಯ;
		ತಾರ್ಕಿಕ inverted_list = pattern[1] == '!';
		ಧ್ರುವ ವರ್ಣ *p = pattern + (inverted_list ? 1 : 0);

		ಯಾವತ್ (*++p) {
			ಯದಿ (*p == ']') {
				ಯದಿ (found_match != inverted_list && patmatch(p+1, string+1))
					ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
				ಭಙ್ಗ;
			}

			ಯದಿ (*p == '\\') {
				ಯದಿ (*++p == *string)
					found_match = ಸತ್ಯ;
			} ಅನ್ಯಥಾ
			ಯದಿ (*p == *string)
				found_match = ಸತ್ಯ;
		}
	}

	ಯದಿ (*pattern == *string)
		ಪ್ರತ್ಯಾಗಮ patmatch(pattern+1, string+1);

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

std::string get_base_tmpdir()
{
	ಸ್ಥಿರ std::string tmpdir;

	ಯದಿ (!tmpdir.empty()) {
		ಪ್ರತ್ಯಾಗಮ tmpdir;
	}

#ಯದಿ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ಯದಿ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ));
	ಯದಿ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), ರಿಕ್ತ, ರಿಕ್ತ) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ಅನ್ಯಥಾ
	ವರ್ಣ * var = std::getenv("TMPDIR");
	ಯದಿ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ಯಾವತ್ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.ತೆಗೆ_back();
		}
	} ಅನ್ಯಥಾ {
		tmpdir.assign("/tmp");
	}
#endif
	ಪ್ರತ್ಯಾಗಮ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ಯದಿ defined(__wasm)
	ಸ್ಥಿರ size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#ನೋಚೇದ್ಯದಿ defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ಯಾವತ್ (1) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 6; i++) {
			ಸ್ಥಿರ std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ಸ್ಥಿರ uint32_t x = 314159265 ^ uint32_t(time(ರಿಕ್ತ));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ಯದಿ (_access(template_str.c_str(), 0) != 0)
			ಭಙ್ಗ;
	}
#ಅನ್ಯಥಾ
	ಪೂರ್ಣಾಙ್ಕ suffixlen = template_str.size() - pos - 6;

	ವರ್ಣ *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	ಮುಕ್ತ(p);
#endif

	ಪ್ರತ್ಯಾಗಮ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ಯದಿ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ಪ್ರತ್ಯಾಗಮ template_str;
#ನೋಚೇದ್ಯದಿ defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ಪ್ರತ್ಯಾಗಮ template_str;
#ಅನ್ಯಥಾ
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ಪೂರ್ಣಾಙ್ಕ suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	ವರ್ಣ *p = strdup(template_str.c_str());
	log_assert(p);
	ವರ್ಣ *res = mkdtemp(p);
	ಯದಿ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	ಮುಕ್ತ(p);

	ಪ್ರತ್ಯಾಗಮ template_str;
#endif
}

ತಾರ್ಕಿಕ check_is_directory(ಧ್ರುವ std::string& dirname)
{
#ಯದಿ defined(_WIN32)
	ಸಂರಚನಾ _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ಯದಿ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ಯದಿ (_stat(dirname_.c_str(), &info) != 0)
	{
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ (info.st_mode & _S_IFDIR) != 0;
#ಅನ್ಯಥಾ
	ಸಂರಚನಾ stat info;
	ಯದಿ (stat(dirname.c_str(), &info) != 0)
	{
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
ತಾರ್ಕಿಕ check_accessible(ಧ್ರುವ std::string& filename, ತಾರ್ಕಿಕ)
{
	ಪ್ರತ್ಯಾಗಮ _access(filename.c_str(), 0) == 0;
}
#ಅನ್ಯಥಾ
ತಾರ್ಕಿಕ check_accessible(ಧ್ರುವ std::string& filename, ತಾರ್ಕಿಕ is_exec)
{
	ಪ್ರತ್ಯಾಗಮ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

ತಾರ್ಕಿಕ check_file_exists(ಧ್ರುವ std::string& filename, ತಾರ್ಕಿಕ is_exec)
{
	ಪ್ರತ್ಯಾಗಮ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
ತಾರ್ಕಿಕ check_directory_exists(ಧ್ರುವ std::string& filename, ತಾರ್ಕಿಕ is_exec)
{
	ಪ್ರತ್ಯಾಗಮ check_accessible(filename, is_exec) && check_is_directory(filename);
}

ತಾರ್ಕಿಕ is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ಪ್ರತ್ಯಾಗಮ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ filename[0] == '/';
#endif
}

ಶೂನ್ಯ remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#ಅನ್ಯಥಾ
	ಸಂರಚನಾ stat stbuf;
	ಸಂರಚನಾ dirent **namelist;
	ಪೂರ್ಣಾಙ್ಕ n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n; i++) {
		ಯದಿ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ಯದಿ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} ಅನ್ಯಥಾ
				remove_directory(buffer);
		}
		ಮುಕ್ತ(namelist[i]);
	}
	ಮುಕ್ತ(namelist);
	rmdir(dirname.c_str());
#endif
}

ತಾರ್ಕಿಕ create_directory(ಧ್ರುವ std::string& dirname)
{
#ಯದಿ defined(_WIN32)
	ಪೂರ್ಣಾಙ್ಕ ret = _mkdir(dirname.c_str());
#ಅನ್ಯಥಾ
	mode_t mode = 0755;
	ಪೂರ್ಣಾಙ್ಕ ret = mkdir(dirname.c_str(), mode);
#endif
	ಯದಿ (ret == 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

	ವಿಕಲ್ಪ (errno)
	{
	ಪಕ್ಷ ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ಯದಿ (pos == std::string::npos)
#ಯದಿ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ಯದಿ (pos == std::string::npos)
#endif
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (!create_directory( dirname.substr(0, pos) ))
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}
		// now, try to create again
#ಯದಿ defined(_WIN32)
		ಪ್ರತ್ಯಾಗಮ 0 == _mkdir(dirname.c_str());
#ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ 0 == mkdir(dirname.c_str(), mode);
#endif

	ಪಕ್ಷ EEXIST:
		// done!
		ಪ್ರತ್ಯಾಗಮ check_directory_exists(dirname);

	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
}

ತಾರ್ಕಿಕ make_private_directory(ಧ್ರುವ std::string& dirname)
{
#ಯದಿ defined(_WIN32) || defined(__wasi__)
	ಯದಿ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
#ಅನ್ಯಥಾ
	ಯದಿ (mkdir(dirname.c_str(), 0700) == 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ಪೂರ್ಣಾಙ್ಕ err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ಯದಿ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ಯದಿ (mkdir(dirname.c_str(), 0700) == 0)
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		err = errno;
	}

	ಯದಿ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	// Do not use a leftover that isn't ours
	ಸಂರಚನಾ stat st;
	ಯದಿ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ಯದಿ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
#endif
}

std::string make_user_tmpdir(ಧ್ರುವ std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ಯದಿ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#ಅನ್ಯಥಾ
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ಪ್ರತ್ಯಾಗಮ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ಧ್ರುವ std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	ಪ್ರತಿ (auto c : filename)
	{
		ಯದಿ (c == ' ')
			out += "\\ ";
		ಅನ್ಯಥಾ
			out.ತಳ್ಳು_back(c);
	}
	ಪ್ರತ್ಯಾಗಮ out;
}

ಶೂನ್ಯ append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ಪ್ರತ್ಯಾಗಮ std::filesystem::path(path).filename().string();
}

std::string absolute_path(ಧ್ರುವ std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ಯದಿ (ec)
		ಪ್ರತ್ಯಾಗಮ "";

	result = std::filesystem::weakly_canonical(result, ec);

	ಯದಿ (ec)
		ಪ್ರತ್ಯಾಗಮ "";

	ಪ್ರತ್ಯಾಗಮ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ಯದಿ (parent.empty()) {
		ಪ್ರತ್ಯಾಗಮ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ಯದಿ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	ಪ್ರತಿ (size_t i = 0; i < fmt.size(); ++i) {
		ವರ್ಣ ch = fmt[i];
		result.ತಳ್ಳು_back(ch);
		ಯದಿ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	ವಿಕಲ್ಪ (spec[spec.size() - 1]) {
	ಪಕ್ಷ 'd':
	ಪಕ್ಷ 'i':
	ಪಕ್ಷ 'o':
	ಪಕ್ಷ 'u':
	ಪಕ್ಷ 'x':
	ಪಕ್ಷ 'X':
		ವಿಕಲ್ಪ (spec[spec.size() - 2]) {
		ಪಕ್ಷ 'h':
			ಯದಿ (spec[spec.size() - 3] == 'h')
				ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ವರ್ಣ);
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(short);
		ಪಕ್ಷ 'l':
			ಯದಿ (spec[spec.size() - 3] == 'l')
				ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(long long);
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(long);
		ಪಕ್ಷ 'L':
		ಪಕ್ಷ 'q':
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(long long);
		ಪಕ್ಷ 'j':
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(intmax_t);
		ಪಕ್ಷ 'z':
		ಪಕ್ಷ 'Z':
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(size_t);
		ಪಕ್ಷ 't':
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ptrdiff_t);
		}
		ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
	ಪಕ್ಷ 'e':
	ಪಕ್ಷ 'E':
	ಪಕ್ಷ 'f':
	ಪಕ್ಷ 'F':
	ಪಕ್ಷ 'g':
	ಪಕ್ಷ 'G':
	ಪಕ್ಷ 'a':
	ಪಕ್ಷ 'A':
		ಯದಿ (spec[spec.size() - 2] == 'L')
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(long ದ್ವಿಗುಣ);
		ಯದಿ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(long ದ್ವಿಗುಣ);
		ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ದ್ವಿಗುಣ);
	ಪಕ್ಷ 'c':
		ಯದಿ (spec[spec.size() - 2] == 'l') {
			ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(wchar_t);
		}
		ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(unsigned ವರ್ಣ);
	ಪಕ್ಷ 'C':
		ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(wchar_t);
	ಪಕ್ಷ 's':
	ಪಕ್ಷ 'p':
	ಪಕ್ಷ 'S':
	ಪಕ್ಷ 'n':
		ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಶೂನ್ಯ *);
	ಪಕ್ಷ 'm':
		ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ -1;
	}
}

template <typename Arg>
ಸ್ಥಿರ ಶೂನ್ಯ format_emit_stringf(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	ವಿಕಲ್ಪ (ಅಂಕೆ_dynamic_ints) {
	ಪಕ್ಷ Dynamic_Int_ಎಣಿಕೆ::NONE:
		result += string_view_stringf(spec, arg);
		ಪ್ರತ್ಯಾಗಮ;
	ಪಕ್ಷ Dynamic_Int_ಎಣಿಕೆ::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ಪ್ರತ್ಯಾಗಮ;
	ಪಕ್ಷ Dynamic_Int_ಎಣಿಕೆ::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ಪ್ರತ್ಯಾಗಮ;
	}
	YOSYS_ABORT("Internal error");
}

ಶೂನ್ಯ format_emit_long_long(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, long long arg)
{
	ಯದಿ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<ಪೂರ್ಣಾಙ್ಕ>(arg));
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, (ಪೂರ್ಣಾಙ್ಕ)arg);
	} ಅನ್ಯಥಾ {
		format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
	}
}

ಶೂನ್ಯ format_emit_unsigned_long_long(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, unsigned long long arg)
{
	ಯದಿ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned ಪೂರ್ಣಾಙ್ಕ>(arg));
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (spec == "%c") {
		result += static_cast<ವರ್ಣ>(arg);
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, (unsigned ಪೂರ್ಣಾಙ್ಕ)arg);
	} ಅನ್ಯಥಾ {
		format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
	}
}

ಶೂನ್ಯ format_emit_double(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ದ್ವಿಗುಣ arg)
{
	format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
}

ಶೂನ್ಯ format_emit_char_ptr(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಧ್ರುವ ವರ್ಣ *arg)
{
	ಯದಿ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ಪ್ರತ್ಯಾಗಮ;
	}
	format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
}

ಶೂನ್ಯ format_emit_string(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಧ್ರುವ std::string &arg)
{
	ಯದಿ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ಪ್ರತ್ಯಾಗಮ;
	}
	format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg.c_str());
}

ಶೂನ್ಯ format_emit_string_view(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, std::string_view arg)
{
	ಯದಿ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ಪ್ರತ್ಯಾಗಮ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, std::string(arg).c_str());
}

ಶೂನ್ಯ format_emit_void_ptr(std::string &result, std::string_view spec, ಪೂರ್ಣಾಙ್ಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಧ್ರುವ ಶೂನ್ಯ *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
}

#ಯದಿ defined(_WIN32)
ScopedFileLock::ScopedFileLock(ಧ್ರುವ std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ರಿಕ್ತ, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, ರಿಕ್ತ);
	ಯದಿ (h == INVALID_HANDLE_ಮೌಲ್ಯ) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ಪ್ರತ್ಯಾಗಮ;
	}
	OVERLAPPED ov = {};
	ಯದಿ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ಪ್ರತ್ಯಾಗಮ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ಯದಿ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#ನೋಚೇದ್ಯದಿ !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ಧ್ರುವ std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ಯದಿ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ಯದಿ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಯಾವತ್ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ಯದಿ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ಯದಿ (fd >= 0)
		close(fd); // releases the flock
}
#ಅನ್ಯಥಾ
ScopedFileLock::ScopedFileLock(ಧ್ರುವ std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ಯದಿ defined(_WIN32)
	ಪ್ರತ್ಯಾಗಮ GetCurrentProcessId();
#ನೋಚೇದ್ಯದಿ defined(__wasi__)
	ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ getpid();
#endif
}

YOSYS_NAMESPACE_END
