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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/log.h"
#অন্তর্ভাব "kernel/newcelltypes.h"

#ifdef YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

#ifdef YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#endif

#ifdef YOSYS_ENABLE_TCL
#  include <tcl.h>
#endif

#ifdef YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
নামাকাশ py = pybind11;
#endif

#যদি defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#নতুবা_যদি defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#অন্যথা
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  যদি defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#যদি !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#যদি defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
বাহ্যিক "C" PyObject* PyInit_libyosys();
বাহ্যিক "C" PyObject* PyInit_pyosys();
#অন্তর্ভাব <signal.h>
#endif

#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
পূর্ণসংখ্যা yosys_xtrace = 0;
যৌক্তিক yosys_write_versions = সত্য;
ধ্রুবক অক্ষর* yosys_maybe_version() {
	যদি (yosys_write_versions)
		ফেরত yosys_version_str;
	অন্যথা
		ফেরত "Yosys";
}

RTLIL::Design *yosys_design = ফাঁকা;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ফাঁকা;
Tcl_Interp *yosys_sdc_interp = ফাঁকা;
#endif

std::set<std::string> yosys_প্রবেশ_files, yosys_output_files;

যৌক্তিক memhasher_active = মিথ্যা;
uint32_t memhasher_rng = 123456;
std::vector<শূন্য*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

যৌক্তিক Multithreading::active_ = মিথ্যা;

শূন্য init_share_dirname();
শূন্য init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = সত্য;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = মিথ্যা;
}

শূন্য Autoidx::ensure_at_least(পূর্ণসংখ্যা v) {
	log_assert(!Multithreading::active());
	মান = std::max(মান, v);
}

পূর্ণসংখ্যা Autoidx::operator++(পূর্ণসংখ্যা) {
	log_assert(!Multithreading::active());
	ফেরত মান++;
}

শূন্য memhasher_on()
{
#যদি defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(ফাঁকা) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = সত্য;
}

শূন্য memhasher_off()
{
	জন্য (auto p : memhasher_store)
		যদি (p) মুক্ত(p);
	memhasher_store.clear();
	memhasher_active = মিথ্যা;
}

শূন্য memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	পূর্ণসংখ্যা আকার_2, সূচক = (memhasher_rng >> 4) & 0xffff;
	নির্বাচন (memhasher_rng & 7) {
		ক্ষেত্র 0: আকার_2 =   16; বিরতি;
		ক্ষেত্র 1: আকার_2 =  256; বিরতি;
		ক্ষেত্র 2: আকার_2 = 1024; বিরতি;
		ক্ষেত্র 3: আকার_2 = 4096; বিরতি;
		পূর্বনির্ধারিত: আকার_2 = 0;
	}
	যদি (সূচক < 16) আকার_2 *= 16;
	memhasher_store[সূচক] = realloc(memhasher_store[সূচক], আকার_2);
}

শূন্য yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#যদি defined(YOSYS_ENABLE_SPAWN)
পূর্ণসংখ্যা run_command(ধ্রুবক std::string &command, std::function<শূন্য(ধ্রুবক std::string&)> process_সারি)
{
	যদি (!process_সারি)
		ফেরত system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	যদি (f == nullptr)
		ফেরত -1;

	std::string সারি;
	অক্ষর logbuf[128];
	যখন (সত্য) {
		যদি (fgets(logbuf, 128, f) != ফাঁকা) {
			সারি += logbuf;
			যদি (!সারি.empty() && সারি.back() == '\n')
				process_সারি(সারি), সারি.clear();
		} অন্যথা যদি (ferror(f) && errno == EINTR) {
			clearerr(f);
			চালু;
		} অন্যথা {
			বিরতি;
		}
	}
	যদি (!সারি.empty())
		process_সারি(সারি);

	পূর্ণসংখ্যা ret = pclose(f);
	যদি (ret < 0)
		ফেরত -2;
#ifdef _WIN32
	ফেরত ret;
#অন্যথা
	যদি (!WIFEXITED(ret))
		ফেরত -1;
	ফেরত WEXITSTATUS(ret);
#endif
}
#endif

যৌক্তিক already_setup = মিথ্যা;
যৌক্তিক already_shutdown = মিথ্যা;

#ifdef YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.add_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(শূন্য)_;
	ছোড়ো py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

