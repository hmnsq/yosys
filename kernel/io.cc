#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#གལ་སྲིད !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#དེ་མིན
#include <io.h>
#endif

#གལ་སྲིད !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ཧྲིལ readsome(std::istream &f, var_na *s, ཧྲིལ n)
{
	ཧྲིལ rc = ཧྲིལ(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	གལ་སྲིད (rc == 0) {
		ཧྲིལ c = f.get();
		གལ་སྲིད (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ལོག rc;
}

std::string next_token(std::string &text, const var_na *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	གལ་སྲིད (pos_begin == std::string::npos)
		pos_begin = text.size();

	གལ་སྲིད (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		དོན་ལུ (size_t i = pos_begin+1; i < text.size(); i++) {
			གལ་སྲིད (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ལོག token;
			}
			གལ་སྲིད (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ལོག token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	གལ་སྲིད (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ལོག token;
}

std::vector<std::string> split_tokens(const std::string &text, const var_na *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	དོན་ལུ (var_na c : text) {
		གལ་སྲིད (strchr(sep, c)) {
			གལ་སྲིད (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} དེ་མིན
			current_token += c;
	}
	གལ་སྲིད (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	ལོག tokens;
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
	གལ་སྲིད (*pattern == 0)
		ལོག *string == 0;

	གལ་སྲིད (*pattern == '\\') {
		གལ་སྲིད (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ལོག true;
	}

	གལ་སྲིད (*pattern == '?') {
		གལ་སྲིད (*string == 0)
			ལོག false;
		ལོག patmatch(pattern+1, string+1);
	}

	གལ་སྲིད (*pattern == '*') {
		སྐབས (*string) {
			གལ་སྲིད (patmatch(pattern+1, string++))
				ལོག true;
		}
		ལོག pattern[1] == 0;
	}

	གལ་སྲིད (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const var_na *p = pattern + (inverted_list ? 1 : 0);

		སྐབས (*++p) {
			གལ་སྲིད (*p == ']') {
				གལ་སྲིད (found_match != inverted_list && patmatch(p+1, string+1))
					ལོག true;
				bha_nga;
			}

			གལ་སྲིད (*p == '\\') {
				གལ་སྲིད (*++p == *string)
					found_match = true;
			} དེ་མིན
			གལ་སྲིད (*p == *string)
				found_match = true;
		}
	}

	གལ་སྲིད (*pattern == *string)
		ལོག patmatch(pattern+1, string+1);

	ལོག false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	གལ་སྲིད (!tmpdir.empty()) {
		ལོག tmpdir;
	}

#གལ་སྲིད defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	གལ་སྲིད (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	གལ་སྲིད (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#དེ་མིན
	var_na * var = std::getenv("TMPDIR");
	གལ་སྲིད (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		སྐབས (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} དེ་མིན {
		tmpdir.assign("/tmp");
	}
#endif
	ལོག tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#གལ་སྲིད defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	སྐབས (1) {
		དོན་ལུ (ཧྲིལ i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		གལ་སྲིད (_access(template_str.c_str(), 0) != 0)
			bha_nga;
	}
#དེ་མིན
	ཧྲིལ suffixlen = template_str.size() - pos - 6;

	var_na *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	ལོག template_str;
}

std::string make_temp_dir(std::string template_str)
{
#གལ་སྲིད defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ལོག template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ལོག template_str;
#དེ་མིན
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ཧྲིལ suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	var_na *p = strdup(template_str.c_str());
	log_assert(p);
	var_na *res = mkdtemp(p);
	གལ་སྲིད (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	ལོག template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#གལ་སྲིད defined(_WIN32)
	sa_nracanaa _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	གལ་སྲིད (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	གལ་སྲིད (_stat(dirname_.c_str(), &info) != 0)
	{
		ལོག false;
	}
	ལོག (info.st_mode & _S_IFDIR) != 0;
#དེ་མིན
	sa_nracanaa stat info;
	གལ་སྲིད (stat(dirname.c_str(), &info) != 0)
	{
		ལོག false;
	}
	ལོག (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	ལོག _access(filename.c_str(), 0) == 0;
}
#དེ་མིན
bool check_accessible(const std::string& filename, bool is_exec)
{
	ལོག access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	ལོག check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	ལོག check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ལོག filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#དེ་མིན
	ལོག filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#དེ་མིན
	sa_nracanaa stat stbuf;
	sa_nracanaa dirent **namelist;
	ཧྲིལ n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	དོན་ལུ (ཧྲིལ i = 0; i < n; i++) {
		གལ་སྲིད (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			གལ་སྲིད (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} དེ་མིན
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
#གལ་སྲིད defined(_WIN32)
	ཧྲིལ ret = _mkdir(dirname.c_str());
#དེ་མིན
	mode_t mode = 0755;
	ཧྲིལ ret = mkdir(dirname.c_str(), mode);
#endif
	གལ་སྲིད (ret == 0)
		ལོག true;

	switch (errno)
	{
	pak_sa ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			གལ་སྲིད (pos == std::string::npos)
#གལ་སྲིད defined(_WIN32)
				pos = dirname.find_last_of('\\');
			གལ་སྲིད (pos == std::string::npos)
#endif
				ལོག false;
			གལ་སྲིད (!create_directory( dirname.substr(0, pos) ))
				ལོག false;
		}
		// now, try to create again
#གལ་སྲིད defined(_WIN32)
		ལོག 0 == _mkdir(dirname.c_str());
#དེ་མིན
		ལོག 0 == mkdir(dirname.c_str(), mode);
#endif

	pak_sa EEXIST:
		// done!
		ལོག check_directory_exists(dirname);

	default:
		ལོག false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#གལ་སྲིད defined(_WIN32) || defined(__wasi__)
	གལ་སྲིད (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ལོག false;
	}
	ལོག true;
#དེ་མིན
	གལ་སྲིད (mkdir(dirname.c_str(), 0700) == 0)
		ལོག true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ཧྲིལ err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	གལ་སྲིད (err == ENOENT && !parent.empty() && create_directory(parent)) {
		གལ་སྲིད (mkdir(dirname.c_str(), 0700) == 0)
			ལོག true;
		err = errno;
	}

	གལ་སྲིད (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ལོག false;
	}

	// Do not use a leftover that isn't ours
	sa_nracanaa stat st;
	གལ་སྲིད (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ལོག false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	གལ་སྲིད (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ལོག false;
	}

	ལོག true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#གལ་སྲིད defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#དེ་མིན
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ལོག make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	དོན་ལུ (auto c : filename)
	{
		གལ་སྲིད (c == ' ')
			out += "\\ ";
		དེ་མིན
			out.push_back(c);
	}
	ལོག out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ལོག std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	གལ་སྲིད (ec)
		ལོག "";

	result = std::filesystem::weakly_canonical(result, ec);

	གལ་སྲིད (ec)
		ལོག "";

	ལོག result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	གལ་སྲིད (parent.empty()) {
		ལོག "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	གལ་སྲིད (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ལོག result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	དོན་ལུ (size_t i = 0; i < fmt.size(); ++i) {
		var_na ch = fmt[i];
		result.push_back(ch);
		གལ་སྲིད (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ལོག result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ལོག result;
}

static ཧྲིལ spec_parameter_size(std::string_view spec)
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
			གལ་སྲིད (spec[spec.size() - 3] == 'h')
				ལོག parimaa_na(var_na);
			ལོག parimaa_na(short);
		pak_sa 'l':
			གལ་སྲིད (spec[spec.size() - 3] == 'l')
				ལོག parimaa_na(long long);
			ལོག parimaa_na(long);
		pak_sa 'L':
		pak_sa 'q':
			ལོག parimaa_na(long long);
		pak_sa 'j':
			ལོག parimaa_na(intmax_t);
		pak_sa 'z':
		pak_sa 'Z':
			ལོག parimaa_na(size_t);
		pak_sa 't':
			ལོག parimaa_na(ptrdiff_t);
		}
		ལོག parimaa_na(ཧྲིལ);
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		གལ་སྲིད (spec[spec.size() - 2] == 'L')
			ལོག parimaa_na(long dviguu_na);
		གལ་སྲིད (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ལོག parimaa_na(long dviguu_na);
		ལོག parimaa_na(dviguu_na);
	pak_sa 'c':
		གལ་སྲིད (spec[spec.size() - 2] == 'l') {
			ལོག parimaa_na(wchar_t);
		}
		ལོག parimaa_na(unsigned var_na);
	pak_sa 'C':
		ལོག parimaa_na(wchar_t);
	pak_sa 's':
	pak_sa 'p':
	pak_sa 'S':
	pak_sa 'n':
		ལོག parimaa_na(void *);
	pak_sa 'm':
		ལོག parimaa_na(ཧྲིལ);
	default:
		ལོག -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	pak_sa DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		ལོག;
	pak_sa DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ལོག;
	pak_sa DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ལོག;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	གལ་སྲིད (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<ཧྲིལ>(arg));
		ལོག;
	}
	གལ་སྲིད (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (ཧྲིལ)arg);
	} དེ་མིན {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	གལ་སྲིད (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned ཧྲིལ>(arg));
		ལོག;
	}
	གལ་སྲིད (spec == "%c") {
		result += static_cast<var_na>(arg);
		ལོག;
	}
	གལ་སྲིད (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned ཧྲིལ)arg);
	} དེ་མིན {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg)
{
	གལ་སྲིད (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ལོག;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	གལ་སྲིད (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ལོག;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	གལ་སྲིད (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ལོག;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, ཧྲིལ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#གལ་སྲིད defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	གལ་སྲིད (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ལོག;
	}
	OVERLAPPED ov = {};
	གལ་སྲིད (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ལོག;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	གལ་སྲིད (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	གལ་སྲིད (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	གལ་སྲིད (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ལོག;
	}
	ཧྲིལ ret;
	སྐབས ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	གལ་སྲིད (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	གལ་སྲིད (fd >= 0)
		close(fd); // releases the flock
}
#དེ་མིན
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#གལ་སྲིད defined(_WIN32)
	ལོག GetCurrentProcessId();
#elif defined(__wasi__)
	ལོག 0;
#དེ་མིན
	ལོག getpid();
#endif
}

YOSYS_NAMESPACE_END
