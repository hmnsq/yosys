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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/newcelltypes.h"

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
नामाकाश py = pybind11;
#endif

#जइ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#नोचेद्यदि defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#अण्णहा
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  जइ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#जइ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#जइ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
बाह्य "C" PyObject* PyInit_libyosys();
बाह्य "C" PyObject* PyInit_pyosys();
#अन्तर्भाव <signal.h>
#endif

#अन्तर्भाव <limits.h>
#अन्तर्भाव <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
पूर्णाङ्क yosys_xtrace = 0;
तार्किक yosys_write_versions = सच्च;
ध्रुव वर्ण* yosys_maybe_version() {
	जइ (yosys_write_versions)
		प्रत्यागम yosys_version_str;
	अण्णहा
		प्रत्यागम "Yosys";
}

RTLIL::Design *yosys_design = रिक्त;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = रिक्त;
Tcl_Interp *yosys_sdc_interp = रिक्त;
#endif

std::set<std::string> yosys_निवेशः_files, yosys_output_files;

तार्किक memhasher_active = मिच्छ;
uint32_t memhasher_rng = 123456;
std::vector<शून्य*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

तार्किक Multithreading::active_ = मिच्छ;

शून्य init_share_dirname();
शून्य init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = सच्च;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = मिच्छ;
}

शून्य Autoidx::ensure_at_least(पूर्णाङ्क v) {
	log_assert(!Multithreading::active());
	मानम् = std::max(मानम्, v);
}

पूर्णाङ्क Autoidx::operator++(पूर्णाङ्क) {
	log_assert(!Multithreading::active());
	प्रत्यागम मानम्++;
}

शून्य memhasher_on()
{
#जइ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(रिक्त) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = सच्च;
}

शून्य memhasher_off()
{
	प्रति (auto p : memhasher_store)
		जइ (p) मुक्त(p);
	memhasher_store.clear();
	memhasher_active = मिच्छ;
}

