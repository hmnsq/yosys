#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#ប្រសិនបើ !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#បើមិនដូច្នេះ
#include <io.h>
#endif

#ប្រសិនបើ !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ចំនួនគត់ readsome(std::istream &f, var_na *s, ចំនួនគត់ n)
{
	ចំនួនគត់ rc = ចំនួនគត់(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ប្រសិនបើ (rc == 0) {
		ចំនួនគត់ c = f.get();
		ប្រសិនបើ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ត្រឡប់ rc;
}

std::string next_token(std::string &text, const var_na *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ប្រសិនបើ (pos_begin == std::string::npos)
		pos_begin = text.size();

	ប្រសិនបើ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		សម្រាប់ (size_t i = pos_begin+1; i < text.size(); i++) {
			ប្រសិនបើ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ត្រឡប់ token;
			}
			ប្រសិនបើ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ត្រឡប់ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ប្រសិនបើ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ត្រឡប់ token;
}

std::vector<std::string> split_tokens(const std::string &text, const var_na *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	សម្រាប់ (var_na c : text) {
		ប្រសិនបើ (strchr(sep, c)) {
			ប្រសិនបើ (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} បើមិនដូច្នេះ
			current_token += c;
	}
	ប្រសិនបើ (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	ត្រឡប់ tokens;
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
	ប្រសិនបើ (*pattern == 0)
		ត្រឡប់ *string == 0;

	ប្រសិនបើ (*pattern == '\\') {
		ប្រសិនបើ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ត្រឡប់ true;
	}

	ប្រសិនបើ (*pattern == '?') {
		ប្រសិនបើ (*string == 0)
			ត្រឡប់ false;
		ត្រឡប់ patmatch(pattern+1, string+1);
	}

	ប្រសិនបើ (*pattern == '*') {
		ខណៈពេល (*string) {
			ប្រសិនបើ (patmatch(pattern+1, string++))
				ត្រឡប់ true;
		}
		ត្រឡប់ pattern[1] == 0;
	}

	ប្រសិនបើ (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const var_na *p = pattern + (inverted_list ? 1 : 0);

		ខណៈពេល (*++p) {
			ប្រសិនបើ (*p == ']') {
				ប្រសិនបើ (found_match != inverted_list && patmatch(p+1, string+1))
					ត្រឡប់ true;
				bha_nga;
			}

			ប្រសិនបើ (*p == '\\') {
				ប្រសិនបើ (*++p == *string)
					found_match = true;
			} បើមិនដូច្នេះ
			ប្រសិនបើ (*p == *string)
				found_match = true;
		}
	}

	ប្រសិនបើ (*pattern == *string)
		ត្រឡប់ patmatch(pattern+1, string+1);

	ត្រឡប់ false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	ប្រសិនបើ (!tmpdir.empty()) {
		ត្រឡប់ tmpdir;
	}

#ប្រសិនបើ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ប្រសិនបើ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	ប្រសិនបើ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#បើមិនដូច្នេះ
	var_na * var = std::getenv("TMPDIR");
	ប្រសិនបើ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ខណៈពេល (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} បើមិនដូច្នេះ {
		tmpdir.assign("/tmp");
	}
#endif
	ត្រឡប់ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ប្រសិនបើ defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ខណៈពេល (1) {
		សម្រាប់ (ចំនួនគត់ i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ប្រសិនបើ (_access(template_str.c_str(), 0) != 0)
			bha_nga;
	}
#បើមិនដូច្នេះ
	ចំនួនគត់ suffixlen = template_str.size() - pos - 6;

	var_na *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	ត្រឡប់ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ប្រសិនបើ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ត្រឡប់ template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ត្រឡប់ template_str;
#បើមិនដូច្នេះ
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ចំនួនគត់ suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	var_na *p = strdup(template_str.c_str());
	log_assert(p);
	var_na *res = mkdtemp(p);
	ប្រសិនបើ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	ត្រឡប់ template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#ប្រសិនបើ defined(_WIN32)
	sa_nracanaa _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ប្រសិនបើ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ប្រសិនបើ (_stat(dirname_.c_str(), &info) != 0)
	{
		ត្រឡប់ false;
	}
	ត្រឡប់ (info.st_mode & _S_IFDIR) != 0;
#បើមិនដូច្នេះ
	sa_nracanaa stat info;
	ប្រសិនបើ (stat(dirname.c_str(), &info) != 0)
	{
		ត្រឡប់ false;
	}
	ត្រឡប់ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	ត្រឡប់ _access(filename.c_str(), 0) == 0;
}
#បើមិនដូច្នេះ
bool check_accessible(const std::string& filename, bool is_exec)
{
	ត្រឡប់ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	ត្រឡប់ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	ត្រឡប់ check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ត្រឡប់ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#បើមិនដូច្នេះ
	ត្រឡប់ filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#បើមិនដូច្នេះ
	sa_nracanaa stat stbuf;
	sa_nracanaa dirent **namelist;
	ចំនួនគត់ n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	សម្រាប់ (ចំនួនគត់ i = 0; i < n; i++) {
		ប្រសិនបើ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ប្រសិនបើ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} បើមិនដូច្នេះ
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
#ប្រសិនបើ defined(_WIN32)
	ចំនួនគត់ ret = _mkdir(dirname.c_str());
#បើមិនដូច្នេះ
	mode_t mode = 0755;
	ចំនួនគត់ ret = mkdir(dirname.c_str(), mode);
#endif
	ប្រសិនបើ (ret == 0)
		ត្រឡប់ true;

	switch (errno)
	{
	pak_sa ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ប្រសិនបើ (pos == std::string::npos)
#ប្រសិនបើ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ប្រសិនបើ (pos == std::string::npos)
#endif
				ត្រឡប់ false;
			ប្រសិនបើ (!create_directory( dirname.substr(0, pos) ))
				ត្រឡប់ false;
		}
		// now, try to create again
#ប្រសិនបើ defined(_WIN32)
		ត្រឡប់ 0 == _mkdir(dirname.c_str());
#បើមិនដូច្នេះ
		ត្រឡប់ 0 == mkdir(dirname.c_str(), mode);
#endif

	pak_sa EEXIST:
		// done!
		ត្រឡប់ check_directory_exists(dirname);

	default:
		ត្រឡប់ false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#ប្រសិនបើ defined(_WIN32) || defined(__wasi__)
	ប្រសិនបើ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ត្រឡប់ false;
	}
	ត្រឡប់ true;
#បើមិនដូច្នេះ
	ប្រសិនបើ (mkdir(dirname.c_str(), 0700) == 0)
		ត្រឡប់ true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ចំនួនគត់ err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ប្រសិនបើ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ប្រសិនបើ (mkdir(dirname.c_str(), 0700) == 0)
			ត្រឡប់ true;
		err = errno;
	}

	ប្រសិនបើ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ត្រឡប់ false;
	}

	// Do not use a leftover that isn't ours
	sa_nracanaa stat st;
	ប្រសិនបើ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ត្រឡប់ false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ប្រសិនបើ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ត្រឡប់ false;
	}

	ត្រឡប់ true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ប្រសិនបើ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#បើមិនដូច្នេះ
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ត្រឡប់ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	សម្រាប់ (auto c : filename)
	{
		ប្រសិនបើ (c == ' ')
			out += "\\ ";
		បើមិនដូច្នេះ
			out.push_back(c);
	}
	ត្រឡប់ out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ត្រឡប់ std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ប្រសិនបើ (ec)
		ត្រឡប់ "";

	result = std::filesystem::weakly_canonical(result, ec);

	ប្រសិនបើ (ec)
		ត្រឡប់ "";

	ត្រឡប់ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ប្រសិនបើ (parent.empty()) {
		ត្រឡប់ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ប្រសិនបើ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ត្រឡប់ result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	សម្រាប់ (size_t i = 0; i < fmt.size(); ++i) {
		var_na ch = fmt[i];
		result.push_back(ch);
		ប្រសិនបើ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ត្រឡប់ result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ត្រឡប់ result;
}

static ចំនួនគត់ spec_parameter_size(std::string_view spec)
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
			ប្រសិនបើ (spec[spec.size() - 3] == 'h')
				ត្រឡប់ parimaa_na(var_na);
			ត្រឡប់ parimaa_na(short);
		pak_sa 'l':
			ប្រសិនបើ (spec[spec.size() - 3] == 'l')
				ត្រឡប់ parimaa_na(long long);
			ត្រឡប់ parimaa_na(long);
		pak_sa 'L':
		pak_sa 'q':
			ត្រឡប់ parimaa_na(long long);
		pak_sa 'j':
			ត្រឡប់ parimaa_na(intmax_t);
		pak_sa 'z':
		pak_sa 'Z':
			ត្រឡប់ parimaa_na(size_t);
		pak_sa 't':
			ត្រឡប់ parimaa_na(ptrdiff_t);
		}
		ត្រឡប់ parimaa_na(ចំនួនគត់);
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		ប្រសិនបើ (spec[spec.size() - 2] == 'L')
			ត្រឡប់ parimaa_na(long dviguu_na);
		ប្រសិនបើ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ត្រឡប់ parimaa_na(long dviguu_na);
		ត្រឡប់ parimaa_na(dviguu_na);
	pak_sa 'c':
		ប្រសិនបើ (spec[spec.size() - 2] == 'l') {
			ត្រឡប់ parimaa_na(wchar_t);
		}
		ត្រឡប់ parimaa_na(unsigned var_na);
	pak_sa 'C':
		ត្រឡប់ parimaa_na(wchar_t);
	pak_sa 's':
	pak_sa 'p':
	pak_sa 'S':
	pak_sa 'n':
		ត្រឡប់ parimaa_na(void *);
	pak_sa 'm':
		ត្រឡប់ parimaa_na(ចំនួនគត់);
	default:
		ត្រឡប់ -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	pak_sa DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		ត្រឡប់;
	pak_sa DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ត្រឡប់;
	pak_sa DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ត្រឡប់;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	ប្រសិនបើ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<ចំនួនគត់>(arg));
		ត្រឡប់;
	}
	ប្រសិនបើ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (ចំនួនគត់)arg);
	} បើមិនដូច្នេះ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	ប្រសិនបើ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned ចំនួនគត់>(arg));
		ត្រឡប់;
	}
	ប្រសិនបើ (spec == "%c") {
		result += static_cast<var_na>(arg);
		ត្រឡប់;
	}
	ប្រសិនបើ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned ចំនួនគត់)arg);
	} បើមិនដូច្នេះ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg)
{
	ប្រសិនបើ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ត្រឡប់;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	ប្រសិនបើ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ត្រឡប់;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	ប្រសិនបើ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ត្រឡប់;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, ចំនួនគត់ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#ប្រសិនបើ defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ប្រសិនបើ (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ត្រឡប់;
	}
	OVERLAPPED ov = {};
	ប្រសិនបើ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ត្រឡប់;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ប្រសិនបើ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ប្រសិនបើ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ប្រសិនបើ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ត្រឡប់;
	}
	ចំនួនគត់ ret;
	ខណៈពេល ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ប្រសិនបើ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ប្រសិនបើ (fd >= 0)
		close(fd); // releases the flock
}
#បើមិនដូច្នេះ
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ប្រសិនបើ defined(_WIN32)
	ត្រឡប់ GetCurrentProcessId();
#elif defined(__wasi__)
	ត្រឡប់ 0;
#បើមិនដូច្នេះ
	ត្រឡប់ getpid();
#endif
}

YOSYS_NAMESPACE_END
