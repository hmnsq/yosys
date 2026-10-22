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

#شامل "kernel/yosys.h"
#شامل "kernel/log.h"
#شامل "kernel/newcelltypes.h"

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
فضاینام py = pybind11;
#endif

#اگر defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#وگرنهاگر defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#وگرنه
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  اگر defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#اگر !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#اگر defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
بیرونی "C" PyObject* PyInit_libyosys();
بیرونی "C" PyObject* PyInit_pyosys();
#شامل <signal.h>
#endif

#شامل <limits.h>
#شامل <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحیح yosys_xtrace = 0;
منطقی yosys_write_versions = درست;
ثابت نویسه* yosys_maybe_version() {
	اگر (yosys_write_versions)
		بازگشت yosys_version_str;
	وگرنه
		بازگشت "Yosys";
}

RTLIL::Design *yosys_design = پوچ;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = پوچ;
Tcl_Interp *yosys_sdc_interp = پوچ;
#endif

std::set<std::string> yosys_درون_داد_files, yosys_output_files;

منطقی memhasher_active = نادرست;
uint32_t memhasher_rng = 123456;
std::vector<تهی*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقی Multithreading::active_ = نادرست;

تهی init_share_dirname();
تهی init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = درست;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = نادرست;
}

تهی Autoidx::ensure_at_least(صحیح v) {
	log_assert(!Multithreading::active());
	مقدار = std::max(مقدار, v);
}

صحیح Autoidx::operator++(صحیح) {
	log_assert(!Multithreading::active());
	بازگشت مقدار++;
}

تهی memhasher_on()
{
#اگر defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(پوچ) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = درست;
}

تهی memhasher_off()
{
	برای (auto p : memhasher_store)
		اگر (p) آزاد(p);
	memhasher_store.clear();
	memhasher_active = نادرست;
}

