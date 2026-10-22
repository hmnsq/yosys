#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>

#ຖ້າ !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#ບໍ່ດັ່ງນັ້ນ
#include <io.h>
#endif

#ຖ້າ !defined(_WIN32) && !defined(__wasi__)
#include <sys/file.h>
#include <fcntl.h>
#include <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ຈຳນວນເຕັມ readsome(std::istream &f, var_na *s, ຈຳນວນເຕັມ n)
{
	ຈຳນວນເຕັມ rc = ຈຳນວນເຕັມ(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ຖ້າ (rc == 0) {
		ຈຳນວນເຕັມ c = f.get();
		ຖ້າ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ສົ່ງຄືນ rc;
}

std::string next_token(std::string &text, const var_na *sep, bool long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ຖ້າ (pos_begin == std::string::npos)
		pos_begin = text.size();

	ຖ້າ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		ສຳລັບ (size_t i = pos_begin+1; i < text.size(); i++) {
			ຖ້າ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ສົ່ງຄືນ token;
			}
			ຖ້າ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ສົ່ງຄືນ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ຖ້າ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ສົ່ງຄືນ token;
}

std::vector<std::string> split_tokens(const std::string &text, const var_na *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	ສຳລັບ (var_na c : text) {
		ຖ້າ (strchr(sep, c)) {
			ຖ້າ (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} ບໍ່ດັ່ງນັ້ນ
			current_token += c;
	}
	ຖ້າ (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	ສົ່ງຄືນ tokens;
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
	ຖ້າ (*pattern == 0)
		ສົ່ງຄືນ *string == 0;

	ຖ້າ (*pattern == '\\') {
		ຖ້າ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ສົ່ງຄືນ true;
	}

	ຖ້າ (*pattern == '?') {
		ຖ້າ (*string == 0)
			ສົ່ງຄືນ false;
		ສົ່ງຄືນ patmatch(pattern+1, string+1);
	}

	ຖ້າ (*pattern == '*') {
		ໃນຂະນະທີ່ (*string) {
			ຖ້າ (patmatch(pattern+1, string++))
				ສົ່ງຄືນ true;
		}
		ສົ່ງຄືນ pattern[1] == 0;
	}

	ຖ້າ (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		const var_na *p = pattern + (inverted_list ? 1 : 0);

		ໃນຂະນະທີ່ (*++p) {
			ຖ້າ (*p == ']') {
				ຖ້າ (found_match != inverted_list && patmatch(p+1, string+1))
					ສົ່ງຄືນ true;
				bha_nga;
			}

			ຖ້າ (*p == '\\') {
				ຖ້າ (*++p == *string)
					found_match = true;
			} ບໍ່ດັ່ງນັ້ນ
			ຖ້າ (*p == *string)
				found_match = true;
		}
	}

	ຖ້າ (*pattern == *string)
		ສົ່ງຄືນ patmatch(pattern+1, string+1);

	ສົ່ງຄືນ false;
}

std::string get_base_tmpdir()
{
	static std::string tmpdir;

	ຖ້າ (!tmpdir.empty()) {
		ສົ່ງຄືນ tmpdir;
	}

#ຖ້າ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ຖ້າ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), NULL, 0, NULL, NULL));
	ຖ້າ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), NULL, NULL) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ບໍ່ດັ່ງນັ້ນ
	var_na * var = std::getenv("TMPDIR");
	ຖ້າ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ໃນຂະນະທີ່ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} ບໍ່ດັ່ງນັ້ນ {
		tmpdir.assign("/tmp");
	}
#endif
	ສົ່ງຄືນ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ຖ້າ defined(__wasm)
	static size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#elif defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ໃນຂະນະທີ່ (1) {
		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 6; i++) {
			static std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			static uint32_t x = 314159265 ^ uint32_t(time(NULL));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ຖ້າ (_access(template_str.c_str(), 0) != 0)
			bha_nga;
	}
#ບໍ່ດັ່ງນັ້ນ
	ຈຳນວນເຕັມ suffixlen = template_str.size() - pos - 6;

	var_na *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	free(p);
#endif

	ສົ່ງຄືນ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ຖ້າ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ສົ່ງຄືນ template_str;
#elif defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ສົ່ງຄືນ template_str;
#ບໍ່ດັ່ງນັ້ນ
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ຈຳນວນເຕັມ suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	var_na *p = strdup(template_str.c_str());
	log_assert(p);
	var_na *res = mkdtemp(p);
	ຖ້າ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	free(p);

	ສົ່ງຄືນ template_str;
#endif
}

