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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/log.h"
#அந்தர்பாவ "kernel/newcelltypes.h"

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
நாமாகாஶ py = pybind11;
#endif

#எனில் defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#அல்லதெனில் defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#இல்லையெனில்
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  எனில் defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#எனில் !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#எனில் defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
வெளி "C" PyObject* PyInit_libyosys();
வெளி "C" PyObject* PyInit_pyosys();
#அந்தர்பாவ <signal.h>
#endif

#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
முழு yosys_xtrace = 0;
தருக்க yosys_write_versions = உண்மை;
மாறிலி எழுத்து* yosys_maybe_version() {
	எனில் (yosys_write_versions)
		திரும்பு yosys_version_str;
	இல்லையெனில்
		திரும்பு "Yosys";
}

RTLIL::Design *yosys_design = வெற்று;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = வெற்று;
Tcl_Interp *yosys_sdc_interp = வெற்று;
#endif

std::set<std::string> yosys_உள்ளீடு_files, yosys_output_files;

தருக்க memhasher_active = பொய்;
uint32_t memhasher_rng = 123456;
std::vector<வெறுமை*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

தருக்க Multithreading::active_ = பொய்;

வெறுமை init_share_dirname();
வெறுமை init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = உண்மை;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = பொய்;
}

வெறுமை Autoidx::ensure_at_least(முழு v) {
	log_assert(!Multithreading::active());
	மதிப்பு = std::max(மதிப்பு, v);
}

முழு Autoidx::operator++(முழு) {
	log_assert(!Multithreading::active());
	திரும்பு மதிப்பு++;
}

வெறுமை memhasher_on()
{
#எனில் defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(வெற்று) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = உண்மை;
}

வெறுமை memhasher_off()
{
	க்கு (auto p : memhasher_store)
		எனில் (p) விடுவி(p);
	memhasher_store.clear();
	memhasher_active = பொய்;
}

