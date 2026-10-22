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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
#  include <tcl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
ਨਾਮਾਕਾਸ਼ py = pybind11;
#ਜੇ_ਅੰਤ

#ਜੇ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#ਜਾਂਜੇ defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#ਨਹੀਂਤਾਂ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ਜੇ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
ਬਾਹਰੀ "C" PyObject* PyInit_libyosys();
ਬਾਹਰੀ "C" PyObject* PyInit_pyosys();
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
ਪੂਰਨ yosys_xtrace = 0;
ਤਾਰਕਿਕ yosys_write_versions = ਸੱਚ;
ਅਚਲ ਅੱਖਰ* yosys_maybe_ਸੰਸਕਰਣ() {
	ਜੇ (yosys_write_versions)
		ਵਾਪਸ yosys_ਸੰਸਕਰਣ_str;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ "Yosys";
}

RTLIL::Design *yosys_design = ਨਿਰਾ;
NewCellTypes yosys_celltypes;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ਨਿਰਾ;
Tcl_Interp *yosys_sdc_interp = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

std::ਸੈੱਟ<std::string> yosys_ਨਿਵੇਸ਼_files, yosys_output_files;

ਤਾਰਕਿਕ memhasher_ਸਰਗਰਮ = ਝੂਠ;
uint32_t memhasher_rng = 123456;
std::ਸਦਿਸ਼<ਖਾਲੀ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

ਤਾਰਕਿਕ Multithreading::ਸਰਗਰਮ_2 = ਝੂਠ;

ਖਾਲੀ init_share_dirname();
ਖਾਲੀ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!ਸਰਗਰਮ_2);
	ਸਰਗਰਮ_2 = ਸੱਚ;
}

Multithreading::~Multithreading() {
	log_assert(ਸਰਗਰਮ_2);
	ਸਰਗਰਮ_2 = ਝੂਠ;
}

ਖਾਲੀ Autoidx::ensure_at_ਘੱਟ_ਤੋਂ_ਘੱਟ(ਪੂਰਨ v) {
	log_assert(!Multithreading::ਸਰਗਰਮ());
	ਮੁੱਲ = std::max(ਮੁੱਲ, v);
}

ਪੂਰਨ Autoidx::ਸੰਚਾਲਕ++(ਪੂਰਨ) {
	log_assert(!Multithreading::ਸਰਗਰਮ());
	ਵਾਪਸ ਮੁੱਲ++;
}

ਖਾਲੀ memhasher_on()
{
#ਜੇ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += ਸਮਾਂ_2(ਨਿਰਾ) << 16 ^ getpid();
#ਜੇ_ਅੰਤ
	memhasher_store.resize(0x10000);
	memhasher_ਸਰਗਰਮ = ਸੱਚ;
}

ਖਾਲੀ memhasher_off()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ p : memhasher_store)
		ਜੇ (p) ਮੁਕਤ(p);
	memhasher_store.clear();
	memhasher_ਸਰਗਰਮ = ਝੂਠ;
}

ਖਾਲੀ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	ਪੂਰਨ ਮਾਪ, ਸੂਚਕਾਂਕ = (memhasher_rng >> 4) & 0xffff;
	ਚੋਣ (memhasher_rng & 7) {
		ਹਾਲਤ 0: ਮਾਪ =   16; ਰੋਕੋ;
		ਹਾਲਤ 1: ਮਾਪ =  256; ਰੋਕੋ;
		ਹਾਲਤ 2: ਮਾਪ = 1024; ਰੋਕੋ;
		ਹਾਲਤ 3: ਮਾਪ = 4096; ਰੋਕੋ;
		ਮੂਲ: ਮਾਪ = 0;
	}
	ਜੇ (ਸੂਚਕਾਂਕ < 16) ਮਾਪ *= 16;
	memhasher_store[ਸੂਚਕਾਂਕ] = realloc(memhasher_store[ਸੂਚਕਾਂਕ], ਮਾਪ);
}

