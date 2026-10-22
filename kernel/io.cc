#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#නම් !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#නැතහොත්
#include <io.h>
#endif

#නම් !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

පූර්ණ_සංඛ්‍යා readsome(std::istream &f, var_na *s, පූර්ණ_සංඛ්‍යා n)
{
	පූර්ණ_සංඛ්‍යා rc = පූර්ණ_සංඛ්‍යා(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	නම් (rc == 0) {
		පූර්ණ_සංඛ්‍යා c = f.get();
		නම් (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ආපසු rc;
}

std::string next_token(std::string &text, const var_na *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	නම් (pos_begin == std::string::npos)
		pos_begin = text.size();

	නම් (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		සඳහා (size_t i = pos_begin+1; i < text.size(); i++) {
			නම් (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ආපසු token;
			}
			නම් (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ආපසු token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	නම් (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ආපසු token;
}

std::vector<std::string> split_tokens(const std::string &text, const var_na *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	සඳහා (var_na c : text) {
		නම් (strchr(sep, c)) {
			නම් (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} නැතහොත්
			current_token += c;
	}
	නම් (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	ආපසු tokens;
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
	නම් (*pattern == 0)
		ආපසු *string == 0;

	නම් (*pattern == '\\') {
		නම් (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ආපසු true;
	}

	නම් (*pattern == '?') {
		නම් (*string == 0)
			ආපසු false;
		ආපසු patmatch(pattern+1, string+1);
	}

	නම් (*pattern == '*') {
		අතරතුර (*string) {
			නම් (patmatch(pattern+1, string++))
				ආපසු true;
		}
		ආපසු pattern[1] == 0;
	}

	නම් (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const var_na *p = pattern + (inverted_list ? 1 : 0);

		අතරතුර (*++p) {
			නම් (*p == ']') {
				නම් (found_match != inverted_list && patmatch(p+1, string+1))
					ආපසු true;
				bha_nga;
			}

			නම් (*p == '\\') {
				නම් (*++p == *string)
					found_match = true;
			} නැතහොත්
			නම් (*p == *string)
				found_match = true;
		}
	}

	නම් (*pattern == *string)
		ආපසු patmatch(pattern+1, string+1);

	ආපසු false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	නම් (!tmpdir.empty()) {
		ආපසු tmpdir;
	}

#නම් defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	නම් (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	නම් (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#නැතහොත්
	var_na * var = std::getenv("TMPDIR");
	නම් (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		අතරතුර (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} නැතහොත් {
		tmpdir.assign("/tmp");
	}
#endif
	ආපසු tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#නම් defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	අතරතුර (1) {
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		නම් (_access(template_str.c_str(), 0) != 0)
			bha_nga;
	}
#නැතහොත්
	පූර්ණ_සංඛ්‍යා suffixlen = template_str.size() - pos - 6;

	var_na *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	ආපසු template_str;
}

std::string make_temp_dir(std::string template_str)
{
#නම් defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ආපසු template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ආපසු template_str;
#නැතහොත්
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	පූර්ණ_සංඛ්‍යා suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	var_na *p = strdup(template_str.c_str());
	log_assert(p);
	var_na *res = mkdtemp(p);
	නම් (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	ආපසු template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#නම් defined(_WIN32)
	sa_nracanaa _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	නම් (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	නම් (_stat(dirname_.c_str(), &info) != 0)
	{
		ආපසු false;
	}
	ආපසු (info.st_mode & _S_IFDIR) != 0;
#නැතහොත්
	sa_nracanaa stat info;
	නම් (stat(dirname.c_str(), &info) != 0)
	{
		ආපසු false;
	}
	ආපසු (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	ආපසු _access(filename.c_str(), 0) == 0;
}
#නැතහොත්
bool check_accessible(const std::string& filename, bool is_exec)
{
	ආපසු access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	ආපසු check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	ආපසු check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ආපසු filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#නැතහොත්
	ආපසු filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#නැතහොත්
	sa_nracanaa stat stbuf;
	sa_nracanaa dirent **namelist;
	පූර්ණ_සංඛ්‍යා n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < n; i++) {
		නම් (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			නම් (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} නැතහොත්
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
#නම් defined(_WIN32)
	පූර්ණ_සංඛ්‍යා ret = _mkdir(dirname.c_str());
#නැතහොත්
	mode_t mode = 0755;
	පූර්ණ_සංඛ්‍යා ret = mkdir(dirname.c_str(), mode);
#endif
	නම් (ret == 0)
		ආපසු true;

	switch (errno)
	{
	pak_sa ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			නම් (pos == std::string::npos)
#නම් defined(_WIN32)
				pos = dirname.find_last_of('\\');
			නම් (pos == std::string::npos)
#endif
				ආපසු false;
			නම් (!create_directory( dirname.substr(0, pos) ))
				ආපසු false;
		}
		// now, try to create again
#නම් defined(_WIN32)
		ආපසු 0 == _mkdir(dirname.c_str());
#නැතහොත්
		ආපසු 0 == mkdir(dirname.c_str(), mode);
#endif

	pak_sa EEXIST:
		// done!
		ආපසු check_directory_exists(dirname);

	default:
		ආපසු false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#නම් defined(_WIN32) || defined(__wasi__)
	නම් (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ආපසු false;
	}
	ආපසු true;
#නැතහොත්
	නම් (mkdir(dirname.c_str(), 0700) == 0)
		ආපසු true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	පූර්ණ_සංඛ්‍යා err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	නම් (err == ENOENT && !parent.empty() && create_directory(parent)) {
		නම් (mkdir(dirname.c_str(), 0700) == 0)
			ආපසු true;
		err = errno;
	}

	නම් (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ආපසු false;
	}

	// Do not use a leftover that isn't ours
	sa_nracanaa stat st;
	නම් (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ආපසු false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	නම් (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ආපසු false;
	}

	ආපසු true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#නම් defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#නැතහොත්
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ආපසු make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	සඳහා (auto c : filename)
	{
		නම් (c == ' ')
			out += "\\ ";
		නැතහොත්
			out.push_back(c);
	}
	ආපසු out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ආපසු std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	නම් (ec)
		ආපසු "";

	result = std::filesystem::weakly_canonical(result, ec);

	නම් (ec)
		ආපසු "";

	ආපසු result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	නම් (parent.empty()) {
		ආපසු "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	නම් (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ආපසු result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	සඳහා (size_t i = 0; i < fmt.size(); ++i) {
		var_na ch = fmt[i];
		result.push_back(ch);
		නම් (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ආපසු result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ආපසු result;
}

static පූර්ණ_සංඛ්‍යා spec_parameter_size(std::string_view spec)
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
			නම් (spec[spec.size() - 3] == 'h')
				ආපසු parimaa_na(var_na);
			ආපසු parimaa_na(short);
		pak_sa 'l':
			නම් (spec[spec.size() - 3] == 'l')
				ආපසු parimaa_na(long long);
			ආපසු parimaa_na(long);
		pak_sa 'L':
		pak_sa 'q':
			ආපසු parimaa_na(long long);
		pak_sa 'j':
			ආපසු parimaa_na(intmax_t);
		pak_sa 'z':
		pak_sa 'Z':
			ආපසු parimaa_na(size_t);
		pak_sa 't':
			ආපසු parimaa_na(ptrdiff_t);
		}
		ආපසු parimaa_na(පූර්ණ_සංඛ්‍යා);
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		නම් (spec[spec.size() - 2] == 'L')
			ආපසු parimaa_na(long dviguu_na);
		නම් (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ආපසු parimaa_na(long dviguu_na);
		ආපසු parimaa_na(dviguu_na);
	pak_sa 'c':
		නම් (spec[spec.size() - 2] == 'l') {
			ආපසු parimaa_na(wchar_t);
		}
		ආපසු parimaa_na(unsigned var_na);
	pak_sa 'C':
		ආපසු parimaa_na(wchar_t);
	pak_sa 's':
	pak_sa 'p':
	pak_sa 'S':
	pak_sa 'n':
		ආපසු parimaa_na(void *);
	pak_sa 'm':
		ආපසු parimaa_na(පූර්ණ_සංඛ්‍යා);
	default:
		ආපසු -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	pak_sa DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		ආපසු;
	pak_sa DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ආපසු;
	pak_sa DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ආපසු;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	නම් (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<පූර්ණ_සංඛ්‍යා>(arg));
		ආපසු;
	}
	නම් (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (පූර්ණ_සංඛ්‍යා)arg);
	} නැතහොත් {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	නම් (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned පූර්ණ_සංඛ්‍යා>(arg));
		ආපසු;
	}
	නම් (spec == "%c") {
		result += static_cast<var_na>(arg);
		ආපසු;
	}
	නම් (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned පූර්ණ_සංඛ්‍යා)arg);
	} නැතහොත් {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg)
{
	නම් (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ආපසු;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	නම් (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ආපසු;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	නම් (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ආපසු;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, පූර්ණ_සංඛ්‍යා *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#නම් defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	නම් (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ආපසු;
	}
	OVERLAPPED ov = {};
	නම් (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ආපසු;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	නම් (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	නම් (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	නම් (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ආපසු;
	}
	පූර්ණ_සංඛ්‍යා ret;
	අතරතුර ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	නම් (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	නම් (fd >= 0)
		close(fd); // releases the flock
}
#නැතහොත්
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#නම් defined(_WIN32)
	ආපසු GetCurrentProcessId();
#elif defined(__wasi__)
	ආපසු 0;
#නැතහොත්
	ආපසු getpid();
#endif
}

YOSYS_NAMESPACE_END