bool check_is_directory(const std::string& dirname)
{
#ຖ້າ defined(_WIN32)
	sa_nracanaa _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ຖ້າ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ຖ້າ (_stat(dirname_.c_str(), &info) != 0)
	{
		ສົ່ງຄືນ false;
	}
	ສົ່ງຄືນ (info.st_mode & _S_IFDIR) != 0;
#ບໍ່ດັ່ງນັ້ນ
	sa_nracanaa stat info;
	ຖ້າ (stat(dirname.c_str(), &info) != 0)
	{
		ສົ່ງຄືນ false;
	}
	ສົ່ງຄືນ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
bool check_accessible(const std::string& filename, bool)
{
	ສົ່ງຄືນ _access(filename.c_str(), 0) == 0;
}
#ບໍ່ດັ່ງນັ້ນ
bool check_accessible(const std::string& filename, bool is_exec)
{
	ສົ່ງຄືນ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

bool check_file_exists(const std::string& filename, bool is_exec)
{
	ສົ່ງຄືນ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(const std::string& filename, bool is_exec)
{
	ສົ່ງຄືນ check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ສົ່ງຄືນ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ filename[0] == '/';
#endif
}

void remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#ບໍ່ດັ່ງນັ້ນ
	sa_nracanaa stat stbuf;
	sa_nracanaa dirent **namelist;
	ຈຳນວນເຕັມ n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < n; i++) {
		ຖ້າ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ຖ້າ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} ບໍ່ດັ່ງນັ້ນ
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
#ຖ້າ defined(_WIN32)
	ຈຳນວນເຕັມ ret = _mkdir(dirname.c_str());
#ບໍ່ດັ່ງນັ້ນ
	mode_t mode = 0755;
	ຈຳນວນເຕັມ ret = mkdir(dirname.c_str(), mode);
#endif
	ຖ້າ (ret == 0)
		ສົ່ງຄືນ true;

	switch (errno)
	{
	pak_sa ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ຖ້າ (pos == std::string::npos)
#ຖ້າ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ຖ້າ (pos == std::string::npos)
#endif
				ສົ່ງຄືນ false;
			ຖ້າ (!create_directory( dirname.substr(0, pos) ))
				ສົ່ງຄືນ false;
		}
		// now, try to create again
#ຖ້າ defined(_WIN32)
		ສົ່ງຄືນ 0 == _mkdir(dirname.c_str());
#ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ 0 == mkdir(dirname.c_str(), mode);
#endif

	pak_sa EEXIST:
		// done!
		ສົ່ງຄືນ check_directory_exists(dirname);

	default:
		ສົ່ງຄືນ false;
	}
}

bool make_private_directory(const std::string& dirname)
{
#ຖ້າ defined(_WIN32) || defined(__wasi__)
	ຖ້າ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ສົ່ງຄືນ false;
	}
	ສົ່ງຄືນ true;
#ບໍ່ດັ່ງນັ້ນ
	ຖ້າ (mkdir(dirname.c_str(), 0700) == 0)
		ສົ່ງຄືນ true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ຈຳນວນເຕັມ err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ຖ້າ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ຖ້າ (mkdir(dirname.c_str(), 0700) == 0)
			ສົ່ງຄືນ true;
		err = errno;
	}

	ຖ້າ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ສົ່ງຄືນ false;
	}

	// Do not use a leftover that isn't ours
	sa_nracanaa stat st;
	ຖ້າ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ສົ່ງຄືນ false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ຖ້າ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ສົ່ງຄືນ false;
	}

	ສົ່ງຄືນ true;
#endif
}

std::string make_user_tmpdir(const std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ຖ້າ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#ບໍ່ດັ່ງນັ້ນ
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ສົ່ງຄືນ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(const std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	ສຳລັບ (auto c : filename)
	{
		ຖ້າ (c == ' ')
			out += "\\ ";
		ບໍ່ດັ່ງນັ້ນ
			out.push_back(c);
	}
	ສົ່ງຄືນ out;
}

void append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ສົ່ງຄືນ std::filesystem::path(path).filename().string();
}

std::string absolute_path(const std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ຖ້າ (ec)
		ສົ່ງຄືນ "";

	result = std::filesystem::weakly_canonical(result, ec);

	ຖ້າ (ec)
		ສົ່ງຄືນ "";

	ສົ່ງຄືນ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ຖ້າ (parent.empty()) {
		ສົ່ງຄືນ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ຖ້າ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ສົ່ງຄືນ result;
}

void format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	ສຳລັບ (size_t i = 0; i < fmt.size(); ++i) {
		var_na ch = fmt[i];
		result.push_back(ch);
		ຖ້າ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ສົ່ງຄືນ result;
}

static std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ສົ່ງຄືນ result;
}

