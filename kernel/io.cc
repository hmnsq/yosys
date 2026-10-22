#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#ถ้า !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#มิฉะนั้น
#include <io.h>
#endif

#ถ้า !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

จำนวนเต็ม readsome(std::istream &f, var_na *s, จำนวนเต็ม n)
{
	จำนวนเต็ม rc = จำนวนเต็ม(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ถ้า (rc == 0) {
		จำนวนเต็ม c = f.get();
		ถ้า (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	คืน rc;
}

std::string next_token(std::string &text, const var_na *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ถ้า (pos_begin == std::string::npos)
		pos_begin = text.size();

	ถ้า (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		สำหรับ (size_t i = pos_begin+1; i < text.size(); i++) {
			ถ้า (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				คืน token;
			}
			ถ้า (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				คืน token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ถ้า (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	คืน token;
}

std::vector<std::string> split_tokens(const std::string &text, const var_na *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	สำหรับ (var_na c : text) {
		ถ้า (strchr(sep, c)) {
			ถ้า (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} มิฉะนั้น
			current_token += c;
	}
	ถ้า (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	คืน tokens;
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
	ถ้า (*pattern == 0)
		คืน *string == 0;

	ถ้า (*pattern == '\\') {
		ถ้า (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			คืน true;
	}

	ถ้า (*pattern == '?') {
		ถ้า (*string == 0)
			คืน false;
		คืน patmatch(pattern+1, string+1);
	}

	ถ้า (*pattern == '*') {
		ขณะ (*string) {
			ถ้า (patmatch(pattern+1, string++))
				คืน true;
		}
		คืน pattern[1] == 0;
	}

	ถ้า (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const var_na *p = pattern + (inverted_list ? 1 : 0);

		ขณะ (*++p) {
			ถ้า (*p == ']') {
				ถ้า (found_match != inverted_list && patmatch(p+1, string+1))
					คืน true;
				bha_nga;
			}

			ถ้า (*p == '\\') {
				ถ้า (*++p == *string)
					found_match = true;
			} มิฉะนั้น
			ถ้า (*p == *string)
				found_match = true;
		}
	}

	ถ้า (*pattern == *string)
		คืน patmatch(pattern+1, string+1);

	คืน false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	ถ้า (!tmpdir.empty()) {
		คืน tmpdir;
	}

#ถ้า defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ถ้า (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	ถ้า (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#มิฉะนั้น
	var_na * var = std::getenv("TMPDIR");
	ถ้า (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ขณะ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} มิฉะนั้น {
		tmpdir.assign("/tmp");
	}
#endif
	คืน tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ถ้า defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ขณะ (1) {
		สำหรับ (จำนวนเต็ม i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ถ้า (_access(template_str.c_str(), 0) != 0)
			bha_nga;
	}
#มิฉะนั้น
	จำนวนเต็ม suffixlen = template_str.size() - pos - 6;

	var_na *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	คืน template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ถ้า defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	คืน template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	คืน template_str;
#มิฉะนั้น
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	จำนวนเต็ม suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	var_na *p = strdup(template_str.c_str());
	log_assert(p);
	var_na *res = mkdtemp(p);
	ถ้า (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	คืน template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#ถ้า defined(_WIN32)
	sa_nracanaa _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ถ้า (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ถ้า (_stat(dirname_.c_str(), &info) != 0)
	{
		คืน false;
	}
	คืน (info.st_mode & _S_IFDIR) != 0;
#มิฉะนั้น
	sa_nracanaa stat info;
	ถ้า (stat(dirname.c_str(), &info) != 0)
	{
		คืน false;
	}
	คืน (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	คืน _access(filename.c_str(), 0) == 0;
}
#มิฉะนั้น
bool check_accessible(const std::string& filename, bool is_exec)
{
	คืน access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	คืน check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	คืน check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	คืน filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#มิฉะนั้น
	คืน filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#มิฉะนั้น
	sa_nracanaa stat stbuf;
	sa_nracanaa dirent **namelist;
	จำนวนเต็ม n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	สำหรับ (จำนวนเต็ม i = 0; i < n; i++) {
		ถ้า (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ถ้า (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} มิฉะนั้น
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
#ถ้า defined(_WIN32)
	จำนวนเต็ม ret = _mkdir(dirname.c_str());
#มิฉะนั้น
	mode_t mode = 0755;
	จำนวนเต็ม ret = mkdir(dirname.c_str(), mode);
#endif
	ถ้า (ret == 0)
		คืน true;

	switch (errno)
	{
	pak_sa ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ถ้า (pos == std::string::npos)
#ถ้า defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ถ้า (pos == std::string::npos)
#endif
				คืน false;
			ถ้า (!create_directory( dirname.substr(0, pos) ))
				คืน false;
		}
		// now, try to create again
#ถ้า defined(_WIN32)
		คืน 0 == _mkdir(dirname.c_str());
#มิฉะนั้น
		คืน 0 == mkdir(dirname.c_str(), mode);
#endif

	pak_sa EEXIST:
		// done!
		คืน check_directory_exists(dirname);

	default:
		คืน false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#ถ้า defined(_WIN32) || defined(__wasi__)
	ถ้า (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		คืน false;
	}
	คืน true;
#มิฉะนั้น
	ถ้า (mkdir(dirname.c_str(), 0700) == 0)
		คืน true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	จำนวนเต็ม err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ถ้า (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ถ้า (mkdir(dirname.c_str(), 0700) == 0)
			คืน true;
		err = errno;
	}

	ถ้า (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		คืน false;
	}

	// Do not use a leftover that isn't ours
	sa_nracanaa stat st;
	ถ้า (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		คืน false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ถ้า (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		คืน false;
	}

	คืน true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ถ้า defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#มิฉะนั้น
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	คืน make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	สำหรับ (auto c : filename)
	{
		ถ้า (c == ' ')
			out += "\\ ";
		มิฉะนั้น
			out.push_back(c);
	}
	คืน out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	คืน std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ถ้า (ec)
		คืน "";

	result = std::filesystem::weakly_canonical(result, ec);

	ถ้า (ec)
		คืน "";

	คืน result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ถ้า (parent.empty()) {
		คืน "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ถ้า (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	คืน result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	สำหรับ (size_t i = 0; i < fmt.size(); ++i) {
		var_na ch = fmt[i];
		result.push_back(ch);
		ถ้า (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	คืน result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	คืน result;
}

static จำนวนเต็ม spec_parameter_size(std::string_view spec)
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
			ถ้า (spec[spec.size() - 3] == 'h')
				คืน parimaa_na(var_na);
			คืน parimaa_na(short);
		pak_sa 'l':
			ถ้า (spec[spec.size() - 3] == 'l')
				คืน parimaa_na(long long);
			คืน parimaa_na(long);
		pak_sa 'L':
		pak_sa 'q':
			คืน parimaa_na(long long);
		pak_sa 'j':
			คืน parimaa_na(intmax_t);
		pak_sa 'z':
		pak_sa 'Z':
			คืน parimaa_na(size_t);
		pak_sa 't':
			คืน parimaa_na(ptrdiff_t);
		}
		คืน parimaa_na(จำนวนเต็ม);
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		ถ้า (spec[spec.size() - 2] == 'L')
			คืน parimaa_na(long dviguu_na);
		ถ้า (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			คืน parimaa_na(long dviguu_na);
		คืน parimaa_na(dviguu_na);
	pak_sa 'c':
		ถ้า (spec[spec.size() - 2] == 'l') {
			คืน parimaa_na(wchar_t);
		}
		คืน parimaa_na(unsigned var_na);
	pak_sa 'C':
		คืน parimaa_na(wchar_t);
	pak_sa 's':
	pak_sa 'p':
	pak_sa 'S':
	pak_sa 'n':
		คืน parimaa_na(void *);
	pak_sa 'm':
		คืน parimaa_na(จำนวนเต็ม);
	default:
		คืน -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	pak_sa DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		คืน;
	pak_sa DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		คืน;
	pak_sa DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		คืน;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	ถ้า (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<จำนวนเต็ม>(arg));
		คืน;
	}
	ถ้า (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (จำนวนเต็ม)arg);
	} มิฉะนั้น {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	ถ้า (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned จำนวนเต็ม>(arg));
		คืน;
	}
	ถ้า (spec == "%c") {
		result += static_cast<var_na>(arg);
		คืน;
	}
	ถ้า (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned จำนวนเต็ม)arg);
	} มิฉะนั้น {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg)
{
	ถ้า (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		คืน;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	ถ้า (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		คืน;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	ถ้า (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		คืน;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, จำนวนเต็ม *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#ถ้า defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ถ้า (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		คืน;
	}
	OVERLAPPED ov = {};
	ถ้า (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		คืน;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ถ้า (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ถ้า (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ถ้า (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		คืน;
	}
	จำนวนเต็ม ret;
	ขณะ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ถ้า (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ถ้า (fd >= 0)
		close(fd); // releases the flock
}
#มิฉะนั้น
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ถ้า defined(_WIN32)
	คืน GetCurrentProcessId();
#elif defined(__wasi__)
	คืน 0;
#มิฉะนั้น
	คืน getpid();
#endif
}

YOSYS_NAMESPACE_END
