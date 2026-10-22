#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <filesystem>
#ಅನ್ತರ್ಭಾವ <system_error>

#ಆದರೆ !defined(WIN32)
#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <io.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(_WIN32) && !defined(__wasi__)
#ಅನ್ತರ್ಭಾವ <sys/file.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <cerrno>
#ಆದರೆ_ಅಂತ್ಯ

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ಪೂರ್ಣಾಂಕ readsome(std::istream &f, ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ n)
{
	ಪೂರ್ಣಾಂಕ rc = ಪೂರ್ಣಾಂಕ(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ಆದರೆ (rc == 0) {
		ಪೂರ್ಣಾಂಕ c = f.get();
		ಆದರೆ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}

std::string ಮುಂದಿನ_token(std::string &ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep, ತಾರ್ಕಿಕ long_ಅಕ್ಷರಸರಣಿಗಳು)
{
	ಗಾತ್ರ_t pos_begin = ಪಠ್ಯ.ಕಂಡುಹಿಡಿ_ಮೊದಲ_not_ದ(sep);

	ಆದರೆ (pos_begin == std::string::npos)
		pos_begin = ಪಠ್ಯ.ಗಾತ್ರ_2();

	ಆದರೆ (long_ಅಕ್ಷರಸರಣಿಗಳು && pos_begin != ಪಠ್ಯ.ಗಾತ್ರ_2() && ಪಠ್ಯ[pos_begin] == '"') {
		std::string sep_string = sep;
		ಗಾಗಿ (ಗಾತ್ರ_t i = pos_begin+1; i < ಪಠ್ಯ.ಗಾತ್ರ_2(); i++) {
			ಆದರೆ (ಪಠ್ಯ[i] == '"' && (i+1 == ಪಠ್ಯ.ಗಾತ್ರ_2() || sep_string.ಕಂಡುಹಿಡಿ(ಪಠ್ಯ[i+1]) != std::string::npos)) {
				std::string token = ಪಠ್ಯ.substr(pos_begin, i-pos_begin+1);
				ಪಠ್ಯ = ಪಠ್ಯ.substr(i+1);
				ಹಿಂತಿರುಗಿ token;
			}
			ಆದರೆ (i+1 < ಪಠ್ಯ.ಗಾತ್ರ_2() && ಪಠ್ಯ[i] == '"' && ಪಠ್ಯ[i+1] == ';' && (i+2 == ಪಠ್ಯ.ಗಾತ್ರ_2() || sep_string.ಕಂಡುಹಿಡಿ(ಪಠ್ಯ[i+2]) != std::string::npos)) {
				std::string token = ಪಠ್ಯ.substr(pos_begin, i-pos_begin+1);
				ಪಠ್ಯ = ಪಠ್ಯ.substr(i+2);
				ಹಿಂತಿರುಗಿ token + ";";
			}
		}
	}

	ಗಾತ್ರ_t pos_ಕೊನೆ = ಪಠ್ಯ.ಕಂಡುಹಿಡಿ_ಮೊದಲ_ದ(sep, pos_begin);

	ಆದರೆ (pos_ಕೊನೆ == std::string::npos)
		pos_ಕೊನೆ = ಪಠ್ಯ.ಗಾತ್ರ_2();

	std::string token = ಪಠ್ಯ.substr(pos_begin, pos_ಕೊನೆ-pos_begin);
	ಪಠ್ಯ = ಪಠ್ಯ.substr(pos_ಕೊನೆ);
	ಹಿಂತಿರುಗಿ token;
}

std::ಸದಿಶ<std::string> ವಿಭಜಿಸು_ಟೋಕನುಗಳು(ಸ್ಥಿರಾಂಕ std::string &ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep)
{
	std::ಸದಿಶ<std::string> ಟೋಕನುಗಳು;
	std::string current_token;
	ಗಾಗಿ (ಅಕ್ಷರ c : ಪಠ್ಯ) {
		ಆದರೆ (strchr(sep, c)) {
			ಆದರೆ (!current_token.ಖಾಲಿಗಳು()) {
				ಟೋಕನುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(current_token);
				current_token.clear();
			}
		} ಇಲ್ಲದಿದ್ದರೆ
			current_token += c;
	}
	ಆದರೆ (!current_token.ಖಾಲಿಗಳು()) {
		ಟೋಕನುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(current_token);
		current_token.clear();
	}
	ಹಿಂತಿರುಗಿ ಟೋಕನುಗಳು;
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
ತಾರ್ಕಿಕ patmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
	ಆದರೆ (*ನಮೂನೆ == 0)
		ಹಿಂತಿರುಗಿ *string == 0;

	ಆದರೆ (*ನಮೂನೆ == '\\') {
		ಆದರೆ (ನಮೂನೆ[1] == string[0] && patmatch(ನಮೂನೆ+2, string+1))
			ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (*ನಮೂನೆ == '?') {
		ಆದರೆ (*string == 0)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ patmatch(ನಮೂನೆ+1, string+1);
	}

	ಆದರೆ (*ನಮೂನೆ == '*') {
		ಆಗ (*string) {
			ಆದರೆ (patmatch(ನಮೂನೆ+1, string++))
				ಹಿಂತಿರುಗಿ ನಿಜ;
		}
		ಹಿಂತಿರುಗಿ ನಮೂನೆ[1] == 0;
	}

	ಆದರೆ (*ನಮೂನೆ == '[') {
		ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = ಸುಳ್ಳು;
		ತಾರ್ಕಿಕ inverted_list = ನಮೂನೆ[1] == '!';
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ನಮೂನೆ + (inverted_list ? 1 : 0);

		ಆಗ (*++p) {
			ಆದರೆ (*p == ']') {
				ಆದರೆ (ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ != inverted_list && patmatch(p+1, string+1))
					ಹಿಂತಿರುಗಿ ನಿಜ;
				ನಿಲ್ಲಿಸು;
			}

			ಆದರೆ (*p == '\\') {
				ಆದರೆ (*++p == *string)
					ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = ನಿಜ;
			} ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (*p == *string)
				ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = ನಿಜ;
		}
	}

	ಆದರೆ (*ನಮೂನೆ == *string)
		ಹಿಂತಿರುಗಿ patmatch(ನಮೂನೆ+1, string+1);

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

std::string get_ತಳ_tmpdir()
{
	ಸ್ಥಿರ std::string tmpdir;

	ಆದರೆ (!tmpdir.ಖಾಲಿಗಳು()) {
		ಹಿಂತಿರುಗಿ tmpdir;
	}

#ಆದರೆ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ಆದರೆ (!Get_ತಾತ್ಕಾಲಿಕ_ಮಾರ್ಗ_W(wtmppath.ಗಾತ್ರ_2(), wtmppath.ದತ್ತಾಂಶ()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.ಕಂಡುಹಿಡಿ(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, wtmppath.ದತ್ತಾಂಶ(), wtmppath.ಗಾತ್ರ_2(), ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ));
	ಆದರೆ (Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, wtmppath.ದತ್ತಾಂಶ(), wtmppath.ಗಾತ್ರ_2(), utmppath.ದತ್ತಾಂಶ(), utmppath.ಗಾತ್ರ_2(), ಖಾಲಿ, ಖಾಲಿ) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ಇಲ್ಲದಿದ್ದರೆ
	ಅಕ್ಷರ * var = std::getenv("TMPDIR");
	ಆದರೆ (var && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(var)!=0) {
		tmpdir.ನಿಯೋಜಿಸು(var);
		// We return the directory name without the trailing '/'
		ಆಗ (!tmpdir.ಖಾಲಿಗಳು() && (tmpdir.ಹಿಂಭಾಗ() == '/')) {
			tmpdir.ತೆಗೆ_ಹಿಂಭಾಗ();
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		tmpdir.ನಿಯೋಜಿಸು("/tmp");
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ tmpdir;
}

std::string make_ತಾತ್ಕಾಲಿಕ_file(std::string template_str)
{
	ಗಾತ್ರ_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ಆದರೆ defined(__wasm)
	ಸ್ಥಿರ ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = 0;
	template_str.replace(pos, 6, stringf("%06zu", ಸೂಚ್ಯಂಕ++));
#ಅಥವಾಆದರೆ defined(_WIN32)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.ಕೊನೆ(), '/', '\\');
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (1) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 6; i++) {
			ಸ್ಥಿರ std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ಸ್ಥಿರ uint32_t x = 314159265 ^ uint32_t(ಸಮಯ_2(ಖಾಲಿ));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.ಗಾತ್ರ_2()];
		}
		ಆದರೆ (_access(template_str.c_str(), 0) != 0)
			ನಿಲ್ಲಿಸು;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಪೂರ್ಣಾಂಕ suffixlen = template_str.ಗಾತ್ರ_2() - pos - 6;

	ಅಕ್ಷರ *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	ಬಿಡುಗಡೆ(p);
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ template_str;
}

std::string make_ತಾತ್ಕಾಲಿಕ_dir(std::string template_str)
{
#ಆದರೆ defined(_WIN32)
	template_str = make_ತಾತ್ಕಾಲಿಕ_file(template_str);
	mkdir(template_str.c_str());
	ಹಿಂತಿರುಗಿ template_str;
#ಅಥವಾಆದರೆ defined(__wasm)
	template_str = make_ತಾತ್ಕಾಲಿಕ_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ಹಿಂತಿರುಗಿ template_str;
#ಇಲ್ಲದಿದ್ದರೆ
#  ifndef NDEBUG
	ಗಾತ್ರ_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ಪೂರ್ಣಾಂಕ suffixlen = template_str.ಗಾತ್ರ_2() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	ಅಕ್ಷರ *p = strdup(template_str.c_str());
	log_assert(p);
	ಅಕ್ಷರ *res = mkdtemp(p);
	ಆದರೆ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	ಬಿಡುಗಡೆ(p);

	ಹಿಂತಿರುಗಿ template_str;
#ಆದರೆ_ಅಂತ್ಯ
}

ತಾರ್ಕಿಕ check_is_directory(ಸ್ಥಿರಾಂಕ std::string& dirname)
{
#ಆದರೆ defined(_WIN32)
	ರಚನೆ _stat info;
	ಸ್ವಯಂಚಾಲಿತ dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ಆದರೆ (dirname.ಹಿಂಭಾಗ() == '/' || dirname.ಹಿಂಭಾಗ() == '\\') {
		dirname_ = dirname.substr(0, dirname.ಉದ್ದಳತೆ() - 1);
	}

	ಆದರೆ (_stat(dirname_.c_str(), &info) != 0)
	{
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ (info.st_ಬಹುಲಕ & _S_IFDIR) != 0;
#ಇಲ್ಲದಿದ್ದರೆ
	ರಚನೆ stat info;
	ಆದರೆ (stat(dirname.c_str(), &info) != 0)
	{
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ (info.st_ಬಹುಲಕ & S_IFDIR) != 0;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
ತಾರ್ಕಿಕ check_accessible(ಸ್ಥಿರಾಂಕ std::string& filename, ತಾರ್ಕಿಕ)
{
	ಹಿಂತಿರುಗಿ _access(filename.c_str(), 0) == 0;
}
#ಇಲ್ಲದಿದ್ದರೆ
ತಾರ್ಕಿಕ check_accessible(ಸ್ಥಿರಾಂಕ std::string& filename, ತಾರ್ಕಿಕ is_exec)
{
	ಹಿಂತಿರುಗಿ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ check_file_exists(ಸ್ಥಿರಾಂಕ std::string& filename, ತಾರ್ಕಿಕ is_exec)
{
	ಹಿಂತಿರುಗಿ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
ತಾರ್ಕಿಕ check_directory_exists(ಸ್ಥಿರಾಂಕ std::string& filename, ತಾರ್ಕಿಕ is_exec)
{
	ಹಿಂತಿರುಗಿ check_accessible(filename, is_exec) && check_is_directory(filename);
}

ತಾರ್ಕಿಕ is_absolute_ಮಾರ್ಗ(std::string filename)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
	ಹಿಂತಿರುಗಿ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ filename[0] == '/';
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ remove_directory(std::string dirname)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
	ಸತತ_command(stringf("rmdir /s /q \"%s\"", dirname));
#ಇಲ್ಲದಿದ್ದರೆ
	ರಚನೆ stat stbuf;
	ರಚನೆ dirent **namelist;
	ಪೂರ್ಣಾಂಕ n = scandir(dirname.c_str(), &namelist, ಶೂನ್ಯಸೂಚಕ, alphasort);
	log_assert(n >= 0);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n; i++) {
		ಆದರೆ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string ಬಫರ್ = stringf("%s/%s", dirname, namelist[i]->d_name);
			ಆದರೆ (!stat(ಬಫರ್.c_str(), &stbuf) && S_ISREG(stbuf.st_ಬಹುಲಕ)) {
				remove(ಬಫರ್.c_str());
			} ಇಲ್ಲದಿದ್ದರೆ
				remove_directory(ಬಫರ್);
		}
		ಬಿಡುಗಡೆ(namelist[i]);
	}
	ಬಿಡುಗಡೆ(namelist);
	rmdir(dirname.c_str());
#ಆದರೆ_ಅಂತ್ಯ
}

ತಾರ್ಕಿಕ create_directory(ಸ್ಥಿರಾಂಕ std::string& dirname)
{
#ಆದರೆ defined(_WIN32)
	ಪೂರ್ಣಾಂಕ ret = _mkdir(dirname.c_str());
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಹುಲಕ_t ಬಹುಲಕ = 0755;
	ಪೂರ್ಣಾಂಕ ret = mkdir(dirname.c_str(), ಬಹುಲಕ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (ret == 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;

	ಆಯ್ಕೆ (errno)
	{
	ಸ್ಥಿತಿ ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::ಗಾತ್ರ_type pos = dirname.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ('/');
			ಆದರೆ (pos == std::string::npos)
#ಆದರೆ defined(_WIN32)
				pos = dirname.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ('\\');
			ಆದರೆ (pos == std::string::npos)
#ಆದರೆ_ಅಂತ್ಯ
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (!create_directory( dirname.substr(0, pos) ))
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}
		// now, try to create again
#ಆದರೆ defined(_WIN32)
		ಹಿಂತಿರುಗಿ 0 == _mkdir(dirname.c_str());
#ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ 0 == mkdir(dirname.c_str(), ಬಹುಲಕ);
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ EEXIST:
		// done!
		ಹಿಂತಿರುಗಿ check_directory_exists(dirname);

	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
}

ತಾರ್ಕಿಕ make_ಖಾಸಗಿ_directory(ಸ್ಥಿರಾಂಕ std::string& dirname)
{
#ಆದರೆ defined(_WIN32) || defined(__wasi__)
	ಆದರೆ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (mkdir(dirname.c_str(), 0700) == 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ಪೂರ್ಣಾಂಕ err = errno;
	std::string ಪೋಷಕ = std::filesystem::ಮಾರ್ಗ_2(dirname).ಪೋಷಕ_ಮಾರ್ಗ().string();

	ಆದರೆ (err == ENOENT && !ಪೋಷಕ.ಖಾಲಿಗಳು() && create_directory(ಪೋಷಕ)) {
		ಆದರೆ (mkdir(dirname.c_str(), 0700) == 0)
			ಹಿಂತಿರುಗಿ ನಿಜ;
		err = errno;
	}

	ಆದರೆ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// Do not use a leftover that isn't ours
	ರಚನೆ stat st;
	ಆದರೆ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_ಬಹುಲಕ) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	// Repair the mode if something changed it
	ಬಹುಲಕ_t ಬಹುಲಕ = st.st_ಬಹುಲಕ & 0777;
	ಆದರೆ (((ಬಹುಲಕ & S_IRWXU) != S_IRWXU || (ಬಹುಲಕ & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
#ಆದರೆ_ಅಂತ್ಯ
}

std::string make_user_tmpdir(ಸ್ಥಿರಾಂಕ std::string& name)
{
	log_assert(!name.ಖಾಲಿಗಳು() && name.ಕಂಡುಹಿಡಿ('/') == std::string::npos);
#ಆದರೆ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_ತಳ_tmpdir() + "/" + name;
#ಇಲ್ಲದಿದ್ದರೆ
	std::string dirname = stringf("%s/%s-%u", get_ತಳ_tmpdir().c_str(), name.c_str(), (ಚಿಹ್ನೆರಹಿತ)geteuid());
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ make_ಖಾಸಗಿ_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ಸ್ಥಿರಾಂಕ std::string& filename)
{
	std::string ಹೊರಹೋಗು;
	ಹೊರಹೋಗು.reserve(filename.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c : filename)
	{
		ಆದರೆ (c == ' ')
			ಹೊರಹೋಗು += "\\ ";
		ಇಲ್ಲದಿದ್ದರೆ
			ಹೊರಹೋಗು.ತಳ್ಳು_ಹಿಂಭಾಗ(c);
	}
	ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು;
}

ಶೂನ್ಯ append_globbed(std::ಸದಿಶ<std::string>& paths, std::string ನಮೂನೆ)
{
	rewrite_filename(ನಮೂನೆ);
	std::ಸದಿಶ<std::string> globbed = glob_filename(ನಮೂನೆ);
	copy(globbed.begin(), globbed.ಕೊನೆ(), ಹಿಂಭಾಗ_inserter(paths));
}

std::string name_from_file_ಮಾರ್ಗ(std::string ಮಾರ್ಗ_2) {
	ಹಿಂತಿರುಗಿ std::filesystem::ಮಾರ್ಗ_2(ಮಾರ್ಗ_2).filename().string();
}

std::string absolute_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ std::string& ಮಾರ್ಗ_2)
{
	std::error_ಸಂಕೇತ ec;
	std::filesystem::ಮಾರ್ಗ_2 ಫಲಿತಾಂಶ = std::filesystem::absolute(ಮಾರ್ಗ_2, ec);

	ಆದರೆ (ec)
		ಹಿಂತಿರುಗಿ "";

	ಫಲಿತಾಂಶ = std::filesystem::weakly_canonical(ಫಲಿತಾಂಶ, ec);

	ಆದರೆ (ec)
		ಹಿಂತಿರುಗಿ "";

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.string();
}

// Includes OS_PATH_SEP at the end if present
std::string ಪೋಷಕ_from_file_ಮಾರ್ಗ(std::string ಮಾರ್ಗ_2) {
	ಸ್ವಯಂಚಾಲಿತ ಪೋಷಕ = std::filesystem::ಮಾರ್ಗ_2(ಮಾರ್ಗ_2).ಪೋಷಕ_ಮಾರ್ಗ();
	ಆದರೆ (ಪೋಷಕ.ಖಾಲಿಗಳು()) {
		ಹಿಂತಿರುಗಿ "";
	}
	// Add trailing separator to match original behavior
	std::string ಫಲಿತಾಂಶ = ಪೋಷಕ.string();
	ಆದರೆ (!ಫಲಿತಾಂಶ.ಖಾಲಿಗಳು() && ಫಲಿತಾಂಶ.ಹಿಂಭಾಗ() != std::filesystem::ಮಾರ್ಗ_2::preferred_separator) {
		ಫಲಿತಾಂಶ += std::filesystem::ಮಾರ್ಗ_2::preferred_separator;
	}
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ format_emit_unescaped(std::string &ಫಲಿತಾಂಶ, std::string_view fmt)
{
	ಫಲಿತಾಂಶ.reserve(ಫಲಿತಾಂಶ.ಗಾತ್ರ_2() + fmt.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < fmt.ಗಾತ್ರ_2(); ++i) {
		ಅಕ್ಷರ ch = fmt[i];
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ch);
		ಆದರೆ (ch == '%' && i + 1 < fmt.ಗಾತ್ರ_2() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string ಫಲಿತಾಂಶ;
	format_emit_unescaped(ಫಲಿತಾಂಶ, fmt);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string ಫಲಿತಾಂಶ = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spec_parameter_ಗಾತ್ರ(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	ಆಯ್ಕೆ (spec[spec.ಗಾತ್ರ_2() - 1]) {
	ಸ್ಥಿತಿ 'd':
	ಸ್ಥಿತಿ 'i':
	ಸ್ಥಿತಿ 'o':
	ಸ್ಥಿತಿ 'u':
	ಸ್ಥಿತಿ 'x':
	ಸ್ಥಿತಿ 'X':
		ಆಯ್ಕೆ (spec[spec.ಗಾತ್ರ_2() - 2]) {
		ಸ್ಥಿತಿ 'h':
			ಆದರೆ (spec[spec.ಗಾತ್ರ_2() - 3] == 'h')
				ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಅಕ್ಷರ);
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಗಿಡ್ಡ);
		ಸ್ಥಿತಿ 'l':
			ಆದರೆ (spec[spec.ಗಾತ್ರ_2() - 3] == 'l')
				ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ದೀರ್ಘ ದೀರ್ಘ);
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ದೀರ್ಘ);
		ಸ್ಥಿತಿ 'L':
		ಸ್ಥಿತಿ 'q':
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ದೀರ್ಘ ದೀರ್ಘ);
		ಸ್ಥಿತಿ 'j':
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(intmax_t);
		ಸ್ಥಿತಿ 'z':
		ಸ್ಥಿತಿ 'Z':
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಗಾತ್ರ_t);
		ಸ್ಥಿತಿ 't':
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ptrdiff_t);
		}
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
	ಸ್ಥಿತಿ 'e':
	ಸ್ಥಿತಿ 'E':
	ಸ್ಥಿತಿ 'f':
	ಸ್ಥಿತಿ 'F':
	ಸ್ಥಿತಿ 'g':
	ಸ್ಥಿತಿ 'G':
	ಸ್ಥಿತಿ 'a':
	ಸ್ಥಿತಿ 'A':
		ಆದರೆ (spec[spec.ಗಾತ್ರ_2() - 2] == 'L')
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ದೀರ್ಘ ದ್ವಿಗುಣ);
		ಆದರೆ (spec[spec.ಗಾತ್ರ_2() - 2] == 'l' && spec[spec.ಗಾತ್ರ_2() - 3] == 'l')
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ದೀರ್ಘ ದ್ವಿಗುಣ);
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ದ್ವಿಗುಣ);
	ಸ್ಥಿತಿ 'c':
		ಆದರೆ (spec[spec.ಗಾತ್ರ_2() - 2] == 'l') {
			ಹಿಂತಿರುಗಿ ಗಾತ್ರ(wchar_t);
		}
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ);
	ಸ್ಥಿತಿ 'C':
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ(wchar_t);
	ಸ್ಥಿತಿ 's':
	ಸ್ಥಿತಿ 'p':
	ಸ್ಥಿತಿ 'S':
	ಸ್ಥಿತಿ 'n':
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಶೂನ್ಯ *);
	ಸ್ಥಿತಿ 'm':
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ -1;
	}
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Arg>
ಸ್ಥಿರ ಶೂನ್ಯ format_emit_stringf(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	ಆಯ್ಕೆ (ಅಂಕೆ_dynamic_ints) {
	ಸ್ಥಿತಿ Dynamic_Int_ಎಣಿಕೆ::NONE:
		ಫಲಿತಾಂಶ += string_view_stringf(spec, arg);
		ಹಿಂತಿರುಗಿ;
	ಸ್ಥಿತಿ Dynamic_Int_ಎಣಿಕೆ::ONE:
		ಫಲಿತಾಂಶ += string_view_stringf(spec, dynamic_ints[0], arg);
		ಹಿಂತಿರುಗಿ;
	ಸ್ಥಿತಿ Dynamic_Int_ಎಣಿಕೆ::TWO:
		ಫಲಿತಾಂಶ += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ಹಿಂತಿರುಗಿ;
	}
	YOSYS_ABORT("Internal error");
}

ಶೂನ್ಯ format_emit_long_long(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ದೀರ್ಘ ದೀರ್ಘ arg)
{
	ಆದರೆ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ಫಲಿತಾಂಶ += std::ಗಮ್ಯ_string(static_cast<ಪೂರ್ಣಾಂಕ>(arg));
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (spec_parameter_ಗಾತ್ರ(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, (ಪೂರ್ಣಾಂಕ)arg);
	} ಇಲ್ಲದಿದ್ದರೆ {
		format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
	}
}

ಶೂನ್ಯ format_emit_unsigned_long_long(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ arg)
{
	ಆದರೆ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ಫಲಿತಾಂಶ += std::ಗಮ್ಯ_string(static_cast<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ>(arg));
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (spec == "%c") {
		ಫಲಿತಾಂಶ += static_cast<ಅಕ್ಷರ>(arg);
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (spec_parameter_ಗಾತ್ರ(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arg);
	} ಇಲ್ಲದಿದ್ದರೆ {
		format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
	}
}

ಶೂನ್ಯ format_emit_double(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ದ್ವಿಗುಣ arg)
{
	format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
}

ಶೂನ್ಯ format_emit_char_ptr(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *arg)
{
	ಆದರೆ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ಫಲಿತಾಂಶ += arg;
		ಹಿಂತಿರುಗಿ;
	}
	format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
}

ಶೂನ್ಯ format_emit_string(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಸ್ಥಿರಾಂಕ std::string &arg)
{
	ಆದರೆ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ಫಲಿತಾಂಶ += arg;
		ಹಿಂತಿರುಗಿ;
	}
	format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg.c_str());
}

