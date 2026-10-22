#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن <iostream>
#ضمن <string>
#ضمن <filesystem>
#ضمن <system_error>

#إذا !defined(WIN32)
#ضمن <dirent.h>
#ضمن <unistd.h>
#وإلا
#ضمن <io.h>
#جيڪڏهن_پورو

#إذا !defined(_WIN32) && !defined(__wasi__)
#ضمن <sys/file.h>
#ضمن <fcntl.h>
#ضمن <cerrno>
#جيڪڏهن_پورو

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

صحيح readsome(std::istream &f, حرف *s, صحيح n)
{
	صحيح rc = صحيح(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	إذا (rc == 0) {
		صحيح c = f.get();
		إذا (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	أرجع rc;
}

std::string ايندڙ_token(std::string &متن, ثابت حرف *sep, منطقي long_لڙيون)
{
	سائيز_t pos_begin = متن.ڳوليو_پهريون_not_جو(sep);

	إذا (pos_begin == std::string::npos)
		pos_begin = متن.سائيز();

	إذا (long_لڙيون && pos_begin != متن.سائيز() && متن[pos_begin] == '"') {
		std::string sep_string = sep;
		لكل (سائيز_t i = pos_begin+1; i < متن.سائيز(); i++) {
			إذا (متن[i] == '"' && (i+1 == متن.سائيز() || sep_string.ڳوليو(متن[i+1]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+1);
				أرجع token;
			}
			إذا (i+1 < متن.سائيز() && متن[i] == '"' && متن[i+1] == ';' && (i+2 == متن.سائيز() || sep_string.ڳوليو(متن[i+2]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+2);
				أرجع token + ";";
			}
		}
	}

	سائيز_t pos_پڇاڙي = متن.ڳوليو_پهريون_جو(sep, pos_begin);

	إذا (pos_پڇاڙي == std::string::npos)
		pos_پڇاڙي = متن.سائيز();

	std::string token = متن.substr(pos_begin, pos_پڇاڙي-pos_begin);
	متن = متن.substr(pos_پڇاڙي);
	أرجع token;
}

std::ويڪٽر<std::string> ورهايو_ٽوڪن(ثابت std::string &متن, ثابت حرف *sep)
{
	std::ويڪٽر<std::string> ٽوڪن;
	std::string current_token;
	لكل (حرف c : متن) {
		إذا (strchr(sep, c)) {
			إذا (!current_token.خالي_جايون()) {
				ٽوڪن.ڌڪو_پٺ(current_token);
				current_token.clear();
			}
		} وإلا
			current_token += c;
	}
	إذا (!current_token.خالي_جايون()) {
		ٽوڪن.ڌڪو_پٺ(current_token);
		current_token.clear();
	}
	أرجع ٽوڪن;
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
منطقي patmatch(ثابت حرف *نمونو, ثابت حرف *string)
{
	إذا (*نمونو == 0)
		أرجع *string == 0;

	إذا (*نمونو == '\\') {
		إذا (نمونو[1] == string[0] && patmatch(نمونو+2, string+1))
			أرجع سچ;
	}

	إذا (*نمونو == '?') {
		إذا (*string == 0)
			أرجع خطأ;
		أرجع patmatch(نمونو+1, string+1);
	}

	إذا (*نمونو == '*') {
		طالما (*string) {
			إذا (patmatch(نمونو+1, string++))
				أرجع سچ;
		}
		أرجع نمونو[1] == 0;
	}

	إذا (*نمونو == '[') {
		منطقي مليو_ميلاپ = خطأ;
		منطقي inverted_list = نمونو[1] == '!';
		ثابت حرف *p = نمونو + (inverted_list ? 1 : 0);

		طالما (*++p) {
			إذا (*p == ']') {
				إذا (مليو_ميلاپ != inverted_list && patmatch(p+1, string+1))
					أرجع سچ;
				اكسر;
			}

			إذا (*p == '\\') {
				إذا (*++p == *string)
					مليو_ميلاپ = سچ;
			} وإلا
			إذا (*p == *string)
				مليو_ميلاپ = سچ;
		}
	}

	إذا (*نمونو == *string)
		أرجع patmatch(نمونو+1, string+1);

	أرجع خطأ;
}

std::string get_بنياد_tmpdir()
{
	ساكن std::string tmpdir;

	إذا (!tmpdir.خالي_جايون()) {
		أرجع tmpdir;
	}

#إذا defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	إذا (!Get_عارضي_رستو_W(wtmppath.سائيز(), wtmppath.معلومات()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.ڳوليو(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wtmppath.معلومات(), wtmppath.سائيز(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wtmppath.معلومات(), wtmppath.سائيز(), utmppath.معلومات(), utmppath.سائيز(), عدم, عدم) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#وإلا
	حرف * var = std::getenv("TMPDIR");
	إذا (var && لڙي_ڊيگهه(var)!=0) {
		tmpdir.سونپيو(var);
		// We return the directory name without the trailing '/'
		طالما (!tmpdir.خالي_جايون() && (tmpdir.پٺ() == '/')) {
			tmpdir.ڪڍو_پٺ();
		}
	} وإلا {
		tmpdir.سونپيو("/tmp");
	}
#جيڪڏهن_پورو
	أرجع tmpdir;
}

std::string make_عارضي_file(std::string template_str)
{
	سائيز_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#إذا defined(__wasm)
	ساكن سائيز_t اشاريو = 0;
	template_str.replace(pos, 6, stringf("%06zu", اشاريو++));
#وإلاإذا defined(_WIN32)
#جيڪڏهن_وصف_نه_ٿيل YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.پڇاڙي(), '/', '\\');
#جيڪڏهن_پورو
	طالما (1) {
		لكل (صحيح i = 0; i < 6; i++) {
			ساكن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساكن uint32_t x = 314159265 ^ uint32_t(وقت_2(عدم));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.سائيز()];
		}
		إذا (_access(template_str.c_str(), 0) != 0)
			اكسر;
	}
#وإلا
	صحيح suffixlen = template_str.سائيز() - pos - 6;

	حرف *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	حرر(p);
#جيڪڏهن_پورو

	أرجع template_str;
}

std::string make_عارضي_dir(std::string template_str)
{
#إذا defined(_WIN32)
	template_str = make_عارضي_file(template_str);
	mkdir(template_str.c_str());
	أرجع template_str;
#وإلاإذا defined(__wasm)
	template_str = make_عارضي_file(template_str);
	mkdir(template_str.c_str(), 0777);
	أرجع template_str;
#وإلا
#  ifndef NDEBUG
	سائيز_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحيح suffixlen = template_str.سائيز() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	حرف *p = strdup(template_str.c_str());
	log_assert(p);
	حرف *res = mkdtemp(p);
	إذا (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	حرر(p);

	أرجع template_str;
#جيڪڏهن_پورو
}

منطقي check_is_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	بنية _stat info;
	خودڪار dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	إذا (dirname.پٺ() == '/' || dirname.پٺ() == '\\') {
		dirname_ = dirname.substr(0, dirname.ڊيگهائي() - 1);
	}

	إذا (_stat(dirname_.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_گهڻائي & _S_IFDIR) != 0;
#وإلا
	بنية stat info;
	إذا (stat(dirname.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_گهڻائي & S_IFDIR) != 0;
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل _WIN32
منطقي check_accessible(ثابت std::string& filename, منطقي)
{
	أرجع _access(filename.c_str(), 0) == 0;
}
#وإلا
منطقي check_accessible(ثابت std::string& filename, منطقي is_exec)
{
	أرجع access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#جيڪڏهن_پورو

منطقي check_file_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقي check_directory_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقي is_absolute_رستو(std::string filename)
{
#جيڪڏهن_وصف_ٿيل _WIN32
	أرجع filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#وإلا
	أرجع filename[0] == '/';
#جيڪڏهن_پورو
}

فراغ remove_directory(std::string dirname)
{
#جيڪڏهن_وصف_ٿيل _WIN32
	لاڳيتو_command(stringf("rmdir /s /q \"%s\"", dirname));
#وإلا
	بنية stat stbuf;
	بنية dirent **namelist;
	صحيح n = scandir(dirname.c_str(), &namelist, خالي_اشارو, alphasort);
	log_assert(n >= 0);
	لكل (صحيح i = 0; i < n; i++) {
		إذا (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string بفر = stringf("%s/%s", dirname, namelist[i]->d_name);
			إذا (!stat(بفر.c_str(), &stbuf) && S_ISREG(stbuf.st_گهڻائي)) {
				remove(بفر.c_str());
			} وإلا
				remove_directory(بفر);
		}
		حرر(namelist[i]);
	}
	حرر(namelist);
	rmdir(dirname.c_str());
#جيڪڏهن_پورو
}

منطقي create_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	صحيح ret = _mkdir(dirname.c_str());
#وإلا
	گهڻائي_t گهڻائي = 0755;
	صحيح ret = mkdir(dirname.c_str(), گهڻائي);
#جيڪڏهن_پورو
	إذا (ret == 0)
		أرجع سچ;

	بدل (errno)
	{
	حالة ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::سائيز_type pos = dirname.ڳوليو_آخري_جو('/');
			إذا (pos == std::string::npos)
#إذا defined(_WIN32)
				pos = dirname.ڳوليو_آخري_جو('\\');
			إذا (pos == std::string::npos)
#جيڪڏهن_پورو
				أرجع خطأ;
			إذا (!create_directory( dirname.substr(0, pos) ))
				أرجع خطأ;
		}
		// now, try to create again
#إذا defined(_WIN32)
		أرجع 0 == _mkdir(dirname.c_str());
#وإلا
		أرجع 0 == mkdir(dirname.c_str(), گهڻائي);
#جيڪڏهن_پورو

	حالة EEXIST:
		// done!
		أرجع check_directory_exists(dirname);

	افتراضي:
		أرجع خطأ;
	}
}

منطقي make_ذاتي_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32) || defined(__wasi__)
	إذا (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		أرجع خطأ;
	}
	أرجع سچ;
#وإلا
	إذا (mkdir(dirname.c_str(), 0700) == 0)
		أرجع سچ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحيح err = errno;
	std::string والد = std::filesystem::رستو_2(dirname).والد_رستو().string();

	إذا (err == ENOENT && !والد.خالي_جايون() && create_directory(والد)) {
		إذا (mkdir(dirname.c_str(), 0700) == 0)
			أرجع سچ;
		err = errno;
	}

	إذا (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		أرجع خطأ;
	}

	// Do not use a leftover that isn't ours
	بنية stat st;
	إذا (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_گهڻائي) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		أرجع خطأ;
	}
	// Repair the mode if something changed it
	گهڻائي_t گهڻائي = st.st_گهڻائي & 0777;
	إذا (((گهڻائي & S_IRWXU) != S_IRWXU || (گهڻائي & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		أرجع خطأ;
	}

	أرجع سچ;
#جيڪڏهن_پورو
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.خالي_جايون() && name.ڳوليو('/') == std::string::npos);
#إذا defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_بنياد_tmpdir() + "/" + name;
#وإلا
	std::string dirname = stringf("%s/%s-%u", get_بنياد_tmpdir().c_str(), name.c_str(), (بنانشان)geteuid());
#جيڪڏهن_پورو
	أرجع make_ذاتي_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string ٻاهريون;
	ٻاهريون.reserve(filename.سائيز());
	لكل (خودڪار c : filename)
	{
		إذا (c == ' ')
			ٻاهريون += "\\ ";
		وإلا
			ٻاهريون.ڌڪو_پٺ(c);
	}
	أرجع ٻاهريون;
}

فراغ append_globbed(std::ويڪٽر<std::string>& paths, std::string نمونو)
{
	rewrite_filename(نمونو);
	std::ويڪٽر<std::string> globbed = glob_filename(نمونو);
	copy(globbed.begin(), globbed.پڇاڙي(), پٺ_inserter(paths));
}

std::string name_from_file_رستو(std::string رستو_2) {
	أرجع std::filesystem::رستو_2(رستو_2).filename().string();
}

std::string absolute_رستو(ثابت std::string& رستو_2)
{
	std::error_ڪوڊ ec;
	std::filesystem::رستو_2 نتيجو = std::filesystem::absolute(رستو_2, ec);

	إذا (ec)
		أرجع "";

	نتيجو = std::filesystem::weakly_canonical(نتيجو, ec);

	إذا (ec)
		أرجع "";

	أرجع نتيجو.string();
}

// Includes OS_PATH_SEP at the end if present
std::string والد_from_file_رستو(std::string رستو_2) {
	خودڪار والد = std::filesystem::رستو_2(رستو_2).والد_رستو();
	إذا (والد.خالي_جايون()) {
		أرجع "";
	}
	// Add trailing separator to match original behavior
	std::string نتيجو = والد.string();
	إذا (!نتيجو.خالي_جايون() && نتيجو.پٺ() != std::filesystem::رستو_2::preferred_separator) {
		نتيجو += std::filesystem::رستو_2::preferred_separator;
	}
	أرجع نتيجو;
}

فراغ format_emit_unescaped(std::string &نتيجو, std::string_view fmt)
{
	نتيجو.reserve(نتيجو.سائيز() + fmt.سائيز());
	لكل (سائيز_t i = 0; i < fmt.سائيز(); ++i) {
		حرف ch = fmt[i];
		نتيجو.ڌڪو_پٺ(ch);
		إذا (ch == '%' && i + 1 < fmt.سائيز() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string نتيجو;
	format_emit_unescaped(نتيجو, fmt);
	أرجع نتيجو;
}

ساكن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string نتيجو = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	أرجع نتيجو;
}

ساكن صحيح spec_parameter_سائيز(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	بدل (spec[spec.سائيز() - 1]) {
	حالة 'd':
	حالة 'i':
	حالة 'o':
	حالة 'u':
	حالة 'x':
	حالة 'X':
		بدل (spec[spec.سائيز() - 2]) {
		حالة 'h':
			إذا (spec[spec.سائيز() - 3] == 'h')
				أرجع حجم(حرف);
			أرجع حجم(ننڍو);
		حالة 'l':
			إذا (spec[spec.سائيز() - 3] == 'l')
				أرجع حجم(ڊگهو ڊگهو);
			أرجع حجم(ڊگهو);
		حالة 'L':
		حالة 'q':
			أرجع حجم(ڊگهو ڊگهو);
		حالة 'j':
			أرجع حجم(intmax_t);
		حالة 'z':
		حالة 'Z':
			أرجع حجم(سائيز_t);
		حالة 't':
			أرجع حجم(ptrdiff_t);
		}
		أرجع حجم(صحيح);
	حالة 'e':
	حالة 'E':
	حالة 'f':
	حالة 'F':
	حالة 'g':
	حالة 'G':
	حالة 'a':
	حالة 'A':
		إذا (spec[spec.سائيز() - 2] == 'L')
			أرجع حجم(ڊگهو مزدوج);
		إذا (spec[spec.سائيز() - 2] == 'l' && spec[spec.سائيز() - 3] == 'l')
			أرجع حجم(ڊگهو مزدوج);
		أرجع حجم(مزدوج);
	حالة 'c':
		إذا (spec[spec.سائيز() - 2] == 'l') {
			أرجع حجم(wchar_t);
		}
		أرجع حجم(بنانشان حرف);
	حالة 'C':
		أرجع حجم(wchar_t);
	حالة 's':
	حالة 'p':
	حالة 'S':
	حالة 'n':
		أرجع حجم(فراغ *);
	حالة 'm':
		أرجع حجم(صحيح);
	افتراضي:
		أرجع -1;
	}
}

سانچو <قسم_نالو Arg>
ساكن فراغ format_emit_stringf(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	بدل (انگ_dynamic_ints) {
	حالة Dynamic_Int_ڳڻپ::NONE:
		نتيجو += string_view_stringf(spec, arg);
		أرجع;
	حالة Dynamic_Int_ڳڻپ::ONE:
		نتيجو += string_view_stringf(spec, dynamic_ints[0], arg);
		أرجع;
	حالة Dynamic_Int_ڳڻپ::TWO:
		نتيجو += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		أرجع;
	}
	YOSYS_ABORT("Internal error");
}

فراغ format_emit_long_long(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, ڊگهو ڊگهو arg)
{
	إذا (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتيجو += std::منزل_string(static_cast<صحيح>(arg));
		أرجع;
	}
	إذا (spec_parameter_سائيز(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, (صحيح)arg);
	} وإلا {
		format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, arg);
	}
}

فراغ format_emit_unsigned_long_long(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, بنانشان ڊگهو ڊگهو arg)
{
	إذا (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتيجو += std::منزل_string(static_cast<بنانشان صحيح>(arg));
		أرجع;
	}
	إذا (spec == "%c") {
		نتيجو += static_cast<حرف>(arg);
		أرجع;
	}
	إذا (spec_parameter_سائيز(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, (بنانشان صحيح)arg);
	} وإلا {
		format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, arg);
	}
}

فراغ format_emit_double(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, مزدوج arg)
{
	format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, arg);
}

فراغ format_emit_char_ptr(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, ثابت حرف *arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتيجو += arg;
		أرجع;
	}
	format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, arg);
}

فراغ format_emit_string(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, ثابت std::string &arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتيجو += arg;
		أرجع;
	}
	format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, arg.c_str());
}

