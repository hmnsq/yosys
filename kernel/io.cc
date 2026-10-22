#समावेश "kernel/yosys_common.h"
#समावेश "kernel/log.h"
#समावेश <iostream>
#समावेश <string>
#समावेश <filesystem>
#समावेश <system_error>

#अगर !defined(WIN32)
#समावेश <dirent.h>
#समावेश <unistd.h>
#अन्यथा
#समावेश <io.h>
#पूर्ण_अगर

#अगर !defined(_WIN32) && !defined(__wasi__)
#समावेश <sys/file.h>
#समावेश <fcntl.h>
#समावेश <cerrno>
#पूर्ण_अगर

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

पूर्णांक readsome(std::अ_प्रवाह &f, अक्षर *s, पूर्णांक n)
{
	पूर्णांक rc = पूर्णांक(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	अगर (rc == 0) {
		पूर्णांक c = f.लो();
		अगर (c != खातापूर्ण) {
			*s = c;
			rc = 1;
		}
	}

	वापस rc;
}

std::string अग्र_token(std::string &text, स्थिर अक्षर *sep, bool long_strings)
{
	माप_प्रकार pos_begin = text.find_first_not_of(sep);

	अगर (pos_begin == std::string::npos)
		pos_begin = text.आकार();

	अगर (long_strings && pos_begin != text.आकार() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		क्रम (माप_प्रकार i = pos_begin+1; i < text.आकार(); i++) {
			अगर (text[i] == '"' && (i+1 == text.आकार() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				वापस token;
			}
			अगर (i+1 < text.आकार() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.आकार() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				वापस token + ";";
			}
		}
	}

	माप_प्रकार pos_end = text.find_first_of(sep, pos_begin);

	अगर (pos_end == std::string::npos)
		pos_end = text.आकार();

	std::string token = text.substr(pos_begin, pos_end-pos_begin);
	text = text.substr(pos_end);
	वापस token;
}

std::vector<std::string> split_tokens(स्थिर std::string &text, स्थिर अक्षर *sep)
{
	std::vector<std::string> tokens;
	std::string current_token;
	क्रम (अक्षर c : text) {
		अगर (म_अक्षर(sep, c)) {
			अगर (!current_token.empty()) {
				tokens.धकेलो_back(current_token);
				current_token.सफा();
			}
		} अन्यथा
			current_token += c;
	}
	अगर (!current_token.empty()) {
		tokens.धकेलो_back(current_token);
		current_token.सफा();
	}
	वापस tokens;
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
bool patmatch(स्थिर अक्षर *pattern, स्थिर अक्षर *string)
{
	अगर (*pattern == 0)
		वापस *string == 0;

	अगर (*pattern == '\\') {
		अगर (pattern[1] == string[0] && patmatch(pattern+2, string+1))
			वापस true;
	}

	अगर (*pattern == '?') {
		अगर (*string == 0)
			वापस false;
		वापस patmatch(pattern+1, string+1);
	}

	अगर (*pattern == '*') {
		जबतक (*string) {
			अगर (patmatch(pattern+1, string++))
				वापस true;
		}
		वापस pattern[1] == 0;
	}

	अगर (*pattern == '[') {
		bool found_match = false;
		bool inverted_list = pattern[1] == '!';
		स्थिर अक्षर *p = pattern + (inverted_list ? 1 : 0);

		जबतक (*++p) {
			अगर (*p == ']') {
				अगर (found_match != inverted_list && patmatch(p+1, string+1))
					वापस true;
				अवरोध;
			}

			अगर (*p == '\\') {
				अगर (*++p == *string)
					found_match = true;
			} अन्यथा
			अगर (*p == *string)
				found_match = true;
		}
	}

	अगर (*pattern == *string)
		वापस patmatch(pattern+1, string+1);

	वापस false;
}

std::string get_base_tmpdir()
{
	अटल std::string tmpdir;

	अगर (!tmpdir.empty()) {
		वापस tmpdir;
	}

#अगर defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	अगर (!Get_अस्थायी_Path_W(wtmppath.आकार(), wtmppath.तथ्य()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(WideCharToMultiByte(CP_UTF8, 0, wtmppath.तथ्य(), wtmppath.आकार(), शून्य, 0, शून्य, शून्य));
	अगर (WideCharToMultiByte(CP_UTF8, 0, wtmppath.तथ्य(), wtmppath.आकार(), utmppath.तथ्य(), utmppath.आकार(), शून्य, शून्य) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#अन्यथा
	अक्षर * var = std::दो_पर्या("TMPDIR");
	अगर (var && म_माप(var)!=0) {
		tmpdir.assign(var);
		// We return the directory name without the trailing '/'
		जबतक (!tmpdir.empty() && (tmpdir.back() == '/')) {
			tmpdir.निकालो_back();
		}
	} अन्यथा {
		tmpdir.assign("/tmp");
	}
#पूर्ण_अगर
	वापस tmpdir;
}

std::string make_अस्थायी_file(std::string template_str)
{
	माप_प्रकार pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#अगर defined(__wasm)
	अटल माप_प्रकार सूचकांक = 0;
	template_str.replace(pos, 6, stringf("%06zu", सूचकांक++));
#या_अगर defined(_WIN32)
#अगर_अघोषित YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.end(), '/', '\\');
#पूर्ण_अगर
	जबतक (1) {
		क्रम (पूर्णांक i = 0; i < 6; i++) {
			अटल std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			अटल uint32_t x = 314159265 ^ uint32_t(समय(शून्य));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.आकार()];
		}
		अगर (_access(template_str.c_str(), 0) != 0)
			अवरोध;
	}
#अन्यथा
	पूर्णांक suffixlen = template_str.आकार() - pos - 6;

	अक्षर *p = strdup(template_str.c_str());
	बंद(mkstemps(p, suffixlen));
	template_str = p;
	मुक्त(p);
#पूर्ण_अगर

	वापस template_str;
}

std::string make_अस्थायी_dir(std::string template_str)
{
#अगर defined(_WIN32)
	template_str = make_अस्थायी_file(template_str);
	सूची_गढ़ो(template_str.c_str());
	वापस template_str;
#या_अगर defined(__wasm)
	template_str = make_अस्थायी_file(template_str);
	सूची_गढ़ो(template_str.c_str(), 0777);
	वापस template_str;
#अन्यथा
#  ifndef न_संशोधन
	माप_प्रकार pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	पूर्णांक suffixlen = template_str.आकार() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	अक्षर *p = strdup(template_str.c_str());
	log_assert(p);
	अक्षर *res = mkdtemp(p);
	अगर (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, म_त्रुटि(त्रुटि_सं), त्रुटि_सं);
	template_str = p;
	मुक्त(p);

	वापस template_str;
#पूर्ण_अगर
}

bool check_is_directory(स्थिर std::string& स_नाम)
{
#अगर defined(_WIN32)
	काष्ठा _stat info;
	स्वतः dirname_ = स_नाम;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	अगर (स_नाम.back() == '/' || स_नाम.back() == '\\') {
		dirname_ = स_नाम.substr(0, स_नाम.लंबाई() - 1);
	}

	अगर (_stat(dirname_.c_str(), &info) != 0)
	{
		वापस false;
	}
	वापस (info.st_mode & _S_IFDIR) != 0;
#अन्यथा
	काष्ठा stat info;
	अगर (stat(स_नाम.c_str(), &info) != 0)
	{
		वापस false;
	}
	वापस (info.st_mode & S_IFDIR) != 0;
#पूर्ण_अगर
}

#अगर_घोषित _WIN32
bool check_accessible(स्थिर std::string& filename, bool)
{
	वापस _access(filename.c_str(), 0) == 0;
}
#अन्यथा
bool check_accessible(स्थिर std::string& filename, bool is_exec)
{
	वापस access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#पूर्ण_अगर

bool check_file_exists(स्थिर std::string& filename, bool is_exec)
{
	वापस check_accessible(filename, is_exec) && !check_is_directory(filename);
}
bool check_directory_exists(स्थिर std::string& filename, bool is_exec)
{
	वापस check_accessible(filename, is_exec) && check_is_directory(filename);
}

bool is_absolute_path(std::string filename)
{
#अगर_घोषित _WIN32
	वापस filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#अन्यथा
	वापस filename[0] == '/';
#पूर्ण_अगर
}

व्योम remove_directory(std::string स_नाम)
{
#अगर_घोषित _WIN32
	run_command(stringf("rmdir /s /q \"%s\"", स_नाम));
#अन्यथा
	काष्ठा stat stbuf;
	काष्ठा dirent **namelist;
	पूर्णांक n = scandir(स_नाम.c_str(), &namelist, nullptr, alphasort);
	log_assert(n >= 0);
	क्रम (पूर्णांक i = 0; i < n; i++) {
		अगर (म_भेद(namelist[i]->d_name, ".") && म_भेद(namelist[i]->d_name, "..")) {
			std::string बफर = stringf("%s/%s", स_नाम, namelist[i]->d_name);
			अगर (!stat(बफर.c_str(), &stbuf) && S_ISREG(stbuf.st_mode)) {
				हटाओ(बफर.c_str());
			} अन्यथा
				remove_directory(बफर);
		}
		मुक्त(namelist[i]);
	}
	मुक्त(namelist);
	सूची_हटाओ(स_नाम.c_str());
#पूर्ण_अगर
}

bool create_directory(स्थिर std::string& स_नाम)
{
#अगर defined(_WIN32)
	पूर्णांक ret = _mkdir(स_नाम.c_str());
#अन्यथा
	mode_t mode = 0755;
	पूर्णांक ret = सूची_गढ़ो(स_नाम.c_str(), mode);
#पूर्ण_अगर
	अगर (ret == 0)
		वापस true;

	चयन (त्रुटि_सं)
	{
	हाल ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::आकार_type pos = स_नाम.find_last_of('/');
			अगर (pos == std::string::npos)
#अगर defined(_WIN32)
				pos = स_नाम.find_last_of('\\');
			अगर (pos == std::string::npos)
#पूर्ण_अगर
				वापस false;
			अगर (!create_directory( स_नाम.substr(0, pos) ))
				वापस false;
		}
		// now, try to create again
#अगर defined(_WIN32)
		वापस 0 == _mkdir(स_नाम.c_str());
#अन्यथा
		वापस 0 == सूची_गढ़ो(स_नाम.c_str(), mode);
#पूर्ण_अगर

	हाल EEXIST:
		// done!
		वापस check_directory_exists(स_नाम);

	शेष:
		वापस false;
	}
}

bool make_private_directory(स्थिर std::string& स_नाम)
{
#अगर defined(_WIN32) || defined(__wasi__)
	अगर (!create_directory(स_नाम)) {
		log_warning("Cannot create the directory %s\n", स_नाम.c_str());
		वापस false;
	}
	वापस true;
#अन्यथा
	अगर (सूची_गढ़ो(स_नाम.c_str(), 0700) == 0)
		वापस true;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	पूर्णांक err = त्रुटि_सं;
	std::string parent = std::filesystem::path(स_नाम).parent_path().string();

	अगर (err == ENOENT && !parent.empty() && create_directory(parent)) {
		अगर (सूची_गढ़ो(स_नाम.c_str(), 0700) == 0)
			वापस true;
		err = त्रुटि_सं;
	}

	अगर (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", स_नाम.c_str(), म_त्रुटि(err));
		वापस false;
	}

	// Do not use a leftover that isn't ours
	काष्ठा stat st;
	अगर (lstat(स_नाम.c_str(), &st) != 0 || !S_ISDIR(st.st_mode) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", स_नाम.c_str());
		वापस false;
	}
	// Repair the mode if something changed it
	mode_t mode = st.st_mode & 0777;
	अगर (((mode & S_IRWXU) != S_IRWXU || (mode & (S_IWGRP | S_IWOTH)) != 0) && chmod(स_नाम.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", स_नाम.c_str(), म_त्रुटि(त्रुटि_सं));
		वापस false;
	}

	वापस true;
#पूर्ण_अगर
}

std::string make_user_tmpdir(स्थिर std::string& name)
{
	log_assert(!name.empty() && name.find('/') == std::string::npos);
#अगर defined(_WIN32) || defined(__wasi__)
	std::string स_नाम = get_base_tmpdir() + "/" + name;
#अन्यथा
	std::string स_नाम = stringf("%s/%s-%u", get_base_tmpdir().c_str(), name.c_str(), (अचिन्हित)geteuid());
#पूर्ण_अगर
	वापस make_private_directory(स_नाम) ? स_नाम : "";
}

std::string escape_filename_spaces(स्थिर std::string& filename)
{
	std::string out;
	out.reserve(filename.आकार());
	क्रम (स्वतः c : filename)
	{
		अगर (c == ' ')
			out += "\\ ";
		अन्यथा
			out.धकेलो_back(c);
	}
	वापस out;
}

व्योम append_globbed(std::vector<std::string>& paths, std::string pattern)
{
	rewrite_filename(pattern);
	std::vector<std::string> globbed = glob_filename(pattern);
	copy(globbed.begin(), globbed.end(), back_inserter(paths));
}

std::string name_from_file_path(std::string path) {
	वापस std::filesystem::path(path).filename().string();
}

std::string absolute_path(स्थिर std::string& path)
{
	std::error_code ec;
	std::filesystem::path परिणाम = std::filesystem::absolute(path, ec);

	अगर (ec)
		वापस "";

	परिणाम = std::filesystem::weakly_canonical(परिणाम, ec);

	अगर (ec)
		वापस "";

	वापस परिणाम.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_path(std::string path) {
	स्वतः parent = std::filesystem::path(path).parent_path();
	अगर (parent.empty()) {
		वापस "";
	}
	// Add trailing separator to match original behavior
	std::string परिणाम = parent.string();
	अगर (!परिणाम.empty() && परिणाम.back() != std::filesystem::path::preferred_separator) {
		परिणाम += std::filesystem::path::preferred_separator;
	}
	वापस परिणाम;
}

व्योम format_emit_unescaped(std::string &परिणाम, std::string_view fmt)
{
	परिणाम.reserve(परिणाम.आकार() + fmt.आकार());
	क्रम (माप_प्रकार i = 0; i < fmt.आकार(); ++i) {
		अक्षर ch = fmt[i];
		परिणाम.धकेलो_back(ch);
		अगर (ch == '%' && i + 1 < fmt.आकार() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string परिणाम;
	format_emit_unescaped(परिणाम, fmt);
	वापस परिणाम;
}

अटल std::string string_view_stringf(std::string_view spec, ...)
{
	बहु_सूची ap;
	बहु_शुरू(ap, spec);
	std::string परिणाम = vstringf(std::string(spec).c_str(), ap);
	बहु_पूर्ण(ap);
	वापस परिणाम;
}

अटल पूर्णांक spec_parameter_आकार(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	चयन (spec[spec.आकार() - 1]) {
	हाल 'd':
	हाल 'i':
	हाल 'o':
	हाल 'u':
	हाल 'x':
	हाल 'X':
		चयन (spec[spec.आकार() - 2]) {
		हाल 'h':
			अगर (spec[spec.आकार() - 3] == 'h')
				वापस माप(अक्षर);
			वापस माप(लघु);
		हाल 'l':
			अगर (spec[spec.आकार() - 3] == 'l')
				वापस माप(दीर्घ दीर्घ);
			वापस माप(दीर्घ);
		हाल 'L':
		हाल 'q':
			वापस माप(दीर्घ दीर्घ);
		हाल 'j':
			वापस माप(intmax_t);
		हाल 'z':
		हाल 'Z':
			वापस माप(माप_प्रकार);
		हाल 't':
			वापस माप(सूचक_भेद_प्रकार);
		}
		वापस माप(पूर्णांक);
	हाल 'e':
	हाल 'E':
	हाल 'f':
	हाल 'F':
	हाल 'g':
	हाल 'G':
	हाल 'a':
	हाल 'A':
		अगर (spec[spec.आकार() - 2] == 'L')
			वापस माप(दीर्घ द्विगुन);
		अगर (spec[spec.आकार() - 2] == 'l' && spec[spec.आकार() - 3] == 'l')
			वापस माप(दीर्घ द्विगुन);
		वापस माप(द्विगुन);
	हाल 'c':
		अगर (spec[spec.आकार() - 2] == 'l') {
			वापस माप(ब_अक्षर_प्रकार);
		}
		वापस माप(अचिन्हित अक्षर);
	हाल 'C':
		वापस माप(ब_अक्षर_प्रकार);
	हाल 's':
	हाल 'p':
	हाल 'S':
	हाल 'n':
		वापस माप(व्योम *);
	हाल 'm':
		वापस माप(पूर्णांक);
	शेष:
		वापस -1;
	}
}

ढाँचा <typename Arg>
अटल व्योम format_emit_stringf(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	चयन (अंक_dynamic_ints) {
	हाल Dynamic_Int_गिनती::NONE:
		परिणाम += string_view_stringf(spec, arg);
		वापस;
	हाल Dynamic_Int_गिनती::ONE:
		परिणाम += string_view_stringf(spec, dynamic_ints[0], arg);
		वापस;
	हाल Dynamic_Int_गिनती::TWO:
		परिणाम += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		वापस;
	}
	YOSYS_ABORT("Internal error");
}

व्योम format_emit_long_long(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, दीर्घ दीर्घ arg)
{
	अगर (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += std::to_string(static_cast<पूर्णांक>(arg));
		वापस;
	}
	अगर (spec_parameter_आकार(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, (पूर्णांक)arg);
	} अन्यथा {
		format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, arg);
	}
}

व्योम format_emit_unsigned_long_long(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, अचिन्हित दीर्घ दीर्घ arg)
{
	अगर (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += std::to_string(static_cast<अचिन्हित पूर्णांक>(arg));
		वापस;
	}
	अगर (spec == "%c") {
		परिणाम += static_cast<अक्षर>(arg);
		वापस;
	}
	अगर (spec_parameter_आकार(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, (अचिन्हित पूर्णांक)arg);
	} अन्यथा {
		format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, arg);
	}
}

व्योम format_emit_double(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, द्विगुन arg)
{
	format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, arg);
}

व्योम format_emit_char_ptr(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, स्थिर अक्षर *arg)
{
	अगर (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += arg;
		वापस;
	}
	format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, arg);
}

व्योम format_emit_string(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, स्थिर std::string &arg)
{
	अगर (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += arg;
		वापस;
	}
	format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, arg.c_str());
}

व्योम format_emit_string_view(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, std::string_view arg)
{
	अगर (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		परिणाम += arg;
		वापस;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, std::string(arg).c_str());
}

व्योम format_emit_void_ptr(std::string &परिणाम, std::string_view spec, पूर्णांक *dynamic_ints,
	Dynamic_Int_गिनती अंक_dynamic_ints, स्थिर व्योम *arg)
{
	format_emit_stringf(परिणाम, spec, dynamic_ints, अंक_dynamic_ints, arg);
}

#अगर defined(_WIN32)
ScopedFileLock::ScopedFileLock(स्थिर std::string &path)
{
	HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			शून्य, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, शून्य);
	अगर (h == INVALID_HANDLE_मूल्य) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		वापस;
	}
	OVERLAPPED ov = {};
	अगर (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
		CloseHandle(h);
		वापस;
	}
	handle = h;
}

ScopedFileLock::~ScopedFileLock()
{
	अगर (handle != nullptr) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#या_अगर !defined(__wasi__)
ScopedFileLock::ScopedFileLock(स्थिर std::string &path)
{
	fd = खोलो(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	अगर (fd < 0)
		fd = खोलो(path.c_str(), O_RDONLY | O_CLOEXEC);
	अगर (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", path.c_str());
		वापस;
	}
	पूर्णांक ret;
	जबतक ((ret = flock(fd, LOCK_EX)) != 0 && त्रुटि_सं == EINTR);
	अगर (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", path.c_str());
}

ScopedFileLock::~ScopedFileLock()
{
	अगर (fd >= 0)
		बंद(fd); // releases the flock
}
#अन्यथा
ScopedFileLock::ScopedFileLock(स्थिर std::string &) {}
ScopedFileLock::~ScopedFileLock() {}
#पूर्ण_अगर

अचिन्हित get_process_id()
{
#अगर defined(_WIN32)
	वापस GetCurrentProcessId();
#या_अगर defined(__wasi__)
	वापस 0;
#अन्यथा
	वापस getpid();
#पूर्ण_अगर
}

YOSYS_NAMESPACE_END
