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

#समावेश "kernel/yosys.h"
#समावेश "kernel/log.h"
#समावेश "kernel/newcelltypes.h"

#अगर_घोषित YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_TCL
#  include <tcl.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
namespace py = pybind11;
#पूर्ण_अगर

#अगर defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#या_अगर defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#अन्यथा
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  अगर defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#पूर्ण_अगर

#अगर !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#पूर्ण_अगर

#अगर defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_PYTHON
बाह्य "C" PyObject* PyInit_libyosys();
बाह्य "C" PyObject* PyInit_pyosys();
#समावेश <signal.h>
#पूर्ण_अगर

#समावेश <limits.h>
#समावेश <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
पूर्णांक yosys_xtrace = 0;
bool yosys_write_versions = true;
स्थिर अक्षर* yosys_maybe_version() {
	अगर (yosys_write_versions)
		वापस yosys_version_str;
	अन्यथा
		वापस "Yosys";
}

RTLIL::Design *yosys_design = शून्य;
NewCellTypes yosys_celltypes;

#अगर_घोषित YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = शून्य;
Tcl_Interp *yosys_sdc_interp = शून्य;
#पूर्ण_अगर

std::set<std::string> yosys_निवेश_files, yosys_output_files;

bool memhasher_active = false;
uint32_t memhasher_rng = 123456;
std::vector<व्योम*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

bool Multithreading::active_ = false;

व्योम init_share_dirname();
व्योम init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = true;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = false;
}

व्योम Autoidx::ensure_at_least(पूर्णांक v) {
	log_assert(!Multithreading::active());
	मूल्य = std::max(मूल्य, v);
}

पूर्णांक Autoidx::चालक++(पूर्णांक) {
	log_assert(!Multithreading::active());
	वापस मूल्य++;
}

व्योम memhasher_on()
{
#अगर defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += समय(शून्य) << 16 ^ getpid();
#पूर्ण_अगर
	memhasher_store.resize(0x10000);
	memhasher_active = true;
}

व्योम memhasher_off()
{
	क्रम (स्वतः p : memhasher_store)
		अगर (p) मुक्त(p);
	memhasher_store.सफा();
	memhasher_active = false;
}

व्योम memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	पूर्णांक आकार, सूचकांक = (memhasher_rng >> 4) & 0xffff;
	चयन (memhasher_rng & 7) {
		हाल 0: आकार =   16; अवरोध;
		हाल 1: आकार =  256; अवरोध;
		हाल 2: आकार = 1024; अवरोध;
		हाल 3: आकार = 4096; अवरोध;
		शेष: आकार = 0;
	}
	अगर (सूचकांक < 16) आकार *= 16;
	memhasher_store[सूचकांक] = पुनः_स्मृति(memhasher_store[सूचकांक], आकार);
}

व्योम yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#अगर defined(YOSYS_ENABLE_SPAWN)
पूर्णांक run_command(स्थिर std::string &command, std::function<व्योम(स्थिर std::string&)> process_पंक्ति)
{
	अगर (!process_पंक्ति)
		वापस प्रणाली(command.c_str());

	खाता *f = popen(command.c_str(), "r");
	अगर (f == nullptr)
		वापस -1;

	std::string पंक्ति;
	अक्षर logbuf[128];
	जबतक (true) {
		अगर (ख_माला_लो(logbuf, 128, f) != शून्य) {
			पंक्ति += logbuf;
			अगर (!पंक्ति.empty() && पंक्ति.back() == '\n')
				process_पंक्ति(पंक्ति), पंक्ति.सफा();
		} अन्यथा अगर (ख_त्रुटि(f) && त्रुटि_सं == EINTR) {
			साफ_त्रुटि(f);
			जारी;
		} अन्यथा {
			अवरोध;
		}
	}
	अगर (!पंक्ति.empty())
		process_पंक्ति(पंक्ति);

	पूर्णांक ret = pclose(f);
	अगर (ret < 0)
		वापस -2;
#अगर_घोषित _WIN32
	वापस ret;
#अन्यथा
	अगर (!WIFEXITED(ret))
		वापस -1;
	वापस WEXITSTATUS(ret);
#पूर्ण_अगर
}
#पूर्ण_अगर

