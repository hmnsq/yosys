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
#د_که_پای

#إذا !defined(_WIN32) && !defined(__wasi__)
#ضمن <sys/file.h>
#ضمن <fcntl.h>
#ضمن <cerrno>
#د_که_پای

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

std::string بل_token(std::string &متن, ثابت حرف *sep, منطقي long_مزي_ګانې)
{
	اندازه_t pos_begin = متن.پیدا_کړه_لومړی_not_د(sep);

	إذا (pos_begin == std::string::npos)
		pos_begin = متن.اندازه();

	إذا (long_مزي_ګانې && pos_begin != متن.اندازه() && متن[pos_begin] == '"') {
		std::string sep_string = sep;
		لكل (اندازه_t i = pos_begin+1; i < متن.اندازه(); i++) {
			إذا (متن[i] == '"' && (i+1 == متن.اندازه() || sep_string.پیدا_کړه(متن[i+1]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+1);
				أرجع token;
			}
			إذا (i+1 < متن.اندازه() && متن[i] == '"' && متن[i+1] == ';' && (i+2 == متن.اندازه() || sep_string.پیدا_کړه(متن[i+2]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+2);
				أرجع token + ";";
			}
		}
	}

	اندازه_t pos_پای_ټکی = متن.پیدا_کړه_لومړی_د(sep, pos_begin);

	إذا (pos_پای_ټکی == std::string::npos)
		pos_پای_ټکی = متن.اندازه();

	std::string token = متن.substr(pos_begin, pos_پای_ټکی-pos_begin);
	متن = متن.substr(pos_پای_ټکی);
	أرجع token;
}

std::ویکټور<std::string> وېشل_نښې(ثابت std::string &متن, ثابت حرف *sep)
{
	std::ویکټور<std::string> نښې;
	std::string current_token;
	لكل (حرف c : متن) {
		إذا (strchr(sep, c)) {
			إذا (!current_token.تشه()) {
				نښې.ټېل_شا(current_token);
				current_token.clear();
			}
		} وإلا
			current_token += c;
	}
	إذا (!current_token.تشه()) {
		نښې.ټېل_شا(current_token);
		current_token.clear();
	}
	أرجع نښې;
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
منطقي patmatch(ثابت حرف *نمونه, ثابت حرف *string)
{
	إذا (*نمونه == 0)
		أرجع *string == 0;

	إذا (*نمونه == '\\') {
		إذا (نمونه[1] == string[0] && patmatch(نمونه+2, string+1))
			أرجع ریښتیا;
	}

	إذا (*نمونه == '?') {
		إذا (*string == 0)
			أرجع خطأ;
		أرجع patmatch(نمونه+1, string+1);
	}

	إذا (*نمونه == '*') {
		طالما (*string) {
			إذا (patmatch(نمونه+1, string++))
				أرجع ریښتیا;
		}
		أرجع نمونه[1] == 0;
	}

	إذا (*نمونه == '[') {
		منطقي وموندل_سمون = خطأ;
		منطقي inverted_list = نمونه[1] == '!';
		ثابت حرف *p = نمونه + (inverted_list ? 1 : 0);

		طالما (*++p) {
			إذا (*p == ']') {
				إذا (وموندل_سمون != inverted_list && patmatch(p+1, string+1))
					أرجع ریښتیا;
				اكسر;
			}

			إذا (*p == '\\') {
				إذا (*++p == *string)
					وموندل_سمون = ریښتیا;
			} وإلا
			إذا (*p == *string)
				وموندل_سمون = ریښتیا;
		}
	}

	إذا (*نمونه == *string)
		أرجع patmatch(نمونه+1, string+1);

	أرجع خطأ;
}

std::string get_بنسټ_tmpdir()
{
	ساكن std::string tmpdir;

	إذا (!tmpdir.تشه()) {
		أرجع tmpdir;
	}

#إذا defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	إذا (!Get_لنډمهاله_لار_W(wtmppath.اندازه(), wtmppath.معلومات()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.پیدا_کړه(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wtmppath.معلومات(), wtmppath.اندازه(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wtmppath.معلومات(), wtmppath.اندازه(), utmppath.معلومات(), utmppath.اندازه(), عدم, عدم) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#وإلا
	حرف * var = std::getenv("TMPDIR");
	إذا (var && مزي_اوږدوالی(var)!=0) {
		tmpdir.ټاکل(var);
		// We return the directory name without the trailing '/'
		طالما (!tmpdir.تشه() && (tmpdir.شا() == '/')) {
			tmpdir.راوباسه_شا();
		}
	} وإلا {
		tmpdir.ټاکل("/tmp");
	}
#د_که_پای
	أرجع tmpdir;
}

std::string make_لنډمهاله_file(std::string template_str)
{
	اندازه_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#إذا defined(__wasm)
	ساكن اندازه_t شاخص = 0;
	template_str.replace(pos, 6, stringf("%06zu", شاخص++));
#وإلاإذا defined(_WIN32)
#که_نه_تعریف_شوی YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.پای_ټکی(), '/', '\\');
#د_که_پای
	طالما (1) {
		لكل (صحيح i = 0; i < 6; i++) {
			ساكن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساكن uint32_t x = 314159265 ^ uint32_t(وخت_2(عدم));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.اندازه()];
		}
		إذا (_access(template_str.c_str(), 0) != 0)
			اكسر;
	}
#وإلا
	صحيح suffixlen = template_str.اندازه() - pos - 6;

	حرف *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	حرر(p);
#د_که_پای

	أرجع template_str;
}

std::string make_لنډمهاله_dir(std::string template_str)
{
#إذا defined(_WIN32)
	template_str = make_لنډمهاله_file(template_str);
	mkdir(template_str.c_str());
	أرجع template_str;
#وإلاإذا defined(__wasm)
	template_str = make_لنډمهاله_file(template_str);
	mkdir(template_str.c_str(), 0777);
	أرجع template_str;
#وإلا
#  ifndef NDEBUG
	اندازه_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحيح suffixlen = template_str.اندازه() - pos - 6;
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
#د_که_پای
}

منطقي check_is_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	بنية _stat info;
	خپلکاری dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	إذا (dirname.شا() == '/' || dirname.شا() == '\\') {
		dirname_ = dirname.substr(0, dirname.اوږدتیا() - 1);
	}

	إذا (_stat(dirname_.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_ډېرپېښ & _S_IFDIR) != 0;
#وإلا
	بنية stat info;
	إذا (stat(dirname.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_ډېرپېښ & S_IFDIR) != 0;
#د_که_پای
}

#که_تعریف_شوی _WIN32
منطقي check_accessible(ثابت std::string& filename, منطقي)
{
	أرجع _access(filename.c_str(), 0) == 0;
}
#وإلا
منطقي check_accessible(ثابت std::string& filename, منطقي is_exec)
{
	أرجع access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#د_که_پای

منطقي check_file_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقي check_directory_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقي is_absolute_لار(std::string filename)
{
#که_تعریف_شوی _WIN32
	أرجع filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#وإلا
	أرجع filename[0] == '/';
#د_که_پای
}

فراغ remove_directory(std::string dirname)
{
#که_تعریف_شوی _WIN32
	پرلپسې_command(stringf("rmdir /s /q \"%s\"", dirname));
#وإلا
	بنية stat stbuf;
	بنية dirent **namelist;
	صحيح n = scandir(dirname.c_str(), &namelist, تش_اشاره, alphasort);
	log_assert(n >= 0);
	لكل (صحيح i = 0; i < n; i++) {
		إذا (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string بفر = stringf("%s/%s", dirname, namelist[i]->d_name);
			إذا (!stat(بفر.c_str(), &stbuf) && S_ISREG(stbuf.st_ډېرپېښ)) {
				remove(بفر.c_str());
			} وإلا
				remove_directory(بفر);
		}
		حرر(namelist[i]);
	}
	حرر(namelist);
	rmdir(dirname.c_str());
#د_که_پای
}

منطقي create_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	صحيح ret = _mkdir(dirname.c_str());
#وإلا
	ډېرپېښ_t ډېرپېښ = 0755;
	صحيح ret = mkdir(dirname.c_str(), ډېرپېښ);
#د_که_پای
	إذا (ret == 0)
		أرجع ریښتیا;

	بدل (errno)
	{
	حالة ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::اندازه_type pos = dirname.پیدا_کړه_وروستی_د('/');
			إذا (pos == std::string::npos)
#إذا defined(_WIN32)
				pos = dirname.پیدا_کړه_وروستی_د('\\');
			إذا (pos == std::string::npos)
#د_که_پای
				أرجع خطأ;
			إذا (!create_directory( dirname.substr(0, pos) ))
				أرجع خطأ;
		}
		// now, try to create again
#إذا defined(_WIN32)
		أرجع 0 == _mkdir(dirname.c_str());
#وإلا
		أرجع 0 == mkdir(dirname.c_str(), ډېرپېښ);
#د_که_پای

	حالة EEXIST:
		// done!
		أرجع check_directory_exists(dirname);

	افتراضي:
		أرجع خطأ;
	}
}

منطقي make_شخصي_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32) || defined(__wasi__)
	إذا (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		أرجع خطأ;
	}
	أرجع ریښتیا;
#وإلا
	إذا (mkdir(dirname.c_str(), 0700) == 0)
		أرجع ریښتیا;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحيح err = errno;
	std::string پلار = std::filesystem::لار_2(dirname).پلار_لار().string();

	إذا (err == ENOENT && !پلار.تشه() && create_directory(پلار)) {
		إذا (mkdir(dirname.c_str(), 0700) == 0)
			أرجع ریښتیا;
		err = errno;
	}

	إذا (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		أرجع خطأ;
	}

	// Do not use a leftover that isn't ours
	بنية stat st;
	إذا (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_ډېرپېښ) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		أرجع خطأ;
	}
	// Repair the mode if something changed it
	ډېرپېښ_t ډېرپېښ = st.st_ډېرپېښ & 0777;
	إذا (((ډېرپېښ & S_IRWXU) != S_IRWXU || (ډېرپېښ & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		أرجع خطأ;
	}

	أرجع ریښتیا;
#د_که_پای
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.تشه() && name.پیدا_کړه('/') == std::string::npos);
#إذا defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_بنسټ_tmpdir() + "/" + name;
#وإلا
	std::string dirname = stringf("%s/%s-%u", get_بنسټ_tmpdir().c_str(), name.c_str(), (بینښه)geteuid());
#د_که_پای
	أرجع make_شخصي_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string بهر;
	بهر.reserve(filename.اندازه());
	لكل (خپلکاری c : filename)
	{
		إذا (c == ' ')
			بهر += "\\ ";
		وإلا
			بهر.ټېل_شا(c);
	}
	أرجع بهر;
}

