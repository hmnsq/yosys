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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/log.h"
#অন্তর্ভাৱ "kernel/newcelltypes.h"

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
নামাকাশ py = pybind11;
#endif

#যদি defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#নোচেদ্যদি defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#অন্যথা
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  যদি defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#যদি !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#যদি defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
বাহ্য "C" PyObject* PyInit_libyosys();
বাহ্য "C" PyObject* PyInit_pyosys();
#অন্তর্ভাৱ <signal.h>
#endif

#অন্তর্ভাৱ <limits.h>
#অন্তর্ভাৱ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
পূর্ণাঙ্ক yosys_xtrace = 0;
তার্কিক yosys_write_versions = সঁচা;
ধ্রুৱ ৱর্ণ* yosys_maybe_version() {
	যদি (yosys_write_versions)
		ঘূৰাই yosys_version_str;
	অন্যথা
		ঘূৰাই "Yosys";
}

RTLIL::Design *yosys_design = রিক্ত;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = রিক্ত;
Tcl_Interp *yosys_sdc_interp = রিক্ত;
#endif

std::set<std::string> yosys_প্ৰবেশ_files, yosys_output_files;

তার্কিক memhasher_active = মিছা;
uint32_t memhasher_rng = 123456;
std::vector<শূন্য*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

তার্কিক Multithreading::active_ = মিছা;

শূন্য init_share_dirname();
শূন্য init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = সঁচা;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = মিছা;
}

শূন্য Autoidx::ensure_at_least(পূর্ণাঙ্ক v) {
	log_assert(!Multithreading::active());
	মান = std::max(মান, v);
}

পূর্ণাঙ্ক Autoidx::operator++(পূর্ণাঙ্ক) {
	log_assert(!Multithreading::active());
	ঘূৰাই মান++;
}

শূন্য memhasher_on()
{
#যদি defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(রিক্ত) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = সঁচা;
}

শূন্য memhasher_off()
{
	বাবে (auto p : memhasher_store)
		যদি (p) মুক্ত(p);
	memhasher_store.clear();
	memhasher_active = মিছা;
}