bool already_setup = false;
bool already_shutdown = false;

#अगर_घोषित YOSYS_ENABLE_PYTHON
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
	(व्योम)_;
	फेंको py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#पूर्ण_अगर

व्योम yosys_setup()
{
	अगर(already_setup)
		वापस;
	already_setup = true;
	already_shutdown = false;

	StaticTwines::init();

#अगर_घोषित YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	अगर (!Py_IsInitialized()) {
		PyImport_AppendInittab((अक्षर*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((अक्षर*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((अक्षर*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		संकेत(संक_विघ्न, संक_पूर्व);
	}
#पूर्ण_अगर

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = नया RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_धकेलो();
}

bool yosys_already_setup()
{
	वापस already_setup;
}

व्योम yosys_shutdown()
{
	अगर(already_shutdown)
		वापस;
	already_setup = false;
	already_shutdown = true;
	log_निकालो();

	Pass::done_register();

	मिटाओ yosys_design;
	yosys_design = शून्य;

	logger().सफा();

#अगर_घोषित YOSYS_ENABLE_TCL
	अगर (yosys_tcl_interp != शून्य) {
		अगर (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = शून्य;
	}
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_PLUGINS
	क्रम (स्वतः &it : loaded_plugins)
		dlclose(it.second);

	loaded_plugins.सफा();
#अगर_घोषित YOSYS_ENABLE_PYTHON
	loaded_python_plugins.सफा();
#पूर्ण_अगर
	loaded_plugin_aliases.सफा();
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_PYTHON
	Py_Finalize();
#पूर्ण_अगर
}

स्थिर std::string *create_id_prefix(std::string_view file, पूर्णांक पंक्ति, std::string_view func)
{
#अगर_घोषित _WIN32
	माप_प्रकार pos = file.find_last_of("/\\");
#अन्यथा
	माप_प्रकार pos = file.find_last_of('/');
#पूर्ण_अगर
	अगर (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	अगर (pos != std::string_view::npos)
		func = func.substr(pos+1);

	वापस नया std::string(stringf("$auto$%s:%d:%s$", file, पंक्ति, func));
}

RTLIL::Design *yosys_get_design()
{
	वापस yosys_design;
}

स्थिर अक्षर *create_prompt(RTLIL::Design *design, पूर्णांक recursion_गणक, bool newline)
{
	अटल अक्षर बफर[100];
	std::string str = newline ? "\n" : "";
	अगर (recursion_गणक > 1)
		str += stringf("(%d) ", recursion_गणक);
	str += "yosys";
	अगर (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	अगर (!design->full_selection()) {
		अगर (design->selected_active_module.empty())
			str += "*";
		अन्यथा अगर (design->selection().selected_modules.आकार() != 1 || design->selection().selected_members.आकार() != 0 ||
				design->selection().selected_modules.गिनती(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(बफर, 100, "%s> ", str.c_str());
	वापस बफर;
}

std::vector<std::string> glob_filename(स्थिर std::string &filename_pattern)
{
	std::vector<std::string> results;

#अगर defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.धकेलो_back(filename_pattern);
#अन्यथा
	glob_t globbuf;

	पूर्णांक err = glob(filename_pattern.c_str(), 0, शून्य, &globbuf);

	अगर(err == 0) {
		क्रम (माप_प्रकार i = 0; i < globbuf.gl_pathc; i++)
			results.धकेलो_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} अन्यथा {
		results.धकेलो_back(filename_pattern);
	}
#पूर्ण_अगर

	वापस results;
}

व्योम rewrite_filename(std::string &filename)
{
	अगर (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_आकार(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_आकार(filename)-2);
	अगर (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#अगर_अघोषित _WIN32
	अगर (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, दो_पर्या("HOME"));
#पूर्ण_अगर
}

#अगर_घोषित YOSYS_ENABLE_TCL

// defined in tclapi.cc
बाह्य पूर्णांक yosys_tcl_interp_init(Tcl_Interp *interp);

बाह्य Tcl_Interp *yosys_get_tcl_interp()
{
	अगर (yosys_tcl_interp == शून्य) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	वापस yosys_tcl_interp;
}

// Also see SdcPass
काष्ठा TclPass : खुला Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	व्योम help() override {
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
	व्योम execute(std::vector<std::string> args, RTLIL::Design *) override {
		अगर (args.आकार() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		क्रम (स्वतः it = args.begin() + 2; it != args.end(); ++it)
			script_args.धकेलो_back(Tcl_NewStringObj((*it).c_str(), (*it).आकार()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), शून्य, Tcl_NewIntObj(script_args.आकार()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), शून्य, Tcl_NewListObj(script_args.आकार(), script_args.तथ्य()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), शून्य, Tcl_NewStringObj(args[1].c_str(), args[1].आकार()), 0);
		अगर (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_परिणाम(interp));
		Tcl_Release(interp);
	}
} TclPass;

#पूर्ण_अगर

#अगर defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	क्रम (पूर्णांक tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.तथ्य(), path.आकार());
		अगर (buflen < (ssize_t)path.आकार())
			अवरोध;
		अन्यथा
			path.resize(path.आकार() * 2);
	}
	अगर (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", म_त्रुटि(त्रुटि_सं));
		path.resize(0);
	} अन्यथा {
		जबतक (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	वापस path;
}
#या_अगर defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#अगर_घोषित __NetBSD__
	पूर्णांक mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#अन्यथा
	पूर्णांक mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#पूर्ण_अगर
	माप_प्रकार buflen;
	अक्षर *बफर;
	std::string path;
	अगर (sysctl(mib, 4, शून्य, &buflen, शून्य, 0) != 0)
		log_error("sysctl failed: %s\n", म_त्रुटि(त्रुटि_सं));
	बफर = (अक्षर*)दो_स्मृति(buflen);
	अगर (बफर == शून्य)
		log_error("malloc failed: %s\n", म_त्रुटि(त्रुटि_सं));
	अगर (sysctl(mib, 4, बफर, &buflen, शून्य, 0) != 0)
		log_error("sysctl failed: %s\n", म_त्रुटि(त्रुटि_सं));
	जबतक (buflen > 0 && बफर[buflen-1] != '/')
		buflen--;
	path.assign(बफर, buflen);
	मुक्त(बफर);
	वापस path;
}
#या_अगर defined(__APPLE__)
std::string proc_self_dirname()
{
	अक्षर *path = शून्य;
	uint32_t buflen = 0;
	जबतक (_NSGetExecutablePath(path, &buflen) != 0)
		path = (अक्षर *) पुनः_स्मृति((व्योम *) path, buflen);
	अक्षर resolved[PATH_MAX];
	अगर (realpath(path, resolved) != शून्य) {
		मुक्त(path);
		path = strdup(resolved);
		buflen = म_माप(path);
	}
	जबतक (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	मुक्त(path);
	वापस str;
}
#या_अगर defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	अगर (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.आकार()))
	ख_लिखो(मानक_त्रुटि, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.तथ्य(), wbinpath.आकार(), शून्य, 0, शून्य, शून्य));
	अगर (WideCharToMultiByte(CP_UTF8, 0, wbinpath.तथ्य(), wbinpath.आकार(), &ubinpath[0], ubinpath.आकार(), शून्य, शून्य) == 0)
	ख_लिखो(मानक_त्रुटि, "WideCharToMultiByte() failed.\n");
	वापस ubinpath;

}
#या_अगर defined(__wasm)
std::string proc_self_dirname()
{
	वापस "/";
}
#या_अगर defined(__OpenBSD__) || defined(__HAIKU__)
अक्षर yosys_path[PATH_MAX];
अक्षर *yosys_argv0;

std::string proc_self_dirname(व्योम)
{
	अक्षर buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	अगर (म_माप(yosys_path) > 0) {
		p = म_खोजप(yosys_path, '/');
		snprintf(buf, माप buf, "%*s/", (पूर्णांक)(yosys_path - p), yosys_path);
		वापस buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(दो_पर्या("PATH"));
	अगर (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  म_त्रुटि(त्रुटि_सं));
	क्रम (p = म_मोहर(path, ":"); p; p = म_मोहर(शून्य, ":")) {
		snprintf(buf, माप buf, "%s/%s", p, yosys_argv0);
		अगर (access(buf, X_OK) == 0) {
			*(म_खोजप(buf, '/') + 1) = '\0';
			मुक्त(path);
			वापस buf;
		}
	}
	मुक्त(path);
	log_error("Can't determine yosys executable path.\n");
	वापस शून्य;
}
#अन्यथा
	#त्रुटि "Don't know how to determine process executable base path!"
#पूर्ण_अगर

#अगर defined(__wasm)
व्योम init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#अन्यथा
व्योम init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	अगर (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		स्थिर अक्षर *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		वापस;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  अगर defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	अगर (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		वापस;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	अगर (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		वापस;
	}
#  अन्यथा
	std::string proc_share_path = proc_self_path + "share/";
	अगर (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		वापस;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	अगर (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		वापस;
	}
	proc_share_path = YOSYS_DATDIR "/";
	अगर (check_directory_exists(proc_share_path, true)) {
		yosys_share_dirname = proc_share_path;
		वापस;
	}
#  endif
}
#पूर्ण_अगर

व्योम init_abc_executable_name()
{
#अगर_घोषित ABCEXTERNAL
	std::string exe_file;
	अगर (std::दो_पर्या("ABC")) {
		yosys_abc_executable = std::दो_पर्या("ABC");
	} अन्यथा {
		yosys_abc_executable = ABCEXTERNAL;
	}
#अन्यथा
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	अगर (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	अगर (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		स्थिर अक्षर *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#पूर्ण_अगर
}

std::string proc_share_dirname()
{
	अगर (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	वापस yosys_share_dirname;
}

std::string proc_program_prefix()
{
	वापस YOSYS_PROGRAM_PREFIX;
}

bool fgetline(खाता *f, std::string &बफर)
{
	बफर = "";
	अक्षर block[4096];
	जबतक (1) {
		अगर (ख_माला_लो(block, 4096, f) == शून्य)
			वापस false;
		बफर += block;
		अगर (बफर.आकार() > 0 && (बफर[बफर.आकार()-1] == '\n' ||  बफर[बफर.आकार()-1] == '\r')) {
			जबतक (बफर.आकार() > 0 && (बफर[बफर.आकार()-1] == '\n' ||  बफर[बफर.आकार()-1] == '\r'))
				बफर.resize(बफर.आकार()-1);
			वापस true;
		}
	}
}

अटल व्योम handle_label(std::string &command, bool &from_to_active, स्थिर std::string &run_from, स्थिर std::string &run_to)
{
	पूर्णांक pos = 0;
	std::string label;

	जबतक (pos < Get_आकार(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	अगर (pos < Get_आकार(command) && command[pos] == '#')
		वापस;

	जबतक (pos < Get_आकार(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	अगर (Get_आकार(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, Get_आकार(label)-1);
		command = command.substr(pos);

		अगर (label == run_from)
			from_to_active = true;
		अन्यथा अगर (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = false;
	}
}

bool run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	अगर (design == nullptr)
		design = yosys_design;

	अगर (command == "auto") {
	  std::string filename_trim = filename;

	  स्वतः has_extension = [](स्थिर std::string& filename, स्थिर std::string& extension) {
	    अगर (filename.आकार() >= extension.आकार()) {
	      वापस filename.compare(filename.आकार() - extension.आकार(), extension.आकार(), extension) == 0;
	    }
	    वापस false;
	  };

	  अगर (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.आकार() - 3);
	  }

	  अगर (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } अन्यथा अगर (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } अन्यथा अगर (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } अन्यथा अगर (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } अन्यथा अगर (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } अन्यथा अगर (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } अन्यथा अगर (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } अन्यथा अगर (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } अन्यथा अगर (filename == "-") {
	    command = "script";
	  } अन्यथा {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	अगर (command == "script")
	{
		std::string run_from, run_to;
		bool from_to_active = true;

		अगर (from_to_label != शून्य) {
			माप_प्रकार pos = from_to_label->find(':');
			अगर (pos == std::string::npos) {
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

		अगर (filename != "-") {
			f = ख_खोलो(filename.c_str(), "r");
			yosys_निवेश_files.जोड़ो(filename);
		}

		अगर (f == शून्य)
			log_error("Can't open script file `%s' for reading: %s\n", filename, म_त्रुटि(त्रुटि_सं));

		खाता *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		पूर्णांक backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		प्रयास {
			std::string command;
			जबतक (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				जबतक (!command.empty() && command[command.आकार()-1] == '\\') {
					std::string अग्र_पंक्ति;
					अगर (!fgetline(f, अग्र_पंक्ति))
						अवरोध;
					Frontend::current_script_lineno++;
					command.resize(command.आकार()-1);
					command += अग्र_पंक्ति;
				}
				handle_label(command, from_to_active, run_from, run_to);
				अगर (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			अगर (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				अगर (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		पकड़ो (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			फेंको;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		अगर (filename != "-")
			ख_बंद(f);

		वापस true;
	}

	अगर (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		वापस true;
	}

	log("\n");
	अगर (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} अन्यथा {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	अगर (command[0] == ' ') {
		स्वतः argv = split_tokens("read" + command);
		argv.धकेलो_back(filename);
		Pass::call(design, argv);
	} अन्यथा
		Frontend::frontend_call(design, शून्य, filename, command);

	design->check();
	वापस false;
}

व्योम run_pass(std::string command, RTLIL::Design *design)
{
	अगर (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

व्योम run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	अगर (design == nullptr)
		design = yosys_design;

	अगर (command == "auto") {
		अगर (filename.आकार() > 2 && filename.compare(filename.आकार()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		अन्यथा अगर (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		अन्यथा अगर (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		अन्यथा अगर (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		अन्यथा अगर (filename.आकार() > 4 && filename.compare(filename.आकार()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		अन्यथा अगर (filename.आकार() > 5 && filename.compare(filename.आकार()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		अन्यथा अगर (filename.आकार() > 5 && filename.compare(filename.आकार()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		अन्यथा अगर (filename.आकार() > 5 && filename.compare(filename.आकार()-5, std::string::npos, ".json") == 0)
			command = "json";
		अन्यथा अगर (filename == "-")
			command = "rtlil";
		अन्यथा अगर (filename.empty())
			वापस;
		अन्यथा
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	अगर (filename.empty())
		filename = "-";

	log("\n");
	अगर (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} अन्यथा {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, शून्य, filename, command);
}

#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
अटल अक्षर *readline_cmd_generator(स्थिर अक्षर *text, पूर्णांक state)
{
	अटल std::map<std::string, Pass*>::iterator it;
	अटल पूर्णांक len;

	अगर (!state) {
		it = pass_register.begin();
		len = म_माप(text);
	}

	क्रम (; it != pass_register.end(); it++) {
		अगर (it->first.compare(0, len, text) == 0)
			वापस strdup((it++)->first.c_str());
	}
	वापस शून्य;
}

अटल अक्षर *readline_obj_generator(स्थिर अक्षर *text, पूर्णांक state)
{
	अटल std::vector<अक्षर*> obj_names;
	अटल माप_प्रकार idx;

	अगर (!state)
	{
		idx = 0;
		obj_names.सफा();

		RTLIL::Design *design = yosys_get_design();
		पूर्णांक len = म_माप(text);

		अगर (design->selected_active_module.empty())
		{
			क्रम (स्वतः mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				अगर (mod_name.compare(0, len, text) == 0)
					obj_names.धकेलो_back(strdup(mod_name.c_str()));
			}
		}
		अन्यथा अगर (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			क्रम (स्वतः w : module->wires())
				अगर (w->name.unescape().compare(0, len, text) == 0)
					obj_names.धकेलो_back(strdup(w->name.unescape().c_str()));

			क्रम (स्वतः &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				अगर (mem_name.compare(0, len, text) == 0)
					obj_names.धकेलो_back(strdup(mem_name.c_str()));
			}

			क्रम (स्वतः cell : module->cells())
				अगर (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.धकेलो_back(strdup(cell->name.unescape().c_str()));

			क्रम (स्वतः &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				अगर (proc_name.compare(0, len, text) == 0)
					obj_names.धकेलो_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	अगर (idx < obj_names.आकार())
		वापस strdup(obj_names[idx++]);

	idx = 0;
	obj_names.सफा();
	वापस शून्य;
}

अटल अक्षर **readline_completion(स्थिर अक्षर *text, पूर्णांक start, पूर्णांक)
{
	अगर (start == 0)
		वापस rl_completion_matches(text, readline_cmd_generator);
	अगर (म_भेदन(rl_पंक्ति_बफर, "read_", 5) && म_भेदन(rl_पंक्ति_बफर, "write_", 6))
		वापस rl_completion_matches(text, readline_obj_generator);
	वापस शून्य;
}
#पूर्ण_अगर

व्योम shell(RTLIL::Design *design)
{
	अटल पूर्णांक recursion_गणक = 0;

	recursion_गणक++;
	स्वतः guard = logger().error_throw_scope();

#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (अक्षर*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (अक्षर*)" \t\n";
#पूर्ण_अगर

	अक्षर *command = शून्य;
#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	जबतक ((command = readline(create_prompt(design, recursion_गणक))) != शून्य)
	{
#अन्यथा
	अक्षर command_बफर[4096];
	जबतक (1)
	{
		ख_माला_दो(create_prompt(design, recursion_गणक), मानक_निकास);
		ख_साफ(मानक_निकास);
		अगर ((command = ख_माला_लो(command_बफर, 4096, मानक_निवेश)) == शून्य)
			अवरोध;
#पूर्ण_अगर
		अगर (command[म_अखोज(command, " \t\r\n")] == 0) {
#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			मुक्त(command);
#पूर्ण_अगर
			जारी;
		}
#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#पूर्ण_अगर

		अक्षर *p = command + म_अखोज(command, " \t\r\n");
		अगर (!म_भेदन(p, "exit", 4)) {
			p += 4;
			p += म_अखोज(p, " \t\r\n");
			अगर (*p == 0)
				अवरोध;
		}

		प्रयास {
			log_assert(design->selection_ढेर.आकार() == 1);
			Pass::call(design, command);
		} पकड़ो (log_cmd_error_exception) {
			जबतक (design->selection_ढेर.आकार() > 1)
				design->निकालो_selection();
			log_reset_ढेर();
		}
		design->check();
#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		अगर (command)
			मुक्त(command);
#पूर्ण_अगर
	}
	अगर (command == शून्य)
		म_लिखो("exit\n");
#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	अन्यथा
		मुक्त(command);
#पूर्ण_अगर
	recursion_गणक--;
}

काष्ठा ShellPass : खुला Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	व्योम help() override {
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
	व्योम execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, false);
		shell(design);
	}
} ShellPass;

#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
काष्ठा HistoryPass : खुला Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	व्योम help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	व्योम execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, false);
#अगर_घोषित YOSYS_ENABLE_READLINE
		क्रम(HIST_ENTRY **list = history_list(); *list != शून्य; list++)
			log("%s\n", (*list)->पंक्ति);
#अन्यथा
		क्रम (पूर्णांक i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->पंक्ति);
#पूर्ण_अगर
	}
} HistoryPass;
#पूर्ण_अगर

काष्ठा ScriptCmdPass : खुला Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	व्योम help() override {
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
	व्योम execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		bool scriptwire = false;

		माप_प्रकार argidx;
		क्रम (argidx = 1; argidx < args.आकार(); argidx++) {
			अगर (args[argidx] == "-scriptwire") {
				scriptwire = true;
				जारी;
			}
			अवरोध;
		}
		अगर (scriptwire) {
			extra_args(args, argidx, design);

			क्रम (स्वतः mod : design->selected_modules())
				क्रम (स्वतः &c : mod->connections()) {
					अगर (!c.first.is_wire())
						जारी;
					स्वतः w = c.first.as_wire();
					अगर (!mod->selected(w))
						जारी;
					अगर (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					स्वतः v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		अन्यथा अगर (args.आकार() < 2)
			log_cmd_error("Missing script file.\n");
		अन्यथा अगर (args.आकार() == 2)
			run_frontend(args[1], "script", design);
		अन्यथा अगर (args.आकार() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		अन्यथा
			extra_args(args, 2, design, false);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
