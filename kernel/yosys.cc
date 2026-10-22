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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/log.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"

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
നാമാകാശ py = pybind11;
#endif

#എങ്കിൽ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#അല്ലെങ്കിലെങ്കിൽ defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#അല്ലെങ്കിൽ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  എങ്കിൽ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#എങ്കിൽ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#എങ്കിൽ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
ബാഹ്യ "C" PyObject* PyInit_libyosys();
ബാഹ്യ "C" PyObject* PyInit_pyosys();
#അന്തര്ഭാവ <signal.h>
#endif

#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
പൂർണ്ണസംഖ്യ yosys_xtrace = 0;
തർക്കം yosys_write_versions = സത്യം;
സ്ഥിരം അക്ഷരം* yosys_maybe_version() {
	എങ്കിൽ (yosys_write_versions)
		തിരികെ yosys_version_str;
	അല്ലെങ്കിൽ
		തിരികെ "Yosys";
}

RTLIL::Design *yosys_design = ശൂന്യ;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ശൂന്യ;
Tcl_Interp *yosys_sdc_interp = ശൂന്യ;
#endif

std::set<std::string> yosys_നിവേശം_files, yosys_output_files;

തർക്കം memhasher_active = അസത്യം;
uint32_t memhasher_rng = 123456;
std::vector<ശൂന്യം*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

തർക്കം Multithreading::active_ = അസത്യം;

ശൂന്യം init_share_dirname();
ശൂന്യം init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = സത്യം;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = അസത്യം;
}

ശൂന്യം Autoidx::ensure_at_least(പൂർണ്ണസംഖ്യ v) {
	log_assert(!Multithreading::active());
	മൂല്യം = std::max(മൂല്യം, v);
}

പൂർണ്ണസംഖ്യ Autoidx::operator++(പൂർണ്ണസംഖ്യ) {
	log_assert(!Multithreading::active());
	തിരികെ മൂല്യം++;
}

ശൂന്യം memhasher_on()
{
#എങ്കിൽ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(ശൂന്യ) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = സത്യം;
}

ശൂന്യം memhasher_off()
{
	വേണ്ടി (auto p : memhasher_store)
		എങ്കിൽ (p) സ്വതന്ത്രം(p);
	memhasher_store.clear();
	memhasher_active = അസത്യം;
}

