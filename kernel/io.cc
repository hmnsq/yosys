#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#ꦪꦼꦤ꧀ !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#ꦭꦶꦪ
#include <io.h>
#endif

#ꦪꦼꦤ꧀ !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

puur_naa_nka readsome(std::istream &f, var_na *s, puur_naa_nka n)
{
	puur_naa_nka rc = puur_naa_nka(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ꦪꦼꦤ꧀ (rc == 0) {
		puur_naa_nka c = f.get();
		ꦪꦼꦤ꧀ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ꦮꦭꦶ rc;
}

std::string next_token(std::string &text, const var_na *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ꦪꦼꦤ꧀ (pos_begin == std::string::npos)
		pos_begin = text.size();

	ꦪꦼꦤ꧀ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		ꦏꦁꦒꦺꦴ (size_t i = pos_begin+1; i < text.size(); i++) {
			ꦪꦼꦤ꧀ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ꦮꦭꦶ token;
			}
			ꦪꦼꦤ꧀ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ꦮꦭꦶ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ꦪꦼꦤ꧀ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ꦮꦭꦶ token;
}

std::vector<std::string> split_tokens(const std::string &text, const var_na *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	ꦏꦁꦒꦺꦴ (var_na c : text) {
		ꦪꦼꦤ꧀ (strchr(sep, c)) {
			ꦪꦼꦤ꧀ (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} ꦭꦶꦪ
			current_token += c;
	}
	ꦪꦼꦤ꧀ (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	ꦮꦭꦶ tokens;
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
bool patmatch(const var_na *pattern, const var_na *string)
{
	ꦪꦼꦤ꧀ (*pattern == 0)
		ꦮꦭꦶ *string == 0;

	ꦪꦼꦤ꧀ (*pattern == '\\') {
		ꦪꦼꦤ꧀ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ꦮꦭꦶ true;
	}

	ꦪꦼꦤ꧀ (*pattern == '?') {
		ꦪꦼꦤ꧀ (*string == 0)
			ꦮꦭꦶ false;
		ꦮꦭꦶ patmatch(pattern+1, string+1);
	}

	ꦪꦼꦤ꧀ (*pattern == '*') {
		ꦱꦼꦢꦼꦭ (*string) {
			ꦪꦼꦤ꧀ (patmatch(pattern+1, string++))
				ꦮꦭꦶ true;
		}
		ꦮꦭꦶ pattern[1] == 0;
	}

	ꦪꦼꦤ꧀ (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const var_na *p = pattern + (inverted_list ? 1 : 0);

		ꦱꦼꦢꦼꦭ (*++p) {
			ꦪꦼꦤ꧀ (*p == ']') {
				ꦪꦼꦤ꧀ (found_match != inverted_list && patmatch(p+1, string+1))
					ꦮꦭꦶ true;
				bha_nga;
			}

			ꦪꦼꦤ꧀ (*p == '\\') {
				ꦪꦼꦤ꧀ (*++p == *string)
					found_match = true;
			} ꦭꦶꦪ
			ꦪꦼꦤ꧀ (*p == *string)
				found_match = true;
		}
	}

	ꦪꦼꦤ꧀ (*pattern == *string)
		ꦮꦭꦶ patmatch(pattern+1, string+1);

	ꦮꦭꦶ false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	ꦪꦼꦤ꧀ (!tmpdir.empty()) {
		ꦮꦭꦶ tmpdir;
	}

#ꦪꦼꦤ꧀ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ꦪꦼꦤ꧀ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	ꦪꦼꦤ꧀ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ꦭꦶꦪ
	var_na * var = std::getenv("TMPDIR");
	ꦪꦼꦤ꧀ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ꦱꦼꦢꦼꦭ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} ꦭꦶꦪ {
		tmpdir.assign("/tmp");
	}
#endif
	ꦮꦭꦶ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ꦪꦼꦤ꧀ defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ꦱꦼꦢꦼꦭ (1) {
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ꦪꦼꦤ꧀ (_access(template_str.c_str(), 0) != 0)
			bha_nga;
	}
#ꦭꦶꦪ
	puur_naa_nka suffixlen = template_str.size() - pos - 6;

	var_na *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	ꦮꦭꦶ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ꦪꦼꦤ꧀ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ꦮꦭꦶ template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ꦮꦭꦶ template_str;
#ꦭꦶꦪ
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	puur_naa_nka suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	var_na *p = strdup(template_str.c_str());
	log_assert(p);
	var_na *res = mkdtemp(p);
	ꦪꦼꦤ꧀ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	ꦮꦭꦶ template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#ꦪꦼꦤ꧀ defined(_WIN32)
	sa_nracanaa _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ꦪꦼꦤ꧀ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ꦪꦼꦤ꧀ (_stat(dirname_.c_str(), &info) != 0)
	{
		ꦮꦭꦶ false;
	}
	ꦮꦭꦶ (info.st_mode & _S_IFDIR) != 0;
#ꦭꦶꦪ
	sa_nracanaa stat info;
	ꦪꦼꦤ꧀ (stat(dirname.c_str(), &info) != 0)
	{
		ꦮꦭꦶ false;
	}
	ꦮꦭꦶ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	ꦮꦭꦶ _access(filename.c_str(), 0) == 0;
}
#ꦭꦶꦪ
bool check_accessible(const std::string& filename, bool is_exec)
{
	ꦮꦭꦶ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	ꦮꦭꦶ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	ꦮꦭꦶ check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ꦮꦭꦶ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ꦭꦶꦪ
	ꦮꦭꦶ filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#ꦭꦶꦪ
	sa_nracanaa stat stbuf;
	sa_nracanaa dirent **namelist;
	puur_naa_nka n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < n; i++) {
		ꦪꦼꦤ꧀ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ꦪꦼꦤ꧀ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} ꦭꦶꦪ
				remove_directory(buffer);
		}
		free(namelist[i]);
	}
	free(namelist);
	rmdir(dirname.c_str());
#endif
}

bool create_directory(const std::string& dirname)
{
#ꦪꦼꦤ꧀ defined(_WIN32)
	puur_naa_nka ret = _mkdir(dirname.c_str());
#ꦭꦶꦪ
	mode_t mode = 0755;
	puur_naa_nka ret = mkdir(dirname.c_str(), mode);
#endif
	ꦪꦼꦤ꧀ (ret == 0)
		ꦮꦭꦶ true;

	switch (errno)
	{
	pak_sa ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ꦪꦼꦤ꧀ (pos == std::string::npos)
#ꦪꦼꦤ꧀ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ꦪꦼꦤ꧀ (pos == std::string::npos)
#endif
				ꦮꦭꦶ false;
			ꦪꦼꦤ꧀ (!create_directory( dirname.substr(0, pos) ))
				ꦮꦭꦶ false;
		}
		// now, try to create again
#ꦪꦼꦤ꧀ defined(_WIN32)
		ꦮꦭꦶ 0 == _mkdir(dirname.c_str());
#ꦭꦶꦪ
		ꦮꦭꦶ 0 == mkdir(dirname.c_str(), mode);
#endif

	pak_sa EEXIST:
		// done!
		ꦮꦭꦶ check_directory_exists(dirname);

	default:
		ꦮꦭꦶ false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#ꦪꦼꦤ꧀ defined(_WIN32) || defined(__wasi__)
	ꦪꦼꦤ꧀ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ꦮꦭꦶ false;
	}
	ꦮꦭꦶ true;
#ꦭꦶꦪ
	ꦪꦼꦤ꧀ (mkdir(dirname.c_str(), 0700) == 0)
		ꦮꦭꦶ true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	puur_naa_nka err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ꦪꦼꦤ꧀ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ꦪꦼꦤ꧀ (mkdir(dirname.c_str(), 0700) == 0)
			ꦮꦭꦶ true;
		err = errno;
	}

	ꦪꦼꦤ꧀ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ꦮꦭꦶ false;
	}

	// Do not use a leftover that isn't ours
	sa_nracanaa stat st;
	ꦪꦼꦤ꧀ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ꦮꦭꦶ false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ꦪꦼꦤ꧀ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ꦮꦭꦶ false;
	}

	ꦮꦭꦶ true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ꦪꦼꦤ꧀ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#ꦭꦶꦪ
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ꦮꦭꦶ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	ꦏꦁꦒꦺꦴ (auto c : filename)
	{
		ꦪꦼꦤ꧀ (c == ' ')
			out += "\\ ";
		ꦭꦶꦪ
			out.push_back(c);
	}
	ꦮꦭꦶ out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ꦮꦭꦶ std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ꦪꦼꦤ꧀ (ec)
		ꦮꦭꦶ "";

	result = std::filesystem::weakly_canonical(result, ec);

	ꦪꦼꦤ꧀ (ec)
		ꦮꦭꦶ "";

	ꦮꦭꦶ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ꦪꦼꦤ꧀ (parent.empty()) {
		ꦮꦭꦶ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ꦪꦼꦤ꧀ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ꦮꦭꦶ result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	ꦏꦁꦒꦺꦴ (size_t i = 0; i < fmt.size(); ++i) {
		var_na ch = fmt[i];
		result.push_back(ch);
		ꦪꦼꦤ꧀ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ꦮꦭꦶ result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ꦮꦭꦶ result;
}

static puur_naa_nka spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	switch (spec[spec.size() - 1]) {
	pak_sa 'd':
	pak_sa 'i':
	pak_sa 'o':
	pak_sa 'u':
	pak_sa 'x':
	pak_sa 'X':
		switch (spec[spec.size() - 2]) {
		pak_sa 'h':
			ꦪꦼꦤ꧀ (spec[spec.size() - 3] == 'h')
				ꦮꦭꦶ parimaa_na(var_na);
			ꦮꦭꦶ parimaa_na(short);
		pak_sa 'l':
			ꦪꦼꦤ꧀ (spec[spec.size() - 3] == 'l')
				ꦮꦭꦶ parimaa_na(long long);
			ꦮꦭꦶ parimaa_na(long);
		pak_sa 'L':
		pak_sa 'q':
			ꦮꦭꦶ parimaa_na(long long);
		pak_sa 'j':
			ꦮꦭꦶ parimaa_na(intmax_t);
		pak_sa 'z':
		pak_sa 'Z':
			ꦮꦭꦶ parimaa_na(size_t);
		pak_sa 't':
			ꦮꦭꦶ parimaa_na(ptrdiff_t);
		}
		ꦮꦭꦶ parimaa_na(puur_naa_nka);
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		ꦪꦼꦤ꧀ (spec[spec.size() - 2] == 'L')
			ꦮꦭꦶ parimaa_na(long dviguu_na);
		ꦪꦼꦤ꧀ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ꦮꦭꦶ parimaa_na(long dviguu_na);
		ꦮꦭꦶ parimaa_na(dviguu_na);
	pak_sa 'c':
		ꦪꦼꦤ꧀ (spec[spec.size() - 2] == 'l') {
			ꦮꦭꦶ parimaa_na(wchar_t);
		}
		ꦮꦭꦶ parimaa_na(unsigned var_na);
	pak_sa 'C':
		ꦮꦭꦶ parimaa_na(wchar_t);
	pak_sa 's':
	pak_sa 'p':
	pak_sa 'S':
	pak_sa 'n':
		ꦮꦭꦶ parimaa_na(void *);
	pak_sa 'm':
		ꦮꦭꦶ parimaa_na(puur_naa_nka);
	default:
		ꦮꦭꦶ -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	pak_sa DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		ꦮꦭꦶ;
	pak_sa DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ꦮꦭꦶ;
	pak_sa DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ꦮꦭꦶ;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	ꦪꦼꦤ꧀ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<puur_naa_nka>(arg));
		ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (puur_naa_nka)arg);
	} ꦭꦶꦪ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	ꦪꦼꦤ꧀ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned puur_naa_nka>(arg));
		ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (spec == "%c") {
		result += static_cast<var_na>(arg);
		ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned puur_naa_nka)arg);
	} ꦭꦶꦪ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg)
{
	ꦪꦼꦤ꧀ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ꦮꦭꦶ;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	ꦪꦼꦤ꧀ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ꦮꦭꦶ;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	ꦪꦼꦤ꧀ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ꦮꦭꦶ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, puur_naa_nka *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#ꦪꦼꦤ꧀ defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ꦪꦼꦤ꧀ (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ꦮꦭꦶ;
	}
	OVERLAPPED ov = {};
	ꦪꦼꦤ꧀ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ꦮꦭꦶ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ꦪꦼꦤ꧀ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ꦪꦼꦤ꧀ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ꦪꦼꦤ꧀ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ꦮꦭꦶ;
	}
	puur_naa_nka ret;
	ꦱꦼꦢꦼꦭ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ꦪꦼꦤ꧀ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ꦪꦼꦤ꧀ (fd >= 0)
		close(fd); // releases the flock
}
#ꦭꦶꦪ
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ꦪꦼꦤ꧀ defined(_WIN32)
	ꦮꦭꦶ GetCurrentProcessId();
#elif defined(__wasi__)
	ꦮꦭꦶ 0;
#ꦭꦶꦪ
	ꦮꦭꦶ getpid();
#endif
}

YOSYS_NAMESPACE_END
