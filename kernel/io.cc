#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/log.h"
#অন্তর্ভাব <iostream>
#অন্তর্ভাব <string>
#অন্তর্ভাব <filesystem>
#অন্তর্ভাব <system_error>

#যদি !defined(WIN32)
#অন্তর্ভাব <dirent.h>
#অন্তর্ভাব <unistd.h>
#অন্যথা
#অন্তর্ভাব <io.h>
#endif

#যদি !defined(_WIN32) && !defined(__wasi__)
#অন্তর্ভাব <sys/file.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

পূর্ণাঙ্ক readsome(std::istream &f, বর্ণ *s, পূর্ণাঙ্ক n)
{
	পূর্ণাঙ্ক rc = পূর্ণাঙ্ক(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	যদি (rc == 0) {
		পূর্ণাঙ্ক c = f.get();
		যদি (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	প্রত্যাগম rc;
}

std::string next_token(std::string &text, ধ্রুব বর্ণ *sep, তার্কিক long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	যদি (pos_begin == std::string::npos)
		pos_begin = text.size();

	যদি (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		প্রতি (size_t i = pos_begin+1; i < text.size(); i++) {
			যদি (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				প্রত্যাগম token;
			}
			যদি (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				প্রত্যাগম token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	যদি (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	প্রত্যাগম token;
}

std::vector<std::string> split_tokens(ধ্রুব std::string &text, ধ্রুব বর্ণ *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	প্রতি (বর্ণ c : text) {
		যদি (strchr(sep, c)) {
			যদি (!current_token.empty()) {
				tokens.ঠেলো_back(current_token);
				current_token.clear();
			}
		} অন্যথা
			current_token += c;
	}
	যদি (!current_token.empty()) {
		tokens.ঠেলো_back(current_token);
		current_token.clear();
	}
	প্রত্যাগম tokens;
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
তার্কিক patmatch(ধ্রুব বর্ণ *pattern, ধ্রুব বর্ণ *string)
{
	যদি (*pattern == 0)
		প্রত্যাগম *string == 0;

	যদি (*pattern == '\\') {
		যদি (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			প্রত্যাগম সত্য;
	}

	যদি (*pattern == '?') {
		যদি (*string == 0)
			প্রত্যাগম অসত্য;
		প্রত্যাগম patmatch(pattern+1, string+1);
	}

	যদি (*pattern == '*') {
		যাবত্ (*string) {
			যদি (patmatch(pattern+1, string++))
				প্রত্যাগম সত্য;
		}
		প্রত্যাগম pattern[1] == 0;
	}

	যদি (*pattern == '[') {
		তার্কিক found_match = অসত্য;
		তার্কিক inverted_list = pattern[1] == '!';
		ধ্রুব বর্ণ *p = pattern + (inverted_list ? 1 : 0);

		যাবত্ (*++p) {
			যদি (*p == ']') {
				যদি (found_match != inverted_list && patmatch(p+1, string+1))
					প্রত্যাগম সত্য;
				ভঙ্গ;
			}

			যদি (*p == '\\') {
				যদি (*++p == *string)
					found_match = সত্য;
			} অন্যথা
			যদি (*p == *string)
				found_match = সত্য;
		}
	}

	যদি (*pattern == *string)
		প্রত্যাগম patmatch(pattern+1, string+1);

	প্রত্যাগম অসত্য;
}

std::string get_base_tmpdir()
{
	স্থির std::string tmpdir;

	যদি (!tmpdir.empty()) {
		প্রত্যাগম tmpdir;
	}

#যদি defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	যদি (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), রিক্ত, 0, রিক্ত, রিক্ত));
	যদি (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), রিক্ত, রিক্ত) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#অন্যথা
	বর্ণ * var = std::getenv("TMPDIR");
	যদি (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		যাবত্ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.বের_করো_back();
		}
	} অন্যথা {
		tmpdir.assign("/tmp");
	}
#endif
	প্রত্যাগম tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#যদি defined(__wasm)
	স্থির size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#নোচেদ্যদি defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	যাবত্ (1) {
		প্রতি (পূর্ণাঙ্ক i = 0; i < 6; i++) {
			স্থির std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			স্থির uint32_t x = 314159265 ^ uint32_t(time(রিক্ত));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		যদি (_access(template_str.c_str(), 0) != 0)
			ভঙ্গ;
	}
#অন্যথা
	পূর্ণাঙ্ক suffixlen = template_str.size() - pos - 6;

	বর্ণ *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	মুক্ত(p);
#endif

	প্রত্যাগম template_str;
}

std::string make_temp_dir(std::string template_str)
{
#যদি defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	প্রত্যাগম template_str;
#নোচেদ্যদি defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	প্রত্যাগম template_str;
#অন্যথা
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	পূর্ণাঙ্ক suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	বর্ণ *p = strdup(template_str.c_str());
	log_assert(p);
	বর্ণ *res = mkdtemp(p);
	যদি (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	মুক্ত(p);

	প্রত্যাগম template_str;
#endif
}

তার্কিক check_is_directory(ধ্রুব std::string& dirname)
{
#যদি defined(_WIN32)
	সংরচনা _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	যদি (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	যদি (_stat(dirname_.c_str(), &info) != 0)
	{
		প্রত্যাগম অসত্য;
	}
	প্রত্যাগম (info.st_mode & _S_IFDIR) != 0;
#অন্যথা
	সংরচনা stat info;
	যদি (stat(dirname.c_str(), &info) != 0)
	{
		প্রত্যাগম অসত্য;
	}
	প্রত্যাগম (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
তার্কিক check_accessible(ধ্রুব std::string& filename, তার্কিক)
{
	প্রত্যাগম _access(filename.c_str(), 0) == 0;
}
#অন্যথা
তার্কিক check_accessible(ধ্রুব std::string& filename, তার্কিক is_exec)
{
	প্রত্যাগম access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

তার্কিক check_file_exists(ধ্রুব std::string& filename, তার্কিক is_exec)
{
	প্রত্যাগম check_accessible(filename, is_exec) && !check_is_directory(filename);
}
তার্কিক check_directory_exists(ধ্রুব std::string& filename, তার্কিক is_exec)
{
	প্রত্যাগম check_accessible(filename, is_exec) && check_is_directory(filename);
}

তার্কিক is_absolute_path(std::string filename)
{
#ifdef _WIN32
	প্রত্যাগম filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#অন্যথা
	প্রত্যাগম filename[0] == '/';
#endif
}

শূন্য remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#অন্যথা
	সংরচনা stat stbuf;
	সংরচনা dirent **namelist;
	পূর্ণাঙ্ক n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	প্রতি (পূর্ণাঙ্ক i = 0; i < n; i++) {
		যদি (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			যদি (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} অন্যথা
				remove_directory(buffer);
		}
		মুক্ত(namelist[i]);
	}
	মুক্ত(namelist);
	rmdir(dirname.c_str());
#endif
}

তার্কিক create_directory(ধ্রুব std::string& dirname)
{
#যদি defined(_WIN32)
	পূর্ণাঙ্ক ret = _mkdir(dirname.c_str());
#অন্যথা
	mode_t mode = 0755;
	পূর্ণাঙ্ক ret = mkdir(dirname.c_str(), mode);
#endif
	যদি (ret == 0)
		প্রত্যাগম সত্য;

	বিকল্প (errno)
	{
	পক্ষ ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			যদি (pos == std::string::npos)
#যদি defined(_WIN32)
				pos = dirname.find_last_of('\\');
			যদি (pos == std::string::npos)
#endif
				প্রত্যাগম অসত্য;
			যদি (!create_directory( dirname.substr(0, pos) ))
				প্রত্যাগম অসত্য;
		}
		// now, try to create again
#যদি defined(_WIN32)
		প্রত্যাগম 0 == _mkdir(dirname.c_str());
#অন্যথা
		প্রত্যাগম 0 == mkdir(dirname.c_str(), mode);
#endif

	পক্ষ EEXIST:
		// done!
		প্রত্যাগম check_directory_exists(dirname);

	স্বভাব:
		প্রত্যাগম অসত্য;
	}
}

তার্কিক make_private_directory(ধ্রুব std::string& dirname)
{
#যদি defined(_WIN32) || defined(__wasi__)
	যদি (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		প্রত্যাগম অসত্য;
	}
	প্রত্যাগম সত্য;
#অন্যথা
	যদি (mkdir(dirname.c_str(), 0700) == 0)
		প্রত্যাগম সত্য;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	পূর্ণাঙ্ক err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	যদি (err == ENOENT && !parent.empty() && create_directory(parent)) {
		যদি (mkdir(dirname.c_str(), 0700) == 0)
			প্রত্যাগম সত্য;
		err = errno;
	}

	যদি (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		প্রত্যাগম অসত্য;
	}

	// Do not use a leftover that isn't ours
	সংরচনা stat st;
	যদি (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		প্রত্যাগম অসত্য;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	যদি (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		প্রত্যাগম অসত্য;
	}

	প্রত্যাগম সত্য;
#endif
}

std::string make_user_tmpdir(ধ্রুব std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#যদি defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#অন্যথা
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	প্রত্যাগম make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ধ্রুব std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	প্রতি (auto c : filename)
	{
		যদি (c == ' ')
			out += "\\ ";
		অন্যথা
			out.ঠেলো_back(c);
	}
	প্রত্যাগম out;
}

শূন্য append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	প্রত্যাগম std::filesystem::path(path).filename().string();
}

std::string absolute_path(ধ্রুব std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	যদি (ec)
		প্রত্যাগম "";

	result = std::filesystem::weakly_canonical(result, ec);

	যদি (ec)
		প্রত্যাগম "";

	প্রত্যাগম result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	যদি (parent.empty()) {
		প্রত্যাগম "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	যদি (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	প্রত্যাগম result;
}

শূন্য format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	প্রতি (size_t i = 0; i < fmt.size(); ++i) {
		বর্ণ ch = fmt[i];
		result.ঠেলো_back(ch);
		যদি (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	প্রত্যাগম result;
}

স্থির std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	বিকল্প (spec[spec.size() - 1]) {
	পক্ষ 'd':
	পক্ষ 'i':
	পক্ষ 'o':
	পক্ষ 'u':
	পক্ষ 'x':
	পক্ষ 'X':
		বিকল্প (spec[spec.size() - 2]) {
		পক্ষ 'h':
			যদি (spec[spec.size() - 3] == 'h')
				প্রত্যাগম পরিমাণ(বর্ণ);
			প্রত্যাগম পরিমাণ(short);
		পক্ষ 'l':
			যদি (spec[spec.size() - 3] == 'l')
				প্রত্যাগম পরিমাণ(long long);
			প্রত্যাগম পরিমাণ(long);
		পক্ষ 'L':
		পক্ষ 'q':
			প্রত্যাগম পরিমাণ(long long);
		পক্ষ 'j':
			প্রত্যাগম পরিমাণ(intmax_t);
		পক্ষ 'z':
		পক্ষ 'Z':
			প্রত্যাগম পরিমাণ(size_t);
		পক্ষ 't':
			প্রত্যাগম পরিমাণ(ptrdiff_t);
		}
		প্রত্যাগম পরিমাণ(পূর্ণাঙ্ক);
	পক্ষ 'e':
	পক্ষ 'E':
	পক্ষ 'f':
	পক্ষ 'F':
	পক্ষ 'g':
	পক্ষ 'G':
	পক্ষ 'a':
	পক্ষ 'A':
		যদি (spec[spec.size() - 2] == 'L')
			প্রত্যাগম পরিমাণ(long দ্বিগুণ);
		যদি (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			প্রত্যাগম পরিমাণ(long দ্বিগুণ);
		প্রত্যাগম পরিমাণ(দ্বিগুণ);
	পক্ষ 'c':
		যদি (spec[spec.size() - 2] == 'l') {
			প্রত্যাগম পরিমাণ(wchar_t);
		}
		প্রত্যাগম পরিমাণ(unsigned বর্ণ);
	পক্ষ 'C':
		প্রত্যাগম পরিমাণ(wchar_t);
	পক্ষ 's':
	পক্ষ 'p':
	পক্ষ 'S':
	পক্ষ 'n':
		প্রত্যাগম পরিমাণ(শূন্য *);
	পক্ষ 'm':
		প্রত্যাগম পরিমাণ(পূর্ণাঙ্ক);
	স্বভাব:
		প্রত্যাগম -1;
	}
}

template <typename Arg>
স্থির শূন্য format_emit_stringf(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	বিকল্প (অঙ্ক_dynamic_ints) {
	পক্ষ Dynamic_Int_গুনতি::NONE:
		result += string_view_stringf(spec, arg);
		প্রত্যাগম;
	পক্ষ Dynamic_Int_গুনতি::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		প্রত্যাগম;
	পক্ষ Dynamic_Int_গুনতি::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		প্রত্যাগম;
	}
	YOSYS_ABORT("Internal error");
}

শূন্য format_emit_long_long(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, long long arg)
{
	যদি (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<পূর্ণাঙ্ক>(arg));
		প্রত্যাগম;
	}
	যদি (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, (পূর্ণাঙ্ক)arg);
	} অন্যথা {
		format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
	}
}

শূন্য format_emit_unsigned_long_long(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, unsigned long long arg)
{
	যদি (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned পূর্ণাঙ্ক>(arg));
		প্রত্যাগম;
	}
	যদি (spec == "%c") {
		result += static_cast<বর্ণ>(arg);
		প্রত্যাগম;
	}
	যদি (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, (unsigned পূর্ণাঙ্ক)arg);
	} অন্যথা {
		format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
	}
}

শূন্য format_emit_double(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, দ্বিগুণ arg)
{
	format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
}

শূন্য format_emit_char_ptr(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুব বর্ণ *arg)
{
	যদি (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		প্রত্যাগম;
	}
	format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
}

শূন্য format_emit_string(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুব std::string &arg)
{
	যদি (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		প্রত্যাগম;
	}
	format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg.c_str());
}

শূন্য format_emit_string_view(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, std::string_view arg)
{
	যদি (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		প্রত্যাগম;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, std::string(arg).c_str());
}

শূন্য format_emit_void_ptr(std::string &result, std::string_view spec, পূর্ণাঙ্ক *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুব শূন্য *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
}

#যদি defined(_WIN32)
ScopedFileLock::ScopedFileLock(ধ্রুব std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			রিক্ত, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, রিক্ত);
	যদি (h == INVALID_HANDLE_মান) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		প্রত্যাগম;
	}
	OVERLAPPED ov = {};
	যদি (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		প্রত্যাগম;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	যদি (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#নোচেদ্যদি !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ধ্রুব std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	যদি (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	যদি (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		প্রত্যাগম;
	}
	পূর্ণাঙ্ক ret;
	যাবত্ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	যদি (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	যদি (fd >= 0)
		close(fd); // releases the flock
}
#অন্যথা
ScopedFileLock::ScopedFileLock(ধ্রুব std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#যদি defined(_WIN32)
	প্রত্যাগম GetCurrentProcessId();
#নোচেদ্যদি defined(__wasi__)
	প্রত্যাগম 0;
#অন্যথা
	প্রত্যাগম getpid();
#endif
}

YOSYS_NAMESPACE_END