فراغ append_globbed(std::ویکټور<std::string>& paths, std::string نمونه)
{
	rewrite_filename(نمونه);
	std::ویکټور<std::string> globbed = glob_filename(نمونه);
	copy(globbed.begin(), globbed.پای_ټکی(), شا_inserter(paths));
}

std::string name_from_file_لار(std::string لار_2) {
	أرجع std::filesystem::لار_2(لار_2).filename().string();
}

std::string absolute_لار(ثابت std::string& لار_2)
{
	std::error_کوډ ec;
	std::filesystem::لار_2 پایله = std::filesystem::absolute(لار_2, ec);

	إذا (ec)
		أرجع "";

	پایله = std::filesystem::weakly_canonical(پایله, ec);

	إذا (ec)
		أرجع "";

	أرجع پایله.string();
}

// Includes OS_PATH_SEP at the end if present
std::string پلار_from_file_لار(std::string لار_2) {
	خپلکاری پلار = std::filesystem::لار_2(لار_2).پلار_لار();
	إذا (پلار.تشه()) {
		أرجع "";
	}
	// Add trailing separator to match original behavior
	std::string پایله = پلار.string();
	إذا (!پایله.تشه() && پایله.شا() != std::filesystem::لار_2::preferred_separator) {
		پایله += std::filesystem::لار_2::preferred_separator;
	}
	أرجع پایله;
}