ਖਾਲੀ yosys_banner()
{
	ਲਘੂਗਣਕ("\n");
	ਲਘੂਗਣਕ(" /----------------------------------------------------------------------------\\\n");
	ਲਘੂਗਣਕ(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	ਲਘੂਗਣਕ(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	ਲਘੂਗਣਕ(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	ਲਘੂਗਣਕ(" \\----------------------------------------------------------------------------/\n");
	ਲਘੂਗਣਕ(" %s\n", yosys_maybe_ਸੰਸਕਰਣ());
}

#ਜੇ defined(YOSYS_ENABLE_SPAWN)
ਪੂਰਨ ਲੜੀਵਾਰ_command(ਅਚਲ std::string &command, std::function<ਖਾਲੀ(ਅਚਲ std::string&)> process_ਸਤਰ)
{
	ਜੇ (!process_ਸਤਰ)
		ਵਾਪਸ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ਜੇ (f == ਖਾਲੀ_ਸੂਚਕ)
		ਵਾਪਸ -1;

	std::string ਸਤਰ;
	ਅੱਖਰ logbuf[128];
	ਜਦੋਂ (ਸੱਚ) {
		ਜੇ (fgets(logbuf, 128, f) != ਨਿਰਾ) {
			ਸਤਰ += logbuf;
			ਜੇ (!ਸਤਰ.ਖਾਲੀ_ਥਾਵਾਂ() && ਸਤਰ.ਪਿੱਠ() == '\n')
				process_ਸਤਰ(ਸਤਰ), ਸਤਰ.clear();
		} ਨਹੀਂਤਾਂ ਜੇ (ferror(f) && errno == EINTR) {
			clearerr(f);
			ਜਾਰੀ;
		} ਨਹੀਂਤਾਂ {
			ਰੋਕੋ;
		}
	}
	ਜੇ (!ਸਤਰ.ਖਾਲੀ_ਥਾਵਾਂ())
		process_ਸਤਰ(ਸਤਰ);

	ਪੂਰਨ ret = pclose(f);
	ਜੇ (ret < 0)
		ਵਾਪਸ -2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
	ਵਾਪਸ ret;
#ਨਹੀਂਤਾਂ
	ਜੇ (!WIFEXITED(ret))
		ਵਾਪਸ -1;
	ਵਾਪਸ WEXITSTATUS(ret);
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ already_ਤਿਆਰੀ = ਝੂਠ;
ਤਾਰਕਿਕ already_shutdown = ਝੂਠ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.ਜੋੜੋ_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(ਖਾਲੀ)_;
	ਸੁੱਟੋ py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ yosys_ਤਿਆਰੀ()
{
	ਜੇ(already_ਤਿਆਰੀ)
		ਵਾਪਸ;
	already_ਤਿਆਰੀ = ਸੱਚ;
	already_shutdown = ਝੂਠ;

	StaticTwines::init();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ਜੇ (!Py_IsInitialized()) {
		PyImport_AppendInittab((ਅੱਖਰ*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((ਅੱਖਰ*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((ਅੱਖਰ*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_ਲੜੀਵਾਰ_Simple_String("import sys");
		ਸੰਕੇਤ(SIGINT, SIG_DFL);
	}
#ਜੇ_ਅੰਤ

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_ਰਜਿਸਟਰ();
	yosys_design = ਨਵਾਂ RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_ਗਿਆਤ;
	log_ਧੱਕੋ();
}

ਤਾਰਕਿਕ yosys_already_ਤਿਆਰੀ()
{
	ਵਾਪਸ already_ਤਿਆਰੀ;
}

ਖਾਲੀ yosys_shutdown()
{
	ਜੇ(already_shutdown)
		ਵਾਪਸ;
	already_ਤਿਆਰੀ = ਝੂਠ;
	already_shutdown = ਸੱਚ;
	log_ਕੱਢੋ();

	Pass::ਪੂਰਾ_ਰਜਿਸਟਰ();

	ਮਿਟਾਓ yosys_design;
	yosys_design = ਨਿਰਾ;

	logger().clear();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
	ਜੇ (yosys_tcl_interp != ਨਿਰਾ) {
		ਜੇ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PLUGINS
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : loaded_plugins)
		dlclose(it.ਦੂਜਾ);

	loaded_plugins.clear();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#ਜੇ_ਅੰਤ
	loaded_plugin_aliases.clear();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	Py_Finalize();
#ਜੇ_ਅੰਤ
}

ਅਚਲ std::string *create_id_ਅਗੇਤਰ(std::string_view file, ਪੂਰਨ ਸਤਰ, std::string_view func)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
	ਮਾਪ_t pos = file.ਲੱਭੋ_ਆਖਰੀ_ਦਾ("/\\");
#ਨਹੀਂਤਾਂ
	ਮਾਪ_t pos = file.ਲੱਭੋ_ਆਖਰੀ_ਦਾ('/');
#ਜੇ_ਅੰਤ
	ਜੇ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.ਲੱਭੋ_ਆਖਰੀ_ਦਾ(':');
	ਜੇ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ਵਾਪਸ ਨਵਾਂ std::string(stringf("$auto$%s:%d:%s$", file, ਸਤਰ, func));
}

RTLIL::Design *yosys_get_design()
{
	ਵਾਪਸ yosys_design;
}

ਅਚਲ ਅੱਖਰ *create_prompt(RTLIL::Design *design, ਪੂਰਨ recursion_counter, ਤਾਰਕਿਕ newline)
{
	ਸਥਿਰ ਅੱਖਰ ਬਫਰ[100];
	std::string str = newline ? "\n" : "";
	ਜੇ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ਜੇ (!design->selected_ਸਰਗਰਮ_module.ਖਾਲੀ_ਥਾਵਾਂ())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_ਸਰਗਰਮ_module)).c_str());
	ਜੇ (!design->ਭਰਿਆ_selection()) {
		ਜੇ (design->selected_ਸਰਗਰਮ_module.ਖਾਲੀ_ਥਾਵਾਂ())
			str += "*";
		ਨਹੀਂਤਾਂ ਜੇ (design->selection().selected_modules.ਮਾਪ() != 1 || design->selection().selected_members.ਮਾਪ() != 0 ||
				design->selection().selected_modules.ਗਿਣਤੀ_2(design->selected_ਸਰਗਰਮ_module) == 0)
			str += "*";
	}
	snprintf(ਬਫਰ, 100, "%s> ", str.c_str());
	ਵਾਪਸ ਬਫਰ;
}

std::ਸਦਿਸ਼<std::string> glob_filename(ਅਚਲ std::string &filename_ਨਮੂਨਾ)
{
	std::ਸਦਿਸ਼<std::string> results;

#ਜੇ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ਧੱਕੋ_ਪਿੱਠ(filename_ਨਮੂਨਾ);
#ਨਹੀਂਤਾਂ
	glob_t globbuf;

	ਪੂਰਨ err = glob(filename_ਨਮੂਨਾ.c_str(), 0, ਨਿਰਾ, &globbuf);

	ਜੇ(err == 0) {
		ਲਈ (ਮਾਪ_t i = 0; i < globbuf.gl_pathc; i++)
			results.ਧੱਕੋ_ਪਿੱਠ(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} ਨਹੀਂਤਾਂ {
		results.ਧੱਕੋ_ਪਿੱਠ(filename_ਨਮੂਨਾ);
	}
#ਜੇ_ਅੰਤ

	ਵਾਪਸ results;
}

ਖਾਲੀ rewrite_filename(std::string &filename)
{
	ਜੇ (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_ਮਾਪ(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_ਮਾਪ(filename)-2);
	ਜੇ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32
	ਜੇ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL

// defined in tclapi.cc
ਬਾਹਰੀ ਪੂਰਨ yosys_tcl_interp_init(Tcl_Interp *interp);

ਬਾਹਰੀ Tcl_Interp *yosys_get_tcl_interp()
{
	ਜੇ (yosys_tcl_interp == ਨਿਰਾ) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ਵਾਪਸ yosys_tcl_interp;
}

// Also see SdcPass
ਬਣਤਰ TclPass : ਜਨਤਕ Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	ਖਾਲੀ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    tcl <filename> [args]\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("This command executes the tcl commands in the specified file.\n");
		ਲਘੂਗਣਕ("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("The tcl command 'yosys -import' can be used to import all yosys\n");
		ਲਘੂਗਣਕ("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		ਲਘੂਗਣਕ("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		ਲਘੂਗਣਕ("in order to avoid a name collision with the built in commands.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("If any arguments are specified, these arguments are provided to the script via\n");
		ਲਘੂਗਣਕ("the standard $argc and $argv variables.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("Note, tcl will not receive the output of any yosys command. If the output\n");
		ਲਘੂਗਣਕ("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		ਲਘੂਗਣਕ("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		ਲਘੂਗਣਕ("The 'result.string' value is then used as the tcl output value of the command.\n");
		ਲਘੂਗਣਕ("\n");
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *) override {
		ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() < 2)
			log_cmd_error("Missing script file.\n");

		std::ਸਦਿਸ਼<Tcl_Obj*> script_ਦਲੀਲਾਂ;
		ਲਈ (ਸਵੈਚਾਲਿਤ it = ਦਲੀਲਾਂ_2.begin() + 2; it != ਦਲੀਲਾਂ_2.ਸਿਰਾ(); ++it)
			script_ਦਲੀਲਾਂ.ਧੱਕੋ_ਪਿੱਠ(Tcl_NewStringObj((*it).c_str(), (*it).ਮਾਪ()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_ਸੈੱਟ_Var2(interp, Tcl_NewStringObj("argc", 4), ਨਿਰਾ, Tcl_NewIntObj(script_ਦਲੀਲਾਂ.ਮਾਪ()), 0);
		Tcl_Obj_ਸੈੱਟ_Var2(interp, Tcl_NewStringObj("argv", 4), ਨਿਰਾ, Tcl_NewListObj(script_ਦਲੀਲਾਂ.ਮਾਪ(), script_ਦਲੀਲਾਂ.ਅੰਕੜੇ()), 0);
		Tcl_Obj_ਸੈੱਟ_Var2(interp, Tcl_NewStringObj("argv0", 5), ਨਿਰਾ, Tcl_NewStringObj(ਦਲੀਲਾਂ_2[1].c_str(), ਦਲੀਲਾਂ_2[1].ਮਾਪ()), 0);
		ਜੇ (Tcl_EvalFile(interp, ਦਲੀਲਾਂ_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ਨਤੀਜਾ(interp));
		Tcl_Release(interp);
	}
} TclPass;

#ਜੇ_ਅੰਤ

#ਜੇ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string ਰਸਤਾ_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	ਲਈ (ਪੂਰਨ tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", ਰਸਤਾ_2.ਅੰਕੜੇ(), ਰਸਤਾ_2.ਮਾਪ());
		ਜੇ (buflen < (ssize_t)ਰਸਤਾ_2.ਮਾਪ())
			ਰੋਕੋ;
		ਨਹੀਂਤਾਂ
			ਰਸਤਾ_2.resize(ਰਸਤਾ_2.ਮਾਪ() * 2);
	}
	ਜੇ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		ਰਸਤਾ_2.resize(0);
	} ਨਹੀਂਤਾਂ {
		ਜਦੋਂ (buflen > 0 && ਰਸਤਾ_2[buflen-1] != '/')
			buflen--;
		ਰਸਤਾ_2.resize(buflen);
	}
	ਵਾਪਸ ਰਸਤਾ_2;
}
#ਜਾਂਜੇ defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __NetBSD__
	ਪੂਰਨ mib[4] = {CTL_KERN, KERN_PROC_ਦਲੀਲਾਂ, getpid(), KERN_PROC_PATHNAME};
#ਨਹੀਂਤਾਂ
	ਪੂਰਨ mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#ਜੇ_ਅੰਤ
	ਮਾਪ_t buflen;
	ਅੱਖਰ *ਬਫਰ;
	std::string ਰਸਤਾ_2;
	ਜੇ (sysctl(mib, 4, ਨਿਰਾ, &buflen, ਨਿਰਾ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ਬਫਰ = (ਅੱਖਰ*)ਸਮ੍ਰਿਤੀ_ਦਿਓ(buflen);
	ਜੇ (ਬਫਰ == ਨਿਰਾ)
		log_error("malloc failed: %s\n", strerror(errno));
	ਜੇ (sysctl(mib, 4, ਬਫਰ, &buflen, ਨਿਰਾ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ਜਦੋਂ (buflen > 0 && ਬਫਰ[buflen-1] != '/')
		buflen--;
	ਰਸਤਾ_2.ਸੌਂਪੋ(ਬਫਰ, buflen);
	ਮੁਕਤ(ਬਫਰ);
	ਵਾਪਸ ਰਸਤਾ_2;
}
#ਜਾਂਜੇ defined(__APPLE__)
std::string proc_self_dirname()
{
	ਅੱਖਰ *ਰਸਤਾ_2 = ਨਿਰਾ;
	uint32_t buflen = 0;
	ਜਦੋਂ (NSGet_Executable_ਰਸਤਾ(ਰਸਤਾ_2, &buflen) != 0)
		ਰਸਤਾ_2 = (ਅੱਖਰ *) realloc((ਖਾਲੀ *) ਰਸਤਾ_2, buflen);
	ਅੱਖਰ resolved[ਰਸਤਾ_MAX];
	ਜੇ (realpath(ਰਸਤਾ_2, resolved) != ਨਿਰਾ) {
		ਮੁਕਤ(ਰਸਤਾ_2);
		ਰਸਤਾ_2 = strdup(resolved);
		buflen = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ_2);
	}
	ਜਦੋਂ (buflen > 0 && ਰਸਤਾ_2[buflen-1] != '/')
		buflen--;
	std::string str(ਰਸਤਾ_2, buflen);
	ਮੁਕਤ(ਰਸਤਾ_2);
	ਵਾਪਸ str;
}
#ਜਾਂਜੇ defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ਜੇ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.ਮਾਪ()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, wbinpath.ਅੰਕੜੇ(), wbinpath.ਮਾਪ(), ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ));
	ਜੇ (Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, wbinpath.ਅੰਕੜੇ(), wbinpath.ਮਾਪ(), &ubinpath[0], ubinpath.ਮਾਪ(), ਨਿਰਾ, ਨਿਰਾ) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ਵਾਪਸ ubinpath;

}
#ਜਾਂਜੇ defined(__wasm)
std::string proc_self_dirname()
{
	ਵਾਪਸ "/";
}
#ਜਾਂਜੇ defined(__OpenBSD__) || defined(__HAIKU__)
ਅੱਖਰ yosys_ਰਸਤਾ[ਰਸਤਾ_MAX];
ਅੱਖਰ *yosys_argv0;

std::string proc_self_dirname(ਖਾਲੀ)
{
	ਅੱਖਰ buf[ਰਸਤਾ_MAX + 1] = "", *ਰਸਤਾ_2, *p;
	// if case argv[0] contains a valid path, return it
	ਜੇ (ਲੜੀ_ਲੰਬਾਈ(yosys_ਰਸਤਾ) > 0) {
		p = strrchr(yosys_ਰਸਤਾ, '/');
		snprintf(buf, ਆਕਾਰ buf, "%*s/", (ਪੂਰਨ)(yosys_ਰਸਤਾ - p), yosys_ਰਸਤਾ);
		ਵਾਪਸ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	ਰਸਤਾ_2 = strdup(getenv("PATH"));
	ਜੇ (!ਰਸਤਾ_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	ਲਈ (p = strtok(ਰਸਤਾ_2, ":"); p; p = strtok(ਨਿਰਾ, ":")) {
		snprintf(buf, ਆਕਾਰ buf, "%s/%s", p, yosys_argv0);
		ਜੇ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			ਮੁਕਤ(ਰਸਤਾ_2);
			ਵਾਪਸ buf;
		}
	}
	ਮੁਕਤ(ਰਸਤਾ_2);
	log_error("Can't determine yosys executable path.\n");
	ਵਾਪਸ ਨਿਰਾ;
}
#ਨਹੀਂਤਾਂ
	#ਗਲਤੀ "Don't know how to determine process executable base path!"
#ਜੇ_ਅੰਤ

#ਜੇ defined(__wasm)
ਖਾਲੀ init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#ਨਹੀਂਤਾਂ
ਖਾਲੀ init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ਜੇ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_ਰਸਤਾ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ਅਚਲ ਅੱਖਰ *share_ਰਸਤਾ = PyUnicode_AsUTF8(share_ਰਸਤਾ_obj);
		yosys_share_dirname = std::string(share_ਰਸਤਾ);
		ਵਾਪਸ;
	}
#  endif
	std::string proc_self_ਰਸਤਾ = proc_self_dirname();
#  ਜੇ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_ਰਸਤਾ = proc_self_ਰਸਤਾ + "share\\";
	ਜੇ (check_directory_exists(proc_share_ਰਸਤਾ, ਸੱਚ)) {
		yosys_share_dirname = proc_share_ਰਸਤਾ;
		ਵਾਪਸ;
	}
	proc_share_ਰਸਤਾ = proc_self_ਰਸਤਾ + "..\\share\\";
	ਜੇ (check_directory_exists(proc_share_ਰਸਤਾ, ਸੱਚ)) {
		yosys_share_dirname = proc_share_ਰਸਤਾ;
		ਵਾਪਸ;
	}
#  ਨਹੀਂਤਾਂ
	std::string proc_share_ਰਸਤਾ = proc_self_ਰਸਤਾ + "share/";
	ਜੇ (check_directory_exists(proc_share_ਰਸਤਾ, ਸੱਚ)) {
		yosys_share_dirname = proc_share_ਰਸਤਾ;
		ਵਾਪਸ;
	}
	proc_share_ਰਸਤਾ = proc_self_ਰਸਤਾ + "../share/" + proc_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ()+ "yosys/";
	ਜੇ (check_directory_exists(proc_share_ਰਸਤਾ, ਸੱਚ)) {
		yosys_share_dirname = proc_share_ਰਸਤਾ;
		ਵਾਪਸ;
	}
	proc_share_ਰਸਤਾ = YOSYS_DATDIR "/";
	ਜੇ (check_directory_exists(proc_share_ਰਸਤਾ, ਸੱਚ)) {
		yosys_share_dirname = proc_share_ਰਸਤਾ;
		ਵਾਪਸ;
	}
#  endif
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ init_abc_executable_name()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ABCEXTERNAL
	std::string exe_file;
	ਜੇ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} ਨਹੀਂਤਾਂ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#ਨਹੀਂਤਾਂ
	yosys_abc_executable = proc_self_dirname() + proc_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ()+ "yosys-abc";
#  ifdef _WIN32
	ਜੇ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ਜੇ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_ਰਸਤਾ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ਅਚਲ ਅੱਖਰ *abc_ਰਸਤਾ = PyUnicode_AsUTF8(abc_ਰਸਤਾ_obj);
		yosys_abc_executable = std::string(abc_ਰਸਤਾ);
	}
#  endif
#ਜੇ_ਅੰਤ
}

std::string proc_share_dirname()
{
	ਜੇ (yosys_share_dirname.ਖਾਲੀ_ਥਾਵਾਂ())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ਵਾਪਸ yosys_share_dirname;
}

std::string proc_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ()
{
	ਵਾਪਸ YOSYS_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ;
}

ਤਾਰਕਿਕ fgetline(FILE *f, std::string &ਬਫਰ)
{
	ਬਫਰ = "";
	ਅੱਖਰ ਖੰਡ[4096];
	ਜਦੋਂ (1) {
		ਜੇ (fgets(ਖੰਡ, 4096, f) == ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;
		ਬਫਰ += ਖੰਡ;
		ਜੇ (ਬਫਰ.ਮਾਪ() > 0 && (ਬਫਰ[ਬਫਰ.ਮਾਪ()-1] == '\n' ||  ਬਫਰ[ਬਫਰ.ਮਾਪ()-1] == '\r')) {
			ਜਦੋਂ (ਬਫਰ.ਮਾਪ() > 0 && (ਬਫਰ[ਬਫਰ.ਮਾਪ()-1] == '\n' ||  ਬਫਰ[ਬਫਰ.ਮਾਪ()-1] == '\r'))
				ਬਫਰ.resize(ਬਫਰ.ਮਾਪ()-1);
			ਵਾਪਸ ਸੱਚ;
		}
	}
}

ਸਥਿਰ ਖਾਲੀ handle_label(std::string &command, ਤਾਰਕਿਕ &from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ, ਅਚਲ std::string &ਲੜੀਵਾਰ_from, ਅਚਲ std::string &ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ)
{
	ਪੂਰਨ pos = 0;
	std::string label;

	ਜਦੋਂ (pos < Get_ਮਾਪ(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ਜੇ (pos < Get_ਮਾਪ(command) && command[pos] == '#')
		ਵਾਪਸ;

	ਜਦੋਂ (pos < Get_ਮਾਪ(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ਜੇ (Get_ਮਾਪ(label) > 1 && label.ਪਿੱਠ() == ':')
	{
		label = label.substr(0, Get_ਮਾਪ(label)-1);
		command = command.substr(pos);

		ਜੇ (label == ਲੜੀਵਾਰ_from)
			from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ = ਸੱਚ;
		ਨਹੀਂਤਾਂ ਜੇ (label == ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ || (ਲੜੀਵਾਰ_from == ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ && !ਲੜੀਵਾਰ_from.ਖਾਲੀ_ਥਾਵਾਂ()))
			from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ = ਝੂਠ;
	}
}

ਤਾਰਕਿਕ ਲੜੀਵਾਰ_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_ਮੰਜ਼ਿਲ_label)
{
	ਜੇ (design == ਖਾਲੀ_ਸੂਚਕ)
		design = yosys_design;

	ਜੇ (command == "auto") {
	  std::string filename_trim = filename;

	  ਸਵੈਚਾਲਿਤ has_extension = [](ਅਚਲ std::string& filename, ਅਚਲ std::string& extension) {
	    ਜੇ (filename.ਮਾਪ() >= extension.ਮਾਪ()) {
	      ਵਾਪਸ filename.compare(filename.ਮਾਪ() - extension.ਮਾਪ(), extension.ਮਾਪ(), extension) == 0;
	    }
	    ਵਾਪਸ ਝੂਠ;
	  };

	  ਜੇ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.ਮਾਪ() - 3);
	  }

	  ਜੇ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } ਨਹੀਂਤਾਂ ਜੇ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } ਨਹੀਂਤਾਂ ਜੇ (filename == "-") {
	    command = "script";
	  } ਨਹੀਂਤਾਂ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ਜੇ (command == "script")
	{
		std::string ਲੜੀਵਾਰ_from, ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ;
		ਤਾਰਕਿਕ from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ = ਸੱਚ;

		ਜੇ (from_ਮੰਜ਼ਿਲ_label != ਨਿਰਾ) {
			ਮਾਪ_t pos = from_ਮੰਜ਼ਿਲ_label->ਲੱਭੋ(':');
			ਜੇ (pos == std::string::npos) {
				ਲੜੀਵਾਰ_from = *from_ਮੰਜ਼ਿਲ_label;
				ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ = *from_ਮੰਜ਼ਿਲ_label;
			} ਨਹੀਂਤਾਂ {
				ਲੜੀਵਾਰ_from = from_ਮੰਜ਼ਿਲ_label->substr(0, pos);
				ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ = from_ਮੰਜ਼ਿਲ_label->substr(pos+1);
			}
			from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ = ਲੜੀਵਾਰ_from.ਖਾਲੀ_ਥਾਵਾਂ();
		}

		ਲਘੂਗਣਕ("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ਜੇ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_ਨਿਵੇਸ਼_files.ਪਾਓ(filename);
		}

		ਜੇ (f == ਨਿਰਾ)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		ਪੂਰਨ backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		ਕੋਸ਼ਿਸ਼ {
			std::string command;
			ਜਦੋਂ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ਜਦੋਂ (!command.ਖਾਲੀ_ਥਾਵਾਂ() && command[command.ਮਾਪ()-1] == '\\') {
					std::string ਅਗਲਾ_ਸਤਰ;
					ਜੇ (!fgetline(f, ਅਗਲਾ_ਸਤਰ))
						ਰੋਕੋ;
					Frontend::current_script_lineno++;
					command.resize(command.ਮਾਪ()-1);
					command += ਅਗਲਾ_ਸਤਰ;
				}
				handle_label(command, from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ, ਲੜੀਵਾਰ_from, ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ);
				ਜੇ (from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ) {
					Pass::call(design, command);
					design->check();
				}
			}

			ਜੇ (!command.ਖਾਲੀ_ਥਾਵਾਂ()) {
				handle_label(command, from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ, ਲੜੀਵਾਰ_from, ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ);
				ਜੇ (from_ਮੰਜ਼ਿਲ_ਸਰਗਰਮ) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		ਫੜੋ (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ਸੁੱਟੋ;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		ਜੇ (filename != "-")
			fclose(f);

		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (command == "tcl") {
		Pass::call(design, ਸਦਿਸ਼<string>({command, filename}));
		ਵਾਪਸ ਸੱਚ;
	}

	ਲਘੂਗਣਕ("\n");
	ਜੇ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} ਨਹੀਂਤਾਂ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ਜੇ (command[0] == ' ') {
		ਸਵੈਚਾਲਿਤ argv = ਵੰਡੋ_ਟੋਕਨ("read" + command);
		argv.ਧੱਕੋ_ਪਿੱਠ(filename);
		Pass::call(design, argv);
	} ਨਹੀਂਤਾਂ
		Frontend::frontend_call(design, ਨਿਰਾ, filename, command);

	design->check();
	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ ਲੜੀਵਾਰ_pass(std::string command, RTLIL::Design *design)
{
	ਜੇ (design == ਖਾਲੀ_ਸੂਚਕ)
		design = yosys_design;

	ਲਘੂਗਣਕ("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

ਖਾਲੀ ਲੜੀਵਾਰ_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ਜੇ (design == ਖਾਲੀ_ਸੂਚਕ)
		design = yosys_design;

	ਜੇ (command == "auto") {
		ਜੇ (filename.ਮਾਪ() > 2 && filename.compare(filename.ਮਾਪ()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 3 && filename.compare(filename.ਮਾਪ()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 3 && filename.compare(filename.ਮਾਪ()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 3 && filename.compare(filename.ਮਾਪ()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 4 && filename.compare(filename.ਮਾਪ()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 5 && filename.compare(filename.ਮਾਪ()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 5 && filename.compare(filename.ਮਾਪ()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਮਾਪ() > 5 && filename.compare(filename.ਮਾਪ()-5, std::string::npos, ".json") == 0)
			command = "json";
		ਨਹੀਂਤਾਂ ਜੇ (filename == "-")
			command = "rtlil";
		ਨਹੀਂਤਾਂ ਜੇ (filename.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ;
		ਨਹੀਂਤਾਂ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ਜੇ (filename.ਖਾਲੀ_ਥਾਵਾਂ())
		filename = "-";

	ਲਘੂਗਣਕ("\n");
	ਜੇ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} ਨਹੀਂਤਾਂ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ਨਿਰਾ, filename, command);
}

#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ਸਥਿਰ ਅੱਖਰ *readline_cmd_generator(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਪੂਰਨ state)
{
	ਸਥਿਰ std::map<std::string, Pass*>::iterator it;
	ਸਥਿਰ ਪੂਰਨ len;

	ਜੇ (!state) {
		it = pass_ਰਜਿਸਟਰ.begin();
		len = ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ);
	}

	ਲਈ (; it != pass_ਰਜਿਸਟਰ.ਸਿਰਾ(); it++) {
		ਜੇ (it->ਪਹਿਲਾ.compare(0, len, ਪਾਠ) == 0)
			ਵਾਪਸ strdup((it++)->ਪਹਿਲਾ.c_str());
	}
	ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅੱਖਰ *readline_obj_generator(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਪੂਰਨ state)
{
	ਸਥਿਰ std::ਸਦਿਸ਼<ਅੱਖਰ*> obj_ਨਾਂ;
	ਸਥਿਰ ਮਾਪ_t idx;

	ਜੇ (!state)
	{
		idx = 0;
		obj_ਨਾਂ.clear();

		RTLIL::Design *design = yosys_get_design();
		ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ);

		ਜੇ (design->selected_ਸਰਗਰਮ_module.ਖਾਲੀ_ਥਾਵਾਂ())
		{
			ਲਈ (ਸਵੈਚਾਲਿਤ mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ਜੇ (mod_name.compare(0, len, ਪਾਠ) == 0)
					obj_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ(strdup(mod_name.c_str()));
			}
		}
		ਨਹੀਂਤਾਂ ਜੇ (design->module(design->selected_ਸਰਗਰਮ_module) != ਖਾਲੀ_ਸੂਚਕ)
		{
			RTLIL::Module *module = design->module(design->selected_ਸਰਗਰਮ_module);

			ਲਈ (ਸਵੈਚਾਲਿਤ w : module->wires())
				ਜੇ (w->name.unescape().compare(0, len, ਪਾਠ) == 0)
					obj_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ(strdup(w->name.unescape().c_str()));

			ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.ਪਹਿਲਾ);
				ਜੇ (mem_name.compare(0, len, ਪਾਠ) == 0)
					obj_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ(strdup(mem_name.c_str()));
			}

			ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells())
				ਜੇ (cell->name.unescape().compare(0, len, ਪਾਠ) == 0)
					obj_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ(strdup(cell->name.unescape().c_str()));

			ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.ਪਹਿਲਾ);
				ਜੇ (proc_name.compare(0, len, ਪਾਠ) == 0)
					obj_ਨਾਂ.ਧੱਕੋ_ਪਿੱਠ(strdup(proc_name.c_str()));
			}
		}

		std::ਛਾਂਟੋ(obj_ਨਾਂ.begin(), obj_ਨਾਂ.ਸਿਰਾ());
	}

	ਜੇ (idx < obj_ਨਾਂ.ਮਾਪ())
		ਵਾਪਸ strdup(obj_ਨਾਂ[idx++]);

	idx = 0;
	obj_ਨਾਂ.clear();
	ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅੱਖਰ **readline_completion(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ)
{
	ਜੇ (ਸ਼ੁਰੂ == 0)
		ਵਾਪਸ rl_completion_matches(ਪਾਠ, readline_cmd_generator);
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ(rl_ਸਤਰ_ਬਫਰ, "read_", 5) && ਲੜੀ_ਤੁਲਨਾ(rl_ਸਤਰ_ਬਫਰ, "write_", 6))
		ਵਾਪਸ rl_completion_matches(ਪਾਠ, readline_obj_generator);
	ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ shell(RTLIL::Design *design)
{
	ਸਥਿਰ ਪੂਰਨ recursion_counter = 0;

	recursion_counter++;
	ਸਵੈਚਾਲਿਤ guard = logger().error_throw_scope();

#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (ਅੱਖਰ*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_ਸ਼ਬਦ_break_characters = (ਅੱਖਰ*)" \t\n";
#ਜੇ_ਅੰਤ

	ਅੱਖਰ *command = ਨਿਰਾ;
#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ਜਦੋਂ ((command = readline(create_prompt(design, recursion_counter))) != ਨਿਰਾ)
	{
#ਨਹੀਂਤਾਂ
	ਅੱਖਰ command_ਬਫਰ[4096];
	ਜਦੋਂ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ਜੇ ((command = fgets(command_ਬਫਰ, 4096, stdin)) == ਨਿਰਾ)
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਜੇ (command[strspn(command, " \t\r\n")] == 0) {
#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			ਮੁਕਤ(command);
#ਜੇ_ਅੰਤ
			ਜਾਰੀ;
		}
#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ਜੋੜੋ_history(command);
#ਜੇ_ਅੰਤ

		ਅੱਖਰ *p = command + strspn(command, " \t\r\n");
		ਜੇ (!ਲੜੀ_ਤੁਲਨਾ(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ਜੇ (*p == 0)
				ਰੋਕੋ;
		}

		ਕੋਸ਼ਿਸ਼ {
			log_assert(design->selection_ਢੇਰ.ਮਾਪ() == 1);
			Pass::call(design, command);
		} ਫੜੋ (log_cmd_error_exception) {
			ਜਦੋਂ (design->selection_ਢੇਰ.ਮਾਪ() > 1)
				design->ਕੱਢੋ_selection();
			log_reset_ਢੇਰ();
		}
		design->check();
#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ਜੇ (command)
			ਮੁਕਤ(command);
#ਜੇ_ਅੰਤ
	}
	ਜੇ (command == ਨਿਰਾ)
		ਲਿਖੋ("exit\n");
#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ਨਹੀਂਤਾਂ
		ਮੁਕਤ(command);
#ਜੇ_ਅੰਤ
	recursion_counter--;
}

ਬਣਤਰ ShellPass : ਜਨਤਕ Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	ਖਾਲੀ help() override {
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    shell\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("This command enters the interactive command mode. This can be useful\n");
		ਲਘੂਗਣਕ("in a script to interrupt the script at a certain point and allow for\n");
		ਲਘੂਗਣਕ("interactive inspection or manual synthesis of the design at this point.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("The command prompt of the interactive shell indicates the current\n");
		ਲਘੂਗਣਕ("selection (see 'help select'):\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    yosys>\n");
		ਲਘੂਗਣਕ("        the entire design is selected\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    yosys*>\n");
		ਲਘੂਗਣਕ("        only part of the design is selected\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    yosys [modname]>\n");
		ਲਘੂਗਣਕ("        the entire module 'modname' is selected using 'select -module modname'\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    yosys [modname]*>\n");
		ਲਘੂਗਣਕ("        only part of current module 'modname' is selected\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		ਲਘੂਗਣਕ("do not terminate yosys but return to the command prompt.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("This command is the default action if nothing else has been specified\n");
		ਲਘੂਗਣਕ("on the command line.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		ਲਘੂਗਣਕ("\n");
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) override {
		extra_ਦਲੀਲਾਂ(ਦਲੀਲਾਂ_2, 1, design, ਝੂਠ);
		shell(design);
	}
} ShellPass;

#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ਬਣਤਰ HistoryPass : ਜਨਤਕ Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	ਖਾਲੀ help() override {
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    history\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("This command prints all commands in the shell history buffer. This are\n");
		ਲਘੂਗਣਕ("all commands executed in an interactive session, but not the commands\n");
		ਲਘੂਗਣਕ("from executed scripts.\n");
		ਲਘੂਗਣਕ("\n");
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) override {
		extra_ਦਲੀਲਾਂ(ਦਲੀਲਾਂ_2, 1, design, ਝੂਠ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_READLINE
		ਲਈ(HIST_ਇੰਦਰਾਜ **list = history_list(); *list != ਨਿਰਾ; list++)
			ਲਘੂਗਣਕ("%s\n", (*list)->ਸਤਰ);
#ਨਹੀਂਤਾਂ
		ਲਈ (ਪੂਰਨ i = where_history(); history_get(i); i++)
			ਲਘੂਗਣਕ("%s\n", history_get(i)->ਸਤਰ);
#ਜੇ_ਅੰਤ
	}
} HistoryPass;
#ਜੇ_ਅੰਤ

ਬਣਤਰ ScriptCmdPass : ਜਨਤਕ Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	ਖਾਲੀ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    script <filename> [<from_label>:<to_label>]\n");
		ਲਘੂਗਣਕ("    script -scriptwire [selection]\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("This command executes the yosys commands in the specified file (default\n");
		ਲਘੂਗਣਕ("behaviour), or commands embedded in the constant text value connected to the\n");
		ਲਘੂਗਣਕ("selected wires.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("In the default (file) case, the 2nd argument can be used to only execute the\n");
		ਲਘੂਗਣਕ("section of the file between the specified labels. An empty from label is\n");
		ਲਘੂਗਣਕ("synonymous with the beginning of the file and an empty to label is synonymous\n");
		ਲਘੂਗਣਕ("with the end of the file.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("If only one label is specified (without ':') then only the block\n");
		ਲਘੂਗਣਕ("marked with that label (until the next label) is executed.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		ਲਘੂਗਣਕ("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		ਲਘੂਗਣਕ("'-module' mode can be exited by using the 'cd' command.\n");
		ਲਘੂਗਣਕ("\n");
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) override
	{
		ਤਾਰਕਿਕ scriptwire = ਝੂਠ;

		ਮਾਪ_t argidx;
		ਲਈ (argidx = 1; argidx < ਦਲੀਲਾਂ_2.ਮਾਪ(); argidx++) {
			ਜੇ (ਦਲੀਲਾਂ_2[argidx] == "-scriptwire") {
				scriptwire = ਸੱਚ;
				ਜਾਰੀ;
			}
			ਰੋਕੋ;
		}
		ਜੇ (scriptwire) {
			extra_ਦਲੀਲਾਂ(ਦਲੀਲਾਂ_2, argidx, design);

			ਲਈ (ਸਵੈਚਾਲਿਤ mod : design->selected_modules())
				ਲਈ (ਸਵੈਚਾਲਿਤ &c : mod->connections()) {
					ਜੇ (!c.ਪਹਿਲਾ.is_wire())
						ਜਾਰੀ;
					ਸਵੈਚਾਲਿਤ w = c.ਪਹਿਲਾ.as_wire();
					ਜੇ (!mod->selected(w))
						ਜਾਰੀ;
					ਜੇ (!c.ਦੂਜਾ.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					ਸਵੈਚਾਲਿਤ v = c.ਦੂਜਾ.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() < 2)
			log_cmd_error("Missing script file.\n");
		ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 2)
			ਲੜੀਵਾਰ_frontend(ਦਲੀਲਾਂ_2[1], "script", design);
		ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 3)
			ਲੜੀਵਾਰ_frontend(ਦਲੀਲਾਂ_2[1], "script", design, &ਦਲੀਲਾਂ_2[2]);
		ਨਹੀਂਤਾਂ
			extra_ਦਲੀਲਾਂ(ਦਲੀਲਾਂ_2, 2, design, ਝੂਠ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_ਸਿਰਾ
