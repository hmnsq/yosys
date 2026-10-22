#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <filesystem>
#अन्तर्भाव <system_error>

#जर !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <io.h>
#endif

#जर !defined(_WIN32) && !defined(__wasi__)
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <cerrno>
#endif

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

पूर्णांक readsome(std::istream &f, अक्षर *s, पूर्णांक n)
{
	पूर्णांक rc = पूर्णांक(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	जर (rc == 0) {
		पूर्णांक c = f.get();
		जर (c != खातापूर्ण) {
			*s = c;
			rc = 1;
		}
	}

	परत rc;
}

std::string next_token(std::string &text, स्थिरांक अक्षर *sep, तार्किक long_strings)
{
	माप_प्रकार pos_begin = text.find_first_not_of(sep);

	जर (pos_begin == std::string::npos)
		pos_begin = text.size();

	जर (long_strings && pos_begin != text.size() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		साठी (माप_प्रकार i = pos_begin+1; i < text.size(); i++) {
			जर (text[i] == '"' && (i+1 == text.size() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				परत token;
			}
			जर (i+1 < text.size() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.size() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				परत token + ";";
			}
		}
	}

	माप_प्रकार pos_end = text.find_first_of(sep, pos_begin);

	जर (pos_end == std::string::npos)
		pos_end = text.size();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	परत token;
}

std::vector<std::string> split_tokens(स्थिरांक std::string &text, स्थिरांक अक्षर *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	साठी (अक्षर c : text) {
		जर (म_अक्षर(sep, c)) {
			जर (!current_token.empty()) {
				tokens.ढकला_back(current_token);
				current_token.clear();
			}
		} अन्यथा
			current_token += c;
	}
	जर (!current_token.empty()) {
		tokens.ढकला_back(current_token);
		current_token.clear();
	}
	परत tokens;
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
तार्किक patmatch(स्थिरांक अक्षर *pattern, स्थिरांक अक्षर *string)
{
	जर (*pattern == 0)
		परत *string == 0;

	जर (*pattern == '\\') {
		जर (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			परत सत्य;
	}

	जर (*pattern == '?') {
		जर (*string == 0)
			परत असत्य;
		परत patmatch(pattern+1, string+1);
	}

	जर (*pattern == '*') {
		पर्यंत (*string) {
			जर (patmatch(pattern+1, string++))
				परत सत्य;
		}
		परत pattern[1] == 0;
	}

	जर (*pattern == '[') {
		तार्किक found_match = असत्य;
		तार्किक inverted_list = pattern[1] == '!';
		स्थिरांक अक्षर *p = pattern + (inverted_list ? 1 : 0);

		पर्यंत (*++p) {
			जर (*p == ']') {
				जर (found_match != inverted_list && patmatch(p+1, string+1))
					परत सत्य;
				थांबा;
			}

			जर (*p == '\\') {
				जर (*++p == *string)
					found_match = सत्य;
			} अन्यथा
			जर (*p == *string)
				found_match = सत्य;
		}
	}

	जर (*pattern == *string)
		परत patmatch(pattern+1, string+1);

	परत असत्य;
}

std::string get_base_tmpdir()
{
	स्थिर std::string tmpdir;

	जर (!tmpdir.empty()) {
		परत tmpdir;
	}

#जर defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	जर (!GetTempPathW(wtmppath.size(), wtmppath.data()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), रिक्त, 0, रिक्त, रिक्त));
	जर (WideCharToMultiByte(CP_UTF8, 0, wtmppath.data(), wtmppath.size(), utmppath.data(), utmppath.size(), रिक्त, रिक्त) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#अन्यथा
	अक्षर * var = std::दो_पर्या("TMPDIR");
	जर (var && म_माप(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		पर्यंत (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.काढा_back();
		}
	} अन्यथा {
		tmpdir.assign("/tmp");
	}
#endif
	परत tmpdir;
}

std::string make_temp_file(std::string template_str)
{
	माप_प्रकार pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#जर defined(__wasm)
	स्थिर माप_प्रकार index = 0;
	template_str.replace(pos, 6, stringf("%06zu", index++));
#किंवाजर defined(_WIN32)
#ifndef YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#endif
	पर्यंत (1) {
		साठी (पूर्णांक i = 0; i < 6; i++) {
			स्थिर std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			स्थिर uint32_t x = 314159265 ^ uint32_t(समय(रिक्त));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.size()];
		}
		जर (_access(template_str.c_str(), 0) != 0)
			थांबा;
	}
#अन्यथा
	पूर्णांक suffixlen = template_str.size() - pos - 6;

	अक्षर *p = strdup(template_str.c_str());
	बंद(mkstemps(p, suffixlen));
	template_str = p;
	मुक्त(p);
#endif

	परत template_str;
}

std::string make_temp_dir(std::string template_str)
{
#जर defined(_WIN32)
	template_str = make_temp_file(template_str);
	सूची_गढ़ो(template_str.c_str());
	परत template_str;
#किंवाजर defined(__wasm)
	template_str = make_temp_file(template_str);
	सूची_गढ़ो(template_str.c_str(), 0777);
	परत template_str;
#अन्यथा
#  ifndef न_संशोधन
	माप_प्रकार pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	पूर्णांक suffixlen = template_str.size() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	अक्षर *p = strdup(template_str.c_str());
	log_assert(p);
	अक्षर *res = mkdtemp(p);
	जर (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, म_त्रुटि(त्रुटि_सं), त्रुटि_सं);
	template_str = p;
	मुक्त(p);

	परत template_str;
#endif
}

तार्किक check_is_directory(स्थिरांक std::string& स_नाम)
{
#जर defined(_WIN32)
	संरचना _stat info;
	स्वतः dirname_ = स_नाम;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	जर (स_नाम.back() == '/' || स_नाम.back() == '\\') {
		dirname_ = स_नाम.substr(0, स_नाम.length() - 1);
	}

	जर (_stat(dirname_.c_str(), &info) != 0)
	{
		परत असत्य;
	}
	परत (info.st_mode & _S_IFDIR) != 0;
#अन्यथा
	संरचना stat info;
	जर (stat(स_नाम.c_str(), &info) != 0)
	{
		परत असत्य;
	}
	परत (info.st_mode & S_IFDIR) != 0;
#endif
}

#ifdef _WIN32
तार्किक check_accessible(स्थिरांक std::string& filename, तार्किक)
{
	परत _access(filename.c_str(), 0) == 0;
}
#अन्यथा
तार्किक check_accessible(स्थिरांक std::string& filename, तार्किक is_exec)
{
	परत access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#endif

तार्किक check_file_exists(स्थिरांक std::string& filename, तार्किक is_exec)
{
	परत check_accessible(filename, is_exec) && !check_is_directory(filename);
}
तार्किक check_directory_exists(स्थिरांक std::string& filename, तार्किक is_exec)
{
	परत check_accessible(filename, is_exec) && check_is_directory(filename);
}

तार्किक is_absolute_path(std::string filename)
{
#ifdef _WIN32
	परत filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#अन्यथा
	परत filename[0] == '/';
#endif
}

शून्य remove_directory(std::string स_नाम)
{
#ifdef _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", स_नाम));
#अन्यथा
	संरचना stat stbuf;
	संरचना dirent **namelist;
	पूर्णांक n = scandir(स_नाम.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	साठी (पूर्णांक i = 0; i < n; i++) {
		जर (म_भेद(namelist[i]->d_name, ".") && म_भेद(namelist[i]->d_name, "..")) {
			std::string buffer = stringf("%s/%s", स_नाम, namelist[i]->d_name);
			जर (!stat(buffer.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
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

तार्किक create_directory(स्थिरांक std::string& स_नाम)
{
#जर defined(_WIN32)
	पूर्णांक ret = _mkdir(स_नाम.c_str());
#अन्यथा
	mode_t mode = 0755;
	पूर्णांक ret = सूची_गढ़ो(स_नाम.c_str(), mode);
#endif
	जर (ret == 0)
		परत सत्य;

	निवड (त्रुटि_सं)
	{
	स्थिती ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::size_type pos = स_नाम.find_last_of('/');
			जर (pos == std::string::npos)
#जर defined(_WIN32)
				pos = स_नाम.find_last_of('\\');
			जर (pos == std::string::npos)
#endif
				परत असत्य;
			जर (!create_directory( स_नाम.substr(0, pos) ))
				परत असत्य;
		}
		// now, try to create again
#जर defined(_WIN32)
		परत 0 == _mkdir(स_नाम.c_str());
#अन्यथा
		परत 0 == सूची_गढ़ो(स_नाम.c_str(), mode);
#endif

	स्थिती EEXIST:
		// done!
		परत check_directory_exists(स_नाम);

	पूर्वनिर्धारित:
		परत असत्य;
	}
}

तार्किक make_private_directory(स्थिरांक std::string& स_नाम)
{
#जर defined(_WIN32) || defined(__wasi__)
	जर (!create_directory(स_नाम)) {
		log_warning("Cannot create the directory %s\n", स_नाम.c_str());
		परत असत्य;
	}
	परत सत्य;
#अन्यथा
	जर (सूची_गढ़ो(स_नाम.c_str(), 0700) == 0)
		परत सत्य;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	पूर्णांक err = त्रुटि_सं;
	std::string parent = std::filesystem::path(स_नाम).parent_path().string();

	जर (err == ENOENT && !parent.empty() && create_directory(parent)) {
		जर (सूची_गढ़ो(स_नाम.c_str(), 0700) == 0)
			परत सत्य;
		err = त्रुटि_सं;
	}

	जर (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", स_नाम.c_str(), म_त्रुटि(err));
		परत असत्य;
	}

	// Do not use a leftover that isn't ours
	संरचना stat st;
	जर (lstat(स_नाम.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", स_नाम.c_str());
		परत असत्य;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	जर (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(स_नाम.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", स_नाम.c_str(), म_त्रुटि(त्रुटि_सं));
		परत असत्य;
	}

	परत सत्य;
#endif
}

std::string make_user_tmpdir(स्थिरांक std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#जर defined(_WIN32) || defined(__wasi__)
	std::string स_नाम = get_base_tmpdir() + "/" + name;
#अन्यथा
	std::string स_नाम = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (अचिन्हित)geteuid());
#endif
	परत make_private_directory(स_नाम) ? स_नाम : "";
}

std::string escape_filename_spaces(स्थिरांक std::string& filename)
{
	std::string out;
	out.reserve(filename.size());
	साठी (स्वतः c : filename)
	{
		जर (c == ' ')
			out += "\\ ";
		अन्यथा
			out.ढकला_back(c);
	}
	परत out;
}

शून्य append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	परत std::filesystem::path(path).filename().string();
}

std::string absolute_path(स्थिरांक std::string& path)
{
	std::error_code ec;
	std::filesystem::path result = std::filesystem::absolute(path, ec);

	जर (ec)
		परत "";

	result = std::filesystem::weakly_canonical(result, ec);

	जर (ec)
		परत "";

	परत result.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	स्वतः parent = std::filesystem::path(path).parent_path();
	जर (parent.empty()) {
		परत "";
	}
	// Add trailing separator to match original behavior
	std::string result = parent.string();
	जर (!result.empty() && result.back() != std::filesystem::path::preferred_separator) {
		result += std::filesystem::path::preferred_separator;
	}
	परत result;
}

शून्य format_emit_unescaped(std::string &result, std::string_view fmt)
{
	result.reserve(result.size() + fmt.size());
	साठी (माप_प्रकार i = 0; i < fmt.size(); ++i) {
		अक्षर ch = fmt[i];
		result.ढकला_back(ch);
		जर (ch == '%' && i + 1 < fmt.size() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string result;
	format_emit_unescaped(result, fmt);
	परत result;
}

स्थिर std::string string_view_stringf(std::string_view spec, ...)
{
	बहु_सूची ap;
	बहु_शुरू(ap, spec);
	std::string result = vstringf(std::string(spec).c_str(), ap);
	बहु_पूर्ण(ap);
	परत result;
}

स्थिर पूर्णांक spec_parameter_size(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	निवड (spec[spec.size() - 1]) {
	स्थिती 'd':
	स्थिती 'i':
	स्थिती 'o':
	स्थिती 'u':
	स्थिती 'x':
	स्थिती 'X':
		निवड (spec[spec.size() - 2]) {
		स्थिती 'h':
			जर (spec[spec.size() - 3] == 'h')
				परत आकार(अक्षर);
			परत आकार(लघु);
		स्थिती 'l':
			जर (spec[spec.size() - 3] == 'l')
				परत आकार(दीर्घ दीर्घ);
			परत आकार(दीर्घ);
		स्थिती 'L':
		स्थिती 'q':
			परत आकार(दीर्घ दीर्घ);
		स्थिती 'j':
			परत आकार(intmax_t);
		स्थिती 'z':
		स्थिती 'Z':
			परत आकार(माप_प्रकार);
		स्थिती 't':
			परत आकार(सूचक_भेद_प्रकार);
		}
		परत आकार(पूर्णांक);
	स्थिती 'e':
	स्थिती 'E':
	स्थिती 'f':
	स्थिती 'F':
	स्थिती 'g':
	स्थिती 'G':
	स्थिती 'a':
	स्थिती 'A':
		जर (spec[spec.size() - 2] == 'L')
			परत आकार(दीर्घ द्विगुण);
		जर (spec[spec.size() - 2] == 'l' && spec[spec.size() - 3] == 'l')
			परत आकार(दीर्घ द्विगुण);
		परत आकार(द्विगुण);
	स्थिती 'c':
		जर (spec[spec.size() - 2] == 'l') {
			परत आकार(ब_अक्षर_प्रकार);
		}
		परत आकार(अचिन्हित अक्षर);
	स्थिती 'C':
		परत आकार(ब_अक्षर_प्रकार);
	स्थिती 's':
	स्थिती 'p':
	स्थिती 'S':
	स्थिती 'n':
		परत आकार(शून्य *);
	स्थिती 'm':
		परत आकार(पूर्णांक);
	पूर्वनिर्धारित:
		परत -1;
	}
}

ढाँचा <typename Arg>
स्थिर शून्य format_emit_stringf(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	निवड (अंक_dynamic_ints) {
	स्थिती Dynamic_Int_मोजणी::NONE:
		result += string_view_stringf(spec, arg);
		परत;
	स्थिती Dynamic_Int_मोजणी::ONE:
		result += string_view_stringf(spec, dynamic_ints[0], arg);
		परत;
	स्थिती Dynamic_Int_मोजणी::TWO:
		result += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		परत;
	}
	YOSYS_ABORT("Internal error");
}

शून्य format_emit_long_long(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, दीर्घ दीर्घ arg)
{
	जर (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<पूर्णांक>(arg));
		परत;
	}
	जर (spec_parameter_size(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, (पूर्णांक)arg);
	} अन्यथा {
		format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, arg);
	}
}

शून्य format_emit_unsigned_long_long(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, अचिन्हित दीर्घ दीर्घ arg)
{
	जर (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += std::to_string(static_cast<अचिन्हित पूर्णांक>(arg));
		परत;
	}
	जर (spec == "%c") {
		result += static_cast<अक्षर>(arg);
		परत;
	}
	जर (spec_parameter_size(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, (अचिन्हित पूर्णांक)arg);
	} अन्यथा {
		format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, arg);
	}
}

शून्य format_emit_double(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, द्विगुण arg)
{
	format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, arg);
}

शून्य format_emit_char_ptr(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, स्थिरांक अक्षर *arg)
{
	जर (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		परत;
	}
	format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, arg);
}

शून्य format_emit_string(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, स्थिरांक std::string &arg)
{
	जर (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		result += arg;
		परत;
	}
	format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, arg.c_str());
}

शून्य format_emit_string_view(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, std::string_view arg)
{
	जर (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		result += arg;
		परत;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, std::string(arg).c_str());
}

शून्य format_emit_void_ptr(std::string &result, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_मोजणी अंक_dynamic_ints, स्थिरांक शून्य *arg)
{
	format_emit_stringf(result, spec, dynamic_ints, अंक_dynamic_ints, arg);
}

#जर defined(_WIN32)
ScopedFileLock::ScopedFileLock(स्थिरांक std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			रिक्त, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, रिक्त);
	जर (h == INVALID_HANDLE_मूल्य) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		परत;
	}
	OVERLAPPED ov = {};
	जर (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		परत;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	जर (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#किंवाजर !defined(__wasi__)
ScopedFileLock::ScopedFileLock(स्थिरांक std::string &path)
{
	fd = खोलो(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	जर (fd < 0)
		fd = खोलो(path.c_str(), O_RDONLY | O_CLOEXEC);
	जर (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		परत;
	}
	पूर्णांक ret;
	पर्यंत ((ret = flock(fd, LOCK_EX)) != 0 && त्रुटि_सं == EINTR);
	जर (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	जर (fd >= 0)
		बंद(fd); // releases the flock
}
#अन्यथा
ScopedFileLock::ScopedFileLock(स्थिरांक std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#endif

अचिन्हित get_process_id()
{
#जर defined(_WIN32)
	परत GetCurrentProcessId();
#किंवाजर defined(__wasi__)
	परत 0;
#अन्यथा
	परत getpid();
#endif
}

YOSYS_NAMESPACE_END
