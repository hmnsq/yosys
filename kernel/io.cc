#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#nếu !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#không_thì
#include <io.h>
#endif

#nếu !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

số_nguyên readsome(std::istream &f, char *s, số_nguyên n)
{
	số_nguyên rc = số_nguyên(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	nếu (rc == 0) {
		số_nguyên c = f.get();
		nếu (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	trả_về rc;
}

std::string next_token(std::string &text, const char *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	nếu (pos_begin == std::string::npos)
		pos_begin = text.size();

	nếu (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		cho (size_t i = pos_begin+1; i < text.size(); i++) {
			nếu (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				trả_về token;
			}
			nếu (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				trả_về token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	nếu (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	trả_về token;
}

std::vector<std::string> split_tokens(const std::string &text, const char *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	cho (char c : text) {
		nếu (strchr(sep, c)) {
			nếu (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} không_thì
			current_token += c;
	}
	nếu (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	trả_về tokens;
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
	nếu (*pattern == 0)
		trả_về *string == 0;

	nếu (*pattern == '\\') {
		nếu (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			trả_về true;
	}

	nếu (*pattern == '?') {
		nếu (*string == 0)
			trả_về false;
		trả_về patmatch(pattern+1, string+1);
	}

	nếu (*pattern == '*') {
		trong_khi (*string) {
			nếu (patmatch(pattern+1, string++))
				trả_về true;
		}
		trả_về pattern[1] == 0;
	}

	nếu (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const char *p = pattern + (inverted_list ? 1 : 0);

		trong_khi (*++p) {
			nếu (*p == ']') {
				nếu (found_match != inverted_list && patmatch(p+1, string+1))
					trả_về true;
				break;
			}

			nếu (*p == '\\') {
				nếu (*++p == *string)
					found_match = true;
			} không_thì
			nếu (*p == *string)
				found_match = true;
		}
	}

	nếu (*pattern == *string)
		trả_về patmatch(pattern+1, string+1);

	trả_về false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	nếu (!tmpdir.empty()) {
		trả_về tmpdir;
	}

#nếu defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	nếu (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	nếu (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#không_thì
	char * var = std::getenv("TMPDIR");
	nếu (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		trong_khi (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} không_thì {
		tmpdir.assign("/tmp");
	}
#endif
	trả_về tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#nếu defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	trong_khi (1) {
		cho (số_nguyên i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		nếu (_access(template_str.c_str(), 0) != 0)
			break;
	}
#không_thì
	số_nguyên suffixlen = template_str.size() - pos - 6;

	char *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	trả_về template_str;
}

std::string make_temp_dir(std::string template_str)
{
#nếu defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	trả_về template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	trả_về template_str;
#không_thì
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	số_nguyên suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	char *p = strdup(template_str.c_str());
	log_assert(p);
	char *res = mkdtemp(p);
	nếu (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	trả_về template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#nếu defined(_WIN32)
	struct _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	nếu (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	nếu (_stat(dirname_.c_str(), &info) != 0)
	{
		trả_về false;
	}
	trả_về (info.st_mode & _S_IFDIR) != 0;
#không_thì
	struct stat info;
	nếu (stat(dirname.c_str(), &info) != 0)
	{
		trả_về false;
	}
	trả_về (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	trả_về _access(filename.c_str(), 0) == 0;
}
#không_thì
bool check_accessible(const std::string& filename, bool is_exec)
{
	trả_về access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	trả_về check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	trả_về check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	trả_về filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#không_thì
	trả_về filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#không_thì
	struct stat stbuf;
	struct dirent **namelist;
	số_nguyên n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	cho (số_nguyên i = 0; i < n; i++) {
		nếu (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			nếu (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} không_thì
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
#nếu defined(_WIN32)
	số_nguyên ret = _mkdir(dirname.c_str());
#không_thì
	mode_t mode = 0755;
	số_nguyên ret = mkdir(dirname.c_str(), mode);
#endif
	nếu (ret == 0)
		trả_về true;

	switch (errno)
	{
	case ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			nếu (pos == std::string::npos)
#nếu defined(_WIN32)
				pos = dirname.find_last_of('\\');
			nếu (pos == std::string::npos)
#endif
				trả_về false;
			nếu (!create_directory( dirname.substr(0, pos) ))
				trả_về false;
		}
		// now, try to create again
#nếu defined(_WIN32)
		trả_về 0 == _mkdir(dirname.c_str());
#không_thì
		trả_về 0 == mkdir(dirname.c_str(), mode);
#endif

	case EEXIST:
		// done!
		trả_về check_directory_exists(dirname);

	default:
		trả_về false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#nếu defined(_WIN32) || defined(__wasi__)
	nếu (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		trả_về false;
	}
	trả_về true;
#không_thì
	nếu (mkdir(dirname.c_str(), 0700) == 0)
		trả_về true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	số_nguyên err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	nếu (err == ENOENT && !parent.empty() && create_directory(parent)) {
		nếu (mkdir(dirname.c_str(), 0700) == 0)
			trả_về true;
		err = errno;
	}

	nếu (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		trả_về false;
	}

	// Do not use a leftover that isn't ours
	struct stat st;
	nếu (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		trả_về false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	nếu (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		trả_về false;
	}

	trả_về true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#nếu defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#không_thì
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	trả_về make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	cho (auto c : filename)
	{
		nếu (c == ' ')
			out += "\\ ";
		không_thì
			out.push_back(c);
	}
	trả_về out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	trả_về std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	nếu (ec)
		trả_về "";

	result = std::filesystem::weakly_canonical(result, ec);

	nếu (ec)
		trả_về "";

	trả_về result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	nếu (parent.empty()) {
		trả_về "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	nếu (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	trả_về result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	cho (size_t i = 0; i < fmt.size(); ++i) {
		char ch = fmt[i];
		result.push_back(ch);
		nếu (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	trả_về result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	trả_về result;
}

static số_nguyên spec_parameter_size(std::string_view spec)
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
			nếu (spec[spec.size() - 3] == 'h')
				trả_về sizeof(char);
			trả_về sizeof(short);
		case 'l':
			nếu (spec[spec.size() - 3] == 'l')
				trả_về sizeof(long long);
			trả_về sizeof(long);
		case 'L':
		case 'q':
			trả_về sizeof(long long);
		case 'j':
			trả_về sizeof(intmax_t);
		case 'z':
		case 'Z':
			trả_về sizeof(size_t);
		case 't':
			trả_về sizeof(ptrdiff_t);
		}
		trả_về sizeof(số_nguyên);
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
	case 'a':
	case 'A':
		nếu (spec[spec.size() - 2] == 'L')
			trả_về sizeof(long double);
		nếu (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			trả_về sizeof(long double);
		trả_về sizeof(double);
	case 'c':
		nếu (spec[spec.size() - 2] == 'l') {
			trả_về sizeof(wchar_t);
		}
		trả_về sizeof(unsigned char);
	case 'C':
		trả_về sizeof(wchar_t);
	case 's':
	case 'p':
	case 'S':
	case 'n':
		trả_về sizeof(void *);
	case 'm':
		trả_về sizeof(số_nguyên);
	default:
		trả_về -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	case DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		trả_về;
	case DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		trả_về;
	case DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		trả_về;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	nếu (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<số_nguyên>(arg));
		trả_về;
	}
	nếu (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (số_nguyên)arg);
	} không_thì {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	nếu (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned số_nguyên>(arg));
		trả_về;
	}
	nếu (spec == "%c") {
		result += static_cast<char>(arg);
		trả_về;
	}
	nếu (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned số_nguyên)arg);
	} không_thì {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, double arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const char *arg)
{
	nếu (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		trả_về;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	nếu (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		trả_về;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	nếu (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		trả_về;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, số_nguyên *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#nếu defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	nếu (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		trả_về;
	}
	OVERLAPPED ov = {};
	nếu (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		trả_về;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	nếu (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	nếu (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	nếu (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		trả_về;
	}
	số_nguyên ret;
	trong_khi ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	nếu (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	nếu (fd >= 0)
		close(fd); // releases the flock
}
#không_thì
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#nếu defined(_WIN32)
	trả_về GetCurrentProcessId();
#elif defined(__wasi__)
	trả_về 0;
#không_thì
	trả_về getpid();
#endif
}

YOSYS_NAMESPACE_END
