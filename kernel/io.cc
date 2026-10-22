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
#यदि_अन्तः

#यदि !defined(_WIN32) && !defined(__wasi__)
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <cerrno>
#यदि_अन्तः

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

पूर्णाङ्क readsome(std::istream &f, वर्ण *s, पूर्णाङ्क n)
{
	पूर्णाङ्क rc = पूर्णाङ्क(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	यदि (rc == 0) {
		पूर्णाङ्क c = f.get();
		यदि (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	प्रत्यागम rc;
}

std::string अग्र_token(std::string &text, ध्रुव वर्ण *sep, तार्किक long_वाक्यानि)
{
	आकार_t pos_begin = text.find_first_not_सम्बन्धि(sep);

	यदि (pos_begin == std::string::npos)
		pos_begin = text.आकार();

	यदि (long_वाक्यानि && pos_begin != text.आकार() && text[pos_begin] == '"') {
		std::string sep_string = sep;
		प्रति (आकार_t i = pos_begin+1; i < text.आकार(); i++) {
			यदि (text[i] == '"' && (i+1 == text.आकार() || sep_string.find(text[i+1]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+1);
				प्रत्यागम token;
			}
			यदि (i+1 < text.आकार() && text[i] == '"' && text[i+1] == ';' && (i+2 == text.आकार() || sep_string.find(text[i+2]) != std::string::npos)) {
				std::string token = text.substr(pos_begin, i-pos_begin+1);
				text = text.substr(i+2);
				प्रत्यागम token + ";";
			}
		}
	}

	आकार_t pos_अन्तः = text.find_first_सम्बन्धि(sep, pos_begin);

	यदि (pos_अन्तः == std::string::npos)
		pos_अन्तः = text.आकार();

	std::string token = text.substr(pos_begin, pos_अन्तः-pos_begin);
	text = text.substr(pos_अन्तः);
	प्रत्यागम token;
}

std::सदिशम्<std::string> विभज_tokens(ध्रुव std::string &text, ध्रुव वर्ण *sep)
{
	std::सदिशम्<std::string> tokens;
	std::string current_token;
	प्रति (वर्ण c : text) {
		यदि (strchr(sep, c)) {
			यदि (!current_token.रिक्तानि()) {
				tokens.प्रक्षिप_पृष्ठभागः(current_token);
				current_token.clear();
			}
		} अन्यथा
			current_token += c;
	}
	यदि (!current_token.रिक्तानि()) {
		tokens.प्रक्षिप_पृष्ठभागः(current_token);
		current_token.clear();
	}
	प्रत्यागम tokens;
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
तार्किक patmatch(ध्रुव वर्ण *आदर्शः, ध्रुव वर्ण *string)
{
	यदि (*आदर्शः == 0)
		प्रत्यागम *string == 0;

	यदि (*आदर्शः == '\\') {
		यदि (आदर्शः[1] == string[0] && patmatch(आदर्शः+2, string+1))
			प्रत्यागम सत्य;
	}

	यदि (*आदर्शः == '?') {
		यदि (*string == 0)
			प्रत्यागम असत्य;
		प्रत्यागम patmatch(आदर्शः+1, string+1);
	}

	यदि (*आदर्शः == '*') {
		यावत् (*string) {
			यदि (patmatch(आदर्शः+1, string++))
				प्रत्यागम सत्य;
		}
		प्रत्यागम आदर्शः[1] == 0;
	}

	यदि (*आदर्शः == '[') {
		तार्किक प्राप्तम्_मेलनम् = असत्य;
		तार्किक inverted_list = आदर्शः[1] == '!';
		ध्रुव वर्ण *p = आदर्शः + (inverted_list ? 1 : 0);

		यावत् (*++p) {
			यदि (*p == ']') {
				यदि (प्राप्तम्_मेलनम् != inverted_list && patmatch(p+1, string+1))
					प्रत्यागम सत्य;
				भङ्ग;
			}

			यदि (*p == '\\') {
				यदि (*++p == *string)
					प्राप्तम्_मेलनम् = सत्य;
			} अन्यथा
			यदि (*p == *string)
				प्राप्तम्_मेलनम् = सत्य;
		}
	}

	यदि (*आदर्शः == *string)
		प्रत्यागम patmatch(आदर्शः+1, string+1);

	प्रत्यागम असत्य;
}

std::string get_आधारः_tmpdir()
{
	स्थिर std::string tmpdir;

	यदि (!tmpdir.रिक्तानि()) {
		प्रत्यागम tmpdir;
	}

#यदि defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	यदि (!Get_अस्थायी_मार्गः_W(wtmppath.आकार(), wtmppath.दत्त()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.find(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, wtmppath.दत्त(), wtmppath.आकार(), रिक्त, 0, रिक्त, रिक्त));
	यदि (Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, wtmppath.दत्त(), wtmppath.आकार(), utmppath.दत्त(), utmppath.आकार(), रिक्त, रिक्त) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#अन्यथा
	वर्ण * var = std::getenv("TMPDIR");
	यदि (var && वाक्यदैर्घ्यम्(var)!=0) {
		tmpdir.नियोजय(var);
		// We return the directory name without the trailing '/'
		यावत् (!tmpdir.रिक्तानि() && (tmpdir.पृष्ठभागः() == '/')) {
			tmpdir.निष्कासय_पृष्ठभागः();
		}
	} अन्यथा {
		tmpdir.नियोजय("/tmp");
	}
#यदि_अन्तः
	प्रत्यागम tmpdir;
}

std::string make_अस्थायी_file(std::string template_str)
{
	आकार_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#यदि defined(__wasm)
	स्थिर आकार_t सूचकांक = 0;
	template_str.replace(pos, 6, stringf("%06zu", सूचकांक++));
#नोचेद्यदि defined(_WIN32)
#यदि_अपरिभाषितम् YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.अन्तः(), '/', '\\');
#यदि_अन्तः
	यावत् (1) {
		प्रति (पूर्णाङ्क i = 0; i < 6; i++) {
			स्थिर std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			स्थिर uint32_t x = 314159265 ^ uint32_t(time(रिक्त));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.आकार()];
		}
		यदि (_access(template_str.c_str(), 0) != 0)
			भङ्ग;
	}
#अन्यथा
	पूर्णाङ्क suffixlen = template_str.आकार() - pos - 6;

	वर्ण *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	मुक्त(p);
#यदि_अन्तः

	प्रत्यागम template_str;
}

std::string make_अस्थायी_dir(std::string template_str)
{
#यदि defined(_WIN32)
	template_str = make_अस्थायी_file(template_str);
	mkdir(template_str.c_str());
	प्रत्यागम template_str;
#नोचेद्यदि defined(__wasm)
	template_str = make_अस्थायी_file(template_str);
	mkdir(template_str.c_str(), 0777);
	प्रत्यागम template_str;
#अन्यथा
#  ifndef NDEBUG
	आकार_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	पूर्णाङ्क suffixlen = template_str.आकार() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	वर्ण *p = strdup(template_str.c_str());
	log_assert(p);
	वर्ण *res = mkdtemp(p);
	यदि (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	मुक्त(p);

	प्रत्यागम template_str;
#यदि_अन्तः
}

तार्किक check_is_directory(ध्रुव std::string& dirname)
{
#यदि defined(_WIN32)
	संरचना _stat info;
	स्वचालित dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	यदि (dirname.पृष्ठभागः() == '/' || dirname.पृष्ठभागः() == '\\') {
		dirname_ = dirname.substr(0, dirname.दीर्घता_2() - 1);
	}

	यदि (_stat(dirname_.c_str(), &info) != 0)
	{
		प्रत्यागम असत्य;
	}
	प्रत्यागम (info.st_बहुलकम् & _S_IFDIR) != 0;
#अन्यथा
	संरचना stat info;
	यदि (stat(dirname.c_str(), &info) != 0)
	{
		प्रत्यागम असत्य;
	}
	प्रत्यागम (info.st_बहुलकम् & S_IFDIR) != 0;
#यदि_अन्तः
}

#यदि_परिभाषितम् _WIN32
तार्किक check_accessible(ध्रुव std::string& filename, तार्किक)
{
	प्रत्यागम _access(filename.c_str(), 0) == 0;
}
#अन्यथा
तार्किक check_accessible(ध्रुव std::string& filename, तार्किक is_exec)
{
	प्रत्यागम access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#यदि_अन्तः

तार्किक check_file_exists(ध्रुव std::string& filename, तार्किक is_exec)
{
	प्रत्यागम check_accessible(filename, is_exec) && !check_is_directory(filename);
}
तार्किक check_directory_exists(ध्रुव std::string& filename, तार्किक is_exec)
{
	प्रत्यागम check_accessible(filename, is_exec) && check_is_directory(filename);
}

तार्किक is_absolute_मार्गः(std::string filename)
{
#यदि_परिभाषितम् _WIN32
	प्रत्यागम filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#अन्यथा
	प्रत्यागम filename[0] == '/';
#यदि_अन्तः
}

शून्य remove_directory(std::string dirname)
{
#यदि_परिभाषितम् _WIN32
	सततम्_command(stringf("rmdir /s /q \"%s\"", dirname));
#अन्यथा
	संरचना stat stbuf;
	संरचना dirent **namelist;
	पूर्णाङ्क n = scandir(dirname.c_str(), &namelist, शून्यसूचकः, alphasort);
	log_assert(n >= 0);
	प्रति (पूर्णाङ्क i = 0; i < n; i++) {
		यदि (वाक्यं_तुलय(namelist[i]->d_name, ".") && वाक्यं_तुलय(namelist[i]->d_name, "..")) {
			std::string बफर = stringf("%s/%s", dirname, namelist[i]->d_name);
			यदि (!stat(बफर.c_str(), &stbuf) && S_ISREG(stbuf.st_बहुलकम्)) {
				remove(बफर.c_str());
			} अन्यथा
				remove_directory(बफर);
		}
		मुक्त(namelist[i]);
	}
	मुक्त(namelist);
	rmdir(dirname.c_str());
#यदि_अन्तः
}

तार्किक create_directory(ध्रुव std::string& dirname)
{
#यदि defined(_WIN32)
	पूर्णाङ्क ret = _mkdir(dirname.c_str());
#अन्यथा
	बहुलकम्_t बहुलकम् = 0755;
	पूर्णाङ्क ret = mkdir(dirname.c_str(), बहुलकम्);
#यदि_अन्तः
	यदि (ret == 0)
		प्रत्यागम सत्य;

	विकल्प (errno)
	{
	पक्ष ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::आकार_type pos = dirname.find_चरमः_सम्बन्धि('/');
			यदि (pos == std::string::npos)
#यदि defined(_WIN32)
				pos = dirname.find_चरमः_सम्बन्धि('\\');
			यदि (pos == std::string::npos)
#यदि_अन्तः
				प्रत्यागम असत्य;
			यदि (!create_directory( dirname.substr(0, pos) ))
				प्रत्यागम असत्य;
		}
		// now, try to create again
#यदि defined(_WIN32)
		प्रत्यागम 0 == _mkdir(dirname.c_str());
#अन्यथा
		प्रत्यागम 0 == mkdir(dirname.c_str(), बहुलकम्);
#यदि_अन्तः

	पक्ष EEXIST:
		// done!
		प्रत्यागम check_directory_exists(dirname);

	स्वभाव:
		प्रत्यागम असत्य;
	}
}

तार्किक make_वैयक्तिकम्_directory(ध्रुव std::string& dirname)
{
#यदि defined(_WIN32) || defined(__wasi__)
	यदि (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
#अन्यथा
	यदि (mkdir(dirname.c_str(), 0700) == 0)
		प्रत्यागम सत्य;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	पूर्णाङ्क err = errno;
	std::string parent = std::filesystem::मार्गः_2(dirname).parent_मार्गः().string();

	यदि (err == ENOENT && !parent.रिक्तानि() && create_directory(parent)) {
		यदि (mkdir(dirname.c_str(), 0700) == 0)
			प्रत्यागम सत्य;
		err = errno;
	}

	यदि (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		प्रत्यागम असत्य;
	}

	// Do not use a leftover that isn't ours
	संरचना stat st;
	यदि (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_बहुलकम्) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		प्रत्यागम असत्य;
	}
	// Repair the mode if something changed it
	बहुलकम्_t बहुलकम् = st.st_बहुलकम् & 0777;
	यदि (((बहुलकम् & S_IRWXU) != S_IRWXU || (बहुलकम् & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		प्रत्यागम असत्य;
	}

	प्रत्यागम सत्य;
#यदि_अन्तः
}

std::string make_user_tmpdir(ध्रुव std::string& name)
{
	log_assert(!name.रिक्तानि() && name.find('/') == std::string::npos);
#यदि defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_आधारः_tmpdir() + "/" + name;
#अन्यथा
	std::string dirname = stringf("%s/%s-%u", get_आधारः_tmpdir().c_str(), name.c_str(), (अचिह्नित)geteuid());
#यदि_अन्तः
	प्रत्यागम make_वैयक्तिकम्_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ध्रुव std::string& filename)
{
	std::string बहिर्गमनम्;
	बहिर्गमनम्.reserve(filename.आकार());
	प्रति (स्वचालित c : filename)
	{
		यदि (c == ' ')
			बहिर्गमनम् += "\\ ";
		अन्यथा
			बहिर्गमनम्.प्रक्षिप_पृष्ठभागः(c);
	}
	प्रत्यागम बहिर्गमनम्;
}

शून्य append_globbed(std::सदिशम्<std::string>& paths, std::string आदर्शः)
{
	rewrite_filename(आदर्शः);
	std::सदिशम्<std::string> globbed = glob_filename(आदर्शः);
	copy(globbed.begin(), globbed.अन्तः(), पृष्ठभागः_inserter(paths));
}

std::string name_from_file_मार्गः(std::string मार्गः_2) {
	प्रत्यागम std::filesystem::मार्गः_2(मार्गः_2).filename().string();
}

std::string absolute_मार्गः(ध्रुव std::string& मार्गः_2)
{
	std::error_संहिता ec;
	std::filesystem::मार्गः_2 परिणाम = std::filesystem::absolute(मार्गः_2, ec);

	यदि (ec)
		प्रत्यागम "";

	परिणाम = std::filesystem::weakly_canonical(परिणाम, ec);

	यदि (ec)
		प्रत्यागम "";

	प्रत्यागम परिणाम.string();
}

// Includes OS_PATH_SEP at the end if present
std::string parent_from_file_मार्गः(std::string मार्गः_2) {
	स्वचालित parent = std::filesystem::मार्गः_2(मार्गः_2).parent_मार्गः();
	यदि (parent.रिक्तानि()) {
		प्रत्यागम "";
	}
	// Add trailing separator to match original behavior
	std::string परिणाम = parent.string();
	यदि (!परिणाम.रिक्तानि() && परिणाम.पृष्ठभागः() != std::filesystem::मार्गः_2::preferred_separator) {
		परिणाम += std::filesystem::मार्गः_2::preferred_separator;
	}
	प्रत्यागम परिणाम;
}

शून्य format_emit_unescaped(std::string &परिणाम, std::string_view fmt)
{
	परिणाम.reserve(परिणाम.आकार() + fmt.आकार());
	प्रति (आकार_t i = 0; i < fmt.आकार(); ++i) {
		वर्ण ch = fmt[i];
		परिणाम.प्रक्षिप_पृष्ठभागः(ch);
		यदि (ch == '%' && i + 1 < fmt.आकार() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string परिणाम;
	format_emit_unescaped(परिणाम, fmt);
	प्रत्यागम परिणाम;
}

स्थिर std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string परिणाम = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क spec_parameter_आकार(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	विकल्प (spec[spec.आकार() - 1]) {
	पक्ष 'd':
	पक्ष 'i':
	पक्ष 'o':
	पक्ष 'u':
	पक्ष 'x':
	पक्ष 'X':
		विकल्प (spec[spec.आकार() - 2]) {
		पक्ष 'h':
			यदि (spec[spec.आकार() - 3] == 'h')
				प्रत्यागम परिमाण(वर्ण);
			प्रत्यागम परिमाण(ह्रस्व);
		पक्ष 'l':
			यदि (spec[spec.आकार() - 3] == 'l')
				प्रत्यागम परिमाण(दीर्घ दीर्घ);
			प्रत्यागम परिमाण(दीर्घ);
		पक्ष 'L':
		पक्ष 'q':
			प्रत्यागम परिमाण(दीर्घ दीर्घ);
		पक्ष 'j':
			प्रत्यागम परिमाण(intmax_t);
		पक्ष 'z':
		पक्ष 'Z':
			प्रत्यागम परिमाण(आकार_t);
		पक्ष 't':
			प्रत्यागम परिमाण(ptrdiff_t);
		}
		प्रत्यागम परिमाण(पूर्णाङ्क);
	पक्ष 'e':
	पक्ष 'E':
	पक्ष 'f':
	पक्ष 'F':
	पक्ष 'g':
	पक्ष 'G':
	पक्ष 'a':
	पक्ष 'A':
		यदि (spec[spec.आकार() - 2] == 'L')
			प्रत्यागम परिमाण(दीर्घ द्विगुण);
		यदि (spec[spec.आकार() - 2] == 'l' && spec[spec.आकार() - 3] == 'l')
			प्रत्यागम परिमाण(दीर्घ द्विगुण);
		प्रत्यागम परिमाण(द्विगुण);
	पक्ष 'c':
		यदि (spec[spec.आकार() - 2] == 'l') {
			प्रत्यागम परिमाण(wchar_t);
		}
		प्रत्यागम परिमाण(अचिह्नित वर्ण);
	पक्ष 'C':
		प्रत्यागम परिमाण(wchar_t);
	पक्ष 's':
	पक्ष 'p':
	पक्ष 'S':
	पक्ष 'n':
		प्रत्यागम परिमाण(शून्य *);
	पक्ष 'm':
		प्रत्यागम परिमाण(पूर्णाङ्क);
	स्वभाव:
		प्रत्यागम -1;
	}
}

प्रतिरूपम् <प्रकारनाम Arg>
स्थिर शून्य format_emit_stringf(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	विकल्प (अङ्कः_dynamic_ints) {
	पक्ष Dynamic_Int_गणनम्::NONE:
		परिणाम += string_view_stringf(spec, arg);
		प्रत्यागम;
	पक्ष Dynamic_Int_गणनम्::ONE:
		परिणाम += string_view_stringf(spec, dynamic_ints[0], arg);
		प्रत्यागम;
	पक्ष Dynamic_Int_गणनम्::TWO:
		परिणाम += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		प्रत्यागम;
	}
	YOSYS_ABORT("Internal error");
}

शून्य format_emit_long_long(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, दीर्घ दीर्घ arg)
{
	यदि (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += std::गन्तव्यम्_string(static_cast<पूर्णाङ्क>(arg));
		प्रत्यागम;
	}
	यदि (spec_parameter_आकार(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, (पूर्णाङ्क)arg);
	} अन्यथा {
		format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
	}
}

शून्य format_emit_unsigned_long_long(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, अचिह्नित दीर्घ दीर्घ arg)
{
	यदि (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += std::गन्तव्यम्_string(static_cast<अचिह्नित पूर्णाङ्क>(arg));
		प्रत्यागम;
	}
	यदि (spec == "%c") {
		परिणाम += static_cast<वर्ण>(arg);
		प्रत्यागम;
	}
	यदि (spec_parameter_आकार(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, (अचिह्नित पूर्णाङ्क)arg);
	} अन्यथा {
		format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
	}
}

शून्य format_emit_double(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, द्विगुण arg)
{
	format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

शून्य format_emit_char_ptr(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव वर्ण *arg)
{
	यदि (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += arg;
		प्रत्यागम;
	}
	format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

शून्य format_emit_string(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव std::string &arg)
{
	यदि (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		परिणाम += arg;
		प्रत्यागम;
	}
	format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, arg.c_str());
}

शून्य format_emit_string_view(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, std::string_view arg)
{
	यदि (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		परिणाम += arg;
		प्रत्यागम;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, std::string(arg).c_str());
}

शून्य format_emit_void_ptr(std::string &परिणाम, std::string_view spec, पूर्णाङ्क *dynamic_ints,
	Dynamic_Int_गणनम् अङ्कः_dynamic_ints, ध्रुव शून्य *arg)
{
	format_emit_stringf(परिणाम, spec, dynamic_ints, अङ्कः_dynamic_ints, arg);
}

#यदि defined(_WIN32)
Scoped_File_तालकम्::Scoped_File_तालकम्(ध्रुव std::string &मार्गः_2)
{
	HANDLE h = CreateFileA(मार्गः_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			रिक्त, OPEN_ALWAYS, FILE_ATTRIBUTE_प्रसामान्यम्, रिक्त);
	यदि (h == INVALID_HANDLE_मानम्) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", मार्गः_2.c_str());
		प्रत्यागम;
	}
	OVERLAPPED ov = {};
	यदि (!तालकम्_File_Ex(h, LOCKFILE_EXCLUSIVE_तालकम्, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", मार्गः_2.c_str());
		CloseHandle(h);
		प्रत्यागम;
	}
	handle = h;
}

Scoped_File_तालकम्::~Scoped_File_तालकम्()
{
	यदि (handle != शून्यसूचकः) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#नोचेद्यदि !defined(__wasi__)
Scoped_File_तालकम्::Scoped_File_तालकम्(ध्रुव std::string &मार्गः_2)
{
	fd = उद्घाटय(मार्गः_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	यदि (fd < 0)
		fd = उद्घाटय(मार्गः_2.c_str(), O_RDONLY | O_CLOEXEC);
	यदि (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", मार्गः_2.c_str());
		प्रत्यागम;
	}
	पूर्णाङ्क ret;
	यावत् ((ret = flock(fd, तालकम्_EX)) != 0 && errno == EINTR);
	यदि (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", मार्गः_2.c_str());
}

Scoped_File_तालकम्::~Scoped_File_तालकम्()
{
	यदि (fd >= 0)
		close(fd); // releases the flock
}
#अन्यथा
Scoped_File_तालकम्::Scoped_File_तालकम्(ध्रुव std::string &) {}
Scoped_File_तालकम्::~Scoped_File_तालकम्() {}
#यदि_अन्तः

अचिह्नित get_process_id()
{
#यदि defined(_WIN32)
	प्रत्यागम GetCurrentProcessId();
#नोचेद्यदि defined(__wasi__)
	प्रत्यागम 0;
#अन्यथा
	प्रत्यागम getpid();
#यदि_अन्तः
}

YOSYS_NAMESPACE_अन्तः
