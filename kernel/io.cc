#شامل "kernel/yosys_common.h"
#شامل "kernel/log.h"
#شامل <iostream>
#شامل <string>
#شامل <filesystem>
#شامل <system_error>

#اگر !defined(WIN32)
#شامل <dirent.h>
#شامل <unistd.h>
#ورنہ
#شامل <io.h>
#اگر_ختم

#اگر !defined(_WIN32) && !defined(__wasi__)
#شامل <sys/file.h>
#شامل <fcntl.h>
#شامل <cerrno>
#اگر_ختم

YOSYS_NAMESPACE_BEGIN

// Set of utilities for handling files

صحیح readsome(std::istream &f, حرف *s, صحیح n)
{
	صحیح rc = صحیح(f.readsome(s, n));

	// f.readsome() sometimes returns 0 on a non-empty stream..
	اگر (rc == 0) {
		صحیح c = f.get();
		اگر (c != EOF) {
			*s = c;
			rc = 1;
		}
	}

	واپس rc;
}

std::string اگلا_token(std::string &متن, ثابت حرف *sep, منطقی long_لڑیاں)
{
	سائز_t pos_begin = متن.ڈھونڈو_پہلا_not_کا(sep);

	اگر (pos_begin == std::string::npos)
		pos_begin = متن.سائز();

	اگر (long_لڑیاں && pos_begin != متن.سائز() && متن[pos_begin] == '"') {
		std::string sep_string = sep;
		برائے (سائز_t i = pos_begin+1; i < متن.سائز(); i++) {
			اگر (متن[i] == '"' && (i+1 == متن.سائز() || sep_string.ڈھونڈو(متن[i+1]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+1);
				واپس token;
			}
			اگر (i+1 < متن.سائز() && متن[i] == '"' && متن[i+1] == ';' && (i+2 == متن.سائز() || sep_string.ڈھونڈو(متن[i+2]) != std::string::npos)) {
				std::string token = متن.substr(pos_begin, i-pos_begin+1);
				متن = متن.substr(i+2);
				واپس token + ";";
			}
		}
	}

	سائز_t pos_انتہا = متن.ڈھونڈو_پہلا_کا(sep, pos_begin);

	اگر (pos_انتہا == std::string::npos)
		pos_انتہا = متن.سائز();

	std::string token = متن.substr(pos_begin, pos_انتہا-pos_begin);
	متن = متن.substr(pos_انتہا);
	واپس token;
}

std::سمتیہ<std::string> بانٹو_علامات(ثابت std::string &متن, ثابت حرف *sep)
{
	std::سمتیہ<std::string> علامات;
	std::string current_token;
	برائے (حرف c : متن) {
		اگر (strchr(sep, c)) {
			اگر (!current_token.خالی_خانے()) {
				علامات.ڈالو_عقب(current_token);
				current_token.clear();
			}
		} ورنہ
			current_token += c;
	}
	اگر (!current_token.خالی_خانے()) {
		علامات.ڈالو_عقب(current_token);
		current_token.clear();
	}
	واپس علامات;
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
منطقی patmatch(ثابت حرف *نمونہ, ثابت حرف *string)
{
	اگر (*نمونہ == 0)
		واپس *string == 0;

	اگر (*نمونہ == '\\') {
		اگر (نمونہ[1] == string[0] && patmatch(نمونہ+2, string+1))
			واپس سچ;
	}

	اگر (*نمونہ == '?') {
		اگر (*string == 0)
			واپس جھوٹ;
		واپس patmatch(نمونہ+1, string+1);
	}

	اگر (*نمونہ == '*') {
		جب (*string) {
			اگر (patmatch(نمونہ+1, string++))
				واپس سچ;
		}
		واپس نمونہ[1] == 0;
	}

	اگر (*نمونہ == '[') {
		منطقی ملا_مطابقت = جھوٹ;
		منطقی inverted_list = نمونہ[1] == '!';
		ثابت حرف *p = نمونہ + (inverted_list ? 1 : 0);

		جب (*++p) {
			اگر (*p == ']') {
				اگر (ملا_مطابقت != inverted_list && patmatch(p+1, string+1))
					واپس سچ;
				توڑو;
			}

			اگر (*p == '\\') {
				اگر (*++p == *string)
					ملا_مطابقت = سچ;
			} ورنہ
			اگر (*p == *string)
				ملا_مطابقت = سچ;
		}
	}

	اگر (*نمونہ == *string)
		واپس patmatch(نمونہ+1, string+1);

	واپس جھوٹ;
}

std::string get_بنیاد_tmpdir()
{
	ساکن std::string tmpdir;

	اگر (!tmpdir.خالی_خانے()) {
		واپس tmpdir;
	}

#اگر defined(_WIN32)
	std::wstring wtmppath(4096, L'\0');
	// typically returns a 8.3 path, but this can be disabled via registry
	اگر (!Get_عارضی_راستہ_W(wtmppath.سائز(), wtmppath.ڈیٹا()))
		log_error("GetTempPathW() failed.\n");
	wtmppath.resize(wtmppath.ڈھونڈو(L'\0'));
	std::string utmppath;
	utmppath.resize(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wtmppath.ڈیٹا(), wtmppath.سائز(), تہی, 0, تہی, تہی));
	اگر (Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wtmppath.ڈیٹا(), wtmppath.سائز(), utmppath.ڈیٹا(), utmppath.سائز(), تہی, تہی) == 0)
		log_error("WideCharToMultiByte() failed.\n");
	tmpdir += utmppath;
#ورنہ
	حرف * var = std::getenv("TMPDIR");
	اگر (var && لڑی_لمبائی(var)!=0) {
		tmpdir.تفویض(var);
		// We return the directory name without the trailing '/'
		جب (!tmpdir.خالی_خانے() && (tmpdir.عقب() == '/')) {
			tmpdir.نکالو_عقب();
		}
	} ورنہ {
		tmpdir.تفویض("/tmp");
	}
#اگر_ختم
	واپس tmpdir;
}