تهی memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	صحیح size, index = (memhasher_rng >> 4) & 0xffff;
	انتخاب (memhasher_rng & 7) {
		حالت 0: size =   16; بشکن;
		حالت 1: size =  256; بشکن;
		حالت 2: size = 1024; بشکن;
		حالت 3: size = 4096; بشکن;
		پیشفرض: size = 0;
	}
	اگر (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

تهی yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#اگر defined(YOSYS_ENABLE_SPAWN)
صحیح run_command(ثابت std::string &command, std::function<تهی(ثابت std::string&)> process_سطر)
{
	اگر (!process_سطر)
		بازگشت system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	اگر (f == nullptr)
		بازگشت -1;

	std::string سطر;
	نویسه logbuf[128];
	وقتی (درست) {
		اگر (fgets(logbuf, 128, f) != پوچ) {
			سطر += logbuf;
			اگر (!سطر.empty() && سطر.back() == '\n')
				process_سطر(سطر), سطر.clear();
		} وگرنه اگر (ferror(f) && errno == EINTR) {
			clearerr(f);
			ادامه;
		} وگرنه {
			بشکن;
		}
	}
	اگر (!سطر.empty())
		process_سطر(سطر);

	صحیح ret = pclose(f);
	اگر (ret < 0)
		بازگشت -2;
#ifdef _WIN32
	بازگشت ret;
#وگرنه
	اگر (!WIFEXITED(ret))
		بازگشت -1;
	بازگشت WEXITSTATUS(ret);
#endif
}
#endif

منطقی already_setup = نادرست;
منطقی already_shutdown = نادرست;

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
	(تهی)_;
	پرتابکن py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

تهی yosys_setup()
{
	اگر(already_setup)
		بازگشت;
	already_setup = درست;
	already_shutdown = نادرست;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	اگر (!Py_IsInitialized()) {
		PyImport_AppendInittab((نویسه*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((نویسه*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((نویسه*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		سیگنال(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = جدید RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_افزودن();
}

منطقی yosys_already_setup()
{
	بازگشت already_setup;
}

تهی yosys_shutdown()
{
	اگر(already_shutdown)
		بازگشت;
	already_setup = نادرست;
	already_shutdown = درست;
	log_برداشت();

	Pass::done_register();

	delete yosys_design;
	yosys_design = پوچ;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	اگر (yosys_tcl_interp != پوچ) {
		اگر (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = پوچ;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	برای (auto &it : loaded_plugins)
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

ثابت std::string *create_id_prefix(std::string_view file, صحیح سطر, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#وگرنه
	size_t pos = file.find_last_of('/');
#endif
	اگر (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	اگر (pos != std::string_view::npos)
		func = func.substr(pos+1);

	بازگشت جدید std::string(stringf("$auto$%s:%d:%s$", file, سطر, func));
}

RTLIL::Design *yosys_get_design()
{
	بازگشت yosys_design;
}

ثابت نویسه *create_prompt(RTLIL::Design *design, صحیح recursion_counter, منطقی newline)
{
	ایستا نویسه buffer[100];
	std::string str = newline ? "\n" : "";
	اگر (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	اگر (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	اگر (!design->full_selection()) {
		اگر (design->selected_active_module.empty())
			str += "*";
		وگرنه اگر (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.شمار(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	بازگشت buffer;
}

std::vector<std::string> glob_filename(ثابت std::string &filename_pattern)
{
	std::vector<std::string> results;

#اگر defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.افزودن_back(filename_pattern);
#وگرنه
	glob_t globbuf;

	صحیح err = glob(filename_pattern.c_str(), 0, پوچ, &globbuf);

	اگر(err == 0) {
		برای (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.افزودن_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} وگرنه {
		results.افزودن_back(filename_pattern);
	}
#endif

	بازگشت results;
}

تهی rewrite_filename(std::string &filename)
{
	اگر (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	اگر (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	اگر (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
بیرونی صحیح yosys_tcl_interp_init(Tcl_Interp *interp);

بیرونی Tcl_Interp *yosys_get_tcl_interp()
{
	اگر (yosys_tcl_interp == پوچ) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	بازگشت yosys_tcl_interp;
}

// Also see SdcPass
ساختار TclPass : عمومی Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	تهی help() override {
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
	تهی execute(std::vector<std::string> args, RTLIL::Design *) override {
		اگر (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		برای (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.افزودن_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), پوچ, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), پوچ, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), پوچ, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		اگر (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#اگر defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	برای (صحیح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		اگر (buflen < (ssize_t)path.size())
			بشکن;
		وگرنه
			path.resize(path.size() * 2);
	}
	اگر (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} وگرنه {
		وقتی (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	بازگشت path;
}
#وگرنهاگر defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	صحیح mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#وگرنه
	صحیح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	نویسه *buffer;
	std::string path;
	اگر (sysctl(mib, 4, پوچ, &buflen, پوچ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (نویسه*)malloc(buflen);
	اگر (buffer == پوچ)
		log_error("malloc failed: %s\n", strerror(errno));
	اگر (sysctl(mib, 4, buffer, &buflen, پوچ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	وقتی (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	آزاد(buffer);
	بازگشت path;
}
#وگرنهاگر defined(__APPLE__)
std::string proc_self_dirname()
{
	نویسه *path = پوچ;
	uint32_t buflen = 0;
	وقتی (_NSGetExecutablePath(path, &buflen) != 0)
		path = (نویسه *) realloc((تهی *) path, buflen);
	نویسه resolved[PATH_MAX];
	اگر (realpath(path, resolved) != پوچ) {
		آزاد(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	وقتی (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	آزاد(path);
	بازگشت str;
}
#وگرنهاگر defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	اگر (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), پوچ, 0, پوچ, پوچ));
	اگر (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), پوچ, پوچ) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	بازگشت ubinpath;

}
#وگرنهاگر defined(__wasm)
std::string proc_self_dirname()
{
	بازگشت "/";
}
#وگرنهاگر defined(__OpenBSD__) || defined(__HAIKU__)
نویسه yosys_path[PATH_MAX];
نویسه *yosys_argv0;

std::string proc_self_dirname(تهی)
{
	نویسه buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	اگر (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, اندازه buf, "%*s/", (صحیح)(yosys_path - p), yosys_path);
		بازگشت buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	اگر (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	برای (p = strtok(path, ":"); p; p = strtok(پوچ, ":")) {
		snprintf(buf, اندازه buf, "%s/%s", p, yosys_argv0);
		اگر (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			آزاد(path);
			بازگشت buf;
		}
	}
	آزاد(path);
	log_error("Can't determine yosys executable path.\n");
	بازگشت پوچ;
}
#وگرنه
	#error "Don't know how to determine process executable base path!"
#endif

#اگر defined(__wasm)
تهی init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#وگرنه
تهی init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	اگر (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت نویسه *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		بازگشت;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  اگر defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	اگر (check_directory_exists(proc_share_path, درست)) {
		yosys_share_dirname = proc_share_path;
		بازگشت;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	اگر (check_directory_exists(proc_share_path, درست)) {
		yosys_share_dirname = proc_share_path;
		بازگشت;
	}
#  وگرنه
	std::string proc_share_path = proc_self_path + "share/";
	اگر (check_directory_exists(proc_share_path, درست)) {
		yosys_share_dirname = proc_share_path;
		بازگشت;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	اگر (check_directory_exists(proc_share_path, درست)) {
		yosys_share_dirname = proc_share_path;
		بازگشت;
	}
	proc_share_path = YOSYS_DATDIR "/";
	اگر (check_directory_exists(proc_share_path, درست)) {
		yosys_share_dirname = proc_share_path;
		بازگشت;
	}
#  endif
}
#endif

تهی init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	اگر (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} وگرنه {
		yosys_abc_executable = ABCEXTERNAL;
	}
#وگرنه
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	اگر (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	اگر (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت نویسه *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	اگر (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	بازگشت yosys_share_dirname;
}

std::string proc_program_prefix()
{
	بازگشت YOSYS_PROGRAM_PREFIX;
}

منطقی fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	نویسه block[4096];
	وقتی (1) {
		اگر (fgets(block, 4096, f) == پوچ)
			بازگشت نادرست;
		buffer += block;
		اگر (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			وقتی (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			بازگشت درست;
		}
	}
}

ایستا تهی handle_label(std::string &command, منطقی &from_to_active, ثابت std::string &run_from, ثابت std::string &run_to)
{
	صحیح pos = 0;
	std::string label;

	وقتی (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	اگر (pos < GetSize(command) && command[pos] == '#')
		بازگشت;

	وقتی (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	اگر (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		اگر (label == run_from)
			from_to_active = درست;
		وگرنه اگر (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = نادرست;
	}
}

منطقی run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	اگر (design == nullptr)
		design = yosys_design;

	اگر (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
	    اگر (filename.size() >= extension.size()) {
	      بازگشت filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    بازگشت نادرست;
	  };

	  اگر (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  اگر (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } وگرنه اگر (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } وگرنه اگر (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } وگرنه اگر (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } وگرنه اگر (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } وگرنه اگر (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } وگرنه اگر (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } وگرنه اگر (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } وگرنه اگر (filename == "-") {
	    command = "script";
	  } وگرنه {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	اگر (command == "script")
	{
		std::string run_from, run_to;
		منطقی from_to_active = درست;

		اگر (from_to_label != پوچ) {
			size_t pos = from_to_label->find(':');
			اگر (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} وگرنه {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		اگر (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_درون_داد_files.درج(filename);
		}

		اگر (f == پوچ)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		صحیح backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		تلاش {
			std::string command;
			وقتی (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				وقتی (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_سطر;
					اگر (!fgetline(f, next_سطر))
						بشکن;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_سطر;
				}
				handle_label(command, from_to_active, run_from, run_to);
				اگر (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			اگر (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				اگر (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		بگیر (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			پرتابکن;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		اگر (filename != "-")
			fclose(f);

		بازگشت درست;
	}

	اگر (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		بازگشت درست;
	}

	log("\n");
	اگر (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} وگرنه {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	اگر (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.افزودن_back(filename);
		Pass::call(design, argv);
	} وگرنه
		Frontend::frontend_call(design, پوچ, filename, command);

	design->check();
	بازگشت نادرست;
}

تهی run_pass(std::string command, RTLIL::Design *design)
{
	اگر (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

تهی run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	اگر (design == nullptr)
		design = yosys_design;

	اگر (command == "auto") {
		اگر (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		وگرنه اگر (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		وگرنه اگر (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		وگرنه اگر (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		وگرنه اگر (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		وگرنه اگر (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		وگرنه اگر (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		وگرنه اگر (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		وگرنه اگر (filename == "-")
			command = "rtlil";
		وگرنه اگر (filename.empty())
			بازگشت;
		وگرنه
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	اگر (filename.empty())
		filename = "-";

	log("\n");
	اگر (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} وگرنه {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, پوچ, filename, command);
}

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ایستا نویسه *readline_cmd_generator(ثابت نویسه *text, صحیح state)
{
	ایستا std::map<std::string, Pass*>::iterator it;
	ایستا صحیح len;

	اگر (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	برای (; it != pass_register.end(); it++) {
		اگر (it->first.compare(0, len, text) == 0)
			بازگشت strdup((it++)->first.c_str());
	}
	بازگشت پوچ;
}

ایستا نویسه *readline_obj_generator(ثابت نویسه *text, صحیح state)
{
	ایستا std::vector<نویسه*> obj_names;
	ایستا size_t idx;

	اگر (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		صحیح len = strlen(text);

		اگر (design->selected_active_module.empty())
		{
			برای (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				اگر (mod_name.compare(0, len, text) == 0)
					obj_names.افزودن_back(strdup(mod_name.c_str()));
			}
		}
		وگرنه اگر (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			برای (auto w : module->wires())
				اگر (w->name.unescape().compare(0, len, text) == 0)
					obj_names.افزودن_back(strdup(w->name.unescape().c_str()));

			برای (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				اگر (mem_name.compare(0, len, text) == 0)
					obj_names.افزودن_back(strdup(mem_name.c_str()));
			}

			برای (auto cell : module->cells())
				اگر (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.افزودن_back(strdup(cell->name.unescape().c_str()));

			برای (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				اگر (proc_name.compare(0, len, text) == 0)
					obj_names.افزودن_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	اگر (idx < obj_names.size())
		بازگشت strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	بازگشت پوچ;
}

ایستا نویسه **readline_completion(ثابت نویسه *text, صحیح start, صحیح)
{
	اگر (start == 0)
		بازگشت rl_completion_matches(text, readline_cmd_generator);
	اگر (strncmp(rl_سطر_buffer, "read_", 5) && strncmp(rl_سطر_buffer, "write_", 6))
		بازگشت rl_completion_matches(text, readline_obj_generator);
	بازگشت پوچ;
}
#endif

تهی shell(RTLIL::Design *design)
{
	ایستا صحیح recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (نویسه*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (نویسه*)" \t\n";
#endif

	نویسه *command = پوچ;
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وقتی ((command = readline(create_prompt(design, recursion_counter))) != پوچ)
	{
#وگرنه
	نویسه command_buffer[4096];
	وقتی (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		اگر ((command = fgets(command_buffer, 4096, stdin)) == پوچ)
			بشکن;
#endif
		اگر (command[strspn(command, " \t\r\n")] == 0) {
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			آزاد(command);
#endif
			ادامه;
		}
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		نویسه *p = command + strspn(command, " \t\r\n");
		اگر (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			اگر (*p == 0)
				بشکن;
		}

		تلاش {
			log_assert(design->selection_پشته.size() == 1);
			Pass::call(design, command);
		} بگیر (log_cmd_error_exception) {
			وقتی (design->selection_پشته.size() > 1)
				design->برداشت_selection();
			log_reset_پشته();
		}
		design->check();
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		اگر (command)
			آزاد(command);
#endif
	}
	اگر (command == پوچ)
		printf("exit\n");
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وگرنه
		آزاد(command);
#endif
	recursion_counter--;
}

ساختار ShellPass : عمومی Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	تهی help() override {
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
	تهی execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, نادرست);
		shell(design);
	}
} ShellPass;

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ساختار HistoryPass : عمومی Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	تهی help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	تهی execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, نادرست);
#ifdef YOSYS_ENABLE_READLINE
		برای(HIST_ENTRY **list = history_list(); *list != پوچ; list++)
			log("%s\n", (*list)->سطر);
#وگرنه
		برای (صحیح i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->سطر);
#endif
	}
} HistoryPass;
#endif

ساختار ScriptCmdPass : عمومی Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	تهی help() override {
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
	تهی execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		منطقی scriptwire = نادرست;

		size_t argidx;
		برای (argidx = 1; argidx < args.size(); argidx++) {
			اگر (args[argidx] == "-scriptwire") {
				scriptwire = درست;
				ادامه;
			}
			بشکن;
		}
		اگر (scriptwire) {
			extra_args(args, argidx, design);

			برای (auto mod : design->selected_modules())
				برای (auto &c : mod->connections()) {
					اگر (!c.first.is_wire())
						ادامه;
					auto w = c.first.as_wire();
					اگر (!mod->selected(w))
						ادامه;
					اگر (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		وگرنه اگر (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		وگرنه اگر (args.size() == 2)
			run_frontend(args[1], "script", design);
		وگرنه اگر (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		وگرنه
			extra_args(args, 2, design, نادرست);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
