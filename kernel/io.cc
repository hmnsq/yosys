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
#جے_انت

#إذا !defined(_WIN32) && !defined(__wasi__)
#ضمن <sys/file.h>
#ضمن <fcntl.h>
#ضمن <cerrno>
#جے_انت

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

std::string اگلا_token(std::string &متن, ثابت حرف *sep, منطقي long_لڑیاں)
{
	ناپ_t pos_begin = متن.لبھو_پہلا_not_دا(sep);

	إذا (pos_begin == std::string::npos)
		pos_begin = متن.ناپ();

	إذا (long_لڑیاں && pos_begin != متن.ناپ() && متن[pos_begin] == '"') {
		std::string sep_string = sep;
		لكل (ناپ_t i = pos_begin+1; i < متن.ناپ(); i++) {
			إذا (متن[i] == '"' && (i+1 == متن.ناپ() || sep_string.لبھو(متن[i+1]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+1);
				أرجع token;
			}
			إذا (i+1 < متن.ناپ() && متن[i] == '"' && متن[i+1] == ';' && (i+2 == متن.ناپ() || sep_string.لبھو(متن[i+2]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+2);
				أرجع token + ";";
			}
		}
	}

	ناپ_t pos_سرا = متن.لبھو_پہلا_دا(sep, pos_begin);

	إذا (pos_سرا == std::string::npos)
		pos_سرا = متن.ناپ();

	std::string token = متن.substr(pos_begin, pos_سرا-pos_begin);
	متن = متن.substr(pos_سرا);
	أرجع token;
}

std::سمتیہ<std::string> ونڈو_ٹوکن(ثابت std::string &متن, ثابت حرف *sep)
{
	std::سمتیہ<std::string> ٹوکن;
	std::string current_token;
	لكل (حرف c : متن) {
		إذا (strchr(sep, c)) {
			إذا (!current_token.خالی_تھاواں()) {
				ٹوکن.ڈالو_پٹھ(current_token);
				current_token.clear();
			}
		} وإلا
			current_token += c;
	}
	إذا (!current_token.خالی_تھاواں()) {
		ٹوکن.ڈالو_پٹھ(current_token);
		current_token.clear();
	}
	أرجع ٹوکن;
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
منطقي patmatch(ثابت حرف *نمونہ, ثابت حرف *string)
{
	إذا (*نمونہ == 0)
		أرجع *string == 0;

	إذا (*نمونہ == '\\') {
		إذا (نمونہ[1] == string[0] && patmatch(نمونہ+2, string+1))
			أرجع سچ;
	}

	إذا (*نمونہ == '?') {
		إذا (*string == 0)
			أرجع خطأ;
		أرجع patmatch(نمونہ+1, string+1);
	}

	إذا (*نمونہ == '*') {
		طالما (*string) {
			إذا (patmatch(نمونہ+1, string++))
				أرجع سچ;
		}
		أرجع نمونہ[1] == 0;
	}

	إذا (*نمونہ == '[') {
		منطقي لبھیا_میل = خطأ;
		منطقي inverted_list = نمونہ[1] == '!';
		ثابت حرف *p = نمونہ + (inverted_list ? 1 : 0);

		طالما (*++p) {
			إذا (*p == ']') {
				إذا (لبھیا_میل != inverted_list && patmatch(p+1, string+1))
					أرجع سچ;
				اكسر;
			}

			إذا (*p == '\\') {
				إذا (*++p == *string)
					لبھیا_میل = سچ;
			} وإلا
			إذا (*p == *string)
				لبھیا_میل = سچ;
		}
	}

	إذا (*نمونہ == *string)
		أرجع patmatch(نمونہ+1, string+1);

	أرجع خطأ;
}

std::string get_ادھار_tmpdir()
{
	ساكن std::string tmpdir;

	إذا (!tmpdir.خالی_تھاواں()) {
		أرجع tmpdir;
	}

#إذا defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	إذا (!Get_عارضی_رستہ_W(wtmppath.ناپ(), wtmppath.انکڑے()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.لبھو(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wtmppath.انکڑے(), wtmppath.ناپ(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wtmppath.انکڑے(), wtmppath.ناپ(), utmppath.انکڑے(), utmppath.ناپ(), عدم, عدم) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#وإلا
	حرف * var = std::getenv("TMPDIR");
	إذا (var && لڑی_لمبائی(var)!=0) {
		tmpdir.سونپو(var);
		// We return the directory name without the trailing '/'
		طالما (!tmpdir.خالی_تھاواں() && (tmpdir.پٹھ() == '/')) {
			tmpdir.نکالو_پٹھ();
		}
	} وإلا {
		tmpdir.سونپو("/tmp");
	}
#جے_انت
	أرجع tmpdir;
}

std::string make_عارضی_file(std::string template_str)
{
	ناپ_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#إذا defined(__wasm)
	ساكن ناپ_t اشاریہ = 0;
	template_str.replace(pos, 6, stringf("%06zu", اشاریہ++));
#وإلاإذا defined(_WIN32)
#جے_بنا_تعریف YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.سرا(), '/', '\\');
#جے_انت
	طالما (1) {
		لكل (صحيح i = 0; i < 6; i++) {
			ساكن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساكن uint32_t x = 314159265 ^ uint32_t(ویلا_2(عدم));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.ناپ()];
		}
		إذا (_access(template_str.c_str(), 0) != 0)
			اكسر;
	}
#وإلا
	صحيح suffixlen = template_str.ناپ() - pos - 6;

	حرف *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	حرر(p);
#جے_انت

	أرجع template_str;
}

std::string make_عارضی_dir(std::string template_str)
{
#إذا defined(_WIN32)
	template_str = make_عارضی_file(template_str);
	mkdir(template_str.c_str());
	أرجع template_str;
#وإلاإذا defined(__wasm)
	template_str = make_عارضی_file(template_str);
	mkdir(template_str.c_str(), 0777);
	أرجع template_str;
#وإلا
#  ifndef NDEBUG
	ناپ_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحيح suffixlen = template_str.ناپ() - pos - 6;
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
#جے_انت
}

منطقي check_is_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	بنية _stat info;
	خودکار dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	إذا (dirname.پٹھ() == '/' || dirname.پٹھ() == '\\') {
		dirname_ = dirname.substr(0, dirname.لمائی() - 1);
	}

	إذا (_stat(dirname_.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_منوال & _S_IFDIR) != 0;
#وإلا
	بنية stat info;
	إذا (stat(dirname.c_str(), &info) != 0)
	{
		أرجع خطأ;
	}
	أرجع (info.st_منوال & S_IFDIR) != 0;
#جے_انت
}

#جے_تعریف_شدہ _WIN32
منطقي check_accessible(ثابت std::string& filename, منطقي)
{
	أرجع _access(filename.c_str(), 0) == 0;
}
#وإلا
منطقي check_accessible(ثابت std::string& filename, منطقي is_exec)
{
	أرجع access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#جے_انت

منطقي check_file_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقي check_directory_exists(ثابت std::string& filename, منطقي is_exec)
{
	أرجع check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقي is_absolute_رستہ(std::string filename)
{
#جے_تعریف_شدہ _WIN32
	أرجع filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#وإلا
	أرجع filename[0] == '/';
#جے_انت
}

فراغ remove_directory(std::string dirname)
{
#جے_تعریف_شدہ _WIN32
	لگاتار_command(stringf("rmdir /s /q \"%s\"", dirname));
#وإلا
	بنية stat stbuf;
	بنية dirent **namelist;
	صحيح n = scandir(dirname.c_str(), &namelist, خالی_سوچک, alphasort);
	log_assert(n >= 0);
	لكل (صحيح i = 0; i < n; i++) {
		إذا (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string بفر = stringf("%s/%s", dirname, namelist[i]->d_name);
			إذا (!stat(بفر.c_str(), &stbuf) && S_ISREG(stbuf.st_منوال)) {
				remove(بفر.c_str());
			} وإلا
				remove_directory(بفر);
		}
		حرر(namelist[i]);
	}
	حرر(namelist);
	rmdir(dirname.c_str());
#جے_انت
}

منطقي create_directory(ثابت std::string& dirname)
{
#إذا defined(_WIN32)
	صحيح ret = _mkdir(dirname.c_str());
#وإلا
	منوال_t منوال = 0755;
	صحيح ret = mkdir(dirname.c_str(), منوال);
#جے_انت
	إذا (ret == 0)
		أرجع سچ;

	بدل (errno)
	{
	حالة ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::ناپ_type pos = dirname.لبھو_آخری_دا('/');
			إذا (pos == std::string::npos)
#إذا defined(_WIN32)
				pos = dirname.لبھو_آخری_دا('\\');
			إذا (pos == std::string::npos)
#جے_انت
				أرجع خطأ;
			إذا (!create_directory( dirname.substr(0, pos) ))
				أرجع خطأ;
		}
		// now, try to create again
#إذا defined(_WIN32)
		أرجع 0 == _mkdir(dirname.c_str());
#وإلا
		أرجع 0 == mkdir(dirname.c_str(), منوال);
#جے_انت

	حالة EEXIST:
		// done!
		أرجع check_directory_exists(dirname);

	افتراضي:
		أرجع خطأ;
	}
}

منطقي make_نجی_directory(ثابت std::string& dirname)
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
	std::string ماپا = std::filesystem::رستہ_2(dirname).ماپا_رستہ().string();

	إذا (err == ENOENT && !ماپا.خالی_تھاواں() && create_directory(ماپا)) {
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
	إذا (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_منوال) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		أرجع خطأ;
	}
	// Repair the mode if something changed it
	منوال_t منوال = st.st_منوال & 0777;
	إذا (((منوال & S_IRWXU) != S_IRWXU || (منوال & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		أرجع خطأ;
	}

	أرجع سچ;
#جے_انت
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.خالی_تھاواں() && name.لبھو('/') == std::string::npos);
#إذا defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_ادھار_tmpdir() + "/" + name;
#وإلا
	std::string dirname = stringf("%s/%s-%u", get_ادھار_tmpdir().c_str(), name.c_str(), (بنانشانی)geteuid());
#جے_انت
	أرجع make_نجی_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string باہرلا;
	باہرلا.reserve(filename.ناپ());
	لكل (خودکار c : filename)
	{
		إذا (c == ' ')
			باہرلا += "\\ ";
		وإلا
			باہرلا.ڈالو_پٹھ(c);
	}
	أرجع باہرلا;
}

فراغ append_globbed(std::سمتیہ<std::string>& paths, std::string نمونہ)
{
	rewrite_filename(نمونہ);
	std::سمتیہ<std::string> globbed = glob_filename(نمونہ);
	copy(globbed.begin(), globbed.سرا(), پٹھ_inserter(paths));
}

std::string name_from_file_رستہ(std::string رستہ_2) {
	أرجع std::filesystem::رستہ_2(رستہ_2).filename().string();
}

std::string absolute_رستہ(ثابت std::string& رستہ_2)
{
	std::error_کوڈ ec;
	std::filesystem::رستہ_2 نتیجہ = std::filesystem::absolute(رستہ_2, ec);

	إذا (ec)
		أرجع "";

	نتیجہ = std::filesystem::weakly_canonical(نتیجہ, ec);

	إذا (ec)
		أرجع "";

	أرجع نتیجہ.string();
}

// Includes OS_PATH_SEP at the end if present
std::string ماپا_from_file_رستہ(std::string رستہ_2) {
	خودکار ماپا = std::filesystem::رستہ_2(رستہ_2).ماپا_رستہ();
	إذا (ماپا.خالی_تھاواں()) {
		أرجع "";
	}
	// Add trailing separator to match original behavior
	std::string نتیجہ = ماپا.string();
	إذا (!نتیجہ.خالی_تھاواں() && نتیجہ.پٹھ() != std::filesystem::رستہ_2::preferred_separator) {
		نتیجہ += std::filesystem::رستہ_2::preferred_separator;
	}
	أرجع نتیجہ;
}

فراغ format_emit_unescaped(std::string &نتیجہ, std::string_view fmt)
{
	نتیجہ.reserve(نتیجہ.ناپ() + fmt.ناپ());
	لكل (ناپ_t i = 0; i < fmt.ناپ(); ++i) {
		حرف ch = fmt[i];
		نتیجہ.ڈالو_پٹھ(ch);
		إذا (ch == '%' && i + 1 < fmt.ناپ() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string نتیجہ;
	format_emit_unescaped(نتیجہ, fmt);
	أرجع نتیجہ;
}

ساكن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string نتیجہ = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	أرجع نتیجہ;
}

ساكن صحيح spec_parameter_ناپ(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	بدل (spec[spec.ناپ() - 1]) {
	حالة 'd':
	حالة 'i':
	حالة 'o':
	حالة 'u':
	حالة 'x':
	حالة 'X':
		بدل (spec[spec.ناپ() - 2]) {
		حالة 'h':
			إذا (spec[spec.ناپ() - 3] == 'h')
				أرجع حجم(حرف);
			أرجع حجم(چھوٹا);
		حالة 'l':
			إذا (spec[spec.ناپ() - 3] == 'l')
				أرجع حجم(لما لما);
			أرجع حجم(لما);
		حالة 'L':
		حالة 'q':
			أرجع حجم(لما لما);
		حالة 'j':
			أرجع حجم(intmax_t);
		حالة 'z':
		حالة 'Z':
			أرجع حجم(ناپ_t);
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
		إذا (spec[spec.ناپ() - 2] == 'L')
			أرجع حجم(لما مزدوج);
		إذا (spec[spec.ناپ() - 2] == 'l' && spec[spec.ناپ() - 3] == 'l')
			أرجع حجم(لما مزدوج);
		أرجع حجم(مزدوج);
	حالة 'c':
		إذا (spec[spec.ناپ() - 2] == 'l') {
			أرجع حجم(wchar_t);
		}
		أرجع حجم(بنانشانی حرف);
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

سانچا <قسم_ناں Arg>
ساكن فراغ format_emit_stringf(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	بدل (عدد_dynamic_ints) {
	حالة Dynamic_Int_گنتی::NONE:
		نتیجہ += string_view_stringf(spec, arg);
		أرجع;
	حالة Dynamic_Int_گنتی::ONE:
		نتیجہ += string_view_stringf(spec, dynamic_ints[0], arg);
		أرجع;
	حالة Dynamic_Int_گنتی::TWO:
		نتیجہ += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		أرجع;
	}
	YOSYS_ABORT("Internal error");
}

فراغ format_emit_long_long(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, لما لما arg)
{
	إذا (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += std::منزل_string(static_cast<صحيح>(arg));
		أرجع;
	}
	إذا (spec_parameter_ناپ(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, (صحيح)arg);
	} وإلا {
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
	}
}

فراغ format_emit_unsigned_long_long(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, بنانشانی لما لما arg)
{
	إذا (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += std::منزل_string(static_cast<بنانشانی صحيح>(arg));
		أرجع;
	}
	إذا (spec == "%c") {
		نتیجہ += static_cast<حرف>(arg);
		أرجع;
	}
	إذا (spec_parameter_ناپ(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, (بنانشانی صحيح)arg);
	} وإلا {
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
	}
}

فراغ format_emit_double(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, مزدوج arg)
{
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

فراغ format_emit_char_ptr(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت حرف *arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += arg;
		أرجع;
	}
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

فراغ format_emit_string(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت std::string &arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += arg;
		أرجع;
	}
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg.c_str());
}

فراغ format_emit_string_view(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, std::string_view arg)
{
	إذا (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		نتیجہ += arg;
		أرجع;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, std::string(arg).c_str());
}

فراغ format_emit_void_ptr(std::string &نتیجہ, std::string_view spec, صحيح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت فراغ *arg)
{
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

#إذا defined(_WIN32)
Scoped_File_جندرا::Scoped_File_جندرا(ثابت std::string &رستہ_2)
{
	HANDLE h = CreateFileA(رستہ_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			عدم, OPEN_ALWAYS, FILE_ATTRIBUTE_عام, عدم);
	إذا (h == INVALID_HANDLE_قدر) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", رستہ_2.c_str());
		أرجع;
	}
	OVERLAPPED ov = {};
	إذا (!جندرا_File_Ex(h, LOCKFILE_EXCLUSIVE_جندرا, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", رستہ_2.c_str());
		CloseHandle(h);
		أرجع;
	}
	handle = h;
}

Scoped_File_جندرا::~Scoped_File_جندرا()
{
	إذا (handle != خالی_سوچک) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وإلاإذا !defined(__wasi__)
Scoped_File_جندرا::Scoped_File_جندرا(ثابت std::string &رستہ_2)
{
	fd = open(رستہ_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	إذا (fd < 0)
		fd = open(رستہ_2.c_str(), O_RDONLY | O_CLOEXEC);
	إذا (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", رستہ_2.c_str());
		أرجع;
	}
	صحيح ret;
	طالما ((ret = flock(fd, جندرا_EX)) != 0 && errno == EINTR);
	إذا (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", رستہ_2.c_str());
}

Scoped_File_جندرا::~Scoped_File_جندرا()
{
	إذا (fd >= 0)
		close(fd); // releases the flock
}
#وإلا
Scoped_File_جندرا::Scoped_File_جندرا(ثابت std::string &) {}
Scoped_File_جندرا::~Scoped_File_جندرا() {}
#جے_انت

بنانشانی get_process_id()
{
#إذا defined(_WIN32)
	أرجع GetCurrentProcessId();
#وإلاإذا defined(__wasi__)
	أرجع 0;
#وإلا
	أرجع getpid();
#جے_انت
}

YOSYS_NAMESPACE_سرا