فراغ format_emit_string_view(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, std::string_view arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		نتيجو += arg;
		أرجع;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, std::string(arg).c_str());
}

فراغ format_emit_void_ptr(std::string &نتيجو, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_ڳڻپ انگ_dynamic_ints, ثابت فراغ *arg)
{
	format_emit_stringf(نتيجو, spec, dynamic_ints, انگ_dynamic_ints, arg);
}

#إذا defined(_WIN32)
Scoped_File_تالو::Scoped_File_تالو(ثابت std::string &رستو_2)
{
	HANDLE h = CreateFileA(رستو_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			عدم, OPEN_ALWAYS, FILE_ATTRIBUTE_عام, عدم);
	إذا (h == INVALID_HANDLE_قدر) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", رستو_2.c_str());
		أرجع;
	}
	OVERLAPPED ov = {};
	إذا (!تالو_File_Ex(h, LOCKFILE_EXCLUSIVE_تالو, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", رستو_2.c_str());
		CloseHandle(h);
		أرجع;
	}
	handle = h;
}

Scoped_File_تالو::~Scoped_File_تالو()
{
	إذا (handle != خالي_اشارو) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وإلاإذا !defined(__wasi__)
Scoped_File_تالو::Scoped_File_تالو(ثابت std::string &رستو_2)
{
	fd = open(رستو_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	إذا (fd < 0)
		fd = open(رستو_2.c_str(), O_RDONLY | O_CLOEXEC);
	إذا (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", رستو_2.c_str());
		أرجع;
	}
	صحيح ret;
	طالما ((ret = flock(fd, تالو_EX)) != 0 && errno == EINTR);
	إذا (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", رستو_2.c_str());
}

Scoped_File_تالو::~Scoped_File_تالو()
{
	إذا (fd >= 0)
		close(fd); // releases the flock
}
#وإلا
Scoped_File_تالو::Scoped_File_تالو(ثابت std::string &) {}
Scoped_File_تالو::~Scoped_File_تالو() {}
#جيڪڏهن_پورو

بنانشان get_process_id()
{
#إذا defined(_WIN32)
	أرجع GetCurrentProcessId();
#وإلاإذا defined(__wasi__)
	أرجع 0;
#وإلا
	أرجع getpid();
#جيڪڏهن_پورو
}

YOSYS_NAMESPACE_پڇاڙي
