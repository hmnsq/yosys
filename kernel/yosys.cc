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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

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
ನಾಮಾಕಾಶ py = pybind11;
#endif

#ಯದಿ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#ನೋಚೇದ್ಯದಿ defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#ಅನ್ಯಥಾ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ಯದಿ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#ಯದಿ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#ಯದಿ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
ಬಾಹ್ಯ "C" PyObject* PyInit_libyosys();
ಬಾಹ್ಯ "C" PyObject* PyInit_pyosys();
#ಅನ್ತರ್ಭಾವ <signal.h>
#endif

#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
ಪೂರ್ಣಾಙ್ಕ yosys_xtrace = 0;
ತಾರ್ಕಿಕ yosys_write_versions = ಸತ್ಯ;
ಧ್ರುವ ವರ್ಣ* yosys_maybe_version() {
	ಯದಿ (yosys_write_versions)
		ಪ್ರತ್ಯಾಗಮ yosys_version_str;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ "Yosys";
}

RTLIL::Design *yosys_design = ರಿಕ್ತ;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ರಿಕ್ತ;
Tcl_Interp *yosys_sdc_interp = ರಿಕ್ತ;
#endif

std::set<std::string> yosys_ಒಳಹರಿವು_files, yosys_output_files;

ತಾರ್ಕಿಕ memhasher_active = ಅಸತ್ಯ;
uint32_t memhasher_rng = 123456;
std::vector<ಶೂನ್ಯ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

ತಾರ್ಕಿಕ Multithreading::active_ = ಅಸತ್ಯ;

ಶೂನ್ಯ init_share_dirname();
ಶೂನ್ಯ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = ಸತ್ಯ;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = ಅಸತ್ಯ;
}

ಶೂನ್ಯ Autoidx::ensure_at_least(ಪೂರ್ಣಾಙ್ಕ v) {
	log_assert(!Multithreading::active());
	ಮೌಲ್ಯ = std::max(ಮೌಲ್ಯ, v);
}

ಪೂರ್ಣಾಙ್ಕ Autoidx::operator++(ಪೂರ್ಣಾಙ್ಕ) {
	log_assert(!Multithreading::active());
	ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ++;
}

ಶೂನ್ಯ memhasher_on()
{
#ಯದಿ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(ರಿಕ್ತ) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = ಸತ್ಯ;
}

ಶೂನ್ಯ memhasher_off()
{
	ಪ್ರತಿ (auto p : memhasher_store)
		ಯದಿ (p) ಮುಕ್ತ(p);
	memhasher_store.clear();
	memhasher_active = ಅಸತ್ಯ;
}