std::string make_عارضی_file(std::string template_str)
{
	سائز_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);
#اگر defined(__wasm)
	ساکن سائز_t اشاریہ = 0;
	template_str.replace(pos, 6, stringf("%06zu", اشاریہ++));
#وگرنہ_اگر defined(_WIN32)
#اگر_غیر_تعریف_شدہ YOSYS_WIN32_UNIX_DIR
	std::replace(template_str.begin(), template_str.انتہا(), '/', '\\');
#اگر_ختم
	جب (1) {
		برائے (صحیح i = 0; i < 6; i++) {
			ساکن std::string y = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			ساکن uint32_t x = 314159265 ^ uint32_t(وقت_2(تہی));
			x ^= x << 13, x ^= x >> 17, x ^= x << 5;
			template_str[pos+i] = y[x % y.سائز()];
		}
		اگر (_access(template_str.c_str(), 0) != 0)
			توڑو;
	}
#ورنہ
	صحیح suffixlen = template_str.سائز() - pos - 6;

	حرف *p = strdup(template_str.c_str());
	close(mkstemps(p, suffixlen));
	template_str = p;
	آزاد(p);
#اگر_ختم

	واپس template_str;
}

std::string make_عارضی_dir(std::string template_str)
{
#اگر defined(_WIN32)
	template_str = make_عارضی_file(template_str);
	mkdir(template_str.c_str());
	واپس template_str;
#وگرنہ_اگر defined(__wasm)
	template_str = make_عارضی_file(template_str);
	mkdir(template_str.c_str(), 0777);
	واپس template_str;
#ورنہ
#  ifndef NDEBUG
	سائز_t pos = template_str.rfind("XXXXXX");
	log_assert(pos != std::string::npos);

	صحیح suffixlen = template_str.سائز() - pos - 6;
	log_assert(suffixlen == 0);
#  endif

	حرف *p = strdup(template_str.c_str());
	log_assert(p);
	حرف *res = mkdtemp(p);
	اگر (!res)
		log_error("mkdtemp failed for '%s': %s [Error %d]\n",
			p, strerror(errno), errno);
	template_str = p;
	آزاد(p);

	واپس template_str;
#اگر_ختم
}