शून्य memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	पूर्णाङ्क size, index = (memhasher_rng >> 4) & 0xffff;
	विकल्प (memhasher_rng & 7) {
		पक्ष 0: size =   16; भङ्ग;
		पक्ष 1: size =  256; भङ्ग;
		पक्ष 2: size = 1024; भङ्ग;
		पक्ष 3: size = 4096; भङ्ग;
		स्वभाव: size = 0;
	}
	जइ (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

शून्य yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#जइ defined(YOSYS_ENABLE_SPAWN)
पूर्णाङ्क run_command(ध्रुव std::string &command, std::function<शून्य(ध्रुव std::string&)> process_पङ्क्तिः)
{
	जइ (!process_पङ्क्तिः)
		प्रत्यागम system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	जइ (f == nullptr)
		प्रत्यागम -1;

	std::string पङ्क्तिः;
	वर्ण logbuf[128];
	जाव (सच्च) {
		जइ (fgets(logbuf, 128, f) != रिक्त) {
			पङ्क्तिः += logbuf;
			जइ (!पङ्क्तिः.empty() && पङ्क्तिः.back() == '\n')
				process_पङ्क्तिः(पङ्क्तिः), पङ्क्तिः.clear();
		} अण्णहा जइ (ferror(f) && errno == EINTR) {
			clearerr(f);
			अनुवर्त;
		} अण्णहा {
			भङ्ग;
		}
	}
	जइ (!पङ्क्तिः.empty())
		process_पङ्क्तिः(पङ्क्तिः);

	पूर्णाङ्क ret = pclose(f);
	जइ (ret < 0)
		प्रत्यागम -2;
#ifdef _WIN32
	प्रत्यागम ret;
#अण्णहा
	जइ (!WIFEXITED(ret))
		प्रत्यागम -1;
	प्रत्यागम WEXITSTATUS(ret);
#endif
}
#endif

तार्किक already_setup = मिच्छ;
तार्किक already_shutdown = मिच्छ;

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
	(शून्य)_;
	क्षिप py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

शून्य yosys_setup()
{
	जइ(already_setup)
		प्रत्यागम;
	already_setup = सच्च;
	already_shutdown = मिच्छ;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	जइ (!Py_IsInitialized()) {
		PyImport_AppendInittab((वर्ण*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((वर्ण*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((वर्ण*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		सङ्केत(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = नूतन RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_प्रक्षिप();
}

तार्किक yosys_already_setup()
{
	प्रत्यागम already_setup;
}

शून्य yosys_shutdown()
{
	जइ(already_shutdown)
		प्रत्यागम;
	already_setup = मिच्छ;
	already_shutdown = सच्च;
	log_निष्कासय();

	Pass::done_register();

	delete yosys_design;
	yosys_design = रिक्त;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	जइ (yosys_tcl_interp != रिक्त) {
		जइ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = रिक्त;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	प्रति (auto &it : loaded_plugins)
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

ध्रुव std::string *create_id_prefix(std::string_view file, पूर्णाङ्क पङ्क्तिः, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#अण्णहा
	size_t pos = file.find_last_of('/');
#endif
	जइ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	जइ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	प्रत्यागम नूतन std::string(stringf("$auto$%s:%d:%s$", file, पङ्क्तिः, func));
}

RTLIL::Design *yosys_get_design()
{
	प्रत्यागम yosys_design;
}

ध्रुव वर्ण *create_prompt(RTLIL::Design *design, पूर्णाङ्क recursion_counter, तार्किक newline)
{
	स्थिर वर्ण buffer[100];
	std::string str = newline ? "\n" : "";
	जइ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	जइ (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	जइ (!design->full_selection()) {
		जइ (design->selected_active_module.empty())
			str += "*";
		अण्णहा जइ (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.गणनम्(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	प्रत्यागम buffer;
}

std::vector<std::string> glob_filename(ध्रुव std::string &filename_pattern)
{
	std::vector<std::string> results;

#जइ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.प्रक्षिप_back(filename_pattern);
#अण्णहा
	glob_t globbuf;

	पूर्णाङ्क err = glob(filename_pattern.c_str(), 0, रिक्त, &globbuf);

	जइ(err == 0) {
		प्रति (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.प्रक्षिप_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} अण्णहा {
		results.प्रक्षिप_back(filename_pattern);
	}
#endif

	प्रत्यागम results;
}

शून्य rewrite_filename(std::string &filename)
{
	जइ (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	जइ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	जइ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
बाह्य पूर्णाङ्क yosys_tcl_interp_init(Tcl_Interp *interp);

बाह्य Tcl_Interp *yosys_get_tcl_interp()
{
	जइ (yosys_tcl_interp == रिक्त) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	प्रत्यागम yosys_tcl_interp;
}

// Also see SdcPass
संरचना TclPass : सार्वजनिक Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	शून्य help() override {
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
	शून्य execute(std::vector<std::string> args, RTLIL::Design *) override {
		जइ (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		प्रति (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.प्रक्षिप_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), रिक्त, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), रिक्त, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), रिक्त, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		जइ (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#जइ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	प्रति (पूर्णाङ्क tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		जइ (buflen < (ssize_t)path.size())
			भङ्ग;
		अण्णहा
			path.resize(path.size() * 2);
	}
	जइ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} अण्णहा {
		जाव (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	प्रत्यागम path;
}
#नोचेद्यदि defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	पूर्णाङ्क mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#अण्णहा
	पूर्णाङ्क mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	वर्ण *buffer;
	std::string path;
	जइ (sysctl(mib, 4, रिक्त, &buflen, रिक्त, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (वर्ण*)malloc(buflen);
	जइ (buffer == रिक्त)
		log_error("malloc failed: %s\n", strerror(errno));
	जइ (sysctl(mib, 4, buffer, &buflen, रिक्त, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	जाव (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	मुक्त(buffer);
	प्रत्यागम path;
}
#नोचेद्यदि defined(__APPLE__)
std::string proc_self_dirname()
{
	वर्ण *path = रिक्त;
	uint32_t buflen = 0;
	जाव (_NSGetExecutablePath(path, &buflen) != 0)
		path = (वर्ण *) realloc((शून्य *) path, buflen);
	वर्ण resolved[PATH_MAX];
	जइ (realpath(path, resolved) != रिक्त) {
		मुक्त(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	जाव (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	मुक्त(path);
	प्रत्यागम str;
}
#नोचेद्यदि defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	जइ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), रिक्त, 0, रिक्त, रिक्त));
	जइ (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), रिक्त, रिक्त) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	प्रत्यागम ubinpath;

}
#नोचेद्यदि defined(__wasm)
std::string proc_self_dirname()
{
	प्रत्यागम "/";
}
#नोचेद्यदि defined(__OpenBSD__) || defined(__HAIKU__)
वर्ण yosys_path[PATH_MAX];
वर्ण *yosys_argv0;

std::string proc_self_dirname(शून्य)
{
	वर्ण buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	जइ (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, परिमाण buf, "%*s/", (पूर्णाङ्क)(yosys_path - p), yosys_path);
		प्रत्यागम buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	जइ (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	प्रति (p = strtok(path, ":"); p; p = strtok(रिक्त, ":")) {
		snprintf(buf, परिमाण buf, "%s/%s", p, yosys_argv0);
		जइ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			मुक्त(path);
			प्रत्यागम buf;
		}
	}
	मुक्त(path);
	log_error("Can't determine yosys executable path.\n");
	प्रत्यागम रिक्त;
}
#अण्णहा
	#error "Don't know how to determine process executable base path!"
#endif

#जइ defined(__wasm)
शून्य init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#अण्णहा
शून्य init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	जइ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ध्रुव वर्ण *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		प्रत्यागम;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  जइ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	जइ (check_directory_exists(proc_share_path, सच्च)) {
		yosys_share_dirname = proc_share_path;
		प्रत्यागम;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	जइ (check_directory_exists(proc_share_path, सच्च)) {
		yosys_share_dirname = proc_share_path;
		प्रत्यागम;
	}
#  अण्णहा
	std::string proc_share_path = proc_self_path + "share/";
	जइ (check_directory_exists(proc_share_path, सच्च)) {
		yosys_share_dirname = proc_share_path;
		प्रत्यागम;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	जइ (check_directory_exists(proc_share_path, सच्च)) {
		yosys_share_dirname = proc_share_path;
		प्रत्यागम;
	}
	proc_share_path = YOSYS_DATDIR "/";
	जइ (check_directory_exists(proc_share_path, सच्च)) {
		yosys_share_dirname = proc_share_path;
		प्रत्यागम;
	}
#  endif
}
#endif

शून्य init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	जइ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} अण्णहा {
		yosys_abc_executable = ABCEXTERNAL;
	}
#अण्णहा
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	जइ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	जइ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ध्रुव वर्ण *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	जइ (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	प्रत्यागम yosys_share_dirname;
}

std::string proc_program_prefix()
{
	प्रत्यागम YOSYS_PROGRAM_PREFIX;
}

तार्किक fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	वर्ण block[4096];
	जाव (1) {
		जइ (fgets(block, 4096, f) == रिक्त)
			प्रत्यागम मिच्छ;
		buffer += block;
		जइ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			जाव (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			प्रत्यागम सच्च;
		}
	}
}

स्थिर शून्य handle_label(std::string &command, तार्किक &from_to_active, ध्रुव std::string &run_from, ध्रुव std::string &run_to)
{
	पूर्णाङ्क pos = 0;
	std::string label;

	जाव (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	जइ (pos < GetSize(command) && command[pos] == '#')
		प्रत्यागम;

	जाव (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	जइ (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		जइ (label == run_from)
			from_to_active = सच्च;
		अण्णहा जइ (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = मिच्छ;
	}
}

तार्किक run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	जइ (design == nullptr)
		design = yosys_design;

	जइ (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ध्रुव std::string& filename, ध्रुव std::string& extension) {
	    जइ (filename.size() >= extension.size()) {
	      प्रत्यागम filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    प्रत्यागम मिच्छ;
	  };

	  जइ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  जइ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } अण्णहा जइ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } अण्णहा जइ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } अण्णहा जइ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } अण्णहा जइ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } अण्णहा जइ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } अण्णहा जइ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } अण्णहा जइ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } अण्णहा जइ (filename == "-") {
	    command = "script";
	  } अण्णहा {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	जइ (command == "script")
	{
		std::string run_from, run_to;
		तार्किक from_to_active = सच्च;

		जइ (from_to_label != रिक्त) {
			size_t pos = from_to_label->find(':');
			जइ (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} अण्णहा {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		जइ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_निवेशः_files.निवेशय(filename);
		}

		जइ (f == रिक्त)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		पूर्णाङ्क backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		यत्न {
			std::string command;
			जाव (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				जाव (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_पङ्क्तिः;
					जइ (!fgetline(f, next_पङ्क्तिः))
						भङ्ग;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_पङ्क्तिः;
				}
				handle_label(command, from_to_active, run_from, run_to);
				जइ (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			जइ (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				जइ (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		गृहाण (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			क्षिप;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		जइ (filename != "-")
			fclose(f);

		प्रत्यागम सच्च;
	}

	जइ (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		प्रत्यागम सच्च;
	}

	log("\n");
	जइ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} अण्णहा {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	जइ (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.प्रक्षिप_back(filename);
		Pass::call(design, argv);
	} अण्णहा
		Frontend::frontend_call(design, रिक्त, filename, command);

	design->check();
	प्रत्यागम मिच्छ;
}

शून्य run_pass(std::string command, RTLIL::Design *design)
{
	जइ (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

शून्य run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	जइ (design == nullptr)
		design = yosys_design;

	जइ (command == "auto") {
		जइ (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		अण्णहा जइ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		अण्णहा जइ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		अण्णहा जइ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		अण्णहा जइ (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		अण्णहा जइ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		अण्णहा जइ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		अण्णहा जइ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		अण्णहा जइ (filename == "-")
			command = "rtlil";
		अण्णहा जइ (filename.empty())
			प्रत्यागम;
		अण्णहा
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	जइ (filename.empty())
		filename = "-";

	log("\n");
	जइ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} अण्णहा {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, रिक्त, filename, command);
}

#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
स्थिर वर्ण *readline_cmd_generator(ध्रुव वर्ण *text, पूर्णाङ्क state)
{
	स्थिर std::map<std::string, Pass*>::iterator it;
	स्थिर पूर्णाङ्क len;

	जइ (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	प्रति (; it != pass_register.end(); it++) {
		जइ (it->first.compare(0, len, text) == 0)
			प्रत्यागम strdup((it++)->first.c_str());
	}
	प्रत्यागम रिक्त;
}

स्थिर वर्ण *readline_obj_generator(ध्रुव वर्ण *text, पूर्णाङ्क state)
{
	स्थिर std::vector<वर्ण*> obj_names;
	स्थिर size_t idx;

	जइ (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		पूर्णाङ्क len = strlen(text);

		जइ (design->selected_active_module.empty())
		{
			प्रति (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				जइ (mod_name.compare(0, len, text) == 0)
					obj_names.प्रक्षिप_back(strdup(mod_name.c_str()));
			}
		}
		अण्णहा जइ (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			प्रति (auto w : module->wires())
				जइ (w->name.unescape().compare(0, len, text) == 0)
					obj_names.प्रक्षिप_back(strdup(w->name.unescape().c_str()));

			प्रति (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				जइ (mem_name.compare(0, len, text) == 0)
					obj_names.प्रक्षिप_back(strdup(mem_name.c_str()));
			}

			प्रति (auto cell : module->cells())
				जइ (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.प्रक्षिप_back(strdup(cell->name.unescape().c_str()));

			प्रति (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				जइ (proc_name.compare(0, len, text) == 0)
					obj_names.प्रक्षिप_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	जइ (idx < obj_names.size())
		प्रत्यागम strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	प्रत्यागम रिक्त;
}

स्थिर वर्ण **readline_completion(ध्रुव वर्ण *text, पूर्णाङ्क start, पूर्णाङ्क)
{
	जइ (start == 0)
		प्रत्यागम rl_completion_matches(text, readline_cmd_generator);
	जइ (strncmp(rl_पङ्क्तिः_buffer, "read_", 5) && strncmp(rl_पङ्क्तिः_buffer, "write_", 6))
		प्रत्यागम rl_completion_matches(text, readline_obj_generator);
	प्रत्यागम रिक्त;
}
#endif

शून्य shell(RTLIL::Design *design)
{
	स्थिर पूर्णाङ्क recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (वर्ण*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (वर्ण*)" \t\n";
#endif

	वर्ण *command = रिक्त;
#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	जाव ((command = readline(create_prompt(design, recursion_counter))) != रिक्त)
	{
#अण्णहा
	वर्ण command_buffer[4096];
	जाव (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		जइ ((command = fgets(command_buffer, 4096, stdin)) == रिक्त)
			भङ्ग;
#endif
		जइ (command[strspn(command, " \t\r\n")] == 0) {
#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			मुक्त(command);
#endif
			अनुवर्त;
		}
#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		वर्ण *p = command + strspn(command, " \t\r\n");
		जइ (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			जइ (*p == 0)
				भङ्ग;
		}

		यत्न {
			log_assert(design->selection_राशिः.size() == 1);
			Pass::call(design, command);
		} गृहाण (log_cmd_error_exception) {
			जाव (design->selection_राशिः.size() > 1)
				design->निष्कासय_selection();
			log_reset_राशिः();
		}
		design->check();
#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		जइ (command)
			मुक्त(command);
#endif
	}
	जइ (command == रिक्त)
		printf("exit\n");
#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	अण्णहा
		मुक्त(command);
#endif
	recursion_counter--;
}

संरचना ShellPass : सार्वजनिक Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	शून्य help() override {
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
	शून्य execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, मिच्छ);
		shell(design);
	}
} ShellPass;

#जइ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
संरचना HistoryPass : सार्वजनिक Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	शून्य help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	शून्य execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, मिच्छ);
#ifdef YOSYS_ENABLE_READLINE
		प्रति(HIST_ENTRY **list = history_list(); *list != रिक्त; list++)
			log("%s\n", (*list)->पङ्क्तिः);
#अण्णहा
		प्रति (पूर्णाङ्क i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->पङ्क्तिः);
#endif
	}
} HistoryPass;
#endif

संरचना ScriptCmdPass : सार्वजनिक Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	शून्य help() override {
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
	शून्य execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		तार्किक scriptwire = मिच्छ;

		size_t argidx;
		प्रति (argidx = 1; argidx < args.size(); argidx++) {
			जइ (args[argidx] == "-scriptwire") {
				scriptwire = सच्च;
				अनुवर्त;
			}
			भङ्ग;
		}
		जइ (scriptwire) {
			extra_args(args, argidx, design);

			प्रति (auto mod : design->selected_modules())
				प्रति (auto &c : mod->connections()) {
					जइ (!c.first.is_wire())
						अनुवर्त;
					auto w = c.first.as_wire();
					जइ (!mod->selected(w))
						अनुवर्त;
					जइ (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		अण्णहा जइ (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		अण्णहा जइ (args.size() == 2)
			run_frontend(args[1], "script", design);
		अण्णहा जइ (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		अण्णहा
			extra_args(args, 2, design, मिच्छ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