فراغ format_emit_unescaped(std::string &پایله, std::string_view fmt)
{
	پایله.reserve(پایله.اندازه() + fmt.اندازه());
	لكل (اندازه_t i = 0; i < fmt.اندازه(); ++i) {
		حرف ch = fmt[i];
		پایله.ټېل_شا(ch);
		إذا (ch == '%' && i + 1 < fmt.اندازه() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string پایله;
	format_emit_unescaped(پایله, fmt);
	أرجع پایله;
}

ساكن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string پایله = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	أرجع پایله;
}

ساكن صحيح spec_parameter_اندازه(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	بدل (spec[spec.اندازه() - 1]) {
	حالة 'd':
	حالة 'i':
	حالة 'o':
	حالة 'u':
	حالة 'x':
	حالة 'X':
		بدل (spec[spec.اندازه() - 2]) {
		حالة 'h':
			إذا (spec[spec.اندازه() - 3] == 'h')
				أرجع حجم(حرف);
			أرجع حجم(لنډ);
		حالة 'l':
			إذا (spec[spec.اندازه() - 3] == 'l')
				أرجع حجم(اوږد اوږد);
			أرجع حجم(اوږد);
		حالة 'L':
		حالة 'q':
			أرجع حجم(اوږد اوږد);
		حالة 'j':
			أرجع حجم(intmax_t);
		حالة 'z':
		حالة 'Z':
			أرجع حجم(اندازه_t);
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
		إذا (spec[spec.اندازه() - 2] == 'L')
			أرجع حجم(اوږد مزدوج);
		إذا (spec[spec.اندازه() - 2] == 'l' && spec[spec.اندازه() - 3] == 'l')
			أرجع حجم(اوږد مزدوج);
		أرجع حجم(مزدوج);
	حالة 'c':
		إذا (spec[spec.اندازه() - 2] == 'l') {
			أرجع حجم(wchar_t);
		}
		أرجع حجم(بینښه حرف);
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

کينډۍ <ډول_نوم Arg>
ساكن فراغ format_emit_stringf(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	بدل (عدد_dynamic_ints) {
	حالة Dynamic_Int_شمېر::NONE:
		پایله += string_view_stringf(spec, arg);
		أرجع;
	حالة Dynamic_Int_شمېر::ONE:
		پایله += string_view_stringf(spec, dynamic_ints[0], arg);
		أرجع;
	حالة Dynamic_Int_شمېر::TWO:
		پایله += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		أرجع;
	}
	YOSYS_ABORT("Internal error");
}

فراغ format_emit_long_long(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, اوږد اوږد arg)
{
	إذا (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		پایله += std::منزل_string(static_cast<صحيح>(arg));
		أرجع;
	}
	إذا (spec_parameter_اندازه(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, (صحيح)arg);
	} وإلا {
		format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, arg);
	}
}

فراغ format_emit_unsigned_long_long(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, بینښه اوږد اوږد arg)
{
	إذا (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		پایله += std::منزل_string(static_cast<بینښه صحيح>(arg));
		أرجع;
	}
	إذا (spec == "%c") {
		پایله += static_cast<حرف>(arg);
		أرجع;
	}
	إذا (spec_parameter_اندازه(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, (بینښه صحيح)arg);
	} وإلا {
		format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, arg);
	}
}

فراغ format_emit_double(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, مزدوج arg)
{
	format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

فراغ format_emit_char_ptr(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, ثابت حرف *arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		پایله += arg;
		أرجع;
	}
	format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

فراغ format_emit_string(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, ثابت std::string &arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		پایله += arg;
		أرجع;
	}
	format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, arg.c_str());
}

فراغ format_emit_string_view(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, std::string_view arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		پایله += arg;
		أرجع;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, std::string(arg).c_str());
}

