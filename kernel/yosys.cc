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

#اگر_تعریف_شده YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_TCL
#  include <tcl.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#پایان_اگر

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
#پایان_اگر

#إذا !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#پایان_اگر

#إذا defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
خارجي "C" PyObject* PyInit_libyosys();
خارجي "C" PyObject* PyInit_pyosys();
#ضمن <signal.h>
#پایان_اگر

#ضمن <limits.h>
#ضمن <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحيح yosys_xtrace = 0;
منطقي yosys_write_versions = درست;
ثابت حرف* yosys_maybe_نسخه() {
	إذا (yosys_write_versions)
		أرجع yosys_نسخه_str;
	وإلا
		أرجع "Yosys";
}

RTLIL::Design *yosys_design = عدم;
NewCellTypes yosys_celltypes;

#اگر_تعریف_شده YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = عدم;
Tcl_Interp *yosys_sdc_interp = عدم;
#پایان_اگر

std::مجموعه<std::string> yosys_درون_داد_files, yosys_output_files;

منطقي memhasher_فعال = خطأ;
uint32_t memhasher_rng = 123456;
std::وکتور<فراغ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقي Multithreading::فعال_2 = خطأ;

فراغ init_share_dirname();
فراغ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!فعال_2);
	فعال_2 = درست;
}

Multithreading::~Multithreading() {
	log_assert(فعال_2);
	فعال_2 = خطأ;
}

فراغ Autoidx::ensure_at_کمترین(صحيح v) {
	log_assert(!Multithreading::فعال());
	مقدار = std::max(مقدار, v);
}

صحيح Autoidx::عملگر++(صحيح) {
	log_assert(!Multithreading::فعال());
	أرجع مقدار++;
}

فراغ memhasher_on()
{
#إذا defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += زمان_2(عدم) << 16 ^ getpid();
#پایان_اگر
	memhasher_store.resize(0x10000);
	memhasher_فعال = درست;
}

فراغ memhasher_off()
{
	لكل (خودکار p : memhasher_store)
		إذا (p) حرر(p);
	memhasher_store.clear();
	memhasher_فعال = خطأ;
}

فراغ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	صحيح اندازه, شاخص = (memhasher_rng >> 4) & 0xffff;
	بدل (memhasher_rng & 7) {
		حالة 0: اندازه =   16; اكسر;
		حالة 1: اندازه =  256; اكسر;
		حالة 2: اندازه = 1024; اكسر;
		حالة 3: اندازه = 4096; اكسر;
		افتراضي: اندازه = 0;
	}
	إذا (شاخص < 16) اندازه *= 16;
	memhasher_store[شاخص] = realloc(memhasher_store[شاخص], اندازه);
}

فراغ yosys_banner()
{
	لگاریتم("\n");
	لگاریتم(" /----------------------------------------------------------------------------\\\n");
	لگاریتم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	لگاریتم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	لگاریتم(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	لگاریتم(" \\----------------------------------------------------------------------------/\n");
	لگاریتم(" %s\n", yosys_maybe_نسخه());
}

#إذا defined(YOSYS_ENABLE_SPAWN)
صحيح پیاپی_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_سطر)
{
	إذا (!process_سطر)
		أرجع system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	إذا (f == اشاره_تهی)
		أرجع -1;

	std::string سطر;
	حرف logbuf[128];
	طالما (درست) {
		إذا (fgets(logbuf, 128, f) != عدم) {
			سطر += logbuf;
			إذا (!سطر.خالی() && سطر.پشت() == '\n')
				process_سطر(سطر), سطر.clear();
		} وإلا إذا (ferror(f) && errno == EINTR) {
			clearerr(f);
			استمر;
		} وإلا {
			اكسر;
		}
	}
	إذا (!سطر.خالی())
		process_سطر(سطر);

	صحيح ret = pclose(f);
	إذا (ret < 0)
		أرجع -2;
#اگر_تعریف_شده _WIN32
	أرجع ret;
#وإلا
	إذا (!WIFEXITED(ret))
		أرجع -1;
	أرجع WEXITSTATUS(ret);
#پایان_اگر
}
#پایان_اگر

منطقي already_راهاندازی = خطأ;
منطقي already_shutdown = خطأ;

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.جمع_کن_object("__path__", py::list());
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
#پایان_اگر

