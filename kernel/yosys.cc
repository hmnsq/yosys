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

#که_تعریف_شوی YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_TCL
#  include <tcl.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#د_که_پای

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
#د_که_پای

#إذا !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#د_که_پای

#إذا defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
خارجي "C" PyObject* PyInit_libyosys();
خارجي "C" PyObject* PyInit_pyosys();
#ضمن <signal.h>
#د_که_پای

#ضمن <limits.h>
#ضمن <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحيح yosys_xtrace = 0;
منطقي yosys_write_versions = ریښتیا;
ثابت حرف* yosys_maybe_نسخه() {
	إذا (yosys_write_versions)
		أرجع yosys_نسخه_str;
	وإلا
		أرجع "Yosys";
}

RTLIL::Design *yosys_design = عدم;
NewCellTypes yosys_celltypes;

#که_تعریف_شوی YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = عدم;
Tcl_Interp *yosys_sdc_interp = عدم;
#د_که_پای

std::سېټ<std::string> yosys_ننوت_files, yosys_output_files;

منطقي memhasher_فعال = خطأ;
uint32_t memhasher_rng = 123456;
std::ویکټور<فراغ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقي Multithreading::فعال_2 = خطأ;

فراغ init_share_dirname();
فراغ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!فعال_2);
	فعال_2 = ریښتیا;
}

Multithreading::~Multithreading() {
	log_assert(فعال_2);
	فعال_2 = خطأ;
}

فراغ Autoidx::ensure_at_لږترلږه(صحيح v) {
	log_assert(!Multithreading::فعال());
	ارزښت = std::max(ارزښت, v);
}

صحيح Autoidx::چلوونکی++(صحيح) {
	log_assert(!Multithreading::فعال());
	أرجع ارزښت++;
}

فراغ memhasher_on()
{
#إذا defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += وخت_2(عدم) << 16 ^ getpid();
#د_که_پای
	memhasher_store.resize(0x10000);
	memhasher_فعال = ریښتیا;
}

فراغ memhasher_off()
{
	لكل (خپلکاری p : memhasher_store)
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
	لوګاریتم("\n");
	لوګاریتم(" /----------------------------------------------------------------------------\\\n");
	لوګاریتم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	لوګاریتم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	لوګاریتم(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	لوګاریتم(" \\----------------------------------------------------------------------------/\n");
	لوګاریتم(" %s\n", yosys_maybe_نسخه());
}

#إذا defined(YOSYS_ENABLE_SPAWN)
صحيح پرلپسې_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_کرښه)
{
	إذا (!process_کرښه)
		أرجع system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	إذا (f == تش_اشاره)
		أرجع -1;

	std::string کرښه;
	حرف logbuf[128];
	طالما (ریښتیا) {
		إذا (fgets(logbuf, 128, f) != عدم) {
			کرښه += logbuf;
			إذا (!کرښه.تشه() && کرښه.شا() == '\n')
				process_کرښه(کرښه), کرښه.clear();
		} وإلا إذا (ferror(f) && errno == EINTR) {
			clearerr(f);
			استمر;
		} وإلا {
			اكسر;
		}
	}
	إذا (!کرښه.تشه())
		process_کرښه(کرښه);

	صحيح ret = pclose(f);
	إذا (ret < 0)
		أرجع -2;
#که_تعریف_شوی _WIN32
	أرجع ret;
#وإلا
	إذا (!WIFEXITED(ret))
		أرجع -1;
	أرجع WEXITSTATUS(ret);
#د_که_پای
}
#د_که_پای

منطقي already_تیاری = خطأ;
منطقي already_shutdown = خطأ;

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.جمع_کړه_object("__path__", py::list());
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
#د_که_پای

فراغ yosys_تیاری()
{
	إذا(already_تیاری)
		أرجع;
	already_تیاری = ریښتیا;
	already_shutdown = خطأ;

	StaticTwines::init();

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
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
		Py_پرلپسې_Simple_String("import sys");
		إشارة(SIGINT, SIG_DFL);
	}
