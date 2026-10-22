#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/log.h"
#அந்தர்பாவ <iostream>
#அந்தர்பாவ <string>
#அந்தர்பாவ <filesystem>
#அந்தர்பாவ <system_error>

#எனில் !defined(WIN32)
#அந்தர்பாவ <dirent.h>
#அந்தர்பாவ <unistd.h>
#இல்லையெனில்
#அந்தர்பாவ <io.h>
#endif

#எனில் !defined(_WIN32) && !defined(__wasi__)
#அந்தர்பாவ <sys/file.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

முழு readsome(std::istream &f, எழுத்து *s, முழு n)
{
	முழு rc = முழு(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	எனில் (rc == 0) {
		முழு c = f.get();
		எனில் (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	திரும்பு rc;
}

std::string next_token(std::string &text, மாறிலி எழுத்து *sep, தருக்க long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	எனில் (pos_begin == std::string::npos)
		pos_begin = text.size();

	எனில் (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		க்கு (size_t i = pos_begin+1; i < text.size(); i++) {
			எனில் (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				திரும்பு token;
			}
			எனில் (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				திரும்பு token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	எனில் (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	திரும்பு token;
}

std::vector<std::string> split_tokens(மாறிலி std::string &text, மாறிலி எழுத்து *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	க்கு (எழுத்து c : text) {
		எனில் (strchr(sep, c)) {
			எனில் (!current_token.empty()) {
				tokens.தள்ளு_back(current_token);
				current_token.clear();
			}
		} இல்லையெனில்
			current_token += c;
	}
	எனில் (!current_token.empty()) {
		tokens.தள்ளு_back(current_token);
		current_token.clear();
	}
	திரும்பு tokens;
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
தருக்க patmatch(மாறிலி எழுத்து *pattern, மாறிலி எழுத்து *string)
{
	எனில் (*pattern == 0)
		திரும்பு *string == 0;

	எனில் (*pattern == '\\') {
		எனில் (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			திரும்பு உண்மை;
	}

	எனில் (*pattern == '?') {
		எனில் (*string == 0)
			திரும்பு பொய்;
		திரும்பு patmatch(pattern+1, string+1);
	}

	எனில் (*pattern == '*') {
		போது (*string) {
			எனில் (patmatch(pattern+1, string++))
				திரும்பு உண்மை;
		}
		திரும்பு pattern[1] == 0;
	}

	எனில் (*pattern == '[') {
		தருக்க found_match = பொய்;
		தருக்க inverted_list = pattern[1] == '!';
		மாறிலி எழுத்து *p = pattern + (inverted_list ? 1 : 0);

		போது (*++p) {
			எனில் (*p == ']') {
				எனில் (found_match != inverted_list && patmatch(p+1, string+1))
					திரும்பு உண்மை;
				நிறுத்து;
			}

			எனில் (*p == '\\') {
				எனில் (*++p == *string)
					found_match = உண்மை;
			} இல்லையெனில்
			எனில் (*p == *string)
				found_match = உண்மை;
		}
	}

	எனில் (*pattern == *string)
		திரும்பு patmatch(pattern+1, string+1);

	திரும்பு பொய்;
}

std::string get_base_tmpdir()
{
	நிலையான std::string tmpdir;

	எனில் (!tmpdir.empty()) {
		திரும்பு tmpdir;
	}

#எனில் defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	எனில் (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), வெற்று, 0, வெற்று, வெற்று));
	எனில் (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), வெற்று, வெற்று) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#இல்லையெனில்
	எழுத்து * var = std::getenv("TMPDIR");
	எனில் (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		போது (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.எடு_back();
		}
	} இல்லையெனில் {
		tmpdir.assign("/tmp");
	}
#endif
	திரும்பு tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#எனில் defined(__wasm)
	நிலையான size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#அல்லதெனில் defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	போது (1) {
		க்கு (முழு i = 0; i < 6; i++) {
			நிலையான std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			நிலையான uint32_t x = 314159265 ^ uint32_t(time(வெற்று));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		எனில் (_access(template_str.c_str(), 0) != 0)
			நிறுத்து;
	}
#இல்லையெனில்
	முழு suffixlen = template_str.size() - pos - 6;

	எழுத்து *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	விடுவி(p);
#endif

	திரும்பு template_str;
}

std::string make_temp_dir(std::string template_str)
{
#எனில் defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	திரும்பு template_str;
#அல்லதெனில் defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	திரும்பு template_str;
#இல்லையெனில்
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	முழு suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	எழுத்து *p = strdup(template_str.c_str());
	log_assert(p);
	எழுத்து *res = mkdtemp(p);
	எனில் (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	விடுவி(p);

	திரும்பு template_str;
#endif
}

தருக்க check_is_directory(மாறிலி std::string& dirname)
{
#எனில் defined(_WIN32)
	கட்டமைப்பு _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	எனில் (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	எனில் (_stat(dirname_.c_str(), &info) != 0)
	{
		திரும்பு பொய்;
	}
	திரும்பு (info.st_mode & _S_IFDIR) != 0;
#இல்லையெனில்
	கட்டமைப்பு stat info;
	எனில் (stat(dirname.c_str(), &info) != 0)
	{
		திரும்பு பொய்;
	}
	திரும்பு (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
தருக்க check_accessible(மாறிலி std::string& filename, தருக்க)
{
	திரும்பு _access(filename.c_str(), 0) == 0;
}
#இல்லையெனில்
தருக்க check_accessible(மாறிலி std::string& filename, தருக்க is_exec)
{
	திரும்பு access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

தருக்க check_file_exists(மாறிலி std::string& filename, தருக்க is_exec)
{
	திரும்பு check_accessible(filename, is_exec) && !check_is_directory(filename);
}
தருக்க check_directory_exists(மாறிலி std::string& filename, தருக்க is_exec)
{
	திரும்பு check_accessible(filename, is_exec) && check_is_directory(filename);
}

தருக்க is_absolute_path(std::string filename)
{
#ifdef _WIN32
	திரும்பு filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#இல்லையெனில்
	திரும்பு filename[0] == '/';
#endif
}

வெறுமை remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#இல்லையெனில்
	கட்டமைப்பு stat stbuf;
	கட்டமைப்பு dirent **namelist;
	முழு n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	க்கு (முழு i = 0; i < n; i++) {
		எனில் (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			எனில் (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} இல்லையெனில்
				remove_directory(buffer);
		}
		விடுவி(namelist[i]);
	}
	விடுவி(namelist);
	rmdir(dirname.c_str());
#endif
}

தருக்க create_directory(மாறிலி std::string& dirname)
{
#எனில் defined(_WIN32)
	முழு ret = _mkdir(dirname.c_str());
#இல்லையெனில்
	mode_t mode = 0755;
	முழு ret = mkdir(dirname.c_str(), mode);
#endif
	எனில் (ret == 0)
		திரும்பு உண்மை;

	தேர்வு (errno)
	{
	நிலை ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			எனில் (pos == std::string::npos)
#எனில் defined(_WIN32)
				pos = dirname.find_last_of('\\');
			எனில் (pos == std::string::npos)
#endif
				திரும்பு பொய்;
			எனில் (!create_directory( dirname.substr(0, pos) ))
				திரும்பு பொய்;
		}
		// now, try to create again
#எனில் defined(_WIN32)
		திரும்பு 0 == _mkdir(dirname.c_str());
#இல்லையெனில்
		திரும்பு 0 == mkdir(dirname.c_str(), mode);
#endif

	நிலை EEXIST:
		// done!
		திரும்பு check_directory_exists(dirname);

	இயல்பு:
		திரும்பு பொய்;
	}
}

தருக்க make_private_directory(மாறிலி std::string& dirname)
{
#எனில் defined(_WIN32) || defined(__wasi__)
	எனில் (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		திரும்பு பொய்;
	}
	திரும்பு உண்மை;
#இல்லையெனில்
	எனில் (mkdir(dirname.c_str(), 0700) == 0)
		திரும்பு உண்மை;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	முழு err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	எனில் (err == ENOENT && !parent.empty() && create_directory(parent)) {
		எனில் (mkdir(dirname.c_str(), 0700) == 0)
			திரும்பு உண்மை;
		err = errno;
	}

	எனில் (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		திரும்பு பொய்;
	}

	// Do not use a leftover that isn't ours
	கட்டமைப்பு stat st;
	எனில் (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		திரும்பு பொய்;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	எனில் (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		திரும்பு பொய்;
	}

	திரும்பு உண்மை;
#endif
}

std::string make_user_tmpdir(மாறிலி std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#எனில் defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#இல்லையெனில்
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	திரும்பு make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(மாறிலி std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	க்கு (auto c : filename)
	{
		எனில் (c == ' ')
			out += "\\ ";
		இல்லையெனில்
			out.தள்ளு_back(c);
	}
	திரும்பு out;
}

வெறுமை append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	திரும்பு std::filesystem::path(path).filename().string();
}

std::string absolute_path(மாறிலி std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	எனில் (ec)
		திரும்பு "";

	result = std::filesystem::weakly_canonical(result, ec);

	எனில் (ec)
		திரும்பு "";

	திரும்பு result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	எனில் (parent.empty()) {
		திரும்பு "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	எனில் (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	திரும்பு result;
}

வெறுமை format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	க்கு (size_t i = 0; i < fmt.size(); ++i) {
		எழுத்து ch = fmt[i];
		result.தள்ளு_back(ch);
		எனில் (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	திரும்பு result;
}

நிலையான std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	திரும்பு result;
}

நிலையான முழு spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	தேர்வு (spec[spec.size() - 1]) {
	நிலை 'd':
	நிலை 'i':
	நிலை 'o':
	நிலை 'u':
	நிலை 'x':
	நிலை 'X':
		தேர்வு (spec[spec.size() - 2]) {
		நிலை 'h':
			எனில் (spec[spec.size() - 3] == 'h')
				திரும்பு அளவு(எழுத்து);
			திரும்பு அளவு(short);
		நிலை 'l':
			எனில் (spec[spec.size() - 3] == 'l')
				திரும்பு அளவு(long long);
			திரும்பு அளவு(long);
		நிலை 'L':
		நிலை 'q':
			திரும்பு அளவு(long long);
		நிலை 'j':
			திரும்பு அளவு(intmax_t);
		நிலை 'z':
		நிலை 'Z':
			திரும்பு அளவு(size_t);
		நிலை 't':
			திரும்பு அளவு(ptrdiff_t);
		}
		திரும்பு அளவு(முழு);
	நிலை 'e':
	நிலை 'E':
	நிலை 'f':
	நிலை 'F':
	நிலை 'g':
	நிலை 'G':
	நிலை 'a':
	நிலை 'A':
		எனில் (spec[spec.size() - 2] == 'L')
			திரும்பு அளவு(long இரட்டை);
		எனில் (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			திரும்பு அளவு(long இரட்டை);
		திரும்பு அளவு(இரட்டை);
	நிலை 'c':
		எனில் (spec[spec.size() - 2] == 'l') {
			திரும்பு அளவு(wchar_t);
		}
		திரும்பு அளவு(unsigned எழுத்து);
	நிலை 'C':
		திரும்பு அளவு(wchar_t);
	நிலை 's':
	நிலை 'p':
	நிலை 'S':
	நிலை 'n':
		திரும்பு அளவு(வெறுமை *);
	நிலை 'm':
		திரும்பு அளவு(முழு);
	இயல்பு:
		திரும்பு -1;
	}
}

template <typename Arg>
நிலையான வெறுமை format_emit_stringf(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	தேர்வு (இலக்கம்_dynamic_ints) {
	நிலை Dynamic_Int_மொத்தம்::NONE:
		result += string_view_stringf(spec, arg);
		திரும்பு;
	நிலை Dynamic_Int_மொத்தம்::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		திரும்பு;
	நிலை Dynamic_Int_மொத்தம்::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		திரும்பு;
	}
	YOSYS_ABORT("Internal error");
}

வெறுமை format_emit_long_long(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, long long arg)
{
	எனில் (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<முழு>(arg));
		திரும்பு;
	}
	எனில் (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, (முழு)arg);
	} இல்லையெனில் {
		format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, arg);
	}
}

வெறுமை format_emit_unsigned_long_long(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, unsigned long long arg)
{
	எனில் (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned முழு>(arg));
		திரும்பு;
	}
	எனில் (spec == "%c") {
		result += static_cast<எழுத்து>(arg);
		திரும்பு;
	}
	எனில் (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, (unsigned முழு)arg);
	} இல்லையெனில் {
		format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, arg);
	}
}

வெறுமை format_emit_double(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, இரட்டை arg)
{
	format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, arg);
}

வெறுமை format_emit_char_ptr(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, மாறிலி எழுத்து *arg)
{
	எனில் (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		திரும்பு;
	}
	format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, arg);
}

வெறுமை format_emit_string(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, மாறிலி std::string &arg)
{
	எனில் (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		திரும்பு;
	}
	format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, arg.c_str());
}

வெறுமை format_emit_string_view(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, std::string_view arg)
{
	எனில் (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		திரும்பு;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, std::string(arg).c_str());
}

வெறுமை format_emit_void_ptr(std::string &result, std::string_view spec, முழு *dynamic_ints,
	Dynamic_Int_மொத்தம் இலக்கம்_dynamic_ints, மாறிலி வெறுமை *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, இலக்கம்_dynamic_ints, arg);
}

#எனில் defined(_WIN32)
ScopedFileLock::ScopedFileLock(மாறிலி std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			வெற்று, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, வெற்று);
	எனில் (h == INVALID_HANDLE_மதிப்பு) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		திரும்பு;
	}
	OVERLAPPED ov = {};
	எனில் (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		திரும்பு;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	எனில் (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#அல்லதெனில் !defined(__wasi__)
ScopedFileLock::ScopedFileLock(மாறிலி std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	எனில் (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	எனில் (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		திரும்பு;
	}
	முழு ret;
	போது ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	எனில் (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	எனில் (fd >= 0)
		close(fd); // releases the flock
}
#இல்லையெனில்
ScopedFileLock::ScopedFileLock(மாறிலி std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#எனில் defined(_WIN32)
	திரும்பு GetCurrentProcessId();
#அல்லதெனில் defined(__wasi__)
	திரும்பு 0;
#இல்லையெனில்
	திரும்பு getpid();
#endif
}

YOSYS_NAMESPACE_END