ಶೂನ್ಯ format_emit_string_view(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, std::string_view arg)
{
	ಆದರೆ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		ಫಲಿತಾಂಶ += arg;
		ಹಿಂತಿರುಗಿ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, std::string(arg).c_str());
}

ಶೂನ್ಯ format_emit_void_ptr(std::string &ಫಲಿತಾಂಶ, std::string_view spec, ಪೂರ್ಣಾಂಕ *dynamic_ints,
	Dynamic_Int_ಎಣಿಕೆ ಅಂಕೆ_dynamic_ints, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *arg)
{
	format_emit_stringf(ಫಲಿತಾಂಶ, spec, dynamic_ints, ಅಂಕೆ_dynamic_ints, arg);
}

#ಆದರೆ defined(_WIN32)
Scoped_File_ಬೀಗ::Scoped_File_ಬೀಗ(ಸ್ಥಿರಾಂಕ std::string &ಮಾರ್ಗ_2)
{
	HANDLE h = CreateFileA(ಮಾರ್ಗ_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ಖಾಲಿ, OPEN_ALWAYS, FILE_ATTRIBUTE_ಸಾಮಾನ್ಯ, ಖಾಲಿ);
	ಆದರೆ (h == INVALID_HANDLE_ಮೌಲ್ಯ) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", ಮಾರ್ಗ_2.c_str());
		ಹಿಂತಿರುಗಿ;
	}
	OVERLAPPED ov = {};
	ಆದರೆ (!ಬೀಗ_File_Ex(h, LOCKFILE_EXCLUSIVE_ಬೀಗ, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", ಮಾರ್ಗ_2.c_str());
		CloseHandle(h);
		ಹಿಂತಿರುಗಿ;
	}
	handle = h;
}