#د_که_پای

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_ثبتوونکی();
	yosys_design = جديد RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_معلوم;
	log_ټېل();
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
	already_shutdown = ریښتیا;
	log_راوباسه();

	Pass::بشپړ_ثبتوونکی();

	ړنګول yosys_design;
	yosys_design = عدم;

	logger().clear();

#که_تعریف_شوی YOSYS_ENABLE_TCL
	إذا (yosys_tcl_interp != عدم) {
		إذا (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = عدم;
	}
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_PLUGINS
	لكل (خپلکاری &it : loaded_plugins)
		dlclose(it.دوهم);

	loaded_plugins.clear();
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#د_که_پای
	loaded_plugin_aliases.clear();
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	Py_Finalize();
#د_که_پای
}

ثابت std::string *create_id_مختاړی(std::string_view file, صحيح کرښه, std::string_view func)
{
#که_تعریف_شوی _WIN32
	اندازه_t pos = file.پیدا_کړه_وروستی_د("/\\");
#وإلا
	اندازه_t pos = file.پیدا_کړه_وروستی_د('/');
#د_که_پای
	إذا (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.پیدا_کړه_وروستی_د(':');
	إذا (pos != std::string_view::npos)
		func = func.substr(pos+1);

	أرجع جديد std::string(stringf("$auto$%s:%d:%s$", file, کرښه, func));
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
	إذا (!design->selected_فعال_module.تشه())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_فعال_module)).c_str());
	إذا (!design->ډک_selection()) {
		إذا (design->selected_فعال_module.تشه())
			str += "*";
		وإلا إذا (design->selection().selected_modules.اندازه() != 1 || design->selection().selected_members.اندازه() != 0 ||
				design->selection().selected_modules.شمېر(design->selected_فعال_module) == 0)
			str += "*";
	}
	snprintf(بفر, 100, "%s> ", str.c_str());
	أرجع بفر;
}

