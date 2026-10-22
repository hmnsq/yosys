/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ضمن "kernel/yosys.h"
#ضمن "kernel/log.h"
#ضمن "kernel/newcelltypes.h"

#جے_تعریف_شدہ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_TCL
#  include <tcl.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#جے_انت

#إذا defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#وإلاإذا defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#وإلا
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  إذا defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#جے_انت

#إذا !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#جے_انت

#إذا defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
خارجي "C" PyObject* PyInit_libyosys();
خارجي "C" PyObject* PyInit_pyosys();
#ضمن <signal.h>
#جے_انت

#ضمن <limits.h>
#ضمن <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحيح yosys_xtrace = 0;
منطقي yosys_write_versions = سچ;
ثابت حرف* yosys_maybe_نسخہ() {
	إذا (yosys_write_versions)
		أرجع yosys_نسخہ_str;
	وإلا
		أرجع "Yosys";
}

RTLIL::Design *yosys_design = عدم;
NewCellTypes yosys_celltypes;

#جے_تعریف_شدہ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = عدم;
Tcl_Interp *yosys_sdc_interp = عدم;
#جے_انت

std::سیٹ<std::string> yosys_داخلہ_files, yosys_output_files;

منطقي memhasher_سرگرم = خطأ;
uint32_t memhasher_rng = 123456;
std::سمتیہ<فراغ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقي Multithreading::سرگرم_2 = خطأ;

فراغ init_share_dirname();
فراغ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!سرگرم_2);
	سرگرم_2 = سچ;
}

Multithreading::~Multithreading() {
	log_assert(سرگرم_2);
	سرگرم_2 = خطأ;
}

فراغ Autoidx::ensure_at_گھٹ_توں_گھٹ(صحيح v) {
	log_assert(!Multithreading::سرگرم());
	قدر = std::max(قدر, v);
}

صحيح Autoidx::عامل++(صحيح) {
	log_assert(!Multithreading::سرگرم());
	أرجع قدر++;
}

فراغ memhasher_on()
{
#إذا defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += ویلا_2(عدم) << 16 ^ getpid();
#جے_انت
	memhasher_store.resize(0x10000);
	memhasher_سرگرم = سچ;
}

فراغ memhasher_off()
{
	لكل (خودکار p : memhasher_store)
		إذا (p) حرر(p);
	memhasher_store.clear();
	memhasher_سرگرم = خطأ;
}

فراغ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	صحيح ناپ, اشاریہ = (memhasher_rng >> 4) & 0xffff;
	بدل (memhasher_rng & 7) {
		حالة 0: ناپ =   16; اكسر;
		حالة 1: ناپ =  256; اكسر;
		حالة 2: ناپ = 1024; اكسر;
		حالة 3: ناپ = 4096; اكسر;
		افتراضي: ناپ = 0;
	}
	إذا (اشاریہ < 16) ناپ *= 16;
	memhasher_store[اشاریہ] = realloc(memhasher_store[اشاریہ], ناپ);
}

فراغ yosys_banner()
{
	لوگارتھم("\n");
	لوگارتھم(" /----------------------------------------------------------------------------\\\n");
	لوگارتھم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	لوگارتھم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	لوگارتھم(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	لوگارتھم(" \\----------------------------------------------------------------------------/\n");
	لوگارتھم(" %s\n", yosys_maybe_نسخہ());
}

#إذا defined(YOSYS_ENABLE_SPAWN)
صحيح لگاتار_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_سطر)
{
	إذا (!process_سطر)
		أرجع system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	إذا (f == خالی_سوچک)
		أرجع -1;

	std::string سطر;
	حرف logbuf[128];
	طالما (سچ) {
		إذا (fgets(logbuf, 128, f) != عدم) {
			سطر += logbuf;
			إذا (!سطر.خالی_تھاواں() && سطر.پٹھ() == '\n')
				process_سطر(سطر), سطر.clear();
		} وإلا إذا (ferror(f) && errno == EINTR) {
			clearerr(f);
			استمر;
		} وإلا {
			اكسر;
		}
	}
	إذا (!سطر.خالی_تھاواں())
		process_سطر(سطر);

	صحيح ret = pclose(f);
	إذا (ret < 0)
		أرجع -2;
#جے_تعریف_شدہ _WIN32
	أرجع ret;
#وإلا
	إذا (!WIFEXITED(ret))
		أرجع -1;
	أرجع WEXITSTATUS(ret);
#جے_انت
}
#جے_انت

