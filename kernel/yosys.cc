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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/log.h"
#અન્તર્ભાવ "kernel/newcelltypes.h"

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
નામાકાશ py = pybind11;
#endif

#જો defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#અથવાજો defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#નહીંતર
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  જો defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#જો !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#જો defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
બાહ્ય "C" PyObject* PyInit_libyosys();
બાહ્ય "C" PyObject* PyInit_pyosys();
#અન્તર્ભાવ <signal.h>
#endif

#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
પૂર્ણાંક yosys_xtrace = 0;
તાર્કિક yosys_write_versions = સાચું;
અચલ અક્ષર* yosys_maybe_version() {
	જો (yosys_write_versions)
		પાછા yosys_version_str;
	નહીંતર
		પાછા "Yosys";
}

RTLIL::Design *yosys_design = ખાલી;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ખાલી;
Tcl_Interp *yosys_sdc_interp = ખાલી;
#endif

std::set<std::string> yosys_આગત_files, yosys_output_files;

તાર્કિક memhasher_active = ખોટું;
uint32_t memhasher_rng = 123456;
std::vector<શૂન્ય*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

તાર્કિક Multithreading::active_ = ખોટું;

શૂન્ય init_share_dirname();
શૂન્ય init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = સાચું;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = ખોટું;
}

શૂન્ય Autoidx::ensure_at_least(પૂર્ણાંક v) {
	log_assert(!Multithreading::active());
	કિંમત = std::max(કિંમત, v);
}

પૂર્ણાંક Autoidx::operator++(પૂર્ણાંક) {
	log_assert(!Multithreading::active());
	પાછા કિંમત++;
}

શૂન્ય memhasher_on()
{
#જો defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(ખાલી) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = સાચું;
}

શૂન્ય memhasher_off()
{
	માટે (auto p : memhasher_store)
		જો (p) મુક્ત(p);
	memhasher_store.clear();
	memhasher_active = ખોટું;
}