Scoped_File_ಬೀಗ::~Scoped_File_ಬೀಗ()
{
	ಆದರೆ (handle != ಶೂನ್ಯಸೂಚಕ) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#ಅಥವಾಆದರೆ !defined(__wasi__)
Scoped_File_ಬೀಗ::Scoped_File_ಬೀಗ(ಸ್ಥಿರಾಂಕ std::string &ಮಾರ್ಗ_2)
{
	fd = open(ಮಾರ್ಗ_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ಆದರೆ (fd < 0)
		fd = open(ಮಾರ್ಗ_2.c_str(), O_RDONLY | O_CLOEXEC);
	ಆದರೆ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", ಮಾರ್ಗ_2.c_str());
		ಹಿಂತಿರುಗಿ;
	}
	ಪೂರ್ಣಾಂಕ ret;
	ಆಗ ((ret = flock(fd, ಬೀಗ_EX)) != 0 && errno == EINTR);
	ಆದರೆ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", ಮಾರ್ಗ_2.c_str());
}

Scoped_File_ಬೀಗ::~Scoped_File_ಬೀಗ()
{
	ಆದರೆ (fd >= 0)
		close(fd); // releases the flock
}
#ಇಲ್ಲದಿದ್ದರೆ
Scoped_File_ಬೀಗ::Scoped_File_ಬೀಗ(ಸ್ಥಿರಾಂಕ std::string &) {}
Scoped_File_ಬೀಗ::~Scoped_File_ಬೀಗ() {}
#ಆದರೆ_ಅಂತ್ಯ

ಚಿಹ್ನೆರಹಿತ get_process_id()
{
#ಆದರೆ defined(_WIN32)
	ಹಿಂತಿರುಗಿ GetCurrentProcessId();
#ಅಥವಾಆದರೆ defined(__wasi__)
	ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ getpid();
#ಆದರೆ_ಅಂತ್ಯ
}

YOSYS_NAMESPACE_ಕೊನೆ
