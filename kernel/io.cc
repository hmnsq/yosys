#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#якщо !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#інакше
#include <io.h>
#endif

#якщо !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ціле readsome(std::istream &f, char *s, ціле n)
{
	ціле rc = ціле(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	якщо (rc == 0) {
		ціле c = f.get();
		якщо (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	повернути rc;
}

std::string next_token(std::string &text, const char *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	якщо (pos_begin == std::string::npos)
		pos_begin = text.size();

	якщо (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		для (size_t i = pos_begin+1; i < text.size(); i++) {
			якщо (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				повернути token;
			}
			якщо (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				повернути token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	якщо (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	повернути token;
}

std::vector<std::string> split_tokens(const std::string &text, const char *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	для (char c : text) {
		якщо (strchr(sep, c)) {
			якщо (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} інакше
			current_token += c;
	}
	якщо (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	повернути tokens;
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
	якщо (*pattern == 0)
		повернути *string == 0;

	якщо (*pattern == '\\') {
		якщо (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			повернути true;
	}

	якщо (*pattern == '?') {
		якщо (*string == 0)
			повернути false;
		повернути patmatch(pattern+1, string+1);
	}

	якщо (*pattern == '*') {
		поки (*string) {
			якщо (patmatch(pattern+1, string++))
				повернути true;
		}
		повернути pattern[1] == 0;
	}

	якщо (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const char *p = pattern + (inverted_list ? 1 : 0);

		поки (*++p) {
			якщо (*p == ']') {
				якщо (found_match != inverted_list && patmatch(p+1, string+1))
					повернути true;
				break;
			}

			якщо (*p == '\\') {
				якщо (*++p == *string)
					found_match = true;
			} інакше
			якщо (*p == *string)
				found_match = true;
		}
	}

	якщо (*pattern == *string)
		повернути patmatch(pattern+1, string+1);

	повернути false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	якщо (!tmpdir.empty()) {
		повернути tmpdir;
	}

#якщо defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	якщо (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	якщо (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#інакше
	char * var = std::getenv("TMPDIR");
	якщо (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		поки (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} інакше {
		tmpdir.assign("/tmp");
	}
#endif
	повернути tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#якщо defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	поки (1) {
		для (ціле i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		якщо (_access(template_str.c_str(), 0) != 0)
			break;
	}
#інакше
	ціле suffixlen = template_str.size() - pos - 6;

	char *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	повернути template_str;
}

std::string make_temp_dir(std::string template_str)
{
#якщо defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	повернути template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	повернути template_str;
#інакше
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ціле suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	char *p = strdup(template_str.c_str());
	log_assert(p);
	char *res = mkdtemp(p);
	якщо (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	повернути template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#якщо defined(_WIN32)
	struct _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	якщо (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	якщо (_stat(dirname_.c_str(), &info) != 0)
	{
		повернути false;
	}
	повернути (info.st_mode & _S_IFDIR) != 0;
#інакше
	struct stat info;
	якщо (stat(dirname.c_str(), &info) != 0)
	{
		повернути false;
	}
	повернути (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	повернути _access(filename.c_str(), 0) == 0;
}
#інакше
bool check_accessible(const std::string& filename, bool is_exec)
{
	повернути access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	повернути check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	повернути check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	повернути filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#інакше
	повернути filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#інакше
	struct stat stbuf;
	struct dirent **namelist;
	ціле n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	для (ціле i = 0; i < n; i++) {
		якщо (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			якщо (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} інакше
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
#якщо defined(_WIN32)
	ціле ret = _mkdir(dirname.c_str());
#інакше
	mode_t mode = 0755;
	ціле ret = mkdir(dirname.c_str(), mode);
#endif
	якщо (ret == 0)
		повернути true;

	switch (errno)
	{
	case ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			якщо (pos == std::string::npos)
#якщо defined(_WIN32)
				pos = dirname.find_last_of('\\');
			якщо (pos == std::string::npos)
#endif
				повернути false;
			якщо (!create_directory( dirname.substr(0, pos) ))
				повернути false;
		}
		// now, try to create again
#якщо defined(_WIN32)
		повернути 0 == _mkdir(dirname.c_str());
#інакше
		повернути 0 == mkdir(dirname.c_str(), mode);
#endif

	case EEXIST:
		// done!
		повернути check_directory_exists(dirname);

	default:
		повернути false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#якщо defined(_WIN32) || defined(__wasi__)
	якщо (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		повернути false;
	}
	повернути true;
#інакше
	якщо (mkdir(dirname.c_str(), 0700) == 0)
		повернути true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ціле err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	якщо (err == ENOENT && !parent.empty() && create_directory(parent)) {
		якщо (mkdir(dirname.c_str(), 0700) == 0)
			повернути true;
		err = errno;
	}

	якщо (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		повернути false;
	}

	// Do not use a leftover that isn't ours
	struct stat st;
	якщо (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		повернути false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	якщо (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		повернути false;
	}

	повернути true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#якщо defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#інакше
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	повернути make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	для (auto c : filename)
	{
		якщо (c == ' ')
			out += "\\ ";
		інакше
			out.push_back(c);
	}
	повернути out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	повернути std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	якщо (ec)
		повернути "";

	result = std::filesystem::weakly_canonical(result, ec);

	якщо (ec)
		повернути "";

	повернути result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	якщо (parent.empty()) {
		повернути "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	якщо (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	повернути result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	для (size_t i = 0; i < fmt.size(); ++i) {
		char ch = fmt[i];
		result.push_back(ch);
		якщо (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	повернути result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	повернути result;
}

static ціле spec_parameter_size(std::string_view spec)
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
			якщо (spec[spec.size() - 3] == 'h')
				повернути sizeof(char);
			повернути sizeof(short);
		case 'l':
			якщо (spec[spec.size() - 3] == 'l')
				повернути sizeof(long long);
			повернути sizeof(long);
		case 'L':
		case 'q':
			повернути sizeof(long long);
		case 'j':
			повернути sizeof(intmax_t);
		case 'z':
		case 'Z':
			повернути sizeof(size_t);
		case 't':
			повернути sizeof(ptrdiff_t);
		}
		повернути sizeof(ціле);
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
	case 'a':
	case 'A':
		якщо (spec[spec.size() - 2] == 'L')
			повернути sizeof(long double);
		якщо (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			повернути sizeof(long double);
		повернути sizeof(double);
	case 'c':
		якщо (spec[spec.size() - 2] == 'l') {
			повернути sizeof(wchar_t);
		}
		повернути sizeof(unsigned char);
	case 'C':
		повернути sizeof(wchar_t);
	case 's':
	case 'p':
	case 'S':
	case 'n':
		повернути sizeof(void *);
	case 'm':
		повернути sizeof(ціле);
	default:
		повернути -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	case DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		повернути;
	case DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		повернути;
	case DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		повернути;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	якщо (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<ціле>(arg));
		повернути;
	}
	якщо (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (ціле)arg);
	} інакше {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	якщо (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned ціле>(arg));
		повернути;
	}
	якщо (spec == "%c") {
		result += static_cast<char>(arg);
		повернути;
	}
	якщо (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned ціле)arg);
	} інакше {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, double arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const char *arg)
{
	якщо (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		повернути;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	якщо (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		повернути;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	якщо (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		повернути;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, ціле *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#якщо defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	якщо (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		повернути;
	}
	OVERLAPPED ov = {};
	якщо (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		повернути;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	якщо (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	якщо (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	якщо (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		повернути;
	}
	ціле ret;
	поки ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	якщо (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	якщо (fd >= 0)
		close(fd); // releases the flock
}
#інакше
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#якщо defined(_WIN32)
	повернути GetCurrentProcessId();
#elif defined(__wasi__)
	повернути 0;
#інакше
	повернути getpid();
#endif
}

YOSYS_NAMESPACE_END