வெறுமை memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	முழு size, index = (memhasher_rng >> 4) & 0xffff;
	தேர்வு (memhasher_rng & 7) {
		நிலை 0: size =   16; நிறுத்து;
		நிலை 1: size =  256; நிறுத்து;
		நிலை 2: size = 1024; நிறுத்து;
		நிலை 3: size = 4096; நிறுத்து;
		இயல்பு: size = 0;
	}
	எனில் (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

வெறுமை yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#எனில் defined(YOSYS_ENABLE_SPAWN)
முழு run_command(மாறிலி std::string &command, std::function<வெறுமை(மாறிலி std::string&)> process_வரி)
{
	எனில் (!process_வரி)
		திரும்பு system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	எனில் (f == nullptr)
		திரும்பு -1;

	std::string வரி;
	எழுத்து logbuf[128];
	போது (உண்மை) {
		எனில் (fgets(logbuf, 128, f) != வெற்று) {
			வரி += logbuf;
			எனில் (!வரி.empty() && வரி.back() == '\n')
				process_வரி(வரி), வரி.clear();
		} இல்லையெனில் எனில் (ferror(f) && errno == EINTR) {
			clearerr(f);
			தொடர்;
		} இல்லையெனில் {
			நிறுத்து;
		}
	}
	எனில் (!வரி.empty())
		process_வரி(வரி);

	முழு ret = pclose(f);
	எனில் (ret < 0)
		திரும்பு -2;
#ifdef _WIN32
	திரும்பு ret;
#இல்லையெனில்
	எனில் (!WIFEXITED(ret))
		திரும்பு -1;
	திரும்பு WEXITSTATUS(ret);
#endif
}
#endif

தருக்க already_setup = பொய்;
தருக்க already_shutdown = பொய்;

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
	(வெறுமை)_;
	எறி py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

வெறுமை yosys_setup()
{
	எனில்(already_setup)
		திரும்பு;
	already_setup = உண்மை;
	already_shutdown = பொய்;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	எனில் (!Py_IsInitialized()) {
		PyImport_AppendInittab((எழுத்து*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((எழுத்து*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((எழுத்து*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		சமிக்ஞை(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = புதிய RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_தள்ளு();
}

தருக்க yosys_already_setup()
{
	திரும்பு already_setup;
}

வெறுமை yosys_shutdown()
{
	எனில்(already_shutdown)
		திரும்பு;
	already_setup = பொய்;
	already_shutdown = உண்மை;
	log_எடு();

	Pass::done_register();

	delete yosys_design;
	yosys_design = வெற்று;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	எனில் (yosys_tcl_interp != வெற்று) {
		எனில் (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = வெற்று;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	க்கு (auto &it : loaded_plugins)
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

மாறிலி std::string *create_id_prefix(std::string_view file, முழு வரி, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#இல்லையெனில்
	size_t pos = file.find_last_of('/');
#endif
	எனில் (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	எனில் (pos != std::string_view::npos)
		func = func.substr(pos+1);

	திரும்பு புதிய std::string(stringf("$auto$%s:%d:%s$", file, வரி, func));
}

RTLIL::Design *yosys_get_design()
{
	திரும்பு yosys_design;
}

மாறிலி எழுத்து *create_prompt(RTLIL::Design *design, முழு recursion_counter, தருக்க newline)
{
	நிலையான எழுத்து buffer[100];
	std::string str = newline ? "\n" : "";
	எனில் (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	எனில் (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	எனில் (!design->full_selection()) {
		எனில் (design->selected_active_module.empty())
			str += "*";
		இல்லையெனில் எனில் (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.மொத்தம்(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	திரும்பு buffer;
}

std::vector<std::string> glob_filename(மாறிலி std::string &filename_pattern)
{
	std::vector<std::string> results;

#எனில் defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.தள்ளு_back(filename_pattern);
#இல்லையெனில்
	glob_t globbuf;

	முழு err = glob(filename_pattern.c_str(), 0, வெற்று, &globbuf);

	எனில்(err == 0) {
		க்கு (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.தள்ளு_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} இல்லையெனில் {
		results.தள்ளு_back(filename_pattern);
	}
#endif

	திரும்பு results;
}

வெறுமை rewrite_filename(std::string &filename)
{
	எனில் (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	எனில் (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	எனில் (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
வெளி முழு yosys_tcl_interp_init(Tcl_Interp *interp);

வெளி Tcl_Interp *yosys_get_tcl_interp()
{
	எனில் (yosys_tcl_interp == வெற்று) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	திரும்பு yosys_tcl_interp;
}

// Also see SdcPass
கட்டமைப்பு TclPass : பொது Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	வெறுமை help() override {
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
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design *) override {
		எனில் (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		க்கு (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.தள்ளு_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), வெற்று, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), வெற்று, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), வெற்று, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		எனில் (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#எனில் defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	க்கு (முழு tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		எனில் (buflen < (ssize_t)path.size())
			நிறுத்து;
		இல்லையெனில்
			path.resize(path.size() * 2);
	}
	எனில் (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} இல்லையெனில் {
		போது (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	திரும்பு path;
}
#அல்லதெனில் defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	முழு mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#இல்லையெனில்
	முழு mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	எழுத்து *buffer;
	std::string path;
	எனில் (sysctl(mib, 4, வெற்று, &buflen, வெற்று, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (எழுத்து*)malloc(buflen);
	எனில் (buffer == வெற்று)
		log_error("malloc failed: %s\n", strerror(errno));
	எனில் (sysctl(mib, 4, buffer, &buflen, வெற்று, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	போது (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	விடுவி(buffer);
	திரும்பு path;
}
#அல்லதெனில் defined(__APPLE__)
std::string proc_self_dirname()
{
	எழுத்து *path = வெற்று;
	uint32_t buflen = 0;
	போது (_NSGetExecutablePath(path, &buflen) != 0)
		path = (எழுத்து *) realloc((வெறுமை *) path, buflen);
	எழுத்து resolved[PATH_MAX];
	எனில் (realpath(path, resolved) != வெற்று) {
		விடுவி(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	போது (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	விடுவி(path);
	திரும்பு str;
}
#அல்லதெனில் defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	எனில் (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), வெற்று, 0, வெற்று, வெற்று));
	எனில் (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), வெற்று, வெற்று) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	திரும்பு ubinpath;

}
#அல்லதெனில் defined(__wasm)
std::string proc_self_dirname()
{
	திரும்பு "/";
}
#அல்லதெனில் defined(__OpenBSD__) || defined(__HAIKU__)
எழுத்து yosys_path[PATH_MAX];
எழுத்து *yosys_argv0;

std::string proc_self_dirname(வெறுமை)
{
	எழுத்து buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	எனில் (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, அளவு buf, "%*s/", (முழு)(yosys_path - p), yosys_path);
		திரும்பு buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	எனில் (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	க்கு (p = strtok(path, ":"); p; p = strtok(வெற்று, ":")) {
		snprintf(buf, அளவு buf, "%s/%s", p, yosys_argv0);
		எனில் (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			விடுவி(path);
			திரும்பு buf;
		}
	}
	விடுவி(path);
	log_error("Can't determine yosys executable path.\n");
	திரும்பு வெற்று;
}
#இல்லையெனில்
	#error "Don't know how to determine process executable base path!"
#endif

#எனில் defined(__wasm)
வெறுமை init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#இல்லையெனில்
வெறுமை init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	எனில் (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		மாறிலி எழுத்து *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		திரும்பு;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  எனில் defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	எனில் (check_directory_exists(proc_share_path, உண்மை)) {
		yosys_share_dirname = proc_share_path;
		திரும்பு;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	எனில் (check_directory_exists(proc_share_path, உண்மை)) {
		yosys_share_dirname = proc_share_path;
		திரும்பு;
	}
#  இல்லையெனில்
	std::string proc_share_path = proc_self_path + "share/";
	எனில் (check_directory_exists(proc_share_path, உண்மை)) {
		yosys_share_dirname = proc_share_path;
		திரும்பு;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	எனில் (check_directory_exists(proc_share_path, உண்மை)) {
		yosys_share_dirname = proc_share_path;
		திரும்பு;
	}
	proc_share_path = YOSYS_DATDIR "/";
	எனில் (check_directory_exists(proc_share_path, உண்மை)) {
		yosys_share_dirname = proc_share_path;
		திரும்பு;
	}
#  endif
}
#endif

வெறுமை init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	எனில் (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} இல்லையெனில் {
		yosys_abc_executable = ABCEXTERNAL;
	}
#இல்லையெனில்
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	எனில் (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	எனில் (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		மாறிலி எழுத்து *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	எனில் (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	திரும்பு yosys_share_dirname;
}

std::string proc_program_prefix()
{
	திரும்பு YOSYS_PROGRAM_PREFIX;
}

தருக்க fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	எழுத்து block[4096];
	போது (1) {
		எனில் (fgets(block, 4096, f) == வெற்று)
			திரும்பு பொய்;
		buffer += block;
		எனில் (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			போது (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			திரும்பு உண்மை;
		}
	}
}

நிலையான வெறுமை handle_label(std::string &command, தருக்க &from_to_active, மாறிலி std::string &run_from, மாறிலி std::string &run_to)
{
	முழு pos = 0;
	std::string label;

	போது (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	எனில் (pos < GetSize(command) && command[pos] == '#')
		திரும்பு;

	போது (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	எனில் (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		எனில் (label == run_from)
			from_to_active = உண்மை;
		இல்லையெனில் எனில் (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = பொய்;
	}
}

தருக்க run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	எனில் (design == nullptr)
		design = yosys_design;

	எனில் (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](மாறிலி std::string& filename, மாறிலி std::string& extension) {
	    எனில் (filename.size() >= extension.size()) {
	      திரும்பு filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    திரும்பு பொய்;
	  };

	  எனில் (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  எனில் (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } இல்லையெனில் எனில் (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } இல்லையெனில் எனில் (filename == "-") {
	    command = "script";
	  } இல்லையெனில் {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	எனில் (command == "script")
	{
		std::string run_from, run_to;
		தருக்க from_to_active = உண்மை;

		எனில் (from_to_label != வெற்று) {
			size_t pos = from_to_label->find(':');
			எனில் (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} இல்லையெனில் {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		எனில் (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_உள்ளீடு_files.செருகு(filename);
		}

		எனில் (f == வெற்று)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		முழு backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		முயல் {
			std::string command;
			போது (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				போது (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_வரி;
					எனில் (!fgetline(f, next_வரி))
						நிறுத்து;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_வரி;
				}
				handle_label(command, from_to_active, run_from, run_to);
				எனில் (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			எனில் (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				எனில் (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		பிடி (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			எறி;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		எனில் (filename != "-")
			fclose(f);

		திரும்பு உண்மை;
	}

	எனில் (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		திரும்பு உண்மை;
	}

	log("\n");
	எனில் (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} இல்லையெனில் {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	எனில் (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.தள்ளு_back(filename);
		Pass::call(design, argv);
	} இல்லையெனில்
		Frontend::frontend_call(design, வெற்று, filename, command);

	design->check();
	திரும்பு பொய்;
}

வெறுமை run_pass(std::string command, RTLIL::Design *design)
{
	எனில் (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

வெறுமை run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	எனில் (design == nullptr)
		design = yosys_design;

	எனில் (command == "auto") {
		எனில் (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		இல்லையெனில் எனில் (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		இல்லையெனில் எனில் (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		இல்லையெனில் எனில் (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		இல்லையெனில் எனில் (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		இல்லையெனில் எனில் (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		இல்லையெனில் எனில் (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		இல்லையெனில் எனில் (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		இல்லையெனில் எனில் (filename == "-")
			command = "rtlil";
		இல்லையெனில் எனில் (filename.empty())
			திரும்பு;
		இல்லையெனில்
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	எனில் (filename.empty())
		filename = "-";

	log("\n");
	எனில் (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} இல்லையெனில் {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, வெற்று, filename, command);
}

#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
நிலையான எழுத்து *readline_cmd_generator(மாறிலி எழுத்து *text, முழு state)
{
	நிலையான std::map<std::string, Pass*>::iterator it;
	நிலையான முழு len;

	எனில் (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	க்கு (; it != pass_register.end(); it++) {
		எனில் (it->first.compare(0, len, text) == 0)
			திரும்பு strdup((it++)->first.c_str());
	}
	திரும்பு வெற்று;
}

நிலையான எழுத்து *readline_obj_generator(மாறிலி எழுத்து *text, முழு state)
{
	நிலையான std::vector<எழுத்து*> obj_names;
	நிலையான size_t idx;

	எனில் (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		முழு len = strlen(text);

		எனில் (design->selected_active_module.empty())
		{
			க்கு (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				எனில் (mod_name.compare(0, len, text) == 0)
					obj_names.தள்ளு_back(strdup(mod_name.c_str()));
			}
		}
		இல்லையெனில் எனில் (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			க்கு (auto w : module->wires())
				எனில் (w->name.unescape().compare(0, len, text) == 0)
					obj_names.தள்ளு_back(strdup(w->name.unescape().c_str()));

			க்கு (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				எனில் (mem_name.compare(0, len, text) == 0)
					obj_names.தள்ளு_back(strdup(mem_name.c_str()));
			}

			க்கு (auto cell : module->cells())
				எனில் (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.தள்ளு_back(strdup(cell->name.unescape().c_str()));

			க்கு (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				எனில் (proc_name.compare(0, len, text) == 0)
					obj_names.தள்ளு_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	எனில் (idx < obj_names.size())
		திரும்பு strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	திரும்பு வெற்று;
}

நிலையான எழுத்து **readline_completion(மாறிலி எழுத்து *text, முழு start, முழு)
{
	எனில் (start == 0)
		திரும்பு rl_completion_matches(text, readline_cmd_generator);
	எனில் (strncmp(rl_வரி_buffer, "read_", 5) && strncmp(rl_வரி_buffer, "write_", 6))
		திரும்பு rl_completion_matches(text, readline_obj_generator);
	திரும்பு வெற்று;
}
#endif

வெறுமை shell(RTLIL::Design *design)
{
	நிலையான முழு recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (எழுத்து*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (எழுத்து*)" \t\n";
#endif

	எழுத்து *command = வெற்று;
#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	போது ((command = readline(create_prompt(design, recursion_counter))) != வெற்று)
	{
#இல்லையெனில்
	எழுத்து command_buffer[4096];
	போது (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		எனில் ((command = fgets(command_buffer, 4096, stdin)) == வெற்று)
			நிறுத்து;
#endif
		எனில் (command[strspn(command, " \t\r\n")] == 0) {
#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			விடுவி(command);
#endif
			தொடர்;
		}
#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		எழுத்து *p = command + strspn(command, " \t\r\n");
		எனில் (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			எனில் (*p == 0)
				நிறுத்து;
		}

		முயல் {
			log_assert(design->selection_அடுக்கு.size() == 1);
			Pass::call(design, command);
		} பிடி (log_cmd_error_exception) {
			போது (design->selection_அடுக்கு.size() > 1)
				design->எடு_selection();
			log_reset_அடுக்கு();
		}
		design->check();
#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		எனில் (command)
			விடுவி(command);
#endif
	}
	எனில் (command == வெற்று)
		printf("exit\n");
#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	இல்லையெனில்
		விடுவி(command);
#endif
	recursion_counter--;
}

கட்டமைப்பு ShellPass : பொது Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	வெறுமை help() override {
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
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, பொய்);
		shell(design);
	}
} ShellPass;

#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
கட்டமைப்பு HistoryPass : பொது Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	வெறுமை help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, பொய்);
#ifdef YOSYS_ENABLE_READLINE
		க்கு(HIST_ENTRY **list = history_list(); *list != வெற்று; list++)
			log("%s\n", (*list)->வரி);
#இல்லையெனில்
		க்கு (முழு i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->வரி);
#endif
	}
} HistoryPass;
#endif

கட்டமைப்பு ScriptCmdPass : பொது Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	வெறுமை help() override {
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
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		தருக்க scriptwire = பொய்;

		size_t argidx;
		க்கு (argidx = 1; argidx < args.size(); argidx++) {
			எனில் (args[argidx] == "-scriptwire") {
				scriptwire = உண்மை;
				தொடர்;
			}
			நிறுத்து;
		}
		எனில் (scriptwire) {
			extra_args(args, argidx, design);

			க்கு (auto mod : design->selected_modules())
				க்கு (auto &c : mod->connections()) {
					எனில் (!c.first.is_wire())
						தொடர்;
					auto w = c.first.as_wire();
					எனில் (!mod->selected(w))
						தொடர்;
					எனில் (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		இல்லையெனில் எனில் (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		இல்லையெனில் எனில் (args.size() == 2)
			run_frontend(args[1], "script", design);
		இல்லையெனில் எனில் (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		இல்லையெனில்
			extra_args(args, 2, design, பொய்);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
