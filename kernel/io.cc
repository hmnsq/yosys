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
#پایان_اگر

#إذا !defined(_WIN32) && !defined(__wasi__)
#ضمن <sys/file.h>
#ضمن <fcntl.h>
#ضمن <cerrno>
#پایان_اگر

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

std::string بعدی_token(std::string &متن, ثابت حرف *sep, منطقي long_رشتهها)
{
	اندازه_t pos_begin = متن.پیدا_کن_اولی_not_از(sep);

	إذا (pos_begin == std::string::npos)
		pos_begin = متن.اندازه();

	إذا (long_رشتهها && pos_begin != متن.اندازه() && متن[pos_begin] == '"') {
		std::string sep_string = sep;
		لكل (اندازه_t i = pos_begin+1; i < متن.اندازه(); i++) {
			إذا (متن[i] == '"' && (i+1 == متن.اندازه() || sep_string.پیدا_کن(متن[i+1]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+1);
				أرجع token;
			}
			إذا (i+1 < متن.اندازه() && متن[i] == '"' && متن[i+1] == ';' && (i+2 == متن.اندازه() || sep_string.پیدا_کن(متن[i+2]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+2);
				أرجع token + ";";
			}
		}
	}

	اندازه_t pos_انتها = متن.پیدا_کن_اولی_از(sep, pos_begin);

	إذا (pos_انتها == std::string::npos)
		pos_انتها = متن.اندازه();

	std::string token = متن.substr(pos_begin, pos_انتها-pos_begin);
	متن = متن.substr(pos_انتها);
	أرجع token;
}

std::وکتور<std::string> تقسیم_نشانهها(ثابت std::string &متن, ثابت حرف *sep)
{
	std::وکتور<std::string> نشانهها;
	std::string current_token;
	لكل (حرف c : متن) {
		إذا (strchr(sep, c)) {
			إذا (!current_token.خالی()) {
				نشانهها.افزودن_پشت(current_token);
				current_token.clear();
			}
		} وإلا
			current_token += c;
	}
	إذا (!current_token.خالی()) {
		نشانهها.افزودن_پشت(current_token);
		current_token.clear();
	}
	أرجع نشانهها;
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
منطقي patmatch(ثابت حرف *الگو, ثابت حرف *string)
{
	إذا (*الگو == 0)
		أرجع *string == 0;

	إذا (*الگو == '\\') {
		إذا (الگو[1] == string[0] && patmatch(الگو+2, string+1))
			أرجع درست;
	}

	إذا (*الگو == '?') {
		إذا (*string == 0)
			أرجع خطأ;
		أرجع patmatch(الگو+1, string+1);
	}

	إذا (*الگو == '*') {
		طالما (*string) {
			إذا (patmatch(الگو+1, string++))
				أرجع درست;
		}
		أرجع الگو[1] == 0;
	}

	إذا (*الگو == '[') {
		منطقي یافت_تطابق = خطأ;
		منطقي inverted_list = الگو[1] == '!';
		ثابت حرف *p = الگو + (inverted_list ? 1 : 0);

		طالما (*++p) {
			إذا (*p == ']') {
				إذا (یافت_تطابق != inverted_list && patmatch(p+1, string+1))
					أرجع درست;
				اكسر;
			}

			إذا (*p == '\\') {
				إذا (*++p == *string)
					یافت_تطابق = درست;
			} وإلا
			إذا (*p == *string)
				یافت_تطابق = درست;
		}
	}

	إذا (*الگو == *string)
		أرجع patmatch(الگو+1, string+1);

	أرجع خطأ;
}

std::string get_پایه_tmpdir()
{
	ساكن std::string tmpdir;

	إذا (!tmpdir.خالی()) {
		أرجع tmpdir;
	}

#إذا defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	إذا (!Get_موقت_مسیر_W(wtmppath.اندازه(), wtmppath.داده()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.پیدا_کن(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wtmppath.داده(), wtmppath.اندازه(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wtmppath.داده(), wtmppath.اندازه(), utmppath.داده(), utmppath.اندازه(), عدم, عدم) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#وإلا
	حرف * var = std::getenv("TMPDIR");
	إذا (var && طول_رشته(var)!=0) {
		tmpdir.تخصیص_بده(var);
		// We return the directory name without the trailing '/'
		طالما (!tmpdir.خالی() && (tmpdir.پشت() == '/')) {
			tmpdir.برداشت_پشت();
		}
	} وإلا {
		tmpdir.تخصیص_بده("/tmp");
	}
#پایان_اگر
	أرجع tmpdir;
}

std::string make_موقت_file(std::string template_str)
{
	اندازه_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#إذا defined(__wasm)
	ساكن اندازه_t شاخص = 0;
	template_str.replace(pos, 6, stringf("%06zu", شاخص++));
#وإلاإذا defined(_WIN32)
#اگر_تعریف_نشده YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.انتها(), '/', '\\');
#پایان_اگر
	طالما (1) {
		لكل (صحيح i = 0; i < 6; i++) {
			ساكن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساكن uint32_t x = 314159265 ^ uint32_t(زمان_2(عدم));
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
#پایان_اگر

	أرجع template_str;
}

std::string make_موقت_dir(std::string template_str)
{
#إذا defined(_WIN32)
	template_str = make_موقت_file(template_str);
	mkdir(template_str.c_str());
	أرجع template_str;
#وإلاإذا defined(__wasm)
	template_str = make_موقت_file(template_str);
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
#پایان_اگر
}

منطقي check_is_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	بنية _stat info;
	خودکار dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	إذا (dirname.پشت() == '/' || dirname.پشت() == '\\') {
		dirname_ = dirname.substr(0, dirname.درازی() - 1);
	}

	إذا (_stat(dirname_.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_نما & _S_IFDIR) != 0;
#وإلا
	بنية stat info;
	إذا (stat(dirname.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_نما & S_IFDIR) != 0;
#پایان_اگر
}

#اگر_تعریف_شده _WIN32
منطقي check_accessible(ثابت std::string& filename, منطقي)
{
	أرجع _access(filename.c_str(), 0) == 0;
}
#وإلا
منطقي check_accessible(ثابت std::string& filename, منطقي is_exec)
{
	أرجع access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#پایان_اگر

منطقي check_file_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقي check_directory_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقي is_absolute_مسیر(std::string filename)
{
#اگر_تعریف_شده _WIN32
	أرجع filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#وإلا
	أرجع filename[0] == '/';
#پایان_اگر
}

فراغ remove_directory(std::string dirname)
{
#اگر_تعریف_شده _WIN32
	پیاپی_command(stringf("rmdir /s /q \"%s\"", dirname));
#وإلا
	بنية stat stbuf;
	بنية dirent **namelist;
	صحيح n = scandir(dirname.c_str(), &namelist, اشاره_تهی, alphasort);
	log_assert(n >= 0);
	لكل (صحيح i = 0; i < n; i++) {
		إذا (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string میانگیر = stringf("%s/%s", dirname, namelist[i]->d_name);
			إذا (!stat(میانگیر.c_str(), &stbuf) && S_ISREG(stbuf.st_نما)) {
				remove(میانگیر.c_str());
			} وإلا
				remove_directory(میانگیر);
		}
		حرر(namelist[i]);
	}
	حرر(namelist);
	rmdir(dirname.c_str());
#پایان_اگر
}

منطقي create_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	صحيح ret = _mkdir(dirname.c_str());
#وإلا
	نما_t نما = 0755;
	صحيح ret = mkdir(dirname.c_str(), نما);
#پایان_اگر
	إذا (ret == 0)
		أرجع درست;

	بدل (errno)
	{
	حالة ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::اندازه_type pos = dirname.پیدا_کن_آخرین_از('/');
			إذا (pos == std::string::npos)
#إذا defined(_WIN32)
				pos = dirname.پیدا_کن_آخرین_از('\\');
			إذا (pos == std::string::npos)
#پایان_اگر
				أرجع خطأ;
			إذا (!create_directory( dirname.substr(0, pos) ))
				أرجع خطأ;
		}
		// now, try to create again
#إذا defined(_WIN32)
		أرجع 0 == _mkdir(dirname.c_str());
#وإلا
		أرجع 0 == mkdir(dirname.c_str(), نما);
#پایان_اگر

	حالة EEXIST:
		// done!
		أرجع check_directory_exists(dirname);

	افتراضي:
		أرجع خطأ;
	}
}

منطقي make_خصوصی_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32) || defined(__wasi__)
	إذا (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		أرجع خطأ;
	}
	أرجع درست;
#وإلا
	إذا (mkdir(dirname.c_str(), 0700) == 0)
		أرجع درست;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحيح err = errno;
	std::string والد = std::filesystem::مسیر_2(dirname).والد_مسیر().string();

	إذا (err == ENOENT && !والد.خالی() && create_directory(والد)) {
		إذا (mkdir(dirname.c_str(), 0700) == 0)
			أرجع درست;
		err = errno;
	}

	إذا (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		أرجع خطأ;
	}

	// Do not use a leftover that isn't ours
	بنية stat st;
	إذا (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_نما) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		أرجع خطأ;
	}
	// Repair the mode if something changed it
	نما_t نما = st.st_نما & 0777;
	إذا (((نما & S_IRWXU) != S_IRWXU || (نما & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		أرجع خطأ;
	}

	أرجع درست;
#پایان_اگر
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.خالی() && name.پیدا_کن('/') == std::string::npos);
#إذا defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_پایه_tmpdir() + "/" + name;
#وإلا
	std::string dirname = stringf("%s/%s-%u", get_پایه_tmpdir().c_str(), name.c_str(), (بیعلامت)geteuid());
#پایان_اگر
	أرجع make_خصوصی_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string بیرون;
	بیرون.reserve(filename.اندازه());
	لكل (خودکار c : filename)
	{
		إذا (c == ' ')
			بیرون += "\\ ";
		وإلا
			بیرون.افزودن_پشت(c);
	}
	أرجع بیرون;
}

فراغ append_globbed(std::وکتور<std::string>& paths, std::string الگو)
{
	rewrite_filename(الگو);
	std::وکتور<std::string> globbed = glob_filename(الگو);
	copy(globbed.begin(), globbed.انتها(), پشت_inserter(paths));
}

std::string name_from_file_مسیر(std::string مسیر_2) {
	أرجع std::filesystem::مسیر_2(مسیر_2).filename().string();
}

std::string absolute_مسیر(ثابت std::string& مسیر_2)
{
	std::error_کد ec;
	std::filesystem::مسیر_2 نتیجه = std::filesystem::absolute(مسیر_2, ec);

	إذا (ec)
		أرجع "";

	نتیجه = std::filesystem::weakly_canonical(نتیجه, ec);

	إذا (ec)
		أرجع "";

	أرجع نتیجه.string();
}

// Includes OS_PATH_SEP at the end if present
std::string والد_from_file_مسیر(std::string مسیر_2) {
	خودکار والد = std::filesystem::مسیر_2(مسیر_2).والد_مسیر();
	إذا (والد.خالی()) {
		أرجع "";
	}
	// Add trailing separator to match original behavior
	std::string نتیجه = والد.string();
	إذا (!نتیجه.خالی() && نتیجه.پشت() != std::filesystem::مسیر_2::preferred_separator) {
		نتیجه += std::filesystem::مسیر_2::preferred_separator;
	}
	أرجع نتیجه;
}

فراغ format_emit_unescaped(std::string &نتیجه, std::string_view fmt)
{
	نتیجه.reserve(نتیجه.اندازه() + fmt.اندازه());
	لكل (اندازه_t i = 0; i < fmt.اندازه(); ++i) {
		حرف ch = fmt[i];
		نتیجه.افزودن_پشت(ch);
		إذا (ch == '%' && i + 1 < fmt.اندازه() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string نتیجه;
	format_emit_unescaped(نتیجه, fmt);
	أرجع نتیجه;
}

ساكن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string نتیجه = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	أرجع نتیجه;
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
			أرجع حجم(کوتاه);
		حالة 'l':
			إذا (spec[spec.اندازه() - 3] == 'l')
				أرجع حجم(دراز دراز);
			أرجع حجم(دراز);
		حالة 'L':
		حالة 'q':
			أرجع حجم(دراز دراز);
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
			أرجع حجم(دراز مزدوج);
		إذا (spec[spec.اندازه() - 2] == 'l' && spec[spec.اندازه() - 3] == 'l')
			أرجع حجم(دراز مزدوج);
		أرجع حجم(مزدوج);
	حالة 'c':
		إذا (spec[spec.اندازه() - 2] == 'l') {
			أرجع حجم(wchar_t);
		}
		أرجع حجم(بیعلامت حرف);
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

قالب <نام_نوع Arg>
ساكن فراغ format_emit_stringf(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	بدل (رقم_dynamic_ints) {
	حالة Dynamic_Int_شمار::NONE:
		نتیجه += string_view_stringf(spec, arg);
		أرجع;
	حالة Dynamic_Int_شمار::ONE:
		نتیجه += string_view_stringf(spec, dynamic_ints[0], arg);
		أرجع;
	حالة Dynamic_Int_شمار::TWO:
		نتیجه += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		أرجع;
	}
	YOSYS_ABORT("Internal error");
}

فراغ format_emit_long_long(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, دراز دراز arg)
{
	إذا (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجه += std::مقصد_string(static_cast<صحيح>(arg));
		أرجع;
	}
	إذا (spec_parameter_اندازه(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, (صحيح)arg);
	} وإلا {
		format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, arg);
	}
}

فراغ format_emit_unsigned_long_long(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, بیعلامت دراز دراز arg)
{
	إذا (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجه += std::مقصد_string(static_cast<بیعلامت صحيح>(arg));
		أرجع;
	}
	إذا (spec == "%c") {
		نتیجه += static_cast<حرف>(arg);
		أرجع;
	}
	إذا (spec_parameter_اندازه(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, (بیعلامت صحيح)arg);
	} وإلا {
		format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, arg);
	}
}

فراغ format_emit_double(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, مزدوج arg)
{
	format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, arg);
}

فراغ format_emit_char_ptr(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت حرف *arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجه += arg;
		أرجع;
	}
	format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, arg);
}