static ຈຳນວນເຕັມ spec_parameter_size(std::string_view spec)
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
			ຖ້າ (spec[spec.size() - 3] == 'h')
				ສົ່ງຄືນ parimaa_na(var_na);
			ສົ່ງຄືນ parimaa_na(short);
		pak_sa 'l':
			ຖ້າ (spec[spec.size() - 3] == 'l')
				ສົ່ງຄືນ parimaa_na(long long);
			ສົ່ງຄືນ parimaa_na(long);
		pak_sa 'L':
		pak_sa 'q':
			ສົ່ງຄືນ parimaa_na(long long);
		pak_sa 'j':
			ສົ່ງຄືນ parimaa_na(intmax_t);
		pak_sa 'z':
		pak_sa 'Z':
			ສົ່ງຄືນ parimaa_na(size_t);
		pak_sa 't':
			ສົ່ງຄືນ parimaa_na(ptrdiff_t);
		}
		ສົ່ງຄືນ parimaa_na(ຈຳນວນເຕັມ);
	pak_sa 'e':
	pak_sa 'E':
	pak_sa 'f':
	pak_sa 'F':
	pak_sa 'g':
	pak_sa 'G':
	pak_sa 'a':
	pak_sa 'A':
		ຖ້າ (spec[spec.size() - 2] == 'L')
			ສົ່ງຄືນ parimaa_na(long dviguu_na);
		ຖ້າ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ສົ່ງຄືນ parimaa_na(long dviguu_na);
		ສົ່ງຄືນ parimaa_na(dviguu_na);
	pak_sa 'c':
		ຖ້າ (spec[spec.size() - 2] == 'l') {
			ສົ່ງຄືນ parimaa_na(wchar_t);
		}
		ສົ່ງຄືນ parimaa_na(unsigned var_na);
	pak_sa 'C':
		ສົ່ງຄືນ parimaa_na(wchar_t);
	pak_sa 's':
	pak_sa 'p':
	pak_sa 'S':
	pak_sa 'n':
		ສົ່ງຄືນ parimaa_na(void *);
	pak_sa 'm':
		ສົ່ງຄືນ parimaa_na(ຈຳນວນເຕັມ);
	default:
		ສົ່ງຄືນ -1;
	}
}

template <typename Arg>
static void format_emit_stringf(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	switch (num_dynamic_ints) {
	pak_sa DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		ສົ່ງຄືນ;
	pak_sa DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ສົ່ງຄືນ;
	pak_sa DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ສົ່ງຄືນ;
	}
	YOSYS_ABORT("Internal error");
}

void format_emit_long_long(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	ຖ້າ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<ຈຳນວນເຕັມ>(arg));
		ສົ່ງຄືນ;
	}
	ຖ້າ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (ຈຳນວນເຕັມ)arg);
	} ບໍ່ດັ່ງນັ້ນ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_unsigned_long_long(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	ຖ້າ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned ຈຳນວນເຕັມ>(arg));
		ສົ່ງຄືນ;
	}
	ຖ້າ (spec == "%c") {
		result += static_cast<var_na>(arg);
		ສົ່ງຄືນ;
	}
	ຖ້າ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned ຈຳນວນເຕັມ)arg);
	} ບໍ່ດັ່ງນັ້ນ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

void format_emit_double(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, dviguu_na arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_char_ptr(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const var_na *arg)
{
	ຖ້າ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ສົ່ງຄືນ;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

void format_emit_string(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const std::string &arg)
{
	ຖ້າ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ສົ່ງຄືນ;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

void format_emit_string_view(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	ຖ້າ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ສົ່ງຄືນ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

void format_emit_void_ptr(std::string &result, std::string_view spec, ຈຳນວນເຕັມ *dynamic_ints,
	DynamicIntCount num_dynamic_ints, const void *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#ຖ້າ defined(_WIN32)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ຖ້າ (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ສົ່ງຄືນ;
	}
	OVERLAPPED ov = {};
	ຖ້າ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ສົ່ງຄືນ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ຖ້າ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#elif !defined(__wasi__)
ScopedFileLock::ScopedFileLock(const std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ຖ້າ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ຖ້າ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ສົ່ງຄືນ;
	}
	ຈຳນວນເຕັມ ret;
	ໃນຂະນະທີ່ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ຖ້າ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ຖ້າ (fd >= 0)
		close(fd); // releases the flock
}
#ບໍ່ດັ່ງນັ້ນ
ScopedFileLock::ScopedFileLock(const std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ຖ້າ defined(_WIN32)
	ສົ່ງຄືນ GetCurrentProcessId();
#elif defined(__wasi__)
	ສົ່ງຄືນ 0;
#ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ getpid();
#endif
}

YOSYS_NAMESPACE_END
