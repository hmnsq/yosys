#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <filesystem>
#अन्तर्भाव <system_error>

#यदि !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <io.h>
#endif

#यदि !defined(_WIN32) && !defined(__wasi__)
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

पूर्णाङ्क readsome(std::istream &f, वर्ण *s, पूर्णाङ्क n)
{
	पूर्णाङ्क rc = पूर्णाङ्क(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	यदि (rc == 0) {
		पूर्णाङ्क c = f.get();
		यदि (c != खातापूर्ण) {
			*s = c;
			rc = 1;
		}
	}

	फर्काऊ rc;
}

std::string next_token(std::string &text, अचर वर्ण *sep, तार्किक long_strings)
{
	माप_प्रकार pos_begin = text.find_first_not_of(sep);

	यदि (pos_begin == std::string::npos)
		pos_begin = text.size();

	यदि (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		लागि (माप_प्रकार i = pos_begin+1; i < text.size(); i++) {
			यदि (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				फर्काऊ token;
			}
			यदि (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				फर्काऊ token + ";";
			}
		}
	}

	माप_प्रकार pos_end = text.find_first_of(sep, pos_begin);

	यदि (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	फर्काऊ token;
}

std::vector<std::string> split_tokens(अचर std::string &text, अचर वर्ण *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	लागि (वर्ण c : text) {
		यदि (म_अक्षर(sep, c)) {
			यदि (!current_token.empty()) {
				tokens.थप_back(current_token);
				current_token.clear();
			}
		} अन्यथा
			current_token += c;
	}
	यदि (!current_token.empty()) {
		tokens.थप_back(current_token);
		current_token.clear();
	}
	फर्काऊ tokens;
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
तार्किक patmatch(अचर वर्ण *pattern, अचर वर्ण *string)
{
	यदि (*pattern == 0)
		फर्काऊ *string == 0;

	यदि (*pattern == '\\') {
		यदि (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			फर्काऊ सत्य;
	}

	यदि (*pattern == '?') {
		यदि (*string == 0)
			फर्काऊ असत्य;
		फर्काऊ patmatch(pattern+1, string+1);
	}

	यदि (*pattern == '*') {
		जबसम्म (*string) {
			यदि (patmatch(pattern+1, string++))
				फर्काऊ सत्य;
		}
		फर्काऊ pattern[1] == 0;
	}

	यदि (*pattern == '[') {
		तार्किक found_match = असत्य;
		तार्किक inverted_list = pattern[1] == '!';
		अचर वर्ण *p = pattern + (inverted_list ? 1 : 0);

		जबसम्म (*++p) {
			यदि (*p == ']') {
				यदि (found_match != inverted_list && patmatch(p+1, string+1))
					फर्काऊ सत्य;
				रोक;
			}

			यदि (*p == '\\') {
				यदि (*++p == *string)
					found_match = सत्य;
			} अन्यथा
			यदि (*p == *string)
				found_match = सत्य;
		}
	}

	यदि (*pattern == *string)
		फर्काऊ patmatch(pattern+1, string+1);

	फर्काऊ असत्य;
}

std::string get_base_tmpdir()
{
	स्थिर std::string tmpdir;

	यदि (!tmpdir.empty()) {
		फर्काऊ tmpdir;
	}

#यदि defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	यदि (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), रिक्त, 0, रिक्त, रिक्त));
	यदि (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), रिक्त, रिक्त) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#अन्यथा
	वर्ण * var = std::दो_पर्या("TMPDIR");
	यदि (var && म_माप(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		जबसम्म (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.झिक_back();
		}
	} अन्यथा {
		tmpdir.assign("/tmp");
	}
#endif
	फर्काऊ tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	माप_प्रकार pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#यदि defined(__wasm)
	स्थिर माप_प्रकार index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#अथवायदि defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	जबसम्म (1) {
		लागि (पूर्णाङ्क i = 0; i < 6; i++) {
			स्थिर std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			स्थिर uint32_t x = 314159265 ^ uint32_t(समय(रिक्त));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		यदि (_access(template_str.c_str(), 0) != 0)
			रोक;
	}
#अन्यथा
	पूर्णाङ्क suffixlen = template_str.size() - pos - 6;

	वर्ण *p = strdup(template_str.c_str());
	बंद(mkstemps(p, suffixlen));
	template_str = p;
	मुक्त(p);
#endif

	फर्काऊ template_str;
}

std::string make_temp_dir(std::string template_str)
{
#यदि defined(_WIN32)
	template_str = make_temp_file(template_str);
	सूची_गढ़ो(template_str.c_str());
	फर्काऊ template_str;
#अथवायदि defined(__wasm)
	template_str = make_temp_file(template_str);
	सूची_गढ़ो(template_str.c_str(), 0777);
	फर्काऊ template_str;
#अन्यथा
#  ifndef न_संशोधन
	माप_प्रकार pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	पूर्णाङ्क suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	वर्ण *p = strdup(template_str.c_str());
	log_assert(p);
	वर्ण *res = mkdtemp(p);
	यदि (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, म_त्रुटि(त्रुटि_सं), त्रुटि_सं);
	template_str = p;
	मुक्त(p);

	फर्काऊ template_str;
#endif
}

तार्किक check_is_directory(अचर std::string& स_नाम)
{
#यदि defined(_WIN32)
	संरचना _stat info;
	स्वतः dirname_ = स_नाम;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	यदि (स_नाम.back() == '/' || स_नाम.back() == '\\') {
		dirname_ = स_नाम.substr(0, स_नाम.length() - 1);
	}

	यदि (_stat(dirname_.c_str(), &info) != 0)
	{
		फर्काऊ असत्य;
	}
	फर्काऊ (info.st_mode & _S_IFDIR) != 0;
#अन्यथा
	संरचना stat info;
	यदि (stat(स_नाम.c_str(), &info) != 0)
	{
		फर्काऊ असत्य;
	}
	फर्काऊ (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
तार्किक check_accessible(अचर std::string& filename, तार्किक)
{
	फर्काऊ _access(filename.c_str(), 0) == 0;
}
#अन्यथा
तार्किक check_accessible(अचर std::string& filename, तार्किक is_exec)
{
	फर्काऊ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

तार्किक check_file_exists(अचर std::string& filename, तार्किक is_exec)
{
	फर्काऊ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
तार्किक check_directory_exists(अचर std::string& filename, तार्किक is_exec)
{
	फर्काऊ check_accessible(filename, is_exec) && check_is_directory(filename);
}

तार्किक is_absolute_path(std::string filename)
{
#ifdef _WIN32
	फर्काऊ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#अन्यथा
	फर्काऊ filename[0] == '/';
#endif
}

शून्य remove_directory(std::string स_नाम)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", स_नाम));
#अन्यथा
	संरचना stat stbuf;
	संरचना dirent **namelist;
	पूर्णाङ्क n = scandir(स_नाम.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	लागि (पूर्णाङ्क i = 0; i < n; i++) {
		यदि (म_भेद(namelist[i]->d_name, ".") && म_भेद(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", स_नाम, namelist[i]->d_name);
			यदि (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				हटाओ(buffer.c_str());
			} अन्यथा
				remove_directory(buffer);
		}
		मुक्त(namelist[i]);
	}
	मुक्त(namelist);
	सूची_हटाओ(स_नाम.c_str());
#endif
}

तार्किक create_directory(अचर std::string& स_नाम)
{
#यदि defined(_WIN32)
	पूर्णाङ्क ret = _mkdir(स_नाम.c_str());
#अन्यथा
	mode_t mode = 0755;
	पूर्णाङ्क ret = सूची_गढ़ो(स_नाम.c_str(), mode);
#endif
	यदि (ret == 0)
		फर्काऊ सत्य;

	विकल्प (त्रुटि_सं)
	{
	पक्ष ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = स_नाम.find_last_of('/');
			यदि (pos == std::string::npos)
#यदि defined(_WIN32)
				pos = स_नाम.find_last_of('\\');
			यदि (pos == std::string::npos)
#endif
				फर्काऊ असत्य;
			यदि (!create_directory( स_नाम.substr(0, pos) ))
				फर्काऊ असत्य;
		}
		// now, try to create again
#यदि defined(_WIN32)
		फर्काऊ 0 == _mkdir(स_नाम.c_str());
#अन्यथा
		फर्काऊ 0 == सूची_गढ़ो(स_नाम.c_str(), mode);
#endif

	पक्ष EEXIST:
		// done!
		फर्काऊ check_directory_exists(स_नाम);

	स्वभाव:
		फर्काऊ असत्य;
	}
}

तार्किक make_private_directory(अचर std::string& स_नाम)
{
#यदि defined(_WIN32) || defined(__wasi__)
	यदि (!create_directory(स_नाम)) {
		log_warning("Cannot create the directory %s\n", स_नाम.c_str());
		फर्काऊ असत्य;
	}
	फर्काऊ सत्य;
#अन्यथा
	यदि (सूची_गढ़ो(स_नाम.c_str(), 0700) == 0)
		फर्काऊ सत्य;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	पूर्णाङ्क err = त्रुटि_सं;
	std::string parent = std::filesystem::path(स_नाम).parent_path().string();

	यदि (err == ENOENT && !parent.empty() && create_directory(parent)) {
		यदि (सूची_गढ़ो(स_नाम.c_str(), 0700) == 0)
			फर्काऊ सत्य;
		err = त्रुटि_सं;
	}

	यदि (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", स_नाम.c_str(), म_त्रुटि(err));
		फर्काऊ असत्य;
	}

	// Do not use a leftover that isn't ours
	संरचना stat st;
	यदि (lstat(स_नाम.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", स_नाम.c_str());
		फर्काऊ असत्य;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	यदि (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(स_नाम.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", स_नाम.c_str(), म_त्रुटि(त्रुटि_सं));
		फर्काऊ असत्य;
	}

	फर्काऊ सत्य;
#endif
}

std::string make_user_tmpdir(अचर std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#यदि defined(_WIN32) || defined(__wasi__)
	std::string स_नाम = get_base_tmpdir() + "/" + name;
#अन्यथा
	std::string स_नाम = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (अचिन्हित)geteuid());
#endif
	फर्काऊ make_private_directory(स_नाम) ? स_नाम : "";
}

std::string escape_filename_spaces(अचर std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	लागि (स्वतः c : filename)
	{
		यदि (c == ' ')
			out += "\\ ";
		अन्यथा
			out.थप_back(c);
	}
	फर्काऊ out;
}

शून्य append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	फर्काऊ std::filesystem::path(path).filename().string();
}

std::string absolute_path(अचर std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	यदि (ec)
		फर्काऊ "";

	result = std::filesystem::weakly_canonical(result, ec);

	यदि (ec)
		फर्काऊ "";

	फर्काऊ result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	स्वतः parent = std::filesystem::path(path).parent_path();
	यदि (parent.empty()) {
		फर्काऊ "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	यदि (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	फर्काऊ result;
}

शून्य format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	लागि (माप_प्रकार i = 0; i < fmt.size(); ++i) {
		वर्ण ch = fmt[i];
		result.थप_back(ch);
		यदि (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	फर्काऊ result;
}

स्थिर std::string string_view_stringf(std::string_view spec, ...)
{
	बहु_सूची ap;
	बहु_शुरू(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	बहु_पूर्ण(ap);
	फर्काऊ result;
}

स्थिर पूर्णाङ्क spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	विकल्प (spec[spec.size() - 1]) {
	पक्ष 'd':
	पक्ष 'i':
	पक्ष 'o':
	पक्ष 'u':
	पक्ष 'x':
	पक्ष 'X':
		विकल्प (spec[spec.size() - 2]) {
		पक्ष 'h':
			यदि (spec[spec.size() - 3] == 'h')
				फर्काऊ परिमाण(वर्ण);
			फर्काऊ परिमाण(लघु);
		पक्ष 'l':
			यदि (spec[spec.size() - 3] == 'l')
				फर्काऊ परिमाण(दीर्घ दीर्घ);
			फर्काऊ परिमाण(दीर्घ);
		पक्ष 'L':
		पक्ष 'q':
			फर्काऊ परिमाण(दीर्घ दीर्घ);
		पक्ष 'j':
			फर्काऊ परिमाण(intmax_t);
		पक्ष 'z':
		पक्ष 'Z':
			फर्काऊ परिमाण(माप_प्रकार);
		पक्ष 't':
			फर्काऊ परिमाण(सूचक_भेद_प्रकार);
		}
		फर्काऊ परिमाण(पूर्णाङ्क);
	पक्ष 'e':
	पक्ष 'E':
	पक्ष 'f':
	पक्ष 'F':
	पक्ष 'g':
	पक्ष 'G':
	पक्ष 'a':
	पक्ष 'A':
		यदि (spec[spec.size() - 2] == 'L')
			फर्काऊ परिमाण(दीर्घ द्विगुण);
		यदि (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			फर्काऊ परिमाण(दीर्घ द्विगुण);
		फर्काऊ परिमाण(द्विगुण);
	पक्ष 'c':
		यदि (spec[spec.size() - 2] == 'l') {
			फर्काऊ परिमाण(ब_अक्षर_प्रकार);
		}
		फर्काऊ परिमाण(अचिन्हित वर्ण);
	पक्ष 'C':
		फर्काऊ परिमाण(ब_अक्षर_प्रकार);
	पक्ष 's':
	पक्ष 'p':
	पक्ष 'S':
	पक्ष 'n':
		फर्काऊ परिमाण(शून्य *);
	पक्ष 'm':
		फर्काऊ परिमाण(पूर्णाङ्क);
	स्वभाव:
		फर्काऊ -1;
	}
}

ढाँचा <typename Arg>
स्थिर शून्य format_emit_stringf(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	विकल्प (अङ्क_dynamic_ints) {
	पक्ष Dynamic_Int_गन्ती::NONE:
		result += string_view_stringf(spec, arg);
		फर्काऊ;
	पक्ष Dynamic_Int_गन्ती::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		फर्काऊ;
	पक्ष Dynamic_Int_गन्ती::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		फर्काऊ;
	}
	YOSYS_ABORT("Internal error");
}

शून्य format_emit_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, दीर्घ दीर्घ arg)
{
	यदि (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<पूर्णाङ्क>(arg));
		फर्काऊ;
	}
	यदि (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, (पूर्णाङ्क)arg);
	} अन्यथा {
		format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, arg);
	}
}

शून्य format_emit_unsigned_long_long(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, अचिन्हित दीर्घ दीर्घ arg)
{
	यदि (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<अचिन्हित पूर्णाङ्क>(arg));
		फर्काऊ;
	}
	यदि (spec == "%c") {
		result += static_cast<वर्ण>(arg);
		फर्काऊ;
	}
	यदि (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, (अचिन्हित पूर्णाङ्क)arg);
	} अन्यथा {
		format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, arg);
	}
}

शून्य format_emit_double(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, द्विगुण arg)
{
	format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, arg);
}

शून्य format_emit_char_ptr(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, अचर वर्ण *arg)
{
	यदि (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		फर्काऊ;
	}
	format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, arg);
}

शून्य format_emit_string(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, अचर std::string &arg)
{
	यदि (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		फर्काऊ;
	}
	format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, arg.c_str());
}

शून्य format_emit_string_view(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, std::string_view arg)
{
	यदि (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		फर्काऊ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, std::string(arg).c_str());
}

शून्य format_emit_void_ptr(std::string &result, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गन्ती अङ्क_dynamic_ints, अचर शून्य *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, अङ्क_dynamic_ints, arg);
}

#यदि defined(_WIN32)
ScopedFileLock::ScopedFileLock(अचर std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			रिक्त, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, रिक्त);
	यदि (h == INVALID_HANDLE_मान) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		फर्काऊ;
	}
	OVERLAPPED ov = {};
	यदि (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		फर्काऊ;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	यदि (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#अथवायदि !defined(__wasi__)
ScopedFileLock::ScopedFileLock(अचर std::string &path)
{
	fd = खोलो(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	यदि (fd < 0)
		fd = खोलो(path.c_str(), O_RDONLY | O_CLOEXEC);
	यदि (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		फर्काऊ;
	}
	पूर्णाङ्क ret;
	जबसम्म ((ret = flock(fd, LOCK_EX)) != 0 && त्रुटि_सं == EINTR);
	यदि (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	यदि (fd >= 0)
		बंद(fd); // releases the flock
}
#अन्यथा
ScopedFileLock::ScopedFileLock(अचर std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

अचिन्हित get_process_id()
{
#यदि defined(_WIN32)
	फर्काऊ GetCurrentProcessId();
#अथवायदि defined(__wasi__)
	फर्काऊ 0;
#अन्यथा
	फर्काऊ getpid();
#endif
}

YOSYS_NAMESPACE_END
