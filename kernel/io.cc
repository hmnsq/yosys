#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/log.h"
#అన్తర్భావ <iostream>
#అన్తర్భావ <string>
#అన్తర్భావ <filesystem>
#అన్తర్భావ <system_error>

#ఒకవేళ !defined(WIN32)
#అన్తర్భావ <dirent.h>
#అన్తర్భావ <unistd.h>
#లేకపోతే
#అన్తర్భావ <io.h>
#అయితే_ముగింపు

#ఒకవేళ !defined(_WIN32) && !defined(__wasi__)
#అన్తర్భావ <sys/file.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <cerrno>
#అయితే_ముగింపు

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

పూర్ణాంకం readsome(std::istream &f, అక్షరం *s, పూర్ణాంకం n)
{
	పూర్ణాంకం rc = పూర్ణాంకం(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	ఒకవేళ (rc == 0) {
		పూర్ణాంకం c = f.get();
		ఒకవేళ (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	తిరిగి rc;
}

std::string తదుపరి_token(std::string &పాఠ్యం, స్థిరరాశి అక్షరం *sep, తార్కిక long_అక్షరశ్రేణులు)
{
	కొలత_t pos_begin = పాఠ్యం.కనుగొను_మొదటి_not_యొక్క(sep);

	ఒకవేళ (pos_begin == std::string::npos)
		pos_begin = పాఠ్యం.కొలత();

	ఒకవేళ (long_అక్షరశ్రేణులు && pos_begin != పాఠ్యం.కొలత() && పాఠ్యం[pos_begin] == '"') {
		std::string sep_string = sep;
		కొరకు (కొలత_t i = pos_begin+1; i < పాఠ్యం.కొలత(); i++) {
			ఒకవేళ (పాఠ్యం[i] == '"' && (i+1 == పాఠ్యం.కొలత() || sep_string.కనుగొను(పాఠ్యం[i+1]) != std::string::npos)) {
				std::string token = పాఠ్యం.substr(pos_begin, i-pos_begin+1);
				పాఠ్యం = పాఠ్యం.substr(i+1);
				తిరిగి token;
			}
			ఒకవేళ (i+1 < పాఠ్యం.కొలత() && పాఠ్యం[i] == '"' && పాఠ్యం[i+1] == ';' && (i+2 == పాఠ్యం.కొలత() || sep_string.కనుగొను(పాఠ్యం[i+2]) != std::string::npos)) {
				std::string token = పాఠ్యం.substr(pos_begin, i-pos_begin+1);
				పాఠ్యం = పాఠ్యం.substr(i+2);
				తిరిగి token + ";";
			}
		}
	}

	కొలత_t pos_చివర = పాఠ్యం.కనుగొను_మొదటి_యొక్క(sep, pos_begin);

	ఒకవేళ (pos_చివర == std::string::npos)
		pos_చివర = పాఠ్యం.కొలత();

	std::string token = పాఠ్యం.substr(pos_begin, pos_చివర-pos_begin);
	పాఠ్యం = పాఠ్యం.substr(pos_చివర);
	తిరిగి token;
}

std::సదిశ<std::string> విభజించు_టోకెన్లు(స్థిరరాశి std::string &పాఠ్యం, స్థిరరాశి అక్షరం *sep)
{
	std::సదిశ<std::string> టోకెన్లు;
	std::string current_token;
	కొరకు (అక్షరం c : పాఠ్యం) {
		ఒకవేళ (strchr(sep, c)) {
			ఒకవేళ (!current_token.ఖాళీలు()) {
				టోకెన్లు.నెట్టు_వెనుక(current_token);
				current_token.clear();
			}
		} లేకపోతే
			current_token += c;
	}
	ఒకవేళ (!current_token.ఖాళీలు()) {
		టోకెన్లు.నెట్టు_వెనుక(current_token);
		current_token.clear();
	}
	తిరిగి టోకెన్లు;
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
తార్కిక patmatch(స్థిరరాశి అక్షరం *నమూనా, స్థిరరాశి అక్షరం *string)
{
	ఒకవేళ (*నమూనా == 0)
		తిరిగి *string == 0;

	ఒకవేళ (*నమూనా == '\\') {
		ఒకవేళ (నమూనా[1] == string[0] && patmatch(నమూనా+2, string+1))
			తిరిగి నిజం;
	}

	ఒకవేళ (*నమూనా == '?') {
		ఒకవేళ (*string == 0)
			తిరిగి అబద్ధం;
		తిరిగి patmatch(నమూనా+1, string+1);
	}

	ఒకవేళ (*నమూనా == '*') {
		అయితే (*string) {
			ఒకవేళ (patmatch(నమూనా+1, string++))
				తిరిగి నిజం;
		}
		తిరిగి నమూనా[1] == 0;
	}

	ఒకవేళ (*నమూనా == '[') {
		తార్కిక దొరికింది_సరిపోలిక = అబద్ధం;
		తార్కిక inverted_list = నమూనా[1] == '!';
		స్థిరరాశి అక్షరం *p = నమూనా + (inverted_list ? 1 : 0);

		అయితే (*++p) {
			ఒకవేళ (*p == ']') {
				ఒకవేళ (దొరికింది_సరిపోలిక != inverted_list && patmatch(p+1, string+1))
					తిరిగి నిజం;
				ఆపు;
			}

			ఒకవేళ (*p == '\\') {
				ఒకవేళ (*++p == *string)
					దొరికింది_సరిపోలిక = నిజం;
			} లేకపోతే
			ఒకవేళ (*p == *string)
				దొరికింది_సరిపోలిక = నిజం;
		}
	}

	ఒకవేళ (*నమూనా == *string)
		తిరిగి patmatch(నమూనా+1, string+1);

	తిరిగి అబద్ధం;
}

std::string get_పునాది_tmpdir()
{
	స్థిర std::string tmpdir;

	ఒకవేళ (!tmpdir.ఖాళీలు()) {
		తిరిగి tmpdir;
	}

#ఒకవేళ defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	ఒకవేళ (!Get_తాత్కాలిక_మార్గం_W(wtmppath.కొలత(), wtmppath.దత్తాంశం()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.కనుగొను(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, wtmppath.దత్తాంశం(), wtmppath.కొలత(), ఖాళీ, 0, ఖాళీ, ఖాళీ));
	ఒకవేళ (Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, wtmppath.దత్తాంశం(), wtmppath.కొలత(), utmppath.దత్తాంశం(), utmppath.కొలత(), ఖాళీ, ఖాళీ) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#లేకపోతే
	అక్షరం * var = std::getenv("TMPDIR");
	ఒకవేళ (var && అక్షరశ్రేణి_పొడవు(var)!=0) {
		tmpdir.కేటాయించు(var);
		// We return the directory name without the trailing '/'
		అయితే (!tmpdir.ఖాళీలు() && (tmpdir.వెనుక() == '/')) {
			tmpdir.తీయి_వెనుక();
		}
	} లేకపోతే {
		tmpdir.కేటాయించు("/tmp");
	}
#అయితే_ముగింపు
	తిరిగి tmpdir;
}

std::string make_తాత్కాలిక_file(std::string template_str)
{
	కొలత_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#ఒకవేళ defined(__wasm)
	స్థిర కొలత_t సూచీ = 0;
	template_str.replace(pos, 6, stringf("%06zu", సూచీ++));
#లేదావేళ defined(_WIN32)
#నిర్వచితం_కాకపోతే YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.చివర(), '/', '\\');
#అయితే_ముగింపు
	అయితే (1) {
		కొరకు (పూర్ణాంకం i = 0; i < 6; i++) {
			స్థిర std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			స్థిర uint32_t x = 314159265 ^ uint32_t(సమయం_2(ఖాళీ));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.కొలత()];
		}
		ఒకవేళ (_access(template_str.c_str(), 0) != 0)
			ఆపు;
	}
#లేకపోతే
	పూర్ణాంకం suffixlen = template_str.కొలత() - pos - 6;

	అక్షరం *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	విడుదల(p);
#అయితే_ముగింపు

	తిరిగి template_str;
}

std::string make_తాత్కాలిక_dir(std::string template_str)
{
#ఒకవేళ defined(_WIN32)
	template_str = make_తాత్కాలిక_file(template_str);
	mkdir(template_str.c_str());
	తిరిగి template_str;
#లేదావేళ defined(__wasm)
	template_str = make_తాత్కాలిక_file(template_str);
	mkdir(template_str.c_str(), 0777);
	తిరిగి template_str;
#లేకపోతే
#  ifndef NDEBUG
	కొలత_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	పూర్ణాంకం suffixlen = template_str.కొలత() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	అక్షరం *p = strdup(template_str.c_str());
	log_assert(p);
	అక్షరం *res = mkdtemp(p);
	ఒకవేళ (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	విడుదల(p);

	తిరిగి template_str;
#అయితే_ముగింపు
}

తార్కిక check_is_directory(స్థిరరాశి std::string& dirname)
{
#ఒకవేళ defined(_WIN32)
	నిర్మాణం _stat info;
	స్వయంచాలక dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	ఒకవేళ (dirname.వెనుక() == '/' || dirname.వెనుక() == '\\') {
		dirname_ = dirname.substr(0, dirname.నిడివి() - 1);
	}

	ఒకవేళ (_stat(dirname_.c_str(), &info) != 0)
	{
		తిరిగి అబద్ధం;
	}
	తిరిగి (info.st_బహుళకం & _S_IFDIR) != 0;
#లేకపోతే
	నిర్మాణం stat info;
	ఒకవేళ (stat(dirname.c_str(), &info) != 0)
	{
		తిరిగి అబద్ధం;
	}
	తిరిగి (info.st_బహుళకం & S_IFDIR) != 0;
#అయితే_ముగింపు
}

#నిర్వచితమైతే _WIN32
తార్కిక check_accessible(స్థిరరాశి std::string& filename, తార్కిక)
{
	తిరిగి _access(filename.c_str(), 0) == 0;
}
#లేకపోతే
తార్కిక check_accessible(స్థిరరాశి std::string& filename, తార్కిక is_exec)
{
	తిరిగి access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#అయితే_ముగింపు

తార్కిక check_file_exists(స్థిరరాశి std::string& filename, తార్కిక is_exec)
{
	తిరిగి check_accessible(filename, is_exec) && !check_is_directory(filename);
}
తార్కిక check_directory_exists(స్థిరరాశి std::string& filename, తార్కిక is_exec)
{
	తిరిగి check_accessible(filename, is_exec) && check_is_directory(filename);
}

తార్కిక is_absolute_మార్గం(std::string filename)
{
#నిర్వచితమైతే _WIN32
	తిరిగి filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#లేకపోతే
	తిరిగి filename[0] == '/';
#అయితే_ముగింపు
}

శూన్య remove_directory(std::string dirname)
{
#నిర్వచితమైతే _WIN32
	నిరంతరం_command(stringf("rmdir /s /q \"%s\"", dirname));
#లేకపోతే
	నిర్మాణం stat stbuf;
	నిర్మాణం dirent **namelist;
	పూర్ణాంకం n = scandir(dirname.c_str(), &namelist, శూన్యసూచిక, alphasort);
	log_assert(n >= 0);
	కొరకు (పూర్ణాంకం i = 0; i < n; i++) {
		ఒకవేళ (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string బఫరు = stringf("%s/%s", dirname, namelist[i]->d_name);
			ఒకవేళ (!stat(బఫరు.c_str(), &stbuf) && S_ISREG(stbuf.st_బహుళకం)) {
				remove(బఫరు.c_str());
			} లేకపోతే
				remove_directory(బఫరు);
		}
		విడుదల(namelist[i]);
	}
	విడుదల(namelist);
	rmdir(dirname.c_str());
#అయితే_ముగింపు
}

తార్కిక create_directory(స్థిరరాశి std::string& dirname)
{
#ఒకవేళ defined(_WIN32)
	పూర్ణాంకం ret = _mkdir(dirname.c_str());
#లేకపోతే
	బహుళకం_t బహుళకం = 0755;
	పూర్ణాంకం ret = mkdir(dirname.c_str(), బహుళకం);
#అయితే_ముగింపు
	ఒకవేళ (ret == 0)
		తిరిగి నిజం;

	ఎంపిక (errno)
	{
	స్థితి ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::కొలత_type pos = dirname.కనుగొను_చివరి_యొక్క('/');
			ఒకవేళ (pos == std::string::npos)
#ఒకవేళ defined(_WIN32)
				pos = dirname.కనుగొను_చివరి_యొక్క('\\');
			ఒకవేళ (pos == std::string::npos)
#అయితే_ముగింపు
				తిరిగి అబద్ధం;
			ఒకవేళ (!create_directory( dirname.substr(0, pos) ))
				తిరిగి అబద్ధం;
		}
		// now, try to create again
#ఒకవేళ defined(_WIN32)
		తిరిగి 0 == _mkdir(dirname.c_str());
#లేకపోతే
		తిరిగి 0 == mkdir(dirname.c_str(), బహుళకం);
#అయితే_ముగింపు

	స్థితి EEXIST:
		// done!
		తిరిగి check_directory_exists(dirname);

	అప్రమేయ:
		తిరిగి అబద్ధం;
	}
}

తార్కిక make_వ్యక్తిగత_directory(స్థిరరాశి std::string& dirname)
{
#ఒకవేళ defined(_WIN32) || defined(__wasi__)
	ఒకవేళ (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		తిరిగి అబద్ధం;
	}
	తిరిగి నిజం;
#లేకపోతే
	ఒకవేళ (mkdir(dirname.c_str(), 0700) == 0)
		తిరిగి నిజం;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	పూర్ణాంకం err = errno;
	std::string మాతృ = std::filesystem::మార్గం_2(dirname).మాతృ_మార్గం().string();

	ఒకవేళ (err == ENOENT && !మాతృ.ఖాళీలు() && create_directory(మాతృ)) {
		ఒకవేళ (mkdir(dirname.c_str(), 0700) == 0)
			తిరిగి నిజం;
		err = errno;
	}

	ఒకవేళ (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		తిరిగి అబద్ధం;
	}

	// Do not use a leftover that isn't ours
	నిర్మాణం stat st;
	ఒకవేళ (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_బహుళకం) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		తిరిగి అబద్ధం;
	}
	// Repair the mode if something changed it
	బహుళకం_t బహుళకం = st.st_బహుళకం & 0777;
	ఒకవేళ (((బహుళకం & S_IRWXU) != S_IRWXU || (బహుళకం & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		తిరిగి అబద్ధం;
	}

	తిరిగి నిజం;
#అయితే_ముగింపు
}

std::string make_user_tmpdir(స్థిరరాశి std::string& name)
{
	log_assert(!name.ఖాళీలు() && name.కనుగొను('/') == std::string::npos);
#ఒకవేళ defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_పునాది_tmpdir() + "/" + name;
#లేకపోతే
	std::string dirname = stringf("%s/%s-%u", get_పునాది_tmpdir().c_str(), name.c_str(), (గుర్తులేని)geteuid());
#అయితే_ముగింపు
	తిరిగి make_వ్యక్తిగత_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(స్థిరరాశి std::string& filename)
{
	std::string బహిర్గతం;
	బహిర్గతం.reserve(filename.కొలత());
	కొరకు (స్వయంచాలక c : filename)
	{
		ఒకవేళ (c == ' ')
			బహిర్గతం += "\\ ";
		లేకపోతే
			బహిర్గతం.నెట్టు_వెనుక(c);
	}
	తిరిగి బహిర్గతం;
}

శూన్య append_globbed(std::సదిశ<std::string>& paths, std::string నమూనా)
{
	rewrite_filename(నమూనా);
	std::సదిశ<std::string> globbed = glob_filename(నమూనా);
	copy(globbed.begin(), globbed.చివర(), వెనుక_inserter(paths));
}

std::string name_from_file_మార్గం(std::string మార్గం_2) {
	తిరిగి std::filesystem::మార్గం_2(మార్గం_2).filename().string();
}

std::string absolute_మార్గం(స్థిరరాశి std::string& మార్గం_2)
{
	std::error_సంకేతం ec;
	std::filesystem::మార్గం_2 ఫలితం = std::filesystem::absolute(మార్గం_2, ec);

	ఒకవేళ (ec)
		తిరిగి "";

	ఫలితం = std::filesystem::weakly_canonical(ఫలితం, ec);

	ఒకవేళ (ec)
		తిరిగి "";

	తిరిగి ఫలితం.string();
}

// Includes OS_PATH_SEP at the end if present
std::string మాతృ_from_file_మార్గం(std::string మార్గం_2) {
	స్వయంచాలక మాతృ = std::filesystem::మార్గం_2(మార్గం_2).మాతృ_మార్గం();
	ఒకవేళ (మాతృ.ఖాళీలు()) {
		తిరిగి "";
	}
	// Add trailing separator to match original behavior
	std::string ఫలితం = మాతృ.string();
	ఒకవేళ (!ఫలితం.ఖాళీలు() && ఫలితం.వెనుక() != std::filesystem::మార్గం_2::preferred_separator) {
		ఫలితం += std::filesystem::మార్గం_2::preferred_separator;
	}
	తిరిగి ఫలితం;
}

శూన్య format_emit_unescaped(std::string &ఫలితం, std::string_view fmt)
{
	ఫలితం.reserve(ఫలితం.కొలత() + fmt.కొలత());
	కొరకు (కొలత_t i = 0; i < fmt.కొలత(); ++i) {
		అక్షరం ch = fmt[i];
		ఫలితం.నెట్టు_వెనుక(ch);
		ఒకవేళ (ch == '%' && i + 1 < fmt.కొలత() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string ఫలితం;
	format_emit_unescaped(ఫలితం, fmt);
	తిరిగి ఫలితం;
}

స్థిర std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string ఫలితం = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం spec_parameter_కొలత(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	ఎంపిక (spec[spec.కొలత() - 1]) {
	స్థితి 'd':
	స్థితి 'i':
	స్థితి 'o':
	స్థితి 'u':
	స్థితి 'x':
	స్థితి 'X':
		ఎంపిక (spec[spec.కొలత() - 2]) {
		స్థితి 'h':
			ఒకవేళ (spec[spec.కొలత() - 3] == 'h')
				తిరిగి పరిమాణం(అక్షరం);
			తిరిగి పరిమాణం(పొట్టి);
		స్థితి 'l':
			ఒకవేళ (spec[spec.కొలత() - 3] == 'l')
				తిరిగి పరిమాణం(దీర్ఘ దీర్ఘ);
			తిరిగి పరిమాణం(దీర్ఘ);
		స్థితి 'L':
		స్థితి 'q':
			తిరిగి పరిమాణం(దీర్ఘ దీర్ఘ);
		స్థితి 'j':
			తిరిగి పరిమాణం(intmax_t);
		స్థితి 'z':
		స్థితి 'Z':
			తిరిగి పరిమాణం(కొలత_t);
		స్థితి 't':
			తిరిగి పరిమాణం(ptrdiff_t);
		}
		తిరిగి పరిమాణం(పూర్ణాంకం);
	స్థితి 'e':
	స్థితి 'E':
	స్థితి 'f':
	స్థితి 'F':
	స్థితి 'g':
	స్థితి 'G':
	స్థితి 'a':
	స్థితి 'A':
		ఒకవేళ (spec[spec.కొలత() - 2] == 'L')
			తిరిగి పరిమాణం(దీర్ఘ ద్విగుణ);
		ఒకవేళ (spec[spec.కొలత() - 2] == 'l' && spec[spec.కొలత() - 3] == 'l')
			తిరిగి పరిమాణం(దీర్ఘ ద్విగుణ);
		తిరిగి పరిమాణం(ద్విగుణ);
	స్థితి 'c':
		ఒకవేళ (spec[spec.కొలత() - 2] == 'l') {
			తిరిగి పరిమాణం(wchar_t);
		}
		తిరిగి పరిమాణం(గుర్తులేని అక్షరం);
	స్థితి 'C':
		తిరిగి పరిమాణం(wchar_t);
	స్థితి 's':
	స్థితి 'p':
	స్థితి 'S':
	స్థితి 'n':
		తిరిగి పరిమాణం(శూన్య *);
	స్థితి 'm':
		తిరిగి పరిమాణం(పూర్ణాంకం);
	అప్రమేయ:
		తిరిగి -1;
	}
}

మూస <రకం_పేరు Arg>
స్థిర శూన్య format_emit_stringf(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	ఎంపిక (అంకె_dynamic_ints) {
	స్థితి Dynamic_Int_లెక్క::NONE:
		ఫలితం += string_view_stringf(spec, arg);
		తిరిగి;
	స్థితి Dynamic_Int_లెక్క::ONE:
		ఫలితం += string_view_stringf(spec, dynamic_ints[0], arg);
		తిరిగి;
	స్థితి Dynamic_Int_లెక్క::TWO:
		ఫలితం += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		తిరిగి;
	}
	YOSYS_ABORT("Internal error");
}

శూన్య format_emit_long_long(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, దీర్ఘ దీర్ఘ arg)
{
	ఒకవేళ (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ఫలితం += std::గమ్యం_string(static_cast<పూర్ణాంకం>(arg));
		తిరిగి;
	}
	ఒకవేళ (spec_parameter_కొలత(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, (పూర్ణాంకం)arg);
	} లేకపోతే {
		format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, arg);
	}
}

శూన్య format_emit_unsigned_long_long(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, గుర్తులేని దీర్ఘ దీర్ఘ arg)
{
	ఒకవేళ (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ఫలితం += std::గమ్యం_string(static_cast<గుర్తులేని పూర్ణాంకం>(arg));
		తిరిగి;
	}
	ఒకవేళ (spec == "%c") {
		ఫలితం += static_cast<అక్షరం>(arg);
		తిరిగి;
	}
	ఒకవేళ (spec_parameter_కొలత(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, (గుర్తులేని పూర్ణాంకం)arg);
	} లేకపోతే {
		format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, arg);
	}
}

శూన్య format_emit_double(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, ద్విగుణ arg)
{
	format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, arg);
}

శూన్య format_emit_char_ptr(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, స్థిరరాశి అక్షరం *arg)
{
	ఒకవేళ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ఫలితం += arg;
		తిరిగి;
	}
	format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, arg);
}

శూన్య format_emit_string(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, స్థిరరాశి std::string &arg)
{
	ఒకవేళ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		ఫలితం += arg;
		తిరిగి;
	}
	format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, arg.c_str());
}

శూన్య format_emit_string_view(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, std::string_view arg)
{
	ఒకవేళ (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		ఫలితం += arg;
		తిరిగి;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, std::string(arg).c_str());
}

శూన్య format_emit_void_ptr(std::string &ఫలితం, std::string_view spec, పూర్ణాంకం *dynamic_ints,
	Dynamic_Int_లెక్క అంకె_dynamic_ints, స్థిరరాశి శూన్య *arg)
{
	format_emit_stringf(ఫలితం, spec, dynamic_ints, అంకె_dynamic_ints, arg);
}

#ఒకవేళ defined(_WIN32)
Scoped_File_తాళం::Scoped_File_తాళం(స్థిరరాశి std::string &మార్గం_2)
{
	HANDLE h = CreateFileA(మార్గం_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			ఖాళీ, OPEN_ALWAYS, FILE_ATTRIBUTE_సాధారణ, ఖాళీ);
	ఒకవేళ (h == INVALID_HANDLE_విలువ) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", మార్గం_2.c_str());
		తిరిగి;
	}
	OVERLAPPED ov = {};
	ఒకవేళ (!తాళం_File_Ex(h, LOCKFILE_EXCLUSIVE_తాళం, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", మార్గం_2.c_str());
		CloseHandle(h);
		తిరిగి;
	}
	handle = h;
}

Scoped_File_తాళం::~Scoped_File_తాళం()
{
	ఒకవేళ (handle != శూన్యసూచిక) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#లేదావేళ !defined(__wasi__)
Scoped_File_తాళం::Scoped_File_తాళం(స్థిరరాశి std::string &మార్గం_2)
{
	fd = open(మార్గం_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	ఒకవేళ (fd < 0)
		fd = open(మార్గం_2.c_str(), O_RDONLY | O_CLOEXEC);
	ఒకవేళ (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", మార్గం_2.c_str());
		తిరిగి;
	}
	పూర్ణాంకం ret;
	అయితే ((ret = flock(fd, తాళం_EX)) != 0 && errno == EINTR);
	ఒకవేళ (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", మార్గం_2.c_str());
}

Scoped_File_తాళం::~Scoped_File_తాళం()
{
	ఒకవేళ (fd >= 0)
		close(fd); // releases the flock
}
#లేకపోతే
Scoped_File_తాళం::Scoped_File_తాళం(స్థిరరాశి std::string &) {}
Scoped_File_తాళం::~Scoped_File_తాళం() {}
#అయితే_ముగింపు

గుర్తులేని get_process_id()
{
#ఒకవేళ defined(_WIN32)
	తిరిగి GetCurrentProcessId();
#లేదావేళ defined(__wasi__)
	తిరిగి 0;
#లేకపోతే
	తిరిగి getpid();
#అయితే_ముగింపు
}

YOSYS_NAMESPACE_చివర
