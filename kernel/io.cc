#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن <iostream>
#ضمن <string>
#ضمن <filesystem>
#ضمن <system_error>

#אם !defined(WIN32)
#ضمن <dirent.h>
#ضمن <unistd.h>
#אחרת
#ضمن <io.h>
#endif

#אם !defined(_WIN32) && !defined(__wasi__)
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
	אם (rc == 0) {
		صحيح c = f.get();
		אם (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	החזר rc;
}

std::string next_token(std::string &text, ثابت حرف *sep, منطقي long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	אם (pos_begin == std::string::npos)
		pos_begin = text.size();

	אם (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		עבור (size_t i = pos_begin+1; i < text.size(); i++) {
			אם (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				החזר token;
			}
			אם (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				החזר token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	אם (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	החזר token;
}

std::vector<std::string> split_tokens(ثابت std::string &text, ثابت حرف *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	עבור (حرف c : text) {
		אם (strchr(sep, c)) {
			אם (!current_token.empty()) {
				tokens.push_back(current_token);
				current_token.clear();
			}
		} אחרת
			current_token += c;
	}
	אם (!current_token.empty()) {
		tokens.push_back(current_token);
		current_token.clear();
	}
	החזר tokens;
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
	אם (*pattern == 0)
		החזר *string == 0;

	אם (*pattern == '\\') {
		אם (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			החזר אמת;
	}

	אם (*pattern == '?') {
		אם (*string == 0)
			החזר שקר;
		החזר patmatch(pattern+1, string+1);
	}

	אם (*pattern == '*') {
		כלעוד (*string) {
			אם (patmatch(pattern+1, string++))
				החזר אמת;
		}
		החזר pattern[1] == 0;
	}

	אם (*pattern == '[') {
		منطقي found_match = שקר;
		منطقي inverted_list = pattern[1] == '!';
		ثابت حرف *p = pattern + (inverted_list ? 1 : 0);

		כלעוד (*++p) {
			אם (*p == ']') {
				אם (found_match != inverted_list && patmatch(p+1, string+1))
					החזר אמת;
				اكسر;
			}

			אם (*p == '\\') {
				אם (*++p == *string)
					found_match = אמת;
			} אחרת
			אם (*p == *string)
				found_match = אמת;
		}
	}

	אם (*pattern == *string)
		החזר patmatch(pattern+1, string+1);

	החזר שקר;
}

std::string get_base_tmpdir()
{
	ساكن std::string tmpdir;

	אם (!tmpdir.empty()) {
		החזר tmpdir;
	}

#אם defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	אם (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), عدم, 0, عدم, عدم));
	אם (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), عدم, عدم) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#אחרת
	حرف * var = std::getenv("TMPDIR");
	אם (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		כלעוד (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.pop_back();
		}
	} אחרת {
		tmpdir.assign("/tmp");
	}
#endif
	החזר tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#אם defined(__wasm)
	ساكن size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#وإلاإذا defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	כלעוד (1) {
		עבור (صحيح i = 0; i < 6; i++) {
			ساكن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساكن uint32_t x = 314159265 ^ uint32_t(time(عدم));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		אם (_access(template_str.c_str(), 0) != 0)
			اكسر;
	}
#אחרת
	صحيح suffixlen = template_str.size() - pos - 6;

	حرف *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	حرر(p);
#endif

	החזר template_str;
}

std::string make_temp_dir(std::string template_str)
{
#אם defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	החזר template_str;
#وإلاإذا defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	החזר template_str;
#אחרת
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحيح suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	حرف *p = strdup(template_str.c_str());
	log_assert(p);
	حرف *res = mkdtemp(p);
	אם (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	حرر(p);

	החזר template_str;
#endif
}

منطقي check_is_directory(ثابت std::string& dirname)
{
#אם defined(_WIN32)
	بنية _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	אם (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	אם (_stat(dirname_.c_str(), &info) != 0)
	{
		החזר שקר;
	}
	החזר (info.st_mode & _S_IFDIR) != 0;
#אחרת
	بنية stat info;
	אם (stat(dirname.c_str(), &info) != 0)
	{
		החזר שקר;
	}
	החזר (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
منطقي check_accessible(ثابت std::string& filename, منطقي)
{
	החזר _access(filename.c_str(), 0) == 0;
}
#אחרת
منطقي check_accessible(ثابت std::string& filename, منطقي is_exec)
{
	החזר access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

منطقي check_file_exists(ثابت std::string& filename, منطقي is_exec)
{
	החזר check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقي check_directory_exists(ثابت std::string& filename, منطقي is_exec)
{
	החזר check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقي is_absolute_path(std::string filename)
{
#ifdef _WIN32
	החזר filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#אחרת
	החזר filename[0] == '/';
#endif
}

فراغ remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#אחרת
	بنية stat stbuf;
	بنية dirent **namelist;
	صحيح n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	עבור (صحيح i = 0; i < n; i++) {
		אם (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			אם (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} אחרת
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
#אם defined(_WIN32)
	صحيح ret = _mkdir(dirname.c_str());
#אחרת
	mode_t mode = 0755;
	صحيح ret = mkdir(dirname.c_str(), mode);
#endif
	אם (ret == 0)
		החזר אמת;

	بدل (errno)
	{
	حالة ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			אם (pos == std::string::npos)
#אם defined(_WIN32)
				pos = dirname.find_last_of('\\');
			אם (pos == std::string::npos)
#endif
				החזר שקר;
			אם (!create_directory( dirname.substr(0, pos) ))
				החזר שקר;
		}
		// now, try to create again
#אם defined(_WIN32)
		החזר 0 == _mkdir(dirname.c_str());
#אחרת
		החזר 0 == mkdir(dirname.c_str(), mode);
#endif

	حالة EEXIST:
		// done!
		החזר check_directory_exists(dirname);

	افتراضي:
		החזר שקר;
	}
}

منطقي make_private_directory(ثابت std::string& dirname)
{
#אם defined(_WIN32) || defined(__wasi__)
	אם (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		החזר שקר;
	}
	החזר אמת;
#אחרת
	אם (mkdir(dirname.c_str(), 0700) == 0)
		החזר אמת;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحيح err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	אם (err == ENOENT && !parent.empty() && create_directory(parent)) {
		אם (mkdir(dirname.c_str(), 0700) == 0)
			החזר אמת;
		err = errno;
	}

	אם (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		החזר שקר;
	}

	// Do not use a leftover that isn't ours
	بنية stat st;
	אם (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		החזר שקר;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	אם (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		החזר שקר;
	}

	החזר אמת;
#endif
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#אם defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#אחרת
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	החזר make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	עבור (auto c : filename)
	{
		אם (c == ' ')
			out += "\\ ";
		אחרת
			out.push_back(c);
	}
	החזר out;
}

فراغ append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	החזר std::filesystem::path(path).filename().string();
}

std::string absolute_path(ثابت std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	אם (ec)
		החזר "";

	result = std::filesystem::weakly_canonical(result, ec);

	אם (ec)
		החזר "";

	החזר result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	אם (parent.empty()) {
		החזר "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	אם (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	החזר result;
}

فراغ format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	עבור (size_t i = 0; i < fmt.size(); ++i) {
		حرف ch = fmt[i];
		result.push_back(ch);
		אם (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	החזר result;
}

ساكن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	החזר result;
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
			אם (spec[spec.size() - 3] == 'h')
				החזר حجم(حرف);
			החזר حجم(short);
		حالة 'l':
			אם (spec[spec.size() - 3] == 'l')
				החזר حجم(long long);
			החזר حجم(long);
		حالة 'L':
		حالة 'q':
			החזר حجم(long long);
		حالة 'j':
			החזר حجم(intmax_t);
		حالة 'z':
		حالة 'Z':
			החזר حجم(size_t);
		حالة 't':
			החזר حجم(ptrdiff_t);
		}
		החזר حجم(صحيح);
	حالة 'e':
	حالة 'E':
	حالة 'f':
	حالة 'F':
	حالة 'g':
	حالة 'G':
	حالة 'a':
	حالة 'A':
		אם (spec[spec.size() - 2] == 'L')
			החזר حجم(long مزدوج);
		אם (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			החזר حجم(long مزدوج);
		החזר حجم(مزدوج);
	حالة 'c':
		אם (spec[spec.size() - 2] == 'l') {
			החזר حجم(wchar_t);
		}
		החזר حجم(unsigned حرف);
	حالة 'C':
		החזר حجم(wchar_t);
	حالة 's':
	حالة 'p':
	حالة 'S':
	حالة 'n':
		החזר حجم(فراغ *);
	حالة 'm':
		החזר حجم(صحيح);
	افتراضي:
		החזר -1;
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
		החזר;
	حالة DynamicIntCount::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		החזר;
	حالة DynamicIntCount::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		החזר;
	}
	YOSYS_ABORT("Internal error");
}

فراغ format_emit_long_long(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, long long arg)
{
	אם (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<صحيح>(arg));
		החזר;
	}
	אם (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (صحيح)arg);
	} אחרת {
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
	}
}

فراغ format_emit_unsigned_long_long(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, unsigned long long arg)
{
	אם (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned صحيح>(arg));
		החזר;
	}
	אם (spec == "%c") {
		result += static_cast<حرف>(arg);
		החזר;
	}
	אם (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, (unsigned صحيح)arg);
	} אחרת {
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
	אם (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		החזר;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg);
}

فراغ format_emit_string(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, ثابت std::string &arg)
{
	אם (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		החזר;
	}
	format_emit_stringf(result, spec, dynamic_ints, num_dynamic_ints, arg.c_str());
}

فراغ format_emit_string_view(std::string &result, std::string_view spec, صحيح *dynamic_ints,
	DynamicIntCount num_dynamic_ints, std::string_view arg)
{
	אם (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		החזר;
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

#אם defined(_WIN32)
ScopedFileLock::ScopedFileLock(ثابت std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			عدم, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, عدم);
	אם (h == INVALID_HANDLE_VALUE) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		החזר;
	}
	OVERLAPPED ov = {};
	אם (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		החזר;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	אם (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وإلاإذا !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ثابت std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	אם (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	אם (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		החזר;
	}
	صحيح ret;
	כלעוד ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	אם (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	אם (fd >= 0)
		close(fd); // releases the flock
}
#אחרת
ScopedFileLock::ScopedFileLock(ثابت std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#אם defined(_WIN32)
	החזר GetCurrentProcessId();
#وإلاإذا defined(__wasi__)
	החזר 0;
#אחרת
	החזר getpid();
#endif
}

YOSYS_NAMESPACE_END