منطقی check_is_directory(ثابت std::string& dirname)
{
#اگر defined(_WIN32)
	ساخت _stat info;
	خودکار dirname_ = dirname;

	/* On old versions of Visual Studio and current versions on MinGW,
	   _stat will fail if the path ends with a trailing slash. */
	اگر (dirname.عقب() == '/' || dirname.عقب() == '\\') {
		dirname_ = dirname.substr(0, dirname.لمبائی_2() - 1);
	}

	اگر (_stat(dirname_.c_str(), &info) != 0)
	{
		واپس جھوٹ;
	}
	واپس (info.st_منوال & _S_IFDIR) != 0;
#ورنہ
	ساخت stat info;
	اگر (stat(dirname.c_str(), &info) != 0)
	{
		واپس جھوٹ;
	}
	واپس (info.st_منوال & S_IFDIR) != 0;
#اگر_ختم
}

#اگر_تعریف_شدہ _WIN32
منطقی check_accessible(ثابت std::string& filename, منطقی)
{
	واپس _access(filename.c_str(), 0) == 0;
}
#ورنہ
منطقی check_accessible(ثابت std::string& filename, منطقی is_exec)
{
	واپس access(filename.c_str(), is_exec ? X_OK : F_OK) == 0;
}
#اگر_ختم

منطقی check_file_exists(ثابت std::string& filename, منطقی is_exec)
{
	واپس check_accessible(filename, is_exec) && !check_is_directory(filename);
}
منطقی check_directory_exists(ثابت std::string& filename, منطقی is_exec)
{
	واپس check_accessible(filename, is_exec) && check_is_directory(filename);
}

منطقی is_absolute_راستہ(std::string filename)
{
#اگر_تعریف_شدہ _WIN32
	واپس filename[0] == '/' || filename[0] == '\\' || (filename[0] != 0 && filename[1] == ':');
#ورنہ
	واپس filename[0] == '/';
#اگر_ختم
}

خالی remove_directory(std::string dirname)
{
#اگر_تعریف_شدہ _WIN32
	تسلسل_command(stringf("rmdir /s /q \"%s\"", dirname));
#ورنہ
	ساخت stat stbuf;
	ساخت dirent **namelist;
	صحیح n = scandir(dirname.c_str(), &namelist, خالی_اشاریہ, alphasort);
	log_assert(n >= 0);
	برائے (صحیح i = 0; i < n; i++) {
		اگر (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, "..")) {
			std::string بفر = stringf("%s/%s", dirname, namelist[i]->d_name);
			اگر (!stat(بفر.c_str(), &stbuf) && S_ISREG(stbuf.st_منوال)) {
				remove(بفر.c_str());
			} ورنہ
				remove_directory(بفر);
		}
		آزاد(namelist[i]);
	}
	آزاد(namelist);
	rmdir(dirname.c_str());
#اگر_ختم
}

منطقی create_directory(ثابت std::string& dirname)
{
#اگر defined(_WIN32)
	صحیح ret = _mkdir(dirname.c_str());
#ورنہ
	منوال_t منوال = 0755;
	صحیح ret = mkdir(dirname.c_str(), منوال);
#اگر_ختم
	اگر (ret == 0)
		واپس سچ;

	انتخاب (errno)
	{
	حالت ENOENT:
		// parent didn't exist, try to create it
		{
			std::string::سائز_type pos = dirname.ڈھونڈو_آخری_کا('/');
			اگر (pos == std::string::npos)
#اگر defined(_WIN32)
				pos = dirname.ڈھونڈو_آخری_کا('\\');
			اگر (pos == std::string::npos)
#اگر_ختم
				واپس جھوٹ;
			اگر (!create_directory( dirname.substr(0, pos) ))
				واپس جھوٹ;
		}
		// now, try to create again
#اگر defined(_WIN32)
		واپس 0 == _mkdir(dirname.c_str());
#ورنہ
		واپس 0 == mkdir(dirname.c_str(), منوال);
#اگر_ختم

	حالت EEXIST:
		// done!
		واپس check_directory_exists(dirname);

	طےشدہ:
		واپس جھوٹ;
	}
}

