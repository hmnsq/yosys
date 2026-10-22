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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/log.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"

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
ନାମାକାଶ py = pybind11;
#endif

#ଯଦି defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#ନଚେତଯଦି defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#ଅନ୍ୟଥା
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ଯଦି defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#endif

#ଯଦି !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#endif

#ଯଦି defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
ବାହ୍ୟ "C" PyObject* PyInit_libyosys();
ବାହ୍ୟ "C" PyObject* PyInit_pyosys();
#ଅନ୍ତର୍ଭାଵ <signal.h>
#endif

#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା yosys_xtrace = 0;
ତାର୍କିକ yosys_write_versions = ସତ୍ୟ;
ଅଚଳ ଅକ୍ଷର* yosys_maybe_version() {
	ଯଦି (yosys_write_versions)
		ଫେରନ୍ତୁ yosys_version_str;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ "Yosys";
}

RTLIL::Design *yosys_design = ଖାଲି;
NewCellTypes yosys_celltypes;

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ଖାଲି;
Tcl_Interp *yosys_sdc_interp = ଖାଲି;
#endif

std::set<std::string> yosys_ନିବେଶ_files, yosys_output_files;

ତାର୍କିକ memhasher_active = ମିଥ୍ୟା;
uint32_t memhasher_rng = 123456;
std::vector<ଶୂନ୍ୟ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

ତାର୍କିକ Multithreading::active_ = ମିଥ୍ୟା;

ଶୂନ୍ୟ init_share_dirname();
ଶୂନ୍ୟ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!active_);
	active_ = ସତ୍ୟ;
}