শূন্য yosys_setup()
{
	যদি(already_setup)
		ফেরত;
	already_setup = সত্য;
	already_shutdown = মিথ্যা;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	যদি (!Py_IsInitialized()) {
		PyImport_AppendInittab((অক্ষর*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((অক্ষর*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((অক্ষর*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		সংকেত(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = নতুন RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_ঠেলো();
}

যৌক্তিক yosys_already_setup()
{
	ফেরত already_setup;
}

শূন্য yosys_shutdown()
{
	যদি(already_shutdown)
		ফেরত;
	already_setup = মিথ্যা;
	already_shutdown = সত্য;
	log_বের_করো();

	Pass::done_register();

	delete yosys_design;
	yosys_design = ফাঁকা;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	যদি (yosys_tcl_interp != ফাঁকা) {
		যদি (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ফাঁকা;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	জন্য (auto &it : loaded_plugins)
		dlclose(it.second);

	loaded_plugins.clear();
#ifdef YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#endif
	loaded_plugin_aliases.clear();
#endif

#ifdef YOSYS_ENABLE_PYTHON
	Py_Finalize();
#endif
}

ধ্রুবক std::string *create_id_prefix(std::string_view file, পূর্ণসংখ্যা সারি, std::string_view func)
{
#ifdef _WIN32
	আকার_t pos = file.find_last_of("/\\");
#অন্যথা
	আকার_t pos = file.find_last_of('/');
#endif
	যদি (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	যদি (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ফেরত নতুন std::string(stringf("$auto$%s:%d:%s$", file, সারি, func));
}

RTLIL::Design *yosys_get_design()
{
	ফেরত yosys_design;
}

ধ্রুবক অক্ষর *create_prompt(RTLIL::Design *design, পূর্ণসংখ্যা recursion_গণক, যৌক্তিক newline)
{
	স্থির অক্ষর বাফার[100];
	std::string str = newline ? "\n" : "";
	যদি (recursion_গণক > 1)
		str += stringf("(%d) ", recursion_গণক);
	str += "yosys";
	যদি (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	যদি (!design->full_selection()) {
		যদি (design->selected_active_module.empty())
			str += "*";
		অন্যথা যদি (design->selection().selected_modules.আকার_2() != 1 || design->selection().selected_members.আকার_2() != 0 ||
				design->selection().selected_modules.গুনতি(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(বাফার, 100, "%s> ", str.c_str());
	ফেরত বাফার;
}

std::vector<std::string> glob_filename(ধ্রুবক std::string &filename_pattern)
{
	std::vector<std::string> results;

#যদি defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ঠেলো_back(filename_pattern);
#অন্যথা
	glob_t globbuf;

	পূর্ণসংখ্যা err = glob(filename_pattern.c_str(), 0, ফাঁকা, &globbuf);

	যদি(err == 0) {
		জন্য (আকার_t i = 0; i < globbuf.gl_pathc; i++)
			results.ঠেলো_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} অন্যথা {
		results.ঠেলো_back(filename_pattern);
	}
#endif

	ফেরত results;
}

শূন্য rewrite_filename(std::string &filename)
{
	যদি (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_আকার(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_আকার(filename)-2);
	যদি (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	যদি (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
বাহ্যিক পূর্ণসংখ্যা yosys_tcl_interp_init(Tcl_Interp *interp);

বাহ্যিক Tcl_Interp *yosys_get_tcl_interp()
{
	যদি (yosys_tcl_interp == ফাঁকা) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ফেরত yosys_tcl_interp;
}

// Also see SdcPass
কাঠামো TclPass : সর্বজনীন Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	শূন্য help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		log("\n");
		log("    tcl <filename> [args]\n");
		log("\n");
		log("This command executes the tcl commands in the specified file.\n");
		log("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		log("\n");
		log("The tcl command 'yosys -import' can be used to import all yosys\n");
		log("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		log("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		log("in order to avoid a name collision with the built in commands.\n");
		log("\n");
		log("If any arguments are specified, these arguments are provided to the script via\n");
		log("the standard $argc and $argv variables.\n");
		log("\n");
		log("Note, tcl will not receive the output of any yosys command. If the output\n");
		log("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		log("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		log("The 'result.string' value is then used as the tcl output value of the command.\n");
		log("\n");
	}
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *) override {
		যদি (args.আকার_2() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		জন্য (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.ঠেলো_back(Tcl_NewStringObj((*it).c_str(), (*it).আকার_2()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ফাঁকা, Tcl_NewIntObj(script_args.আকার_2()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ফাঁকা, Tcl_NewListObj(script_args.আকার_2(), script_args.তথ্য()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ফাঁকা, Tcl_NewStringObj(args[1].c_str(), args[1].আকার_2()), 0);
		যদি (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ফলাফল(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#যদি defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	জন্য (পূর্ণসংখ্যা tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.তথ্য(), path.আকার_2());
		যদি (buflen < (ssize_t)path.আকার_2())
			বিরতি;
		অন্যথা
			path.resize(path.আকার_2() * 2);
	}
	যদি (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} অন্যথা {
		যখন (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	ফেরত path;
}
#নতুবা_যদি defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	পূর্ণসংখ্যা mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#অন্যথা
	পূর্ণসংখ্যা mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	আকার_t buflen;
	অক্ষর *বাফার;
	std::string path;
	যদি (sysctl(mib, 4, ফাঁকা, &buflen, ফাঁকা, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	বাফার = (অক্ষর*)malloc(buflen);
	যদি (বাফার == ফাঁকা)
		log_error("malloc failed: %s\n", strerror(errno));
	যদি (sysctl(mib, 4, বাফার, &buflen, ফাঁকা, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	যখন (buflen > 0 && বাফার[buflen-1] != '/')
		buflen--;
	path.assign(বাফার, buflen);
	মুক্ত(বাফার);
	ফেরত path;
}
#নতুবা_যদি defined(__APPLE__)
std::string proc_self_dirname()
{
	অক্ষর *path = ফাঁকা;
	uint32_t buflen = 0;
	যখন (_NSGetExecutablePath(path, &buflen) != 0)
		path = (অক্ষর *) realloc((শূন্য *) path, buflen);
	অক্ষর resolved[PATH_MAX];
	যদি (realpath(path, resolved) != ফাঁকা) {
		মুক্ত(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	যখন (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	মুক্ত(path);
	ফেরত str;
}
#নতুবা_যদি defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	যদি (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.আকার_2()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.তথ্য(), wbinpath.আকার_2(), ফাঁকা, 0, ফাঁকা, ফাঁকা));
	যদি (WideCharToMultiByte(CP_UTF8, 0, wbinpath.তথ্য(), wbinpath.আকার_2(), &ubinpath[0], ubinpath.আকার_2(), ফাঁকা, ফাঁকা) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ফেরত ubinpath;

}
#নতুবা_যদি defined(__wasm)
std::string proc_self_dirname()
{
	ফেরত "/";
}
#নতুবা_যদি defined(__OpenBSD__) || defined(__HAIKU__)
অক্ষর yosys_path[PATH_MAX];
অক্ষর *yosys_argv0;

std::string proc_self_dirname(শূন্য)
{
	অক্ষর buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	যদি (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, আকার buf, "%*s/", (পূর্ণসংখ্যা)(yosys_path - p), yosys_path);
		ফেরত buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	যদি (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	জন্য (p = strtok(path, ":"); p; p = strtok(ফাঁকা, ":")) {
		snprintf(buf, আকার buf, "%s/%s", p, yosys_argv0);
		যদি (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			মুক্ত(path);
			ফেরত buf;
		}
	}
	মুক্ত(path);
	log_error("Can't determine yosys executable path.\n");
	ফেরত ফাঁকা;
}
#অন্যথা
	#error "Don't know how to determine process executable base path!"
#endif

#যদি defined(__wasm)
শূন্য init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#অন্যথা
শূন্য init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	যদি (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ধ্রুবক অক্ষর *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		ফেরত;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  যদি defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	যদি (check_directory_exists(proc_share_path, সত্য)) {
		yosys_share_dirname = proc_share_path;
		ফেরত;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	যদি (check_directory_exists(proc_share_path, সত্য)) {
		yosys_share_dirname = proc_share_path;
		ফেরত;
	}
#  অন্যথা
	std::string proc_share_path = proc_self_path + "share/";
	যদি (check_directory_exists(proc_share_path, সত্য)) {
		yosys_share_dirname = proc_share_path;
		ফেরত;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	যদি (check_directory_exists(proc_share_path, সত্য)) {
		yosys_share_dirname = proc_share_path;
		ফেরত;
	}
	proc_share_path = YOSYS_DATDIR "/";
	যদি (check_directory_exists(proc_share_path, সত্য)) {
		yosys_share_dirname = proc_share_path;
		ফেরত;
	}
#  endif
}
#endif

শূন্য init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	যদি (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} অন্যথা {
		yosys_abc_executable = ABCEXTERNAL;
	}
#অন্যথা
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	যদি (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	যদি (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ধ্রুবক অক্ষর *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	যদি (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ফেরত yosys_share_dirname;
}

std::string proc_program_prefix()
{
	ফেরত YOSYS_PROGRAM_PREFIX;
}

যৌক্তিক fgetline(FILE *f, std::string &বাফার)
{
	বাফার = "";
	অক্ষর block[4096];
	যখন (1) {
		যদি (fgets(block, 4096, f) == ফাঁকা)
			ফেরত মিথ্যা;
		বাফার += block;
		যদি (বাফার.আকার_2() > 0 && (বাফার[বাফার.আকার_2()-1] == '\n' ||  বাফার[বাফার.আকার_2()-1] == '\r')) {
			যখন (বাফার.আকার_2() > 0 && (বাফার[বাফার.আকার_2()-1] == '\n' ||  বাফার[বাফার.আকার_2()-1] == '\r'))
				বাফার.resize(বাফার.আকার_2()-1);
			ফেরত সত্য;
		}
	}
}

স্থির শূন্য handle_label(std::string &command, যৌক্তিক &from_to_active, ধ্রুবক std::string &run_from, ধ্রুবক std::string &run_to)
{
	পূর্ণসংখ্যা pos = 0;
	std::string label;

	যখন (pos < Get_আকার(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	যদি (pos < Get_আকার(command) && command[pos] == '#')
		ফেরত;

	যখন (pos < Get_আকার(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	যদি (Get_আকার(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, Get_আকার(label)-1);
		command = command.substr(pos);

		যদি (label == run_from)
			from_to_active = সত্য;
		অন্যথা যদি (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = মিথ্যা;
	}
}

যৌক্তিক run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	যদি (design == nullptr)
		design = yosys_design;

	যদি (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ধ্রুবক std::string& filename, ধ্রুবক std::string& extension) {
	    যদি (filename.আকার_2() >= extension.আকার_2()) {
	      ফেরত filename.compare(filename.আকার_2() - extension.আকার_2(), extension.আকার_2(), extension) == 0;
	    }
	    ফেরত মিথ্যা;
	  };

	  যদি (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.আকার_2() - 3);
	  }

	  যদি (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } অন্যথা যদি (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } অন্যথা যদি (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } অন্যথা যদি (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } অন্যথা যদি (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } অন্যথা যদি (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } অন্যথা যদি (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } অন্যথা যদি (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } অন্যথা যদি (filename == "-") {
	    command = "script";
	  } অন্যথা {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	যদি (command == "script")
	{
		std::string run_from, run_to;
		যৌক্তিক from_to_active = সত্য;

		যদি (from_to_label != ফাঁকা) {
			আকার_t pos = from_to_label->find(':');
			যদি (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} অন্যথা {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		যদি (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_প্রবেশ_files.ঢোকাও(filename);
		}

		যদি (f == ফাঁকা)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		পূর্ণসংখ্যা backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		চেষ্টা {
			std::string command;
			যখন (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				যখন (!command.empty() && command[command.আকার_2()-1] == '\\') {
					std::string পরবর্তী_সারি;
					যদি (!fgetline(f, পরবর্তী_সারি))
						বিরতি;
					Frontend::current_script_lineno++;
					command.resize(command.আকার_2()-1);
					command += পরবর্তী_সারি;
				}
				handle_label(command, from_to_active, run_from, run_to);
				যদি (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			যদি (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				যদি (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		ধরো (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ছোড়ো;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		যদি (filename != "-")
			fclose(f);

		ফেরত সত্য;
	}

	যদি (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		ফেরত সত্য;
	}

	log("\n");
	যদি (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} অন্যথা {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	যদি (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.ঠেলো_back(filename);
		Pass::call(design, argv);
	} অন্যথা
		Frontend::frontend_call(design, ফাঁকা, filename, command);

	design->check();
	ফেরত মিথ্যা;
}

শূন্য run_pass(std::string command, RTLIL::Design *design)
{
	যদি (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

শূন্য run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	যদি (design == nullptr)
		design = yosys_design;

	যদি (command == "auto") {
		যদি (filename.আকার_2() > 2 && filename.compare(filename.আকার_2()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		অন্যথা যদি (filename.আকার_2() > 3 && filename.compare(filename.আকার_2()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		অন্যথা যদি (filename.আকার_2() > 3 && filename.compare(filename.আকার_2()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		অন্যথা যদি (filename.আকার_2() > 3 && filename.compare(filename.আকার_2()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		অন্যথা যদি (filename.আকার_2() > 4 && filename.compare(filename.আকার_2()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		অন্যথা যদি (filename.আকার_2() > 5 && filename.compare(filename.আকার_2()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		অন্যথা যদি (filename.আকার_2() > 5 && filename.compare(filename.আকার_2()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		অন্যথা যদি (filename.আকার_2() > 5 && filename.compare(filename.আকার_2()-5, std::string::npos, ".json") == 0)
			command = "json";
		অন্যথা যদি (filename == "-")
			command = "rtlil";
		অন্যথা যদি (filename.empty())
			ফেরত;
		অন্যথা
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	যদি (filename.empty())
		filename = "-";

	log("\n");
	যদি (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} অন্যথা {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ফাঁকা, filename, command);
}

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
স্থির অক্ষর *readline_cmd_generator(ধ্রুবক অক্ষর *text, পূর্ণসংখ্যা state)
{
	স্থির std::map<std::string, Pass*>::iterator it;
	স্থির পূর্ণসংখ্যা len;

	যদি (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	জন্য (; it != pass_register.end(); it++) {
		যদি (it->first.compare(0, len, text) == 0)
			ফেরত strdup((it++)->first.c_str());
	}
	ফেরত ফাঁকা;
}

স্থির অক্ষর *readline_obj_generator(ধ্রুবক অক্ষর *text, পূর্ণসংখ্যা state)
{
	স্থির std::vector<অক্ষর*> obj_names;
	স্থির আকার_t idx;

	যদি (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		পূর্ণসংখ্যা len = strlen(text);

		যদি (design->selected_active_module.empty())
		{
			জন্য (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				যদি (mod_name.compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(mod_name.c_str()));
			}
		}
		অন্যথা যদি (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			জন্য (auto w : module->wires())
				যদি (w->name.unescape().compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(w->name.unescape().c_str()));

			জন্য (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				যদি (mem_name.compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(mem_name.c_str()));
			}

			জন্য (auto cell : module->cells())
				যদি (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(cell->name.unescape().c_str()));

			জন্য (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				যদি (proc_name.compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	যদি (idx < obj_names.আকার_2())
		ফেরত strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	ফেরত ফাঁকা;
}

স্থির অক্ষর **readline_completion(ধ্রুবক অক্ষর *text, পূর্ণসংখ্যা start, পূর্ণসংখ্যা)
{
	যদি (start == 0)
		ফেরত rl_completion_matches(text, readline_cmd_generator);
	যদি (strncmp(rl_সারি_বাফার, "read_", 5) && strncmp(rl_সারি_বাফার, "write_", 6))
		ফেরত rl_completion_matches(text, readline_obj_generator);
	ফেরত ফাঁকা;
}
#endif

শূন্য shell(RTLIL::Design *design)
{
	স্থির পূর্ণসংখ্যা recursion_গণক = 0;

	recursion_গণক++;
	auto guard = logger().error_throw_scope();

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (অক্ষর*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (অক্ষর*)" \t\n";
#endif

	অক্ষর *command = ফাঁকা;
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	যখন ((command = readline(create_prompt(design, recursion_গণক))) != ফাঁকা)
	{
#অন্যথা
	অক্ষর command_বাফার[4096];
	যখন (1)
	{
		fputs(create_prompt(design, recursion_গণক), stdout);
		fflush(stdout);
		যদি ((command = fgets(command_বাফার, 4096, stdin)) == ফাঁকা)
			বিরতি;
#endif
		যদি (command[strspn(command, " \t\r\n")] == 0) {
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			মুক্ত(command);
#endif
			চালু;
		}
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		অক্ষর *p = command + strspn(command, " \t\r\n");
		যদি (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			যদি (*p == 0)
				বিরতি;
		}

		চেষ্টা {
			log_assert(design->selection_স্তূপ.আকার_2() == 1);
			Pass::call(design, command);
		} ধরো (log_cmd_error_exception) {
			যখন (design->selection_স্তূপ.আকার_2() > 1)
				design->বের_করো_selection();
			log_reset_স্তূপ();
		}
		design->check();
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		যদি (command)
			মুক্ত(command);
#endif
	}
	যদি (command == ফাঁকা)
		printf("exit\n");
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	অন্যথা
		মুক্ত(command);
#endif
	recursion_গণক--;
}

কাঠামো ShellPass : সর্বজনীন Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	শূন্য help() override {
		log("\n");
		log("    shell\n");
		log("\n");
		log("This command enters the interactive command mode. This can be useful\n");
		log("in a script to interrupt the script at a certain point and allow for\n");
		log("interactive inspection or manual synthesis of the design at this point.\n");
		log("\n");
		log("The command prompt of the interactive shell indicates the current\n");
		log("selection (see 'help select'):\n");
		log("\n");
		log("    yosys>\n");
		log("        the entire design is selected\n");
		log("\n");
		log("    yosys*>\n");
		log("        only part of the design is selected\n");
		log("\n");
		log("    yosys [modname]>\n");
		log("        the entire module 'modname' is selected using 'select -module modname'\n");
		log("\n");
		log("    yosys [modname]*>\n");
		log("        only part of current module 'modname' is selected\n");
		log("\n");
		log("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		log("do not terminate yosys but return to the command prompt.\n");
		log("\n");
		log("This command is the default action if nothing else has been specified\n");
		log("on the command line.\n");
		log("\n");
		log("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		log("\n");
	}
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, মিথ্যা);
		shell(design);
	}
} ShellPass;

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
কাঠামো HistoryPass : সর্বজনীন Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	শূন্য help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, মিথ্যা);
#ifdef YOSYS_ENABLE_READLINE
		জন্য(HIST_ENTRY **list = history_list(); *list != ফাঁকা; list++)
			log("%s\n", (*list)->সারি);
#অন্যথা
		জন্য (পূর্ণসংখ্যা i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->সারি);
#endif
	}
} HistoryPass;
#endif

কাঠামো ScriptCmdPass : সর্বজনীন Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	শূন্য help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		log("\n");
		log("    script <filename> [<from_label>:<to_label>]\n");
		log("    script -scriptwire [selection]\n");
		log("\n");
		log("This command executes the yosys commands in the specified file (default\n");
		log("behaviour), or commands embedded in the constant text value connected to the\n");
		log("selected wires.\n");
		log("\n");
		log("In the default (file) case, the 2nd argument can be used to only execute the\n");
		log("section of the file between the specified labels. An empty from label is\n");
		log("synonymous with the beginning of the file and an empty to label is synonymous\n");
		log("with the end of the file.\n");
		log("\n");
		log("If only one label is specified (without ':') then only the block\n");
		log("marked with that label (until the next label) is executed.\n");
		log("\n");
		log("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		log("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		log("'-module' mode can be exited by using the 'cd' command.\n");
		log("\n");
	}
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		যৌক্তিক scriptwire = মিথ্যা;

		আকার_t argidx;
		জন্য (argidx = 1; argidx < args.আকার_2(); argidx++) {
			যদি (args[argidx] == "-scriptwire") {
				scriptwire = সত্য;
				চালু;
			}
			বিরতি;
		}
		যদি (scriptwire) {
			extra_args(args, argidx, design);

			জন্য (auto mod : design->selected_modules())
				জন্য (auto &c : mod->connections()) {
					যদি (!c.first.is_wire())
						চালু;
					auto w = c.first.as_wire();
					যদি (!mod->selected(w))
						চালু;
					যদি (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		অন্যথা যদি (args.আকার_2() < 2)
			log_cmd_error("Missing script file.\n");
		অন্যথা যদি (args.আকার_2() == 2)
			run_frontend(args[1], "script", design);
		অন্যথা যদি (args.আকার_2() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		অন্যথা
			extra_args(args, 2, design, মিথ্যা);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
