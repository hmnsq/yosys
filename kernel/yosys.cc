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
نطاق py = pybind11;
#endif

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
#endif

#إذا !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#إذا defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
خارجي "C" PyObject* PyInit_libyosys();
خارجي "C" PyObject* PyInit_pyosys();
#ضمن <signal.h>
#endif

#ضمن <limits.h>
#ضمن <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحيح yosys_xtrace = 0;
منطقي yosys_write_versions = true;
ثابت حرف* yosys_maybe_version() {
	إذا (yosys_write_versions)
		أرجع yosys_version_str;
	وإلا
		أرجع "Yosys";
}

RTLIL::Design *yosys_design = عدم;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = عدم;
Tcl_Interp *yosys_sdc_interp = عدم;
#endif

std::set<std::string> yosys_تێخستن_files, yosys_output_files;

منطقي memhasher_active = خطأ;
uint32_t memhasher_rng = 123456;
std::vector<فراغ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقي Multithreading::active_ = خطأ;

فراغ init_share_dirname();
فراغ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = true;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = خطأ;
}

فراغ Autoidx::ensure_at_least(صحيح v) {
	log_assert(!Multithreading::active());
	بەها = std::max(بەها, v);
}

صحيح Autoidx::operator++(صحيح) {
	log_assert(!Multithreading::active());
	أرجع بەها++;
}

فراغ memhasher_on()
{
#إذا defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(عدم) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = true;
}

فراغ memhasher_off()
{
	لكل (auto p : memhasher_store)
		إذا (p) حرر(p);
	memhasher_store.clear();
	memhasher_active = خطأ;
}

فراغ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	صحيح size, index = (memhasher_rng >> 4) & 0xffff;
	بدل (memhasher_rng & 7) {
		حالة 0: size =   16; اكسر;
		حالة 1: size =  256; اكسر;
		حالة 2: size = 1024; اكسر;
		حالة 3: size = 4096; اكسر;
		افتراضي: size = 0;
	}
	إذا (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

فراغ yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#إذا defined(YOSYS_ENABLE_SPAWN)
صحيح run_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_هێڵ)
{
	إذا (!process_هێڵ)
		أرجع system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	إذا (f == nullptr)
		أرجع -1;

	std::string هێڵ;
	حرف logbuf[128];
	طالما (true) {
		إذا (fgets(logbuf, 128, f) != عدم) {
			هێڵ += logbuf;
			إذا (!هێڵ.empty() && هێڵ.back() == '\n')
				process_هێڵ(هێڵ), هێڵ.clear();
		} وإلا إذا (ferror(f) && errno == EINTR) {
			clearerr(f);
			استمر;
		} وإلا {
			اكسر;
		}
	}
	إذا (!هێڵ.empty())
		process_هێڵ(هێڵ);

	صحيح ret = pclose(f);
	إذا (ret < 0)
		أرجع -2;
#ifdef _WIN32
	أرجع ret;
#وإلا
	إذا (!WIFEXITED(ret))
		أرجع -1;
	أرجع WEXITSTATUS(ret);
#endif
}
#endif

منطقي already_setup = خطأ;
منطقي already_shutdown = خطأ;

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
	(فراغ)_;
	ارم py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