فراغ format_emit_string(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت std::string &arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجه += arg;
		أرجع;
	}
	format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, arg.c_str());
}

فراغ format_emit_string_view(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, std::string_view arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		نتیجه += arg;
		أرجع;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, std::string(arg).c_str());
}

فراغ format_emit_void_ptr(std::string &نتیجه, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_شمار رقم_dynamic_ints, ثابت فراغ *arg)
{
	format_emit_stringf(نتیجه, spec, dynamic_ints, رقم_dynamic_ints, arg);
}

#إذا defined(_WIN32)
Scoped_File_قفل::Scoped_File_قفل(ثابت std::string &مسیر_2)
{
	HANDLE h = CreateFileA(مسیر_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			عدم, OPEN_ALWAYS, FILE_ATTRIBUTE_نرمال, عدم);
	إذا (h == INVALID_HANDLE_مقدار) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", مسیر_2.c_str());
		أرجع;
	}
	OVERLAPPED ov = {};
	إذا (!قفل_File_Ex(h, LOCKFILE_EXCLUSIVE_قفل, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", مسیر_2.c_str());
		CloseHandle(h);
		أرجع;
	}
	handle = h;
}

Scoped_File_قفل::~Scoped_File_قفل()
{
	إذا (handle != اشاره_تهی) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وإلاإذا !defined(__wasi__)
Scoped_File_قفل::Scoped_File_قفل(ثابت std::string &مسیر_2)
{
	fd = open(مسیر_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	إذا (fd < 0)
		fd = open(مسیر_2.c_str(), O_RDONLY | O_CLOEXEC);
	إذا (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", مسیر_2.c_str());
		أرجع;
	}
	صحيح ret;
	طالما ((ret = flock(fd, قفل_EX)) != 0 && errno == EINTR);
	إذا (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", مسیر_2.c_str());
}

Scoped_File_قفل::~Scoped_File_قفل()
{
	إذا (fd >= 0)
		close(fd); // releases the flock
}
#وإلا
Scoped_File_قفل::Scoped_File_قفل(ثابت std::string &) {}
Scoped_File_قفل::~Scoped_File_قفل() {}
#پایان_اگر

بیعلامت get_process_id()
{
#إذا defined(_WIN32)
	أرجع GetCurrentProcessId();
#وإلاإذا defined(__wasi__)
	أرجع 0;
#وإلا
	أرجع getpid();
#پایان_اگر
}

YOSYS_NAMESPACE_انتها
