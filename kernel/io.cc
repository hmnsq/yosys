#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <filesystem>
#ਅਨ੍ਤਰ੍ਭਾਵ <system_error>

#ਜੇ !defined(WIN32)
#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>
#ਜੇ_ਅੰਤ

#ਜੇ !defined(_WIN32) && !defined(__wasi__)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/file.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <cerrno>
#ਜੇ_ਅੰਤ

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

ਪੂਰਨ readsome(std::istream &f, ਅੱਖਰ *s, ਪੂਰਨ n)
{
	ਪੂਰਨ rc = ਪੂਰਨ(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ਜੇ (rc == 0) {
		ਪੂਰਨ c = f.get();
		ਜੇ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	ਵਾਪਸ rc;
}

std::string ਅਗਲਾ_token(std::string &ਪਾਠ, ਅਚਲ ਅੱਖਰ *sep, ਤਾਰਕਿਕ long_ਲੜੀਆਂ)
{
	ਮਾਪ_t pos_begin = ਪਾਠ.ਲੱਭੋ_ਪਹਿਲਾ_not_ਦਾ(sep);

	ਜੇ (pos_begin == std::string::npos)
		pos_begin = ਪਾਠ.ਮਾਪ();

	ਜੇ (long_ਲੜੀਆਂ && pos_begin != ਪਾਠ.ਮਾਪ() && ਪਾਠ[pos_begin] == '"') {
		std::string sep_string = sep;
		ਲਈ (ਮਾਪ_t i = pos_begin+1; i < ਪਾਠ.ਮਾਪ(); i++) {
			ਜੇ (ਪਾਠ[i] == '"' && (i+1 == ਪਾਠ.ਮਾਪ() || sep_string.ਲੱਭੋ(ਪਾਠ[i+1]) != std::string::npos)) {
				std::string token = ਪਾਠ.substr(pos_begin, i-pos_begin+1);
				ਪਾਠ = ਪਾਠ.substr(i+1);
				ਵਾਪਸ token;
			}
			ਜੇ (i+1 < ਪਾਠ.ਮਾਪ() && ਪਾਠ[i] == '"' && ਪਾਠ[i+1] == ';' && (i+2 == ਪਾਠ.ਮਾਪ() || sep_string.ਲੱਭੋ(ਪਾਠ[i+2]) != std::string::npos)) {
				std::string token = ਪਾਠ.substr(pos_begin, i-pos_begin+1);
				ਪਾਠ = ਪਾਠ.substr(i+2);
				ਵਾਪਸ token + ";";
			}
		}
	}

	ਮਾਪ_t pos_ਸਿਰਾ = ਪਾਠ.ਲੱਭੋ_ਪਹਿਲਾ_ਦਾ(sep, pos_begin);

	ਜੇ (pos_ਸਿਰਾ == std::string::npos)
		pos_ਸਿਰਾ = ਪਾਠ.ਮਾਪ();

	std::string token = ਪਾਠ.substr(pos_begin, pos_ਸਿਰਾ-pos_begin);
	ਪਾਠ = ਪਾਠ.substr(pos_ਸਿਰਾ);
	ਵਾਪਸ token;
}

std::ਸਦਿਸ਼<std::string> ਵੰਡੋ_ਟੋਕਨ(ਅਚਲ std::string &ਪਾਠ, ਅਚਲ ਅੱਖਰ *sep)
{
	std::ਸਦਿਸ਼<std::string> ਟੋਕਨ;
	std::string current_token;
	ਲਈ (ਅੱਖਰ c : ਪਾਠ) {
		ਜੇ (strchr(sep, c)) {
			ਜੇ (!current_token.ਖਾਲੀ_ਥਾਵਾਂ()) {
				ਟੋਕਨ.ਧੱਕੋ_ਪਿੱਠ(current_token);
				current_token.clear();
			}
		} ਨਹੀਂਤਾਂ
			current_token += c;
	}
	ਜੇ (!current_token.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਟੋਕਨ.ਧੱਕੋ_ਪਿੱਠ(current_token);
		current_token.clear();
	}
	ਵਾਪਸ ਟੋਕਨ;
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
ਤਾਰਕਿਕ patmatch(ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *string)
{
	ਜੇ (*ਨਮੂਨਾ == 0)
		ਵਾਪਸ *string == 0;

	ਜੇ (*ਨਮੂਨਾ == '\\') {
		ਜੇ (ਨਮੂਨਾ[1] == string[0] && patmatch(ਨਮੂਨਾ+2, string+1))
			ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (*ਨਮੂਨਾ == '?') {
		ਜੇ (*string == 0)
			ਵਾਪਸ ਝੂਠ;
		ਵਾਪਸ patmatch(ਨਮੂਨਾ+1, string+1);
	}

	ਜੇ (*ਨਮੂਨਾ == '*') {
		ਜਦੋਂ (*string) {
			ਜੇ (patmatch(ਨਮੂਨਾ+1, string++))
				ਵਾਪਸ ਸੱਚ;
		}
		ਵਾਪਸ ਨਮੂਨਾ[1] == 0;
	}

	ਜੇ (*ਨਮੂਨਾ == '[') {
		ਤਾਰਕਿਕ ਮਿਲਿਆ_ਮੇਲ = ਝੂਠ;
		ਤਾਰਕਿਕ inverted_list = ਨਮੂਨਾ[1] == '!';
		ਅਚਲ ਅੱਖਰ *p = ਨਮੂਨਾ + (inverted_list ? 1 : 0);

		ਜਦੋਂ (*++p) {
			ਜੇ (*p == ']') {
				ਜੇ (ਮਿਲਿਆ_ਮੇਲ != inverted_list && patmatch(p+1, string+1))
					ਵਾਪਸ ਸੱਚ;
				ਰੋਕੋ;
			}

			ਜੇ (*p == '\\') {
				ਜੇ (*++p == *string)
					ਮਿਲਿਆ_ਮੇਲ = ਸੱਚ;
			} ਨਹੀਂਤਾਂ
			ਜੇ (*p == *string)
				ਮਿਲਿਆ_ਮੇਲ = ਸੱਚ;
		}
	}

	ਜੇ (*ਨਮੂਨਾ == *string)
		ਵਾਪਸ patmatch(ਨਮੂਨਾ+1, string+1);

	ਵਾਪਸ ਝੂਠ;
}

std::string get_ਅਧਾਰ_tmpdir()
{
	ਸਥਿਰ std::string tmpdir;

	ਜੇ (!tmpdir.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਵਾਪਸ tmpdir;
	}

#ਜੇ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ਜੇ (!Get_ਆਰਜ਼ੀ_ਰਸਤਾ_W(wtmppath.ਮਾਪ(), wtmppath.ਅੰਕੜੇ()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.ਲੱਭੋ(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, wtmppath.ਅੰਕੜੇ(), wtmppath.ਮਾਪ(), ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ));
	ਜੇ (Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, wtmppath.ਅੰਕੜੇ(), wtmppath.ਮਾਪ(), utmppath.ਅੰਕੜੇ(), utmppath.ਮਾਪ(), ਨਿਰਾ, ਨਿਰਾ) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ਨਹੀਂਤਾਂ
	ਅੱਖਰ * var = std::getenv("TMPDIR");
	ਜੇ (var && ਲੜੀ_ਲੰਬਾਈ(var)!=0) {
		tmpdir.ਸੌਂਪੋ(var);
		// We return the directory name without the trailing '/'
		ਜਦੋਂ (!tmpdir.ਖਾਲੀ_ਥਾਵਾਂ() && (tmpdir.ਪਿੱਠ() == '/')) {
			tmpdir.ਕੱਢੋ_ਪਿੱਠ();
		}
	} ਨਹੀਂਤਾਂ {
		tmpdir.ਸੌਂਪੋ("/tmp");
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ tmpdir;
}

std::string make_ਆਰਜ਼ੀ_file(std::string template_str)
{
	ਮਾਪ_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ਜੇ defined(__wasm)
	ਸਥਿਰ ਮਾਪ_t ਸੂਚਕਾਂਕ = 0;
	template_str.replace(pos, 6, stringf("%06zu", ਸੂਚਕਾਂਕ++));
#ਜਾਂਜੇ defined(_WIN32)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.ਸਿਰਾ(), '/', '\\');
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (1) {
		ਲਈ (ਪੂਰਨ i = 0; i < 6; i++) {
			ਸਥਿਰ std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ਸਥਿਰ uint32_t x = 314159265 ^ uint32_t(ਸਮਾਂ_2(ਨਿਰਾ));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.ਮਾਪ()];
		}
		ਜੇ (_access(template_str.c_str(), 0) != 0)
			ਰੋਕੋ;
	}
#ਨਹੀਂਤਾਂ
	ਪੂਰਨ suffixlen = template_str.ਮਾਪ() - pos - 6;

	ਅੱਖਰ *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	ਮੁਕਤ(p);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ template_str;
}

std::string make_ਆਰਜ਼ੀ_dir(std::string template_str)
{
#ਜੇ defined(_WIN32)
	template_str = make_ਆਰਜ਼ੀ_file(template_str);
	mkdir(template_str.c_str());
	ਵਾਪਸ template_str;
#ਜਾਂਜੇ defined(__wasm)
	template_str = make_ਆਰਜ਼ੀ_file(template_str);
	mkdir(template_str.c_str(), 0777);
	ਵਾਪਸ template_str;
#ਨਹੀਂਤਾਂ
#  ifndef NDEBUG
	ਮਾਪ_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	ਪੂਰਨ suffixlen = template_str.ਮਾਪ() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	ਅੱਖਰ *p = strdup(template_str.c_str());
	log_assert(p);
	ਅੱਖਰ *res = mkdtemp(p);
	ਜੇ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	ਮੁਕਤ(p);

	ਵਾਪਸ template_str;
#ਜੇ_ਅੰਤ
}

ਤਾਰਕਿਕ check_is_directory(ਅਚਲ std::string& dirname)
{
#ਜੇ defined(_WIN32)
	ਬਣਤਰ _stat info;
	ਸਵੈਚਾਲਿਤ dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ਜੇ (dirname.ਪਿੱਠ() == '/' || dirname.ਪਿੱਠ() == '\\') {
		dirname_ = dirname.substr(0, dirname.ਲੰਮਾਈ() - 1);
	}

	ਜੇ (_stat(dirname_.c_str(), &info) != 0)
	{
		ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ (info.st_ਬਹੁਲਕ & _S_IFDIR) != 0;
#ਨਹੀਂਤਾਂ
	ਬਣਤਰ stat info;
	ਜੇ (stat(dirname.c_str(), &info) != 0)
	{
		ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ (info.st_ਬਹੁਲਕ & S_IFDIR) != 0;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
ਤਾਰਕਿਕ check_accessible(ਅਚਲ std::string& filename, ਤਾਰਕਿਕ)
{
	ਵਾਪਸ _access(filename.c_str(), 0) == 0;
}
#ਨਹੀਂਤਾਂ
ਤਾਰਕਿਕ check_accessible(ਅਚਲ std::string& filename, ਤਾਰਕਿਕ is_exec)
{
	ਵਾਪਸ access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ check_file_exists(ਅਚਲ std::string& filename, ਤਾਰਕਿਕ is_exec)
{
	ਵਾਪਸ check_accessible(filename, is_exec) && !check_is_directory(filename);
}
ਤਾਰਕਿਕ check_directory_exists(ਅਚਲ std::string& filename, ਤਾਰਕਿਕ is_exec)
{
	ਵਾਪਸ check_accessible(filename, is_exec) && check_is_directory(filename);
}

ਤਾਰਕਿਕ is_absolute_ਰਸਤਾ(std::string filename)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
	ਵਾਪਸ filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ filename[0] == '/';
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ remove_directory(std::string dirname)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
	ਲੜੀਵਾਰ_command(stringf("rmdir /s /q \"%s\"", dirname));
#ਨਹੀਂਤਾਂ
	ਬਣਤਰ stat stbuf;
	ਬਣਤਰ dirent **namelist;
	ਪੂਰਨ n = scandir(dirname.c_str(), &namelist, ਖਾਲੀ_ਸੂਚਕ, alphasort);
	log_assert(n >= 0);
	ਲਈ (ਪੂਰਨ i = 0; i < n; i++) {
		ਜੇ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string ਬਫਰ = stringf("%s/%s", dirname, namelist[i]->d_name);
			ਜੇ (!stat(ਬਫਰ.c_str(), &stbuf) && S_ISREG(stbuf.st_ਬਹੁਲਕ)) {
				remove(ਬਫਰ.c_str());
			} ਨਹੀਂਤਾਂ
				remove_directory(ਬਫਰ);
		}
		ਮੁਕਤ(namelist[i]);
	}
	ਮੁਕਤ(namelist);
	rmdir(dirname.c_str());
#ਜੇ_ਅੰਤ
}

ਤਾਰਕਿਕ create_directory(ਅਚਲ std::string& dirname)
{
#ਜੇ defined(_WIN32)
	ਪੂਰਨ ret = _mkdir(dirname.c_str());
#ਨਹੀਂਤਾਂ
	ਬਹੁਲਕ_t ਬਹੁਲਕ = 0755;
	ਪੂਰਨ ret = mkdir(dirname.c_str(), ਬਹੁਲਕ);
#ਜੇ_ਅੰਤ
	ਜੇ (ret == 0)
		ਵਾਪਸ ਸੱਚ;

	ਚੋਣ (errno)
	{
	ਹਾਲਤ ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::ਮਾਪ_type pos = dirname.ਲੱਭੋ_ਆਖਰੀ_ਦਾ('/');
			ਜੇ (pos == std::string::npos)
#ਜੇ defined(_WIN32)
				pos = dirname.ਲੱਭੋ_ਆਖਰੀ_ਦਾ('\\');
			ਜੇ (pos == std::string::npos)
#ਜੇ_ਅੰਤ
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (!create_directory( dirname.substr(0, pos) ))
				ਵਾਪਸ ਝੂਠ;
		}
		// now, try to create again
#ਜੇ defined(_WIN32)
		ਵਾਪਸ 0 == _mkdir(dirname.c_str());
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ 0 == mkdir(dirname.c_str(), ਬਹੁਲਕ);
#ਜੇ_ਅੰਤ

	ਹਾਲਤ EEXIST:
		// done!
		ਵਾਪਸ check_directory_exists(dirname);

	ਮੂਲ:
		ਵਾਪਸ ਝੂਠ;
	}
}

ਤਾਰਕਿਕ make_ਨਿੱਜੀ_directory(ਅਚਲ std::string& dirname)
{
#ਜੇ defined(_WIN32) || defined(__wasi__)
	ਜੇ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ ਸੱਚ;
#ਨਹੀਂਤਾਂ
	ਜੇ (mkdir(dirname.c_str(), 0700) == 0)
		ਵਾਪਸ ਸੱਚ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	ਪੂਰਨ err = errno;
	std::string ਮਾਪਾ = std::filesystem::ਰਸਤਾ_2(dirname).ਮਾਪਾ_ਰਸਤਾ().string();

	ਜੇ (err == ENOENT && !ਮਾਪਾ.ਖਾਲੀ_ਥਾਵਾਂ() && create_directory(ਮਾਪਾ)) {
		ਜੇ (mkdir(dirname.c_str(), 0700) == 0)
			ਵਾਪਸ ਸੱਚ;
		err = errno;
	}

	ਜੇ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		ਵਾਪਸ ਝੂਠ;
	}

	// Do not use a leftover that isn't ours
	ਬਣਤਰ stat st;
	ਜੇ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_ਬਹੁਲਕ) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		ਵਾਪਸ ਝੂਠ;
	}
	// Repair the mode if something changed it
	ਬਹੁਲਕ_t ਬਹੁਲਕ = st.st_ਬਹੁਲਕ & 0777;
	ਜੇ (((ਬਹੁਲਕ & S_IRWXU) != S_IRWXU || (ਬਹੁਲਕ & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		ਵਾਪਸ ਝੂਠ;
	}

	ਵਾਪਸ ਸੱਚ;
#ਜੇ_ਅੰਤ
}

std::string make_user_tmpdir(ਅਚਲ std::string& name)
{
	log_assert(!name.ਖਾਲੀ_ਥਾਵਾਂ() && name.ਲੱਭੋ('/') == std::string::npos);
#ਜੇ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_ਅਧਾਰ_tmpdir() + "/" + name;
#ਨਹੀਂਤਾਂ
	std::string dirname = stringf("%s/%s-%u", get_ਅਧਾਰ_tmpdir().c_str(), name.c_str(), (ਬਿਨਾਨਿਸ਼ਾਨੀ)geteuid());
#ਜੇ_ਅੰਤ
	ਵਾਪਸ make_ਨਿੱਜੀ_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ਅਚਲ std::string& filename)
{
	std::string ਬਾਹਰਲਾ;
	ਬਾਹਰਲਾ.reserve(filename.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ c : filename)
	{
		ਜੇ (c == ' ')
			ਬਾਹਰਲਾ += "\\ ";
		ਨਹੀਂਤਾਂ
			ਬਾਹਰਲਾ.ਧੱਕੋ_ਪਿੱਠ(c);
	}
	ਵਾਪਸ ਬਾਹਰਲਾ;
}

ਖਾਲੀ append_globbed(std::ਸਦਿਸ਼<std::string>& paths, std::string ਨਮੂਨਾ)
{
	rewrite_filename(ਨਮੂਨਾ);
	std::ਸਦਿਸ਼<std::string> globbed = glob_filename(ਨਮੂਨਾ);
	copy(globbed.begin(), globbed.ਸਿਰਾ(), ਪਿੱਠ_inserter(paths));
}

std::string name_from_file_ਰਸਤਾ(std::string ਰਸਤਾ_2) {
	ਵਾਪਸ std::filesystem::ਰਸਤਾ_2(ਰਸਤਾ_2).filename().string();
}

std::string absolute_ਰਸਤਾ(ਅਚਲ std::string& ਰਸਤਾ_2)
{
	std::error_ਕੋਡ ec;
	std::filesystem::ਰਸਤਾ_2 ਨਤੀਜਾ = std::filesystem::absolute(ਰਸਤਾ_2, ec);

	ਜੇ (ec)
		ਵਾਪਸ "";

	ਨਤੀਜਾ = std::filesystem::weakly_canonical(ਨਤੀਜਾ, ec);

	ਜੇ (ec)
		ਵਾਪਸ "";

	ਵਾਪਸ ਨਤੀਜਾ.string();
}

// Includes OS_PATH_SEP at the end if present
std::string ਮਾਪਾ_from_file_ਰਸਤਾ(std::string ਰਸਤਾ_2) {
	ਸਵੈਚਾਲਿਤ ਮਾਪਾ = std::filesystem::ਰਸਤਾ_2(ਰਸਤਾ_2).ਮਾਪਾ_ਰਸਤਾ();
	ਜੇ (ਮਾਪਾ.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਵਾਪਸ "";
	}
	// Add trailing separator to match original behavior
	std::string ਨਤੀਜਾ = ਮਾਪਾ.string();
	ਜੇ (!ਨਤੀਜਾ.ਖਾਲੀ_ਥਾਵਾਂ() && ਨਤੀਜਾ.ਪਿੱਠ() != std::filesystem::ਰਸਤਾ_2::preferred_separator) {
		ਨਤੀਜਾ += std::filesystem::ਰਸਤਾ_2::preferred_separator;
	}
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ format_emit_unescaped(std::string &ਨਤੀਜਾ, std::string_view fmt)
{
	ਨਤੀਜਾ.reserve(ਨਤੀਜਾ.ਮਾਪ() + fmt.ਮਾਪ());
	ਲਈ (ਮਾਪ_t i = 0; i < fmt.ਮਾਪ(); ++i) {
		ਅੱਖਰ ch = fmt[i];
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(ch);
		ਜੇ (ch == '%' && i + 1 < fmt.ਮਾਪ() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string ਨਤੀਜਾ;
	format_emit_unescaped(ਨਤੀਜਾ, fmt);
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string ਨਤੀਜਾ = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ spec_parameter_ਮਾਪ(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	ਚੋਣ (spec[spec.ਮਾਪ() - 1]) {
	ਹਾਲਤ 'd':
	ਹਾਲਤ 'i':
	ਹਾਲਤ 'o':
	ਹਾਲਤ 'u':
	ਹਾਲਤ 'x':
	ਹਾਲਤ 'X':
		ਚੋਣ (spec[spec.ਮਾਪ() - 2]) {
		ਹਾਲਤ 'h':
			ਜੇ (spec[spec.ਮਾਪ() - 3] == 'h')
				ਵਾਪਸ ਆਕਾਰ(ਅੱਖਰ);
			ਵਾਪਸ ਆਕਾਰ(ਛੋਟਾ);
		ਹਾਲਤ 'l':
			ਜੇ (spec[spec.ਮਾਪ() - 3] == 'l')
				ਵਾਪਸ ਆਕਾਰ(ਲੰਮਾ ਲੰਮਾ);
			ਵਾਪਸ ਆਕਾਰ(ਲੰਮਾ);
		ਹਾਲਤ 'L':
		ਹਾਲਤ 'q':
			ਵਾਪਸ ਆਕਾਰ(ਲੰਮਾ ਲੰਮਾ);
		ਹਾਲਤ 'j':
			ਵਾਪਸ ਆਕਾਰ(intmax_t);
		ਹਾਲਤ 'z':
		ਹਾਲਤ 'Z':
			ਵਾਪਸ ਆਕਾਰ(ਮਾਪ_t);
		ਹਾਲਤ 't':
			ਵਾਪਸ ਆਕਾਰ(ptrdiff_t);
		}
		ਵਾਪਸ ਆਕਾਰ(ਪੂਰਨ);
	ਹਾਲਤ 'e':
	ਹਾਲਤ 'E':
	ਹਾਲਤ 'f':
	ਹਾਲਤ 'F':
	ਹਾਲਤ 'g':
	ਹਾਲਤ 'G':
	ਹਾਲਤ 'a':
	ਹਾਲਤ 'A':
		ਜੇ (spec[spec.ਮਾਪ() - 2] == 'L')
			ਵਾਪਸ ਆਕਾਰ(ਲੰਮਾ ਦੁੱਗਣਾ);
		ਜੇ (spec[spec.ਮਾਪ() - 2] == 'l' && spec[spec.ਮਾਪ() - 3] == 'l')
			ਵਾਪਸ ਆਕਾਰ(ਲੰਮਾ ਦੁੱਗਣਾ);
		ਵਾਪਸ ਆਕਾਰ(ਦੁੱਗਣਾ);
	ਹਾਲਤ 'c':
		ਜੇ (spec[spec.ਮਾਪ() - 2] == 'l') {
			ਵਾਪਸ ਆਕਾਰ(wchar_t);
		}
		ਵਾਪਸ ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ);
	ਹਾਲਤ 'C':
		ਵਾਪਸ ਆਕਾਰ(wchar_t);
	ਹਾਲਤ 's':
	ਹਾਲਤ 'p':
	ਹਾਲਤ 'S':
	ਹਾਲਤ 'n':
		ਵਾਪਸ ਆਕਾਰ(ਖਾਲੀ *);
	ਹਾਲਤ 'm':
		ਵਾਪਸ ਆਕਾਰ(ਪੂਰਨ);
	ਮੂਲ:
		ਵਾਪਸ -1;
	}
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Arg>
ਸਥਿਰ ਖਾਲੀ format_emit_stringf(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	ਚੋਣ (ਅੰਕ_dynamic_ints) {
	ਹਾਲਤ Dynamic_Int_ਗਿਣਤੀ::NONE:
		ਨਤੀਜਾ += string_view_stringf(spec, arg);
		ਵਾਪਸ;
	ਹਾਲਤ Dynamic_Int_ਗਿਣਤੀ::ONE:
		ਨਤੀਜਾ += string_view_stringf(spec, dynamic_ints[0], arg);
		ਵਾਪਸ;
	ਹਾਲਤ Dynamic_Int_ਗਿਣਤੀ::TWO:
		ਨਤੀਜਾ += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		ਵਾਪਸ;
	}
	YOSYS_ABORT("Internal error");
}

ਖਾਲੀ format_emit_long_long(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਲੰਮਾ ਲੰਮਾ arg)
{
	ਜੇ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ਨਤੀਜਾ += std::ਮੰਜ਼ਿਲ_string(static_cast<ਪੂਰਨ>(arg));
		ਵਾਪਸ;
	}
	ਜੇ (spec_parameter_ਮਾਪ(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, (ਪੂਰਨ)arg);
	} ਨਹੀਂਤਾਂ {
		format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, arg);
	}
}

ਖਾਲੀ format_emit_unsigned_long_long(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ arg)
{
	ਜੇ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ਨਤੀਜਾ += std::ਮੰਜ਼ਿਲ_string(static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ>(arg));
		ਵਾਪਸ;
	}
	ਜੇ (spec == "%c") {
		ਨਤੀਜਾ += static_cast<ਅੱਖਰ>(arg);
		ਵਾਪਸ;
	}
	ਜੇ (spec_parameter_ਮਾਪ(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arg);
	} ਨਹੀਂਤਾਂ {
		format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, arg);
	}
}

ਖਾਲੀ format_emit_double(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਦੁੱਗਣਾ arg)
{
	format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, arg);
}

ਖਾਲੀ format_emit_char_ptr(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਅਚਲ ਅੱਖਰ *arg)
{
	ਜੇ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ਨਤੀਜਾ += arg;
		ਵਾਪਸ;
	}
	format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, arg);
}

ਖਾਲੀ format_emit_string(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਅਚਲ std::string &arg)
{
	ਜੇ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ਨਤੀਜਾ += arg;
		ਵਾਪਸ;
	}
	format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, arg.c_str());
}