فراغ yosys_setup()
{
	إذا(already_setup)
		أرجع;
	already_setup = true;
	already_shutdown = خطأ;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
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
		PyRun_SimpleString("import sys");
		إشارة(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = جديد RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_پاڵنان();
}

منطقي yosys_already_setup()
{
	أرجع already_setup;
}

فراغ yosys_shutdown()
{
	إذا(already_shutdown)
		أرجع;
	already_setup = خطأ;
	already_shutdown = true;
	log_دەرهێنان();

	Pass::done_register();

	delete yosys_design;
	yosys_design = عدم;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	إذا (yosys_tcl_interp != عدم) {
		إذا (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = عدم;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	لكل (auto &it : loaded_plugins)
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

ثابت std::string *create_id_prefix(std::string_view file, صحيح هێڵ, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#وإلا
	size_t pos = file.find_last_of('/');
#endif
	إذا (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	إذا (pos != std::string_view::npos)
		func = func.substr(pos+1);

	أرجع جديد std::string(stringf("$auto$%s:%d:%s$", file, هێڵ, func));
}

RTLIL::Design *yosys_get_design()
{
	أرجع yosys_design;
}

ثابت حرف *create_prompt(RTLIL::Design *design, صحيح recursion_counter, منطقي newline)
{
	ساكن حرف buffer[100];
	std::string str = newline ? "\n" : "";
	إذا (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	إذا (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	إذا (!design->full_selection()) {
		إذا (design->selected_active_module.empty())
			str += "*";
		وإلا إذا (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.ژماردن(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	أرجع buffer;
}

std::vector<std::string> glob_filename(ثابت std::string &filename_pattern)
{
	std::vector<std::string> results;

#إذا defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.پاڵنان_back(filename_pattern);
#وإلا
	glob_t globbuf;

	صحيح err = glob(filename_pattern.c_str(), 0, عدم, &globbuf);

	إذا(err == 0) {
		لكل (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.پاڵنان_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} وإلا {
		results.پاڵنان_back(filename_pattern);
	}
#endif

	أرجع results;
}

فراغ rewrite_filename(std::string &filename)
{
	إذا (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	إذا (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	إذا (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

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
	فراغ execute(std::vector<std::string> args, RTLIL::Design *) override {
		إذا (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		لكل (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.پاڵنان_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		إذا (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#إذا defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	لكل (صحيح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		إذا (buflen < (ssize_t)path.size())
			اكسر;
		وإلا
			path.resize(path.size() * 2);
	}
	إذا (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} وإلا {
		طالما (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	أرجع path;
}
#وإلاإذا defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	صحيح mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#وإلا
	صحيح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	حرف *buffer;
	std::string path;
	إذا (sysctl(mib, 4, عدم, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (حرف*)malloc(buflen);
	إذا (buffer == عدم)
		log_error("malloc failed: %s\n", strerror(errno));
	إذا (sysctl(mib, 4, buffer, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	طالما (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	حرر(buffer);
	أرجع path;
}
#وإلاإذا defined(__APPLE__)
std::string proc_self_dirname()
{
	حرف *path = عدم;
	uint32_t buflen = 0;
	طالما (_NSGetExecutablePath(path, &buflen) != 0)
		path = (حرف *) realloc((فراغ *) path, buflen);
	حرف resolved[PATH_MAX];
	إذا (realpath(path, resolved) != عدم) {
		حرر(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	طالما (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	حرر(path);
	أرجع str;
}
#وإلاإذا defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	إذا (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), عدم, 0, عدم, عدم));
	إذا (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), عدم, عدم) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	أرجع ubinpath;

}
#وإلاإذا defined(__wasm)
std::string proc_self_dirname()
{
	أرجع "/";
}
#وإلاإذا defined(__OpenBSD__) || defined(__HAIKU__)
حرف yosys_path[PATH_MAX];
حرف *yosys_argv0;

std::string proc_self_dirname(فراغ)
{
	حرف buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	إذا (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, حجم buf, "%*s/", (صحيح)(yosys_path - p), yosys_path);
		أرجع buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	إذا (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	لكل (p = strtok(path, ":"); p; p = strtok(عدم, ":")) {
		snprintf(buf, حجم buf, "%s/%s", p, yosys_argv0);
		إذا (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			حرر(path);
			أرجع buf;
		}
	}
	حرر(path);
	log_error("Can't determine yosys executable path.\n");
	أرجع عدم;
}
#وإلا
	#error "Don't know how to determine process executable base path!"
#endif

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
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت حرف *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		أرجع;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  إذا defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	إذا (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		أرجع;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	إذا (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		أرجع;
	}
#  وإلا
	std::string proc_share_path = proc_self_path + "share/";
	إذا (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		أرجع;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	إذا (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		أرجع;
	}
	proc_share_path = YOSYS_DATDIR "/";
	إذا (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		أرجع;
	}
#  endif
}
#endif

فراغ init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	إذا (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} وإلا {
		yosys_abc_executable = ABCEXTERNAL;
	}
#وإلا
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	إذا (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت حرف *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	إذا (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	أرجع yosys_share_dirname;
}

std::string proc_program_prefix()
{
	أرجع YOSYS_PROGRAM_PREFIX;
}

منطقي fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	حرف block[4096];
	طالما (1) {
		إذا (fgets(block, 4096, f) == عدم)
			أرجع خطأ;
		buffer += block;
		إذا (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			طالما (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			أرجع true;
		}
	}
}

ساكن فراغ handle_label(std::string &command, منطقي &from_to_active, ثابت std::string &run_from, ثابت std::string &run_to)
{
	صحيح pos = 0;
	std::string label;

	طالما (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	إذا (pos < GetSize(command) && command[pos] == '#')
		أرجع;

	طالما (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	إذا (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		إذا (label == run_from)
			from_to_active = true;
		وإلا إذا (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = خطأ;
	}
}

منطقي run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	إذا (design == nullptr)
		design = yosys_design;

	إذا (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
	    إذا (filename.size() >= extension.size()) {
	      أرجع filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    أرجع خطأ;
	  };

	  إذا (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
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
		std::string run_from, run_to;
		منطقي from_to_active = true;

		إذا (from_to_label != عدم) {
			size_t pos = from_to_label->find(':');
			إذا (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} وإلا {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		إذا (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_تێخستن_files.زیادکردن(filename);
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
				طالما (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_هێڵ;
					إذا (!fgetline(f, next_هێڵ))
						اكسر;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_هێڵ;
				}
				handle_label(command, from_to_active, run_from, run_to);
				إذا (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			إذا (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				إذا (from_to_active) {
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

		أرجع true;
	}

	إذا (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		أرجع true;
	}

	log("\n");
	إذا (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} وإلا {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	إذا (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.پاڵنان_back(filename);
		Pass::call(design, argv);
	} وإلا
		Frontend::frontend_call(design, عدم, filename, command);

	design->check();
	أرجع خطأ;
}

فراغ run_pass(std::string command, RTLIL::Design *design)
{
	إذا (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

فراغ run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	إذا (design == nullptr)
		design = yosys_design;

	إذا (command == "auto") {
		إذا (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		وإلا إذا (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		وإلا إذا (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		وإلا إذا (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		وإلا إذا (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		وإلا إذا (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		وإلا إذا (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		وإلا إذا (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		وإلا إذا (filename == "-")
			command = "rtlil";
		وإلا إذا (filename.empty())
			أرجع;
		وإلا
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	إذا (filename.empty())
		filename = "-";

	log("\n");
	إذا (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} وإلا {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, عدم, filename, command);
}

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ساكن حرف *readline_cmd_generator(ثابت حرف *text, صحيح state)
{
	ساكن std::map<std::string, Pass*>::iterator it;
	ساكن صحيح len;

	إذا (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	لكل (; it != pass_register.end(); it++) {
		إذا (it->first.compare(0, len, text) == 0)
			أرجع strdup((it++)->first.c_str());
	}
	أرجع عدم;
}

ساكن حرف *readline_obj_generator(ثابت حرف *text, صحيح state)
{
	ساكن std::vector<حرف*> obj_names;
	ساكن size_t idx;

	إذا (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		صحيح len = strlen(text);

		إذا (design->selected_active_module.empty())
		{
			لكل (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				إذا (mod_name.compare(0, len, text) == 0)
					obj_names.پاڵنان_back(strdup(mod_name.c_str()));
			}
		}
		وإلا إذا (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			لكل (auto w : module->wires())
				إذا (w->name.unescape().compare(0, len, text) == 0)
					obj_names.پاڵنان_back(strdup(w->name.unescape().c_str()));

			لكل (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				إذا (mem_name.compare(0, len, text) == 0)
					obj_names.پاڵنان_back(strdup(mem_name.c_str()));
			}

			لكل (auto cell : module->cells())
				إذا (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.پاڵنان_back(strdup(cell->name.unescape().c_str()));

			لكل (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				إذا (proc_name.compare(0, len, text) == 0)
					obj_names.پاڵنان_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	إذا (idx < obj_names.size())
		أرجع strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	أرجع عدم;
}

ساكن حرف **readline_completion(ثابت حرف *text, صحيح start, صحيح)
{
	إذا (start == 0)
		أرجع rl_completion_matches(text, readline_cmd_generator);
	إذا (strncmp(rl_هێڵ_buffer, "read_", 5) && strncmp(rl_هێڵ_buffer, "write_", 6))
		أرجع rl_completion_matches(text, readline_obj_generator);
	أرجع عدم;
}
#endif

فراغ shell(RTLIL::Design *design)
{
	ساكن صحيح recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (حرف*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (حرف*)" \t\n";
#endif

	حرف *command = عدم;
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	طالما ((command = readline(create_prompt(design, recursion_counter))) != عدم)
	{
#وإلا
	حرف command_buffer[4096];
	طالما (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		إذا ((command = fgets(command_buffer, 4096, stdin)) == عدم)
			اكسر;
#endif
		إذا (command[strspn(command, " \t\r\n")] == 0) {
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			حرر(command);
#endif
			استمر;
		}
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		حرف *p = command + strspn(command, " \t\r\n");
		إذا (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			إذا (*p == 0)
				اكسر;
		}

		حاول {
			log_assert(design->selection_کەڵەکە.size() == 1);
			Pass::call(design, command);
		} امسك (log_cmd_error_exception) {
			طالما (design->selection_کەڵەکە.size() > 1)
				design->دەرهێنان_selection();
			log_reset_کەڵەکە();
		}
		design->check();
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		إذا (command)
			حرر(command);
#endif
	}
	إذا (command == عدم)
		printf("exit\n");
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وإلا
		حرر(command);
#endif
	recursion_counter--;
}

بنية ShellPass : عام Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	فراغ help() override {
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
	فراغ execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, خطأ);
		shell(design);
	}
} ShellPass;

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
بنية HistoryPass : عام Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	فراغ help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	فراغ execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, خطأ);
#ifdef YOSYS_ENABLE_READLINE
		لكل(HIST_ENTRY **list = history_list(); *list != عدم; list++)
			log("%s\n", (*list)->هێڵ);
#وإلا
		لكل (صحيح i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->هێڵ);
#endif
	}
} HistoryPass;
#endif

بنية ScriptCmdPass : عام Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	فراغ help() override {
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
	فراغ execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		منطقي scriptwire = خطأ;

		size_t argidx;
		لكل (argidx = 1; argidx < args.size(); argidx++) {
			إذا (args[argidx] == "-scriptwire") {
				scriptwire = true;
				استمر;
			}
			اكسر;
		}
		إذا (scriptwire) {
			extra_args(args, argidx, design);

			لكل (auto mod : design->selected_modules())
				لكل (auto &c : mod->connections()) {
					إذا (!c.first.is_wire())
						استمر;
					auto w = c.first.as_wire();
					إذا (!mod->selected(w))
						استمر;
					إذا (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		وإلا إذا (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		وإلا إذا (args.size() == 2)
			run_frontend(args[1], "script", design);
		وإلا إذا (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		وإلا
			extra_args(args, 2, design, خطأ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
