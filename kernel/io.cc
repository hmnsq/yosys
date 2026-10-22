#شامل "kernel/yosys_common.h"
#شامل "kernel/log.h"
#شامل <iostream>
#شامل <string>
#شامل <filesystem>
#شامل <system_error>

#اگر !defined(WIN32)
#شامل <dirent.h>
#شامل <unistd.h>
#وگرنه
#شامل <io.h>
#endif

#اگر !defined(_WIN32) && !defined(__wasi__)
#شامل <sys/file.h>
#شامل <fcntl.h>
#شامل <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

صحیح readsome(std::istream &f, نویسه *s, صحیح n)
{
	صحیح rc = صحیح(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	اگر (rc == 0) {
		صحیح c = f.get();
		اگر (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	بازگشت rc;
}

std::string next_token(std::string &text, ثابت نویسه *sep, منطقی long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	اگر (pos_begin == std::string::npos)
		pos_begin = text.size();

	اگر (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		برای (size_t i = pos_begin+1; i < text.size(); i++) {
			اگر (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				بازگشت token;
			}
			اگر (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				بازگشت token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	اگر (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	بازگشت token;
}

std::vector<std::string> split_tokens(ثابت std::string &text, ثابت نویسه *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	برای (نویسه c : text) {
		اگر (strchr(sep, c)) {
			اگر (!current_token.empty()) {
				tokens.افزودن_back(current_token);
				current_token.clear();
			}
		} وگرنه
			current_token += c;
	}
	اگر (!current_token.empty()) {
		tokens.افزودن_back(current_token);
		current_token.clear();
	}
	بازگشت tokens;
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
منطقی patmatch(ثابت نویسه *pattern, ثابت نویسه *string)
{
	اگر (*pattern == 0)
		بازگشت *string == 0;

	اگر (*pattern == '\\') {
		اگر (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			بازگشت درست;
	}

	اگر (*pattern == '?') {
		اگر (*string == 0)
			بازگشت نادرست;
		بازگشت patmatch(pattern+1, string+1);
	}

	اگر (*pattern == '*') {
		وقتی (*string) {
			اگر (patmatch(pattern+1, string++))
				بازگشت درست;
		}
		بازگشت pattern[1] == 0;
	}

	اگر (*pattern == '[') {
		منطقی found_match = نادرست;
		منطقی inverted_list = pattern[1] == '!';
		ثابت نویسه *p = pattern + (inverted_list ? 1 : 0);

		وقتی (*++p) {
			اگر (*p == ']') {
				اگر (found_match != inverted_list && patmatch(p+1, string+1))
					بازگشت درست;
				بشکن;
			}

			اگر (*p == '\\') {
				اگر (*++p == *string)
					found_match = درست;
			} وگرنه
			اگر (*p == *string)
				found_match = درست;
		}
	}

	اگر (*pattern == *string)
		بازگشت patmatch(pattern+1, string+1);

	بازگشت نادرست;
}

std::string get_base_tmpdir()
{
	ایستا std::string tmpdir;

	اگر (!tmpdir.empty()) {
		بازگشت tmpdir;
	}

#اگر defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	اگر (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), پوچ, 0, پوچ, پوچ));
	اگر (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), پوچ, پوچ) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#وگرنه
	نویسه * var = std::getenv("TMPDIR");
	اگر (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		وقتی (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.برداشت_back();
		}
	} وگرنه {
		tmpdir.assign("/tmp");
	}
#endif
	بازگشت tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#اگر defined(__wasm)
	ایستا size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#وگرنهاگر defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	وقتی (1) {
		برای (صحیح i = 0; i < 6; i++) {
			ایستا std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ایستا uint32_t x = 314159265 ^ uint32_t(time(پوچ));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		اگر (_access(template_str.c_str(), 0) != 0)
			بشکن;
	}
#وگرنه
	صحیح suffixlen = template_str.size() - pos - 6;

	نویسه *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	آزاد(p);
#endif

	بازگشت template_str;
}

std::string make_temp_dir(std::string template_str)
{
#اگر defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	بازگشت template_str;
#وگرنهاگر defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	بازگشت template_str;
#وگرنه
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحیح suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	نویسه *p = strdup(template_str.c_str());
	log_assert(p);
	نویسه *res = mkdtemp(p);
	اگر (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	آزاد(p);

	بازگشت template_str;
#endif
}

منطقی check_is_directory(ثابت std::string& dirname)
{
#اگر defined(_WIN32)
	ساختار _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	اگر (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	اگر (_stat(dirname_.c_str(), &info) != 0)
	{
		بازگشت نادرست;
	}
	بازگشت (info.st_mode & _S_IFDIR) != 0;
#وگرنه
	ساختار stat info;
	اگر (stat(dirname.c_str(), &info) != 0)
	{
		بازگشت نادرست;
	}
	بازگشت (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
منطقی check_accessible(ثابت std::string& filename, منطقی)
{
	بازگشت _access(filename.c_str(), 0) == 0;
}
#وگرنه
منطقی check_accessible(ثابت std::string& filename, منطقی is_exec)
{
	بازگشت access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

منطقی check_file_exists(ثابت std::string& filename, منطقی is_exec)
{
	بازگشت check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقی check_directory_exists(ثابت std::string& filename, منطقی is_exec)
{
	بازگشت check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقی is_absolute_path(std::string filename)
{
#ifdef _WIN32
	بازگشت filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#وگرنه
	بازگشت filename[0] == '/';
#endif
}

تهی remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#وگرنه
	ساختار stat stbuf;
	ساختار dirent **namelist;
	صحیح n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	برای (صحیح i = 0; i < n; i++) {
		اگر (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			اگر (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} وگرنه
				remove_directory(buffer);
		}
		آزاد(namelist[i]);
	}
	آزاد(namelist);
	rmdir(dirname.c_str());
#endif
}

منطقی create_directory(ثابت std::string& dirname)
{
#اگر defined(_WIN32)
	صحیح ret = _mkdir(dirname.c_str());
#وگرنه
	mode_t mode = 0755;
	صحیح ret = mkdir(dirname.c_str(), mode);
#endif
	اگر (ret == 0)
		بازگشت درست;

	انتخاب (errno)
	{
	حالت ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			اگر (pos == std::string::npos)
#اگر defined(_WIN32)
				pos = dirname.find_last_of('\\');
			اگر (pos == std::string::npos)
#endif
				بازگشت نادرست;
			اگر (!create_directory( dirname.substr(0, pos) ))
				بازگشت نادرست;
		}
		// now, try to create again
#اگر defined(_WIN32)
		بازگشت 0 == _mkdir(dirname.c_str());
#وگرنه
		بازگشت 0 == mkdir(dirname.c_str(), mode);
#endif

	حالت EEXIST:
		// done!
		بازگشت check_directory_exists(dirname);

	پیشفرض:
		بازگشت نادرست;
	}
}

منطقی make_private_directory(ثابت std::string& dirname)
{
#اگر defined(_WIN32) || defined(__wasi__)
	اگر (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		بازگشت نادرست;
	}
	بازگشت درست;
#وگرنه
	اگر (mkdir(dirname.c_str(), 0700) == 0)
		بازگشت درست;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحیح err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	اگر (err == ENOENT && !parent.empty() && create_directory(parent)) {
		اگر (mkdir(dirname.c_str(), 0700) == 0)
			بازگشت درست;
		err = errno;
	}

	اگر (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		بازگشت نادرست;
	}

	// Do not use a leftover that isn't ours
	ساختار stat st;
	اگر (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		بازگشت نادرست;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	اگر (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		بازگشت نادرست;
	}

	بازگشت درست;
#endif
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#اگر defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#وگرنه
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	بازگشت make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	برای (auto c : filename)
	{
		اگر (c == ' ')
			out += "\\ ";
		وگرنه
			out.افزودن_back(c);
	}
	بازگشت out;
}

تهی append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	بازگشت std::filesystem::path(path).filename().string();
}

std::string absolute_path(ثابت std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	اگر (ec)
		بازگشت "";

	result = std::filesystem::weakly_canonical(result, ec);

	اگر (ec)
		بازگشت "";

	بازگشت result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	اگر (parent.empty()) {
		بازگشت "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	اگر (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	بازگشت result;
}

تهی format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	برای (size_t i = 0; i < fmt.size(); ++i) {
		نویسه ch = fmt[i];
		result.افزودن_back(ch);
		اگر (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	بازگشت result;
}

ایستا std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	بازگشت result;
}

ایستا صحیح spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	انتخاب (spec[spec.size() - 1]) {
	حالت 'd':
	حالت 'i':
	حالت 'o':
	حالت 'u':
	حالت 'x':
	حالت 'X':
		انتخاب (spec[spec.size() - 2]) {
		حالت 'h':
			اگر (spec[spec.size() - 3] == 'h')
				بازگشت اندازه(نویسه);
			بازگشت اندازه(short);
		حالت 'l':
			اگر (spec[spec.size() - 3] == 'l')
				بازگشت اندازه(long long);
			بازگشت اندازه(long);
		حالت 'L':
		حالت 'q':
			بازگشت اندازه(long long);
		حالت 'j':
			بازگشت اندازه(intmax_t);
		حالت 'z':
		حالت 'Z':
			بازگشت اندازه(size_t);
		حالت 't':
			بازگشت اندازه(ptrdiff_t);
		}
		بازگشت اندازه(صحیح);
	حالت 'e':
	حالت 'E':
	حالت 'f':
	حالت 'F':
	حالت 'g':
	حالت 'G':
	حالت 'a':
	حالت 'A':
		اگر (spec[spec.size() - 2] == 'L')
			بازگشت اندازه(long دوگانه);
		اگر (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			بازگشت اندازه(long دوگانه);
		بازگشت اندازه(دوگانه);
	حالت 'c':
		اگر (spec[spec.size() - 2] == 'l') {
			بازگشت اندازه(wchar_t);
		}
		بازگشت اندازه(unsigned نویسه);
	حالت 'C':
		بازگشت اندازه(wchar_t);
	حالت 's':
	حالت 'p':
	حالت 'S':
	حالت 'n':
		بازگشت اندازه(تهی *);
	حالت 'm':
		بازگشت اندازه(صحیح);
	پیشفرض:
		بازگشت -1;
	}
}

template <typename Arg>
ایستا تهی format_emit_stringf(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	انتخاب (رقم_dynamic_ints) {
	حالت Dynamic_Int_شمار::NONE:
		result += string_view_stringf(spec, arg);
		بازگشت;
	حالت Dynamic_Int_شمار::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		بازگشت;
	حالت Dynamic_Int_شمار::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		بازگشت;
	}
	YOSYS_ABORT("Internal error");
}

تهی format_emit_long_long(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, long long arg)
{
	اگر (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<صحیح>(arg));
		بازگشت;
	}
	اگر (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, (صحیح)arg);
	} وگرنه {
		format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, arg);
	}
}

تهی format_emit_unsigned_long_long(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, unsigned long long arg)
{
	اگر (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned صحیح>(arg));
		بازگشت;
	}
	اگر (spec == "%c") {
		result += static_cast<نویسه>(arg);
		بازگشت;
	}
	اگر (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, (unsigned صحیح)arg);
	} وگرنه {
		format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, arg);
	}
}

تهی format_emit_double(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, دوگانه arg)
{
	format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, arg);
}

تهی format_emit_char_ptr(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت نویسه *arg)
{
	اگر (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		بازگشت;
	}
	format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, arg);
}

تهی format_emit_string(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت std::string &arg)
{
	اگر (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		بازگشت;
	}
	format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, arg.c_str());
}

تهی format_emit_string_view(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, std::string_view arg)
{
	اگر (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		بازگشت;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, std::string(arg).c_str());
}

تهی format_emit_void_ptr(std::string &result, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت تهی *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, رقم_dynamic_ints, arg);
}

#اگر defined(_WIN32)
ScopedFileLock::ScopedFileLock(ثابت std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			پوچ, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, پوچ);
	اگر (h == INVALID_HANDLE_مقدار) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		بازگشت;
	}
	OVERLAPPED ov = {};
	اگر (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		بازگشت;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	اگر (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وگرنهاگر !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ثابت std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	اگر (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	اگر (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		بازگشت;
	}
	صحیح ret;
	وقتی ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	اگر (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	اگر (fd >= 0)
		close(fd); // releases the flock
}
#وگرنه
ScopedFileLock::ScopedFileLock(ثابت std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#اگر defined(_WIN32)
	بازگشت GetCurrentProcessId();
#وگرنهاگر defined(__wasi__)
	بازگشت 0;
#وگرنه
	بازگشت getpid();
#endif
}

YOSYS_NAMESPACE_END