منطقي already_تیاری = خطأ;
منطقي already_shutdown = خطأ;

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.جوڑو_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(فراغ)_;
	ارم py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#جے_انت

فراغ yosys_تیاری()
{
	إذا(already_تیاری)
		أرجع;
	already_تیاری = سچ;
	already_shutdown = خطأ;

	StaticTwines::init();

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	إذا (!Py_IsInitialized()) {
		PyImport_AppendInittab((حرف*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((حرف*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((حرف*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_لگاتار_Simple_String("import sys");
		إشارة(SIGINT, SIG_DFL);
	}
#جے_انت

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_رجسٹر();
	yosys_design = جديد RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_معلوم;
	log_ڈالو();
}

منطقي yosys_already_تیاری()
{
	أرجع already_تیاری;
}

فراغ yosys_shutdown()
{
	إذا(already_shutdown)
		أرجع;
	already_تیاری = خطأ;
	already_shutdown = سچ;
	log_نکالو();

	Pass::پورا_رجسٹر();

	مٹاؤ yosys_design;
	yosys_design = عدم;

	logger().clear();

#جے_تعریف_شدہ YOSYS_ENABLE_TCL
	إذا (yosys_tcl_interp != عدم) {
		إذا (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = عدم;
	}
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_PLUGINS
	لكل (خودکار &it : loaded_plugins)
		dlclose(it.دوجا);

	loaded_plugins.clear();
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#جے_انت
	loaded_plugin_aliases.clear();
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	Py_Finalize();
#جے_انت
}

ثابت std::string *create_id_اگیتر(std::string_view file, صحيح سطر, std::string_view func)
{
#جے_تعریف_شدہ _WIN32
	ناپ_t pos = file.لبھو_آخری_دا("/\\");
#وإلا
	ناپ_t pos = file.لبھو_آخری_دا('/');
#جے_انت
	إذا (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.لبھو_آخری_دا(':');
	إذا (pos != std::string_view::npos)
		func = func.substr(pos+1);

	أرجع جديد std::string(stringf("$auto$%s:%d:%s$", file, سطر, func));
}

RTLIL::Design *yosys_get_design()
{
	أرجع yosys_design;
}

ثابت حرف *create_prompt(RTLIL::Design *design, صحيح recursion_counter, منطقي newline)
{
	ساكن حرف بفر[100];
	std::string str = newline ? "\n" : "";
	إذا (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	إذا (!design->selected_سرگرم_module.خالی_تھاواں())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_سرگرم_module)).c_str());
	إذا (!design->بھریا_selection()) {
		إذا (design->selected_سرگرم_module.خالی_تھاواں())
			str += "*";
		وإلا إذا (design->selection().selected_modules.ناپ() != 1 || design->selection().selected_members.ناپ() != 0 ||
				design->selection().selected_modules.گنتی(design->selected_سرگرم_module) == 0)
			str += "*";
	}
	snprintf(بفر, 100, "%s> ", str.c_str());
	أرجع بفر;
}

std::سمتیہ<std::string> glob_filename(ثابت std::string &filename_نمونہ)
{
	std::سمتیہ<std::string> results;

#إذا defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ڈالو_پٹھ(filename_نمونہ);
#وإلا
	glob_t globbuf;

	صحيح err = glob(filename_نمونہ.c_str(), 0, عدم, &globbuf);

	إذا(err == 0) {
		لكل (ناپ_t i = 0; i < globbuf.gl_pathc; i++)
			results.ڈالو_پٹھ(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} وإلا {
		results.ڈالو_پٹھ(filename_نمونہ);
	}
#جے_انت

	أرجع results;
}

فراغ rewrite_filename(std::string &filename)
{
	إذا (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_ناپ(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_ناپ(filename)-2);
	إذا (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#جے_بنا_تعریف _WIN32
	إذا (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#جے_انت
}

#جے_تعریف_شدہ YOSYS_ENABLE_TCL

// defined in tclapi.cc
خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);

خارجي Tcl_Interp *yosys_get_tcl_interp()
{
	إذا (yosys_tcl_interp == عدم) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	أرجع yosys_tcl_interp;
}

// Also see SdcPass
بنية TclPass : عام Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	فراغ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لوگارتھم("\n");
		لوگارتھم("    tcl <filename> [args]\n");
		لوگارتھم("\n");
		لوگارتھم("This command executes the tcl commands in the specified file.\n");
		لوگارتھم("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		لوگارتھم("\n");
		لوگارتھم("The tcl command 'yosys -import' can be used to import all yosys\n");
		لوگارتھم("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		لوگارتھم("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		لوگارتھم("in order to avoid a name collision with the built in commands.\n");
		لوگارتھم("\n");
		لوگارتھم("If any arguments are specified, these arguments are provided to the script via\n");
		لوگارتھم("the standard $argc and $argv variables.\n");
		لوگارتھم("\n");
		لوگارتھم("Note, tcl will not receive the output of any yosys command. If the output\n");
		لوگارتھم("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		لوگارتھم("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		لوگارتھم("The 'result.string' value is then used as the tcl output value of the command.\n");
		لوگارتھم("\n");
	}
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *) override {
		إذا (دلیلاں_2.ناپ() < 2)
			log_cmd_error("Missing script file.\n");

		std::سمتیہ<Tcl_Obj*> script_دلیلاں;
		لكل (خودکار it = دلیلاں_2.begin() + 2; it != دلیلاں_2.سرا(); ++it)
			script_دلیلاں.ڈالو_پٹھ(Tcl_NewStringObj((*it).c_str(), (*it).ناپ()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(script_دلیلاں.ناپ()), 0);
		Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(script_دلیلاں.ناپ(), script_دلیلاں.انکڑے()), 0);
		Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(دلیلاں_2[1].c_str(), دلیلاں_2[1].ناپ()), 0);
		إذا (Tcl_EvalFile(interp, دلیلاں_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتیجہ(interp));
		Tcl_Release(interp);
	}
} TclPass;

#جے_انت

#إذا defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string رستہ_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	لكل (صحيح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", رستہ_2.انکڑے(), رستہ_2.ناپ());
		إذا (buflen < (ssize_t)رستہ_2.ناپ())
			اكسر;
		وإلا
			رستہ_2.resize(رستہ_2.ناپ() * 2);
	}
	إذا (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		رستہ_2.resize(0);
	} وإلا {
		طالما (buflen > 0 && رستہ_2[buflen-1] != '/')
			buflen--;
		رستہ_2.resize(buflen);
	}
	أرجع رستہ_2;
}
#وإلاإذا defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#جے_تعریف_شدہ __NetBSD__
	صحيح mib[4] = {CTL_KERN, KERN_PROC_دلیلاں, getpid(), KERN_PROC_PATHNAME};
#وإلا
	صحيح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#جے_انت
	ناپ_t buflen;
	حرف *بفر;
	std::string رستہ_2;
	إذا (sysctl(mib, 4, عدم, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	بفر = (حرف*)حافظہ_دیو(buflen);
	إذا (بفر == عدم)
		log_error("malloc failed: %s\n", strerror(errno));
	إذا (sysctl(mib, 4, بفر, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	طالما (buflen > 0 && بفر[buflen-1] != '/')
		buflen--;
	رستہ_2.سونپو(بفر, buflen);
	حرر(بفر);
	أرجع رستہ_2;
}
#وإلاإذا defined(__APPLE__)
std::string proc_self_dirname()
{
	حرف *رستہ_2 = عدم;
	uint32_t buflen = 0;
	طالما (NSGet_Executable_رستہ(رستہ_2, &buflen) != 0)
		رستہ_2 = (حرف *) realloc((فراغ *) رستہ_2, buflen);
	حرف resolved[رستہ_MAX];
	إذا (realpath(رستہ_2, resolved) != عدم) {
		حرر(رستہ_2);
		رستہ_2 = strdup(resolved);
		buflen = لڑی_لمبائی(رستہ_2);
	}
	طالما (buflen > 0 && رستہ_2[buflen-1] != '/')
		buflen--;
	std::string str(رستہ_2, buflen);
	حرر(رستہ_2);
	أرجع str;
}
#وإلاإذا defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	إذا (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.ناپ()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wbinpath.انکڑے(), wbinpath.ناپ(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wbinpath.انکڑے(), wbinpath.ناپ(), &ubinpath[0], ubinpath.ناپ(), عدم, عدم) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	أرجع ubinpath;

}
#وإلاإذا defined(__wasm)
std::string proc_self_dirname()
{
	أرجع "/";
}
#وإلاإذا defined(__OpenBSD__) || defined(__HAIKU__)
حرف yosys_رستہ[رستہ_MAX];
حرف *yosys_argv0;

std::string proc_self_dirname(فراغ)
{
	حرف buf[رستہ_MAX + 1] = "", *رستہ_2, *p;
	// if case argv[0] contains a valid path, return it
	إذا (لڑی_لمبائی(yosys_رستہ) > 0) {
		p = strrchr(yosys_رستہ, '/');
		snprintf(buf, حجم buf, "%*s/", (صحيح)(yosys_رستہ - p), yosys_رستہ);
		أرجع buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	رستہ_2 = strdup(getenv("PATH"));
	إذا (!رستہ_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	لكل (p = strtok(رستہ_2, ":"); p; p = strtok(عدم, ":")) {
		snprintf(buf, حجم buf, "%s/%s", p, yosys_argv0);
		إذا (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			حرر(رستہ_2);
			أرجع buf;
		}
	}
	حرر(رستہ_2);
	log_error("Can't determine yosys executable path.\n");
	أرجع عدم;
}
#وإلا
	#غلطی "Don't know how to determine process executable base path!"
#جے_انت

#إذا defined(__wasm)
فراغ init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#وإلا
فراغ init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_رستہ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت حرف *share_رستہ = PyUnicode_AsUTF8(share_رستہ_obj);
		yosys_share_dirname = std::string(share_رستہ);
		أرجع;
	}
#  endif
	std::string proc_self_رستہ = proc_self_dirname();
#  إذا defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_رستہ = proc_self_رستہ + "share\\";
	إذا (check_directory_exists(proc_share_رستہ, سچ)) {
		yosys_share_dirname = proc_share_رستہ;
		أرجع;
	}
	proc_share_رستہ = proc_self_رستہ + "..\\share\\";
	إذا (check_directory_exists(proc_share_رستہ, سچ)) {
		yosys_share_dirname = proc_share_رستہ;
		أرجع;
	}
#  وإلا
	std::string proc_share_رستہ = proc_self_رستہ + "share/";
	إذا (check_directory_exists(proc_share_رستہ, سچ)) {
		yosys_share_dirname = proc_share_رستہ;
		أرجع;
	}
	proc_share_رستہ = proc_self_رستہ + "../share/" + proc_پروگرام_اگیتر()+ "yosys/";
	إذا (check_directory_exists(proc_share_رستہ, سچ)) {
		yosys_share_dirname = proc_share_رستہ;
		أرجع;
	}
	proc_share_رستہ = YOSYS_DATDIR "/";
	إذا (check_directory_exists(proc_share_رستہ, سچ)) {
		yosys_share_dirname = proc_share_رستہ;
		أرجع;
	}
#  endif
}
#جے_انت

فراغ init_abc_executable_name()
{
#جے_تعریف_شدہ ABCEXTERNAL
	std::string exe_file;
	إذا (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} وإلا {
		yosys_abc_executable = ABCEXTERNAL;
	}
#وإلا
	yosys_abc_executable = proc_self_dirname() + proc_پروگرام_اگیتر()+ "yosys-abc";
#  ifdef _WIN32
	إذا (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_پروگرام_اگیتر() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_پروگرام_اگیتر() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_رستہ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت حرف *abc_رستہ = PyUnicode_AsUTF8(abc_رستہ_obj);
		yosys_abc_executable = std::string(abc_رستہ);
	}
#  endif
#جے_انت
}

std::string proc_share_dirname()
{
	إذا (yosys_share_dirname.خالی_تھاواں())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	أرجع yosys_share_dirname;
}

std::string proc_پروگرام_اگیتر()
{
	أرجع YOSYS_پروگرام_اگیتر;
}

منطقي fgetline(FILE *f, std::string &بفر)
{
	بفر = "";
	حرف بلاک[4096];
	طالما (1) {
		إذا (fgets(بلاک, 4096, f) == عدم)
			أرجع خطأ;
		بفر += بلاک;
		إذا (بفر.ناپ() > 0 && (بفر[بفر.ناپ()-1] == '\n' ||  بفر[بفر.ناپ()-1] == '\r')) {
			طالما (بفر.ناپ() > 0 && (بفر[بفر.ناپ()-1] == '\n' ||  بفر[بفر.ناپ()-1] == '\r'))
				بفر.resize(بفر.ناپ()-1);
			أرجع سچ;
		}
	}
}

ساكن فراغ handle_label(std::string &command, منطقي &from_منزل_سرگرم, ثابت std::string &لگاتار_from, ثابت std::string &لگاتار_منزل)
{
	صحيح pos = 0;
	std::string label;

	طالما (pos < Get_ناپ(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	إذا (pos < Get_ناپ(command) && command[pos] == '#')
		أرجع;

	طالما (pos < Get_ناپ(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	إذا (Get_ناپ(label) > 1 && label.پٹھ() == ':')
	{
		label = label.substr(0, Get_ناپ(label)-1);
		command = command.substr(pos);

		إذا (label == لگاتار_from)
			from_منزل_سرگرم = سچ;
		وإلا إذا (label == لگاتار_منزل || (لگاتار_from == لگاتار_منزل && !لگاتار_from.خالی_تھاواں()))
			from_منزل_سرگرم = خطأ;
	}
}

منطقي لگاتار_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_منزل_label)
{
	إذا (design == خالی_سوچک)
		design = yosys_design;

	إذا (command == "auto") {
	  std::string filename_trim = filename;

	  خودکار has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
	    إذا (filename.ناپ() >= extension.ناپ()) {
	      أرجع filename.compare(filename.ناپ() - extension.ناپ(), extension.ناپ(), extension) == 0;
	    }
	    أرجع خطأ;
	  };

	  إذا (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.ناپ() - 3);
	  }

	  إذا (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } وإلا إذا (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } وإلا إذا (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } وإلا إذا (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } وإلا إذا (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } وإلا إذا (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } وإلا إذا (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } وإلا إذا (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } وإلا إذا (filename == "-") {
	    command = "script";
	  } وإلا {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	إذا (command == "script")
	{
		std::string لگاتار_from, لگاتار_منزل;
		منطقي from_منزل_سرگرم = سچ;

		إذا (from_منزل_label != عدم) {
			ناپ_t pos = from_منزل_label->لبھو(':');
			إذا (pos == std::string::npos) {
				لگاتار_from = *from_منزل_label;
				لگاتار_منزل = *from_منزل_label;
			} وإلا {
				لگاتار_from = from_منزل_label->substr(0, pos);
				لگاتار_منزل = from_منزل_label->substr(pos+1);
			}
			from_منزل_سرگرم = لگاتار_from.خالی_تھاواں();
		}

		لوگارتھم("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		إذا (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_داخلہ_files.داخل_کرو(filename);
		}

		إذا (f == عدم)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		صحيح backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		حاول {
			std::string command;
			طالما (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				طالما (!command.خالی_تھاواں() && command[command.ناپ()-1] == '\\') {
					std::string اگلا_سطر;
					إذا (!fgetline(f, اگلا_سطر))
						اكسر;
					Frontend::current_script_lineno++;
					command.resize(command.ناپ()-1);
					command += اگلا_سطر;
				}
				handle_label(command, from_منزل_سرگرم, لگاتار_from, لگاتار_منزل);
				إذا (from_منزل_سرگرم) {
					Pass::call(design, command);
					design->check();
				}
			}

			إذا (!command.خالی_تھاواں()) {
				handle_label(command, from_منزل_سرگرم, لگاتار_from, لگاتار_منزل);
				إذا (from_منزل_سرگرم) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		امسك (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ارم;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		إذا (filename != "-")
			fclose(f);

		أرجع سچ;
	}

	إذا (command == "tcl") {
		Pass::call(design, سمتیہ<string>({command, filename}));
		أرجع سچ;
	}

	لوگارتھم("\n");
	إذا (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} وإلا {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	إذا (command[0] == ' ') {
		خودکار argv = ونڈو_ٹوکن("read" + command);
		argv.ڈالو_پٹھ(filename);
		Pass::call(design, argv);
	} وإلا
		Frontend::frontend_call(design, عدم, filename, command);

	design->check();
	أرجع خطأ;
}

فراغ لگاتار_pass(std::string command, RTLIL::Design *design)
{
	إذا (design == خالی_سوچک)
		design = yosys_design;

	لوگارتھم("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

فراغ لگاتار_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	إذا (design == خالی_سوچک)
		design = yosys_design;

	إذا (command == "auto") {
		إذا (filename.ناپ() > 2 && filename.compare(filename.ناپ()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		وإلا إذا (filename.ناپ() > 3 && filename.compare(filename.ناپ()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		وإلا إذا (filename.ناپ() > 3 && filename.compare(filename.ناپ()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		وإلا إذا (filename.ناپ() > 3 && filename.compare(filename.ناپ()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		وإلا إذا (filename.ناپ() > 4 && filename.compare(filename.ناپ()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		وإلا إذا (filename.ناپ() > 5 && filename.compare(filename.ناپ()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		وإلا إذا (filename.ناپ() > 5 && filename.compare(filename.ناپ()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		وإلا إذا (filename.ناپ() > 5 && filename.compare(filename.ناپ()-5, std::string::npos, ".json") == 0)
			command = "json";
		وإلا إذا (filename == "-")
			command = "rtlil";
		وإلا إذا (filename.خالی_تھاواں())
			أرجع;
		وإلا
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	إذا (filename.خالی_تھاواں())
		filename = "-";

	لوگارتھم("\n");
	إذا (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} وإلا {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, عدم, filename, command);
}

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ساكن حرف *readline_cmd_generator(ثابت حرف *متن, صحيح state)
{
	ساكن std::map<std::string, Pass*>::iterator it;
	ساكن صحيح len;

	إذا (!state) {
		it = pass_رجسٹر.begin();
		len = لڑی_لمبائی(متن);
	}

	لكل (; it != pass_رجسٹر.سرا(); it++) {
		إذا (it->پہلا.compare(0, len, متن) == 0)
			أرجع strdup((it++)->پہلا.c_str());
	}
	أرجع عدم;
}

ساكن حرف *readline_obj_generator(ثابت حرف *متن, صحيح state)
{
	ساكن std::سمتیہ<حرف*> obj_ناں;
	ساكن ناپ_t idx;

	إذا (!state)
	{
		idx = 0;
		obj_ناں.clear();

		RTLIL::Design *design = yosys_get_design();
		صحيح len = لڑی_لمبائی(متن);

		إذا (design->selected_سرگرم_module.خالی_تھاواں())
		{
			لكل (خودکار mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				إذا (mod_name.compare(0, len, متن) == 0)
					obj_ناں.ڈالو_پٹھ(strdup(mod_name.c_str()));
			}
		}
		وإلا إذا (design->module(design->selected_سرگرم_module) != خالی_سوچک)
		{
			RTLIL::Module *module = design->module(design->selected_سرگرم_module);

			لكل (خودکار w : module->wires())
				إذا (w->name.unescape().compare(0, len, متن) == 0)
					obj_ناں.ڈالو_پٹھ(strdup(w->name.unescape().c_str()));

			لكل (خودکار &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.پہلا);
				إذا (mem_name.compare(0, len, متن) == 0)
					obj_ناں.ڈالو_پٹھ(strdup(mem_name.c_str()));
			}

			لكل (خودکار cell : module->cells())
				إذا (cell->name.unescape().compare(0, len, متن) == 0)
					obj_ناں.ڈالو_پٹھ(strdup(cell->name.unescape().c_str()));

			لكل (خودکار &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.پہلا);
				إذا (proc_name.compare(0, len, متن) == 0)
					obj_ناں.ڈالو_پٹھ(strdup(proc_name.c_str()));
			}
		}

		std::ترتیب_دیو(obj_ناں.begin(), obj_ناں.سرا());
	}

	إذا (idx < obj_ناں.ناپ())
		أرجع strdup(obj_ناں[idx++]);

	idx = 0;
	obj_ناں.clear();
	أرجع عدم;
}

ساكن حرف **readline_completion(ثابت حرف *متن, صحيح شروع, صحيح)
{
	إذا (شروع == 0)
		أرجع rl_completion_matches(متن, readline_cmd_generator);
	إذا (لڑی_مقابلہ(rl_سطر_بفر, "read_", 5) && لڑی_مقابلہ(rl_سطر_بفر, "write_", 6))
		أرجع rl_completion_matches(متن, readline_obj_generator);
	أرجع عدم;
}
#جے_انت

فراغ shell(RTLIL::Design *design)
{
	ساكن صحيح recursion_counter = 0;

	recursion_counter++;
	خودکار guard = logger().error_throw_scope();

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (حرف*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_لفظ_break_characters = (حرف*)" \t\n";
#جے_انت

	حرف *command = عدم;
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	طالما ((command = readline(create_prompt(design, recursion_counter))) != عدم)
	{
#وإلا
	حرف command_بفر[4096];
	طالما (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		إذا ((command = fgets(command_بفر, 4096, stdin)) == عدم)
			اكسر;
#جے_انت
		إذا (command[strspn(command, " \t\r\n")] == 0) {
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			حرر(command);
#جے_انت
			استمر;
		}
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		جوڑو_history(command);
#جے_انت

		حرف *p = command + strspn(command, " \t\r\n");
		إذا (!لڑی_مقابلہ(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			إذا (*p == 0)
				اكسر;
		}

		حاول {
			log_assert(design->selection_ڈھیر.ناپ() == 1);
			Pass::call(design, command);
		} امسك (log_cmd_error_exception) {
			طالما (design->selection_ڈھیر.ناپ() > 1)
				design->نکالو_selection();
			log_reset_ڈھیر();
		}
		design->check();
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		إذا (command)
			حرر(command);
#جے_انت
	}
	إذا (command == عدم)
		لکھو("exit\n");
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وإلا
		حرر(command);
#جے_انت
	recursion_counter--;
}

بنية ShellPass : عام Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	فراغ help() override {
		لوگارتھم("\n");
		لوگارتھم("    shell\n");
		لوگارتھم("\n");
		لوگارتھم("This command enters the interactive command mode. This can be useful\n");
		لوگارتھم("in a script to interrupt the script at a certain point and allow for\n");
		لوگارتھم("interactive inspection or manual synthesis of the design at this point.\n");
		لوگارتھم("\n");
		لوگارتھم("The command prompt of the interactive shell indicates the current\n");
		لوگارتھم("selection (see 'help select'):\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys>\n");
		لوگارتھم("        the entire design is selected\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys*>\n");
		لوگارتھم("        only part of the design is selected\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys [modname]>\n");
		لوگارتھم("        the entire module 'modname' is selected using 'select -module modname'\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys [modname]*>\n");
		لوگارتھم("        only part of current module 'modname' is selected\n");
		لوگارتھم("\n");
		لوگارتھم("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		لوگارتھم("do not terminate yosys but return to the command prompt.\n");
		لوگارتھم("\n");
		لوگارتھم("This command is the default action if nothing else has been specified\n");
		لوگارتھم("on the command line.\n");
		لوگارتھم("\n");
		لوگارتھم("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		لوگارتھم("\n");
	}
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) override {
		extra_دلیلاں(دلیلاں_2, 1, design, خطأ);
		shell(design);
	}
} ShellPass;

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
بنية HistoryPass : عام Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	فراغ help() override {
		لوگارتھم("\n");
		لوگارتھم("    history\n");
		لوگارتھم("\n");
		لوگارتھم("This command prints all commands in the shell history buffer. This are\n");
		لوگارتھم("all commands executed in an interactive session, but not the commands\n");
		لوگارتھم("from executed scripts.\n");
		لوگارتھم("\n");
	}
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) override {
		extra_دلیلاں(دلیلاں_2, 1, design, خطأ);
#جے_تعریف_شدہ YOSYS_ENABLE_READLINE
		لكل(HIST_اندراج **list = history_list(); *list != عدم; list++)
			لوگارتھم("%s\n", (*list)->سطر);
#وإلا
		لكل (صحيح i = where_history(); history_get(i); i++)
			لوگارتھم("%s\n", history_get(i)->سطر);
#جے_انت
	}
} HistoryPass;
#جے_انت

بنية ScriptCmdPass : عام Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	فراغ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لوگارتھم("\n");
		لوگارتھم("    script <filename> [<from_label>:<to_label>]\n");
		لوگارتھم("    script -scriptwire [selection]\n");
		لوگارتھم("\n");
		لوگارتھم("This command executes the yosys commands in the specified file (default\n");
		لوگارتھم("behaviour), or commands embedded in the constant text value connected to the\n");
		لوگارتھم("selected wires.\n");
		لوگارتھم("\n");
		لوگارتھم("In the default (file) case, the 2nd argument can be used to only execute the\n");
		لوگارتھم("section of the file between the specified labels. An empty from label is\n");
		لوگارتھم("synonymous with the beginning of the file and an empty to label is synonymous\n");
		لوگارتھم("with the end of the file.\n");
		لوگارتھم("\n");
		لوگارتھم("If only one label is specified (without ':') then only the block\n");
		لوگارتھم("marked with that label (until the next label) is executed.\n");
		لوگارتھم("\n");
		لوگارتھم("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		لوگارتھم("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		لوگارتھم("'-module' mode can be exited by using the 'cd' command.\n");
		لوگارتھم("\n");
	}
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) override
	{
		منطقي scriptwire = خطأ;

		ناپ_t argidx;
		لكل (argidx = 1; argidx < دلیلاں_2.ناپ(); argidx++) {
			إذا (دلیلاں_2[argidx] == "-scriptwire") {
				scriptwire = سچ;
				استمر;
			}
			اكسر;
		}
		إذا (scriptwire) {
			extra_دلیلاں(دلیلاں_2, argidx, design);

			لكل (خودکار mod : design->selected_modules())
				لكل (خودکار &c : mod->connections()) {
					إذا (!c.پہلا.is_wire())
						استمر;
					خودکار w = c.پہلا.as_wire();
					إذا (!mod->selected(w))
						استمر;
					إذا (!c.دوجا.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					خودکار v = c.دوجا.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		وإلا إذا (دلیلاں_2.ناپ() < 2)
			log_cmd_error("Missing script file.\n");
		وإلا إذا (دلیلاں_2.ناپ() == 2)
			لگاتار_frontend(دلیلاں_2[1], "script", design);
		وإلا إذا (دلیلاں_2.ناپ() == 3)
			لگاتار_frontend(دلیلاں_2[1], "script", design, &دلیلاں_2[2]);
		وإلا
			extra_دلیلاں(دلیلاں_2, 2, design, خطأ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_سرا