Multithreading::~Multithreading() {
	log_assert(active_);
	active_ = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Autoidx::ensure_at_least(ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) {
	log_assert(!Multithreading::active());
	ମୂଲ୍ୟ = std::max(ମୂଲ୍ୟ, v);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Autoidx::operator++(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) {
	log_assert(!Multithreading::active());
	ଫେରନ୍ତୁ ମୂଲ୍ୟ++;
}

ଶୂନ୍ୟ memhasher_on()
{
#ଯଦି defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(ଖାଲି) << 16 ^ getpid();
#endif
	memhasher_store.resize(0x10000);
	memhasher_active = ସତ୍ୟ;
}

ଶୂନ୍ୟ memhasher_off()
{
	ପାଇଁ (auto p : memhasher_store)
		ଯଦି (p) ମୁକ୍ତ(p);
	memhasher_store.clear();
	memhasher_active = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, index = (memhasher_rng >> 4) & 0xffff;
	ବାଛ (memhasher_rng & 7) {
		ସ୍ଥିତି 0: size =   16; ଭାଙ୍ଗ;
		ସ୍ଥିତି 1: size =  256; ଭାଙ୍ଗ;
		ସ୍ଥିତି 2: size = 1024; ଭାଙ୍ଗ;
		ସ୍ଥିତି 3: size = 4096; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: size = 0;
	}
	ଯଦି (index < 16) size *= 16;
	memhasher_store[index] = realloc(memhasher_store[index], size);
}

ଶୂନ୍ୟ yosys_banner()
{
	log("\n");
	log(" /----------------------------------------------------------------------------\\\n");
	log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	log(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	log(" \\----------------------------------------------------------------------------/\n");
	log(" %s\n", yosys_maybe_version());
}

#ଯଦି defined(YOSYS_ENABLE_SPAWN)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା run_command(ଅଚଳ std::string &command, std::function<ଶୂନ୍ୟ(ଅଚଳ std::string&)> process_ଧାଡ଼ି)
{
	ଯଦି (!process_ଧାଡ଼ି)
		ଫେରନ୍ତୁ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ଯଦି (f == nullptr)
		ଫେରନ୍ତୁ -1;

	std::string ଧାଡ଼ି;
	ଅକ୍ଷର logbuf[128];
	ଯେତେବେଳେ (ସତ୍ୟ) {
		ଯଦି (fgets(logbuf, 128, f) != ଖାଲି) {
			ଧାଡ଼ି += logbuf;
			ଯଦି (!ଧାଡ଼ି.empty() && ଧାଡ଼ି.back() == '\n')
				process_ଧାଡ଼ି(ଧାଡ଼ି), ଧାଡ଼ି.clear();
		} ଅନ୍ୟଥା ଯଦି (ferror(f) && errno == EINTR) {
			clearerr(f);
			ଜାରି;
		} ଅନ୍ୟଥା {
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି (!ଧାଡ଼ି.empty())
		process_ଧାଡ଼ି(ଧାଡ଼ି);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = pclose(f);
	ଯଦି (ret < 0)
		ଫେରନ୍ତୁ -2;
#ifdef _WIN32
	ଫେରନ୍ତୁ ret;
#ଅନ୍ୟଥା
	ଯଦି (!WIFEXITED(ret))
		ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ WEXITSTATUS(ret);
#endif
}
#endif

ତାର୍କିକ already_setup = ମିଥ୍ୟା;
ତାର୍କିକ already_shutdown = ମିଥ୍ୟା;

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
	(ଶୂନ୍ୟ)_;
	ଫିଙ୍ଗ py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#endif

ଶୂନ୍ୟ yosys_setup()
{
	ଯଦି(already_setup)
		ଫେରନ୍ତୁ;
	already_setup = ସତ୍ୟ;
	already_shutdown = ମିଥ୍ୟା;

	StaticTwines::init();

#ifdef YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ଯଦି (!Py_IsInitialized()) {
		PyImport_AppendInittab((ଅକ୍ଷର*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((ଅକ୍ଷର*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((ଅକ୍ଷର*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		PyRun_SimpleString("import sys");
		ସଙ୍କେତ(SIGINT, SIG_DFL);
	}
#endif

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_register();
	yosys_design = ନୂତନ RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_known;
	log_ଠେଲ();
}

ତାର୍କିକ yosys_already_setup()
{
	ଫେରନ୍ତୁ already_setup;
}

ଶୂନ୍ୟ yosys_shutdown()
{
	ଯଦି(already_shutdown)
		ଫେରନ୍ତୁ;
	already_setup = ମିଥ୍ୟା;
	already_shutdown = ସତ୍ୟ;
	log_କାଢ଼();

	Pass::done_register();

	delete yosys_design;
	yosys_design = ଖାଲି;

	logger().clear();

#ifdef YOSYS_ENABLE_TCL
	ଯଦି (yosys_tcl_interp != ଖାଲି) {
		ଯଦି (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ଖାଲି;
	}
#endif

#ifdef YOSYS_ENABLE_PLUGINS
	ପାଇଁ (auto &it : loaded_plugins)
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

ଅଚଳ std::string *create_id_prefix(std::string_view file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, std::string_view func)
{
#ifdef _WIN32
	size_t pos = file.find_last_of("/\\");
#ଅନ୍ୟଥା
	size_t pos = file.find_last_of('/');
#endif
	ଯଦି (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_last_of(':');
	ଯଦି (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ଫେରନ୍ତୁ ନୂତନ std::string(stringf("$auto$%s:%d:%s$", file, ଧାଡ଼ି, func));
}

RTLIL::Design *yosys_get_design()
{
	ଫେରନ୍ତୁ yosys_design;
}

ଅଚଳ ଅକ୍ଷର *create_prompt(RTLIL::Design *design, ପୂର୍ଣ୍ଣସଂଖ୍ୟା recursion_counter, ତାର୍କିକ newline)
{
	ସ୍ଥିର ଅକ୍ଷର buffer[100];
	std::string str = newline ? "\n" : "";
	ଯଦି (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ଯଦି (!design->selected_active_module.empty())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_active_module)).c_str());
	ଯଦି (!design->full_selection()) {
		ଯଦି (design->selected_active_module.empty())
			str += "*";
		ଅନ୍ୟଥା ଯଦି (design->selection().selected_modules.size() != 1 || design->selection().selected_members.size() != 0 ||
				design->selection().selected_modules.ଗଣନା_2(design->selected_active_module) == 0)
			str += "*";
	}
	snprintf(buffer, 100, "%s> ", str.c_str());
	ଫେରନ୍ତୁ buffer;
}

std::vector<std::string> glob_filename(ଅଚଳ std::string &filename_pattern)
{
	std::vector<std::string> results;

#ଯଦି defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ଠେଲ_back(filename_pattern);
#ଅନ୍ୟଥା
	glob_t globbuf;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = glob(filename_pattern.c_str(), 0, ଖାଲି, &globbuf);

	ଯଦି(err == 0) {
		ପାଇଁ (size_t i = 0; i < globbuf.gl_pathc; i++)
			results.ଠେଲ_back(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} ଅନ୍ୟଥା {
		results.ଠେଲ_back(filename_pattern);
	}
#endif

	ଫେରନ୍ତୁ results;
}

ଶୂନ୍ୟ rewrite_filename(std::string &filename)
{
	ଯଦି (filename.compare(0, 1, "\"") == 0 && filename.compare(GetSize(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, GetSize(filename)-2);
	ଯଦି (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ifndef _WIN32
	ଯଦି (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#endif
}

#ifdef YOSYS_ENABLE_TCL

// defined in tclapi.cc
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା yosys_tcl_interp_init(Tcl_Interp *interp);

ବାହ୍ୟ Tcl_Interp *yosys_get_tcl_interp()
{
	ଯଦି (yosys_tcl_interp == ଖାଲି) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ଫେରନ୍ତୁ yosys_tcl_interp;
}

// Also see SdcPass
ସଂରଚନା TclPass : ସର୍ବସାଧାରଣ Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	ଶୂନ୍ୟ help() override {
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
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *) override {
		ଯଦି (args.size() < 2)
			log_cmd_error("Missing script file.\n");

		std::vector<Tcl_Obj*> script_args;
		ପାଇଁ (auto it = args.begin() + 2; it != args.end(); ++it)
			script_args.ଠେଲ_back(Tcl_NewStringObj((*it).c_str(), (*it).size()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ଖାଲି, Tcl_NewIntObj(script_args.size()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ଖାଲି, Tcl_NewListObj(script_args.size(), script_args.data()), 0);
		Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ଖାଲି, Tcl_NewStringObj(args[1].c_str(), args[1].size()), 0);
		ଯଦି (Tcl_EvalFile(interp, args[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(interp));
		Tcl_Release(interp);
	}
} TclPass;

#endif

#ଯଦି defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string path(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", path.data(), path.size());
		ଯଦି (buflen < (ssize_t)path.size())
			ଭାଙ୍ଗ;
		ଅନ୍ୟଥା
			path.resize(path.size() * 2);
	}
	ଯଦି (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		path.resize(0);
	} ଅନ୍ୟଥା {
		ଯେତେବେଳେ (buflen > 0 && path[buflen-1] != '/')
			buflen--;
		path.resize(buflen);
	}
	ଫେରନ୍ତୁ path;
}
#ନଚେତଯଦି defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ifdef __NetBSD__
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mib[4] = {CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_PATHNAME};
#ଅନ୍ୟଥା
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#endif
	size_t buflen;
	ଅକ୍ଷର *buffer;
	std::string path;
	ଯଦି (sysctl(mib, 4, ଖାଲି, &buflen, ଖାଲି, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	buffer = (ଅକ୍ଷର*)malloc(buflen);
	ଯଦି (buffer == ଖାଲି)
		log_error("malloc failed: %s\n", strerror(errno));
	ଯଦି (sysctl(mib, 4, buffer, &buflen, ଖାଲି, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ଯେତେବେଳେ (buflen > 0 && buffer[buflen-1] != '/')
		buflen--;
	path.assign(buffer, buflen);
	ମୁକ୍ତ(buffer);
	ଫେରନ୍ତୁ path;
}
#ନଚେତଯଦି defined(__APPLE__)
std::string proc_self_dirname()
{
	ଅକ୍ଷର *path = ଖାଲି;
	uint32_t buflen = 0;
	ଯେତେବେଳେ (_NSGetExecutablePath(path, &buflen) != 0)
		path = (ଅକ୍ଷର *) realloc((ଶୂନ୍ୟ *) path, buflen);
	ଅକ୍ଷର resolved[PATH_MAX];
	ଯଦି (realpath(path, resolved) != ଖାଲି) {
		ମୁକ୍ତ(path);
		path = strdup(resolved);
		buflen = strlen(path);
	}
	ଯେତେବେଳେ (buflen > 0 && path[buflen-1] != '/')
		buflen--;
	std::string str(path, buflen);
	ମୁକ୍ତ(path);
	ଫେରନ୍ତୁ str;
}
#ନଚେତଯଦି defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ଯଦି (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.size()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), ଖାଲି, 0, ଖାଲି, ଖାଲି));
	ଯଦି (WideCharToMultiByte(CP_UTF8, 0, wbinpath.data(), wbinpath.size(), &ubinpath[0], ubinpath.size(), ଖାଲି, ଖାଲି) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ଫେରନ୍ତୁ ubinpath;

}
#ନଚେତଯଦି defined(__wasm)
std::string proc_self_dirname()
{
	ଫେରନ୍ତୁ "/";
}
#ନଚେତଯଦି defined(__OpenBSD__) || defined(__HAIKU__)
ଅକ୍ଷର yosys_path[PATH_MAX];
ଅକ୍ଷର *yosys_argv0;

std::string proc_self_dirname(ଶୂନ୍ୟ)
{
	ଅକ୍ଷର buf[PATH_MAX + 1] = "", *path, *p;
	// if case argv[0] contains a valid path, return it
	ଯଦି (strlen(yosys_path) > 0) {
		p = strrchr(yosys_path, '/');
		snprintf(buf, ଆକାର buf, "%*s/", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(yosys_path - p), yosys_path);
		ଫେରନ୍ତୁ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	path = strdup(getenv("PATH"));
	ଯଦି (!path)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	ପାଇଁ (p = strtok(path, ":"); p; p = strtok(ଖାଲି, ":")) {
		snprintf(buf, ଆକାର buf, "%s/%s", p, yosys_argv0);
		ଯଦି (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			ମୁକ୍ତ(path);
			ଫେରନ୍ତୁ buf;
		}
	}
	ମୁକ୍ତ(path);
	log_error("Can't determine yosys executable path.\n");
	ଫେରନ୍ତୁ ଖାଲି;
}
#ଅନ୍ୟଥା
	#error "Don't know how to determine process executable base path!"
#endif

#ଯଦି defined(__wasm)
ଶୂନ୍ୟ init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#ଅନ୍ୟଥା
ଶୂନ୍ୟ init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ଯଦି (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ଅଚଳ ଅକ୍ଷର *share_path = PyUnicode_AsUTF8(share_path_obj);
		yosys_share_dirname = std::string(share_path);
		ଫେରନ୍ତୁ;
	}
#  endif
	std::string proc_self_path = proc_self_dirname();
#  ଯଦି defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_path = proc_self_path + "share\\";
	ଯଦି (check_directory_exists(proc_share_path, ସତ୍ୟ)) {
		yosys_share_dirname = proc_share_path;
		ଫେରନ୍ତୁ;
	}
	proc_share_path = proc_self_path + "..\\share\\";
	ଯଦି (check_directory_exists(proc_share_path, ସତ୍ୟ)) {
		yosys_share_dirname = proc_share_path;
		ଫେରନ୍ତୁ;
	}
#  ଅନ୍ୟଥା
	std::string proc_share_path = proc_self_path + "share/";
	ଯଦି (check_directory_exists(proc_share_path, ସତ୍ୟ)) {
		yosys_share_dirname = proc_share_path;
		ଫେରନ୍ତୁ;
	}
	proc_share_path = proc_self_path + "../share/" + proc_program_prefix()+ "yosys/";
	ଯଦି (check_directory_exists(proc_share_path, ସତ୍ୟ)) {
		yosys_share_dirname = proc_share_path;
		ଫେରନ୍ତୁ;
	}
	proc_share_path = YOSYS_DATDIR "/";
	ଯଦି (check_directory_exists(proc_share_path, ସତ୍ୟ)) {
		yosys_share_dirname = proc_share_path;
		ଫେରନ୍ତୁ;
	}
#  endif
}
#endif

ଶୂନ୍ୟ init_abc_executable_name()
{
#ifdef ABCEXTERNAL
	std::string exe_file;
	ଯଦି (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} ଅନ୍ୟଥା {
		yosys_abc_executable = ABCEXTERNAL;
	}
#ଅନ୍ୟଥା
	yosys_abc_executable = proc_self_dirname() + proc_program_prefix()+ "yosys-abc";
#  ifdef _WIN32
	ଯଦି (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_program_prefix() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ଯଦି (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_path_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ଅଚଳ ଅକ୍ଷର *abc_path = PyUnicode_AsUTF8(abc_path_obj);
		yosys_abc_executable = std::string(abc_path);
	}
#  endif
#endif
}

std::string proc_share_dirname()
{
	ଯଦି (yosys_share_dirname.empty())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ଫେରନ୍ତୁ yosys_share_dirname;
}

std::string proc_program_prefix()
{
	ଫେରନ୍ତୁ YOSYS_PROGRAM_PREFIX;
}

ତାର୍କିକ fgetline(FILE *f, std::string &buffer)
{
	buffer = "";
	ଅକ୍ଷର block[4096];
	ଯେତେବେଳେ (1) {
		ଯଦି (fgets(block, 4096, f) == ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		buffer += block;
		ଯଦି (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r')) {
			ଯେତେବେଳେ (buffer.size() > 0 && (buffer[buffer.size()-1] == '\n' ||  buffer[buffer.size()-1] == '\r'))
				buffer.resize(buffer.size()-1);
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}
}

ସ୍ଥିର ଶୂନ୍ୟ handle_label(std::string &command, ତାର୍କିକ &from_to_active, ଅଚଳ std::string &run_from, ଅଚଳ std::string &run_to)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos = 0;
	std::string label;

	ଯେତେବେଳେ (pos < GetSize(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ଯଦି (pos < GetSize(command) && command[pos] == '#')
		ଫେରନ୍ତୁ;

	ଯେତେବେଳେ (pos < GetSize(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ଯଦି (GetSize(label) > 1 && label.back() == ':')
	{
		label = label.substr(0, GetSize(label)-1);
		command = command.substr(pos);

		ଯଦି (label == run_from)
			from_to_active = ସତ୍ୟ;
		ଅନ୍ୟଥା ଯଦି (label == run_to || (run_from == run_to && !run_from.empty()))
			from_to_active = ମିଥ୍ୟା;
	}
}

ତାର୍କିକ run_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_to_label)
{
	ଯଦି (design == nullptr)
		design = yosys_design;

	ଯଦି (command == "auto") {
	  std::string filename_trim = filename;

	  auto has_extension = [](ଅଚଳ std::string& filename, ଅଚଳ std::string& extension) {
	    ଯଦି (filename.size() >= extension.size()) {
	      ଫେରନ୍ତୁ filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
	    }
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
	  };

	  ଯଦି (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.size() - 3);
	  }

	  ଯଦି (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } ଅନ୍ୟଥା ଯଦି (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } ଅନ୍ୟଥା ଯଦି (filename == "-") {
	    command = "script";
	  } ଅନ୍ୟଥା {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ଯଦି (command == "script")
	{
		std::string run_from, run_to;
		ତାର୍କିକ from_to_active = ସତ୍ୟ;

		ଯଦି (from_to_label != ଖାଲି) {
			size_t pos = from_to_label->find(':');
			ଯଦି (pos == std::string::npos) {
				run_from = *from_to_label;
				run_to = *from_to_label;
			} ଅନ୍ୟଥା {
				run_from = from_to_label->substr(0, pos);
				run_to = from_to_label->substr(pos+1);
			}
			from_to_active = run_from.empty();
		}

		log("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ଯଦି (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_ନିବେଶ_files.ଭର୍ତ୍ତି(filename);
		}

		ଯଦି (f == ଖାଲି)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		ଚେଷ୍ଟା {
			std::string command;
			ଯେତେବେଳେ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ଯେତେବେଳେ (!command.empty() && command[command.size()-1] == '\\') {
					std::string next_ଧାଡ଼ି;
					ଯଦି (!fgetline(f, next_ଧାଡ଼ି))
						ଭାଙ୍ଗ;
					Frontend::current_script_lineno++;
					command.resize(command.size()-1);
					command += next_ଧାଡ଼ି;
				}
				handle_label(command, from_to_active, run_from, run_to);
				ଯଦି (from_to_active) {
					Pass::call(design, command);
					design->check();
				}
			}

			ଯଦି (!command.empty()) {
				handle_label(command, from_to_active, run_from, run_to);
				ଯଦି (from_to_active) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		ଧର (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ଫିଙ୍ଗ;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		ଯଦି (filename != "-")
			fclose(f);

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (command == "tcl") {
		Pass::call(design, vector<string>({command, filename}));
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	log("\n");
	ଯଦି (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} ଅନ୍ୟଥା {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ଯଦି (command[0] == ' ') {
		auto argv = split_tokens("read" + command);
		argv.ଠେଲ_back(filename);
		Pass::call(design, argv);
	} ଅନ୍ୟଥା
		Frontend::frontend_call(design, ଖାଲି, filename, command);

	design->check();
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ run_pass(std::string command, RTLIL::Design *design)
{
	ଯଦି (design == nullptr)
		design = yosys_design;

	log("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

ଶୂନ୍ୟ run_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ଯଦି (design == nullptr)
		design = yosys_design;

	ଯଦି (command == "auto") {
		ଯଦି (filename.size() > 2 && filename.compare(filename.size()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 4 && filename.compare(filename.size()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		ଅନ୍ୟଥା ଯଦି (filename.size() > 5 && filename.compare(filename.size()-5, std::string::npos, ".json") == 0)
			command = "json";
		ଅନ୍ୟଥା ଯଦି (filename == "-")
			command = "rtlil";
		ଅନ୍ୟଥା ଯଦି (filename.empty())
			ଫେରନ୍ତୁ;
		ଅନ୍ୟଥା
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ଯଦି (filename.empty())
		filename = "-";

	log("\n");
	ଯଦି (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} ଅନ୍ୟଥା {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ଖାଲି, filename, command);
}

#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ସ୍ଥିର ଅକ୍ଷର *readline_cmd_generator(ଅଚଳ ଅକ୍ଷର *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା state)
{
	ସ୍ଥିର std::map<std::string, Pass*>::iterator it;
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	ଯଦି (!state) {
		it = pass_register.begin();
		len = strlen(text);
	}

	ପାଇଁ (; it != pass_register.end(); it++) {
		ଯଦି (it->first.compare(0, len, text) == 0)
			ଫେରନ୍ତୁ strdup((it++)->first.c_str());
	}
	ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅକ୍ଷର *readline_obj_generator(ଅଚଳ ଅକ୍ଷର *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା state)
{
	ସ୍ଥିର std::vector<ଅକ୍ଷର*> obj_names;
	ସ୍ଥିର size_t idx;

	ଯଦି (!state)
	{
		idx = 0;
		obj_names.clear();

		RTLIL::Design *design = yosys_get_design();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(text);

		ଯଦି (design->selected_active_module.empty())
		{
			ପାଇଁ (auto mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ଯଦି (mod_name.compare(0, len, text) == 0)
					obj_names.ଠେଲ_back(strdup(mod_name.c_str()));
			}
		}
		ଅନ୍ୟଥା ଯଦି (design->module(design->selected_active_module) != nullptr)
		{
			RTLIL::Module *module = design->module(design->selected_active_module);

			ପାଇଁ (auto w : module->wires())
				ଯଦି (w->name.unescape().compare(0, len, text) == 0)
					obj_names.ଠେଲ_back(strdup(w->name.unescape().c_str()));

			ପାଇଁ (auto &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				ଯଦି (mem_name.compare(0, len, text) == 0)
					obj_names.ଠେଲ_back(strdup(mem_name.c_str()));
			}

			ପାଇଁ (auto cell : module->cells())
				ଯଦି (cell->name.unescape().compare(0, len, text) == 0)
					obj_names.ଠେଲ_back(strdup(cell->name.unescape().c_str()));

			ପାଇଁ (auto &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				ଯଦି (proc_name.compare(0, len, text) == 0)
					obj_names.ଠେଲ_back(strdup(proc_name.c_str()));
			}
		}

		std::sort(obj_names.begin(), obj_names.end());
	}

	ଯଦି (idx < obj_names.size())
		ଫେରନ୍ତୁ strdup(obj_names[idx++]);

	idx = 0;
	obj_names.clear();
	ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅକ୍ଷର **readline_completion(ଅଚଳ ଅକ୍ଷର *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
	ଯଦି (start == 0)
		ଫେରନ୍ତୁ rl_completion_matches(text, readline_cmd_generator);
	ଯଦି (strncmp(rl_ଧାଡ଼ି_buffer, "read_", 5) && strncmp(rl_ଧାଡ଼ି_buffer, "write_", 6))
		ଫେରନ୍ତୁ rl_completion_matches(text, readline_obj_generator);
	ଫେରନ୍ତୁ ଖାଲି;
}
#endif

ଶୂନ୍ୟ shell(RTLIL::Design *design)
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା recursion_counter = 0;

	recursion_counter++;
	auto guard = logger().error_throw_scope();

#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (ଅକ୍ଷର*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (ଅକ୍ଷର*)" \t\n";
#endif

	ଅକ୍ଷର *command = ଖାଲି;
#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ଯେତେବେଳେ ((command = readline(create_prompt(design, recursion_counter))) != ଖାଲି)
	{
#ଅନ୍ୟଥା
	ଅକ୍ଷର command_buffer[4096];
	ଯେତେବେଳେ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ଯଦି ((command = fgets(command_buffer, 4096, stdin)) == ଖାଲି)
			ଭାଙ୍ଗ;
#endif
		ଯଦି (command[strspn(command, " \t\r\n")] == 0) {
#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			ମୁକ୍ତ(command);
#endif
			ଜାରି;
		}
#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		add_history(command);
#endif

		ଅକ୍ଷର *p = command + strspn(command, " \t\r\n");
		ଯଦି (!strncmp(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ଯଦି (*p == 0)
				ଭାଙ୍ଗ;
		}

		ଚେଷ୍ଟା {
			log_assert(design->selection_ଗଦା.size() == 1);
			Pass::call(design, command);
		} ଧର (log_cmd_error_exception) {
			ଯେତେବେଳେ (design->selection_ଗଦା.size() > 1)
				design->କାଢ଼_selection();
			log_reset_ଗଦା();
		}
		design->check();
#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ଯଦି (command)
			ମୁକ୍ତ(command);
#endif
	}
	ଯଦି (command == ଖାଲି)
		printf("exit\n");
#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ଅନ୍ୟଥା
		ମୁକ୍ତ(command);
#endif
	recursion_counter--;
}

ସଂରଚନା ShellPass : ସର୍ବସାଧାରଣ Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	ଶୂନ୍ୟ help() override {
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
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, ମିଥ୍ୟା);
		shell(design);
	}
} ShellPass;

#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ସଂରଚନା HistoryPass : ସର୍ବସାଧାରଣ Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	ଶୂନ୍ୟ help() override {
		log("\n");
		log("    history\n");
		log("\n");
		log("This command prints all commands in the shell history buffer. This are\n");
		log("all commands executed in an interactive session, but not the commands\n");
		log("from executed scripts.\n");
		log("\n");
	}
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *design) override {
		extra_args(args, 1, design, ମିଥ୍ୟା);
#ifdef YOSYS_ENABLE_READLINE
		ପାଇଁ(HIST_ENTRY **list = history_list(); *list != ଖାଲି; list++)
			log("%s\n", (*list)->ଧାଡ଼ି);
#ଅନ୍ୟଥା
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = where_history(); history_get(i); i++)
			log("%s\n", history_get(i)->ଧାଡ଼ି);
#endif
	}
} HistoryPass;
#endif

ସଂରଚନା ScriptCmdPass : ସର୍ବସାଧାରଣ Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	ଶୂନ୍ୟ help() override {
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
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		ତାର୍କିକ scriptwire = ମିଥ୍ୟା;

		size_t argidx;
		ପାଇଁ (argidx = 1; argidx < args.size(); argidx++) {
			ଯଦି (args[argidx] == "-scriptwire") {
				scriptwire = ସତ୍ୟ;
				ଜାରି;
			}
			ଭାଙ୍ଗ;
		}
		ଯଦି (scriptwire) {
			extra_args(args, argidx, design);

			ପାଇଁ (auto mod : design->selected_modules())
				ପାଇଁ (auto &c : mod->connections()) {
					ଯଦି (!c.first.is_wire())
						ଜାରି;
					auto w = c.first.as_wire();
					ଯଦି (!mod->selected(w))
						ଜାରି;
					ଯଦି (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					auto v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		ଅନ୍ୟଥା ଯଦି (args.size() < 2)
			log_cmd_error("Missing script file.\n");
		ଅନ୍ୟଥା ଯଦି (args.size() == 2)
			run_frontend(args[1], "script", design);
		ଅନ୍ୟଥା ଯଦି (args.size() == 3)
			run_frontend(args[1], "script", design, &args[2]);
		ଅନ୍ୟଥା
			extra_args(args, 2, design, ମିଥ୍ୟା);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_END