ശൂന്യം memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	പൂർണ്ണസംഖ്യ size, index = (memhasher_rng >> 4) & 0xffff;
	തിരഞ്ഞെടുപ്പ് (memhasher_rng & 7) {
		അവസ്ഥ 0: size =   16; നിർത്തുക;
		അവസ്ഥ 1: size =  256; നിർത്തുക;
		അവസ്ഥ 2: size = 1024; നിർത്തുക;
		അവസ്ഥ 3: size = 4096; നിർത്തുക;
		സ്ഥിരസ്ഥിതി: size = 0;
	}
	എങ്കിൽ (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

ശൂന്യം yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#എങ്കിൽ defined(YOSYS_ENABLE_SPAWN)
പൂർണ്ണസംഖ്യ run_command(സ്ഥിരം std::string &command, std::function<ശൂന്യം(സ്ഥിരം std::string&)> process_വരി)
{
	എങ്കിൽ (!process_വരി)
		തിരികെ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	എങ്കിൽ (f == nullptr)
		തിരികെ -1;

	std::string വരി;
	അക്ഷരം logbuf[128];
	ആകുമ്പോൾ (സത്യം) {
		എങ്കിൽ (fgets(logbuf, 128, f) != ശൂന്യ) {
			വരി += logbuf;
			എങ്കിൽ (!വരി.empty() && വരി.back() == '\n')
				process_വരി(വരി), വരി.clear();
		} അല്ലെങ്കിൽ എങ്കിൽ (ferror(f) && errno == EINTR) {
			clearerr(f);
			തുടരുക;
		} അല്ലെങ്കിൽ {
			നിർത്തുക;
		}
	}
	എങ്കിൽ (!വരി.empty())
		process_വരി(വരി);

	പൂർണ്ണസംഖ്യ ret = pclose(f);
	എങ്കിൽ (ret < 0)
		തിരികെ -2;
#ifdef _WIN32
	തിരികെ ret;
#അല്ലെങ്കിൽ
	എങ്കിൽ (!WIFEXITED(ret))
		തിരികെ -1;
	തിരികെ WEXITSTATUS(ret);
#endif
}
#endif

തർക്കം already_setup = അസത്യം;
തർക്കം already_shutdown = അസത്യം;

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
	(ശൂന്യം)_;
	എറിയുക py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

ശൂന്യം yosys_setup()
{
	എങ്കിൽ(already_setup)
		തിരികെ;
	already_setup = സത്യം;
	already_shutdown = അസത്യം;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	എങ്കിൽ (!Py_IsInitialized()) {
		PyImport_AppendInittab((അക്ഷരം*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((അക്ഷരം*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((അക്ഷരം*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		സൂചന(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = പുതിയ RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_തള്ളുക();
}

തർക്കം yosys_already_setup()
{
	തിരികെ already_setup;
}

ശൂന്യം yosys_shutdown()
{
	എങ്കിൽ(already_shutdown)
		തിരികെ;
	already_setup = അസത്യം;
	already_shutdown = സത്യം;
	log_എടുക്കുക();

	Pass::done_register();

	delete yosys_design;
	yosys_design = ശൂന്യ;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	എങ്കിൽ (yosys_tcl_interp != ശൂന്യ) {
		എങ്കിൽ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ശൂന്യ;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	വേണ്ടി (auto &it : loaded_plugins)
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

സ്ഥിരം std::string *create_id_prefix(std::string_view file, പൂർണ്ണസംഖ്യ വരി, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#അല്ലെങ്കിൽ
	size_t pos = file.find_last_of('/');
#endif
	എങ്കിൽ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	എങ്കിൽ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	തിരികെ പുതിയ std::string(stringf("$auto$%s:%d:%s$", file, വരി, func));
}

RTLIL::Design *yosys_get_design()
{
	തിരികെ yosys_design;
}

സ്ഥിരം അക്ഷരം *create_prompt(RTLIL::Design *design, പൂർണ്ണസംഖ്യ recursion_counter, തർക്കം newline)
{
	സ്ഥിതം അക്ഷരം buffer[100];
	std::string str = newline ? "\n" : "";
	എങ്കിൽ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	എങ്കിൽ (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	എങ്കിൽ (!design->full_selection()) {
		എങ്കിൽ (design->selected_active_module.empty())
			str += "*";
		അല്ലെങ്കിൽ എങ്കിൽ (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.എണ്ണം(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	തിരികെ buffer;
}

std::vector<std::string> glob_filename(സ്ഥിരം std::string &filename_pattern)
{
	std::vector<std::string> results;

#എങ്കിൽ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.തള്ളുക_back(filename_pattern);
#അല്ലെങ്കിൽ
	glob_t globbuf;

	പൂർണ്ണസംഖ്യ err = glob(filename_pattern.c_str(), 0, ശൂന്യ, &globbuf);

	എങ്കിൽ(err == 0) {
		വേണ്ടി (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.തള്ളുക_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} അല്ലെങ്കിൽ {
		results.തള്ളുക_back(filename_pattern);
	}
#endif

	തിരികെ results;
}

ശൂന്യം rewrite_filename(std::string &filename)
{
	എങ്കിൽ (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	എങ്കിൽ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	എങ്കിൽ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
ബാഹ്യ പൂർണ്ണസംഖ്യ yosys_tcl_interp_init(Tcl_Interp *interp);

ബാഹ്യ Tcl_Interp *yosys_get_tcl_interp()
{
	എങ്കിൽ (yosys_tcl_interp == ശൂന്യ) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	തിരികെ yosys_tcl_interp;
}

// Also see SdcPass
ഘടന TclPass : പൊതു Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	ശൂന്യം help() override {
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
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *) override {
		എങ്കിൽ (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		വേണ്ടി (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.തള്ളുക_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ശൂന്യ, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ശൂന്യ, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ശൂന്യ, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		എങ്കിൽ (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#എങ്കിൽ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	വേണ്ടി (പൂർണ്ണസംഖ്യ tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		എങ്കിൽ (buflen < (ssize_t)path.size())
			നിർത്തുക;
		അല്ലെങ്കിൽ
			path.resize(path.size() * 2);
	}
	എങ്കിൽ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} അല്ലെങ്കിൽ {
		ആകുമ്പോൾ (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	തിരികെ path;
}
#അല്ലെങ്കിലെങ്കിൽ defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	പൂർണ്ണസംഖ്യ mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#അല്ലെങ്കിൽ
	പൂർണ്ണസംഖ്യ mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	അക്ഷരം *buffer;
	std::string path;
	എങ്കിൽ (sysctl(mib, 4, ശൂന്യ, &buflen, ശൂന്യ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (അക്ഷരം*)malloc(buflen);
	എങ്കിൽ (buffer == ശൂന്യ)
		log_error("malloc failed: %s\n", strerror(errno));
	എങ്കിൽ (sysctl(mib, 4, buffer, &buflen, ശൂന്യ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ആകുമ്പോൾ (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	സ്വതന്ത്രം(buffer);
	തിരികെ path;
}
#അല്ലെങ്കിലെങ്കിൽ defined(__APPLE__)
std::string proc_self_dirname()
{
	അക്ഷരം *path = ശൂന്യ;
	uint32_t buflen = 0;
	ആകുമ്പോൾ (_NSGetExecutablePath(path, &buflen) != 0)
		path = (അക്ഷരം *) realloc((ശൂന്യം *) path, buflen);
	അക്ഷരം resolved[PATH_MAX];
	എങ്കിൽ (realpath(path, resolved) != ശൂന്യ) {
		സ്വതന്ത്രം(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	ആകുമ്പോൾ (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	സ്വതന്ത്രം(path);
	തിരികെ str;
}
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	എങ്കിൽ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), ശൂന്യ, 0, ശൂന്യ, ശൂന്യ));
	എങ്കിൽ (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), ശൂന്യ, ശൂന്യ) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	തിരികെ ubinpath;

}
#അല്ലെങ്കിലെങ്കിൽ defined(__wasm)
std::string proc_self_dirname()
{
	തിരികെ "/";
}
#അല്ലെങ്കിലെങ്കിൽ defined(__OpenBSD__) || defined(__HAIKU__)
അക്ഷരം yosys_path[PATH_MAX];
അക്ഷരം *yosys_argv0;

std::string proc_self_dirname(ശൂന്യം)
{
	അക്ഷരം buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	എങ്കിൽ (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, വലുപ്പം buf, "%*s/", (പൂർണ്ണസംഖ്യ)(yosys_path - p), yosys_path);
		തിരികെ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	എങ്കിൽ (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	വേണ്ടി (p = strtok(path, ":"); p; p = strtok(ശൂന്യ, ":")) {
		snprintf(buf, വലുപ്പം buf, "%s/%s", p, yosys_argv0);
		എങ്കിൽ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			സ്വതന്ത്രം(path);
			തിരികെ buf;
		}
	}
	സ്വതന്ത്രം(path);
	log_error("Can't determine yosys executable path.\n");
	തിരികെ ശൂന്യ;
}
#അല്ലെങ്കിൽ
	#error "Don't know how to determine process executable base path!"
#endif

#എങ്കിൽ defined(__wasm)
ശൂന്യം init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#അല്ലെങ്കിൽ
ശൂന്യം init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	എങ്കിൽ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		സ്ഥിരം അക്ഷരം *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		തിരികെ;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  എങ്കിൽ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	എങ്കിൽ (check_directory_exists(proc_share_path, സത്യം)) {
		yosys_share_dirname = proc_share_path;
		തിരികെ;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	എങ്കിൽ (check_directory_exists(proc_share_path, സത്യം)) {
		yosys_share_dirname = proc_share_path;
		തിരികെ;
	}
#  അല്ലെങ്കിൽ
	std::string proc_share_path = proc_self_path + "share/";
	എങ്കിൽ (check_directory_exists(proc_share_path, സത്യം)) {
		yosys_share_dirname = proc_share_path;
		തിരികെ;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	എങ്കിൽ (check_directory_exists(proc_share_path, സത്യം)) {
		yosys_share_dirname = proc_share_path;
		തിരികെ;
	}
	proc_share_path = YOSYS_DATDIR "/";
	എങ്കിൽ (check_directory_exists(proc_share_path, സത്യം)) {
		yosys_share_dirname = proc_share_path;
		തിരികെ;
	}
#  endif
}
#endif

ശൂന്യം init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	എങ്കിൽ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} അല്ലെങ്കിൽ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#അല്ലെങ്കിൽ
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	എങ്കിൽ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	എങ്കിൽ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		സ്ഥിരം അക്ഷരം *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	എങ്കിൽ (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	തിരികെ yosys_share_dirname;
}

std::string proc_program_prefix()
{
	തിരികെ YOSYS_PROGRAM_PREFIX;
}

തർക്കം fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	അക്ഷരം block[4096];
	ആകുമ്പോൾ (1) {
		എങ്കിൽ (fgets(block, 4096, f) == ശൂന്യ)
			തിരികെ അസത്യം;
		buffer += block;
		എങ്കിൽ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			ആകുമ്പോൾ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			തിരികെ സത്യം;
		}
	}
}

സ്ഥിതം ശൂന്യം handle_label(std::string &command, തർക്കം &from_to_active, സ്ഥിരം std::string &run_from, സ്ഥിരം std::string &run_to)
{
	പൂർണ്ണസംഖ്യ pos = 0;
	std::string label;

	ആകുമ്പോൾ (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	എങ്കിൽ (pos < GetSize(command) && command[pos] == '#')
		തിരികെ;

	ആകുമ്പോൾ (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	എങ്കിൽ (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		എങ്കിൽ (label == run_from)
			from_to_active = സത്യം;
		അല്ലെങ്കിൽ എങ്കിൽ (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = അസത്യം;
	}
}

തർക്കം run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	എങ്കിൽ (design == nullptr)
		design = yosys_design;

	എങ്കിൽ (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](സ്ഥിരം std::string& filename, സ്ഥിരം std::string& extension) {
	    എങ്കിൽ (filename.size() >= extension.size()) {
	      തിരികെ filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    തിരികെ അസത്യം;
	  };

	  എങ്കിൽ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  എങ്കിൽ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } അല്ലെങ്കിൽ എങ്കിൽ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } അല്ലെങ്കിൽ എങ്കിൽ (filename == "-") {
	    command = "script";
	  } അല്ലെങ്കിൽ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	എങ്കിൽ (command == "script")
	{
		std::string run_from, run_to;
		തർക്കം from_to_active = സത്യം;

		എങ്കിൽ (from_to_label != ശൂന്യ) {
			size_t pos = from_to_label->find(':');
			എങ്കിൽ (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} അല്ലെങ്കിൽ {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		എങ്കിൽ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_നിവേശം_files.തിരുകുക(filename);
		}

		എങ്കിൽ (f == ശൂന്യ)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		പൂർണ്ണസംഖ്യ backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		ശ്രമിക്കുക {
			std::string command;
			ആകുമ്പോൾ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ആകുമ്പോൾ (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_വരി;
					എങ്കിൽ (!fgetline(f, next_വരി))
						നിർത്തുക;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_വരി;
				}
				handle_label(command, from_to_active, run_from, run_to);
				എങ്കിൽ (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			എങ്കിൽ (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				എങ്കിൽ (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		പിടിക്കുക (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			എറിയുക;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		എങ്കിൽ (filename != "-")
			fclose(f);

		തിരികെ സത്യം;
	}

	എങ്കിൽ (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		തിരികെ സത്യം;
	}

	log("\n");
	എങ്കിൽ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} അല്ലെങ്കിൽ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	എങ്കിൽ (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.തള്ളുക_back(filename);
		Pass::call(design, argv);
	} അല്ലെങ്കിൽ
		Frontend::frontend_call(design, ശൂന്യ, filename, command);

	design->check();
	തിരികെ അസത്യം;
}

ശൂന്യം run_pass(std::string command, RTLIL::Design *design)
{
	എങ്കിൽ (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

ശൂന്യം run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	എങ്കിൽ (design == nullptr)
		design = yosys_design;

	എങ്കിൽ (command == "auto") {
		എങ്കിൽ (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		അല്ലെങ്കിൽ എങ്കിൽ (filename == "-")
			command = "rtlil";
		അല്ലെങ്കിൽ എങ്കിൽ (filename.empty())
			തിരികെ;
		അല്ലെങ്കിൽ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	എങ്കിൽ (filename.empty())
		filename = "-";

	log("\n");
	എങ്കിൽ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} അല്ലെങ്കിൽ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ശൂന്യ, filename, command);
}

#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
സ്ഥിതം അക്ഷരം *readline_cmd_generator(സ്ഥിരം അക്ഷരം *text, പൂർണ്ണസംഖ്യ state)
{
	സ്ഥിതം std::map<std::string, Pass*>::iterator it;
	സ്ഥിതം പൂർണ്ണസംഖ്യ len;

	എങ്കിൽ (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	വേണ്ടി (; it != pass_register.end(); it++) {
		എങ്കിൽ (it->first.compare(0, len, text) == 0)
			തിരികെ strdup((it++)->first.c_str());
	}
	തിരികെ ശൂന്യ;
}

സ്ഥിതം അക്ഷരം *readline_obj_generator(സ്ഥിരം അക്ഷരം *text, പൂർണ്ണസംഖ്യ state)
{
	സ്ഥിതം std::vector<അക്ഷരം*> obj_names;
	സ്ഥിതം size_t idx;

	എങ്കിൽ (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		പൂർണ്ണസംഖ്യ len = strlen(text);

		എങ്കിൽ (design->selected_active_module.empty())
		{
			വേണ്ടി (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				എങ്കിൽ (mod_name.compare(0, len, text) == 0)
					obj_names.തള്ളുക_back(strdup(mod_name.c_str()));
			}
		}
		അല്ലെങ്കിൽ എങ്കിൽ (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			വേണ്ടി (auto w : module->wires())
				എങ്കിൽ (w->name.unescape().compare(0, len, text) == 0)
					obj_names.തള്ളുക_back(strdup(w->name.unescape().c_str()));

			വേണ്ടി (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				എങ്കിൽ (mem_name.compare(0, len, text) == 0)
					obj_names.തള്ളുക_back(strdup(mem_name.c_str()));
			}

			വേണ്ടി (auto cell : module->cells())
				എങ്കിൽ (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.തള്ളുക_back(strdup(cell->name.unescape().c_str()));

			വേണ്ടി (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				എങ്കിൽ (proc_name.compare(0, len, text) == 0)
					obj_names.തള്ളുക_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	എങ്കിൽ (idx < obj_names.size())
		തിരികെ strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	തിരികെ ശൂന്യ;
}

സ്ഥിതം അക്ഷരം **readline_completion(സ്ഥിരം അക്ഷരം *text, പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ)
{
	എങ്കിൽ (start == 0)
		തിരികെ rl_completion_matches(text, readline_cmd_generator);
	എങ്കിൽ (strncmp(rl_വരി_buffer, "read_", 5) && strncmp(rl_വരി_buffer, "write_", 6))
		തിരികെ rl_completion_matches(text, readline_obj_generator);
	തിരികെ ശൂന്യ;
}
#endif

ശൂന്യം shell(RTLIL::Design *design)
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (അക്ഷരം*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (അക്ഷരം*)" \t\n";
#endif

	അക്ഷരം *command = ശൂന്യ;
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ആകുമ്പോൾ ((command = readline(create_prompt(design, recursion_counter))) != ശൂന്യ)
	{
#അല്ലെങ്കിൽ
	അക്ഷരം command_buffer[4096];
	ആകുമ്പോൾ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		എങ്കിൽ ((command = fgets(command_buffer, 4096, stdin)) == ശൂന്യ)
			നിർത്തുക;
#endif
		എങ്കിൽ (command[strspn(command, " \t\r\n")] == 0) {
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			സ്വതന്ത്രം(command);
#endif
			തുടരുക;
		}
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		അക്ഷരം *p = command + strspn(command, " \t\r\n");
		എങ്കിൽ (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			എങ്കിൽ (*p == 0)
				നിർത്തുക;
		}

		ശ്രമിക്കുക {
			log_assert(design->selection_അട്ടി.size() == 1);
			Pass::call(design, command);
		} പിടിക്കുക (log_cmd_error_exception) {
			ആകുമ്പോൾ (design->selection_അട്ടി.size() > 1)
				design->എടുക്കുക_selection();
			log_reset_അട്ടി();
		}
		design->check();
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		എങ്കിൽ (command)
			സ്വതന്ത്രം(command);
#endif
	}
	എങ്കിൽ (command == ശൂന്യ)
		printf("exit\n");
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	അല്ലെങ്കിൽ
		സ്വതന്ത്രം(command);
#endif
	recursion_counter--;
}

ഘടന ShellPass : പൊതു Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	ശൂന്യം help() override {
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
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, അസത്യം);
		shell(design);
	}
} ShellPass;

#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ഘടന HistoryPass : പൊതു Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	ശൂന്യം help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, അസത്യം);
#ifdef YOSYS_ENABLE_READLINE
		വേണ്ടി(HIST_ENTRY **list = history_list(); *list != ശൂന്യ; list++)
			log("%s\n", (*list)->വരി);
#അല്ലെങ്കിൽ
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->വരി);
#endif
	}
} HistoryPass;
#endif

ഘടന ScriptCmdPass : പൊതു Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	ശൂന്യം help() override {
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
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		തർക്കം scriptwire = അസത്യം;

		size_t argidx;
		വേണ്ടി (argidx = 1; argidx < args.size(); argidx++) {
			എങ്കിൽ (args[argidx] == "-scriptwire") {
				scriptwire = സത്യം;
				തുടരുക;
			}
			നിർത്തുക;
		}
		എങ്കിൽ (scriptwire) {
			extra_args(args, argidx, design);

			വേണ്ടി (auto mod : design->selected_modules())
				വേണ്ടി (auto &c : mod->connections()) {
					എങ്കിൽ (!c.first.is_wire())
						തുടരുക;
					auto w = c.first.as_wire();
					എങ്കിൽ (!mod->selected(w))
						തുടരുക;
					എങ്കിൽ (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		അല്ലെങ്കിൽ എങ്കിൽ (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		അല്ലെങ്കിൽ എങ്കിൽ (args.size() == 2)
			run_frontend(args[1], "script", design);
		അല്ലെങ്കിൽ എങ്കിൽ (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		അല്ലെങ്കിൽ
			extra_args(args, 2, design, അസത്യം);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