منطقی make_نجی_directory(ثابت std::string& dirname)
{
#اگر defined(_WIN32) || defined(__wasi__)
	اگر (!create_directory(dirname)) {
		log_warning("Cannot create the directory %s\n", dirname.c_str());
		واپس جھوٹ;
	}
	واپس سچ;
#ورنہ
	اگر (mkdir(dirname.c_str(), 0700) == 0)
		واپس سچ;

	// Keep the errno of each mkdir(), create_directory() below makes its own calls
	صحیح err = errno;
	std::string والد = std::filesystem::راستہ_2(dirname).والد_راستہ().string();

	اگر (err == ENOENT && !والد.خالی_خانے() && create_directory(والد)) {
		اگر (mkdir(dirname.c_str(), 0700) == 0)
			واپس سچ;
		err = errno;
	}

	اگر (err != EEXIST) {
		log_warning("Cannot create the directory %s: %s\n", dirname.c_str(), strerror(err));
		واپس جھوٹ;
	}

	// Do not use a leftover that isn't ours
	ساخت stat st;
	اگر (lstat(dirname.c_str(), &st) != 0 || !S_ISDIR(st.st_منوال) || st.st_uid != geteuid()) {
		log_warning("%s is not a directory of this user\n", dirname.c_str());
		واپس جھوٹ;
	}
	// Repair the mode if something changed it
	منوال_t منوال = st.st_منوال & 0777;
	اگر (((منوال & S_IRWXU) != S_IRWXU || (منوال & (S_IWGRP | S_IWOTH)) != 0) && chmod(dirname.c_str(), 0700) != 0) {
		log_warning("Cannot make the directory %s private: %s\n", dirname.c_str(), strerror(errno));
		واپس جھوٹ;
	}

	واپس سچ;
#اگر_ختم
}

std::string make_user_tmpdir(ثابت std::string& name)
{
	log_assert(!name.خالی_خانے() && name.ڈھونڈو('/') == std::string::npos);
#اگر defined(_WIN32) || defined(__wasi__)
	std::string dirname = get_بنیاد_tmpdir() + "/" + name;
#ورنہ
	std::string dirname = stringf("%s/%s-%u", get_بنیاد_tmpdir().c_str(), name.c_str(), (غیرعلامتی)geteuid());
#اگر_ختم
	واپس make_نجی_directory(dirname) ? dirname : "";
}

std::string escape_filename_spaces(ثابت std::string& filename)
{
	std::string برآمد;
	برآمد.reserve(filename.سائز());
	برائے (خودکار c : filename)
	{
		اگر (c == ' ')
			برآمد += "\\ ";
		ورنہ
			برآمد.ڈالو_عقب(c);
	}
	واپس برآمد;
}

خالی append_globbed(std::سمتیہ<std::string>& paths, std::string نمونہ)
{
	rewrite_filename(نمونہ);
	std::سمتیہ<std::string> globbed = glob_filename(نمونہ);
	copy(globbed.begin(), globbed.انتہا(), عقب_inserter(paths));
}

std::string name_from_file_راستہ(std::string راستہ_2) {
	واپس std::filesystem::راستہ_2(راستہ_2).filename().string();
}

std::string absolute_راستہ(ثابت std::string& راستہ_2)
{
	std::error_رمز ec;
	std::filesystem::راستہ_2 نتیجہ = std::filesystem::absolute(راستہ_2, ec);

	اگر (ec)
		واپس "";

	نتیجہ = std::filesystem::weakly_canonical(نتیجہ, ec);

	اگر (ec)
		واپس "";

	واپس نتیجہ.string();
}

// Includes OS_PATH_SEP at the end if present
std::string والد_from_file_راستہ(std::string راستہ_2) {
	خودکار والد = std::filesystem::راستہ_2(راستہ_2).والد_راستہ();
	اگر (والد.خالی_خانے()) {
		واپس "";
	}
	// Add trailing separator to match original behavior
	std::string نتیجہ = والد.string();
	اگر (!نتیجہ.خالی_خانے() && نتیجہ.عقب() != std::filesystem::راستہ_2::preferred_separator) {
		نتیجہ += std::filesystem::راستہ_2::preferred_separator;
	}
	واپس نتیجہ;
}

