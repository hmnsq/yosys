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

#जर defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#किंवाजर defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#अन्यथा
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  जर defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#जर !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#जर defined(__FreeBSD__) || defined(__NetBSD__)
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
पूर्णांक yosys_xtrace = 0;
तार्किक yosys_write_versions = सत्य;
स्थिरांक अक्षर* yosys_maybe_version() {
	जर (yosys_write_versions)
		परत yosys_version_str;
	अन्यथा
		परत "Yosys";
}

RTLIL::Design *yosys_design = रिक्त;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = रिक्त;
Tcl_Interp *yosys_sdc_interp = रिक्त;
#endif

std::set<std::string> yosys_आदान_files, yosys_output_files;

तार्किक memhasher_active = असत्य;
uint32_t memhasher_rng = 123456;
std::vector<शून्य*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

तार्किक Multithreading::active_ = असत्य;

शून्य init_share_dirname();
शून्य init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = सत्य;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = असत्य;
}

शून्य Autoidx::ensure_at_least(पूर्णांक v) {
	log_assert(!Multithreading::active());
	मूल्य = std::max(मूल्य, v);
}

पूर्णांक Autoidx::चालक++(पूर्णांक) {
	log_assert(!Multithreading::active());
	परत मूल्य++;
}

शून्य memhasher_on()
{
#जर defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += समय(रिक्त) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = सत्य;
}

शून्य memhasher_off()
{
	साठी (स्वतः p : memhasher_store)
		जर (p) मुक्त(p);
	memhasher_store.clear();
	memhasher_active = असत्य;
}

शून्य memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	पूर्णांक size, index = (memhasher_rng >> 4) & 0xffff;
	निवड (memhasher_rng & 7) {
		स्थिती 0: size =   16; थांबा;
		स्थिती 1: size =  256; थांबा;
		स्थिती 2: size = 1024; थांबा;
		स्थिती 3: size = 4096; थांबा;
		पूर्वनिर्धारित: size = 0;
	}
	जर (index < 16) size *= 16;
	memhasher_store[index] = पुनः_स्मृति(memhasher_store[index], size);
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

#जर defined(YOSYS_ENABLE_SPAWN)
पूर्णांक run_command(स्थिरांक std::string &command, std::function<शून्य(स्थिरांक std::string&)> process_ओळ)
{
	जर (!process_ओळ)
		परत प्रणाली(command.c_str());

	खाता *f = popen(command.c_str(), "r");
	जर (f == nullptr)
		परत -1;

	std::string ओळ;
	अक्षर logbuf[128];
	पर्यंत (सत्य) {
		जर (ख_माला_लो(logbuf, 128, f) != रिक्त) {
			ओळ += logbuf;
			जर (!ओळ.empty() && ओळ.back() == '\n')
				process_ओळ(ओळ), ओळ.clear();
		} अन्यथा जर (ख_त्रुटि(f) && त्रुटि_सं == EINTR) {
			साफ_त्रुटि(f);
			सुरूठेवा;
		} अन्यथा {
			थांबा;
		}
	}
	जर (!ओळ.empty())
		process_ओळ(ओळ);

	पूर्णांक ret = pclose(f);
	जर (ret < 0)
		परत -2;
#ifdef _WIN32
	परत ret;
#अन्यथा
	जर (!WIFEXITED(ret))
		परत -1;
	परत WEXITSTATUS(ret);
#endif
}
#endif

तार्किक already_setup = असत्य;
तार्किक already_shutdown = असत्य;

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
	फेका py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

