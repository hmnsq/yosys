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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
#  include <tcl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
ನಾಮಾಕಾಶ py = pybind11;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#ಅಥವಾಆದರೆ defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ಆದರೆ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
ಬಾಹ್ಯ "C" PyObject* PyInit_libyosys();
ಬಾಹ್ಯ "C" PyObject* PyInit_pyosys();
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
ಪೂರ್ಣಾಂಕ yosys_xtrace = 0;
ತಾರ್ಕಿಕ yosys_write_versions = ನಿಜ;
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* yosys_maybe_ಆವೃತ್ತಿ() {
	ಆದರೆ (yosys_write_versions)
		ಹಿಂತಿರುಗಿ yosys_ಆವೃತ್ತಿ_str;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ "Yosys";
}

RTLIL::Design *yosys_design = ಖಾಲಿ;
NewCellTypes yosys_celltypes;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ಖಾಲಿ;
Tcl_Interp *yosys_sdc_interp = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

std::ಗಣ<std::string> yosys_ಒಳಹರಿವು_files, yosys_output_files;

ತಾರ್ಕಿಕ memhasher_ಸಕ್ರಿಯ = ಸುಳ್ಳು;
uint32_t memhasher_rng = 123456;
std::ಸದಿಶ<ಶೂನ್ಯ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

ತಾರ್ಕಿಕ Multithreading::ಸಕ್ರಿಯ_2 = ಸುಳ್ಳು;

ಶೂನ್ಯ init_share_dirname();
ಶೂನ್ಯ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!ಸಕ್ರಿಯ_2);
	ಸಕ್ರಿಯ_2 = ನಿಜ;
}

Multithreading::~Multithreading() {
	log_assert(ಸಕ್ರಿಯ_2);
	ಸಕ್ರಿಯ_2 = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Autoidx::ensure_at_ಕನಿಷ್ಠ(ಪೂರ್ಣಾಂಕ v) {
	log_assert(!Multithreading::ಸಕ್ರಿಯ());
	ಮೌಲ್ಯ = std::max(ಮೌಲ್ಯ, v);
}

ಪೂರ್ಣಾಂಕ Autoidx::ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {
	log_assert(!Multithreading::ಸಕ್ರಿಯ());
	ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ++;
}

ಶೂನ್ಯ memhasher_on()
{
#ಆದರೆ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += ಸಮಯ_2(ಖಾಲಿ) << 16 ^ getpid();
#ಆದರೆ_ಅಂತ್ಯ
	memhasher_store.resize(0x10000);
	memhasher_ಸಕ್ರಿಯ = ನಿಜ;
}

ಶೂನ್ಯ memhasher_off()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ p : memhasher_store)
		ಆದರೆ (p) ಬಿಡುಗಡೆ(p);
	memhasher_store.clear();
	memhasher_ಸಕ್ರಿಯ = ಸುಳ್ಳು;
}

ಶೂನ್ಯ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, ಸೂಚ್ಯಂಕ = (memhasher_rng >> 4) & 0xffff;
	ಆಯ್ಕೆ (memhasher_rng & 7) {
		ಸ್ಥಿತಿ 0: ಗಾತ್ರ_2 =   16; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 1: ಗಾತ್ರ_2 =  256; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2: ಗಾತ್ರ_2 = 1024; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 3: ಗಾತ್ರ_2 = 4096; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: ಗಾತ್ರ_2 = 0;
	}
	ಆದರೆ (ಸೂಚ್ಯಂಕ < 16) ಗಾತ್ರ_2 *= 16;
	memhasher_store[ಸೂಚ್ಯಂಕ] = realloc(memhasher_store[ಸೂಚ್ಯಂಕ], ಗಾತ್ರ_2);
}