فراغ yosys_راهاندازی()
{
	إذا(already_راهاندازی)
		أرجع;
	already_راهاندازی = درست;
	already_shutdown = خطأ;

	StaticTwines::init();

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
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
		Py_پیاپی_Simple_String("import sys");
		إشارة(SIGINT, SIG_DFL);
	}
#پایان_اگر

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_ثبات();
	yosys_design = جديد RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_معلوم;
	log_افزودن();
}

منطقي yosys_already_راهاندازی()
{
	أرجع already_راهاندازی;
}

فراغ yosys_shutdown()
{
	إذا(already_shutdown)
		أرجع;
	already_راهاندازی = خطأ;
	already_shutdown = درست;
	log_برداشت();

	Pass::انجامشده_ثبات();

	حذف yosys_design;
	yosys_design = عدم;

	logger().clear();

#اگر_تعریف_شده YOSYS_ENABLE_TCL
	إذا (yosys_tcl_interp != عدم) {
		إذا (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = عدم;
	}
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_PLUGINS
	لكل (خودکار &it : loaded_plugins)
		dlclose(it.دومی);

	loaded_plugins.clear();
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#پایان_اگر
	loaded_plugin_aliases.clear();
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	Py_Finalize();
#پایان_اگر
}

ثابت std::string *create_id_پیشوند(std::string_view file, صحيح سطر, std::string_view func)
{
#اگر_تعریف_شده _WIN32
	اندازه_t pos = file.پیدا_کن_آخرین_از("/\\");
#وإلا
	اندازه_t pos = file.پیدا_کن_آخرین_از('/');
#پایان_اگر
	إذا (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.پیدا_کن_آخرین_از(':');
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
	ساكن حرف میانگیر[100];
	std::string str = newline ? "\n" : "";
	إذا (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	إذا (!design->selected_فعال_module.خالی())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_فعال_module)).c_str());
	إذا (!design->پر_selection()) {
		إذا (design->selected_فعال_module.خالی())
			str += "*";
		وإلا إذا (design->selection().selected_modules.اندازه() != 1 || design->selection().selected_members.اندازه() != 0 ||
				design->selection().selected_modules.شمار(design->selected_فعال_module) == 0)
			str += "*";
	}
	snprintf(میانگیر, 100, "%s> ", str.c_str());
	أرجع میانگیر;
}

