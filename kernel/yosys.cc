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

#include "kernel/yosys.h"
#include "kernel/log.h"
#include "kernel/newcelltypes.h"

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
namespace py = pybind11;
#endif

#ប្រសិនបើ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#elif defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#បើមិនដូច្នេះ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ប្រសិនបើ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#ប្រសិនបើ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#ប្រសិនបើ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
extern "C" PyObject* PyInit_libyosys();
extern "C" PyObject* PyInit_pyosys();
#include <signal.h>
#endif

#include <limits.h>
#include <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
ចំនួនគត់ yosys_xtrace = 0;
bool yosys_write_versions = true;
const var_na* yosys_maybe_version() {
	ប្រសិនបើ (yosys_write_versions)
		ត្រឡប់ yosys_version_str;
	បើមិនដូច្នេះ
		ត្រឡប់ "Yosys";
}

RTLIL::Design *yosys_design = NULL;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = NULL;
Tcl_Interp *yosys_sdc_interp = NULL;
#endif

std::set<std::string> yosys_input_files, yosys_output_files;

bool memhasher_active = false;
uint32_t memhasher_rng = 123456;
std::vector<void*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

bool Multithreading::active_ = false;

void init_share_dirname();
void init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = true;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = false;
}

void Autoidx::ensure_at_least(ចំនួនគត់ v) {
	log_assert(!Multithreading::active());
	value = std::max(value, v);
}

ចំនួនគត់ Autoidx::operator++(ចំនួនគត់) {
	log_assert(!Multithreading::active());
	ត្រឡប់ value++;
}

void memhasher_on()
{
#ប្រសិនបើ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(NULL) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = true;
}

void memhasher_off()
{
	សម្រាប់ (auto p : memhasher_store)
		ប្រសិនបើ (p) free(p);
	memhasher_store.clear();
	memhasher_active = false;
}

void memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	ចំនួនគត់ size, index = (memhasher_rng >> 4) & 0xffff;
	switch (memhasher_rng & 7) {
		pak_sa 0: size =   16; bha_nga;
		pak_sa 1: size =  256; bha_nga;
		pak_sa 2: size = 1024; bha_nga;
		pak_sa 3: size = 4096; bha_nga;
		default: size = 0;
	}
	ប្រសិនបើ (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

void yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#ប្រសិនបើ defined(YOSYS_ENABLE_SPAWN)
ចំនួនគត់ run_command(const std::string &command, std::function<void(const std::string&)> process_line)
{
	ប្រសិនបើ (!process_line)
		ត្រឡប់ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ប្រសិនបើ (f == nullptr)
		ត្រឡប់ -1;

	std::string line;
	var_na logbuf[128];
	ខណៈពេល (true) {
		ប្រសិនបើ (fgets(logbuf, 128, f) != NULL) {
			line += logbuf;
			ប្រសិនបើ (!line.empty() && line.back() == '\n')
				process_line(line), line.clear();
		} បើមិនដូច្នេះ ប្រសិនបើ (ferror(f) && errno == EINTR) {
			clearerr(f);
			continue;
		} បើមិនដូច្នេះ {
			bha_nga;
		}
	}
	ប្រសិនបើ (!line.empty())
		process_line(line);

	ចំនួនគត់ ret = pclose(f);
	ប្រសិនបើ (ret < 0)
		ត្រឡប់ -2;
#ifdef _WIN32
	ត្រឡប់ ret;
#បើមិនដូច្នេះ
	ប្រសិនបើ (!WIFEXITED(ret))
		ត្រឡប់ -1;
	ត្រឡប់ WEXITSTATUS(ret);
#endif
}
#endif

bool already_setup = false;
bool already_shutdown = false;

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
	(void)_;
	k_sipa py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

void yosys_setup()
{
	ប្រសិនបើ(already_setup)
		ត្រឡប់;
	already_setup = true;
	already_shutdown = false;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ប្រសិនបើ (!Py_IsInitialized()) {
		PyImport_AppendInittab((var_na*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((var_na*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((var_na*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		sa_nkeeta(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = new RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_push();
}

bool yosys_already_setup()
{
	ត្រឡប់ already_setup;
}

void yosys_shutdown()
{
	ប្រសិនបើ(already_shutdown)
		ត្រឡប់;
	already_setup = false;
	already_shutdown = true;
	log_pop();

	Pass::done_register();

	delete yosys_design;
	yosys_design = NULL;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	ប្រសិនបើ (yosys_tcl_interp != NULL) {
		ប្រសិនបើ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = NULL;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	សម្រាប់ (auto &it : loaded_plugins)
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

const std::string *create_id_prefix(std::string_view file, ចំនួនគត់ line, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#បើមិនដូច្នេះ
	size_t pos = file.find_last_of('/');
#endif
	ប្រសិនបើ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	ប្រសិនបើ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ត្រឡប់ new std::string(stringf("$auto$%s:%d:%s$", file, line, func));
}

RTLIL::Design *yosys_get_design()
{
	ត្រឡប់ yosys_design;
}

const var_na *create_prompt(RTLIL::Design *design, ចំនួនគត់ recursion_counter, bool newline)
{
	static var_na buffer[100];
	std::string str = newline ? "\n" : "";
	ប្រសិនបើ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ប្រសិនបើ (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	ប្រសិនបើ (!design->full_selection()) {
		ប្រសិនបើ (design->selected_active_module.empty())
			str += "*";
		បើមិនដូច្នេះ ប្រសិនបើ (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.count(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	ត្រឡប់ buffer;
}

std::vector<std::string> glob_filename(const std::string &filename_pattern)
{
	std::vector<std::string> results;

#ប្រសិនបើ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.push_back(filename_pattern);
#បើមិនដូច្នេះ
	glob_t globbuf;

	ចំនួនគត់ err = glob(filename_pattern.c_str(), 0, NULL, &globbuf);

	ប្រសិនបើ(err == 0) {
		សម្រាប់ (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.push_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} បើមិនដូច្នេះ {
		results.push_back(filename_pattern);
	}
#endif

	ត្រឡប់ results;
}

void rewrite_filename(std::string &filename)
{
	ប្រសិនបើ (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	ប្រសិនបើ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	ប្រសិនបើ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
extern ចំនួនគត់ yosys_tcl_interp_init(Tcl_Interp *interp);

extern Tcl_Interp *yosys_get_tcl_interp()
{
	ប្រសិនបើ (yosys_tcl_interp == NULL) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ត្រឡប់ yosys_tcl_interp;
}

// Also see SdcPass
sa_nracanaa TclPass : public Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	void help() override {
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
	void execute(std::vector<std::string> args, RTLIL::Design *) override {
		ប្រសិនបើ (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		សម្រាប់ (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.push_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), NULL, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), NULL, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), NULL, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		ប្រសិនបើ (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#ប្រសិនបើ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	សម្រាប់ (ចំនួនគត់ tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		ប្រសិនបើ (buflen < (ssize_t)path.size())
			bha_nga;
		បើមិនដូច្នេះ
			path.resize(path.size() * 2);
	}
	ប្រសិនបើ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} បើមិនដូច្នេះ {
		ខណៈពេល (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	ត្រឡប់ path;
}
#elif defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	ចំនួនគត់ mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#បើមិនដូច្នេះ
	ចំនួនគត់ mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	var_na *buffer;
	std::string path;
	ប្រសិនបើ (sysctl(mib, 4, NULL, &buflen, NULL, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (var_na*)malloc(buflen);
	ប្រសិនបើ (buffer == NULL)
		log_error("malloc failed: %s\n", strerror(errno));
	ប្រសិនបើ (sysctl(mib, 4, buffer, &buflen, NULL, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ខណៈពេល (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	free(buffer);
	ត្រឡប់ path;
}
#elif defined(__APPLE__)
std::string proc_self_dirname()
{
	var_na *path = NULL;
	uint32_t buflen = 0;
	ខណៈពេល (_NSGetExecutablePath(path, &buflen) != 0)
		path = (var_na *) realloc((void *) path, buflen);
	var_na resolved[PATH_MAX];
	ប្រសិនបើ (realpath(path, resolved) != NULL) {
		free(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	ខណៈពេល (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	free(path);
	ត្រឡប់ str;
}
#elif defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ប្រសិនបើ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), NULL, 0, NULL, NULL));
	ប្រសិនបើ (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), NULL, NULL) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ត្រឡប់ ubinpath;

}
#elif defined(__wasm)
std::string proc_self_dirname()
{
	ត្រឡប់ "/";
}
#elif defined(__OpenBSD__) || defined(__HAIKU__)
var_na yosys_path[PATH_MAX];
var_na *yosys_argv0;

std::string proc_self_dirname(void)
{
	var_na buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	ប្រសិនបើ (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, parimaa_na buf, "%*s/", (ចំនួនគត់)(yosys_path - p), yosys_path);
		ត្រឡប់ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	ប្រសិនបើ (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	សម្រាប់ (p = strtok(path, ":"); p; p = strtok(NULL, ":")) {
		snprintf(buf, parimaa_na buf, "%s/%s", p, yosys_argv0);
		ប្រសិនបើ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			free(path);
			ត្រឡប់ buf;
		}
	}
	free(path);
	log_error("Can't determine yosys executable path.\n");
	ត្រឡប់ NULL;
}
#បើមិនដូច្នេះ
	#error "Don't know how to determine process executable base path!"
#endif

#ប្រសិនបើ defined(__wasm)
void init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#បើមិនដូច្នេះ
void init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ប្រសិនបើ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		const var_na *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		ត្រឡប់;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  ប្រសិនបើ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	ប្រសិនបើ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ត្រឡប់;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	ប្រសិនបើ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ត្រឡប់;
	}
#  បើមិនដូច្នេះ
	std::string proc_share_path = proc_self_path + "share/";
	ប្រសិនបើ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ត្រឡប់;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	ប្រសិនបើ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ត្រឡប់;
	}
	proc_share_path = YOSYS_DATDIR "/";
	ប្រសិនបើ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ត្រឡប់;
	}
#  endif
}
#endif

void init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	ប្រសិនបើ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} បើមិនដូច្នេះ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#បើមិនដូច្នេះ
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	ប្រសិនបើ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ប្រសិនបើ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		const var_na *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	ប្រសិនបើ (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ត្រឡប់ yosys_share_dirname;
}

std::string proc_program_prefix()
{
	ត្រឡប់ YOSYS_PROGRAM_PREFIX;
}

bool fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	var_na block[4096];
	ខណៈពេល (1) {
		ប្រសិនបើ (fgets(block, 4096, f) == NULL)
			ត្រឡប់ false;
		buffer += block;
		ប្រសិនបើ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			ខណៈពេល (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			ត្រឡប់ true;
		}
	}
}

static void handle_label(std::string &command, bool &from_to_active, const std::string &run_from, const std::string &run_to)
{
	ចំនួនគត់ pos = 0;
	std::string label;

	ខណៈពេល (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ប្រសិនបើ (pos < GetSize(command) && command[pos] == '#')
		ត្រឡប់;

	ខណៈពេល (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ប្រសិនបើ (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		ប្រសិនបើ (label == run_from)
			from_to_active = true;
		បើមិនដូច្នេះ ប្រសិនបើ (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = false;
	}
}

bool run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	ប្រសិនបើ (design == nullptr)
		design = yosys_design;

	ប្រសិនបើ (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](const std::string& filename, const std::string& extension) {
	    ប្រសិនបើ (filename.size() >= extension.size()) {
	      ត្រឡប់ filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    ត្រឡប់ false;
	  };

	  ប្រសិនបើ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  ប្រសិនបើ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } បើមិនដូច្នេះ ប្រសិនបើ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } បើមិនដូច្នេះ ប្រសិនបើ (filename == "-") {
	    command = "script";
	  } បើមិនដូច្នេះ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ប្រសិនបើ (command == "script")
	{
		std::string run_from, run_to;
		bool from_to_active = true;

		ប្រសិនបើ (from_to_label != NULL) {
			size_t pos = from_to_label->find(':');
			ប្រសិនបើ (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} បើមិនដូច្នេះ {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ប្រសិនបើ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_input_files.insert(filename);
		}

		ប្រសិនបើ (f == NULL)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		ចំនួនគត់ backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		try {
			std::string command;
			ខណៈពេល (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ខណៈពេល (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_line;
					ប្រសិនបើ (!fgetline(f, next_line))
						bha_nga;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_line;
				}
				handle_label(command, from_to_active, run_from, run_to);
				ប្រសិនបើ (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			ប្រសិនបើ (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				ប្រសិនបើ (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		gr_haa_na (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			k_sipa;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		ប្រសិនបើ (filename != "-")
			fclose(f);

		ត្រឡប់ true;
	}

	ប្រសិនបើ (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		ត្រឡប់ true;
	}

	log("\n");
	ប្រសិនបើ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} បើមិនដូច្នេះ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ប្រសិនបើ (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.push_back(filename);
		Pass::call(design, argv);
	} បើមិនដូច្នេះ
		Frontend::frontend_call(design, NULL, filename, command);

	design->check();
	ត្រឡប់ false;
}

void run_pass(std::string command, RTLIL::Design *design)
{
	ប្រសិនបើ (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

void run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ប្រសិនបើ (design == nullptr)
		design = yosys_design;

	ប្រសិនបើ (command == "auto") {
		ប្រសិនបើ (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		បើមិនដូច្នេះ ប្រសិនបើ (filename == "-")
			command = "rtlil";
		បើមិនដូច្នេះ ប្រសិនបើ (filename.empty())
			ត្រឡប់;
		បើមិនដូច្នេះ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ប្រសិនបើ (filename.empty())
		filename = "-";

	log("\n");
	ប្រសិនបើ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} បើមិនដូច្នេះ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, NULL, filename, command);
}

#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
static var_na *readline_cmd_generator(const var_na *text, ចំនួនគត់ state)
{
	static std::map<std::string, Pass*>::iterator it;
	static ចំនួនគត់ len;

	ប្រសិនបើ (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	សម្រាប់ (; it != pass_register.end(); it++) {
		ប្រសិនបើ (it->first.compare(0, len, text) == 0)
			ត្រឡប់ strdup((it++)->first.c_str());
	}
	ត្រឡប់ NULL;
}

static var_na *readline_obj_generator(const var_na *text, ចំនួនគត់ state)
{
	static std::vector<var_na*> obj_names;
	static size_t idx;

	ប្រសិនបើ (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		ចំនួនគត់ len = strlen(text);

		ប្រសិនបើ (design->selected_active_module.empty())
		{
			សម្រាប់ (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ប្រសិនបើ (mod_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(mod_name.c_str()));
			}
		}
		បើមិនដូច្នេះ ប្រសិនបើ (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			សម្រាប់ (auto w : module->wires())
				ប្រសិនបើ (w->name.unescape().compare(0, len, text) == 0)
					obj_names.push_back(strdup(w->name.unescape().c_str()));

			សម្រាប់ (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				ប្រសិនបើ (mem_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(mem_name.c_str()));
			}

			សម្រាប់ (auto cell : module->cells())
				ប្រសិនបើ (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.push_back(strdup(cell->name.unescape().c_str()));

			សម្រាប់ (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				ប្រសិនបើ (proc_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	ប្រសិនបើ (idx < obj_names.size())
		ត្រឡប់ strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	ត្រឡប់ NULL;
}

static var_na **readline_completion(const var_na *text, ចំនួនគត់ start, ចំនួនគត់)
{
	ប្រសិនបើ (start == 0)
		ត្រឡប់ rl_completion_matches(text, readline_cmd_generator);
	ប្រសិនបើ (strncmp(rl_line_buffer, "read_", 5) && strncmp(rl_line_buffer, "write_", 6))
		ត្រឡប់ rl_completion_matches(text, readline_obj_generator);
	ត្រឡប់ NULL;
}
#endif

void shell(RTLIL::Design *design)
{
	static ចំនួនគត់ recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (var_na*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (var_na*)" \t\n";
#endif

	var_na *command = NULL;
#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ខណៈពេល ((command = readline(create_prompt(design, recursion_counter))) != NULL)
	{
#បើមិនដូច្នេះ
	var_na command_buffer[4096];
	ខណៈពេល (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ប្រសិនបើ ((command = fgets(command_buffer, 4096, stdin)) == NULL)
			bha_nga;
#endif
		ប្រសិនបើ (command[strspn(command, " \t\r\n")] == 0) {
#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			free(command);
#endif
			continue;
		}
#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		var_na *p = command + strspn(command, " \t\r\n");
		ប្រសិនបើ (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ប្រសិនបើ (*p == 0)
				bha_nga;
		}

		try {
			log_assert(design->selection_stack.size() == 1);
			Pass::call(design, command);
		} gr_haa_na (log_cmd_error_exception) {
			ខណៈពេល (design->selection_stack.size() > 1)
				design->pop_selection();
			log_reset_stack();
		}
		design->check();
#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ប្រសិនបើ (command)
			free(command);
#endif
	}
	ប្រសិនបើ (command == NULL)
		printf("exit\n");
#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	បើមិនដូច្នេះ
		free(command);
#endif
	recursion_counter--;
}

sa_nracanaa ShellPass : public Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	void help() override {
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
	void execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, false);
		shell(design);
	}
} ShellPass;

#ប្រសិនបើ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
sa_nracanaa HistoryPass : public Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	void help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	void execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, false);
#ifdef YOSYS_ENABLE_READLINE
		សម្រាប់(HIST_ENTRY **list = history_list(); *list != NULL; list++)
			log("%s\n", (*list)->line);
#បើមិនដូច្នេះ
		សម្រាប់ (ចំនួនគត់ i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->line);
#endif
	}
} HistoryPass;
#endif

sa_nracanaa ScriptCmdPass : public Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	void help() override {
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
	void execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		bool scriptwire = false;

		size_t argidx;
		សម្រាប់ (argidx = 1; argidx < args.size(); argidx++) {
			ប្រសិនបើ (args[argidx] == "-scriptwire") {
				scriptwire = true;
				continue;
			}
			bha_nga;
		}
		ប្រសិនបើ (scriptwire) {
			extra_args(args, argidx, design);

			សម្រាប់ (auto mod : design->selected_modules())
				សម្រាប់ (auto &c : mod->connections()) {
					ប្រសិនបើ (!c.first.is_wire())
						continue;
					auto w = c.first.as_wire();
					ប្រសិនបើ (!mod->selected(w))
						continue;
					ប្រសិនបើ (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		បើមិនដូច្នេះ ប្រសិនបើ (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		បើមិនដូច្នេះ ប្រសិនបើ (args.size() == 2)
			run_frontend(args[1], "script", design);
		បើមិនដូច្នេះ ប្រសិនបើ (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		បើមិនដូច្នេះ
			extra_args(args, 2, design, false);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
