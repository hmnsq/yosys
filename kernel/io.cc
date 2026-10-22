#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#如果 !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#否则
#include <io.h>
#endif

#如果 !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

整数 readsome(std::istream &f, char *s, 整数 n)
{
	整数 rc = 整数(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	如果 (rc == 0) {
		整数 c = f.get();
		如果 (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	返回 rc;
}

std::string next_token(std::string &text, const char *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	如果 (pos_begin == std::string::npos)
		pos_begin = text.size();

	如果 (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		为 (size_t i = pos_begin+1; i < text.size(); i++) {
			如果 (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				返回 token;
			}
			如果 (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				返回 token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	如果 (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	返回 token;
}

std::vector<std::string> split_tokens(const std::string &text, const char *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	为 (char c : text) {
		如果 (strchr(sep, c)) {
			如果 (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} 否则
			current_token += c;
	}
	如果 (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	返回 tokens;
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
bool patmatch(const char *pattern, const char *string)
{
	如果 (*pattern == 0)
		返回 *string == 0;

	如果 (*pattern == '\\') {
		如果 (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			返回 true;
	}

	如果 (*pattern == '?') {
		如果 (*string == 0)
			返回 false;
		返回 patmatch(pattern+1, string+1);
	}

	如果 (*pattern == '*') {
		当 (*string) {
			如果 (patmatch(pattern+1, string++))
				返回 true;
		}
		返回 pattern[1] == 0;
	}

	如果 (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const char *p = pattern + (inverted_list ? 1 : 0);

		当 (*++p) {
			如果 (*p == ']') {
				如果 (found_match != inverted_list && patmatch(p+1, string+1))
					返回 true;
				break;
			}

			如果 (*p == '\\') {
				如果 (*++p == *string)
					found_match = true;
			} 否则
			如果 (*p == *string)
				found_match = true;
		}
	}

	如果 (*pattern == *string)
		返回 patmatch(pattern+1, string+1);

	返回 false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	如果 (!tmpdir.empty()) {
		返回 tmpdir;
	}

#如果 defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	如果 (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	如果 (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#否则
	char * var = std::getenv("TMPDIR");
	如果 (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		当 (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} 否则 {
		tmpdir.assign("/tmp");
	}
#endif
	返回 tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#如果 defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	当 (1) {
		为 (整数 i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		如果 (_access(template_str.c_str(), 0) != 0)
			break;
	}
#否则
	整数 suffixlen = template_str.size() - pos - 6;

	char *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	返回 template_str;
}

std::string make_temp_dir(std::string template_str)
{
#如果 defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	返回 template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	返回 template_str;
#否则
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	整数 suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	char *p = strdup(template_str.c_str());
	log_assert(p);
	char *res = mkdtemp(p);
	如果 (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	返回 template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#如果 defined(_WIN32)
	struct _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	如果 (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	如果 (_stat(dirname_.c_str(), &info) != 0)
	{
		返回 false;
	}
	返回 (info.st_mode & _S_IFDIR) != 0;
#否则
	struct stat info;
	如果 (stat(dirname.c_str(), &info) != 0)
	{
		返回 false;
	}
	返回 (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	返回 _access(filename.c_str(), 0) == 0;
}
#否则
bool check_accessible(const std::string& filename, bool is_exec)
{
	返回 access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	返回 check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	返回 check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	返回 filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#否则
	返回 filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#否则
	struct stat stbuf;
	struct dirent **namelist;
	整数 n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	为 (整数 i = 0; i < n; i++) {
		如果 (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			如果 (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} 否则
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
#如果 defined(_WIN32)
	整数 ret = _mkdir(dirname.c_str());
#否则
	mode_t mode = 0755;
	整数 ret = mkdir(dirname.c_str(), mode);
#endif
	如果 (ret == 0)
		返回 true;

	switch (errno)
	{
	case ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			如果 (pos == std::string::npos)
#如果 defined(_WIN32)
				pos = dirname.find_last_of('\\');
			如果 (pos == std::string::npos)
#endif
				返回 false;
			如果 (!create_directory( dirname.substr(0, pos) ))
				返回 false;
		}
		// now, try to create again
#如果 defined(_WIN32)
		返回 0 == _mkdir(dirname.c_str());
#否则
		返回 0 == mkdir(dirname.c_str(), mode);
#endif

	case EEXIST:
		// done!
		返回 check_directory_exists(dirname);

	default:
		返回 false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#如果 defined(_WIN32) || defined(__wasi__)
	如果 (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		返回 false;
	}
	返回 true;
#否则
	如果 (mkdir(dirname.c_str(), 0700) == 0)
		返回 true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	整数 err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	如果 (err == ENOENT && !parent.empty() && create_directory(parent)) {
		如果 (mkdir(dirname.c_str(), 0700) == 0)
			返回 true;
		err = errno;
	}

	如果 (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		返回 false;
	}

	// Do not use a leftover that isn't ours
	struct stat st;
	如果 (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		返回 false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	如果 (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		返回 false;
	}

	返回 true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#如果 defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#否则
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	返回 make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	为 (auto c : filename)
	{
		如果 (c == ' ')
			out += "\\ ";
		否则
			out.push_back(c);
	}
	返回 out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	返回 std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	如果 (ec)
		返回 "";

	result = std::filesystem::weakly_canonical(result, ec);

	如果 (ec)
		返回 "";

	返回 result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	如果 (parent.empty()) {
		返回 "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	如果 (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	返回 result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	为 (size_t i = 0; i < fmt.size(); ++i) {
		char ch = fmt[i];
		result.push_back(ch);
		如果 (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	返回 result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	返回 result;
}

static 整数 spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	switch (spec[spec.size() - 1]) {
	case 'd':
	case 'i':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
		switch (spec[spec.size() - 2]) {
		case 'h':
			如果 (spec[spec.size() - 3] == 'h')
				返回 sizeof(char);
			返回 sizeof(short);
		case 'l':
			如果 (spec[spec.size() - 3] == 'l')
				返回 sizeof(long long);
			返回 sizeof(long);
		case 'L':
		case 'q':
			返回 sizeof(long long);
		case 'j':
			返回 sizeof(intmax_t);
		case 'z':
		case 'Z':
			返回 sizeof(size_t);
		case 't':
			返回 sizeof(ptrdiff_t);
		}
		返回 sizeof(整数);
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
	case 'a':
	case 'A':
		如果 (spec[spec.size() - 2] == 'L')
			返回 sizeof(long double);
		如果 (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			返回 sizeof(long double);
		返回 sizeof(double);
	case 'c':
		如果 (spec[spec.size() - 2] == 'l') {
			返回 sizeof(wchar_t);
		}
		返回 sizeof(unsigned char);
	case 'C':
		返回 sizeof(wchar_t);
	case 's':
	case 'p':
	case 'S':
	case 'n':
		返回 sizeof(void *);
	case 'm':
		返回 sizeof(整数);
	default:
		返回 -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	case DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		返回;
	case DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		返回;
	case DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		返回;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	如果 (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<整数>(arg));
		返回;
	}
	如果 (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (整数)arg);
	} 否则 {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	如果 (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned 整数>(arg));
		返回;
	}
	如果 (spec == "%c") {
		result += static_cast<char>(arg);
		返回;
	}
	如果 (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned 整数)arg);
	} 否则 {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, double arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const char *arg)
{
	如果 (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		返回;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	如果 (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		返回;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	如果 (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		返回;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, 整数 *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#如果 defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	如果 (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		返回;
	}
	OVERLAPPED ov = {};
	如果 (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		返回;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	如果 (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	如果 (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	如果 (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		返回;
	}
	整数 ret;
	当 ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	如果 (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	如果 (fd >= 0)
		close(fd); // releases the flock
}
#否则
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#如果 defined(_WIN32)
	返回 GetCurrentProcessId();
#elif defined(__wasi__)
	返回 0;
#否则
	返回 getpid();
#endif
}

YOSYS_NAMESPACE_END
