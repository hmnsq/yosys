#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن <iostream>
#ضمن <string>
#ضمن <filesystem>
#ضمن <system_error>

#ܐܢ !defined(WIN32)
#ضمن <dirent.h>
#ضمن <unistd.h>
#ܐܚܪܢܐ
#ضمن <io.h>
#endif

#ܐܢ !defined(_WIN32) && !defined(__wasi__)
#ضمن <sys/file.h>
#ضمن <fcntl.h>
#ضمن <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

صحيح readsome(std::istream &f, حرف *s, صحيح n)
{
	صحيح rc = صحيح(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ܐܢ (rc == 0) {
		صحيح c = f.get();
		ܐܢ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ܦܢܐ rc;
}

std::string next_token(std::string &text, ثابت حرف *sep, منطقي long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ܐܢ (pos_begin == std::string::npos)
		pos_begin = text.size();

	ܐܢ (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		لكل (size_t i = pos_begin+1; i < text.size(); i++) {
			ܐܢ (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ܦܢܐ token;
			}
			ܐܢ (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ܦܢܐ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ܐܢ (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ܦܢܐ token;
}

std::vector<std::string> split_tokens(ثابت std::string &text, ثابت حرف *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	لكل (حرف c : text) {
		ܐܢ (strchr(sep, c)) {
			ܐܢ (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} ܐܚܪܢܐ
			current_token += c;
	}
	ܐܢ (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	ܦܢܐ tokens;
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
منطقي patmatch(ثابت حرف *pattern, ثابت حرف *string)
{
	ܐܢ (*pattern == 0)
		ܦܢܐ *string == 0;

	ܐܢ (*pattern == '\\') {
		ܐܢ (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ܦܢܐ ܫܪܪܐ;
	}

	ܐܢ (*pattern == '?') {
		ܐܢ (*string == 0)
			ܦܢܐ ܕܓܠܐ;
		ܦܢܐ patmatch(pattern+1, string+1);
	}

	ܐܢ (*pattern == '*') {
		ܟܡܐ (*string) {
			ܐܢ (patmatch(pattern+1, string++))
				ܦܢܐ ܫܪܪܐ;
		}
		ܦܢܐ pattern[1] == 0;
	}

	ܐܢ (*pattern == '[') {
		منطقي found_match = ܕܓܠܐ;
		منطقي inverted_list = pattern[1] == '!';
		ثابت حرف *p = pattern + (inverted_list ? 1 : 0);

		ܟܡܐ (*++p) {
			ܐܢ (*p == ']') {
				ܐܢ (found_match != inverted_list && patmatch(p+1, string+1))
					ܦܢܐ ܫܪܪܐ;
				اكسر;
			}

			ܐܢ (*p == '\\') {
				ܐܢ (*++p == *string)
					found_match = ܫܪܪܐ;
			} ܐܚܪܢܐ
			ܐܢ (*p == *string)
				found_match = ܫܪܪܐ;
		}
	}

	ܐܢ (*pattern == *string)
		ܦܢܐ patmatch(pattern+1, string+1);

	ܦܢܐ ܕܓܠܐ;
}

std::string get_base_tmpdir()
{
	ساكن std::string tmpdir;

	ܐܢ (!tmpdir.empty()) {
		ܦܢܐ tmpdir;
	}

#ܐܢ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ܐܢ (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), عدم, 0, عدم, عدم));
	ܐܢ (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), عدم, عدم) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ܐܚܪܢܐ
	حرف * var = std::getenv("TMPDIR");
	ܐܢ (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ܟܡܐ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} ܐܚܪܢܐ {
		tmpdir.assign("/tmp");
	}
#endif
	ܦܢܐ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ܐܢ defined(__wasm)
	ساكن size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#وإلاإذا defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ܟܡܐ (1) {
		لكل (صحيح i = 0; i < 6; i++) {
			ساكن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساكن uint32_t x = 314159265 ^ uint32_t(time(عدم));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ܐܢ (_access(template_str.c_str(), 0) != 0)
			اكسر;
	}
#ܐܚܪܢܐ
	صحيح suffixlen = template_str.size() - pos - 6;

	حرف *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	حرر(p);
#endif

	ܦܢܐ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ܐܢ defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ܦܢܐ template_str;
#وإلاإذا defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ܦܢܐ template_str;
#ܐܚܪܢܐ
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحيح suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	حرف *p = strdup(template_str.c_str());
	log_assert(p);
	حرف *res = mkdtemp(p);
	ܐܢ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	حرر(p);

	ܦܢܐ template_str;
#endif
}

منطقي check_is_directory(ثابت std::string& dirname)
{
#ܐܢ defined(_WIN32)
	بنية _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ܐܢ (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ܐܢ (_stat(dirname_.c_str(), &info) != 0)
	{
		ܦܢܐ ܕܓܠܐ;
	}
	ܦܢܐ (info.st_mode & _S_IFDIR) != 0;
#ܐܚܪܢܐ
	بنية stat info;
	ܐܢ (stat(dirname.c_str(), &info) != 0)
	{
		ܦܢܐ ܕܓܠܐ;
	}
	ܦܢܐ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
منطقي check_accessible(ثابت std::string& filename, منطقي)
{
	ܦܢܐ _access(filename.c_str(), 0) == 0;
}
#ܐܚܪܢܐ
منطقي check_accessible(ثابت std::string& filename, منطقي is_exec)
{
	ܦܢܐ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

منطقي check_file_exists(ثابت std::string& filename, منطقي is_exec)
{
	ܦܢܐ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقي check_directory_exists(ثابت std::string& filename, منطقي is_exec)
{
	ܦܢܐ check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقي is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ܦܢܐ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ܐܚܪܢܐ
	ܦܢܐ filename[0] == '/';
#endif
}

فراغ remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#ܐܚܪܢܐ
	بنية stat stbuf;
	بنية dirent **namelist;
	صحيح n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	لكل (صحيح i = 0; i < n; i++) {
		ܐܢ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ܐܢ (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} ܐܚܪܢܐ
				remove_directory(buffer);
		}
		حرر(namelist[i]);
	}
	حرر(namelist);
	rmdir(dirname.c_str());
#endif
}

منطقي create_directory(ثابت std::string& dirname)
{
#ܐܢ defined(_WIN32)
	صحيح ret = _mkdir(dirname.c_str());
#ܐܚܪܢܐ
	mode_t mode = 0755;
	صحيح ret = mkdir(dirname.c_str(), mode);
#endif
	ܐܢ (ret == 0)
		ܦܢܐ ܫܪܪܐ;

	بدل (errno)
	{
	حالة ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ܐܢ (pos == std::string::npos)
#ܐܢ defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ܐܢ (pos == std::string::npos)
#endif
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (!create_directory( dirname.substr(0, pos) ))
				ܦܢܐ ܕܓܠܐ;
		}
		// now, try to create again
#ܐܢ defined(_WIN32)
		ܦܢܐ 0 == _mkdir(dirname.c_str());
#ܐܚܪܢܐ
		ܦܢܐ 0 == mkdir(dirname.c_str(), mode);
#endif

	حالة EEXIST:
		// done!
		ܦܢܐ check_directory_exists(dirname);

	افتراضي:
		ܦܢܐ ܕܓܠܐ;
	}
}

منطقي make_private_directory(ثابت std::string& dirname)
{
#ܐܢ defined(_WIN32) || defined(__wasi__)
	ܐܢ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ܦܢܐ ܕܓܠܐ;
	}
	ܦܢܐ ܫܪܪܐ;
#ܐܚܪܢܐ
	ܐܢ (mkdir(dirname.c_str(), 0700) == 0)
		ܦܢܐ ܫܪܪܐ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحيح err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ܐܢ (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ܐܢ (mkdir(dirname.c_str(), 0700) == 0)
			ܦܢܐ ܫܪܪܐ;
		err = errno;
	}

	ܐܢ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ܦܢܐ ܕܓܠܐ;
	}

	// Do not use a leftover that isn't ours
	بنية stat st;
	ܐܢ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ܦܢܐ ܕܓܠܐ;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ܐܢ (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ܦܢܐ ܕܓܠܐ;
	}

	ܦܢܐ ܫܪܪܐ;
#endif
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ܐܢ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#ܐܚܪܢܐ
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ܦܢܐ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	لكل (auto c : filename)
	{
		ܐܢ (c == ' ')
			out += "\\ ";
		ܐܚܪܢܐ
			out.push_back(c);
	}
	ܦܢܐ out;
}

فراغ append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ܦܢܐ std::filesystem::path(path).filename().string();
}

std::string absolute_path(ثابت std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ܐܢ (ec)
		ܦܢܐ "";

	result = std::filesystem::weakly_canonical(result, ec);

	ܐܢ (ec)
		ܦܢܐ "";

	ܦܢܐ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ܐܢ (parent.empty()) {
		ܦܢܐ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ܐܢ (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ܦܢܐ result;
}

فراغ format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	لكل (size_t i = 0; i < fmt.size(); ++i) {
		حرف ch = fmt[i];
		result.push_back(ch);
		ܐܢ (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ܦܢܐ result;
}

ساكن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ܦܢܐ result;
}

ساكن صحيح spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	بدل (spec[spec.size() - 1]) {
	حالة 'd':
	حالة 'i':
	حالة 'o':
	حالة 'u':
	حالة 'x':
	حالة 'X':
		بدل (spec[spec.size() - 2]) {
		حالة 'h':
			ܐܢ (spec[spec.size() - 3] == 'h')
				ܦܢܐ حجم(حرف);
			ܦܢܐ حجم(short);
		حالة 'l':
			ܐܢ (spec[spec.size() - 3] == 'l')
				ܦܢܐ حجم(long long);
			ܦܢܐ حجم(long);
		حالة 'L':
		حالة 'q':
			ܦܢܐ حجم(long long);
		حالة 'j':
			ܦܢܐ حجم(intmax_t);
		حالة 'z':
		حالة 'Z':
			ܦܢܐ حجم(size_t);
		حالة 't':
			ܦܢܐ حجم(ptrdiff_t);
		}
		ܦܢܐ حجم(صحيح);
	حالة 'e':
	حالة 'E':
	حالة 'f':
	حالة 'F':
	حالة 'g':
	حالة 'G':
	حالة 'a':
	حالة 'A':
		ܐܢ (spec[spec.size() - 2] == 'L')
			ܦܢܐ حجم(long مزدوج);
		ܐܢ (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ܦܢܐ حجم(long مزدوج);
		ܦܢܐ حجم(مزدوج);
	حالة 'c':
		ܐܢ (spec[spec.size() - 2] == 'l') {
			ܦܢܐ حجم(wchar_t);
		}
		ܦܢܐ حجم(unsigned حرف);
	حالة 'C':
		ܦܢܐ حجم(wchar_t);
	حالة 's':
	حالة 'p':
	حالة 'S':
	حالة 'n':
		ܦܢܐ حجم(فراغ *);
	حالة 'm':
		ܦܢܐ حجم(صحيح);
	افتراضي:
		ܦܢܐ -1;
	}
}

template <typename Arg>
ساكن فراغ format_emit_stringf(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	بدل (num_dynamic_ints) {
	حالة DynamicIntCount::NONE:
		result += string_view_stringf(spec, arg);
		ܦܢܐ;
	حالة DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ܦܢܐ;
	حالة DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ܦܢܐ;
	}
	YOSYS_ABORT("Internal error");
}

فراغ format_emit_long_long(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	ܐܢ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<صحيح>(arg));
		ܦܢܐ;
	}
	ܐܢ (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (صحيح)arg);
	} ܐܚܪܢܐ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

فراغ format_emit_unsigned_long_long(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	ܐܢ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned صحيح>(arg));
		ܦܢܐ;
	}
	ܐܢ (spec == "%c") {
		result += static_cast<حرف>(arg);
		ܦܢܐ;
	}
	ܐܢ (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned صحيح)arg);
	} ܐܚܪܢܐ {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

فراغ format_emit_double(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, مزدوج arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

فراغ format_emit_char_ptr(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, ثابت حرف *arg)
{
	ܐܢ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ܦܢܐ;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

فراغ format_emit_string(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, ثابت std::string &arg)
{
	ܐܢ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ܦܢܐ;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

فراغ format_emit_string_view(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	ܐܢ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ܦܢܐ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, std::string(arg).c_str());
}

فراغ format_emit_void_ptr(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, ثابت فراغ *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

#ܐܢ defined(_WIN32)
ScopedFileLock::ScopedFileLock(ثابت std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			عدم, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, عدم);
	ܐܢ (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ܦܢܐ;
	}
	OVERLAPPED ov = {};
	ܐܢ (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ܦܢܐ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ܐܢ (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وإلاإذا !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ثابت std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ܐܢ (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ܐܢ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ܦܢܐ;
	}
	صحيح ret;
	ܟܡܐ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ܐܢ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ܐܢ (fd >= 0)
		close(fd); // releases the flock
}
#ܐܚܪܢܐ
ScopedFileLock::ScopedFileLock(ثابت std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ܐܢ defined(_WIN32)
	ܦܢܐ GetCurrentProcessId();
#وإلاإذا defined(__wasi__)
	ܦܢܐ 0;
#ܐܚܪܢܐ
	ܦܢܐ getpid();
#endif
}

YOSYS_NAMESPACE_END