std::وکتور<std::string> glob_filename(ثابت std::string &filename_الگو)
{
	std::وکتور<std::string> results;

#إذا defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.افزودن_پشت(filename_الگو);
#وإلا
	glob_t globbuf;

	صحيح err = glob(filename_الگو.c_str(), 0, عدم, &globbuf);

	إذا(err == 0) {
		لكل (اندازه_t i = 0; i < globbuf.gl_pathc; i++)
			results.افزودن_پشت(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} وإلا {
		results.افزودن_پشت(filename_الگو);
	}
#پایان_اگر

	أرجع results;
}

فراغ rewrite_filename(std::string &filename)
{
	إذا (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_اندازه(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_اندازه(filename)-2);
	إذا (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#اگر_تعریف_نشده _WIN32
	إذا (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#پایان_اگر
}

#اگر_تعریف_شده YOSYS_ENABLE_TCL

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
		لگاریتم("\n");
		لگاریتم("    tcl <filename> [args]\n");
		لگاریتم("\n");
		لگاریتم("This command executes the tcl commands in the specified file.\n");
		لگاریتم("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		لگاریتم("\n");
		لگاریتم("The tcl command 'yosys -import' can be used to import all yosys\n");
		لگاریتم("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		لگاریتم("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		لگاریتم("in order to avoid a name collision with the built in commands.\n");
		لگاریتم("\n");
		لگاریتم("If any arguments are specified, these arguments are provided to the script via\n");
		لگاریتم("the standard $argc and $argv variables.\n");
		لگاریتم("\n");
		لگاریتم("Note, tcl will not receive the output of any yosys command. If the output\n");
		لگاریتم("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		لگاریتم("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		لگاریتم("The 'result.string' value is then used as the tcl output value of the command.\n");
		لگاریتم("\n");
	}
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *) override {
		إذا (آرگومانها_2.اندازه() < 2)
			log_cmd_error("Missing script file.\n");

		std::وکتور<Tcl_Obj*> script_آرگومانها;
		لكل (خودکار it = آرگومانها_2.begin() + 2; it != آرگومانها_2.انتها(); ++it)
			script_آرگومانها.افزودن_پشت(Tcl_NewStringObj((*it).c_str(), (*it).اندازه()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_مجموعه_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(script_آرگومانها.اندازه()), 0);
		Tcl_Obj_مجموعه_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(script_آرگومانها.اندازه(), script_آرگومانها.داده()), 0);
		Tcl_Obj_مجموعه_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(آرگومانها_2[1].c_str(), آرگومانها_2[1].اندازه()), 0);
		إذا (Tcl_EvalFile(interp, آرگومانها_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتیجه(interp));
		Tcl_Release(interp);
	}
} TclPass;

#پایان_اگر

#إذا defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string مسیر_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	لكل (صحيح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", مسیر_2.داده(), مسیر_2.اندازه());
		إذا (buflen < (ssize_t)مسیر_2.اندازه())
			اكسر;
		وإلا
			مسیر_2.resize(مسیر_2.اندازه() * 2);
	}
	إذا (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		مسیر_2.resize(0);
	} وإلا {
		طالما (buflen > 0 && مسیر_2[buflen-1] != '/')
			buflen--;
		مسیر_2.resize(buflen);
	}
	أرجع مسیر_2;
}
#وإلاإذا defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#اگر_تعریف_شده __NetBSD__
	صحيح mib[4] = {CTL_KERN, KERN_PROC_آرگومانها, getpid(), KERN_PROC_PATHNAME};
#وإلا
	صحيح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#پایان_اگر
	اندازه_t buflen;
	حرف *میانگیر;
	std::string مسیر_2;
	إذا (sysctl(mib, 4, عدم, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	میانگیر = (حرف*)حافظه_بده(buflen);
	إذا (میانگیر == عدم)
		log_error("malloc failed: %s\n", strerror(errno));
	إذا (sysctl(mib, 4, میانگیر, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	طالما (buflen > 0 && میانگیر[buflen-1] != '/')
		buflen--;
	مسیر_2.تخصیص_بده(میانگیر, buflen);
	حرر(میانگیر);
	أرجع مسیر_2;
}
#وإلاإذا defined(__APPLE__)
std::string proc_self_dirname()
{
	حرف *مسیر_2 = عدم;
	uint32_t buflen = 0;
	طالما (NSGet_Executable_مسیر(مسیر_2, &buflen) != 0)
		مسیر_2 = (حرف *) realloc((فراغ *) مسیر_2, buflen);
	حرف resolved[مسیر_MAX];
	إذا (realpath(مسیر_2, resolved) != عدم) {
		حرر(مسیر_2);
		مسیر_2 = strdup(resolved);
		buflen = طول_رشته(مسیر_2);
	}
	طالما (buflen > 0 && مسیر_2[buflen-1] != '/')
		buflen--;
	std::string str(مسیر_2, buflen);
	حرر(مسیر_2);
	أرجع str;
}
#وإلاإذا defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	إذا (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.اندازه()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wbinpath.داده(), wbinpath.اندازه(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wbinpath.داده(), wbinpath.اندازه(), &ubinpath[0], ubinpath.اندازه(), عدم, عدم) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	أرجع ubinpath;

}
#وإلاإذا defined(__wasm)
std::string proc_self_dirname()
{
	أرجع "/";
}
#وإلاإذا defined(__OpenBSD__) || defined(__HAIKU__)
حرف yosys_مسیر[مسیر_MAX];
حرف *yosys_argv0;

std::string proc_self_dirname(فراغ)
{
	حرف buf[مسیر_MAX + 1] = "", *مسیر_2, *p;
	// if case argv[0] contains a valid path, return it
	إذا (طول_رشته(yosys_مسیر) > 0) {
		p = strrchr(yosys_مسیر, '/');
		snprintf(buf, حجم buf, "%*s/", (صحيح)(yosys_مسیر - p), yosys_مسیر);
		أرجع buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	مسیر_2 = strdup(getenv("PATH"));
	إذا (!مسیر_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	لكل (p = strtok(مسیر_2, ":"); p; p = strtok(عدم, ":")) {
		snprintf(buf, حجم buf, "%s/%s", p, yosys_argv0);
		إذا (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			حرر(مسیر_2);
			أرجع buf;
		}
	}
	حرر(مسیر_2);
	log_error("Can't determine yosys executable path.\n");
	أرجع عدم;
}
#وإلا
	#خطا "Don't know how to determine process executable base path!"
#پایان_اگر

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
		PyObject *share_مسیر_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت حرف *share_مسیر = PyUnicode_AsUTF8(share_مسیر_obj);
		yosys_share_dirname = std::string(share_مسیر);
		أرجع;
	}
#  endif
	std::string proc_self_مسیر = proc_self_dirname();
#  إذا defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_مسیر = proc_self_مسیر + "share\\";
	إذا (check_directory_exists(proc_share_مسیر, درست)) {
		yosys_share_dirname = proc_share_مسیر;
		أرجع;
	}
	proc_share_مسیر = proc_self_مسیر + "..\\share\\";
	إذا (check_directory_exists(proc_share_مسیر, درست)) {
		yosys_share_dirname = proc_share_مسیر;
		أرجع;
	}
#  وإلا
	std::string proc_share_مسیر = proc_self_مسیر + "share/";
	إذا (check_directory_exists(proc_share_مسیر, درست)) {
		yosys_share_dirname = proc_share_مسیر;
		أرجع;
	}
	proc_share_مسیر = proc_self_مسیر + "../share/" + proc_برنامه_پیشوند()+ "yosys/";
	إذا (check_directory_exists(proc_share_مسیر, درست)) {
		yosys_share_dirname = proc_share_مسیر;
		أرجع;
	}
	proc_share_مسیر = YOSYS_DATDIR "/";
	إذا (check_directory_exists(proc_share_مسیر, درست)) {
		yosys_share_dirname = proc_share_مسیر;
		أرجع;
	}
#  endif
}
#پایان_اگر

فراغ init_abc_executable_name()
{
#اگر_تعریف_شده ABCEXTERNAL
	std::string exe_file;
	إذا (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} وإلا {
		yosys_abc_executable = ABCEXTERNAL;
	}
#وإلا
	yosys_abc_executable = proc_self_dirname() + proc_برنامه_پیشوند()+ "yosys-abc";
#  ifdef _WIN32
	إذا (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_برنامه_پیشوند() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_برنامه_پیشوند() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_مسیر_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت حرف *abc_مسیر = PyUnicode_AsUTF8(abc_مسیر_obj);
		yosys_abc_executable = std::string(abc_مسیر);
	}
#  endif
#پایان_اگر
}

std::string proc_share_dirname()
{
	إذا (yosys_share_dirname.خالی())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	أرجع yosys_share_dirname;
}

std::string proc_برنامه_پیشوند()
{
	أرجع YOSYS_برنامه_پیشوند;
}

منطقي fgetline(FILE *f, std::string &میانگیر)
{
	میانگیر = "";
	حرف بلوک[4096];
	طالما (1) {
		إذا (fgets(بلوک, 4096, f) == عدم)
			أرجع خطأ;
		میانگیر += بلوک;
		إذا (میانگیر.اندازه() > 0 && (میانگیر[میانگیر.اندازه()-1] == '\n' ||  میانگیر[میانگیر.اندازه()-1] == '\r')) {
			طالما (میانگیر.اندازه() > 0 && (میانگیر[میانگیر.اندازه()-1] == '\n' ||  میانگیر[میانگیر.اندازه()-1] == '\r'))
				میانگیر.resize(میانگیر.اندازه()-1);
			أرجع درست;
		}
	}
}

ساكن فراغ handle_label(std::string &command, منطقي &from_مقصد_فعال, ثابت std::string &پیاپی_from, ثابت std::string &پیاپی_مقصد)
{
	صحيح pos = 0;
	std::string label;

	طالما (pos < Get_اندازه(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	إذا (pos < Get_اندازه(command) && command[pos] == '#')
		أرجع;

	طالما (pos < Get_اندازه(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	إذا (Get_اندازه(label) > 1 && label.پشت() == ':')
	{
		label = label.substr(0, Get_اندازه(label)-1);
		command = command.substr(pos);

		إذا (label == پیاپی_from)
			from_مقصد_فعال = درست;
		وإلا إذا (label == پیاپی_مقصد || (پیاپی_from == پیاپی_مقصد && !پیاپی_from.خالی()))
			from_مقصد_فعال = خطأ;
	}
}

منطقي پیاپی_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_مقصد_label)
{
	إذا (design == اشاره_تهی)
		design = yosys_design;

	إذا (command == "auto") {
	  std::string filename_trim = filename;

	  خودکار has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
	    إذا (filename.اندازه() >= extension.اندازه()) {
	      أرجع filename.compare(filename.اندازه() - extension.اندازه(), extension.اندازه(), extension) == 0;
	    }
	    أرجع خطأ;
	  };

	  إذا (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.اندازه() - 3);
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
		std::string پیاپی_from, پیاپی_مقصد;
		منطقي from_مقصد_فعال = درست;

		إذا (from_مقصد_label != عدم) {
			اندازه_t pos = from_مقصد_label->پیدا_کن(':');
			إذا (pos == std::string::npos) {
				پیاپی_from = *from_مقصد_label;
				پیاپی_مقصد = *from_مقصد_label;
			} وإلا {
				پیاپی_from = from_مقصد_label->substr(0, pos);
				پیاپی_مقصد = from_مقصد_label->substr(pos+1);
			}
			from_مقصد_فعال = پیاپی_from.خالی();
		}

		لگاریتم("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		إذا (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_درون_داد_files.درج(filename);
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
				طالما (!command.خالی() && command[command.اندازه()-1] == '\\') {
					std::string بعدی_سطر;
					إذا (!fgetline(f, بعدی_سطر))
						اكسر;
					Frontend::current_script_lineno++;
					command.resize(command.اندازه()-1);
					command += بعدی_سطر;
				}
				handle_label(command, from_مقصد_فعال, پیاپی_from, پیاپی_مقصد);
				إذا (from_مقصد_فعال) {
					Pass::call(design, command);
					design->check();
				}
			}

			إذا (!command.خالی()) {
				handle_label(command, from_مقصد_فعال, پیاپی_from, پیاپی_مقصد);
				إذا (from_مقصد_فعال) {
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

		أرجع درست;
	}

	إذا (command == "tcl") {
		Pass::call(design, وکتور<string>({command, filename}));
		أرجع درست;
	}

	لگاریتم("\n");
	إذا (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} وإلا {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	إذا (command[0] == ' ') {
		خودکار argv = تقسیم_نشانهها("read" + command);
		argv.افزودن_پشت(filename);
		Pass::call(design, argv);
	} وإلا
		Frontend::frontend_call(design, عدم, filename, command);

	design->check();
	أرجع خطأ;
}

فراغ پیاپی_pass(std::string command, RTLIL::Design *design)
{
	إذا (design == اشاره_تهی)
		design = yosys_design;

	لگاریتم("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

فراغ پیاپی_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	إذا (design == اشاره_تهی)
		design = yosys_design;

	إذا (command == "auto") {
		إذا (filename.اندازه() > 2 && filename.compare(filename.اندازه()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		وإلا إذا (filename.اندازه() > 3 && filename.compare(filename.اندازه()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		وإلا إذا (filename.اندازه() > 3 && filename.compare(filename.اندازه()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		وإلا إذا (filename.اندازه() > 3 && filename.compare(filename.اندازه()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		وإلا إذا (filename.اندازه() > 4 && filename.compare(filename.اندازه()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		وإلا إذا (filename.اندازه() > 5 && filename.compare(filename.اندازه()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		وإلا إذا (filename.اندازه() > 5 && filename.compare(filename.اندازه()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		وإلا إذا (filename.اندازه() > 5 && filename.compare(filename.اندازه()-5, std::string::npos, ".json") == 0)
			command = "json";
		وإلا إذا (filename == "-")
			command = "rtlil";
		وإلا إذا (filename.خالی())
			أرجع;
		وإلا
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	إذا (filename.خالی())
		filename = "-";

	لگاریتم("\n");
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
		it = pass_ثبات.begin();
		len = طول_رشته(متن);
	}

	لكل (; it != pass_ثبات.انتها(); it++) {
		إذا (it->اولی.compare(0, len, متن) == 0)
			أرجع strdup((it++)->اولی.c_str());
	}
	أرجع عدم;
}

ساكن حرف *readline_obj_generator(ثابت حرف *متن, صحيح state)
{
	ساكن std::وکتور<حرف*> obj_نامها;
	ساكن اندازه_t idx;

	إذا (!state)
	{
		idx = 0;
		obj_نامها.clear();

		RTLIL::Design *design = yosys_get_design();
		صحيح len = طول_رشته(متن);

		إذا (design->selected_فعال_module.خالی())
		{
			لكل (خودکار mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				إذا (mod_name.compare(0, len, متن) == 0)
					obj_نامها.افزودن_پشت(strdup(mod_name.c_str()));
			}
		}
		وإلا إذا (design->module(design->selected_فعال_module) != اشاره_تهی)
		{
			RTLIL::Module *module = design->module(design->selected_فعال_module);

			لكل (خودکار w : module->wires())
				إذا (w->name.unescape().compare(0, len, متن) == 0)
					obj_نامها.افزودن_پشت(strdup(w->name.unescape().c_str()));

			لكل (خودکار &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.اولی);
				إذا (mem_name.compare(0, len, متن) == 0)
					obj_نامها.افزودن_پشت(strdup(mem_name.c_str()));
			}

			لكل (خودکار cell : module->cells())
				إذا (cell->name.unescape().compare(0, len, متن) == 0)
					obj_نامها.افزودن_پشت(strdup(cell->name.unescape().c_str()));

			لكل (خودکار &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.اولی);
				إذا (proc_name.compare(0, len, متن) == 0)
					obj_نامها.افزودن_پشت(strdup(proc_name.c_str()));
			}
		}

		std::مرتب_سازی(obj_نامها.begin(), obj_نامها.انتها());
	}

	إذا (idx < obj_نامها.اندازه())
		أرجع strdup(obj_نامها[idx++]);

	idx = 0;
	obj_نامها.clear();
	أرجع عدم;
}

ساكن حرف **readline_completion(ثابت حرف *متن, صحيح شروع, صحيح)
{
	إذا (شروع == 0)
		أرجع rl_completion_matches(متن, readline_cmd_generator);
	إذا (مقایسه_رشته(rl_سطر_میانگیر, "read_", 5) && مقایسه_رشته(rl_سطر_میانگیر, "write_", 6))
		أرجع rl_completion_matches(متن, readline_obj_generator);
	أرجع عدم;
}
#پایان_اگر

فراغ shell(RTLIL::Design *design)
{
	ساكن صحيح recursion_counter = 0;

	recursion_counter++;
	خودکار guard = logger().error_throw_scope();

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (حرف*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_کلمه_break_characters = (حرف*)" \t\n";
#پایان_اگر

	حرف *command = عدم;
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	طالما ((command = readline(create_prompt(design, recursion_counter))) != عدم)
	{
#وإلا
	حرف command_میانگیر[4096];
	طالما (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		إذا ((command = fgets(command_میانگیر, 4096, stdin)) == عدم)
			اكسر;
#پایان_اگر
		إذا (command[strspn(command, " \t\r\n")] == 0) {
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			حرر(command);
#پایان_اگر
			استمر;
		}
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		جمع_کن_history(command);
#پایان_اگر

		حرف *p = command + strspn(command, " \t\r\n");
		إذا (!مقایسه_رشته(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			إذا (*p == 0)
				اكسر;
		}

		حاول {
			log_assert(design->selection_پشته.اندازه() == 1);
			Pass::call(design, command);
		} امسك (log_cmd_error_exception) {
			طالما (design->selection_پشته.اندازه() > 1)
				design->برداشت_selection();
			log_reset_پشته();
		}
		design->check();
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		إذا (command)
			حرر(command);
#پایان_اگر
	}
	إذا (command == عدم)
		چاپ_کن("exit\n");
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وإلا
		حرر(command);
#پایان_اگر
	recursion_counter--;
}

بنية ShellPass : عام Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	فراغ help() override {
		لگاریتم("\n");
		لگاریتم("    shell\n");
		لگاریتم("\n");
		لگاریتم("This command enters the interactive command mode. This can be useful\n");
		لگاریتم("in a script to interrupt the script at a certain point and allow for\n");
		لگاریتم("interactive inspection or manual synthesis of the design at this point.\n");
		لگاریتم("\n");
		لگاریتم("The command prompt of the interactive shell indicates the current\n");
		لگاریتم("selection (see 'help select'):\n");
		لگاریتم("\n");
		لگاریتم("    yosys>\n");
		لگاریتم("        the entire design is selected\n");
		لگاریتم("\n");
		لگاریتم("    yosys*>\n");
		لگاریتم("        only part of the design is selected\n");
		لگاریتم("\n");
		لگاریتم("    yosys [modname]>\n");
		لگاریتم("        the entire module 'modname' is selected using 'select -module modname'\n");
		لگاریتم("\n");
		لگاریتم("    yosys [modname]*>\n");
		لگاریتم("        only part of current module 'modname' is selected\n");
		لگاریتم("\n");
		لگاریتم("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		لگاریتم("do not terminate yosys but return to the command prompt.\n");
		لگاریتم("\n");
		لگاریتم("This command is the default action if nothing else has been specified\n");
		لگاریتم("on the command line.\n");
		لگاریتم("\n");
		لگاریتم("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		لگاریتم("\n");
	}
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) override {
		extra_آرگومانها(آرگومانها_2, 1, design, خطأ);
		shell(design);
	}
} ShellPass;

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
بنية HistoryPass : عام Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	فراغ help() override {
		لگاریتم("\n");
		لگاریتم("    history\n");
		لگاریتم("\n");
		لگاریتم("This command prints all commands in the shell history buffer. This are\n");
		لگاریتم("all commands executed in an interactive session, but not the commands\n");
		لگاریتم("from executed scripts.\n");
		لگاریتم("\n");
	}
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) override {
		extra_آرگومانها(آرگومانها_2, 1, design, خطأ);
#اگر_تعریف_شده YOSYS_ENABLE_READLINE
		لكل(HIST_مدخل **list = history_list(); *list != عدم; list++)
			لگاریتم("%s\n", (*list)->سطر);
#وإلا
		لكل (صحيح i = where_history(); history_get(i); i++)
			لگاریتم("%s\n", history_get(i)->سطر);
#پایان_اگر
	}
} HistoryPass;
#پایان_اگر

بنية ScriptCmdPass : عام Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	فراغ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لگاریتم("\n");
		لگاریتم("    script <filename> [<from_label>:<to_label>]\n");
		لگاریتم("    script -scriptwire [selection]\n");
		لگاریتم("\n");
		لگاریتم("This command executes the yosys commands in the specified file (default\n");
		لگاریتم("behaviour), or commands embedded in the constant text value connected to the\n");
		لگاریتم("selected wires.\n");
		لگاریتم("\n");
		لگاریتم("In the default (file) case, the 2nd argument can be used to only execute the\n");
		لگاریتم("section of the file between the specified labels. An empty from label is\n");
		لگاریتم("synonymous with the beginning of the file and an empty to label is synonymous\n");
		لگاریتم("with the end of the file.\n");
		لگاریتم("\n");
		لگاریتم("If only one label is specified (without ':') then only the block\n");
		لگاریتم("marked with that label (until the next label) is executed.\n");
		لگاریتم("\n");
		لگاریتم("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		لگاریتم("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		لگاریتم("'-module' mode can be exited by using the 'cd' command.\n");
		لگاریتم("\n");
	}
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) override
	{
		منطقي scriptwire = خطأ;

		اندازه_t argidx;
		لكل (argidx = 1; argidx < آرگومانها_2.اندازه(); argidx++) {
			إذا (آرگومانها_2[argidx] == "-scriptwire") {
				scriptwire = درست;
				استمر;
			}
			اكسر;
		}
		إذا (scriptwire) {
			extra_آرگومانها(آرگومانها_2, argidx, design);

			لكل (خودکار mod : design->selected_modules())
				لكل (خودکار &c : mod->connections()) {
					إذا (!c.اولی.is_wire())
						استمر;
					خودکار w = c.اولی.as_wire();
					إذا (!mod->selected(w))
						استمر;
					إذا (!c.دومی.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					خودکار v = c.دومی.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		وإلا إذا (آرگومانها_2.اندازه() < 2)
			log_cmd_error("Missing script file.\n");
		وإلا إذا (آرگومانها_2.اندازه() == 2)
			پیاپی_frontend(آرگومانها_2[1], "script", design);
		وإلا إذا (آرگومانها_2.اندازه() == 3)
			پیاپی_frontend(آرگومانها_2[1], "script", design, &آرگومانها_2[2]);
		وإلا
			extra_آرگومانها(آرگومانها_2, 2, design, خطأ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_انتها