ਖਾਲੀ format_emit_string_view(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, std::string_view arg)
{
	ਜੇ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		ਨਤੀਜਾ += arg;
		ਵਾਪਸ;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, std::string(arg).c_str());
}

ਖਾਲੀ format_emit_void_ptr(std::string &ਨਤੀਜਾ, std::string_view spec, ਪੂਰਨ *dynamic_ints,
	Dynamic_Int_ਗਿਣਤੀ ਅੰਕ_dynamic_ints, ਅਚਲ ਖਾਲੀ *arg)
{
	format_emit_stringf(ਨਤੀਜਾ, spec, dynamic_ints, ਅੰਕ_dynamic_ints, arg);
}

#ਜੇ defined(_WIN32)
Scoped_File_ਤਾਲਾ::Scoped_File_ਤਾਲਾ(ਅਚਲ std::string &ਰਸਤਾ_2)
{
	HANDLE h = CreateFileA(ਰਸਤਾ_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ਨਿਰਾ, OPEN_ALWAYS, FILE_ATTRIBUTE_ਸਾਧਾਰਨ, ਨਿਰਾ);
	ਜੇ (h == INVALID_HANDLE_ਮੁੱਲ) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", ਰਸਤਾ_2.c_str());
		ਵਾਪਸ;
	}
	OVERLAPPED ov = {};
	ਜੇ (!ਤਾਲਾ_File_Ex(h, LOCKFILE_EXCLUSIVE_ਤਾਲਾ, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", ਰਸਤਾ_2.c_str());
		CloseHandle(h);
		ਵਾਪਸ;
	}
	handle = h;
}

