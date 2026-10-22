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

#nếu defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#elif defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#không_thì
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  nếu defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#nếu !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#nếu defined(__FreeBSD__) || defined(__NetBSD__)
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
số_nguyên yosys_xtrace = 0;
bool yosys_write_versions = true;
const char* yosys_maybe_version() {
	nếu (yosys_write_versions)
		trả_về yosys_version_str;
	không_thì
		trả_về "Yosys";
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

void Autoidx::ensure_at_least(số_nguyên v) {
	log_assert(!Multithreading::active());
	value = std::max(value, v);
}

số_nguyên Autoidx::operator++(số_nguyên) {
	log_assert(!Multithreading::active());
	trả_về value++;
}

void memhasher_on()
{
#nếu defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(NULL) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = true;
}

void memhasher_off()
{
	cho (auto p : memhasher_store)
		nếu (p) free(p);
	memhasher_store.clear();
	memhasher_active = false;
}

void memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	số_nguyên size, index = (memhasher_rng >> 4) & 0xffff;
	switch (memhasher_rng & 7) {
		case 0: size =   16; break;
		case 1: size =  256; break;
		case 2: size = 1024; break;
		case 3: size = 4096; break;
		default: size = 0;
	}
	nếu (index < 16) size *= 16;
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

#nếu defined(YOSYS_ENABLE_SPAWN)
số_nguyên run_command(const std::string &command, std::function<void(const std::string&)> process_line)
{
	nếu (!process_line)
		trả_về system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	nếu (f == nullptr)
		trả_về -1;

	std::string line;
	char logbuf[128];
	trong_khi (true) {
		nếu (fgets(logbuf, 128, f) != NULL) {
			line += logbuf;
			nếu (!line.empty() && line.back() == '\n')
				process_line(line), line.clear();
		} không_thì nếu (ferror(f) && errno == EINTR) {
			clearerr(f);
			continue;
		} không_thì {
			break;
		}
	}
	nếu (!line.empty())
		process_line(line);

	số_nguyên ret = pclose(f);
	nếu (ret < 0)
		trả_về -2;
#ifdef _WIN32
	trả_về ret;
#không_thì
	nếu (!WIFEXITED(ret))
		trả_về -1;
	trả_về WEXITSTATUS(ret);
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
	throw py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

void yosys_setup()
{
	nếu(already_setup)
		trả_về;
	already_setup = true;
	already_shutdown = false;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	nếu (!Py_IsInitialized()) {
		PyImport_AppendInittab((char*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((char*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((char*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		signal(SIGINT, SIG_DFL);
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
	trả_về already_setup;
}

void yosys_shutdown()
{
	nếu(already_shutdown)
		trả_về;
	already_setup = false;
	already_shutdown = true;
	log_pop();

	Pass::done_register();

	delete yosys_design;
	yosys_design = NULL;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	nếu (yosys_tcl_interp != NULL) {
		nếu (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = NULL;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	cho (auto &it : loaded_plugins)
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

const std::string *create_id_prefix(std::string_view file, số_nguyên line, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#không_thì
	size_t pos = file.find_last_of('/');
#endif
	nếu (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	nếu (pos != std::string_view::npos)
		func = func.substr(pos+1);

	trả_về new std::string(stringf("$auto$%s:%d:%s$", file, line, func));
}

RTLIL::Design *yosys_get_design()
{
	trả_về yosys_design;
}

const char *create_prompt(RTLIL::Design *design, số_nguyên recursion_counter, bool newline)
{
	static char buffer[100];
	std::string str = newline ? "\n" : "";
	nếu (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	nếu (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	nếu (!design->full_selection()) {
		nếu (design->selected_active_module.empty())
			str += "*";
		không_thì nếu (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.count(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	trả_về buffer;
}

std::vector<std::string> glob_filename(const std::string &filename_pattern)
{
	std::vector<std::string> results;

#nếu defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.push_back(filename_pattern);
#không_thì
	glob_t globbuf;

	số_nguyên err = glob(filename_pattern.c_str(), 0, NULL, &globbuf);

	nếu(err == 0) {
		cho (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.push_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} không_thì {
		results.push_back(filename_pattern);
	}
#endif

	trả_về results;
}

void rewrite_filename(std::string &filename)
{
	nếu (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	nếu (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	nếu (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
extern số_nguyên yosys_tcl_interp_init(Tcl_Interp *interp);

extern Tcl_Interp *yosys_get_tcl_interp()
{
	nếu (yosys_tcl_interp == NULL) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	trả_về yosys_tcl_interp;
}

// Also see SdcPass
struct TclPass : public Pass {
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
		nếu (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		cho (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.push_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), NULL, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), NULL, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), NULL, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		nếu (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#nếu defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	cho (số_nguyên tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		nếu (buflen < (ssize_t)path.size())
			break;
		không_thì
			path.resize(path.size() * 2);
	}
	nếu (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} không_thì {
		trong_khi (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	trả_về path;
}
#elif defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	số_nguyên mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#không_thì
	số_nguyên mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	char *buffer;
	std::string path;
	nếu (sysctl(mib, 4, NULL, &buflen, NULL, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (char*)malloc(buflen);
	nếu (buffer == NULL)
		log_error("malloc failed: %s\n", strerror(errno));
	nếu (sysctl(mib, 4, buffer, &buflen, NULL, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	trong_khi (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	free(buffer);
	trả_về path;
}
#elif defined(__APPLE__)
std::string proc_self_dirname()
{
	char *path = NULL;
	uint32_t buflen = 0;
	trong_khi (_NSGetExecutablePath(path, &buflen) != 0)
		path = (char *) realloc((void *) path, buflen);
	char resolved[PATH_MAX];
	nếu (realpath(path, resolved) != NULL) {
		free(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	trong_khi (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	free(path);
	trả_về str;
}
#elif defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	nếu (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), NULL, 0, NULL, NULL));
	nếu (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), NULL, NULL) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	trả_về ubinpath;

}
#elif defined(__wasm)
std::string proc_self_dirname()
{
	trả_về "/";
}
#elif defined(__OpenBSD__) || defined(__HAIKU__)
char yosys_path[PATH_MAX];
char *yosys_argv0;

std::string proc_self_dirname(void)
{
	char buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	nếu (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, sizeof buf, "%*s/", (số_nguyên)(yosys_path - p), yosys_path);
		trả_về buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	nếu (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	cho (p = strtok(path, ":"); p; p = strtok(NULL, ":")) {
		snprintf(buf, sizeof buf, "%s/%s", p, yosys_argv0);
		nếu (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			free(path);
			trả_về buf;
		}
	}
	free(path);
	log_error("Can't determine yosys executable path.\n");
	trả_về NULL;
}
#không_thì
	#error "Don't know how to determine process executable base path!"
#endif

#nếu defined(__wasm)
void init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#không_thì
void init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	nếu (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		const char *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		trả_về;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  nếu defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	nếu (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		trả_về;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	nếu (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		trả_về;
	}
#  không_thì
	std::string proc_share_path = proc_self_path + "share/";
	nếu (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		trả_về;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	nếu (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		trả_về;
	}
	proc_share_path = YOSYS_DATDIR "/";
	nếu (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		trả_về;
	}
#  endif
}
#endif

void init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	nếu (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} không_thì {
		yosys_abc_executable = ABCEXTERNAL;
	}
#không_thì
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	nếu (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	nếu (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		const char *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	nếu (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	trả_về yosys_share_dirname;
}

std::string proc_program_prefix()
{
	trả_về YOSYS_PROGRAM_PREFIX;
}

bool fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	char block[4096];
	trong_khi (1) {
		nếu (fgets(block, 4096, f) == NULL)
			trả_về false;
		buffer += block;
		nếu (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			trong_khi (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			trả_về true;
		}
	}
}

static void handle_label(std::string &command, bool &from_to_active, const std::string &run_from, const std::string &run_to)
{
	số_nguyên pos = 0;
	std::string label;

	trong_khi (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	nếu (pos < GetSize(command) && command[pos] == '#')
		trả_về;

	trong_khi (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	nếu (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		nếu (label == run_from)
			from_to_active = true;
		không_thì nếu (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = false;
	}
}

bool run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	nếu (design == nullptr)
		design = yosys_design;

	nếu (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](const std::string& filename, const std::string& extension) {
	    nếu (filename.size() >= extension.size()) {
	      trả_về filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    trả_về false;
	  };

	  nếu (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  nếu (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } không_thì nếu (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } không_thì nếu (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } không_thì nếu (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } không_thì nếu (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } không_thì nếu (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } không_thì nếu (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } không_thì nếu (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } không_thì nếu (filename == "-") {
	    command = "script";
	  } không_thì {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	nếu (command == "script")
	{
		std::string run_from, run_to;
		bool from_to_active = true;

		nếu (from_to_label != NULL) {
			size_t pos = from_to_label->find(':');
			nếu (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} không_thì {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		nếu (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_input_files.insert(filename);
		}

		nếu (f == NULL)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		số_nguyên backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		try {
			std::string command;
			trong_khi (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				trong_khi (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_line;
					nếu (!fgetline(f, next_line))
						break;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_line;
				}
				handle_label(command, from_to_active, run_from, run_to);
				nếu (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			nếu (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				nếu (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		catch (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			throw;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		nếu (filename != "-")
			fclose(f);

		trả_về true;
	}

	nếu (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		trả_về true;
	}

	log("\n");
	nếu (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} không_thì {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	nếu (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.push_back(filename);
		Pass::call(design, argv);
	} không_thì
		Frontend::frontend_call(design, NULL, filename, command);

	design->check();
	trả_về false;
}

void run_pass(std::string command, RTLIL::Design *design)
{
	nếu (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

void run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	nếu (design == nullptr)
		design = yosys_design;

	nếu (command == "auto") {
		nếu (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		không_thì nếu (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		không_thì nếu (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		không_thì nếu (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		không_thì nếu (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		không_thì nếu (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		không_thì nếu (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		không_thì nếu (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		không_thì nếu (filename == "-")
			command = "rtlil";
		không_thì nếu (filename.empty())
			trả_về;
		không_thì
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	nếu (filename.empty())
		filename = "-";

	log("\n");
	nếu (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} không_thì {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, NULL, filename, command);
}

#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
static char *readline_cmd_generator(const char *text, số_nguyên state)
{
	static std::map<std::string, Pass*>::iterator it;
	static số_nguyên len;

	nếu (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	cho (; it != pass_register.end(); it++) {
		nếu (it->first.compare(0, len, text) == 0)
			trả_về strdup((it++)->first.c_str());
	}
	trả_về NULL;
}

static char *readline_obj_generator(const char *text, số_nguyên state)
{
	static std::vector<char*> obj_names;
	static size_t idx;

	nếu (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		số_nguyên len = strlen(text);

		nếu (design->selected_active_module.empty())
		{
			cho (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				nếu (mod_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(mod_name.c_str()));
			}
		}
		không_thì nếu (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			cho (auto w : module->wires())
				nếu (w->name.unescape().compare(0, len, text) == 0)
					obj_names.push_back(strdup(w->name.unescape().c_str()));

			cho (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				nếu (mem_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(mem_name.c_str()));
			}

			cho (auto cell : module->cells())
				nếu (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.push_back(strdup(cell->name.unescape().c_str()));

			cho (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				nếu (proc_name.compare(0, len, text) == 0)
					obj_names.push_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	nếu (idx < obj_names.size())
		trả_về strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	trả_về NULL;
}

static char **readline_completion(const char *text, số_nguyên start, số_nguyên)
{
	nếu (start == 0)
		trả_về rl_completion_matches(text, readline_cmd_generator);
	nếu (strncmp(rl_line_buffer, "read_", 5) && strncmp(rl_line_buffer, "write_", 6))
		trả_về rl_completion_matches(text, readline_obj_generator);
	trả_về NULL;
}
#endif

void shell(RTLIL::Design *design)
{
	static số_nguyên recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (char*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (char*)" \t\n";
#endif

	char *command = NULL;
#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	trong_khi ((command = readline(create_prompt(design, recursion_counter))) != NULL)
	{
#không_thì
	char command_buffer[4096];
	trong_khi (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		nếu ((command = fgets(command_buffer, 4096, stdin)) == NULL)
			break;
#endif
		nếu (command[strspn(command, " \t\r\n")] == 0) {
#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			free(command);
#endif
			continue;
		}
#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		char *p = command + strspn(command, " \t\r\n");
		nếu (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			nếu (*p == 0)
				break;
		}

		try {
			log_assert(design->selection_stack.size() == 1);
			Pass::call(design, command);
		} catch (log_cmd_error_exception) {
			trong_khi (design->selection_stack.size() > 1)
				design->pop_selection();
			log_reset_stack();
		}
		design->check();
#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		nếu (command)
			free(command);
#endif
	}
	nếu (command == NULL)
		printf("exit\n");
#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	không_thì
		free(command);
#endif
	recursion_counter--;
}

struct ShellPass : public Pass {
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

#nếu defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
struct HistoryPass : public Pass {
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
		cho(HIST_ENTRY **list = history_list(); *list != NULL; list++)
			log("%s\n", (*list)->line);
#không_thì
		cho (số_nguyên i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->line);
#endif
	}
} HistoryPass;
#endif

struct ScriptCmdPass : public Pass {
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
		cho (argidx = 1; argidx < args.size(); argidx++) {
			nếu (args[argidx] == "-scriptwire") {
				scriptwire = true;
				continue;
			}
			break;
		}
		nếu (scriptwire) {
			extra_args(args, argidx, design);

			cho (auto mod : design->selected_modules())
				cho (auto &c : mod->connections()) {
					nếu (!c.first.is_wire())
						continue;
					auto w = c.first.as_wire();
					nếu (!mod->selected(w))
						continue;
					nếu (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		không_thì nếu (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		không_thì nếu (args.size() == 2)
			run_frontend(args[1], "script", design);
		không_thì nếu (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		không_thì
			extra_args(args, 2, design, false);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