فراغ format_emit_void_ptr(std::string &پایله, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمېر عدد_dynamic_ints, ثابت فراغ *arg)
{
	format_emit_stringf(پایله, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

#إذا defined(_WIN32)
Scoped_File_قلف::Scoped_File_قلف(ثابت std::string &لار_2)
{
	HANDLE h = CreateFileA(لار_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			عدم, OPEN_ALWAYS, FILE_ATTRIBUTE_نورمال, عدم);
	إذا (h == INVALID_HANDLE_ارزښت) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", لار_2.c_str());
		أرجع;
	}
	OVERLAPPED ov = {};
	إذا (!قلف_File_Ex(h, LOCKFILE_EXCLUSIVE_قلف, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", لار_2.c_str());
		CloseHandle(h);
		أرجع;
	}
	handle = h;
}

Scoped_File_قلف::~Scoped_File_قلف()
{
	إذا (handle != تش_اشاره) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وإلاإذا !defined(__wasi__)
Scoped_File_قلف::Scoped_File_قلف(ثابت std::string &لار_2)
{
	fd = open(لار_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	إذا (fd < 0)
		fd = open(لار_2.c_str(), O_RDONLY | O_CLOEXEC);
	إذا (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", لار_2.c_str());
		أرجع;
	}
	صحيح ret;
	طالما ((ret = flock(fd, قلف_EX)) != 0 && errno == EINTR);
	إذا (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", لار_2.c_str());
}

Scoped_File_قلف::~Scoped_File_قلف()
{
	إذا (fd >= 0)
		close(fd); // releases the flock
}
#وإلا
Scoped_File_قلف::Scoped_File_قلف(ثابت std::string &) {}
Scoped_File_قلف::~Scoped_File_قلف() {}
#د_که_پای

بینښه get_process_id()
{
#إذا defined(_WIN32)
	أرجع GetCurrentProcessId();
#وإلاإذا defined(__wasi__)
	أرجع 0;
#وإلا
	أرجع getpid();
#د_که_پای
}

YOSYS_NAMESPACE_پای_ټکی