शून्य yosys_setup()
{
	जर(already_setup)
		परत;
	already_setup = सत्य;
	already_shutdown = असत्य;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	जर (!Py_IsInitialized()) {
		PyImport_AppendInittab((अक्षर*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((अक्षर*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((अक्षर*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		संकेत(संक_विघ्न, संक_पूर्व);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = नवीन RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_ढकला();
}

तार्किक yosys_already_setup()
{
	परत already_setup;
}

शून्य yosys_shutdown()
{
	जर(already_shutdown)
		परत;
	already_setup = असत्य;
	already_shutdown = सत्य;
	log_काढा();

	Pass::done_register();

	delete yosys_design;
	yosys_design = रिक्त;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	जर (yosys_tcl_interp != रिक्त) {
		जर (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = रिक्त;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	साठी (स्वतः &it : loaded_plugins)
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

स्थिरांक std::string *create_id_prefix(std::string_view file, पूर्णांक ओळ, std::string_view func)
{
#ifdef _WIN32
	माप_प्रकार pos = file.find_last_of("/\\");
#अन्यथा
	माप_प्रकार pos = file.find_last_of('/');
#endif
	जर (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	जर (pos != std::string_view::npos)
		func = func.substr(pos+1);

	परत नवीन std::string(stringf("$auto$%s:%d:%s$", file, ओळ, func));
}

RTLIL::Design *yosys_get_design()
{
	परत yosys_design;
}

स्थिरांक अक्षर *create_prompt(RTLIL::Design *design, पूर्णांक recursion_counter, तार्किक newline)
{
	स्थिर अक्षर buffer[100];
	std::string str = newline ? "\n" : "";
	जर (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	जर (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	जर (!design->full_selection()) {
		जर (design->selected_active_module.empty())
			str += "*";
		अन्यथा जर (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.मोजणी(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	परत buffer;
}

std::vector<std::string> glob_filename(स्थिरांक std::string &filename_pattern)
{
	std::vector<std::string> results;

#जर defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ढकला_back(filename_pattern);
#अन्यथा
	glob_t globbuf;

	पूर्णांक err = glob(filename_pattern.c_str(), 0, रिक्त, &globbuf);

	जर(err == 0) {
		साठी (माप_प्रकार i = 0; i < globbuf.gl_pathc; i++)
			results.ढकला_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} अन्यथा {
		results.ढकला_back(filename_pattern);
	}
#endif

	परत results;
}

शून्य rewrite_filename(std::string &filename)
{
	जर (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	जर (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	जर (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, दो_पर्या("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
बाह्य पूर्णांक yosys_tcl_interp_init(Tcl_Interp *interp);

बाह्य Tcl_Interp *yosys_get_tcl_interp()
{
	जर (yosys_tcl_interp == रिक्त) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	परत yosys_tcl_interp;
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
		जर (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		साठी (स्वतः it = args.begin() + 2; it != args.end(); ++it)
			script_args.ढकला_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), रिक्त, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), रिक्त, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), रिक्त, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		जर (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#जर defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	साठी (पूर्णांक tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		जर (buflen < (ssize_t)path.size())
			थांबा;
		अन्यथा
			path.resize(path.size() * 2);
	}
	जर (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", म_त्रुटि(त्रुटि_सं));
		path.resize(0);
	} अन्यथा {
		पर्यंत (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	परत path;
}
#किंवाजर defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	पूर्णांक mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#अन्यथा
	पूर्णांक mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	माप_प्रकार buflen;
	अक्षर *buffer;
	std::string path;
	जर (sysctl(mib, 4, रिक्त, &buflen, रिक्त, 0) != 0)
		log_error("sysctl failed: %s\n", म_त्रुटि(त्रुटि_सं));
	buffer = (अक्षर*)दो_स्मृति(buflen);
	जर (buffer == रिक्त)
		log_error("malloc failed: %s\n", म_त्रुटि(त्रुटि_सं));
	जर (sysctl(mib, 4, buffer, &buflen, रिक्त, 0) != 0)
		log_error("sysctl failed: %s\n", म_त्रुटि(त्रुटि_सं));
	पर्यंत (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	मुक्त(buffer);
	परत path;
}
#किंवाजर defined(__APPLE__)
std::string proc_self_dirname()
{
	अक्षर *path = रिक्त;
	uint32_t buflen = 0;
	पर्यंत (_NSGetExecutablePath(path, &buflen) != 0)
		path = (अक्षर *) पुनः_स्मृति((शून्य *) path, buflen);
	अक्षर resolved[PATH_MAX];
	जर (realpath(path, resolved) != रिक्त) {
		मुक्त(path);
		path = strdup(resolved);
		buflen = म_माप(path);
	}
	पर्यंत (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	मुक्त(path);
	परत str;
}
#किंवाजर defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	जर (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	ख_लिखो(मानक_त्रुटि, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), रिक्त, 0, रिक्त, रिक्त));
	जर (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), रिक्त, रिक्त) == 0)
	ख_लिखो(मानक_त्रुटि, "WideCharToMultiByte() failed.\n");
	परत ubinpath;

}
#किंवाजर defined(__wasm)
std::string proc_self_dirname()
{
	परत "/";
}
#किंवाजर defined(__OpenBSD__) || defined(__HAIKU__)
अक्षर yosys_path[PATH_MAX];
अक्षर *yosys_argv0;

std::string proc_self_dirname(शून्य)
{
	अक्षर buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	जर (म_माप(yosys_path) > 0) {
		p = म_खोजप(yosys_path, '/');
		snprintf(buf, आकार buf, "%*s/", (पूर्णांक)(yosys_path - p), yosys_path);
		परत buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(दो_पर्या("PATH"));
	जर (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  म_त्रुटि(त्रुटि_सं));
	साठी (p = म_मोहर(path, ":"); p; p = म_मोहर(रिक्त, ":")) {
		snprintf(buf, आकार buf, "%s/%s", p, yosys_argv0);
		जर (access(buf, X_OK) == 0) {
			*(म_खोजप(buf, '/') + 1) = '\0';
			मुक्त(path);
			परत buf;
		}
	}
	मुक्त(path);
	log_error("Can't determine yosys executable path.\n");
	परत रिक्त;
}
#अन्यथा
	#error "Don't know how to determine process executable base path!"
#endif

#जर defined(__wasm)
शून्य init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#अन्यथा
शून्य init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	जर (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		स्थिरांक अक्षर *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		परत;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  जर defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	जर (check_directory_exists(proc_share_path, सत्य)) {
		yosys_share_dirname = proc_share_path;
		परत;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	जर (check_directory_exists(proc_share_path, सत्य)) {
		yosys_share_dirname = proc_share_path;
		परत;
	}
#  अन्यथा
	std::string proc_share_path = proc_self_path + "share/";
	जर (check_directory_exists(proc_share_path, सत्य)) {
		yosys_share_dirname = proc_share_path;
		परत;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	जर (check_directory_exists(proc_share_path, सत्य)) {
		yosys_share_dirname = proc_share_path;
		परत;
	}
	proc_share_path = YOSYS_DATDIR "/";
	जर (check_directory_exists(proc_share_path, सत्य)) {
		yosys_share_dirname = proc_share_path;
		परत;
	}
#  endif
}
#endif

शून्य init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	जर (std::दो_पर्या("ABC")) {
		yosys_abc_executable = std::दो_पर्या("ABC");
	} अन्यथा {
		yosys_abc_executable = ABCEXTERNAL;
	}
#अन्यथा
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	जर (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	जर (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		स्थिरांक अक्षर *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	जर (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	परत yosys_share_dirname;
}

std::string proc_program_prefix()
{
	परत YOSYS_PROGRAM_PREFIX;
}

तार्किक fgetline(खाता *f, std::string &buffer)
{
	buffer = "";
	अक्षर block[4096];
	पर्यंत (1) {
		जर (ख_माला_लो(block, 4096, f) == रिक्त)
			परत असत्य;
		buffer += block;
		जर (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			पर्यंत (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			परत सत्य;
		}
	}
}

स्थिर शून्य handle_label(std::string &command, तार्किक &from_to_active, स्थिरांक std::string &run_from, स्थिरांक std::string &run_to)
{
	पूर्णांक pos = 0;
	std::string label;

	पर्यंत (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	जर (pos < GetSize(command) && command[pos] == '#')
		परत;

	पर्यंत (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	जर (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		जर (label == run_from)
			from_to_active = सत्य;
		अन्यथा जर (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = असत्य;
	}
}

तार्किक run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	जर (design == nullptr)
		design = yosys_design;

	जर (command == "auto") {
	  std::string filename_trim = filename;

	  स्वतः has_extension = [](स्थिरांक std::string& filename, स्थिरांक std::string& extension) {
	    जर (filename.size() >= extension.size()) {
	      परत filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    परत असत्य;
	  };

	  जर (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  जर (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } अन्यथा जर (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } अन्यथा जर (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } अन्यथा जर (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } अन्यथा जर (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } अन्यथा जर (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } अन्यथा जर (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } अन्यथा जर (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } अन्यथा जर (filename == "-") {
	    command = "script";
	  } अन्यथा {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	जर (command == "script")
	{
		std::string run_from, run_to;
		तार्किक from_to_active = सत्य;

		जर (from_to_label != रिक्त) {
			माप_प्रकार pos = from_to_label->find(':');
			जर (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} अन्यथा {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		खाता *f = मानक_निवेश;

		जर (filename != "-") {
			f = ख_खोलो(filename.c_str(), "r");
			yosys_आदान_files.घाला(filename);
		}

		जर (f == रिक्त)
			log_error("Can't open script file `%s' for reading: %s\n", filename, म_त्रुटि(त्रुटि_सं));

		खाता *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		पूर्णांक backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		प्रयत्न {
			std::string command;
			पर्यंत (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				पर्यंत (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_ओळ;
					जर (!fgetline(f, next_ओळ))
						थांबा;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_ओळ;
				}
				handle_label(command, from_to_active, run_from, run_to);
				जर (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			जर (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				जर (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		पकडा (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			फेका;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		जर (filename != "-")
			ख_बंद(f);

		परत सत्य;
	}

	जर (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		परत सत्य;
	}

	log("\n");
	जर (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} अन्यथा {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	जर (command[0] == ' ') {
		स्वतः argv = split_tokens("read" + command);
		argv.ढकला_back(filename);
		Pass::call(design, argv);
	} अन्यथा
		Frontend::frontend_call(design, रिक्त, filename, command);

	design->check();
	परत असत्य;
}

शून्य run_pass(std::string command, RTLIL::Design *design)
{
	जर (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

शून्य run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	जर (design == nullptr)
		design = yosys_design;

	जर (command == "auto") {
		जर (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		अन्यथा जर (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		अन्यथा जर (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		अन्यथा जर (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		अन्यथा जर (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		अन्यथा जर (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		अन्यथा जर (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		अन्यथा जर (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		अन्यथा जर (filename == "-")
			command = "rtlil";
		अन्यथा जर (filename.empty())
			परत;
		अन्यथा
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	जर (filename.empty())
		filename = "-";

	log("\n");
	जर (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} अन्यथा {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, रिक्त, filename, command);
}

#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
स्थिर अक्षर *readline_cmd_generator(स्थिरांक अक्षर *text, पूर्णांक state)
{
	स्थिर std::map<std::string, Pass*>::iterator it;
	स्थिर पूर्णांक len;

	जर (!state) {
		it = pass_register.begin();
		len = म_माप(text);
	}

	साठी (; it != pass_register.end(); it++) {
		जर (it->first.compare(0, len, text) == 0)
			परत strdup((it++)->first.c_str());
	}
	परत रिक्त;
}

स्थिर अक्षर *readline_obj_generator(स्थिरांक अक्षर *text, पूर्णांक state)
{
	स्थिर std::vector<अक्षर*> obj_names;
	स्थिर माप_प्रकार idx;

	जर (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		पूर्णांक len = म_माप(text);

		जर (design->selected_active_module.empty())
		{
			साठी (स्वतः mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				जर (mod_name.compare(0, len, text) == 0)
					obj_names.ढकला_back(strdup(mod_name.c_str()));
			}
		}
		अन्यथा जर (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			साठी (स्वतः w : module->wires())
				जर (w->name.unescape().compare(0, len, text) == 0)
					obj_names.ढकला_back(strdup(w->name.unescape().c_str()));

			साठी (स्वतः &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				जर (mem_name.compare(0, len, text) == 0)
					obj_names.ढकला_back(strdup(mem_name.c_str()));
			}

			साठी (स्वतः cell : module->cells())
				जर (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.ढकला_back(strdup(cell->name.unescape().c_str()));

			साठी (स्वतः &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				जर (proc_name.compare(0, len, text) == 0)
					obj_names.ढकला_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	जर (idx < obj_names.size())
		परत strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	परत रिक्त;
}

स्थिर अक्षर **readline_completion(स्थिरांक अक्षर *text, पूर्णांक start, पूर्णांक)
{
	जर (start == 0)
		परत rl_completion_matches(text, readline_cmd_generator);
	जर (म_भेदन(rl_ओळ_buffer, "read_", 5) && म_भेदन(rl_ओळ_buffer, "write_", 6))
		परत rl_completion_matches(text, readline_obj_generator);
	परत रिक्त;
}
#endif

शून्य shell(RTLIL::Design *design)
{
	स्थिर पूर्णांक recursion_counter = 0;

	recursion_counter++;
	स्वतः guard = logger().error_throw_scope();

#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (अक्षर*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (अक्षर*)" \t\n";
#endif

	अक्षर *command = रिक्त;
#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	पर्यंत ((command = readline(create_prompt(design, recursion_counter))) != रिक्त)
	{
#अन्यथा
	अक्षर command_buffer[4096];
	पर्यंत (1)
	{
		ख_माला_दो(create_prompt(design, recursion_counter), मानक_निकास);
		ख_साफ(मानक_निकास);
		जर ((command = ख_माला_लो(command_buffer, 4096, मानक_निवेश)) == रिक्त)
			थांबा;
#endif
		जर (command[म_अखोज(command, " \t\r\n")] == 0) {
#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			मुक्त(command);
#endif
			सुरूठेवा;
		}
#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		अक्षर *p = command + म_अखोज(command, " \t\r\n");
		जर (!म_भेदन(p, "exit", 4)) {
			p += 4;
			p += म_अखोज(p, " \t\r\n");
			जर (*p == 0)
				थांबा;
		}

		प्रयत्न {
			log_assert(design->selection_चळत.size() == 1);
			Pass::call(design, command);
		} पकडा (log_cmd_error_exception) {
			पर्यंत (design->selection_चळत.size() > 1)
				design->काढा_selection();
			log_reset_चळत();
		}
		design->check();
#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		जर (command)
			मुक्त(command);
#endif
	}
	जर (command == रिक्त)
		म_लिखो("exit\n");
#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	अन्यथा
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
		extra_args(args, 1, design, असत्य);
		shell(design);
	}
} ShellPass;

#जर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
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
		extra_args(args, 1, design, असत्य);
#ifdef YOSYS_ENABLE_READLINE
		साठी(HIST_ENTRY **list = history_list(); *list != रिक्त; list++)
			log("%s\n", (*list)->ओळ);
#अन्यथा
		साठी (पूर्णांक i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->ओळ);
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
		तार्किक scriptwire = असत्य;

		माप_प्रकार argidx;
		साठी (argidx = 1; argidx < args.size(); argidx++) {
			जर (args[argidx] == "-scriptwire") {
				scriptwire = सत्य;
				सुरूठेवा;
			}
			थांबा;
		}
		जर (scriptwire) {
			extra_args(args, argidx, design);

			साठी (स्वतः mod : design->selected_modules())
				साठी (स्वतः &c : mod->connections()) {
					जर (!c.first.is_wire())
						सुरूठेवा;
					स्वतः w = c.first.as_wire();
					जर (!mod->selected(w))
						सुरूठेवा;
					जर (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					स्वतः v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		अन्यथा जर (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		अन्यथा जर (args.size() == 2)
			run_frontend(args[1], "script", design);
		अन्यथा जर (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		अन्यथा
			extra_args(args, 2, design, असत्य);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