শূন্য memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	পূর্ণাঙ্ক size, index = (memhasher_rng >> 4) & 0xffff;
	ৱিকল্প (memhasher_rng & 7) {
		পক্ষ 0: size =   16; ভঙ্গ;
		পক্ষ 1: size =  256; ভঙ্গ;
		পক্ষ 2: size = 1024; ভঙ্গ;
		পক্ষ 3: size = 4096; ভঙ্গ;
		স্ৱভাৱ: size = 0;
	}
	যদি (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

শূন্য yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#যদি defined(YOSYS_ENABLE_SPAWN)
পূর্ণাঙ্ক run_command(ধ্রুৱ std::string &command, std::function<শূন্য(ধ্রুৱ std::string&)> process_সাৰি)
{
	যদি (!process_সাৰি)
		ঘূৰাই system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	যদি (f == nullptr)
		ঘূৰাই -1;

	std::string সাৰি;
	ৱর্ণ logbuf[128];
	যেতিয়ালৈ (সঁচা) {
		যদি (fgets(logbuf, 128, f) != রিক্ত) {
			সাৰি += logbuf;
			যদি (!সাৰি.empty() && সাৰি.back() == '\n')
				process_সাৰি(সাৰি), সাৰি.clear();
		} অন্যথা যদি (ferror(f) && errno == EINTR) {
			clearerr(f);
			অনুৱর্ত;
		} অন্যথা {
			ভঙ্গ;
		}
	}
	যদি (!সাৰি.empty())
		process_সাৰি(সাৰি);

	পূর্ণাঙ্ক ret = pclose(f);
	যদি (ret < 0)
		ঘূৰাই -2;
#ifdef _WIN32
	ঘূৰাই ret;
#অন্যথা
	যদি (!WIFEXITED(ret))
		ঘূৰাই -1;
	ঘূৰাই WEXITSTATUS(ret);
#endif
}
#endif

তার্কিক already_setup = মিছা;
তার্কিক already_shutdown = মিছা;

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
	(শূন্য)_;
	ক্ষিপ py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

শূন্য yosys_setup()
{
	যদি(already_setup)
		ঘূৰাই;
	already_setup = সঁচা;
	already_shutdown = মিছা;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	যদি (!Py_IsInitialized()) {
		PyImport_AppendInittab((ৱর্ণ*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((ৱর্ণ*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((ৱর্ণ*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		সঙ্কেত(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = নূতন RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_ঠেলো();
}

তার্কিক yosys_already_setup()
{
	ঘূৰাই already_setup;
}

শূন্য yosys_shutdown()
{
	যদি(already_shutdown)
		ঘূৰাই;
	already_setup = মিছা;
	already_shutdown = সঁচা;
	log_বেৰ_কৰো();

	Pass::done_register();

	delete yosys_design;
	yosys_design = রিক্ত;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	যদি (yosys_tcl_interp != রিক্ত) {
		যদি (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = রিক্ত;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	বাবে (auto &it : loaded_plugins)
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

ধ্রুৱ std::string *create_id_prefix(std::string_view file, পূর্ণাঙ্ক সাৰি, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#অন্যথা
	size_t pos = file.find_last_of('/');
#endif
	যদি (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	যদি (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ঘূৰাই নূতন std::string(stringf("$auto$%s:%d:%s$", file, সাৰি, func));
}

RTLIL::Design *yosys_get_design()
{
	ঘূৰাই yosys_design;
}

ধ্রুৱ ৱর্ণ *create_prompt(RTLIL::Design *design, পূর্ণাঙ্ক recursion_counter, তার্কিক newline)
{
	স্থির ৱর্ণ buffer[100];
	std::string str = newline ? "\n" : "";
	যদি (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	যদি (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	যদি (!design->full_selection()) {
		যদি (design->selected_active_module.empty())
			str += "*";
		অন্যথা যদি (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.গুনতি(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	ঘূৰাই buffer;
}

std::vector<std::string> glob_filename(ধ্রুৱ std::string &filename_pattern)
{
	std::vector<std::string> results;

#যদি defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ঠেলো_back(filename_pattern);
#অন্যথা
	glob_t globbuf;

	পূর্ণাঙ্ক err = glob(filename_pattern.c_str(), 0, রিক্ত, &globbuf);

	যদি(err == 0) {
		বাবে (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.ঠেলো_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} অন্যথা {
		results.ঠেলো_back(filename_pattern);
	}
#endif

	ঘূৰাই results;
}

শূন্য rewrite_filename(std::string &filename)
{
	যদি (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	যদি (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	যদি (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
বাহ্য পূর্ণাঙ্ক yosys_tcl_interp_init(Tcl_Interp *interp);

বাহ্য Tcl_Interp *yosys_get_tcl_interp()
{
	যদি (yosys_tcl_interp == রিক্ত) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ঘূৰাই yosys_tcl_interp;
}

// Also see SdcPass
সংরচনা TclPass : সার্ৱজনিক Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	শূন্য help() override {
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
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *) override {
		যদি (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		বাবে (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.ঠেলো_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), রিক্ত, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), রিক্ত, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), রিক্ত, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		যদি (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#যদি defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	বাবে (পূর্ণাঙ্ক tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		যদি (buflen < (ssize_t)path.size())
			ভঙ্গ;
		অন্যথা
			path.resize(path.size() * 2);
	}
	যদি (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} অন্যথা {
		যেতিয়ালৈ (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	ঘূৰাই path;
}
#নোচেদ্যদি defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	পূর্ণাঙ্ক mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#অন্যথা
	পূর্ণাঙ্ক mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	ৱর্ণ *buffer;
	std::string path;
	যদি (sysctl(mib, 4, রিক্ত, &buflen, রিক্ত, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (ৱর্ণ*)malloc(buflen);
	যদি (buffer == রিক্ত)
		log_error("malloc failed: %s\n", strerror(errno));
	যদি (sysctl(mib, 4, buffer, &buflen, রিক্ত, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	যেতিয়ালৈ (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	মুক্ত(buffer);
	ঘূৰাই path;
}
#নোচেদ্যদি defined(__APPLE__)
std::string proc_self_dirname()
{
	ৱর্ণ *path = রিক্ত;
	uint32_t buflen = 0;
	যেতিয়ালৈ (_NSGetExecutablePath(path, &buflen) != 0)
		path = (ৱর্ণ *) realloc((শূন্য *) path, buflen);
	ৱর্ণ resolved[PATH_MAX];
	যদি (realpath(path, resolved) != রিক্ত) {
		মুক্ত(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	যেতিয়ালৈ (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	মুক্ত(path);
	ঘূৰাই str;
}
#নোচেদ্যদি defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	যদি (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), রিক্ত, 0, রিক্ত, রিক্ত));
	যদি (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), রিক্ত, রিক্ত) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ঘূৰাই ubinpath;

}
#নোচেদ্যদি defined(__wasm)
std::string proc_self_dirname()
{
	ঘূৰাই "/";
}
#নোচেদ্যদি defined(__OpenBSD__) || defined(__HAIKU__)
ৱর্ণ yosys_path[PATH_MAX];
ৱর্ণ *yosys_argv0;

std::string proc_self_dirname(শূন্য)
{
	ৱর্ণ buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	যদি (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, পরিমাণ buf, "%*s/", (পূর্ণাঙ্ক)(yosys_path - p), yosys_path);
		ঘূৰাই buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	যদি (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	বাবে (p = strtok(path, ":"); p; p = strtok(রিক্ত, ":")) {
		snprintf(buf, পরিমাণ buf, "%s/%s", p, yosys_argv0);
		যদি (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			মুক্ত(path);
			ঘূৰাই buf;
		}
	}
	মুক্ত(path);
	log_error("Can't determine yosys executable path.\n");
	ঘূৰাই রিক্ত;
}
#অন্যথা
	#error "Don't know how to determine process executable base path!"
#endif

#যদি defined(__wasm)
শূন্য init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#অন্যথা
শূন্য init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	যদি (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ধ্রুৱ ৱর্ণ *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		ঘূৰাই;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  যদি defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	যদি (check_directory_exists(proc_share_path, সঁচা)) {
		yosys_share_dirname = proc_share_path;
		ঘূৰাই;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	যদি (check_directory_exists(proc_share_path, সঁচা)) {
		yosys_share_dirname = proc_share_path;
		ঘূৰাই;
	}
#  অন্যথা
	std::string proc_share_path = proc_self_path + "share/";
	যদি (check_directory_exists(proc_share_path, সঁচা)) {
		yosys_share_dirname = proc_share_path;
		ঘূৰাই;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	যদি (check_directory_exists(proc_share_path, সঁচা)) {
		yosys_share_dirname = proc_share_path;
		ঘূৰাই;
	}
	proc_share_path = YOSYS_DATDIR "/";
	যদি (check_directory_exists(proc_share_path, সঁচা)) {
		yosys_share_dirname = proc_share_path;
		ঘূৰাই;
	}
#  endif
}
#endif

শূন্য init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	যদি (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} অন্যথা {
		yosys_abc_executable = ABCEXTERNAL;
	}
#অন্যথা
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	যদি (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	যদি (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ধ্রুৱ ৱর্ণ *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	যদি (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ঘূৰাই yosys_share_dirname;
}

std::string proc_program_prefix()
{
	ঘূৰাই YOSYS_PROGRAM_PREFIX;
}

তার্কিক fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	ৱর্ণ block[4096];
	যেতিয়ালৈ (1) {
		যদি (fgets(block, 4096, f) == রিক্ত)
			ঘূৰাই মিছা;
		buffer += block;
		যদি (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			যেতিয়ালৈ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			ঘূৰাই সঁচা;
		}
	}
}

স্থির শূন্য handle_label(std::string &command, তার্কিক &from_to_active, ধ্রুৱ std::string &run_from, ধ্রুৱ std::string &run_to)
{
	পূর্ণাঙ্ক pos = 0;
	std::string label;

	যেতিয়ালৈ (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	যদি (pos < GetSize(command) && command[pos] == '#')
		ঘূৰাই;

	যেতিয়ালৈ (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	যদি (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		যদি (label == run_from)
			from_to_active = সঁচা;
		অন্যথা যদি (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = মিছা;
	}
}

তার্কিক run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	যদি (design == nullptr)
		design = yosys_design;

	যদি (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ধ্রুৱ std::string& filename, ধ্রুৱ std::string& extension) {
	    যদি (filename.size() >= extension.size()) {
	      ঘূৰাই filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    ঘূৰাই মিছা;
	  };

	  যদি (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  যদি (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } অন্যথা যদি (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } অন্যথা যদি (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } অন্যথা যদি (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } অন্যথা যদি (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } অন্যথা যদি (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } অন্যথা যদি (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } অন্যথা যদি (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } অন্যথা যদি (filename == "-") {
	    command = "script";
	  } অন্যথা {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	যদি (command == "script")
	{
		std::string run_from, run_to;
		তার্কিক from_to_active = সঁচা;

		যদি (from_to_label != রিক্ত) {
			size_t pos = from_to_label->find(':');
			যদি (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} অন্যথা {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		যদি (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_প্ৰবেশ_files.ঢোকাও(filename);
		}

		যদি (f == রিক্ত)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		পূর্ণাঙ্ক backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		যত্ন {
			std::string command;
			যেতিয়ালৈ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				যেতিয়ালৈ (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_সাৰি;
					যদি (!fgetline(f, next_সাৰি))
						ভঙ্গ;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_সাৰি;
				}
				handle_label(command, from_to_active, run_from, run_to);
				যদি (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			যদি (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				যদি (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		গৃহাণ (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ক্ষিপ;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		যদি (filename != "-")
			fclose(f);

		ঘূৰাই সঁচা;
	}

	যদি (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		ঘূৰাই সঁচা;
	}

	log("\n");
	যদি (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} অন্যথা {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	যদি (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.ঠেলো_back(filename);
		Pass::call(design, argv);
	} অন্যথা
		Frontend::frontend_call(design, রিক্ত, filename, command);

	design->check();
	ঘূৰাই মিছা;
}

শূন্য run_pass(std::string command, RTLIL::Design *design)
{
	যদি (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

শূন্য run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	যদি (design == nullptr)
		design = yosys_design;

	যদি (command == "auto") {
		যদি (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		অন্যথা যদি (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		অন্যথা যদি (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		অন্যথা যদি (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		অন্যথা যদি (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		অন্যথা যদি (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		অন্যথা যদি (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		অন্যথা যদি (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		অন্যথা যদি (filename == "-")
			command = "rtlil";
		অন্যথা যদি (filename.empty())
			ঘূৰাই;
		অন্যথা
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	যদি (filename.empty())
		filename = "-";

	log("\n");
	যদি (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} অন্যথা {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, রিক্ত, filename, command);
}

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
স্থির ৱর্ণ *readline_cmd_generator(ধ্রুৱ ৱর্ণ *text, পূর্ণাঙ্ক state)
{
	স্থির std::map<std::string, Pass*>::iterator it;
	স্থির পূর্ণাঙ্ক len;

	যদি (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	বাবে (; it != pass_register.end(); it++) {
		যদি (it->first.compare(0, len, text) == 0)
			ঘূৰাই strdup((it++)->first.c_str());
	}
	ঘূৰাই রিক্ত;
}

স্থির ৱর্ণ *readline_obj_generator(ধ্রুৱ ৱর্ণ *text, পূর্ণাঙ্ক state)
{
	স্থির std::vector<ৱর্ণ*> obj_names;
	স্থির size_t idx;

	যদি (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		পূর্ণাঙ্ক len = strlen(text);

		যদি (design->selected_active_module.empty())
		{
			বাবে (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				যদি (mod_name.compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(mod_name.c_str()));
			}
		}
		অন্যথা যদি (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			বাবে (auto w : module->wires())
				যদি (w->name.unescape().compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(w->name.unescape().c_str()));

			বাবে (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				যদি (mem_name.compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(mem_name.c_str()));
			}

			বাবে (auto cell : module->cells())
				যদি (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(cell->name.unescape().c_str()));

			বাবে (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				যদি (proc_name.compare(0, len, text) == 0)
					obj_names.ঠেলো_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	যদি (idx < obj_names.size())
		ঘূৰাই strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	ঘূৰাই রিক্ত;
}

স্থির ৱর্ণ **readline_completion(ধ্রুৱ ৱর্ণ *text, পূর্ণাঙ্ক start, পূর্ণাঙ্ক)
{
	যদি (start == 0)
		ঘূৰাই rl_completion_matches(text, readline_cmd_generator);
	যদি (strncmp(rl_সাৰি_buffer, "read_", 5) && strncmp(rl_সাৰি_buffer, "write_", 6))
		ঘূৰাই rl_completion_matches(text, readline_obj_generator);
	ঘূৰাই রিক্ত;
}
#endif

শূন্য shell(RTLIL::Design *design)
{
	স্থির পূর্ণাঙ্ক recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (ৱর্ণ*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (ৱর্ণ*)" \t\n";
#endif

	ৱর্ণ *command = রিক্ত;
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	যেতিয়ালৈ ((command = readline(create_prompt(design, recursion_counter))) != রিক্ত)
	{
#অন্যথা
	ৱর্ণ command_buffer[4096];
	যেতিয়ালৈ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		যদি ((command = fgets(command_buffer, 4096, stdin)) == রিক্ত)
			ভঙ্গ;
#endif
		যদি (command[strspn(command, " \t\r\n")] == 0) {
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			মুক্ত(command);
#endif
			অনুৱর্ত;
		}
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		ৱর্ণ *p = command + strspn(command, " \t\r\n");
		যদি (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			যদি (*p == 0)
				ভঙ্গ;
		}

		যত্ন {
			log_assert(design->selection_স্তূপ.size() == 1);
			Pass::call(design, command);
		} গৃহাণ (log_cmd_error_exception) {
			যেতিয়ালৈ (design->selection_স্তূপ.size() > 1)
				design->বেৰ_কৰো_selection();
			log_reset_স্তূপ();
		}
		design->check();
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		যদি (command)
			মুক্ত(command);
#endif
	}
	যদি (command == রিক্ত)
		printf("exit\n");
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	অন্যথা
		মুক্ত(command);
#endif
	recursion_counter--;
}

সংরচনা ShellPass : সার্ৱজনিক Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	শূন্য help() override {
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
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, মিছা);
		shell(design);
	}
} ShellPass;

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
সংরচনা HistoryPass : সার্ৱজনিক Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	শূন্য help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, মিছা);
#ifdef YOSYS_ENABLE_READLINE
		বাবে(HIST_ENTRY **list = history_list(); *list != রিক্ত; list++)
			log("%s\n", (*list)->সাৰি);
#অন্যথা
		বাবে (পূর্ণাঙ্ক i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->সাৰি);
#endif
	}
} HistoryPass;
#endif

সংরচনা ScriptCmdPass : সার্ৱজনিক Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	শূন্য help() override {
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
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		তার্কিক scriptwire = মিছা;

		size_t argidx;
		বাবে (argidx = 1; argidx < args.size(); argidx++) {
			যদি (args[argidx] == "-scriptwire") {
				scriptwire = সঁচা;
				অনুৱর্ত;
			}
			ভঙ্গ;
		}
		যদি (scriptwire) {
			extra_args(args, argidx, design);

			বাবে (auto mod : design->selected_modules())
				বাবে (auto &c : mod->connections()) {
					যদি (!c.first.is_wire())
						অনুৱর্ত;
					auto w = c.first.as_wire();
					যদি (!mod->selected(w))
						অনুৱর্ত;
					যদি (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		অন্যথা যদি (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		অন্যথা যদি (args.size() == 2)
			run_frontend(args[1], "script", design);
		অন্যথা যদি (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		অন্যথা
			extra_args(args, 2, design, মিছা);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