Scoped_File_ਤਾਲਾ::~Scoped_File_ਤਾਲਾ()
{
	ਜੇ (handle != ਖਾਲੀ_ਸੂਚਕ) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#ਜਾਂਜੇ !defined(__wasi__)
Scoped_File_ਤਾਲਾ::Scoped_File_ਤਾਲਾ(ਅਚਲ std::string &ਰਸਤਾ_2)
{
	fd = open(ਰਸਤਾ_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ਜੇ (fd < 0)
		fd = open(ਰਸਤਾ_2.c_str(), O_RDONLY | O_CLOEXEC);
	ਜੇ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", ਰਸਤਾ_2.c_str());
		ਵਾਪਸ;
	}
	ਪੂਰਨ ret;
	ਜਦੋਂ ((ret = flock(fd, ਤਾਲਾ_EX)) != 0 && errno == EINTR);
	ਜੇ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", ਰਸਤਾ_2.c_str());
}

Scoped_File_ਤਾਲਾ::~Scoped_File_ਤਾਲਾ()
{
	ਜੇ (fd >= 0)
		close(fd); // releases the flock
}
#ਨਹੀਂਤਾਂ
Scoped_File_ਤਾਲਾ::Scoped_File_ਤਾਲਾ(ਅਚਲ std::string &) {}
Scoped_File_ਤਾਲਾ::~Scoped_File_ਤਾਲਾ() {}
#ਜੇ_ਅੰਤ

ਬਿਨਾਨਿਸ਼ਾਨੀ get_process_id()
{
#ਜੇ defined(_WIN32)
	ਵਾਪਸ GetCurrentProcessId();
#ਜਾਂਜੇ defined(__wasi__)
	ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ getpid();
#ਜੇ_ਅੰਤ
}

YOSYS_NAMESPACE_ਸਿਰਾ
