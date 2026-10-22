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

পূর্ণসংখ্যা readsome(std::istream &f, অক্ষর *s, পূর্ণসংখ্যা n)
{
	পূর্ণসংখ্যা rc = পূর্ণসংখ্যা(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	যদি (rc == 0) {
		পূর্ণসংখ্যা c = f.get();
		যদি (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ফেরত rc;
}

std::string পরবর্তী_token(std::string &text, ধ্রুবক অক্ষর *sep, যৌক্তিক long_strings)
{
	আকার_t pos_begin = text.find_first_not_of(sep);

	যদি (pos_begin == std::string::npos)
		pos_begin = text.আকার_2();

	যদি (long_strings && pos_begin != text.আকার_2() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		জন্য (আকার_t i = pos_begin+1; i < text.আকার_2(); i++) {
			যদি (text[i] == '"' && (i+1 == text.আকার_2() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ফেরত token;
			}
			যদি (i+1 < text.আকার_2() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.আকার_2() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ফেরত token + ";";
			}
		}
	}

	আকার_t pos_end = text.find_first_of(sep, pos_begin);

	যদি (pos_end == std::string::npos)
		pos_end = text.আকার_2();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ফেরত token;
}

std::vector<std::string> split_tokens(ধ্রুবক std::string &text, ধ্রুবক অক্ষর *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	জন্য (অক্ষর c : text) {
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
	ফেরত tokens;
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
যৌক্তিক patmatch(ধ্রুবক অক্ষর *pattern, ধ্রুবক অক্ষর *string)
{
	যদি (*pattern == 0)
		ফেরত *string == 0;

	যদি (*pattern == '\\') {
		যদি (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ফেরত সত্য;
	}

	যদি (*pattern == '?') {
		যদি (*string == 0)
			ফেরত মিথ্যা;
		ফেরত patmatch(pattern+1, string+1);
	}

	যদি (*pattern == '*') {
		যখন (*string) {
			যদি (patmatch(pattern+1, string++))
				ফেরত সত্য;
		}
		ফেরত pattern[1] == 0;
	}

	যদি (*pattern == '[') {
		যৌক্তিক found_match = মিথ্যা;
		যৌক্তিক inverted_list = pattern[1] == '!';
		ধ্রুবক অক্ষর *p = pattern + (inverted_list ? 1 : 0);

		যখন (*++p) {
			যদি (*p == ']') {
				যদি (found_match != inverted_list && patmatch(p+1, string+1))
					ফেরত সত্য;
				বিরতি;
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
		ফেরত patmatch(pattern+1, string+1);

	ফেরত মিথ্যা;
}

std::string get_base_tmpdir()
{
	স্থির std::string tmpdir;

	যদি (!tmpdir.empty()) {
		ফেরত tmpdir;
	}

#যদি defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	যদি (!Get_অস্থায়ী_Path_W(wtmppath.আকার_2(), wtmppath.তথ্য()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.তথ্য(), wtmppath.আকার_2(), ফাঁকা, 0, ফাঁকা, ফাঁকা));
	যদি (WideCharToMultiByte(CP_UTF8, 0, wtmppath.তথ্য(), wtmppath.আকার_2(), utmppath.তথ্য(), utmppath.আকার_2(), ফাঁকা, ফাঁকা) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#অন্যথা
	অক্ষর * var = std::getenv("TMPDIR");
	যদি (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		যখন (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.বের_করো_back();
		}
	} অন্যথা {
		tmpdir.assign("/tmp");
	}
#endif
	ফেরত tmpdir;
}

std::string make_অস্থায়ী_file(std::string template_str)
{
	আকার_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#যদি defined(__wasm)
	স্থির আকার_t সূচক = 0;
	template_str.replace(pos, 6, stringf("%06zu", সূচক++));
#নতুবা_যদি defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	যখন (1) {
		জন্য (পূর্ণসংখ্যা i = 0; i < 6; i++) {
			স্থির std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			স্থির uint32_t x = 314159265 ^ uint32_t(time(ফাঁকা));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.আকার_2()];
		}
		যদি (_access(template_str.c_str(), 0) != 0)
			বিরতি;
	}
#অন্যথা
	পূর্ণসংখ্যা suffixlen = template_str.আকার_2() - pos - 6;

	অক্ষর *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	মুক্ত(p);
#endif

	ফেরত template_str;
}

std::string make_অস্থায়ী_dir(std::string template_str)
{
#যদি defined(_WIN32)
	template_str = make_অস্থায়ী_file(template_str);
	mkdir(template_str.c_str());
	ফেরত template_str;
#নতুবা_যদি defined(__wasm)
	template_str = make_অস্থায়ী_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ফেরত template_str;
#অন্যথা
#  ifndef NDEBUG
	আকার_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	পূর্ণসংখ্যা suffixlen = template_str.আকার_2() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	অক্ষর *p = strdup(template_str.c_str());
	log_assert(p);
	অক্ষর *res = mkdtemp(p);
	যদি (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	মুক্ত(p);

	ফেরত template_str;
#endif
}

যৌক্তিক check_is_directory(ধ্রুবক std::string& dirname)
{
#যদি defined(_WIN32)
	কাঠামো _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	যদি (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.দৈর্ঘ্য() - 1);
	}

	যদি (_stat(dirname_.c_str(), &info) != 0)
	{
		ফেরত মিথ্যা;
	}
	ফেরত (info.st_mode & _S_IFDIR) != 0;
#অন্যথা
	কাঠামো stat info;
	যদি (stat(dirname.c_str(), &info) != 0)
	{
		ফেরত মিথ্যা;
	}
	ফেরত (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
যৌক্তিক check_accessible(ধ্রুবক std::string& filename, যৌক্তিক)
{
	ফেরত _access(filename.c_str(), 0) == 0;
}
#অন্যথা
যৌক্তিক check_accessible(ধ্রুবক std::string& filename, যৌক্তিক is_exec)
{
	ফেরত access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

যৌক্তিক check_file_exists(ধ্রুবক std::string& filename, যৌক্তিক is_exec)
{
	ফেরত check_accessible(filename, is_exec) && !check_is_directory(filename);
}
যৌক্তিক check_directory_exists(ধ্রুবক std::string& filename, যৌক্তিক is_exec)
{
	ফেরত check_accessible(filename, is_exec) && check_is_directory(filename);
}

যৌক্তিক is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ফেরত filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#অন্যথা
	ফেরত filename[0] == '/';
#endif
}

শূন্য remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#অন্যথা
	কাঠামো stat stbuf;
	কাঠামো dirent **namelist;
	পূর্ণসংখ্যা n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	জন্য (পূর্ণসংখ্যা i = 0; i < n; i++) {
		যদি (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string বাফার = stringf("%s/%s", dirname, namelist[i]->d_name);
			যদি (!stat(বাফার.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(বাফার.c_str());
			} অন্যথা
				remove_directory(বাফার);
		}
		মুক্ত(namelist[i]);
	}
	মুক্ত(namelist);
	rmdir(dirname.c_str());
#endif
}

যৌক্তিক create_directory(ধ্রুবক std::string& dirname)
{
#যদি defined(_WIN32)
	পূর্ণসংখ্যা ret = _mkdir(dirname.c_str());
#অন্যথা
	mode_t mode = 0755;
	পূর্ণসংখ্যা ret = mkdir(dirname.c_str(), mode);
#endif
	যদি (ret == 0)
		ফেরত সত্য;

	নির্বাচন (errno)
	{
	ক্ষেত্র ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::আকার_type pos = dirname.find_last_of('/');
			যদি (pos == std::string::npos)
#যদি defined(_WIN32)
				pos = dirname.find_last_of('\\');
			যদি (pos == std::string::npos)
#endif
				ফেরত মিথ্যা;
			যদি (!create_directory( dirname.substr(0, pos) ))
				ফেরত মিথ্যা;
		}
		// now, try to create again
#যদি defined(_WIN32)
		ফেরত 0 == _mkdir(dirname.c_str());
#অন্যথা
		ফেরত 0 == mkdir(dirname.c_str(), mode);
#endif

	ক্ষেত্র EEXIST:
		// done!
		ফেরত check_directory_exists(dirname);

	পূর্বনির্ধারিত:
		ফেরত মিথ্যা;
	}
}

যৌক্তিক make_private_directory(ধ্রুবক std::string& dirname)
{
#যদি defined(_WIN32) || defined(__wasi__)
	যদি (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ফেরত মিথ্যা;
	}
	ফেরত সত্য;
#অন্যথা
	যদি (mkdir(dirname.c_str(), 0700) == 0)
		ফেরত সত্য;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	পূর্ণসংখ্যা err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	যদি (err == ENOENT && !parent.empty() && create_directory(parent)) {
		যদি (mkdir(dirname.c_str(), 0700) == 0)
			ফেরত সত্য;
		err = errno;
	}

	যদি (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ফেরত মিথ্যা;
	}

	// Do not use a leftover that isn't ours
	কাঠামো stat st;
	যদি (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ফেরত মিথ্যা;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	যদি (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ফেরত মিথ্যা;
	}

	ফেরত সত্য;
#endif
}

std::string make_user_tmpdir(ধ্রুবক std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#যদি defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#অন্যথা
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ফেরত make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ধ্রুবক std::string& filename)
{
	std::string out;
	out.reserve(filename.আকার_2());
	জন্য (auto c : filename)
	{
		যদি (c == ' ')
			out += "\\ ";
		অন্যথা
			out.ঠেলো_back(c);
	}
	ফেরত out;
}

শূন্য append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ফেরত std::filesystem::path(path).filename().string();
}

std::string absolute_path(ধ্রুবক std::string& path)
{
	std::error_code ec;
	std::filesystem::path ফলাফল = std::filesystem::absolute(path, ec);

	যদি (ec)
		ফেরত "";

	ফলাফল = std::filesystem::weakly_canonical(ফলাফল, ec);

	যদি (ec)
		ফেরত "";

	ফেরত ফলাফল.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	যদি (parent.empty()) {
		ফেরত "";
	}
	// Add trailing separator to match original behavior
	std::string ফলাফল = parent.string();
	যদি (!ফলাফল.empty() && ফলাফল.back() != std::filesystem::path::preferred_separator) {
		ফলাফল += std::filesystem::path::preferred_separator;
	}
	ফেরত ফলাফল;
}

শূন্য format_emit_unescaped(std::string &ফলাফল, std::string_view fmt)
{
	ফলাফল.reserve(ফলাফল.আকার_2() + fmt.আকার_2());
	জন্য (আকার_t i = 0; i < fmt.আকার_2(); ++i) {
		অক্ষর ch = fmt[i];
		ফলাফল.ঠেলো_back(ch);
		যদি (ch == '%' && i + 1 < fmt.আকার_2() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string ফলাফল;
	format_emit_unescaped(ফলাফল, fmt);
	ফেরত ফলাফল;
}

স্থির std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string ফলাফল = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা spec_parameter_আকার(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	নির্বাচন (spec[spec.আকার_2() - 1]) {
	ক্ষেত্র 'd':
	ক্ষেত্র 'i':
	ক্ষেত্র 'o':
	ক্ষেত্র 'u':
	ক্ষেত্র 'x':
	ক্ষেত্র 'X':
		নির্বাচন (spec[spec.আকার_2() - 2]) {
		ক্ষেত্র 'h':
			যদি (spec[spec.আকার_2() - 3] == 'h')
				ফেরত আকার(অক্ষর);
			ফেরত আকার(short);
		ক্ষেত্র 'l':
			যদি (spec[spec.আকার_2() - 3] == 'l')
				ফেরত আকার(long long);
			ফেরত আকার(long);
		ক্ষেত্র 'L':
		ক্ষেত্র 'q':
			ফেরত আকার(long long);
		ক্ষেত্র 'j':
			ফেরত আকার(intmax_t);
		ক্ষেত্র 'z':
		ক্ষেত্র 'Z':
			ফেরত আকার(আকার_t);
		ক্ষেত্র 't':
			ফেরত আকার(ptrdiff_t);
		}
		ফেরত আকার(পূর্ণসংখ্যা);
	ক্ষেত্র 'e':
	ক্ষেত্র 'E':
	ক্ষেত্র 'f':
	ক্ষেত্র 'F':
	ক্ষেত্র 'g':
	ক্ষেত্র 'G':
	ক্ষেত্র 'a':
	ক্ষেত্র 'A':
		যদি (spec[spec.আকার_2() - 2] == 'L')
			ফেরত আকার(long দ্বিগুণ);
		যদি (spec[spec.আকার_2() - 2] == 'l' && spec[spec.আকার_2() - 3] == 'l')
			ফেরত আকার(long দ্বিগুণ);
		ফেরত আকার(দ্বিগুণ);
	ক্ষেত্র 'c':
		যদি (spec[spec.আকার_2() - 2] == 'l') {
			ফেরত আকার(wchar_t);
		}
		ফেরত আকার(unsigned অক্ষর);
	ক্ষেত্র 'C':
		ফেরত আকার(wchar_t);
	ক্ষেত্র 's':
	ক্ষেত্র 'p':
	ক্ষেত্র 'S':
	ক্ষেত্র 'n':
		ফেরত আকার(শূন্য *);
	ক্ষেত্র 'm':
		ফেরত আকার(পূর্ণসংখ্যা);
	পূর্বনির্ধারিত:
		ফেরত -1;
	}
}

template <typename Arg>
স্থির শূন্য format_emit_stringf(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	নির্বাচন (অঙ্ক_dynamic_ints) {
	ক্ষেত্র Dynamic_Int_গুনতি::NONE:
		ফলাফল += string_view_stringf(spec, arg);
		ফেরত;
	ক্ষেত্র Dynamic_Int_গুনতি::ONE:
		ফলাফল += string_view_stringf(spec, dynamic_ints[0], arg);
		ফেরত;
	ক্ষেত্র Dynamic_Int_গুনতি::TWO:
		ফলাফল += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ফেরত;
	}
	YOSYS_ABORT("Internal error");
}

শূন্য format_emit_long_long(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, long long arg)
{
	যদি (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ফলাফল += std::to_string(static_cast<পূর্ণসংখ্যা>(arg));
		ফেরত;
	}
	যদি (spec_parameter_আকার(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, (পূর্ণসংখ্যা)arg);
	} অন্যথা {
		format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
	}
}

শূন্য format_emit_unsigned_long_long(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, unsigned long long arg)
{
	যদি (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ফলাফল += std::to_string(static_cast<unsigned পূর্ণসংখ্যা>(arg));
		ফেরত;
	}
	যদি (spec == "%c") {
		ফলাফল += static_cast<অক্ষর>(arg);
		ফেরত;
	}
	যদি (spec_parameter_আকার(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, (unsigned পূর্ণসংখ্যা)arg);
	} অন্যথা {
		format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
	}
}

শূন্য format_emit_double(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, দ্বিগুণ arg)
{
	format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
}

শূন্য format_emit_char_ptr(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুবক অক্ষর *arg)
{
	যদি (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ফলাফল += arg;
		ফেরত;
	}
	format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
}

শূন্য format_emit_string(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুবক std::string &arg)
{
	যদি (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ফলাফল += arg;
		ফেরত;
	}
	format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg.c_str());
}

শূন্য format_emit_string_view(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, std::string_view arg)
{
	যদি (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		ফলাফল += arg;
		ফেরত;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, std::string(arg).c_str());
}

শূন্য format_emit_void_ptr(std::string &ফলাফল, std::string_view spec, পূর্ণসংখ্যা *dynamic_ints,
	Dynamic_Int_গুনতি অঙ্ক_dynamic_ints, ধ্রুবক শূন্য *arg)
{
	format_emit_stringf(ফলাফল, spec, dynamic_ints, অঙ্ক_dynamic_ints, arg);
}

#যদি defined(_WIN32)
ScopedFileLock::ScopedFileLock(ধ্রুবক std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ফাঁকা, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, ফাঁকা);
	যদি (h == INVALID_HANDLE_মান) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ফেরত;
	}
	OVERLAPPED ov = {};
	যদি (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ফেরত;
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
#নতুবা_যদি !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ধ্রুবক std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	যদি (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	যদি (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ফেরত;
	}
	পূর্ণসংখ্যা ret;
	যখন ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	যদি (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	যদি (fd >= 0)
		close(fd); // releases the flock
}
#অন্যথা
ScopedFileLock::ScopedFileLock(ধ্রুবক std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#যদি defined(_WIN32)
	ফেরত GetCurrentProcessId();
#নতুবা_যদি defined(__wasi__)
	ফেরত 0;
#অন্যথা
	ফেরত getpid();
#endif
}

YOSYS_NAMESPACE_END