શૂન્ય memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	પૂર્ણાંક size, index = (memhasher_rng >> 4) & 0xffff;
	પસંદગી (memhasher_rng & 7) {
		સ્થિતિ 0: size =   16; તોડો;
		સ્થિતિ 1: size =  256; તોડો;
		સ્થિતિ 2: size = 1024; તોડો;
		સ્થિતિ 3: size = 4096; તોડો;
		મૂળભૂત: size = 0;
	}
	જો (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

શૂન્ય yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#જો defined(YOSYS_ENABLE_SPAWN)
પૂર્ણાંક run_command(અચલ std::string &command, std::function<શૂન્ય(અચલ std::string&)> process_લીટી)
{
	જો (!process_લીટી)
		પાછા system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	જો (f == nullptr)
		પાછા -1;

	std::string લીટી;
	અક્ષર logbuf[128];
	જ્યારે (સાચું) {
		જો (fgets(logbuf, 128, f) != ખાલી) {
			લીટી += logbuf;
			જો (!લીટી.empty() && લીટી.back() == '\n')
				process_લીટી(લીટી), લીટી.clear();
		} નહીંતર જો (ferror(f) && errno == EINTR) {
			clearerr(f);
			ચાલુ;
		} નહીંતર {
			તોડો;
		}
	}
	જો (!લીટી.empty())
		process_લીટી(લીટી);

	પૂર્ણાંક ret = pclose(f);
	જો (ret < 0)
		પાછા -2;
#ifdef _WIN32
	પાછા ret;
#નહીંતર
	જો (!WIFEXITED(ret))
		પાછા -1;
	પાછા WEXITSTATUS(ret);
#endif
}
#endif

તાર્કિક already_setup = ખોટું;
તાર્કિક already_shutdown = ખોટું;

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
	(શૂન્ય)_;
	ફેંકો py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

શૂન્ય yosys_setup()
{
	જો(already_setup)
		પાછા;
	already_setup = સાચું;
	already_shutdown = ખોટું;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	જો (!Py_IsInitialized()) {
		PyImport_AppendInittab((અક્ષર*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((અક્ષર*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((અક્ષર*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		સંકેત(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = નવું RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_ધકેલો();
}

તાર્કિક yosys_already_setup()
{
	પાછા already_setup;
}

શૂન્ય yosys_shutdown()
{
	જો(already_shutdown)
		પાછા;
	already_setup = ખોટું;
	already_shutdown = સાચું;
	log_કાઢો();

	Pass::done_register();

	delete yosys_design;
	yosys_design = ખાલી;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	જો (yosys_tcl_interp != ખાલી) {
		જો (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ખાલી;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	માટે (auto &it : loaded_plugins)
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

અચલ std::string *create_id_prefix(std::string_view file, પૂર્ણાંક લીટી, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#નહીંતર
	size_t pos = file.find_last_of('/');
#endif
	જો (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	જો (pos != std::string_view::npos)
		func = func.substr(pos+1);

	પાછા નવું std::string(stringf("$auto$%s:%d:%s$", file, લીટી, func));
}

RTLIL::Design *yosys_get_design()
{
	પાછા yosys_design;
}

અચલ અક્ષર *create_prompt(RTLIL::Design *design, પૂર્ણાંક recursion_counter, તાર્કિક newline)
{
	સ્થિર અક્ષર buffer[100];
	std::string str = newline ? "\n" : "";
	જો (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	જો (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	જો (!design->full_selection()) {
		જો (design->selected_active_module.empty())
			str += "*";
		નહીંતર જો (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.ગણતરી(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	પાછા buffer;
}

std::vector<std::string> glob_filename(અચલ std::string &filename_pattern)
{
	std::vector<std::string> results;

#જો defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ધકેલો_back(filename_pattern);
#નહીંતર
	glob_t globbuf;

	પૂર્ણાંક err = glob(filename_pattern.c_str(), 0, ખાલી, &globbuf);

	જો(err == 0) {
		માટે (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.ધકેલો_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} નહીંતર {
		results.ધકેલો_back(filename_pattern);
	}
#endif

	પાછા results;
}

શૂન્ય rewrite_filename(std::string &filename)
{
	જો (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	જો (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	જો (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
બાહ્ય પૂર્ણાંક yosys_tcl_interp_init(Tcl_Interp *interp);

બાહ્ય Tcl_Interp *yosys_get_tcl_interp()
{
	જો (yosys_tcl_interp == ખાલી) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	પાછા yosys_tcl_interp;
}

// Also see SdcPass
સંરચના TclPass : જાહેર Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	શૂન્ય help() override {
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
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *) override {
		જો (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		માટે (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.ધકેલો_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ખાલી, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ખાલી, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ખાલી, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		જો (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#જો defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	માટે (પૂર્ણાંક tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		જો (buflen < (ssize_t)path.size())
			તોડો;
		નહીંતર
			path.resize(path.size() * 2);
	}
	જો (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} નહીંતર {
		જ્યારે (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	પાછા path;
}
#અથવાજો defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	પૂર્ણાંક mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#નહીંતર
	પૂર્ણાંક mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	અક્ષર *buffer;
	std::string path;
	જો (sysctl(mib, 4, ખાલી, &buflen, ખાલી, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (અક્ષર*)malloc(buflen);
	જો (buffer == ખાલી)
		log_error("malloc failed: %s\n", strerror(errno));
	જો (sysctl(mib, 4, buffer, &buflen, ખાલી, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	જ્યારે (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	મુક્ત(buffer);
	પાછા path;
}
#અથવાજો defined(__APPLE__)
std::string proc_self_dirname()
{
	અક્ષર *path = ખાલી;
	uint32_t buflen = 0;
	જ્યારે (_NSGetExecutablePath(path, &buflen) != 0)
		path = (અક્ષર *) realloc((શૂન્ય *) path, buflen);
	અક્ષર resolved[PATH_MAX];
	જો (realpath(path, resolved) != ખાલી) {
		મુક્ત(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	જ્યારે (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	મુક્ત(path);
	પાછા str;
}
#અથવાજો defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	જો (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), ખાલી, 0, ખાલી, ખાલી));
	જો (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), ખાલી, ખાલી) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	પાછા ubinpath;

}
#અથવાજો defined(__wasm)
std::string proc_self_dirname()
{
	પાછા "/";
}
#અથવાજો defined(__OpenBSD__) || defined(__HAIKU__)
અક્ષર yosys_path[PATH_MAX];
અક્ષર *yosys_argv0;

std::string proc_self_dirname(શૂન્ય)
{
	અક્ષર buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	જો (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, કદ buf, "%*s/", (પૂર્ણાંક)(yosys_path - p), yosys_path);
		પાછા buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	જો (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	માટે (p = strtok(path, ":"); p; p = strtok(ખાલી, ":")) {
		snprintf(buf, કદ buf, "%s/%s", p, yosys_argv0);
		જો (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			મુક્ત(path);
			પાછા buf;
		}
	}
	મુક્ત(path);
	log_error("Can't determine yosys executable path.\n");
	પાછા ખાલી;
}
#નહીંતર
	#error "Don't know how to determine process executable base path!"
#endif

#જો defined(__wasm)
શૂન્ય init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#નહીંતર
શૂન્ય init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	જો (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		અચલ અક્ષર *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		પાછા;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  જો defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	જો (check_directory_exists(proc_share_path, સાચું)) {
		yosys_share_dirname = proc_share_path;
		પાછા;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	જો (check_directory_exists(proc_share_path, સાચું)) {
		yosys_share_dirname = proc_share_path;
		પાછા;
	}
#  નહીંતર
	std::string proc_share_path = proc_self_path + "share/";
	જો (check_directory_exists(proc_share_path, સાચું)) {
		yosys_share_dirname = proc_share_path;
		પાછા;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	જો (check_directory_exists(proc_share_path, સાચું)) {
		yosys_share_dirname = proc_share_path;
		પાછા;
	}
	proc_share_path = YOSYS_DATDIR "/";
	જો (check_directory_exists(proc_share_path, સાચું)) {
		yosys_share_dirname = proc_share_path;
		પાછા;
	}
#  endif
}
#endif

શૂન્ય init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	જો (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} નહીંતર {
		yosys_abc_executable = ABCEXTERNAL;
	}
#નહીંતર
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	જો (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	જો (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		અચલ અક્ષર *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	જો (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	પાછા yosys_share_dirname;
}

std::string proc_program_prefix()
{
	પાછા YOSYS_PROGRAM_PREFIX;
}

તાર્કિક fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	અક્ષર block[4096];
	જ્યારે (1) {
		જો (fgets(block, 4096, f) == ખાલી)
			પાછા ખોટું;
		buffer += block;
		જો (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			જ્યારે (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			પાછા સાચું;
		}
	}
}

સ્થિર શૂન્ય handle_label(std::string &command, તાર્કિક &from_to_active, અચલ std::string &run_from, અચલ std::string &run_to)
{
	પૂર્ણાંક pos = 0;
	std::string label;

	જ્યારે (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	જો (pos < GetSize(command) && command[pos] == '#')
		પાછા;

	જ્યારે (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	જો (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		જો (label == run_from)
			from_to_active = સાચું;
		નહીંતર જો (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = ખોટું;
	}
}

તાર્કિક run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	જો (design == nullptr)
		design = yosys_design;

	જો (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](અચલ std::string& filename, અચલ std::string& extension) {
	    જો (filename.size() >= extension.size()) {
	      પાછા filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    પાછા ખોટું;
	  };

	  જો (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  જો (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } નહીંતર જો (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } નહીંતર જો (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } નહીંતર જો (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } નહીંતર જો (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } નહીંતર જો (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } નહીંતર જો (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } નહીંતર જો (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } નહીંતર જો (filename == "-") {
	    command = "script";
	  } નહીંતર {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	જો (command == "script")
	{
		std::string run_from, run_to;
		તાર્કિક from_to_active = સાચું;

		જો (from_to_label != ખાલી) {
			size_t pos = from_to_label->find(':');
			જો (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} નહીંતર {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		જો (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_આગત_files.ઉમેરો(filename);
		}

		જો (f == ખાલી)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		પૂર્ણાંક backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		પ્રયાસ {
			std::string command;
			જ્યારે (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				જ્યારે (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_લીટી;
					જો (!fgetline(f, next_લીટી))
						તોડો;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_લીટી;
				}
				handle_label(command, from_to_active, run_from, run_to);
				જો (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			જો (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				જો (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		પકડો (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ફેંકો;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		જો (filename != "-")
			fclose(f);

		પાછા સાચું;
	}

	જો (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		પાછા સાચું;
	}

	log("\n");
	જો (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} નહીંતર {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	જો (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.ધકેલો_back(filename);
		Pass::call(design, argv);
	} નહીંતર
		Frontend::frontend_call(design, ખાલી, filename, command);

	design->check();
	પાછા ખોટું;
}

શૂન્ય run_pass(std::string command, RTLIL::Design *design)
{
	જો (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

શૂન્ય run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	જો (design == nullptr)
		design = yosys_design;

	જો (command == "auto") {
		જો (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		નહીંતર જો (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		નહીંતર જો (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		નહીંતર જો (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		નહીંતર જો (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		નહીંતર જો (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		નહીંતર જો (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		નહીંતર જો (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		નહીંતર જો (filename == "-")
			command = "rtlil";
		નહીંતર જો (filename.empty())
			પાછા;
		નહીંતર
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	જો (filename.empty())
		filename = "-";

	log("\n");
	જો (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} નહીંતર {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ખાલી, filename, command);
}

#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
સ્થિર અક્ષર *readline_cmd_generator(અચલ અક્ષર *text, પૂર્ણાંક state)
{
	સ્થિર std::map<std::string, Pass*>::iterator it;
	સ્થિર પૂર્ણાંક len;

	જો (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	માટે (; it != pass_register.end(); it++) {
		જો (it->first.compare(0, len, text) == 0)
			પાછા strdup((it++)->first.c_str());
	}
	પાછા ખાલી;
}

સ્થિર અક્ષર *readline_obj_generator(અચલ અક્ષર *text, પૂર્ણાંક state)
{
	સ્થિર std::vector<અક્ષર*> obj_names;
	સ્થિર size_t idx;

	જો (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		પૂર્ણાંક len = strlen(text);

		જો (design->selected_active_module.empty())
		{
			માટે (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				જો (mod_name.compare(0, len, text) == 0)
					obj_names.ધકેલો_back(strdup(mod_name.c_str()));
			}
		}
		નહીંતર જો (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			માટે (auto w : module->wires())
				જો (w->name.unescape().compare(0, len, text) == 0)
					obj_names.ધકેલો_back(strdup(w->name.unescape().c_str()));

			માટે (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				જો (mem_name.compare(0, len, text) == 0)
					obj_names.ધકેલો_back(strdup(mem_name.c_str()));
			}

			માટે (auto cell : module->cells())
				જો (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.ધકેલો_back(strdup(cell->name.unescape().c_str()));

			માટે (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				જો (proc_name.compare(0, len, text) == 0)
					obj_names.ધકેલો_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	જો (idx < obj_names.size())
		પાછા strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	પાછા ખાલી;
}

સ્થિર અક્ષર **readline_completion(અચલ અક્ષર *text, પૂર્ણાંક start, પૂર્ણાંક)
{
	જો (start == 0)
		પાછા rl_completion_matches(text, readline_cmd_generator);
	જો (strncmp(rl_લીટી_buffer, "read_", 5) && strncmp(rl_લીટી_buffer, "write_", 6))
		પાછા rl_completion_matches(text, readline_obj_generator);
	પાછા ખાલી;
}
#endif

શૂન્ય shell(RTLIL::Design *design)
{
	સ્થિર પૂર્ણાંક recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (અક્ષર*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (અક્ષર*)" \t\n";
#endif

	અક્ષર *command = ખાલી;
#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	જ્યારે ((command = readline(create_prompt(design, recursion_counter))) != ખાલી)
	{
#નહીંતર
	અક્ષર command_buffer[4096];
	જ્યારે (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		જો ((command = fgets(command_buffer, 4096, stdin)) == ખાલી)
			તોડો;
#endif
		જો (command[strspn(command, " \t\r\n")] == 0) {
#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			મુક્ત(command);
#endif
			ચાલુ;
		}
#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		અક્ષર *p = command + strspn(command, " \t\r\n");
		જો (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			જો (*p == 0)
				તોડો;
		}

		પ્રયાસ {
			log_assert(design->selection_ઢગલો.size() == 1);
			Pass::call(design, command);
		} પકડો (log_cmd_error_exception) {
			જ્યારે (design->selection_ઢગલો.size() > 1)
				design->કાઢો_selection();
			log_reset_ઢગલો();
		}
		design->check();
#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		જો (command)
			મુક્ત(command);
#endif
	}
	જો (command == ખાલી)
		printf("exit\n");
#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	નહીંતર
		મુક્ત(command);
#endif
	recursion_counter--;
}

સંરચના ShellPass : જાહેર Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	શૂન્ય help() override {
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
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, ખોટું);
		shell(design);
	}
} ShellPass;

#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
સંરચના HistoryPass : જાહેર Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	શૂન્ય help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, ખોટું);
#ifdef YOSYS_ENABLE_READLINE
		માટે(HIST_ENTRY **list = history_list(); *list != ખાલી; list++)
			log("%s\n", (*list)->લીટી);
#નહીંતર
		માટે (પૂર્ણાંક i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->લીટી);
#endif
	}
} HistoryPass;
#endif

સંરચના ScriptCmdPass : જાહેર Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	શૂન્ય help() override {
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
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		તાર્કિક scriptwire = ખોટું;

		size_t argidx;
		માટે (argidx = 1; argidx < args.size(); argidx++) {
			જો (args[argidx] == "-scriptwire") {
				scriptwire = સાચું;
				ચાલુ;
			}
			તોડો;
		}
		જો (scriptwire) {
			extra_args(args, argidx, design);

			માટે (auto mod : design->selected_modules())
				માટે (auto &c : mod->connections()) {
					જો (!c.first.is_wire())
						ચાલુ;
					auto w = c.first.as_wire();
					જો (!mod->selected(w))
						ચાલુ;
					જો (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		નહીંતર જો (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		નહીંતર જો (args.size() == 2)
			run_frontend(args[1], "script", design);
		નહીંતર જો (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		નહીંતર
			extra_args(args, 2, design, ખોટું);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
