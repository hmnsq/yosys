#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/log.h"
#અન્તર્ભાવ <iostream>
#અન્તર્ભાવ <string>
#અન્તર્ભાવ <filesystem>
#અન્તર્ભાવ <system_error>

#જો !defined(WIN32)
#અન્તર્ભાવ <dirent.h>
#અન્તર્ભાવ <unistd.h>
#નહીંતર
#અન્તર્ભાવ <io.h>
#endif

#જો !defined(_WIN32) && !defined(__wasi__)
#અન્તર્ભાવ <sys/file.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

પૂર્ણાંક readsome(std::istream &f, અક્ષર *s, પૂર્ણાંક n)
{
	પૂર્ણાંક rc = પૂર્ણાંક(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	જો (rc == 0) {
		પૂર્ણાંક c = f.get();
		જો (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	પાછા rc;
}

std::string next_token(std::string &text, અચલ અક્ષર *sep, તાર્કિક long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	જો (pos_begin == std::string::npos)
		pos_begin = text.size();

	જો (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		માટે (size_t i = pos_begin+1; i < text.size(); i++) {
			જો (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				પાછા token;
			}
			જો (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				પાછા token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	જો (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	પાછા token;
}

std::vector<std::string> split_tokens(અચલ std::string &text, અચલ અક્ષર *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	માટે (અક્ષર c : text) {
		જો (strchr(sep, c)) {
			જો (!current_token.empty()) {
				tokens.ધકેલો_back(current_token);
				current_token.clear();
			}
		} નહીંતર
			current_token += c;
	}
	જો (!current_token.empty()) {
		tokens.ધકેલો_back(current_token);
		current_token.clear();
	}
	પાછા tokens;
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
તાર્કિક patmatch(અચલ અક્ષર *pattern, અચલ અક્ષર *string)
{
	જો (*pattern == 0)
		પાછા *string == 0;

	જો (*pattern == '\\') {
		જો (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			પાછા સાચું;
	}

	જો (*pattern == '?') {
		જો (*string == 0)
			પાછા ખોટું;
		પાછા patmatch(pattern+1, string+1);
	}

	જો (*pattern == '*') {
		જ્યારે (*string) {
			જો (patmatch(pattern+1, string++))
				પાછા સાચું;
		}
		પાછા pattern[1] == 0;
	}

	જો (*pattern == '[') {
		તાર્કિક found_match = ખોટું;
		તાર્કિક inverted_list = pattern[1] == '!';
		અચલ અક્ષર *p = pattern + (inverted_list ? 1 : 0);

		જ્યારે (*++p) {
			જો (*p == ']') {
				જો (found_match != inverted_list && patmatch(p+1, string+1))
					પાછા સાચું;
				તોડો;
			}

			જો (*p == '\\') {
				જો (*++p == *string)
					found_match = સાચું;
			} નહીંતર
			જો (*p == *string)
				found_match = સાચું;
		}
	}

	જો (*pattern == *string)
		પાછા patmatch(pattern+1, string+1);

	પાછા ખોટું;
}

std::string get_base_tmpdir()
{
	સ્થિર std::string tmpdir;

	જો (!tmpdir.empty()) {
		પાછા tmpdir;
	}

#જો defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	જો (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), ખાલી, 0, ખાલી, ખાલી));
	જો (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), ખાલી, ખાલી) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#નહીંતર
	અક્ષર * var = std::getenv("TMPDIR");
	જો (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		જ્યારે (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.કાઢો_back();
		}
	} નહીંતર {
		tmpdir.assign("/tmp");
	}
#endif
	પાછા tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#જો defined(__wasm)
	સ્થિર size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#અથવાજો defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	જ્યારે (1) {
		માટે (પૂર્ણાંક i = 0; i < 6; i++) {
			સ્થિર std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			સ્થિર uint32_t x = 314159265 ^ uint32_t(time(ખાલી));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		જો (_access(template_str.c_str(), 0) != 0)
			તોડો;
	}
#નહીંતર
	પૂર્ણાંક suffixlen = template_str.size() - pos - 6;

	અક્ષર *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	મુક્ત(p);
#endif

	પાછા template_str;
}

std::string make_temp_dir(std::string template_str)
{
#જો defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	પાછા template_str;
#અથવાજો defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	પાછા template_str;
#નહીંતર
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	પૂર્ણાંક suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	અક્ષર *p = strdup(template_str.c_str());
	log_assert(p);
	અક્ષર *res = mkdtemp(p);
	જો (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	મુક્ત(p);

	પાછા template_str;
#endif
}

તાર્કિક check_is_directory(અચલ std::string& dirname)
{
#જો defined(_WIN32)
	સંરચના _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	જો (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	જો (_stat(dirname_.c_str(), &info) != 0)
	{
		પાછા ખોટું;
	}
	પાછા (info.st_mode & _S_IFDIR) != 0;
#નહીંતર
	સંરચના stat info;
	જો (stat(dirname.c_str(), &info) != 0)
	{
		પાછા ખોટું;
	}
	પાછા (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
તાર્કિક check_accessible(અચલ std::string& filename, તાર્કિક)
{
	પાછા _access(filename.c_str(), 0) == 0;
}
#નહીંતર
તાર્કિક check_accessible(અચલ std::string& filename, તાર્કિક is_exec)
{
	પાછા access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

તાર્કિક check_file_exists(અચલ std::string& filename, તાર્કિક is_exec)
{
	પાછા check_accessible(filename, is_exec) && !check_is_directory(filename);
}
તાર્કિક check_directory_exists(અચલ std::string& filename, તાર્કિક is_exec)
{
	પાછા check_accessible(filename, is_exec) && check_is_directory(filename);
}

તાર્કિક is_absolute_path(std::string filename)
{
#ifdef _WIN32
	પાછા filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#નહીંતર
	પાછા filename[0] == '/';
#endif
}

શૂન્ય remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#નહીંતર
	સંરચના stat stbuf;
	સંરચના dirent **namelist;
	પૂર્ણાંક n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	માટે (પૂર્ણાંક i = 0; i < n; i++) {
		જો (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			જો (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} નહીંતર
				remove_directory(buffer);
		}
		મુક્ત(namelist[i]);
	}
	મુક્ત(namelist);
	rmdir(dirname.c_str());
#endif
}

તાર્કિક create_directory(અચલ std::string& dirname)
{
#જો defined(_WIN32)
	પૂર્ણાંક ret = _mkdir(dirname.c_str());
#નહીંતર
	mode_t mode = 0755;
	પૂર્ણાંક ret = mkdir(dirname.c_str(), mode);
#endif
	જો (ret == 0)
		પાછા સાચું;

	પસંદગી (errno)
	{
	સ્થિતિ ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			જો (pos == std::string::npos)
#જો defined(_WIN32)
				pos = dirname.find_last_of('\\');
			જો (pos == std::string::npos)
#endif
				પાછા ખોટું;
			જો (!create_directory( dirname.substr(0, pos) ))
				પાછા ખોટું;
		}
		// now, try to create again
#જો defined(_WIN32)
		પાછા 0 == _mkdir(dirname.c_str());
#નહીંતર
		પાછા 0 == mkdir(dirname.c_str(), mode);
#endif

	સ્થિતિ EEXIST:
		// done!
		પાછા check_directory_exists(dirname);

	મૂળભૂત:
		પાછા ખોટું;
	}
}

તાર્કિક make_private_directory(અચલ std::string& dirname)
{
#જો defined(_WIN32) || defined(__wasi__)
	જો (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		પાછા ખોટું;
	}
	પાછા સાચું;
#નહીંતર
	જો (mkdir(dirname.c_str(), 0700) == 0)
		પાછા સાચું;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	પૂર્ણાંક err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	જો (err == ENOENT && !parent.empty() && create_directory(parent)) {
		જો (mkdir(dirname.c_str(), 0700) == 0)
			પાછા સાચું;
		err = errno;
	}

	જો (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		પાછા ખોટું;
	}

	// Do not use a leftover that isn't ours
	સંરચના stat st;
	જો (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		પાછા ખોટું;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	જો (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		પાછા ખોટું;
	}

	પાછા સાચું;
#endif
}

std::string make_user_tmpdir(અચલ std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#જો defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#નહીંતર
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	પાછા make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(અચલ std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	માટે (auto c : filename)
	{
		જો (c == ' ')
			out += "\\ ";
		નહીંતર
			out.ધકેલો_back(c);
	}
	પાછા out;
}

શૂન્ય append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	પાછા std::filesystem::path(path).filename().string();
}

std::string absolute_path(અચલ std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	જો (ec)
		પાછા "";

	result = std::filesystem::weakly_canonical(result, ec);

	જો (ec)
		પાછા "";

	પાછા result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	જો (parent.empty()) {
		પાછા "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	જો (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	પાછા result;
}

શૂન્ય format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	માટે (size_t i = 0; i < fmt.size(); ++i) {
		અક્ષર ch = fmt[i];
		result.ધકેલો_back(ch);
		જો (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	પાછા result;
}

સ્થિર std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	પાછા result;
}

સ્થિર પૂર્ણાંક spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	પસંદગી (spec[spec.size() - 1]) {
	સ્થિતિ 'd':
	સ્થિતિ 'i':
	સ્થિતિ 'o':
	સ્થિતિ 'u':
	સ્થિતિ 'x':
	સ્થિતિ 'X':
		પસંદગી (spec[spec.size() - 2]) {
		સ્થિતિ 'h':
			જો (spec[spec.size() - 3] == 'h')
				પાછા કદ(અક્ષર);
			પાછા કદ(short);
		સ્થિતિ 'l':
			જો (spec[spec.size() - 3] == 'l')
				પાછા કદ(long long);
			પાછા કદ(long);
		સ્થિતિ 'L':
		સ્થિતિ 'q':
			પાછા કદ(long long);
		સ્થિતિ 'j':
			પાછા કદ(intmax_t);
		સ્થિતિ 'z':
		સ્થિતિ 'Z':
			પાછા કદ(size_t);
		સ્થિતિ 't':
			પાછા કદ(ptrdiff_t);
		}
		પાછા કદ(પૂર્ણાંક);
	સ્થિતિ 'e':
	સ્થિતિ 'E':
	સ્થિતિ 'f':
	સ્થિતિ 'F':
	સ્થિતિ 'g':
	સ્થિતિ 'G':
	સ્થિતિ 'a':
	સ્થિતિ 'A':
		જો (spec[spec.size() - 2] == 'L')
			પાછા કદ(long દ્વિગુણ);
		જો (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			પાછા કદ(long દ્વિગુણ);
		પાછા કદ(દ્વિગુણ);
	સ્થિતિ 'c':
		જો (spec[spec.size() - 2] == 'l') {
			પાછા કદ(wchar_t);
		}
		પાછા કદ(unsigned અક્ષર);
	સ્થિતિ 'C':
		પાછા કદ(wchar_t);
	સ્થિતિ 's':
	સ્થિતિ 'p':
	સ્થિતિ 'S':
	સ્થિતિ 'n':
		પાછા કદ(શૂન્ય *);
	સ્થિતિ 'm':
		પાછા કદ(પૂર્ણાંક);
	મૂળભૂત:
		પાછા -1;
	}
}

template <typename Arg>
સ્થિર શૂન્ય format_emit_stringf(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	પસંદગી (અંક_dynamic_ints) {
	સ્થિતિ Dynamic_Int_ગણતરી::NONE:
		result += string_view_stringf(spec, arg);
		પાછા;
	સ્થિતિ Dynamic_Int_ગણતરી::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		પાછા;
	સ્થિતિ Dynamic_Int_ગણતરી::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		પાછા;
	}
	YOSYS_ABORT("Internal error");
}

શૂન્ય format_emit_long_long(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, long long arg)
{
	જો (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<પૂર્ણાંક>(arg));
		પાછા;
	}
	જો (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, (પૂર્ણાંક)arg);
	} નહીંતર {
		format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, arg);
	}
}

શૂન્ય format_emit_unsigned_long_long(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, unsigned long long arg)
{
	જો (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned પૂર્ણાંક>(arg));
		પાછા;
	}
	જો (spec == "%c") {
		result += static_cast<અક્ષર>(arg);
		પાછા;
	}
	જો (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, (unsigned પૂર્ણાંક)arg);
	} નહીંતર {
		format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, arg);
	}
}

શૂન્ય format_emit_double(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, દ્વિગુણ arg)
{
	format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, arg);
}

શૂન્ય format_emit_char_ptr(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, અચલ અક્ષર *arg)
{
	જો (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		પાછા;
	}
	format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, arg);
}

શૂન્ય format_emit_string(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, અચલ std::string &arg)
{
	જો (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		પાછા;
	}
	format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, arg.c_str());
}

શૂન્ય format_emit_string_view(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, std::string_view arg)
{
	જો (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		પાછા;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, std::string(arg).c_str());
}

શૂન્ય format_emit_void_ptr(std::string &result, std::string_view spec, પૂર્ણાંક *dynamic_ints,
	Dynamic_Int_ગણતરી અંક_dynamic_ints, અચલ શૂન્ય *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, અંક_dynamic_ints, arg);
}

#જો defined(_WIN32)
ScopedFileLock::ScopedFileLock(અચલ std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ખાલી, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, ખાલી);
	જો (h == INVALID_HANDLE_કિંમત) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		પાછા;
	}
	OVERLAPPED ov = {};
	જો (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		પાછા;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	જો (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#અથવાજો !defined(__wasi__)
ScopedFileLock::ScopedFileLock(અચલ std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	જો (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	જો (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		પાછા;
	}
	પૂર્ણાંક ret;
	જ્યારે ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	જો (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	જો (fd >= 0)
		close(fd); // releases the flock
}
#નહીંતર
ScopedFileLock::ScopedFileLock(અચલ std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#જો defined(_WIN32)
	પાછા GetCurrentProcessId();
#અથવાજો defined(__wasi__)
	પાછા 0;
#નહીંતર
	પાછા getpid();
#endif
}

YOSYS_NAMESPACE_END