std::ویکټور<std::string> glob_filename(ثابت std::string &filename_نمونه)
{
	std::ویکټور<std::string> results;

#إذا defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ټېل_شا(filename_نمونه);
#وإلا
	glob_t globbuf;

	صحيح err = glob(filename_نمونه.c_str(), 0, عدم, &globbuf);

	إذا(err == 0) {
		لكل (اندازه_t i = 0; i < globbuf.gl_pathc; i++)
			results.ټېل_شا(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} وإلا {
		results.ټېل_شا(filename_نمونه);
	}
#د_که_پای

	أرجع results;
}

فراغ rewrite_filename(std::string &filename)
{
	إذا (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_اندازه(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_اندازه(filename)-2);
	إذا (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#که_نه_تعریف_شوی _WIN32
	إذا (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#د_که_پای
}

#که_تعریف_شوی YOSYS_ENABLE_TCL

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
		لوګاریتم("\n");
		لوګاریتم("    tcl <filename> [args]\n");
		لوګاریتم("\n");
		لوګاریتم("This command executes the tcl commands in the specified file.\n");
		لوګاریتم("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		لوګاریتم("\n");
		لوګاریتم("The tcl command 'yosys -import' can be used to import all yosys\n");
		لوګاریتم("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		لوګاریتم("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		لوګاریتم("in order to avoid a name collision with the built in commands.\n");
		لوګاریتم("\n");
		لوګاریتم("If any arguments are specified, these arguments are provided to the script via\n");
		لوګاریتم("the standard $argc and $argv variables.\n");
		لوګاریتم("\n");
		لوګاریتم("Note, tcl will not receive the output of any yosys command. If the output\n");
		لوګاریتم("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		لوګاریتم("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		لوګاریتم("The 'result.string' value is then used as the tcl output value of the command.\n");
		لوګاریتم("\n");
	}
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *) override {
		إذا (دلیلونه_2.اندازه() < 2)
			log_cmd_error("Missing script file.\n");

		std::ویکټور<Tcl_Obj*> script_دلیلونه;
		لكل (خپلکاری it = دلیلونه_2.begin() + 2; it != دلیلونه_2.پای_ټکی(); ++it)
			script_دلیلونه.ټېل_شا(Tcl_NewStringObj((*it).c_str(), (*it).اندازه()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_سېټ_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(script_دلیلونه.اندازه()), 0);
		Tcl_Obj_سېټ_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(script_دلیلونه.اندازه(), script_دلیلونه.معلومات()), 0);
		Tcl_Obj_سېټ_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(دلیلونه_2[1].c_str(), دلیلونه_2[1].اندازه()), 0);
		إذا (Tcl_EvalFile(interp, دلیلونه_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_پایله(interp));
		Tcl_Release(interp);
	}
} TclPass;

#د_که_پای

#إذا defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string لار_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	لكل (صحيح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", لار_2.معلومات(), لار_2.اندازه());
		إذا (buflen < (ssize_t)لار_2.اندازه())
			اكسر;
		وإلا
			لار_2.resize(لار_2.اندازه() * 2);
	}
	إذا (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		لار_2.resize(0);
	} وإلا {
		طالما (buflen > 0 && لار_2[buflen-1] != '/')
			buflen--;
		لار_2.resize(buflen);
	}
	أرجع لار_2;
}
#وإلاإذا defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#که_تعریف_شوی __NetBSD__
	صحيح mib[4] = {CTL_KERN, KERN_PROC_دلیلونه, getpid(), KERN_PROC_PATHNAME};
#وإلا
	صحيح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#د_که_پای
	اندازه_t buflen;
	حرف *بفر;
	std::string لار_2;
	إذا (sysctl(mib, 4, عدم, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	بفر = (حرف*)حافظه_ورکړه(buflen);
	إذا (بفر == عدم)
		log_error("malloc failed: %s\n", strerror(errno));
	إذا (sysctl(mib, 4, بفر, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	طالما (buflen > 0 && بفر[buflen-1] != '/')
		buflen--;
	لار_2.ټاکل(بفر, buflen);
	حرر(بفر);
	أرجع لار_2;
}
#وإلاإذا defined(__APPLE__)
std::string proc_self_dirname()
{
	حرف *لار_2 = عدم;
	uint32_t buflen = 0;
	طالما (NSGet_Executable_لار(لار_2, &buflen) != 0)
		لار_2 = (حرف *) realloc((فراغ *) لار_2, buflen);
	حرف resolved[لار_MAX];
	إذا (realpath(لار_2, resolved) != عدم) {
		حرر(لار_2);
		لار_2 = strdup(resolved);
		buflen = مزي_اوږدوالی(لار_2);
	}
	طالما (buflen > 0 && لار_2[buflen-1] != '/')
		buflen--;
	std::string str(لار_2, buflen);
	حرر(لار_2);
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
	ubinpath.resize(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wbinpath.معلومات(), wbinpath.اندازه(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wbinpath.معلومات(), wbinpath.اندازه(), &ubinpath[0], ubinpath.اندازه(), عدم, عدم) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	أرجع ubinpath;

}
#وإلاإذا defined(__wasm)
std::string proc_self_dirname()
{
	أرجع "/";
}
#وإلاإذا defined(__OpenBSD__) || defined(__HAIKU__)
حرف yosys_لار[لار_MAX];
حرف *yosys_argv0;

std::string proc_self_dirname(فراغ)
{
	حرف buf[لار_MAX + 1] = "", *لار_2, *p;
	// if case argv[0] contains a valid path, return it
	إذا (مزي_اوږدوالی(yosys_لار) > 0) {
		p = strrchr(yosys_لار, '/');
		snprintf(buf, حجم buf, "%*s/", (صحيح)(yosys_لار - p), yosys_لار);
		أرجع buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	لار_2 = strdup(getenv("PATH"));
	إذا (!لار_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	لكل (p = strtok(لار_2, ":"); p; p = strtok(عدم, ":")) {
		snprintf(buf, حجم buf, "%s/%s", p, yosys_argv0);
		إذا (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			حرر(لار_2);
			أرجع buf;
		}
	}
	حرر(لار_2);
	log_error("Can't determine yosys executable path.\n");
	أرجع عدم;
}
#وإلا
	#تېروتنه "Don't know how to determine process executable base path!"
#د_که_پای

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
		PyObject *share_لار_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت حرف *share_لار = PyUnicode_AsUTF8(share_لار_obj);
		yosys_share_dirname = std::string(share_لار);
		أرجع;
	}
#  endif
	std::string proc_self_لار = proc_self_dirname();
#  إذا defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_لار = proc_self_لار + "share\\";
	إذا (check_directory_exists(proc_share_لار, ریښتیا)) {
		yosys_share_dirname = proc_share_لار;
		أرجع;
	}
	proc_share_لار = proc_self_لار + "..\\share\\";
	إذا (check_directory_exists(proc_share_لار, ریښتیا)) {
		yosys_share_dirname = proc_share_لار;
		أرجع;
	}
#  وإلا
	std::string proc_share_لار = proc_self_لار + "share/";
	إذا (check_directory_exists(proc_share_لار, ریښتیا)) {
		yosys_share_dirname = proc_share_لار;
		أرجع;
	}
	proc_share_لار = proc_self_لار + "../share/" + proc_پروګرام_مختاړی()+ "yosys/";
	إذا (check_directory_exists(proc_share_لار, ریښتیا)) {
		yosys_share_dirname = proc_share_لار;
		أرجع;
	}
	proc_share_لار = YOSYS_DATDIR "/";
	إذا (check_directory_exists(proc_share_لار, ریښتیا)) {
		yosys_share_dirname = proc_share_لار;
		أرجع;
	}
#  endif
}
#د_که_پای

فراغ init_abc_executable_name()
{
#که_تعریف_شوی ABCEXTERNAL
	std::string exe_file;
	إذا (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} وإلا {
		yosys_abc_executable = ABCEXTERNAL;
	}
#وإلا
	yosys_abc_executable = proc_self_dirname() + proc_پروګرام_مختاړی()+ "yosys-abc";
#  ifdef _WIN32
	إذا (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_پروګرام_مختاړی() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_پروګرام_مختاړی() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_لار_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت حرف *abc_لار = PyUnicode_AsUTF8(abc_لار_obj);
		yosys_abc_executable = std::string(abc_لار);
	}
#  endif
#د_که_پای
}

std::string proc_share_dirname()
{
	إذا (yosys_share_dirname.تشه())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	أرجع yosys_share_dirname;
}

std::string proc_پروګرام_مختاړی()
{
	أرجع YOSYS_پروګرام_مختاړی;
}

منطقي fgetline(FILE *f, std::string &بفر)
{
	بفر = "";
	حرف بلاک[4096];
	طالما (1) {
		إذا (fgets(بلاک, 4096, f) == عدم)
			أرجع خطأ;
		بفر += بلاک;
		إذا (بفر.اندازه() > 0 && (بفر[بفر.اندازه()-1] == '\n' ||  بفر[بفر.اندازه()-1] == '\r')) {
			طالما (بفر.اندازه() > 0 && (بفر[بفر.اندازه()-1] == '\n' ||  بفر[بفر.اندازه()-1] == '\r'))
				بفر.resize(بفر.اندازه()-1);
			أرجع ریښتیا;
		}
	}
}

ساكن فراغ handle_label(std::string &command, منطقي &from_منزل_فعال, ثابت std::string &پرلپسې_from, ثابت std::string &پرلپسې_منزل)
{
	صحيح pos = 0;
	std::string label;

	طالما (pos < Get_اندازه(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	إذا (pos < Get_اندازه(command) && command[pos] == '#')
		أرجع;

	طالما (pos < Get_اندازه(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	إذا (Get_اندازه(label) > 1 && label.شا() == ':')
	{
		label = label.substr(0, Get_اندازه(label)-1);
		command = command.substr(pos);

		إذا (label == پرلپسې_from)
			from_منزل_فعال = ریښتیا;
		وإلا إذا (label == پرلپسې_منزل || (پرلپسې_from == پرلپسې_منزل && !پرلپسې_from.تشه()))
			from_منزل_فعال = خطأ;
	}
}

منطقي پرلپسې_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_منزل_label)
{
	إذا (design == تش_اشاره)
		design = yosys_design;

	إذا (command == "auto") {
	  std::string filename_trim = filename;

	  خپلکاری has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
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
		std::string پرلپسې_from, پرلپسې_منزل;
		منطقي from_منزل_فعال = ریښتیا;

		إذا (from_منزل_label != عدم) {
			اندازه_t pos = from_منزل_label->پیدا_کړه(':');
			إذا (pos == std::string::npos) {
				پرلپسې_from = *from_منزل_label;
				پرلپسې_منزل = *from_منزل_label;
			} وإلا {
				پرلپسې_from = from_منزل_label->substr(0, pos);
				پرلپسې_منزل = from_منزل_label->substr(pos+1);
			}
			from_منزل_فعال = پرلپسې_from.تشه();
		}

		لوګاریتم("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		إذا (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_ننوت_files.ورننباسه(filename);
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
				طالما (!command.تشه() && command[command.اندازه()-1] == '\\') {
					std::string بل_کرښه;
					إذا (!fgetline(f, بل_کرښه))
						اكسر;
					Frontend::current_script_lineno++;
					command.resize(command.اندازه()-1);
					command += بل_کرښه;
				}
				handle_label(command, from_منزل_فعال, پرلپسې_from, پرلپسې_منزل);
				إذا (from_منزل_فعال) {
					Pass::call(design, command);
					design->check();
				}
			}

			إذا (!command.تشه()) {
				handle_label(command, from_منزل_فعال, پرلپسې_from, پرلپسې_منزل);
				إذا (from_منزل_فعال) {
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

		أرجع ریښتیا;
	}

	إذا (command == "tcl") {
		Pass::call(design, ویکټور<string>({command, filename}));
		أرجع ریښتیا;
	}

	لوګاریتم("\n");
	إذا (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} وإلا {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	إذا (command[0] == ' ') {
		خپلکاری argv = وېشل_نښې("read" + command);
		argv.ټېل_شا(filename);
		Pass::call(design, argv);
	} وإلا
		Frontend::frontend_call(design, عدم, filename, command);

	design->check();
	أرجع خطأ;
}

فراغ پرلپسې_pass(std::string command, RTLIL::Design *design)
{
	إذا (design == تش_اشاره)
		design = yosys_design;

	لوګاریتم("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

فراغ پرلپسې_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	إذا (design == تش_اشاره)
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
		وإلا إذا (filename.تشه())
			أرجع;
		وإلا
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	إذا (filename.تشه())
		filename = "-";

	لوګاریتم("\n");
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
		it = pass_ثبتوونکی.begin();
		len = مزي_اوږدوالی(متن);
	}

	لكل (; it != pass_ثبتوونکی.پای_ټکی(); it++) {
		إذا (it->لومړی.compare(0, len, متن) == 0)
			أرجع strdup((it++)->لومړی.c_str());
	}
	أرجع عدم;
}

ساكن حرف *readline_obj_generator(ثابت حرف *متن, صحيح state)
{
	ساكن std::ویکټور<حرف*> obj_نومونه;
	ساكن اندازه_t idx;

	إذا (!state)
	{
		idx = 0;
		obj_نومونه.clear();

		RTLIL::Design *design = yosys_get_design();
		صحيح len = مزي_اوږدوالی(متن);

		إذا (design->selected_فعال_module.تشه())
		{
			لكل (خپلکاری mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				إذا (mod_name.compare(0, len, متن) == 0)
					obj_نومونه.ټېل_شا(strdup(mod_name.c_str()));
			}
		}
		وإلا إذا (design->module(design->selected_فعال_module) != تش_اشاره)
		{
			RTLIL::Module *module = design->module(design->selected_فعال_module);

			لكل (خپلکاری w : module->wires())
				إذا (w->name.unescape().compare(0, len, متن) == 0)
					obj_نومونه.ټېل_شا(strdup(w->name.unescape().c_str()));

			لكل (خپلکاری &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.لومړی);
				إذا (mem_name.compare(0, len, متن) == 0)
					obj_نومونه.ټېل_شا(strdup(mem_name.c_str()));
			}

			لكل (خپلکاری cell : module->cells())
				إذا (cell->name.unescape().compare(0, len, متن) == 0)
					obj_نومونه.ټېل_شا(strdup(cell->name.unescape().c_str()));

			لكل (خپلکاری &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.لومړی);
				إذا (proc_name.compare(0, len, متن) == 0)
					obj_نومونه.ټېل_شا(strdup(proc_name.c_str()));
			}
		}

		std::ترتیبول(obj_نومونه.begin(), obj_نومونه.پای_ټکی());
	}

	إذا (idx < obj_نومونه.اندازه())
		أرجع strdup(obj_نومونه[idx++]);

	idx = 0;
	obj_نومونه.clear();
	أرجع عدم;
}

ساكن حرف **readline_completion(ثابت حرف *متن, صحيح پیل, صحيح)
{
	إذا (پیل == 0)
		أرجع rl_completion_matches(متن, readline_cmd_generator);
	إذا (مزي_پرتله(rl_کرښه_بفر, "read_", 5) && مزي_پرتله(rl_کرښه_بفر, "write_", 6))
		أرجع rl_completion_matches(متن, readline_obj_generator);
	أرجع عدم;
}
#د_که_پای

فراغ shell(RTLIL::Design *design)
{
	ساكن صحيح recursion_counter = 0;

	recursion_counter++;
	خپلکاری guard = logger().error_throw_scope();

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (حرف*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_کلمه_break_characters = (حرف*)" \t\n";
#د_که_پای

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
#د_که_پای
		إذا (command[strspn(command, " \t\r\n")] == 0) {
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			حرر(command);
#د_که_پای
			استمر;
		}
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		جمع_کړه_history(command);
#د_که_پای

		حرف *p = command + strspn(command, " \t\r\n");
		إذا (!مزي_پرتله(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			إذا (*p == 0)
				اكسر;
		}

		حاول {
			log_assert(design->selection_ډېری.اندازه() == 1);
			Pass::call(design, command);
		} امسك (log_cmd_error_exception) {
			طالما (design->selection_ډېری.اندازه() > 1)
				design->راوباسه_selection();
			log_reset_ډېری();
		}
		design->check();
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		إذا (command)
			حرر(command);
#د_که_پای
	}
	إذا (command == عدم)
		چاپ_کړه("exit\n");
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وإلا
		حرر(command);
#د_که_پای
	recursion_counter--;
}

بنية ShellPass : عام Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	فراغ help() override {
		لوګاریتم("\n");
		لوګاریتم("    shell\n");
		لوګاریتم("\n");
		لوګاریتم("This command enters the interactive command mode. This can be useful\n");
		لوګاریتم("in a script to interrupt the script at a certain point and allow for\n");
		لوګاریتم("interactive inspection or manual synthesis of the design at this point.\n");
		لوګاریتم("\n");
		لوګاریتم("The command prompt of the interactive shell indicates the current\n");
		لوګاریتم("selection (see 'help select'):\n");
		لوګاریتم("\n");
		لوګاریتم("    yosys>\n");
		لوګاریتم("        the entire design is selected\n");
		لوګاریتم("\n");
		لوګاریتم("    yosys*>\n");
		لوګاریتم("        only part of the design is selected\n");
		لوګاریتم("\n");
		لوګاریتم("    yosys [modname]>\n");
		لوګاریتم("        the entire module 'modname' is selected using 'select -module modname'\n");
		لوګاریتم("\n");
		لوګاریتم("    yosys [modname]*>\n");
		لوګاریتم("        only part of current module 'modname' is selected\n");
		لوګاریتم("\n");
		لوګاریتم("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		لوګاریتم("do not terminate yosys but return to the command prompt.\n");
		لوګاریتم("\n");
		لوګاریتم("This command is the default action if nothing else has been specified\n");
		لوګاریتم("on the command line.\n");
		لوګاریتم("\n");
		لوګاریتم("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		لوګاریتم("\n");
	}
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) override {
		extra_دلیلونه(دلیلونه_2, 1, design, خطأ);
		shell(design);
	}
} ShellPass;

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
بنية HistoryPass : عام Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	فراغ help() override {
		لوګاریتم("\n");
		لوګاریتم("    history\n");
		لوګاریتم("\n");
		لوګاریتم("This command prints all commands in the shell history buffer. This are\n");
		لوګاریتم("all commands executed in an interactive session, but not the commands\n");
		لوګاریتم("from executed scripts.\n");
		لوګاریتم("\n");
	}
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) override {
		extra_دلیلونه(دلیلونه_2, 1, design, خطأ);
#که_تعریف_شوی YOSYS_ENABLE_READLINE
		لكل(HIST_ثبت **list = history_list(); *list != عدم; list++)
			لوګاریتم("%s\n", (*list)->کرښه);
#وإلا
		لكل (صحيح i = where_history(); history_get(i); i++)
			لوګاریتم("%s\n", history_get(i)->کرښه);
#د_که_پای
	}
} HistoryPass;
#د_که_پای

بنية ScriptCmdPass : عام Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	فراغ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لوګاریتم("\n");
		لوګاریتم("    script <filename> [<from_label>:<to_label>]\n");
		لوګاریتم("    script -scriptwire [selection]\n");
		لوګاریتم("\n");
		لوګاریتم("This command executes the yosys commands in the specified file (default\n");
		لوګاریتم("behaviour), or commands embedded in the constant text value connected to the\n");
		لوګاریتم("selected wires.\n");
		لوګاریتم("\n");
		لوګاریتم("In the default (file) case, the 2nd argument can be used to only execute the\n");
		لوګاریتم("section of the file between the specified labels. An empty from label is\n");
		لوګاریتم("synonymous with the beginning of the file and an empty to label is synonymous\n");
		لوګاریتم("with the end of the file.\n");
		لوګاریتم("\n");
		لوګاریتم("If only one label is specified (without ':') then only the block\n");
		لوګاریتم("marked with that label (until the next label) is executed.\n");
		لوګاریتم("\n");
		لوګاریتم("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		لوګاریتم("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		لوګاریتم("'-module' mode can be exited by using the 'cd' command.\n");
		لوګاریتم("\n");
	}
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) override
	{
		منطقي scriptwire = خطأ;

		اندازه_t argidx;
		لكل (argidx = 1; argidx < دلیلونه_2.اندازه(); argidx++) {
			إذا (دلیلونه_2[argidx] == "-scriptwire") {
				scriptwire = ریښتیا;
				استمر;
			}
			اكسر;
		}
		إذا (scriptwire) {
			extra_دلیلونه(دلیلونه_2, argidx, design);

			لكل (خپلکاری mod : design->selected_modules())
				لكل (خپلکاری &c : mod->connections()) {
					إذا (!c.لومړی.is_wire())
						استمر;
					خپلکاری w = c.لومړی.as_wire();
					إذا (!mod->selected(w))
						استمر;
					إذا (!c.دوهم.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					خپلکاری v = c.دوهم.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		وإلا إذا (دلیلونه_2.اندازه() < 2)
			log_cmd_error("Missing script file.\n");
		وإلا إذا (دلیلونه_2.اندازه() == 2)
			پرلپسې_frontend(دلیلونه_2[1], "script", design);
		وإلا إذا (دلیلونه_2.اندازه() == 3)
			پرلپسې_frontend(دلیلونه_2[1], "script", design, &دلیلونه_2[2]);
		وإلا
			extra_دلیلونه(دلیلونه_2, 2, design, خطأ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_پای_ټکی