ಶೂನ್ಯ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	ಪೂರ್ಣಾಙ್ಕ size, index = (memhasher_rng >> 4) & 0xffff;
	ವಿಕಲ್ಪ (memhasher_rng & 7) {
		ಪಕ್ಷ 0: size =   16; ಭಙ್ಗ;
		ಪಕ್ಷ 1: size =  256; ಭಙ್ಗ;
		ಪಕ್ಷ 2: size = 1024; ಭಙ್ಗ;
		ಪಕ್ಷ 3: size = 4096; ಭಙ್ಗ;
		ಸ್ವಭಾವ: size = 0;
	}
	ಯದಿ (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

ಶೂನ್ಯ yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#ಯದಿ defined(YOSYS_ENABLE_SPAWN)
ಪೂರ್ಣಾಙ್ಕ run_command(ಧ್ರುವ std::string &command, std::function<ಶೂನ್ಯ(ಧ್ರುವ std::string&)> process_ಸಾಲು)
{
	ಯದಿ (!process_ಸಾಲು)
		ಪ್ರತ್ಯಾಗಮ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ಯದಿ (f == nullptr)
		ಪ್ರತ್ಯಾಗಮ -1;

	std::string ಸಾಲು;
	ವರ್ಣ logbuf[128];
	ಯಾವತ್ (ಸತ್ಯ) {
		ಯದಿ (fgets(logbuf, 128, f) != ರಿಕ್ತ) {
			ಸಾಲು += logbuf;
			ಯದಿ (!ಸಾಲು.empty() && ಸಾಲು.back() == '\n')
				process_ಸಾಲು(ಸಾಲು), ಸಾಲು.clear();
		} ಅನ್ಯಥಾ ಯದಿ (ferror(f) && errno == EINTR) {
			clearerr(f);
			ಅನುವರ್ತ;
		} ಅನ್ಯಥಾ {
			ಭಙ್ಗ;
		}
	}
	ಯದಿ (!ಸಾಲು.empty())
		process_ಸಾಲು(ಸಾಲು);

	ಪೂರ್ಣಾಙ್ಕ ret = pclose(f);
	ಯದಿ (ret < 0)
		ಪ್ರತ್ಯಾಗಮ -2;
#ifdef _WIN32
	ಪ್ರತ್ಯಾಗಮ ret;
#ಅನ್ಯಥಾ
	ಯದಿ (!WIFEXITED(ret))
		ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ WEXITSTATUS(ret);
#endif
}
#endif

ತಾರ್ಕಿಕ already_setup = ಅಸತ್ಯ;
ತಾರ್ಕಿಕ already_shutdown = ಅಸತ್ಯ;

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
	(ಶೂನ್ಯ)_;
	ಕ್ಷಿಪ py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

ಶೂನ್ಯ yosys_setup()
{
	ಯದಿ(already_setup)
		ಪ್ರತ್ಯಾಗಮ;
	already_setup = ಸತ್ಯ;
	already_shutdown = ಅಸತ್ಯ;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ಯದಿ (!Py_IsInitialized()) {
		PyImport_AppendInittab((ವರ್ಣ*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((ವರ್ಣ*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((ವರ್ಣ*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		ಸಙ್ಕೇತ(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = ನೂತನ RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_ತಳ್ಳು();
}

ತಾರ್ಕಿಕ yosys_already_setup()
{
	ಪ್ರತ್ಯಾಗಮ already_setup;
}

ಶೂನ್ಯ yosys_shutdown()
{
	ಯದಿ(already_shutdown)
		ಪ್ರತ್ಯಾಗಮ;
	already_setup = ಅಸತ್ಯ;
	already_shutdown = ಸತ್ಯ;
	log_ತೆಗೆ();

	Pass::done_register();

	delete yosys_design;
	yosys_design = ರಿಕ್ತ;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	ಯದಿ (yosys_tcl_interp != ರಿಕ್ತ) {
		ಯದಿ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ರಿಕ್ತ;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	ಪ್ರತಿ (auto &it : loaded_plugins)
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

ಧ್ರುವ std::string *create_id_prefix(std::string_view file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#ಅನ್ಯಥಾ
	size_t pos = file.find_last_of('/');
#endif
	ಯದಿ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	ಯದಿ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ಪ್ರತ್ಯಾಗಮ ನೂತನ std::string(stringf("$auto$%s:%d:%s$", file, ಸಾಲು, func));
}

RTLIL::Design *yosys_get_design()
{
	ಪ್ರತ್ಯಾಗಮ yosys_design;
}

ಧ್ರುವ ವರ್ಣ *create_prompt(RTLIL::Design *design, ಪೂರ್ಣಾಙ್ಕ recursion_counter, ತಾರ್ಕಿಕ newline)
{
	ಸ್ಥಿರ ವರ್ಣ buffer[100];
	std::string str = newline ? "\n" : "";
	ಯದಿ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ಯದಿ (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	ಯದಿ (!design->full_selection()) {
		ಯದಿ (design->selected_active_module.empty())
			str += "*";
		ಅನ್ಯಥಾ ಯದಿ (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.ಎಣಿಕೆ(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	ಪ್ರತ್ಯಾಗಮ buffer;
}

std::vector<std::string> glob_filename(ಧ್ರುವ std::string &filename_pattern)
{
	std::vector<std::string> results;

#ಯದಿ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ತಳ್ಳು_back(filename_pattern);
#ಅನ್ಯಥಾ
	glob_t globbuf;

	ಪೂರ್ಣಾಙ್ಕ err = glob(filename_pattern.c_str(), 0, ರಿಕ್ತ, &globbuf);

	ಯದಿ(err == 0) {
		ಪ್ರತಿ (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.ತಳ್ಳು_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} ಅನ್ಯಥಾ {
		results.ತಳ್ಳು_back(filename_pattern);
	}
#endif

	ಪ್ರತ್ಯಾಗಮ results;
}

ಶೂನ್ಯ rewrite_filename(std::string &filename)
{
	ಯದಿ (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	ಯದಿ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	ಯದಿ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ yosys_tcl_interp_init(Tcl_Interp *interp);

ಬಾಹ್ಯ Tcl_Interp *yosys_get_tcl_interp()
{
	ಯದಿ (yosys_tcl_interp == ರಿಕ್ತ) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ಪ್ರತ್ಯಾಗಮ yosys_tcl_interp;
}

// Also see SdcPass
ಸಂರಚನಾ TclPass : ಸಾರ್ವಜನಿಕ Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	ಶೂನ್ಯ help() override {
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
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *) override {
		ಯದಿ (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		ಪ್ರತಿ (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.ತಳ್ಳು_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ರಿಕ್ತ, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ರಿಕ್ತ, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ರಿಕ್ತ, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		ಯದಿ (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#ಯದಿ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		ಯದಿ (buflen < (ssize_t)path.size())
			ಭಙ್ಗ;
		ಅನ್ಯಥಾ
			path.resize(path.size() * 2);
	}
	ಯದಿ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} ಅನ್ಯಥಾ {
		ಯಾವತ್ (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	ಪ್ರತ್ಯಾಗಮ path;
}
#ನೋಚೇದ್ಯದಿ defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	ಪೂರ್ಣಾಙ್ಕ mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#ಅನ್ಯಥಾ
	ಪೂರ್ಣಾಙ್ಕ mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	ವರ್ಣ *buffer;
	std::string path;
	ಯದಿ (sysctl(mib, 4, ರಿಕ್ತ, &buflen, ರಿಕ್ತ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (ವರ್ಣ*)malloc(buflen);
	ಯದಿ (buffer == ರಿಕ್ತ)
		log_error("malloc failed: %s\n", strerror(errno));
	ಯದಿ (sysctl(mib, 4, buffer, &buflen, ರಿಕ್ತ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ಯಾವತ್ (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	ಮುಕ್ತ(buffer);
	ಪ್ರತ್ಯಾಗಮ path;
}
#ನೋಚೇದ್ಯದಿ defined(__APPLE__)
std::string proc_self_dirname()
{
	ವರ್ಣ *path = ರಿಕ್ತ;
	uint32_t buflen = 0;
	ಯಾವತ್ (_NSGetExecutablePath(path, &buflen) != 0)
		path = (ವರ್ಣ *) realloc((ಶೂನ್ಯ *) path, buflen);
	ವರ್ಣ resolved[PATH_MAX];
	ಯದಿ (realpath(path, resolved) != ರಿಕ್ತ) {
		ಮುಕ್ತ(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	ಯಾವತ್ (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	ಮುಕ್ತ(path);
	ಪ್ರತ್ಯಾಗಮ str;
}
#ನೋಚೇದ್ಯದಿ defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ಯದಿ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ));
	ಯದಿ (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), ರಿಕ್ತ, ರಿಕ್ತ) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ಪ್ರತ್ಯಾಗಮ ubinpath;

}
#ನೋಚೇದ್ಯದಿ defined(__wasm)
std::string proc_self_dirname()
{
	ಪ್ರತ್ಯಾಗಮ "/";
}
#ನೋಚೇದ್ಯದಿ defined(__OpenBSD__) || defined(__HAIKU__)
ವರ್ಣ yosys_path[PATH_MAX];
ವರ್ಣ *yosys_argv0;

std::string proc_self_dirname(ಶೂನ್ಯ)
{
	ವರ್ಣ buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	ಯದಿ (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, ಪರಿಮಾಣ buf, "%*s/", (ಪೂರ್ಣಾಙ್ಕ)(yosys_path - p), yosys_path);
		ಪ್ರತ್ಯಾಗಮ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	ಯದಿ (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	ಪ್ರತಿ (p = strtok(path, ":"); p; p = strtok(ರಿಕ್ತ, ":")) {
		snprintf(buf, ಪರಿಮಾಣ buf, "%s/%s", p, yosys_argv0);
		ಯದಿ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			ಮುಕ್ತ(path);
			ಪ್ರತ್ಯಾಗಮ buf;
		}
	}
	ಮುಕ್ತ(path);
	log_error("Can't determine yosys executable path.\n");
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#ಅನ್ಯಥಾ
	#error "Don't know how to determine process executable base path!"
#endif

#ಯದಿ defined(__wasm)
ಶೂನ್ಯ init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#ಅನ್ಯಥಾ
ಶೂನ್ಯ init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ಯದಿ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ಧ್ರುವ ವರ್ಣ *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		ಪ್ರತ್ಯಾಗಮ;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  ಯದಿ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	ಯದಿ (check_directory_exists(proc_share_path, ಸತ್ಯ)) {
		yosys_share_dirname = proc_share_path;
		ಪ್ರತ್ಯಾಗಮ;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	ಯದಿ (check_directory_exists(proc_share_path, ಸತ್ಯ)) {
		yosys_share_dirname = proc_share_path;
		ಪ್ರತ್ಯಾಗಮ;
	}
#  ಅನ್ಯಥಾ
	std::string proc_share_path = proc_self_path + "share/";
	ಯದಿ (check_directory_exists(proc_share_path, ಸತ್ಯ)) {
		yosys_share_dirname = proc_share_path;
		ಪ್ರತ್ಯಾಗಮ;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	ಯದಿ (check_directory_exists(proc_share_path, ಸತ್ಯ)) {
		yosys_share_dirname = proc_share_path;
		ಪ್ರತ್ಯಾಗಮ;
	}
	proc_share_path = YOSYS_DATDIR "/";
	ಯದಿ (check_directory_exists(proc_share_path, ಸತ್ಯ)) {
		yosys_share_dirname = proc_share_path;
		ಪ್ರತ್ಯಾಗಮ;
	}
#  endif
}
#endif

ಶೂನ್ಯ init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	ಯದಿ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} ಅನ್ಯಥಾ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#ಅನ್ಯಥಾ
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	ಯದಿ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ಯದಿ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ಧ್ರುವ ವರ್ಣ *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	ಯದಿ (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ಪ್ರತ್ಯಾಗಮ yosys_share_dirname;
}

std::string proc_program_prefix()
{
	ಪ್ರತ್ಯಾಗಮ YOSYS_PROGRAM_PREFIX;
}

ತಾರ್ಕಿಕ fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	ವರ್ಣ block[4096];
	ಯಾವತ್ (1) {
		ಯದಿ (fgets(block, 4096, f) == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		buffer += block;
		ಯದಿ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			ಯಾವತ್ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ handle_label(std::string &command, ತಾರ್ಕಿಕ &from_to_active, ಧ್ರುವ std::string &run_from, ಧ್ರುವ std::string &run_to)
{
	ಪೂರ್ಣಾಙ್ಕ pos = 0;
	std::string label;

	ಯಾವತ್ (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ಯದಿ (pos < GetSize(command) && command[pos] == '#')
		ಪ್ರತ್ಯಾಗಮ;

	ಯಾವತ್ (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ಯದಿ (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		ಯದಿ (label == run_from)
			from_to_active = ಸತ್ಯ;
		ಅನ್ಯಥಾ ಯದಿ (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = ಅಸತ್ಯ;
	}
}

ತಾರ್ಕಿಕ run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	ಯದಿ (design == nullptr)
		design = yosys_design;

	ಯದಿ (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ಧ್ರುವ std::string& filename, ಧ್ರುವ std::string& extension) {
	    ಯದಿ (filename.size() >= extension.size()) {
	      ಪ್ರತ್ಯಾಗಮ filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	  };

	  ಯದಿ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  ಯದಿ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } ಅನ್ಯಥಾ ಯದಿ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } ಅನ್ಯಥಾ ಯದಿ (filename == "-") {
	    command = "script";
	  } ಅನ್ಯಥಾ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ಯದಿ (command == "script")
	{
		std::string run_from, run_to;
		ತಾರ್ಕಿಕ from_to_active = ಸತ್ಯ;

		ಯದಿ (from_to_label != ರಿಕ್ತ) {
			size_t pos = from_to_label->find(':');
			ಯದಿ (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} ಅನ್ಯಥಾ {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ಯದಿ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_ಒಳಹರಿವು_files.ಸೇರಿಸು(filename);
		}

		ಯದಿ (f == ರಿಕ್ತ)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		ಪೂರ್ಣಾಙ್ಕ backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		ಯತ್ನ {
			std::string command;
			ಯಾವತ್ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ಯಾವತ್ (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_ಸಾಲು;
					ಯದಿ (!fgetline(f, next_ಸಾಲು))
						ಭಙ್ಗ;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_ಸಾಲು;
				}
				handle_label(command, from_to_active, run_from, run_to);
				ಯದಿ (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			ಯದಿ (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				ಯದಿ (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		ಗೃಹಾಣ (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ಕ್ಷಿಪ;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		ಯದಿ (filename != "-")
			fclose(f);

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	log("\n");
	ಯದಿ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} ಅನ್ಯಥಾ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ಯದಿ (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.ತಳ್ಳು_back(filename);
		Pass::call(design, argv);
	} ಅನ್ಯಥಾ
		Frontend::frontend_call(design, ರಿಕ್ತ, filename, command);

	design->check();
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ run_pass(std::string command, RTLIL::Design *design)
{
	ಯದಿ (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

ಶೂನ್ಯ run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ಯದಿ (design == nullptr)
		design = yosys_design;

	ಯದಿ (command == "auto") {
		ಯದಿ (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		ಅನ್ಯಥಾ ಯದಿ (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		ಅನ್ಯಥಾ ಯದಿ (filename == "-")
			command = "rtlil";
		ಅನ್ಯಥಾ ಯದಿ (filename.empty())
			ಪ್ರತ್ಯಾಗಮ;
		ಅನ್ಯಥಾ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ಯದಿ (filename.empty())
		filename = "-";

	log("\n");
	ಯದಿ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} ಅನ್ಯಥಾ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ರಿಕ್ತ, filename, command);
}

#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ಸ್ಥಿರ ವರ್ಣ *readline_cmd_generator(ಧ್ರುವ ವರ್ಣ *text, ಪೂರ್ಣಾಙ್ಕ state)
{
	ಸ್ಥಿರ std::map<std::string, Pass*>::iterator it;
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ len;

	ಯದಿ (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	ಪ್ರತಿ (; it != pass_register.end(); it++) {
		ಯದಿ (it->first.compare(0, len, text) == 0)
			ಪ್ರತ್ಯಾಗಮ strdup((it++)->first.c_str());
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ವರ್ಣ *readline_obj_generator(ಧ್ರುವ ವರ್ಣ *text, ಪೂರ್ಣಾಙ್ಕ state)
{
	ಸ್ಥಿರ std::vector<ವರ್ಣ*> obj_names;
	ಸ್ಥಿರ size_t idx;

	ಯದಿ (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		ಪೂರ್ಣಾಙ್ಕ len = strlen(text);

		ಯದಿ (design->selected_active_module.empty())
		{
			ಪ್ರತಿ (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ಯದಿ (mod_name.compare(0, len, text) == 0)
					obj_names.ತಳ್ಳು_back(strdup(mod_name.c_str()));
			}
		}
		ಅನ್ಯಥಾ ಯದಿ (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			ಪ್ರತಿ (auto w : module->wires())
				ಯದಿ (w->name.unescape().compare(0, len, text) == 0)
					obj_names.ತಳ್ಳು_back(strdup(w->name.unescape().c_str()));

			ಪ್ರತಿ (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				ಯದಿ (mem_name.compare(0, len, text) == 0)
					obj_names.ತಳ್ಳು_back(strdup(mem_name.c_str()));
			}

			ಪ್ರತಿ (auto cell : module->cells())
				ಯದಿ (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.ತಳ್ಳು_back(strdup(cell->name.unescape().c_str()));

			ಪ್ರತಿ (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				ಯದಿ (proc_name.compare(0, len, text) == 0)
					obj_names.ತಳ್ಳು_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	ಯದಿ (idx < obj_names.size())
		ಪ್ರತ್ಯಾಗಮ strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ವರ್ಣ **readline_completion(ಧ್ರುವ ವರ್ಣ *text, ಪೂರ್ಣಾಙ್ಕ start, ಪೂರ್ಣಾಙ್ಕ)
{
	ಯದಿ (start == 0)
		ಪ್ರತ್ಯಾಗಮ rl_completion_matches(text, readline_cmd_generator);
	ಯದಿ (strncmp(rl_ಸಾಲು_buffer, "read_", 5) && strncmp(rl_ಸಾಲು_buffer, "write_", 6))
		ಪ್ರತ್ಯಾಗಮ rl_completion_matches(text, readline_obj_generator);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

ಶೂನ್ಯ shell(RTLIL::Design *design)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (ವರ್ಣ*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (ವರ್ಣ*)" \t\n";
#endif

	ವರ್ಣ *command = ರಿಕ್ತ;
#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ಯಾವತ್ ((command = readline(create_prompt(design, recursion_counter))) != ರಿಕ್ತ)
	{
#ಅನ್ಯಥಾ
	ವರ್ಣ command_buffer[4096];
	ಯಾವತ್ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ಯದಿ ((command = fgets(command_buffer, 4096, stdin)) == ರಿಕ್ತ)
			ಭಙ್ಗ;
#endif
		ಯದಿ (command[strspn(command, " \t\r\n")] == 0) {
#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			ಮುಕ್ತ(command);
#endif
			ಅನುವರ್ತ;
		}
#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		ವರ್ಣ *p = command + strspn(command, " \t\r\n");
		ಯದಿ (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ಯದಿ (*p == 0)
				ಭಙ್ಗ;
		}

		ಯತ್ನ {
			log_assert(design->selection_ರಾಶಿ.size() == 1);
			Pass::call(design, command);
		} ಗೃಹಾಣ (log_cmd_error_exception) {
			ಯಾವತ್ (design->selection_ರಾಶಿ.size() > 1)
				design->ತೆಗೆ_selection();
			log_reset_ರಾಶಿ();
		}
		design->check();
#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ಯದಿ (command)
			ಮುಕ್ತ(command);
#endif
	}
	ಯದಿ (command == ರಿಕ್ತ)
		printf("exit\n");
#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ಅನ್ಯಥಾ
		ಮುಕ್ತ(command);
#endif
	recursion_counter--;
}

ಸಂರಚನಾ ShellPass : ಸಾರ್ವಜನಿಕ Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	ಶೂನ್ಯ help() override {
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
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, ಅಸತ್ಯ);
		shell(design);
	}
} ShellPass;

#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ಸಂರಚನಾ HistoryPass : ಸಾರ್ವಜನಿಕ Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	ಶೂನ್ಯ help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, ಅಸತ್ಯ);
#ifdef YOSYS_ENABLE_READLINE
		ಪ್ರತಿ(HIST_ENTRY **list = history_list(); *list != ರಿಕ್ತ; list++)
			log("%s\n", (*list)->ಸಾಲು);
#ಅನ್ಯಥಾ
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->ಸಾಲು);
#endif
	}
} HistoryPass;
#endif

ಸಂರಚನಾ ScriptCmdPass : ಸಾರ್ವಜನಿಕ Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	ಶೂನ್ಯ help() override {
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
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		ತಾರ್ಕಿಕ scriptwire = ಅಸತ್ಯ;

		size_t argidx;
		ಪ್ರತಿ (argidx = 1; argidx < args.size(); argidx++) {
			ಯದಿ (args[argidx] == "-scriptwire") {
				scriptwire = ಸತ್ಯ;
				ಅನುವರ್ತ;
			}
			ಭಙ್ಗ;
		}
		ಯದಿ (scriptwire) {
			extra_args(args, argidx, design);

			ಪ್ರತಿ (auto mod : design->selected_modules())
				ಪ್ರತಿ (auto &c : mod->connections()) {
					ಯದಿ (!c.first.is_wire())
						ಅನುವರ್ತ;
					auto w = c.first.as_wire();
					ಯದಿ (!mod->selected(w))
						ಅನುವರ್ತ;
					ಯದಿ (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		ಅನ್ಯಥಾ ಯದಿ (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		ಅನ್ಯಥಾ ಯದಿ (args.size() == 2)
			run_frontend(args[1], "script", design);
		ಅನ್ಯಥಾ ಯದಿ (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		ಅನ್ಯಥಾ
			extra_args(args, 2, design, ಅಸತ್ಯ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