خالی format_emit_unescaped(std::string &نتیجہ, std::string_view fmt)
{
	نتیجہ.reserve(نتیجہ.سائز() + fmt.سائز());
	برائے (سائز_t i = 0; i < fmt.سائز(); ++i) {
		حرف ch = fmt[i];
		نتیجہ.ڈالو_عقب(ch);
		اگر (ch == '%' && i + 1 < fmt.سائز() && fmt[i + 1] == '%') {
			++i;
		}
	}
}

std::string unescape_format_string(std::string_view fmt)
{
	std::string نتیجہ;
	format_emit_unescaped(نتیجہ, fmt);
	واپس نتیجہ;
}

ساکن std::string string_view_stringf(std::string_view spec, ...)
{
	va_list ap;
	va_start(ap, spec);
	std::string نتیجہ = vstringf(std::string(spec).c_str(), ap);
	va_end(ap);
	واپس نتیجہ;
}

ساکن صحیح spec_parameter_سائز(std::string_view spec)
{
	// Every valid spec starts with '%' which means the code below
	// won't look before the spec start.
	انتخاب (spec[spec.سائز() - 1]) {
	حالت 'd':
	حالت 'i':
	حالت 'o':
	حالت 'u':
	حالت 'x':
	حالت 'X':
		انتخاب (spec[spec.سائز() - 2]) {
		حالت 'h':
			اگر (spec[spec.سائز() - 3] == 'h')
				واپس حجم(حرف);
			واپس حجم(مختصر);
		حالت 'l':
			اگر (spec[spec.سائز() - 3] == 'l')
				واپس حجم(طویل طویل);
			واپس حجم(طویل);
		حالت 'L':
		حالت 'q':
			واپس حجم(طویل طویل);
		حالت 'j':
			واپس حجم(intmax_t);
		حالت 'z':
		حالت 'Z':
			واپس حجم(سائز_t);
		حالت 't':
			واپس حجم(ptrdiff_t);
		}
		واپس حجم(صحیح);
	حالت 'e':
	حالت 'E':
	حالت 'f':
	حالت 'F':
	حالت 'g':
	حالت 'G':
	حالت 'a':
	حالت 'A':
		اگر (spec[spec.سائز() - 2] == 'L')
			واپس حجم(طویل دوہرا);
		اگر (spec[spec.سائز() - 2] == 'l' && spec[spec.سائز() - 3] == 'l')
			واپس حجم(طویل دوہرا);
		واپس حجم(دوہرا);
	حالت 'c':
		اگر (spec[spec.سائز() - 2] == 'l') {
			واپس حجم(wchar_t);
		}
		واپس حجم(غیرعلامتی حرف);
	حالت 'C':
		واپس حجم(wchar_t);
	حالت 's':
	حالت 'p':
	حالت 'S':
	حالت 'n':
		واپس حجم(خالی *);
	حالت 'm':
		واپس حجم(صحیح);
	طےشدہ:
		واپس -1;
	}
}

سانچہ <قسم_نام Arg>
ساکن خالی format_emit_stringf(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, Arg arg)
{
	// Delegate nontrivial formats to the C library.
	انتخاب (عدد_dynamic_ints) {
	حالت Dynamic_Int_گنتی::NONE:
		نتیجہ += string_view_stringf(spec, arg);
		واپس;
	حالت Dynamic_Int_گنتی::ONE:
		نتیجہ += string_view_stringf(spec, dynamic_ints[0], arg);
		واپس;
	حالت Dynamic_Int_گنتی::TWO:
		نتیجہ += string_view_stringf(spec, dynamic_ints[0], dynamic_ints[1], arg);
		واپس;
	}
	YOSYS_ABORT("Internal error");
}

خالی format_emit_long_long(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, طویل طویل arg)
{
	اگر (spec == "%d") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += std::مقصد_string(static_cast<صحیح>(arg));
		واپس;
	}
	اگر (spec_parameter_سائز(spec) <= 4) {
		// On some platforms (Wasm) we must ensure that the arg is properly aligned
		// after the dynamic `int` parameters.
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, (صحیح)arg);
	} ورنہ {
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
	}
}

