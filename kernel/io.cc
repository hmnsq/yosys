#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/log.h"
#ଅନ୍ତର୍ଭାଵ <iostream>
#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <filesystem>
#ଅନ୍ତର୍ଭାଵ <system_error>

#ଯଦି !defined(WIN32)
#ଅନ୍ତର୍ଭାଵ <dirent.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <io.h>
#endif

#ଯଦି !defined(_WIN32) && !defined(__wasi__)
#ଅନ୍ତର୍ଭାଵ <sys/file.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readsome(std::istream &f, ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ପୂର୍ଣ୍ଣସଂଖ୍ୟା(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ଯଦି (rc == 0) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = f.get();
		ଯଦି (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ଫେରନ୍ତୁ rc;
}

std::string next_token(std::string &text, ଅଚଳ ଅକ୍ଷର *sep, ତାର୍କିକ long_strings)
{
	size_t pos_begin = text.find_first_not_of(sep);

	ଯଦି (pos_begin == std::string::npos)
		pos_begin = text.size();

	ଯଦି (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		ପାଇଁ (size_t i = pos_begin+1; i < text.size(); i++) {
			ଯଦି (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				ଫେରନ୍ତୁ token;
			}
			ଯଦି (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				ଫେରନ୍ତୁ token + ";";
			}
		}
	}

	size_t pos_end = text.find_first_of(sep, pos_begin);

	ଯଦି (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	ଫେରନ୍ତୁ token;
}

std::vector<std::string> split_tokens(ଅଚଳ std::string &text, ଅଚଳ ଅକ୍ଷର *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	ପାଇଁ (ଅକ୍ଷର c : text) {
		ଯଦି (strchr(sep, c)) {
			ଯଦି (!current_token.empty()) {
				tokens.ଠେଲ_back(current_token);
				current_token.clear();
			}
		} ଅନ୍ୟଥା
			current_token += c;
	}
	ଯଦି (!current_token.empty()) {
		tokens.ଠେଲ_back(current_token);
		current_token.clear();
	}
	ଫେରନ୍ତୁ tokens;
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
ତାର୍କିକ patmatch(ଅଚଳ ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *string)
{
	ଯଦି (*pattern == 0)
		ଫେରନ୍ତୁ *string == 0;

	ଯଦି (*pattern == '\\') {
		ଯଦି (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (*pattern == '?') {
		ଯଦି (*string == 0)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ଫେରନ୍ତୁ patmatch(pattern+1, string+1);
	}

	ଯଦି (*pattern == '*') {
		ଯେତେବେଳେ (*string) {
			ଯଦି (patmatch(pattern+1, string++))
				ଫେରନ୍ତୁ ସତ୍ୟ;
		}
		ଫେରନ୍ତୁ pattern[1] == 0;
	}

	ଯଦି (*pattern == '[') {
		ତାର୍କିକ found_match = ମିଥ୍ୟା;
		ତାର୍କିକ inverted_list = pattern[1] == '!';
		ଅଚଳ ଅକ୍ଷର *p = pattern + (inverted_list ? 1 : 0);

		ଯେତେବେଳେ (*++p) {
			ଯଦି (*p == ']') {
				ଯଦି (found_match != inverted_list && patmatch(p+1, string+1))
					ଫେରନ୍ତୁ ସତ୍ୟ;
				ଭାଙ୍ଗ;
			}

			ଯଦି (*p == '\\') {
				ଯଦି (*++p == *string)
					found_match = ସତ୍ୟ;
			} ଅନ୍ୟଥା
			ଯଦି (*p == *string)
				found_match = ସତ୍ୟ;
		}
	}

	ଯଦି (*pattern == *string)
		ଫେରନ୍ତୁ patmatch(pattern+1, string+1);

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

std::string get_base_tmpdir()
{
	ସ୍ଥିର std::string tmpdir;

	ଯଦି (!tmpdir.empty()) {
		ଫେରନ୍ତୁ tmpdir;
	}

#ଯଦି defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ଯଦି (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), ଖାଲି, 0, ଖାଲି, ଖାଲି));
	ଯଦି (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), ଖାଲି, ଖାଲି) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ଅନ୍ୟଥା
	ଅକ୍ଷର * var = std::getenv("TMPDIR");
	ଯଦି (var && strlen(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		ଯେତେବେଳେ (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.କାଢ଼_back();
		}
	} ଅନ୍ୟଥା {
		tmpdir.assign("/tmp");
	}
#endif
	ଫେରନ୍ତୁ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ଯଦି defined(__wasm)
	ସ୍ଥିର size_t index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#ନଚେତଯଦି defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	ଯେତେବେଳେ (1) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 6; i++) {
			ସ୍ଥିର std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ସ୍ଥିର uint32_t x = 314159265 ^ uint32_t(time(ଖାଲି));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		ଯଦି (_access(template_str.c_str(), 0) != 0)
			ଭାଙ୍ଗ;
	}
#ଅନ୍ୟଥା
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା suffixlen = template_str.size() - pos - 6;

	ଅକ୍ଷର *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	ମୁକ୍ତ(p);
#endif

	ଫେରନ୍ତୁ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#ଯଦି defined(_WIN32)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str());
	ଫେରନ୍ତୁ template_str;
#ନଚେତଯଦି defined(__wasm)
	template_str = make_temp_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ଫେରନ୍ତୁ template_str;
#ଅନ୍ୟଥା
#  ifndef NDEBUG
	size_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	ଅକ୍ଷର *p = strdup(template_str.c_str());
	log_assert(p);
	ଅକ୍ଷର *res = mkdtemp(p);
	ଯଦି (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	ମୁକ୍ତ(p);

	ଫେରନ୍ତୁ template_str;
#endif
}

ତାର୍କିକ check_is_directory(ଅଚଳ std::string& dirname)
{
#ଯଦି defined(_WIN32)
	ସଂରଚନା _stat info;
	auto dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ଯଦି (dirname.back() == '/' || dirname.back() == '\\') {
		dirname_ = dirname.substr(0, dirname.length() - 1);
	}

	ଯଦି (_stat(dirname_.c_str(), &info) != 0)
	{
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ (info.st_mode & _S_IFDIR) != 0;
#ଅନ୍ୟଥା
	ସଂରଚନା stat info;
	ଯଦି (stat(dirname.c_str(), &info) != 0)
	{
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
ତାର୍କିକ check_accessible(ଅଚଳ std::string& filename, ତାର୍କିକ)
{
	ଫେରନ୍ତୁ _access(filename.c_str(), 0) == 0;
}
#ଅନ୍ୟଥା
ତାର୍କିକ check_accessible(ଅଚଳ std::string& filename, ତାର୍କିକ is_exec)
{
	ଫେରନ୍ତୁ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

ତାର୍କିକ check_file_exists(ଅଚଳ std::string& filename, ତାର୍କିକ is_exec)
{
	ଫେରନ୍ତୁ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
ତାର୍କିକ check_directory_exists(ଅଚଳ std::string& filename, ତାର୍କିକ is_exec)
{
	ଫେରନ୍ତୁ check_accessible(filename, is_exec) && check_is_directory(filename);
}

ତାର୍କିକ is_absolute_path(std::string filename)
{
#ifdef _WIN32
	ଫେରନ୍ତୁ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ filename[0] == '/';
#endif
}

ଶୂନ୍ୟ remove_directory(std::string dirname)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", dirname));
#ଅନ୍ୟଥା
	ସଂରଚନା stat stbuf;
	ସଂରଚନା dirent **namelist;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = scandir(dirname.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n; i++) {
		ଯଦି (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", dirname, namelist[i]->d_name);
			ଯଦି (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				remove(buffer.c_str());
			} ଅନ୍ୟଥା
				remove_directory(buffer);
		}
		ମୁକ୍ତ(namelist[i]);
	}
	ମୁକ୍ତ(namelist);
	rmdir(dirname.c_str());
#endif
}

ତାର୍କିକ create_directory(ଅଚଳ std::string& dirname)
{
#ଯଦି defined(_WIN32)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = _mkdir(dirname.c_str());
#ଅନ୍ୟଥା
	mode_t mode = 0755;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = mkdir(dirname.c_str(), mode);
#endif
	ଯଦି (ret == 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;

	ବାଛ (errno)
	{
	ସ୍ଥିତି ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = dirname.find_last_of('/');
			ଯଦି (pos == std::string::npos)
#ଯଦି defined(_WIN32)
				pos = dirname.find_last_of('\\');
			ଯଦି (pos == std::string::npos)
#endif
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			ଯଦି (!create_directory( dirname.substr(0, pos) ))
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}
		// now, try to create again
#ଯଦି defined(_WIN32)
		ଫେରନ୍ତୁ 0 == _mkdir(dirname.c_str());
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ 0 == mkdir(dirname.c_str(), mode);
#endif

	ସ୍ଥିତି EEXIST:
		// done!
		ଫେରନ୍ତୁ check_directory_exists(dirname);

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ତାର୍କିକ make_private_directory(ଅଚଳ std::string& dirname)
{
#ଯଦି defined(_WIN32) || defined(__wasi__)
	ଯଦି (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
#ଅନ୍ୟଥା
	ଯଦି (mkdir(dirname.c_str(), 0700) == 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = errno;
	std::string parent = std::filesystem::path(dirname).parent_path().string();

	ଯଦି (err == ENOENT && !parent.empty() && create_directory(parent)) {
		ଯଦି (mkdir(dirname.c_str(), 0700) == 0)
			ଫେରନ୍ତୁ ସତ୍ୟ;
		err = errno;
	}

	ଯଦି (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	// Do not use a leftover that isn't ours
	ସଂରଚନା stat st;
	ଯଦି (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	ଯଦି (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
#endif
}

std::string make_user_tmpdir(ଅଚଳ std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#ଯଦି defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_base_tmpdir() + "/" + name;
#ଅନ୍ୟଥା
	std::string dirname = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (unsigned)geteuid());
#endif
	ଫେରନ୍ତୁ make_private_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ଅଚଳ std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	ପାଇଁ (auto c : filename)
	{
		ଯଦି (c == ' ')
			out += "\\ ";
		ଅନ୍ୟଥା
			out.ଠେଲ_back(c);
	}
	ଫେରନ୍ତୁ out;
}

ଶୂନ୍ୟ append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	ଫେରନ୍ତୁ std::filesystem::path(path).filename().string();
}

std::string absolute_path(ଅଚଳ std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	ଯଦି (ec)
		ଫେରନ୍ତୁ "";

	result = std::filesystem::weakly_canonical(result, ec);

	ଯଦି (ec)
		ଫେରନ୍ତୁ "";

	ଫେରନ୍ତୁ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	auto parent = std::filesystem::path(path).parent_path();
	ଯଦି (parent.empty()) {
		ଫେରନ୍ତୁ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	ଯଦି (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	ପାଇଁ (size_t i = 0; i < fmt.size(); ++i) {
		ଅକ୍ଷର ch = fmt[i];
		result.ଠେଲ_back(ch);
		ଯଦି (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	ବାଛ (spec[spec.size() - 1]) {
	ସ୍ଥିତି 'd':
	ସ୍ଥିତି 'i':
	ସ୍ଥିତି 'o':
	ସ୍ଥିତି 'u':
	ସ୍ଥିତି 'x':
	ସ୍ଥିତି 'X':
		ବାଛ (spec[spec.size() - 2]) {
		ସ୍ଥିତି 'h':
			ଯଦି (spec[spec.size() - 3] == 'h')
				ଫେରନ୍ତୁ ଆକାର(ଅକ୍ଷର);
			ଫେରନ୍ତୁ ଆକାର(short);
		ସ୍ଥିତି 'l':
			ଯଦି (spec[spec.size() - 3] == 'l')
				ଫେରନ୍ତୁ ଆକାର(long long);
			ଫେରନ୍ତୁ ଆକାର(long);
		ସ୍ଥିତି 'L':
		ସ୍ଥିତି 'q':
			ଫେରନ୍ତୁ ଆକାର(long long);
		ସ୍ଥିତି 'j':
			ଫେରନ୍ତୁ ଆକାର(intmax_t);
		ସ୍ଥିତି 'z':
		ସ୍ଥିତି 'Z':
			ଫେରନ୍ତୁ ଆକାର(size_t);
		ସ୍ଥିତି 't':
			ଫେରନ୍ତୁ ଆକାର(ptrdiff_t);
		}
		ଫେରନ୍ତୁ ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	ସ୍ଥିତି 'e':
	ସ୍ଥିତି 'E':
	ସ୍ଥିତି 'f':
	ସ୍ଥିତି 'F':
	ସ୍ଥିତି 'g':
	ସ୍ଥିତି 'G':
	ସ୍ଥିତି 'a':
	ସ୍ଥିତି 'A':
		ଯଦି (spec[spec.size() - 2] == 'L')
			ଫେରନ୍ତୁ ଆକାର(long ଦ୍ୱିଗୁଣ);
		ଯଦି (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			ଫେରନ୍ତୁ ଆକାର(long ଦ୍ୱିଗୁଣ);
		ଫେରନ୍ତୁ ଆକାର(ଦ୍ୱିଗୁଣ);
	ସ୍ଥିତି 'c':
		ଯଦି (spec[spec.size() - 2] == 'l') {
			ଫେରନ୍ତୁ ଆକାର(wchar_t);
		}
		ଫେରନ୍ତୁ ଆକାର(unsigned ଅକ୍ଷର);
	ସ୍ଥିତି 'C':
		ଫେରନ୍ତୁ ଆକାର(wchar_t);
	ସ୍ଥିତି 's':
	ସ୍ଥିତି 'p':
	ସ୍ଥିତି 'S':
	ସ୍ଥିତି 'n':
		ଫେରନ୍ତୁ ଆକାର(ଶୂନ୍ୟ *);
	ସ୍ଥିତି 'm':
		ଫେରନ୍ତୁ ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ -1;
	}
}

template <typename Arg>
ସ୍ଥିର ଶୂନ୍ୟ format_emit_stringf(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	ବାଛ (ଅଙ୍କ_dynamic_ints) {
	ସ୍ଥିତି Dynamic_Int_ଗଣନା::NONE:
		result += string_view_stringf(spec, arg);
		ଫେରନ୍ତୁ;
	ସ୍ଥିତି Dynamic_Int_ଗଣନା::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		ଫେରନ୍ତୁ;
	ସ୍ଥିତି Dynamic_Int_ଗଣନା::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ଫେରନ୍ତୁ;
	}
	YOSYS_ABORT("Internal error");
}

ଶୂନ୍ୟ format_emit_long_long(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, long long arg)
{
	ଯଦି (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(arg));
		ଫେରନ୍ତୁ;
	}
	ଯଦି (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg);
	} ଅନ୍ୟଥା {
		format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, arg);
	}
}

ଶୂନ୍ୟ format_emit_unsigned_long_long(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, unsigned long long arg)
{
	ଯଦି (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(arg));
		ଫେରନ୍ତୁ;
	}
	ଯଦି (spec == "%c") {
		result += static_cast<ଅକ୍ଷର>(arg);
		ଫେରନ୍ତୁ;
	}
	ଯଦି (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arg);
	} ଅନ୍ୟଥା {
		format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, arg);
	}
}

ଶୂନ୍ୟ format_emit_double(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଦ୍ୱିଗୁଣ arg)
{
	format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, arg);
}

ଶୂନ୍ୟ format_emit_char_ptr(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଅଚଳ ଅକ୍ଷର *arg)
{
	ଯଦି (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ଫେରନ୍ତୁ;
	}
	format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, arg);
}

ଶୂନ୍ୟ format_emit_string(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଅଚଳ std::string &arg)
{
	ଯଦି (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		ଫେରନ୍ତୁ;
	}
	format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, arg.c_str());
}

ଶୂନ୍ୟ format_emit_string_view(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, std::string_view arg)
{
	ଯଦି (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		ଫେରନ୍ତୁ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, std::string(arg).c_str());
}

ଶୂନ୍ୟ format_emit_void_ptr(std::string &result, std::string_view spec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dynamic_ints,
	Dynamic_Int_ଗଣନା ଅଙ୍କ_dynamic_ints, ଅଚଳ ଶୂନ୍ୟ *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, ଅଙ୍କ_dynamic_ints, arg);
}

#ଯଦି defined(_WIN32)
ScopedFileLock::ScopedFileLock(ଅଚଳ std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ଖାଲି, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, ଖାଲି);
	ଯଦି (h == INVALID_HANDLE_ମୂଲ୍ୟ) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ଫେରନ୍ତୁ;
	}
	OVERLAPPED ov = {};
	ଯଦି (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		ଫେରନ୍ତୁ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	ଯଦି (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#ନଚେତଯଦି !defined(__wasi__)
ScopedFileLock::ScopedFileLock(ଅଚଳ std::string &path)
{
	fd = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ଯଦି (fd < 0)
		fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	ଯଦି (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		ଫେରନ୍ତୁ;
	}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ଯେତେବେଳେ ((ret = flock(fd, LOCK_EX)) != 0 && errno == EINTR);
	ଯଦି (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	ଯଦି (fd >= 0)
		close(fd); // releases the flock
}
#ଅନ୍ୟଥା
ScopedFileLock::ScopedFileLock(ଅଚଳ std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

unsigned get_process_id()
{
#ଯଦି defined(_WIN32)
	ଫେରନ୍ତୁ GetCurrentProcessId();
#ନଚେତଯଦି defined(__wasi__)
	ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ getpid();
#endif
}

YOSYS_NAMESPACE_END
