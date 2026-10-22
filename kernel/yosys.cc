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

#ꦪꦼꦤ꧀ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#elif defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#ꦭꦶꦪ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#ꦪꦼꦤ꧀ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#ꦪꦼꦤ꧀ defined(__FreeBSD__) || defined(__NetBSD__)
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
puur_naa_nka yosys_xtrace = 0;
bool yosys_write_versions = true;
const var_na* yosys_maybe_version() {
	ꦪꦼꦤ꧀ (yosys_write_versions)
		ꦮꦭꦶ yosys_version_str;
	ꦭꦶꦪ
		ꦮꦭꦶ "Yosys";
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

void Autoidx::ensure_at_least(puur_naa_nka v) {
	log_assert(!Multithreading::active());
	value = std::max(value, v);
}

puur_naa_nka Autoidx::operator++(puur_naa_nka) {
	log_assert(!Multithreading::active());
	ꦮꦭꦶ value++;
}

void memhasher_on()
{
#ꦪꦼꦤ꧀ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(NULL) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = true;
}

void memhasher_off()
{
	ꦏꦁꦒꦺꦴ (auto p : memhasher_store)
		ꦪꦼꦤ꧀ (p) free(p);
	memhasher_store.clear();
	memhasher_active = false;
}

void memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	puur_naa_nka size, index = (memhasher_rng >> 4) & 0xffff;
	switch (memhasher_rng & 7) {
		pak_sa 0: size =   16; bha_nga;
		pak_sa 1: size =  256; bha_nga;
		pak_sa 2: size = 1024; bha_nga;
		pak_sa 3: size = 4096; bha_nga;
		default: size = 0;
	}
	ꦪꦼꦤ꧀ (index < 16) size *= 16;
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

#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_SPAWN)
puur_naa_nka run_command(const std::string &command, std::function<void(const std::string&)> process_line)
{
	ꦪꦼꦤ꧀ (!process_line)
		ꦮꦭꦶ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ꦪꦼꦤ꧀ (f == nullptr)
		ꦮꦭꦶ -1;

	std::string line;
	var_na logbuf[128];
	ꦱꦼꦢꦼꦭ (true) {
		ꦪꦼꦤ꧀ (fgets(logbuf, 128, f) != NULL) {
			line += logbuf;
			ꦪꦼꦤ꧀ (!line.empty() && line.back() == '\n')
				process_line(line), line.clear();
		} ꦭꦶꦪ ꦪꦼꦤ꧀ (ferror(f) && errno == EINTR) {
			clearerr(f);
			continue;
		} ꦭꦶꦪ {
			bha_nga;
		}
	}
	ꦪꦼꦤ꧀ (!line.empty())
		process_line(line);

	puur_naa_nka ret = pclose(f);
	ꦪꦼꦤ꧀ (ret < 0)
		ꦮꦭꦶ -2;
#ifdef _WIN32
	ꦮꦭꦶ ret;
#ꦭꦶꦪ
	ꦪꦼꦤ꧀ (!WIFEXITED(ret))
		ꦮꦭꦶ -1;
	ꦮꦭꦶ WEXITSTATUS(ret);
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
	ꦪꦼꦤ꧀(already_setup)
		ꦮꦭꦶ;
	already_setup = true;
	already_shutdown = false;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ꦪꦼꦤ꧀ (!Py_IsInitialized()) {
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
	ꦮꦭꦶ already_setup;
}

void yosys_shutdown()
{
	ꦪꦼꦤ꧀(already_shutdown)
		ꦮꦭꦶ;
	already_setup = false;
	already_shutdown = true;
	log_pop();

	Pass::done_register();

	delete yosys_design;
	yosys_design = NULL;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	ꦪꦼꦤ꧀ (yosys_tcl_interp != NULL) {
		ꦪꦼꦤ꧀ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = NULL;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	ꦏꦁꦒꦺꦴ (auto &it : loaded_plugins)
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

const std::string *create_id_prefix(std::string_view file, puur_naa_nka line, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#ꦭꦶꦪ
	size_t pos = file.find_last_of('/');
#endif
	ꦪꦼꦤ꧀ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	ꦪꦼꦤ꧀ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ꦮꦭꦶ new std::string(stringf("$auto$%s:%d:%s$", file, line, func));
}

RTLIL::Design *yosys_get_design()
{
	ꦮꦭꦶ yosys_design;
}

const var_na *create_prompt(RTLIL::Design *design, puur_naa_nka recursion_counter, bool newline)
{
	static var_na buffer[100];
	std::string str = newline ? "\n" : "";
	ꦪꦼꦤ꧀ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ꦪꦼꦤ꧀ (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	ꦪꦼꦤ꧀ (!design->full_selection()) {
		ꦪꦼꦤ꧀ (design->selected_active_module.empty())
			str += "*";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.count(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	ꦮꦭꦶ buffer;
}

std::vector<std::string> glob_filename(const std::string &filename_pattern)
{
	std::vector<std::string> results;

#ꦪꦼꦤ꧀ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.push_back(filename_pattern);
#ꦭꦶꦪ
	glob_t globbuf;

	puur_naa_nka err = glob(filename_pattern.c_str(), 0, NULL, &globbuf);

	ꦪꦼꦤ꧀(err == 0) {
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.push_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} ꦭꦶꦪ {
		results.push_back(filename_pattern);
	}
#endif

	ꦮꦭꦶ results;
}

void rewrite_filename(std::string &filename)
{
	ꦪꦼꦤ꧀ (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	ꦪꦼꦤ꧀ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	ꦪꦼꦤ꧀ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
extern puur_naa_nka yosys_tcl_interp_init(Tcl_Interp *interp);

extern Tcl_Interp *yosys_get_tcl_interp()
{
	ꦪꦼꦤ꧀ (yosys_tcl_interp == NULL) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ꦮꦭꦶ yosys_tcl_interp;
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
		ꦪꦼꦤ꧀ (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		ꦏꦁꦒꦺꦴ (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.push_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), NULL, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), NULL, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), NULL, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		ꦪꦼꦤ꧀ (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#ꦪꦼꦤ꧀ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	ꦏꦁꦒꦺꦴ (puur_naa_nka tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		ꦪꦼꦤ꧀ (buflen < (ssize_t)path.size())
			bha_nga;
		ꦭꦶꦪ
			path.resize(path.size() * 2);
	}
	ꦪꦼꦤ꧀ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} ꦭꦶꦪ {
		ꦱꦼꦢꦼꦭ (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	ꦮꦭꦶ path;
}
#elif defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	puur_naa_nka mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#ꦭꦶꦪ
	puur_naa_nka mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	var_na *buffer;
	std::string path;
	ꦪꦼꦤ꧀ (sysctl(mib, 4, NULL, &buflen, NULL, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (var_na*)malloc(buflen);
	ꦪꦼꦤ꧀ (buffer == NULL)
		log_error("malloc failed: %s\n", strerror(errno));
	ꦪꦼꦤ꧀ (sysctl(mib, 4, buffer, &buflen, NULL, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ꦱꦼꦢꦼꦭ (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	free(buffer);
	ꦮꦭꦶ path;
}
#elif defined(__APPLE__)
std::string proc_self_dirname()
{
	var_na *path = NULL;
	uint32_t buflen = 0;
	ꦱꦼꦢꦼꦭ (_NSGetExecutablePath(path, &buflen) != 0)
		path = (var_na *) realloc((void *) path, buflen);
	var_na resolved[PATH_MAX];
	ꦪꦼꦤ꧀ (realpath(path, resolved) != NULL) {
		free(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	ꦱꦼꦢꦼꦭ (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	free(path);
	ꦮꦭꦶ str;
}
#elif defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ꦪꦼꦤ꧀ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), NULL, 0, NULL, NULL));
	ꦪꦼꦤ꧀ (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), NULL, NULL) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ꦮꦭꦶ ubinpath;

}
#elif defined(__wasm)
std::string proc_self_dirname()
{
	ꦮꦭꦶ "/";
}
#elif defined(__OpenBSD__) || defined(__HAIKU__)
var_na yosys_path[PATH_MAX];
var_na *yosys_argv0;

std::string proc_self_dirname(void)
{
	var_na buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	ꦪꦼꦤ꧀ (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, parimaa_na buf, "%*s/", (puur_naa_nka)(yosys_path - p), yosys_path);
		ꦮꦭꦶ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	ꦪꦼꦤ꧀ (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	ꦏꦁꦒꦺꦴ (p = strtok(path, ":"); p; p = strtok(NULL, ":")) {
		snprintf(buf, parimaa_na buf, "%s/%s", p, yosys_argv0);
		ꦪꦼꦤ꧀ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			free(path);
			ꦮꦭꦶ buf;
		}
	}
	free(path);
	log_error("Can't determine yosys executable path.\n");
	ꦮꦭꦶ NULL;
}
#ꦭꦶꦪ
	#error "Don't know how to determine process executable base path!"
#endif

#ꦪꦼꦤ꧀ defined(__wasm)
void init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#ꦭꦶꦪ
void init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ꦪꦼꦤ꧀ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		const var_na *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		ꦮꦭꦶ;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  ꦪꦼꦤ꧀ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	ꦪꦼꦤ꧀ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ꦮꦭꦶ;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	ꦪꦼꦤ꧀ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ꦮꦭꦶ;
	}
#  ꦭꦶꦪ
	std::string proc_share_path = proc_self_path + "share/";
	ꦪꦼꦤ꧀ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ꦮꦭꦶ;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	ꦪꦼꦤ꧀ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ꦮꦭꦶ;
	}
	proc_share_path = YOSYS_DATDIR "/";
	ꦪꦼꦤ꧀ (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		ꦮꦭꦶ;
	}
#  endif
}
#endif

void init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	ꦪꦼꦤ꧀ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} ꦭꦶꦪ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#ꦭꦶꦪ
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	ꦪꦼꦤ꧀ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ꦪꦼꦤ꧀ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		const var_na *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	ꦪꦼꦤ꧀ (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ꦮꦭꦶ yosys_share_dirname;
}

std::string proc_program_prefix()
{
	ꦮꦭꦶ YOSYS_PROGRAM_PREFIX;
}

bool fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	var_na block[4096];
	ꦱꦼꦢꦼꦭ (1) {
		ꦪꦼꦤ꧀ (fgets(block, 4096, f) == NULL)
			ꦮꦭꦶ false;
		buffer += block;
		ꦪꦼꦤ꧀ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			ꦱꦼꦢꦼꦭ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			ꦮꦭꦶ true;
		}
	}
}

static void handle_label(std::string &command, bool &from_to_active, const std::string &run_from, const std::string &run_to)
{
	puur_naa_nka pos = 0;
	std::string label;

	ꦱꦼꦢꦼꦭ (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ꦪꦼꦤ꧀ (pos < GetSize(command) && command[pos] == '#')
		ꦮꦭꦶ;

	ꦱꦼꦢꦼꦭ (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ꦪꦼꦤ꧀ (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		ꦪꦼꦤ꧀ (label == run_from)
			from_to_active = true;
		ꦭꦶꦪ ꦪꦼꦤ꧀ (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = false;
	}
}

bool run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	ꦪꦼꦤ꧀ (design == nullptr)
		design = yosys_design;

	ꦪꦼꦤ꧀ (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](const std::string& filename, const std::string& extension) {
	    ꦪꦼꦤ꧀ (filename.size() >= extension.size()) {
	      ꦮꦭꦶ filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    ꦮꦭꦶ false;
	  };

	  ꦪꦼꦤ꧀ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  ꦪꦼꦤ꧀ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } ꦭꦶꦪ ꦪꦼꦤ꧀ (filename == "-") {
	    command = "script";
	  } ꦭꦶꦪ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ꦪꦼꦤ꧀ (command == "script")
	{
		std::string run_from, run_to;
		bool from_to_active = true;

		ꦪꦼꦤ꧀ (from_to_label != NULL) {
			size_t pos = from_to_label->find(':');
			ꦪꦼꦤ꧀ (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} ꦭꦶꦪ {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ꦪꦼꦤ꧀ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_input_files.insert(filename);
		}

		ꦪꦼꦤ꧀ (f == NULL)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		puur_naa_nka backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		try {
			std::string command;
			ꦱꦼꦢꦼꦭ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ꦱꦼꦢꦼꦭ (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_line;
					ꦪꦼꦤ꧀ (!fgetline(f, next_line))
						bha_nga;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_line;
				}
				handle_label(command, from_to_active, run_from, run_to);
				ꦪꦼꦤ꧀ (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			ꦪꦼꦤ꧀ (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				ꦪꦼꦤ꧀ (from_to_active) {
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

		ꦪꦼꦤ꧀ (filename != "-")
			fclose(f);

		ꦮꦭꦶ true;
	}

	ꦪꦼꦤ꧀ (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		ꦮꦭꦶ true;
	}

	log("\n");
	ꦪꦼꦤ꧀ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} ꦭꦶꦪ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ꦪꦼꦤ꧀ (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.push_back(filename);
		Pass::call(design, argv);
	} ꦭꦶꦪ
		Frontend::frontend_call(design, NULL, filename, command);

	design->check();
	ꦮꦭꦶ false;
}

void run_pass(std::string command, RTLIL::Design *design)
{
	ꦪꦼꦤ꧀ (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

void run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ꦪꦼꦤ꧀ (design == nullptr)
		design = yosys_design;

	ꦪꦼꦤ꧀ (command == "auto") {
		ꦪꦼꦤ꧀ (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename == "-")
			command = "rtlil";
		ꦭꦶꦪ ꦪꦼꦤ꧀ (filename.empty())
			ꦮꦭꦶ;
		ꦭꦶꦪ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ꦪꦼꦤ꧀ (filename.empty())
		filename = "-";

	log("\n");
	ꦪꦼꦤ꧀ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} ꦭꦶꦪ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, NULL, filename, command);
}

#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
static var_na *readline_cmd_generator(const var_na *text, puur_naa_nka state)
{
	static std::map<std::string, Pass*>::iterator it;
	static puur_naa_nka len;

	ꦪꦼꦤ꧀ (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	ꦏꦁꦒꦺꦴ (; it != pass_register.end(); it++) {
		ꦪꦼꦤ꧀ (it->first.compare(0, len, text) == 0)
			ꦮꦭꦶ strdup((it++)->first.c_str());
	}
	ꦮꦭꦶ NULL;
}

static var_na *readline_obj_generator(const var_na *text, puur_naa_nka state)
{
	static std::vector<var_na*> obj_names;
	static size_t idx;

	ꦪꦼꦤ꧀ (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		puur_naa_nka len = strlen(text);

		ꦪꦼꦤ꧀ (design->selected_active_module.empty())
		{
			ꦏꦁꦒꦺꦴ (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ꦪꦼꦤ꧀ (mod_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(mod_name.c_str()));
			}
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			ꦏꦁꦒꦺꦴ (auto w : module->wires())
				ꦪꦼꦤ꧀ (w->name.unescape().compare(0, len, text) == 0)
					obj_names.push_back(strdup(w->name.unescape().c_str()));

			ꦏꦁꦒꦺꦴ (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				ꦪꦼꦤ꧀ (mem_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(mem_name.c_str()));
			}

			ꦏꦁꦒꦺꦴ (auto cell : module->cells())
				ꦪꦼꦤ꧀ (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.push_back(strdup(cell->name.unescape().c_str()));

			ꦏꦁꦒꦺꦴ (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				ꦪꦼꦤ꧀ (proc_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	ꦪꦼꦤ꧀ (idx < obj_names.size())
		ꦮꦭꦶ strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	ꦮꦭꦶ NULL;
}

static var_na **readline_completion(const var_na *text, puur_naa_nka start, puur_naa_nka)
{
	ꦪꦼꦤ꧀ (start == 0)
		ꦮꦭꦶ rl_completion_matches(text, readline_cmd_generator);
	ꦪꦼꦤ꧀ (strncmp(rl_line_buffer, "read_", 5) && strncmp(rl_line_buffer, "write_", 6))
		ꦮꦭꦶ rl_completion_matches(text, readline_obj_generator);
	ꦮꦭꦶ NULL;
}
#endif

void shell(RTLIL::Design *design)
{
	static puur_naa_nka recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (var_na*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (var_na*)" \t\n";
#endif

	var_na *command = NULL;
#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ꦱꦼꦢꦼꦭ ((command = readline(create_prompt(design, recursion_counter))) != NULL)
	{
#ꦭꦶꦪ
	var_na command_buffer[4096];
	ꦱꦼꦢꦼꦭ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ꦪꦼꦤ꧀ ((command = fgets(command_buffer, 4096, stdin)) == NULL)
			bha_nga;
#endif
		ꦪꦼꦤ꧀ (command[strspn(command, " \t\r\n")] == 0) {
#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			free(command);
#endif
			continue;
		}
#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		var_na *p = command + strspn(command, " \t\r\n");
		ꦪꦼꦤ꧀ (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ꦪꦼꦤ꧀ (*p == 0)
				bha_nga;
		}

		try {
			log_assert(design->selection_stack.size() == 1);
			Pass::call(design, command);
		} gr_haa_na (log_cmd_error_exception) {
			ꦱꦼꦢꦼꦭ (design->selection_stack.size() > 1)
				design->pop_selection();
			log_reset_stack();
		}
		design->check();
#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ꦪꦼꦤ꧀ (command)
			free(command);
#endif
	}
	ꦪꦼꦤ꧀ (command == NULL)
		printf("exit\n");
#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ꦭꦶꦪ
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

#ꦪꦼꦤ꧀ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
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
		ꦏꦁꦒꦺꦴ(HIST_ENTRY **list = history_list(); *list != NULL; list++)
			log("%s\n", (*list)->line);
#ꦭꦶꦪ
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = where_history(); history_get(i); i++)
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
		ꦏꦁꦒꦺꦴ (argidx = 1; argidx < args.size(); argidx++) {
			ꦪꦼꦤ꧀ (args[argidx] == "-scriptwire") {
				scriptwire = true;
				continue;
			}
			bha_nga;
		}
		ꦪꦼꦤ꧀ (scriptwire) {
			extra_args(args, argidx, design);

			ꦏꦁꦒꦺꦴ (auto mod : design->selected_modules())
				ꦏꦁꦒꦺꦴ (auto &c : mod->connections()) {
					ꦪꦼꦤ꧀ (!c.first.is_wire())
						continue;
					auto w = c.first.as_wire();
					ꦪꦼꦤ꧀ (!mod->selected(w))
						continue;
					ꦪꦼꦤ꧀ (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		ꦭꦶꦪ ꦪꦼꦤ꧀ (args.size() == 2)
			run_frontend(args[1], "script", design);
		ꦭꦶꦪ ꦪꦼꦤ꧀ (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		ꦭꦶꦪ
			extra_args(args, 2, design, false);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