خالی format_emit_unsigned_long_long(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, غیرعلامتی طویل طویل arg)
{
	اگر (spec == "%u") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += std::مقصد_string(static_cast<غیرعلامتی صحیح>(arg));
		واپس;
	}
	اگر (spec == "%c") {
		نتیجہ += static_cast<حرف>(arg);
		واپس;
	}
	اگر (spec_parameter_سائز(spec) <= 4) {
                // On some platforms (Wasm) we must ensure that the arg is properly aligned
                // after the dynamic `int` parameters.
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, (غیرعلامتی صحیح)arg);
	} ورنہ {
		format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
	}
}

خالی format_emit_double(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, دوہرا arg)
{
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

خالی format_emit_char_ptr(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت حرف *arg)
{
	اگر (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += arg;
		واپس;
	}
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

خالی format_emit_string(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت std::string &arg)
{
	اگر (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		نتیجہ += arg;
		واپس;
	}
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg.c_str());
}

خالی format_emit_string_view(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, std::string_view arg)
{
	اگر (spec == "%s") {
		// Format checking will have guaranteed num_dynamic_ints == 0.
		// We can output the string without creating a temporary copy.
		نتیجہ += arg;
		واپس;
	}
	// Delegate nontrivial formats to the C library. We need to construct
	// a temporary string to ensure null termination.
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, std::string(arg).c_str());
}

خالی format_emit_void_ptr(std::string &نتیجہ, std::string_view spec, صحیح *dynamic_ints,
	Dynamic_Int_گنتی عدد_dynamic_ints, ثابت خالی *arg)
{
	format_emit_stringf(نتیجہ, spec, dynamic_ints, عدد_dynamic_ints, arg);
}

#اگر defined(_WIN32)
Scoped_File_تالا::Scoped_File_تالا(ثابت std::string &راستہ_2)
{
	HANDLE h = CreateFileA(راستہ_2.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			تہی, OPEN_ALWAYS, FILE_ATTRIBUTE_معمول, تہی);
	اگر (h == INVALID_HANDLE_قدر) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", راستہ_2.c_str());
		واپس;
	}
	OVERLAPPED ov = {};
	اگر (!تالا_File_Ex(h, LOCKFILE_EXCLUSIVE_تالا, 0, MAXDWORD, MAXDWORD, &ov)) {
		log_warning("Cannot lock %s, proceeding without lock\n", راستہ_2.c_str());
		CloseHandle(h);
		واپس;
	}
	handle = h;
}

Scoped_File_تالا::~Scoped_File_تالا()
{
	اگر (handle != خالی_اشاریہ) {
		OVERLAPPED ov = {};
		UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &ov);
		CloseHandle(handle);
	}
}
#وگرنہ_اگر !defined(__wasi__)
Scoped_File_تالا::Scoped_File_تالا(ثابت std::string &راستہ_2)
{
	fd = open(راستہ_2.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0666);
	اگر (fd < 0)
		fd = open(راستہ_2.c_str(), O_RDONLY | O_CLOEXEC);
	اگر (fd < 0) {
		log_warning("Cannot open lock file %s, proceeding without lock\n", راستہ_2.c_str());
		واپس;
	}
	صحیح ret;
	جب ((ret = flock(fd, تالا_EX)) != 0 && errno == EINTR);
	اگر (ret != 0)
		log_warning("Cannot lock %s, proceeding without lock\n", راستہ_2.c_str());
}

Scoped_File_تالا::~Scoped_File_تالا()
{
	اگر (fd >= 0)
		close(fd); // releases the flock
}
#ورنہ
Scoped_File_تالا::Scoped_File_تالا(ثابت std::string &) {}
Scoped_File_تالا::~Scoped_File_تالا() {}
#اگر_ختم

غیرعلامتی get_process_id()
{
#اگر defined(_WIN32)
	واپس GetCurrentProcessId();
#وگرنہ_اگر defined(__wasi__)
	واپس 0;
#ورنہ
	واپس getpid();
#اگر_ختم
}

YOSYS_NAMESPACE_انتہا