ಶೂನ್ಯ yosys_banner()
{
	ಲಘುಗಣಕ("\n");
	ಲಘುಗಣಕ(" /----------------------------------------------------------------------------\\\n");
	ಲಘುಗಣಕ(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	ಲಘುಗಣಕ(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	ಲಘುಗಣಕ(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	ಲಘುಗಣಕ(" \\----------------------------------------------------------------------------/\n");
	ಲಘುಗಣಕ(" %s\n", yosys_maybe_ಆವೃತ್ತಿ());
}

#ಆದರೆ defined(YOSYS_ENABLE_SPAWN)
ಪೂರ್ಣಾಂಕ ಸತತ_command(ಸ್ಥಿರಾಂಕ std::string &command, std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ std::string&)> process_ಸಾಲು)
{
	ಆದರೆ (!process_ಸಾಲು)
		ಹಿಂತಿರುಗಿ system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ಆದರೆ (f == ಶೂನ್ಯಸೂಚಕ)
		ಹಿಂತಿರುಗಿ -1;

	std::string ಸಾಲು;
	ಅಕ್ಷರ logbuf[128];
	ಆಗ (ನಿಜ) {
		ಆದರೆ (fgets(logbuf, 128, f) != ಖಾಲಿ) {
			ಸಾಲು += logbuf;
			ಆದರೆ (!ಸಾಲು.ಖಾಲಿಗಳು() && ಸಾಲು.ಹಿಂಭಾಗ() == '\n')
				process_ಸಾಲು(ಸಾಲು), ಸಾಲು.clear();
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ferror(f) && errno == EINTR) {
			clearerr(f);
			ಮುಂದುವರಿ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ (!ಸಾಲು.ಖಾಲಿಗಳು())
		process_ಸಾಲು(ಸಾಲು);

	ಪೂರ್ಣಾಂಕ ret = pclose(f);
	ಆದರೆ (ret < 0)
		ಹಿಂತಿರುಗಿ -2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
	ಹಿಂತಿರುಗಿ ret;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (!WIFEXITED(ret))
		ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ WEXITSTATUS(ret);
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ already_ಸಿದ್ಧತೆ = ಸುಳ್ಳು;
ತಾರ್ಕಿಕ already_shutdown = ಸುಳ್ಳು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.ಕೂಡಿಸು_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(ಶೂನ್ಯ)_;
	ಎಸೆ py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ yosys_ಸಿದ್ಧತೆ()
{
	ಆದರೆ(already_ಸಿದ್ಧತೆ)
		ಹಿಂತಿರುಗಿ;
	already_ಸಿದ್ಧತೆ = ನಿಜ;
	already_shutdown = ಸುಳ್ಳು;

	StaticTwines::init();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ಆದರೆ (!Py_IsInitialized()) {
		PyImport_AppendInittab((ಅಕ್ಷರ*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((ಅಕ್ಷರ*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((ಅಕ್ಷರ*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_ಸತತ_Simple_String("import sys");
		ಸಂಕೇತ(SIGINT, SIG_DFL);
	}
#ಆದರೆ_ಅಂತ್ಯ

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_ರಿಜಿಸ್ಟರ್();
	yosys_design = ಹೊಸ RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_ತಿಳಿದ;
	log_ತಳ್ಳು();
}

ತಾರ್ಕಿಕ yosys_already_ಸಿದ್ಧತೆ()
{
	ಹಿಂತಿರುಗಿ already_ಸಿದ್ಧತೆ;
}

ಶೂನ್ಯ yosys_shutdown()
{
	ಆದರೆ(already_shutdown)
		ಹಿಂತಿರುಗಿ;
	already_ಸಿದ್ಧತೆ = ಸುಳ್ಳು;
	already_shutdown = ನಿಜ;
	log_ತೆಗೆ();

	Pass::ಮುಗಿದ_ರಿಜಿಸ್ಟರ್();

	ಅಳಿಸು yosys_design;
	yosys_design = ಖಾಲಿ;

	logger().clear();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
	ಆದರೆ (yosys_tcl_interp != ಖಾಲಿ) {
		ಆದರೆ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PLUGINS
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : loaded_plugins)
		dlclose(it.ಎರಡನೇ);

	loaded_plugins.clear();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#ಆದರೆ_ಅಂತ್ಯ
	loaded_plugin_aliases.clear();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	Py_Finalize();
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ std::string *create_id_ಪೂರ್ವಪ್ರತ್ಯಯ(std::string_view file, ಪೂರ್ಣಾಂಕ ಸಾಲು, std::string_view func)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
	ಗಾತ್ರ_t pos = file.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ("/\\");
#ಇಲ್ಲದಿದ್ದರೆ
	ಗಾತ್ರ_t pos = file.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ('/');
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ(':');
	ಆದರೆ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	ಹಿಂತಿರುಗಿ ಹೊಸ std::string(stringf("$auto$%s:%d:%s$", file, ಸಾಲು, func));
}

RTLIL::Design *yosys_get_design()
{
	ಹಿಂತಿರುಗಿ yosys_design;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *create_prompt(RTLIL::Design *design, ಪೂರ್ಣಾಂಕ recursion_counter, ತಾರ್ಕಿಕ newline)
{
	ಸ್ಥಿರ ಅಕ್ಷರ ಬಫರ್[100];
	std::string str = newline ? "\n" : "";
	ಆದರೆ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ಆದರೆ (!design->selected_ಸಕ್ರಿಯ_module.ಖಾಲಿಗಳು())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_ಸಕ್ರಿಯ_module)).c_str());
	ಆದರೆ (!design->ತುಂಬಿದ_selection()) {
		ಆದರೆ (design->selected_ಸಕ್ರಿಯ_module.ಖಾಲಿಗಳು())
			str += "*";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (design->selection().selected_modules.ಗಾತ್ರ_2() != 1 || design->selection().selected_members.ಗಾತ್ರ_2() != 0 ||
				design->selection().selected_modules.ಎಣಿಕೆ_2(design->selected_ಸಕ್ರಿಯ_module) == 0)
			str += "*";
	}
	snprintf(ಬಫರ್, 100, "%s> ", str.c_str());
	ಹಿಂತಿರುಗಿ ಬಫರ್;
}

std::ಸದಿಶ<std::string> glob_filename(ಸ್ಥಿರಾಂಕ std::string &filename_ನಮೂನೆ)
{
	std::ಸದಿಶ<std::string> results;

#ಆದರೆ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ತಳ್ಳು_ಹಿಂಭಾಗ(filename_ನಮೂನೆ);
#ಇಲ್ಲದಿದ್ದರೆ
	glob_t globbuf;

	ಪೂರ್ಣಾಂಕ err = glob(filename_ನಮೂನೆ.c_str(), 0, ಖಾಲಿ, &globbuf);

	ಆದರೆ(err == 0) {
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < globbuf.gl_pathc; i++)
			results.ತಳ್ಳು_ಹಿಂಭಾಗ(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} ಇಲ್ಲದಿದ್ದರೆ {
		results.ತಳ್ಳು_ಹಿಂಭಾಗ(filename_ನಮೂನೆ);
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ results;
}

ಶೂನ್ಯ rewrite_filename(std::string &filename)
{
	ಆದರೆ (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_ಗಾತ್ರ(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_ಗಾತ್ರ(filename)-2);
	ಆದರೆ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _WIN32
	ಆದರೆ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL

// defined in tclapi.cc
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ yosys_tcl_interp_init(Tcl_Interp *interp);

ಬಾಹ್ಯ Tcl_Interp *yosys_get_tcl_interp()
{
	ಆದರೆ (yosys_tcl_interp == ಖಾಲಿ) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	ಹಿಂತಿರುಗಿ yosys_tcl_interp;
}

// Also see SdcPass
ರಚನೆ TclPass : ಸಾರ್ವಜನಿಕ Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	ಶೂನ್ಯ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    tcl <filename> [args]\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("This command executes the tcl commands in the specified file.\n");
		ಲಘುಗಣಕ("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("The tcl command 'yosys -import' can be used to import all yosys\n");
		ಲಘುಗಣಕ("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		ಲಘುಗಣಕ("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		ಲಘುಗಣಕ("in order to avoid a name collision with the built in commands.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("If any arguments are specified, these arguments are provided to the script via\n");
		ಲಘುಗಣಕ("the standard $argc and $argv variables.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("Note, tcl will not receive the output of any yosys command. If the output\n");
		ಲಘುಗಣಕ("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		ಲಘುಗಣಕ("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		ಲಘುಗಣಕ("The 'result.string' value is then used as the tcl output value of the command.\n");
		ಲಘುಗಣಕ("\n");
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *) override {
		ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() < 2)
			log_cmd_error("Missing script file.\n");

		std::ಸದಿಶ<Tcl_Obj*> script_ವಾದಗಳು;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = ವಾದಗಳು_2.begin() + 2; it != ವಾದಗಳು_2.ಕೊನೆ(); ++it)
			script_ವಾದಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(Tcl_NewStringObj((*it).c_str(), (*it).ಗಾತ್ರ_2()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_ಗಣ_Var2(interp, Tcl_NewStringObj("argc", 4), ಖಾಲಿ, Tcl_NewIntObj(script_ವಾದಗಳು.ಗಾತ್ರ_2()), 0);
		Tcl_Obj_ಗಣ_Var2(interp, Tcl_NewStringObj("argv", 4), ಖಾಲಿ, Tcl_NewListObj(script_ವಾದಗಳು.ಗಾತ್ರ_2(), script_ವಾದಗಳು.ದತ್ತಾಂಶ()), 0);
		Tcl_Obj_ಗಣ_Var2(interp, Tcl_NewStringObj("argv0", 5), ಖಾಲಿ, Tcl_NewStringObj(ವಾದಗಳು_2[1].c_str(), ವಾದಗಳು_2[1].ಗಾತ್ರ_2()), 0);
		ಆದರೆ (Tcl_EvalFile(interp, ವಾದಗಳು_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ಫಲಿತಾಂಶ(interp));
		Tcl_Release(interp);
	}
} TclPass;

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string ಮಾರ್ಗ_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", ಮಾರ್ಗ_2.ದತ್ತಾಂಶ(), ಮಾರ್ಗ_2.ಗಾತ್ರ_2());
		ಆದರೆ (buflen < (ssize_t)ಮಾರ್ಗ_2.ಗಾತ್ರ_2())
			ನಿಲ್ಲಿಸು;
		ಇಲ್ಲದಿದ್ದರೆ
			ಮಾರ್ಗ_2.resize(ಮಾರ್ಗ_2.ಗಾತ್ರ_2() * 2);
	}
	ಆದರೆ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		ಮಾರ್ಗ_2.resize(0);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆಗ (buflen > 0 && ಮಾರ್ಗ_2[buflen-1] != '/')
			buflen--;
		ಮಾರ್ಗ_2.resize(buflen);
	}
	ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_2;
}
#ಅಥವಾಆದರೆ defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __NetBSD__
	ಪೂರ್ಣಾಂಕ mib[4] = {CTL_KERN, KERN_PROC_ವಾದಗಳು, getpid(), KERN_PROC_PATHNAME};
#ಇಲ್ಲದಿದ್ದರೆ
	ಪೂರ್ಣಾಂಕ mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#ಆದರೆ_ಅಂತ್ಯ
	ಗಾತ್ರ_t buflen;
	ಅಕ್ಷರ *ಬಫರ್;
	std::string ಮಾರ್ಗ_2;
	ಆದರೆ (sysctl(mib, 4, ಖಾಲಿ, &buflen, ಖಾಲಿ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ಬಫರ್ = (ಅಕ್ಷರ*)ಸ್ಮರಣೆ_ಕೊಡು(buflen);
	ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
		log_error("malloc failed: %s\n", strerror(errno));
	ಆದರೆ (sysctl(mib, 4, ಬಫರ್, &buflen, ಖಾಲಿ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	ಆಗ (buflen > 0 && ಬಫರ್[buflen-1] != '/')
		buflen--;
	ಮಾರ್ಗ_2.ನಿಯೋಜಿಸು(ಬಫರ್, buflen);
	ಬಿಡುಗಡೆ(ಬಫರ್);
	ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_2;
}
#ಅಥವಾಆದರೆ defined(__APPLE__)
std::string proc_self_dirname()
{
	ಅಕ್ಷರ *ಮಾರ್ಗ_2 = ಖಾಲಿ;
	uint32_t buflen = 0;
	ಆಗ (NSGet_Executable_ಮಾರ್ಗ(ಮಾರ್ಗ_2, &buflen) != 0)
		ಮಾರ್ಗ_2 = (ಅಕ್ಷರ *) realloc((ಶೂನ್ಯ *) ಮಾರ್ಗ_2, buflen);
	ಅಕ್ಷರ resolved[ಮಾರ್ಗ_MAX];
	ಆದರೆ (realpath(ಮಾರ್ಗ_2, resolved) != ಖಾಲಿ) {
		ಬಿಡುಗಡೆ(ಮಾರ್ಗ_2);
		ಮಾರ್ಗ_2 = strdup(resolved);
		buflen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ_2);
	}
	ಆಗ (buflen > 0 && ಮಾರ್ಗ_2[buflen-1] != '/')
		buflen--;
	std::string str(ಮಾರ್ಗ_2, buflen);
	ಬಿಡುಗಡೆ(ಮಾರ್ಗ_2);
	ಹಿಂತಿರುಗಿ str;
}
#ಅಥವಾಆದರೆ defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ಆದರೆ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.ಗಾತ್ರ_2()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, wbinpath.ದತ್ತಾಂಶ(), wbinpath.ಗಾತ್ರ_2(), ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ));
	ಆದರೆ (Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, wbinpath.ದತ್ತಾಂಶ(), wbinpath.ಗಾತ್ರ_2(), &ubinpath[0], ubinpath.ಗಾತ್ರ_2(), ಖಾಲಿ, ಖಾಲಿ) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	ಹಿಂತಿರುಗಿ ubinpath;

}
#ಅಥವಾಆದರೆ defined(__wasm)
std::string proc_self_dirname()
{
	ಹಿಂತಿರುಗಿ "/";
}
#ಅಥವಾಆದರೆ defined(__OpenBSD__) || defined(__HAIKU__)
ಅಕ್ಷರ yosys_ಮಾರ್ಗ[ಮಾರ್ಗ_MAX];
ಅಕ್ಷರ *yosys_argv0;

std::string proc_self_dirname(ಶೂನ್ಯ)
{
	ಅಕ್ಷರ buf[ಮಾರ್ಗ_MAX + 1] = "", *ಮಾರ್ಗ_2, *p;
	// if case argv[0] contains a valid path, return it
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(yosys_ಮಾರ್ಗ) > 0) {
		p = strrchr(yosys_ಮಾರ್ಗ, '/');
		snprintf(buf, ಗಾತ್ರ buf, "%*s/", (ಪೂರ್ಣಾಂಕ)(yosys_ಮಾರ್ಗ - p), yosys_ಮಾರ್ಗ);
		ಹಿಂತಿರುಗಿ buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	ಮಾರ್ಗ_2 = strdup(getenv("PATH"));
	ಆದರೆ (!ಮಾರ್ಗ_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	ಗಾಗಿ (p = strtok(ಮಾರ್ಗ_2, ":"); p; p = strtok(ಖಾಲಿ, ":")) {
		snprintf(buf, ಗಾತ್ರ buf, "%s/%s", p, yosys_argv0);
		ಆದರೆ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			ಬಿಡುಗಡೆ(ಮಾರ್ಗ_2);
			ಹಿಂತಿರುಗಿ buf;
		}
	}
	ಬಿಡುಗಡೆ(ಮಾರ್ಗ_2);
	log_error("Can't determine yosys executable path.\n");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಇಲ್ಲದಿದ್ದರೆ
	#ದೋಷ "Don't know how to determine process executable base path!"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__wasm)
ಶೂನ್ಯ init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ಆದರೆ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_ಮಾರ್ಗ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *share_ಮಾರ್ಗ = PyUnicode_AsUTF8(share_ಮಾರ್ಗ_obj);
		yosys_share_dirname = std::string(share_ಮಾರ್ಗ);
		ಹಿಂತಿರುಗಿ;
	}
#  endif
	std::string proc_self_ಮಾರ್ಗ = proc_self_dirname();
#  ಆದರೆ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_ಮಾರ್ಗ = proc_self_ಮಾರ್ಗ + "share\\";
	ಆದರೆ (check_directory_exists(proc_share_ಮಾರ್ಗ, ನಿಜ)) {
		yosys_share_dirname = proc_share_ಮಾರ್ಗ;
		ಹಿಂತಿರುಗಿ;
	}
	proc_share_ಮಾರ್ಗ = proc_self_ಮಾರ್ಗ + "..\\share\\";
	ಆದರೆ (check_directory_exists(proc_share_ಮಾರ್ಗ, ನಿಜ)) {
		yosys_share_dirname = proc_share_ಮಾರ್ಗ;
		ಹಿಂತಿರುಗಿ;
	}
#  ಇಲ್ಲದಿದ್ದರೆ
	std::string proc_share_ಮಾರ್ಗ = proc_self_ಮಾರ್ಗ + "share/";
	ಆದರೆ (check_directory_exists(proc_share_ಮಾರ್ಗ, ನಿಜ)) {
		yosys_share_dirname = proc_share_ಮಾರ್ಗ;
		ಹಿಂತಿರುಗಿ;
	}
	proc_share_ಮಾರ್ಗ = proc_self_ಮಾರ್ಗ + "../share/" + proc_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ()+ "yosys/";
	ಆದರೆ (check_directory_exists(proc_share_ಮಾರ್ಗ, ನಿಜ)) {
		yosys_share_dirname = proc_share_ಮಾರ್ಗ;
		ಹಿಂತಿರುಗಿ;
	}
	proc_share_ಮಾರ್ಗ = YOSYS_DATDIR "/";
	ಆದರೆ (check_directory_exists(proc_share_ಮಾರ್ಗ, ನಿಜ)) {
		yosys_share_dirname = proc_share_ಮಾರ್ಗ;
		ಹಿಂತಿರುಗಿ;
	}
#  endif
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ init_abc_executable_name()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ABCEXTERNAL
	std::string exe_file;
	ಆದರೆ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} ಇಲ್ಲದಿದ್ದರೆ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	yosys_abc_executable = proc_self_dirname() + proc_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ()+ "yosys-abc";
#  ifdef _WIN32
	ಆದರೆ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ಆದರೆ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_ಮಾರ್ಗ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *abc_ಮಾರ್ಗ = PyUnicode_AsUTF8(abc_ಮಾರ್ಗ_obj);
		yosys_abc_executable = std::string(abc_ಮಾರ್ಗ);
	}
#  endif
#ಆದರೆ_ಅಂತ್ಯ
}

std::string proc_share_dirname()
{
	ಆದರೆ (yosys_share_dirname.ಖಾಲಿಗಳು())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	ಹಿಂತಿರುಗಿ yosys_share_dirname;
}

std::string proc_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ()
{
	ಹಿಂತಿರುಗಿ YOSYS_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ;
}

ತಾರ್ಕಿಕ fgetline(FILE *f, std::string &ಬಫರ್)
{
	ಬಫರ್ = "";
	ಅಕ್ಷರ ಖಂಡ[4096];
	ಆಗ (1) {
		ಆದರೆ (fgets(ಖಂಡ, 4096, f) == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಬಫರ್ += ಖಂಡ;
		ಆದರೆ (ಬಫರ್.ಗಾತ್ರ_2() > 0 && (ಬಫರ್[ಬಫರ್.ಗಾತ್ರ_2()-1] == '\n' ||  ಬಫರ್[ಬಫರ್.ಗಾತ್ರ_2()-1] == '\r')) {
			ಆಗ (ಬಫರ್.ಗಾತ್ರ_2() > 0 && (ಬಫರ್[ಬಫರ್.ಗಾತ್ರ_2()-1] == '\n' ||  ಬಫರ್[ಬಫರ್.ಗಾತ್ರ_2()-1] == '\r'))
				ಬಫರ್.resize(ಬಫರ್.ಗಾತ್ರ_2()-1);
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ handle_label(std::string &command, ತಾರ್ಕಿಕ &from_ಗಮ್ಯ_ಸಕ್ರಿಯ, ಸ್ಥಿರಾಂಕ std::string &ಸತತ_from, ಸ್ಥಿರಾಂಕ std::string &ಸತತ_ಗಮ್ಯ)
{
	ಪೂರ್ಣಾಂಕ pos = 0;
	std::string label;

	ಆಗ (pos < Get_ಗಾತ್ರ(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ಆದರೆ (pos < Get_ಗಾತ್ರ(command) && command[pos] == '#')
		ಹಿಂತಿರುಗಿ;

	ಆಗ (pos < Get_ಗಾತ್ರ(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ಆದರೆ (Get_ಗಾತ್ರ(label) > 1 && label.ಹಿಂಭಾಗ() == ':')
	{
		label = label.substr(0, Get_ಗಾತ್ರ(label)-1);
		command = command.substr(pos);

		ಆದರೆ (label == ಸತತ_from)
			from_ಗಮ್ಯ_ಸಕ್ರಿಯ = ನಿಜ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (label == ಸತತ_ಗಮ್ಯ || (ಸತತ_from == ಸತತ_ಗಮ್ಯ && !ಸತತ_from.ಖಾಲಿಗಳು()))
			from_ಗಮ್ಯ_ಸಕ್ರಿಯ = ಸುಳ್ಳು;
	}
}

ತಾರ್ಕಿಕ ಸತತ_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_ಗಮ್ಯ_label)
{
	ಆದರೆ (design == ಶೂನ್ಯಸೂಚಕ)
		design = yosys_design;

	ಆದರೆ (command == "auto") {
	  std::string filename_trim = filename;

	  ಸ್ವಯಂಚಾಲಿತ has_extension = [](ಸ್ಥಿರಾಂಕ std::string& filename, ಸ್ಥಿರಾಂಕ std::string& extension) {
	    ಆದರೆ (filename.ಗಾತ್ರ_2() >= extension.ಗಾತ್ರ_2()) {
	      ಹಿಂತಿರುಗಿ filename.compare(filename.ಗಾತ್ರ_2() - extension.ಗಾತ್ರ_2(), extension.ಗಾತ್ರ_2(), extension) == 0;
	    }
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	  };

	  ಆದರೆ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.ಗಾತ್ರ_2() - 3);
	  }

	  ಆದರೆ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename == "-") {
	    command = "script";
	  } ಇಲ್ಲದಿದ್ದರೆ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ಆದರೆ (command == "script")
	{
		std::string ಸತತ_from, ಸತತ_ಗಮ್ಯ;
		ತಾರ್ಕಿಕ from_ಗಮ್ಯ_ಸಕ್ರಿಯ = ನಿಜ;

		ಆದರೆ (from_ಗಮ್ಯ_label != ಖಾಲಿ) {
			ಗಾತ್ರ_t pos = from_ಗಮ್ಯ_label->ಕಂಡುಹಿಡಿ(':');
			ಆದರೆ (pos == std::string::npos) {
				ಸತತ_from = *from_ಗಮ್ಯ_label;
				ಸತತ_ಗಮ್ಯ = *from_ಗಮ್ಯ_label;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಸತತ_from = from_ಗಮ್ಯ_label->substr(0, pos);
				ಸತತ_ಗಮ್ಯ = from_ಗಮ್ಯ_label->substr(pos+1);
			}
			from_ಗಮ್ಯ_ಸಕ್ರಿಯ = ಸತತ_from.ಖಾಲಿಗಳು();
		}

		ಲಘುಗಣಕ("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ಆದರೆ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_ಒಳಹರಿವು_files.ಸೇರಿಸು(filename);
		}

		ಆದರೆ (f == ಖಾಲಿ)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		ಪೂರ್ಣಾಂಕ backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		ಪ್ರಯತ್ನಿಸು {
			std::string command;
			ಆಗ (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				ಆಗ (!command.ಖಾಲಿಗಳು() && command[command.ಗಾತ್ರ_2()-1] == '\\') {
					std::string ಮುಂದಿನ_ಸಾಲು;
					ಆದರೆ (!fgetline(f, ಮುಂದಿನ_ಸಾಲು))
						ನಿಲ್ಲಿಸು;
					Frontend::current_script_lineno++;
					command.resize(command.ಗಾತ್ರ_2()-1);
					command += ಮುಂದಿನ_ಸಾಲು;
				}
				handle_label(command, from_ಗಮ್ಯ_ಸಕ್ರಿಯ, ಸತತ_from, ಸತತ_ಗಮ್ಯ);
				ಆದರೆ (from_ಗಮ್ಯ_ಸಕ್ರಿಯ) {
					Pass::call(design, command);
					design->check();
				}
			}

			ಆದರೆ (!command.ಖಾಲಿಗಳು()) {
				handle_label(command, from_ಗಮ್ಯ_ಸಕ್ರಿಯ, ಸತತ_from, ಸತತ_ಗಮ್ಯ);
				ಆದರೆ (from_ಗಮ್ಯ_ಸಕ್ರಿಯ) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		ಹಿಡಿ (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ಎಸೆ;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		ಆದರೆ (filename != "-")
			fclose(f);

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (command == "tcl") {
		Pass::call(design, ಸದಿಶ<string>({command, filename}));
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಲಘುಗಣಕ("\n");
	ಆದರೆ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ಆದರೆ (command[0] == ' ') {
		ಸ್ವಯಂಚಾಲಿತ argv = ವಿಭಜಿಸು_ಟೋಕನುಗಳು("read" + command);
		argv.ತಳ್ಳು_ಹಿಂಭಾಗ(filename);
		Pass::call(design, argv);
	} ಇಲ್ಲದಿದ್ದರೆ
		Frontend::frontend_call(design, ಖಾಲಿ, filename, command);

	design->check();
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ ಸತತ_pass(std::string command, RTLIL::Design *design)
{
	ಆದರೆ (design == ಶೂನ್ಯಸೂಚಕ)
		design = yosys_design;

	ಲಘುಗಣಕ("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

ಶೂನ್ಯ ಸತತ_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ಆದರೆ (design == ಶೂನ್ಯಸೂಚಕ)
		design = yosys_design;

	ಆದರೆ (command == "auto") {
		ಆದರೆ (filename.ಗಾತ್ರ_2() > 2 && filename.compare(filename.ಗಾತ್ರ_2()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 3 && filename.compare(filename.ಗಾತ್ರ_2()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 3 && filename.compare(filename.ಗಾತ್ರ_2()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 3 && filename.compare(filename.ಗಾತ್ರ_2()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 4 && filename.compare(filename.ಗಾತ್ರ_2()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 5 && filename.compare(filename.ಗಾತ್ರ_2()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 5 && filename.compare(filename.ಗಾತ್ರ_2()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಗಾತ್ರ_2() > 5 && filename.compare(filename.ಗಾತ್ರ_2()-5, std::string::npos, ".json") == 0)
			command = "json";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename == "-")
			command = "rtlil";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ;
		ಇಲ್ಲದಿದ್ದರೆ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ಆದರೆ (filename.ಖಾಲಿಗಳು())
		filename = "-";

	ಲಘುಗಣಕ("\n");
	ಆದರೆ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ಖಾಲಿ, filename, command);
}

#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ಸ್ಥಿರ ಅಕ್ಷರ *readline_cmd_generator(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ state)
{
	ಸ್ಥಿರ std::map<std::string, Pass*>::iterator it;
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ len;

	ಆದರೆ (!state) {
		it = pass_ರಿಜಿಸ್ಟರ್.begin();
		len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ);
	}

	ಗಾಗಿ (; it != pass_ರಿಜಿಸ್ಟರ್.ಕೊನೆ(); it++) {
		ಆದರೆ (it->ಮೊದಲ.compare(0, len, ಪಠ್ಯ) == 0)
			ಹಿಂತಿರುಗಿ strdup((it++)->ಮೊದಲ.c_str());
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಅಕ್ಷರ *readline_obj_generator(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ state)
{
	ಸ್ಥಿರ std::ಸದಿಶ<ಅಕ್ಷರ*> obj_ಹೆಸರುಗಳು;
	ಸ್ಥಿರ ಗಾತ್ರ_t idx;

	ಆದರೆ (!state)
	{
		idx = 0;
		obj_ಹೆಸರುಗಳು.clear();

		RTLIL::Design *design = yosys_get_design();
		ಪೂರ್ಣಾಂಕ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ);

		ಆದರೆ (design->selected_ಸಕ್ರಿಯ_module.ಖಾಲಿಗಳು())
		{
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ಆದರೆ (mod_name.compare(0, len, ಪಠ್ಯ) == 0)
					obj_ಹೆಸರುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(strdup(mod_name.c_str()));
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (design->module(design->selected_ಸಕ್ರಿಯ_module) != ಶೂನ್ಯಸೂಚಕ)
		{
			RTLIL::Module *module = design->module(design->selected_ಸಕ್ರಿಯ_module);

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ w : module->wires())
				ಆದರೆ (w->name.unescape().compare(0, len, ಪಠ್ಯ) == 0)
					obj_ಹೆಸರುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(strdup(w->name.unescape().c_str()));

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.ಮೊದಲ);
				ಆದರೆ (mem_name.compare(0, len, ಪಠ್ಯ) == 0)
					obj_ಹೆಸರುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(strdup(mem_name.c_str()));
			}

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells())
				ಆದರೆ (cell->name.unescape().compare(0, len, ಪಠ್ಯ) == 0)
					obj_ಹೆಸರುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(strdup(cell->name.unescape().c_str()));

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.ಮೊದಲ);
				ಆದರೆ (proc_name.compare(0, len, ಪಠ್ಯ) == 0)
					obj_ಹೆಸರುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(strdup(proc_name.c_str()));
			}
		}

		std::ವಿಂಗಡಿಸು(obj_ಹೆಸರುಗಳು.begin(), obj_ಹೆಸರುಗಳು.ಕೊನೆ());
	}

	ಆದರೆ (idx < obj_ಹೆಸರುಗಳು.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ strdup(obj_ಹೆಸರುಗಳು[idx++]);

	idx = 0;
	obj_ಹೆಸರುಗಳು.clear();
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಅಕ್ಷರ **readline_completion(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ)
{
	ಆದರೆ (ಆರಂಭ == 0)
		ಹಿಂತಿರುಗಿ rl_completion_matches(ಪಠ್ಯ, readline_cmd_generator);
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(rl_ಸಾಲು_ಬಫರ್, "read_", 5) && ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(rl_ಸಾಲು_ಬಫರ್, "write_", 6))
		ಹಿಂತಿರುಗಿ rl_completion_matches(ಪಠ್ಯ, readline_obj_generator);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ shell(RTLIL::Design *design)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recursion_counter = 0;

	recursion_counter++;
	ಸ್ವಯಂಚಾಲಿತ guard = logger().error_throw_scope();

#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (ಅಕ್ಷರ*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_ಶಬ್ದ_break_characters = (ಅಕ್ಷರ*)" \t\n";
#ಆದರೆ_ಅಂತ್ಯ

	ಅಕ್ಷರ *command = ಖಾಲಿ;
#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ಆಗ ((command = readline(create_prompt(design, recursion_counter))) != ಖಾಲಿ)
	{
#ಇಲ್ಲದಿದ್ದರೆ
	ಅಕ್ಷರ command_ಬಫರ್[4096];
	ಆಗ (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ಆದರೆ ((command = fgets(command_ಬಫರ್, 4096, stdin)) == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (command[strspn(command, " \t\r\n")] == 0) {
#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			ಬಿಡುಗಡೆ(command);
#ಆದರೆ_ಅಂತ್ಯ
			ಮುಂದುವರಿ;
		}
#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ಕೂಡಿಸು_history(command);
#ಆದರೆ_ಅಂತ್ಯ

		ಅಕ್ಷರ *p = command + strspn(command, " \t\r\n");
		ಆದರೆ (!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ಆದರೆ (*p == 0)
				ನಿಲ್ಲಿಸು;
		}

		ಪ್ರಯತ್ನಿಸು {
			log_assert(design->selection_ರಾಶಿ.ಗಾತ್ರ_2() == 1);
			Pass::call(design, command);
		} ಹಿಡಿ (log_cmd_error_exception) {
			ಆಗ (design->selection_ರಾಶಿ.ಗಾತ್ರ_2() > 1)
				design->ತೆಗೆ_selection();
			log_reset_ರಾಶಿ();
		}
		design->check();
#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ಆದರೆ (command)
			ಬಿಡುಗಡೆ(command);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (command == ಖಾಲಿ)
		ಬರೆ("exit\n");
#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ಇಲ್ಲದಿದ್ದರೆ
		ಬಿಡುಗಡೆ(command);
#ಆದರೆ_ಅಂತ್ಯ
	recursion_counter--;
}

ರಚನೆ ShellPass : ಸಾರ್ವಜನಿಕ Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	ಶೂನ್ಯ help() override {
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    shell\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("This command enters the interactive command mode. This can be useful\n");
		ಲಘುಗಣಕ("in a script to interrupt the script at a certain point and allow for\n");
		ಲಘುಗಣಕ("interactive inspection or manual synthesis of the design at this point.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("The command prompt of the interactive shell indicates the current\n");
		ಲಘುಗಣಕ("selection (see 'help select'):\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    yosys>\n");
		ಲಘುಗಣಕ("        the entire design is selected\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    yosys*>\n");
		ಲಘುಗಣಕ("        only part of the design is selected\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    yosys [modname]>\n");
		ಲಘುಗಣಕ("        the entire module 'modname' is selected using 'select -module modname'\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    yosys [modname]*>\n");
		ಲಘುಗಣಕ("        only part of current module 'modname' is selected\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		ಲಘುಗಣಕ("do not terminate yosys but return to the command prompt.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("This command is the default action if nothing else has been specified\n");
		ಲಘುಗಣಕ("on the command line.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		ಲಘುಗಣಕ("\n");
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) override {
		extra_ವಾದಗಳು(ವಾದಗಳು_2, 1, design, ಸುಳ್ಳು);
		shell(design);
	}
} ShellPass;

#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ರಚನೆ HistoryPass : ಸಾರ್ವಜನಿಕ Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	ಶೂನ್ಯ help() override {
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    history\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("This command prints all commands in the shell history buffer. This are\n");
		ಲಘುಗಣಕ("all commands executed in an interactive session, but not the commands\n");
		ಲಘುಗಣಕ("from executed scripts.\n");
		ಲಘುಗಣಕ("\n");
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) override {
		extra_ವಾದಗಳು(ವಾದಗಳು_2, 1, design, ಸುಳ್ಳು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_READLINE
		ಗಾಗಿ(HIST_ನಮೂದು **list = history_list(); *list != ಖಾಲಿ; list++)
			ಲಘುಗಣಕ("%s\n", (*list)->ಸಾಲು);
#ಇಲ್ಲದಿದ್ದರೆ
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = where_history(); history_get(i); i++)
			ಲಘುಗಣಕ("%s\n", history_get(i)->ಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ
	}
} HistoryPass;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ ScriptCmdPass : ಸಾರ್ವಜನಿಕ Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	ಶೂನ್ಯ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    script <filename> [<from_label>:<to_label>]\n");
		ಲಘುಗಣಕ("    script -scriptwire [selection]\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("This command executes the yosys commands in the specified file (default\n");
		ಲಘುಗಣಕ("behaviour), or commands embedded in the constant text value connected to the\n");
		ಲಘುಗಣಕ("selected wires.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("In the default (file) case, the 2nd argument can be used to only execute the\n");
		ಲಘುಗಣಕ("section of the file between the specified labels. An empty from label is\n");
		ಲಘುಗಣಕ("synonymous with the beginning of the file and an empty to label is synonymous\n");
		ಲಘುಗಣಕ("with the end of the file.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("If only one label is specified (without ':') then only the block\n");
		ಲಘುಗಣಕ("marked with that label (until the next label) is executed.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		ಲಘುಗಣಕ("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		ಲಘುಗಣಕ("'-module' mode can be exited by using the 'cd' command.\n");
		ಲಘುಗಣಕ("\n");
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) override
	{
		ತಾರ್ಕಿಕ scriptwire = ಸುಳ್ಳು;

		ಗಾತ್ರ_t argidx;
		ಗಾಗಿ (argidx = 1; argidx < ವಾದಗಳು_2.ಗಾತ್ರ_2(); argidx++) {
			ಆದರೆ (ವಾದಗಳು_2[argidx] == "-scriptwire") {
				scriptwire = ನಿಜ;
				ಮುಂದುವರಿ;
			}
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (scriptwire) {
			extra_ವಾದಗಳು(ವಾದಗಳು_2, argidx, design);

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod : design->selected_modules())
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &c : mod->connections()) {
					ಆದರೆ (!c.ಮೊದಲ.is_wire())
						ಮುಂದುವರಿ;
					ಸ್ವಯಂಚಾಲಿತ w = c.ಮೊದಲ.as_wire();
					ಆದರೆ (!mod->selected(w))
						ಮುಂದುವರಿ;
					ಆದರೆ (!c.ಎರಡನೇ.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					ಸ್ವಯಂಚಾಲಿತ v = c.ಎರಡನೇ.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() < 2)
			log_cmd_error("Missing script file.\n");
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 2)
			ಸತತ_frontend(ವಾದಗಳು_2[1], "script", design);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 3)
			ಸತತ_frontend(ವಾದಗಳು_2[1], "script", design, &ವಾದಗಳು_2[2]);
		ಇಲ್ಲದಿದ್ದರೆ
			extra_ವಾದಗಳು(ವಾದಗಳು_2, 2, design, ಸುಳ್ಳು);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_ಕೊನೆ
