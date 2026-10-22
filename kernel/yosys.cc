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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/log.h"
#అన్తర్భావ "kernel/newcelltypes.h"

#నిర్వచితమైతే YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_TCL
#  include <tcl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
నామాకాశ py = pybind11;
#అయితే_ముగింపు

#ఒకవేళ defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#లేదావేళ defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#లేకపోతే
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ఒకవేళ defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#అయితే_ముగింపు

#ఒకవేళ !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#అయితే_ముగింపు

#ఒకవేళ defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
బాహ్య "C" PyObject* PyInit_libyosys();
బాహ్య "C" PyObject* PyInit_pyosys();
#అన్తర్భావ <signal.h>
#అయితే_ముగింపు

#అన్తర్భావ <limits.h>
#అన్తర్భావ <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
పూర్ణాంకం yosys_xtrace = 0;
తార్కిక yosys_write_versions = నిజం;
స్థిరరాశి అక్షరం* yosys_maybe_సంస్కరణ() {
	ఒకవేళ (yosys_write_versions)
		తిరిగి yosys_సంస్కరణ_str;
	లేకపోతే
		తిరిగి "Yosys";
}

RTLIL::Design *yosys_design = ఖాళీ;
NewCellTypes yosys_celltypes;

#నిర్వచితమైతే YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = ఖాళీ;
Tcl_Interp *yosys_sdc_interp = ఖాళీ;
#అయితే_ముగింపు

std::సమితి<std::string> yosys_ఆదానం_files, yosys_output_files;

తార్కిక memhasher_చురుకైన = అబద్ధం;
uint32_t memhasher_rng = 123456;
std::సదిశ<శూన్య*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

తార్కిక Multithreading::చురుకైన_2 = అబద్ధం;

శూన్య init_share_dirname();
శూన్య init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!చురుకైన_2);
	చురుకైన_2 = నిజం;
}

Multithreading::~Multithreading() {
	log_assert(చురుకైన_2);
	చురుకైన_2 = అబద్ధం;
}

శూన్య Autoidx::ensure_at_కనిష్ఠ(పూర్ణాంకం v) {
	log_assert(!Multithreading::చురుకైన());
	విలువ = std::max(విలువ, v);
}

పూర్ణాంకం Autoidx::క్రియాకారకం++(పూర్ణాంకం) {
	log_assert(!Multithreading::చురుకైన());
	తిరిగి విలువ++;
}

శూన్య memhasher_on()
{
#ఒకవేళ defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += సమయం_2(ఖాళీ) << 16 ^ getpid();
#అయితే_ముగింపు
	memhasher_store.resize(0x10000);
	memhasher_చురుకైన = నిజం;
}

శూన్య memhasher_off()
{
	కొరకు (స్వయంచాలక p : memhasher_store)
		ఒకవేళ (p) విడుదల(p);
	memhasher_store.clear();
	memhasher_చురుకైన = అబద్ధం;
}

శూన్య memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	పూర్ణాంకం కొలత, సూచీ = (memhasher_rng >> 4) & 0xffff;
	ఎంపిక (memhasher_rng & 7) {
		స్థితి 0: కొలత =   16; ఆపు;
		స్థితి 1: కొలత =  256; ఆపు;
		స్థితి 2: కొలత = 1024; ఆపు;
		స్థితి 3: కొలత = 4096; ఆపు;
		అప్రమేయ: కొలత = 0;
	}
	ఒకవేళ (సూచీ < 16) కొలత *= 16;
	memhasher_store[సూచీ] = realloc(memhasher_store[సూచీ], కొలత);
}

శూన్య yosys_banner()
{
	సంవర్గమానం("\n");
	సంవర్గమానం(" /----------------------------------------------------------------------------\\\n");
	సంవర్గమానం(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	సంవర్గమానం(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	సంవర్గమానం(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	సంవర్గమానం(" \\----------------------------------------------------------------------------/\n");
	సంవర్గమానం(" %s\n", yosys_maybe_సంస్కరణ());
}

#ఒకవేళ defined(YOSYS_ENABLE_SPAWN)
పూర్ణాంకం నిరంతరం_command(స్థిరరాశి std::string &command, std::function<శూన్య(స్థిరరాశి std::string&)> process_పంక్తి)
{
	ఒకవేళ (!process_పంక్తి)
		తిరిగి system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	ఒకవేళ (f == శూన్యసూచిక)
		తిరిగి -1;

	std::string పంక్తి;
	అక్షరం logbuf[128];
	అయితే (నిజం) {
		ఒకవేళ (fgets(logbuf, 128, f) != ఖాళీ) {
			పంక్తి += logbuf;
			ఒకవేళ (!పంక్తి.ఖాళీలు() && పంక్తి.వెనుక() == '\n')
				process_పంక్తి(పంక్తి), పంక్తి.clear();
		} లేకపోతే ఒకవేళ (ferror(f) && errno == EINTR) {
			clearerr(f);
			కొనసాగు;
		} లేకపోతే {
			ఆపు;
		}
	}
	ఒకవేళ (!పంక్తి.ఖాళీలు())
		process_పంక్తి(పంక్తి);

	పూర్ణాంకం ret = pclose(f);
	ఒకవేళ (ret < 0)
		తిరిగి -2;
#నిర్వచితమైతే _WIN32
	తిరిగి ret;
#లేకపోతే
	ఒకవేళ (!WIFEXITED(ret))
		తిరిగి -1;
	తిరిగి WEXITSTATUS(ret);
#అయితే_ముగింపు
}
#అయితే_ముగింపు

తార్కిక already_అమరిక = అబద్ధం;
తార్కిక already_shutdown = అబద్ధం;

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.కూడు_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(శూన్య)_;
	విసురు py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#అయితే_ముగింపు

శూన్య yosys_అమరిక()
{
	ఒకవేళ(already_అమరిక)
		తిరిగి;
	already_అమరిక = నిజం;
	already_shutdown = అబద్ధం;

	StaticTwines::init();

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	ఒకవేళ (!Py_IsInitialized()) {
		PyImport_AppendInittab((అక్షరం*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((అక్షరం*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((అక్షరం*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_నిరంతరం_Simple_String("import sys");
		సంకేతం(SIGINT, SIG_DFL);
	}
#అయితే_ముగింపు

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_రిజిస్టరు();
	yosys_design = కొత్త RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_తెలిసిన;
	log_నెట్టు();
}

తార్కిక yosys_already_అమరిక()
{
	తిరిగి already_అమరిక;
}

శూన్య yosys_shutdown()
{
	ఒకవేళ(already_shutdown)
		తిరిగి;
	already_అమరిక = అబద్ధం;
	already_shutdown = నిజం;
	log_తీయి();

	Pass::పూర్తి_రిజిస్టరు();

	తొలగించు yosys_design;
	yosys_design = ఖాళీ;

	logger().clear();

#నిర్వచితమైతే YOSYS_ENABLE_TCL
	ఒకవేళ (yosys_tcl_interp != ఖాళీ) {
		ఒకవేళ (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = ఖాళీ;
	}
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_PLUGINS
	కొరకు (స్వయంచాలక &it : loaded_plugins)
		dlclose(it.రెండవ);

	loaded_plugins.clear();
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#అయితే_ముగింపు
	loaded_plugin_aliases.clear();
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	Py_Finalize();
#అయితే_ముగింపు
}

స్థిరరాశి std::string *create_id_ఉపసర్గ(std::string_view file, పూర్ణాంకం పంక్తి, std::string_view func)
{
#నిర్వచితమైతే _WIN32
	కొలత_t pos = file.కనుగొను_చివరి_యొక్క("/\\");
#లేకపోతే
	కొలత_t pos = file.కనుగొను_చివరి_యొక్క('/');
#అయితే_ముగింపు
	ఒకవేళ (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.కనుగొను_చివరి_యొక్క(':');
	ఒకవేళ (pos != std::string_view::npos)
		func = func.substr(pos+1);

	తిరిగి కొత్త std::string(stringf("$auto$%s:%d:%s$", file, పంక్తి, func));
}

RTLIL::Design *yosys_get_design()
{
	తిరిగి yosys_design;
}

స్థిరరాశి అక్షరం *create_prompt(RTLIL::Design *design, పూర్ణాంకం recursion_counter, తార్కిక newline)
{
	స్థిర అక్షరం బఫరు[100];
	std::string str = newline ? "\n" : "";
	ఒకవేళ (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	ఒకవేళ (!design->selected_చురుకైన_module.ఖాళీలు())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_చురుకైన_module)).c_str());
	ఒకవేళ (!design->నిండిన_selection()) {
		ఒకవేళ (design->selected_చురుకైన_module.ఖాళీలు())
			str += "*";
		లేకపోతే ఒకవేళ (design->selection().selected_modules.కొలత() != 1 || design->selection().selected_members.కొలత() != 0 ||
				design->selection().selected_modules.లెక్క(design->selected_చురుకైన_module) == 0)
			str += "*";
	}
	snprintf(బఫరు, 100, "%s> ", str.c_str());
	తిరిగి బఫరు;
}

std::సదిశ<std::string> glob_filename(స్థిరరాశి std::string &filename_నమూనా)
{
	std::సదిశ<std::string> results;

#ఒకవేళ defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.నెట్టు_వెనుక(filename_నమూనా);
#లేకపోతే
	glob_t globbuf;

	పూర్ణాంకం err = glob(filename_నమూనా.c_str(), 0, ఖాళీ, &globbuf);

	ఒకవేళ(err == 0) {
		కొరకు (కొలత_t i = 0; i < globbuf.gl_pathc; i++)
			results.నెట్టు_వెనుక(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} లేకపోతే {
		results.నెట్టు_వెనుక(filename_నమూనా);
	}
#అయితే_ముగింపు

	తిరిగి results;
}

శూన్య rewrite_filename(std::string &filename)
{
	ఒకవేళ (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_కొలత(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_కొలత(filename)-2);
	ఒకవేళ (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#నిర్వచితం_కాకపోతే _WIN32
	ఒకవేళ (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#అయితే_ముగింపు
}

#నిర్వచితమైతే YOSYS_ENABLE_TCL

// defined in tclapi.cc
బాహ్య పూర్ణాంకం yosys_tcl_interp_init(Tcl_Interp *interp);

బాహ్య Tcl_Interp *yosys_get_tcl_interp()
{
	ఒకవేళ (yosys_tcl_interp == ఖాళీ) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	తిరిగి yosys_tcl_interp;
}

// Also see SdcPass
నిర్మాణం TclPass : బహిరంగ Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	శూన్య help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		సంవర్గమానం("\n");
		సంవర్గమానం("    tcl <filename> [args]\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("This command executes the tcl commands in the specified file.\n");
		సంవర్గమానం("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("The tcl command 'yosys -import' can be used to import all yosys\n");
		సంవర్గమానం("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		సంవర్గమానం("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		సంవర్గమానం("in order to avoid a name collision with the built in commands.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("If any arguments are specified, these arguments are provided to the script via\n");
		సంవర్గమానం("the standard $argc and $argv variables.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("Note, tcl will not receive the output of any yosys command. If the output\n");
		సంవర్గమానం("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		సంవర్గమానం("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		సంవర్గమానం("The 'result.string' value is then used as the tcl output value of the command.\n");
		సంవర్గమానం("\n");
	}
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *) override {
		ఒకవేళ (వాదనలు_2.కొలత() < 2)
			log_cmd_error("Missing script file.\n");

		std::సదిశ<Tcl_Obj*> script_వాదనలు;
		కొరకు (స్వయంచాలక it = వాదనలు_2.begin() + 2; it != వాదనలు_2.చివర(); ++it)
			script_వాదనలు.నెట్టు_వెనుక(Tcl_NewStringObj((*it).c_str(), (*it).కొలత()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_సమితి_Var2(interp, Tcl_NewStringObj("argc", 4), ఖాళీ, Tcl_NewIntObj(script_వాదనలు.కొలత()), 0);
		Tcl_Obj_సమితి_Var2(interp, Tcl_NewStringObj("argv", 4), ఖాళీ, Tcl_NewListObj(script_వాదనలు.కొలత(), script_వాదనలు.దత్తాంశం()), 0);
		Tcl_Obj_సమితి_Var2(interp, Tcl_NewStringObj("argv0", 5), ఖాళీ, Tcl_NewStringObj(వాదనలు_2[1].c_str(), వాదనలు_2[1].కొలత()), 0);
		ఒకవేళ (Tcl_EvalFile(interp, వాదనలు_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ఫలితం(interp));
		Tcl_Release(interp);
	}
} TclPass;

#అయితే_ముగింపు

#ఒకవేళ defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string మార్గం_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	కొరకు (పూర్ణాంకం tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", మార్గం_2.దత్తాంశం(), మార్గం_2.కొలత());
		ఒకవేళ (buflen < (ssize_t)మార్గం_2.కొలత())
			ఆపు;
		లేకపోతే
			మార్గం_2.resize(మార్గం_2.కొలత() * 2);
	}
	ఒకవేళ (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		మార్గం_2.resize(0);
	} లేకపోతే {
		అయితే (buflen > 0 && మార్గం_2[buflen-1] != '/')
			buflen--;
		మార్గం_2.resize(buflen);
	}
	తిరిగి మార్గం_2;
}
#లేదావేళ defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#నిర్వచితమైతే __NetBSD__
	పూర్ణాంకం mib[4] = {CTL_KERN, KERN_PROC_వాదనలు, getpid(), KERN_PROC_PATHNAME};
#లేకపోతే
	పూర్ణాంకం mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#అయితే_ముగింపు
	కొలత_t buflen;
	అక్షరం *బఫరు;
	std::string మార్గం_2;
	ఒకవేళ (sysctl(mib, 4, ఖాళీ, &buflen, ఖాళీ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	బఫరు = (అక్షరం*)జ్ఞాపకం_ఇవ్వు(buflen);
	ఒకవేళ (బఫరు == ఖాళీ)
		log_error("malloc failed: %s\n", strerror(errno));
	ఒకవేళ (sysctl(mib, 4, బఫరు, &buflen, ఖాళీ, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	అయితే (buflen > 0 && బఫరు[buflen-1] != '/')
		buflen--;
	మార్గం_2.కేటాయించు(బఫరు, buflen);
	విడుదల(బఫరు);
	తిరిగి మార్గం_2;
}
#లేదావేళ defined(__APPLE__)
std::string proc_self_dirname()
{
	అక్షరం *మార్గం_2 = ఖాళీ;
	uint32_t buflen = 0;
	అయితే (NSGet_Executable_మార్గం(మార్గం_2, &buflen) != 0)
		మార్గం_2 = (అక్షరం *) realloc((శూన్య *) మార్గం_2, buflen);
	అక్షరం resolved[మార్గం_MAX];
	ఒకవేళ (realpath(మార్గం_2, resolved) != ఖాళీ) {
		విడుదల(మార్గం_2);
		మార్గం_2 = strdup(resolved);
		buflen = అక్షరశ్రేణి_పొడవు(మార్గం_2);
	}
	అయితే (buflen > 0 && మార్గం_2[buflen-1] != '/')
		buflen--;
	std::string str(మార్గం_2, buflen);
	విడుదల(మార్గం_2);
	తిరిగి str;
}
#లేదావేళ defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	ఒకవేళ (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.కొలత()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, wbinpath.దత్తాంశం(), wbinpath.కొలత(), ఖాళీ, 0, ఖాళీ, ఖాళీ));
	ఒకవేళ (Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, wbinpath.దత్తాంశం(), wbinpath.కొలత(), &ubinpath[0], ubinpath.కొలత(), ఖాళీ, ఖాళీ) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	తిరిగి ubinpath;

}
#లేదావేళ defined(__wasm)
std::string proc_self_dirname()
{
	తిరిగి "/";
}
#లేదావేళ defined(__OpenBSD__) || defined(__HAIKU__)
అక్షరం yosys_మార్గం[మార్గం_MAX];
అక్షరం *yosys_argv0;

std::string proc_self_dirname(శూన్య)
{
	అక్షరం buf[మార్గం_MAX + 1] = "", *మార్గం_2, *p;
	// if case argv[0] contains a valid path, return it
	ఒకవేళ (అక్షరశ్రేణి_పొడవు(yosys_మార్గం) > 0) {
		p = strrchr(yosys_మార్గం, '/');
		snprintf(buf, పరిమాణం buf, "%*s/", (పూర్ణాంకం)(yosys_మార్గం - p), yosys_మార్గం);
		తిరిగి buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	మార్గం_2 = strdup(getenv("PATH"));
	ఒకవేళ (!మార్గం_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	కొరకు (p = strtok(మార్గం_2, ":"); p; p = strtok(ఖాళీ, ":")) {
		snprintf(buf, పరిమాణం buf, "%s/%s", p, yosys_argv0);
		ఒకవేళ (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			విడుదల(మార్గం_2);
			తిరిగి buf;
		}
	}
	విడుదల(మార్గం_2);
	log_error("Can't determine yosys executable path.\n");
	తిరిగి ఖాళీ;
}
#లేకపోతే
	#దోషం "Don't know how to determine process executable base path!"
#అయితే_ముగింపు

#ఒకవేళ defined(__wasm)
శూన్య init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#లేకపోతే
శూన్య init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ఒకవేళ (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_మార్గం_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		స్థిరరాశి అక్షరం *share_మార్గం = PyUnicode_AsUTF8(share_మార్గం_obj);
		yosys_share_dirname = std::string(share_మార్గం);
		తిరిగి;
	}
#  endif
	std::string proc_self_మార్గం = proc_self_dirname();
#  ఒకవేళ defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_మార్గం = proc_self_మార్గం + "share\\";
	ఒకవేళ (check_directory_exists(proc_share_మార్గం, నిజం)) {
		yosys_share_dirname = proc_share_మార్గం;
		తిరిగి;
	}
	proc_share_మార్గం = proc_self_మార్గం + "..\\share\\";
	ఒకవేళ (check_directory_exists(proc_share_మార్గం, నిజం)) {
		yosys_share_dirname = proc_share_మార్గం;
		తిరిగి;
	}
#  లేకపోతే
	std::string proc_share_మార్గం = proc_self_మార్గం + "share/";
	ఒకవేళ (check_directory_exists(proc_share_మార్గం, నిజం)) {
		yosys_share_dirname = proc_share_మార్గం;
		తిరిగి;
	}
	proc_share_మార్గం = proc_self_మార్గం + "../share/" + proc_కార్యక్రమం_ఉపసర్గ()+ "yosys/";
	ఒకవేళ (check_directory_exists(proc_share_మార్గం, నిజం)) {
		yosys_share_dirname = proc_share_మార్గం;
		తిరిగి;
	}
	proc_share_మార్గం = YOSYS_DATDIR "/";
	ఒకవేళ (check_directory_exists(proc_share_మార్గం, నిజం)) {
		yosys_share_dirname = proc_share_మార్గం;
		తిరిగి;
	}
#  endif
}
#అయితే_ముగింపు

శూన్య init_abc_executable_name()
{
#నిర్వచితమైతే ABCEXTERNAL
	std::string exe_file;
	ఒకవేళ (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} లేకపోతే {
		yosys_abc_executable = ABCEXTERNAL;
	}
#లేకపోతే
	yosys_abc_executable = proc_self_dirname() + proc_కార్యక్రమం_ఉపసర్గ()+ "yosys-abc";
#  ifdef _WIN32
	ఒకవేళ (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_కార్యక్రమం_ఉపసర్గ() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_కార్యక్రమం_ఉపసర్గ() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	ఒకవేళ (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_మార్గం_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		స్థిరరాశి అక్షరం *abc_మార్గం = PyUnicode_AsUTF8(abc_మార్గం_obj);
		yosys_abc_executable = std::string(abc_మార్గం);
	}
#  endif
#అయితే_ముగింపు
}

std::string proc_share_dirname()
{
	ఒకవేళ (yosys_share_dirname.ఖాళీలు())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	తిరిగి yosys_share_dirname;
}

std::string proc_కార్యక్రమం_ఉపసర్గ()
{
	తిరిగి YOSYS_కార్యక్రమం_ఉపసర్గ;
}

తార్కిక fgetline(FILE *f, std::string &బఫరు)
{
	బఫరు = "";
	అక్షరం ఖండం[4096];
	అయితే (1) {
		ఒకవేళ (fgets(ఖండం, 4096, f) == ఖాళీ)
			తిరిగి అబద్ధం;
		బఫరు += ఖండం;
		ఒకవేళ (బఫరు.కొలత() > 0 && (బఫరు[బఫరు.కొలత()-1] == '\n' ||  బఫరు[బఫరు.కొలత()-1] == '\r')) {
			అయితే (బఫరు.కొలత() > 0 && (బఫరు[బఫరు.కొలత()-1] == '\n' ||  బఫరు[బఫరు.కొలత()-1] == '\r'))
				బఫరు.resize(బఫరు.కొలత()-1);
			తిరిగి నిజం;
		}
	}
}

స్థిర శూన్య handle_label(std::string &command, తార్కిక &from_గమ్యం_చురుకైన, స్థిరరాశి std::string &నిరంతరం_from, స్థిరరాశి std::string &నిరంతరం_గమ్యం)
{
	పూర్ణాంకం pos = 0;
	std::string label;

	అయితే (pos < Get_కొలత(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	ఒకవేళ (pos < Get_కొలత(command) && command[pos] == '#')
		తిరిగి;

	అయితే (pos < Get_కొలత(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	ఒకవేళ (Get_కొలత(label) > 1 && label.వెనుక() == ':')
	{
		label = label.substr(0, Get_కొలత(label)-1);
		command = command.substr(pos);

		ఒకవేళ (label == నిరంతరం_from)
			from_గమ్యం_చురుకైన = నిజం;
		లేకపోతే ఒకవేళ (label == నిరంతరం_గమ్యం || (నిరంతరం_from == నిరంతరం_గమ్యం && !నిరంతరం_from.ఖాళీలు()))
			from_గమ్యం_చురుకైన = అబద్ధం;
	}
}

తార్కిక నిరంతరం_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_గమ్యం_label)
{
	ఒకవేళ (design == శూన్యసూచిక)
		design = yosys_design;

	ఒకవేళ (command == "auto") {
	  std::string filename_trim = filename;

	  స్వయంచాలక has_extension = [](స్థిరరాశి std::string& filename, స్థిరరాశి std::string& extension) {
	    ఒకవేళ (filename.కొలత() >= extension.కొలత()) {
	      తిరిగి filename.compare(filename.కొలత() - extension.కొలత(), extension.కొలత(), extension) == 0;
	    }
	    తిరిగి అబద్ధం;
	  };

	  ఒకవేళ (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.కొలత() - 3);
	  }

	  ఒకవేళ (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } లేకపోతే ఒకవేళ (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } లేకపోతే ఒకవేళ (filename == "-") {
	    command = "script";
	  } లేకపోతే {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	ఒకవేళ (command == "script")
	{
		std::string నిరంతరం_from, నిరంతరం_గమ్యం;
		తార్కిక from_గమ్యం_చురుకైన = నిజం;

		ఒకవేళ (from_గమ్యం_label != ఖాళీ) {
			కొలత_t pos = from_గమ్యం_label->కనుగొను(':');
			ఒకవేళ (pos == std::string::npos) {
				నిరంతరం_from = *from_గమ్యం_label;
				నిరంతరం_గమ్యం = *from_గమ్యం_label;
			} లేకపోతే {
				నిరంతరం_from = from_గమ్యం_label->substr(0, pos);
				నిరంతరం_గమ్యం = from_గమ్యం_label->substr(pos+1);
			}
			from_గమ్యం_చురుకైన = నిరంతరం_from.ఖాళీలు();
		}

		సంవర్గమానం("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		ఒకవేళ (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_ఆదానం_files.చొప్పించు(filename);
		}

		ఒకవేళ (f == ఖాళీ)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		పూర్ణాంకం backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		ప్రయత్నించు {
			std::string command;
			అయితే (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				అయితే (!command.ఖాళీలు() && command[command.కొలత()-1] == '\\') {
					std::string తదుపరి_పంక్తి;
					ఒకవేళ (!fgetline(f, తదుపరి_పంక్తి))
						ఆపు;
					Frontend::current_script_lineno++;
					command.resize(command.కొలత()-1);
					command += తదుపరి_పంక్తి;
				}
				handle_label(command, from_గమ్యం_చురుకైన, నిరంతరం_from, నిరంతరం_గమ్యం);
				ఒకవేళ (from_గమ్యం_చురుకైన) {
					Pass::call(design, command);
					design->check();
				}
			}

			ఒకవేళ (!command.ఖాళీలు()) {
				handle_label(command, from_గమ్యం_చురుకైన, నిరంతరం_from, నిరంతరం_గమ్యం);
				ఒకవేళ (from_గమ్యం_చురుకైన) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		పట్టుకో (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			విసురు;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		ఒకవేళ (filename != "-")
			fclose(f);

		తిరిగి నిజం;
	}

	ఒకవేళ (command == "tcl") {
		Pass::call(design, సదిశ<string>({command, filename}));
		తిరిగి నిజం;
	}

	సంవర్గమానం("\n");
	ఒకవేళ (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} లేకపోతే {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	ఒకవేళ (command[0] == ' ') {
		స్వయంచాలక argv = విభజించు_టోకెన్లు("read" + command);
		argv.నెట్టు_వెనుక(filename);
		Pass::call(design, argv);
	} లేకపోతే
		Frontend::frontend_call(design, ఖాళీ, filename, command);

	design->check();
	తిరిగి అబద్ధం;
}

శూన్య నిరంతరం_pass(std::string command, RTLIL::Design *design)
{
	ఒకవేళ (design == శూన్యసూచిక)
		design = yosys_design;

	సంవర్గమానం("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

శూన్య నిరంతరం_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	ఒకవేళ (design == శూన్యసూచిక)
		design = yosys_design;

	ఒకవేళ (command == "auto") {
		ఒకవేళ (filename.కొలత() > 2 && filename.compare(filename.కొలత()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		లేకపోతే ఒకవేళ (filename.కొలత() > 3 && filename.compare(filename.కొలత()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		లేకపోతే ఒకవేళ (filename.కొలత() > 3 && filename.compare(filename.కొలత()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		లేకపోతే ఒకవేళ (filename.కొలత() > 3 && filename.compare(filename.కొలత()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		లేకపోతే ఒకవేళ (filename.కొలత() > 4 && filename.compare(filename.కొలత()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		లేకపోతే ఒకవేళ (filename.కొలత() > 5 && filename.compare(filename.కొలత()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		లేకపోతే ఒకవేళ (filename.కొలత() > 5 && filename.compare(filename.కొలత()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		లేకపోతే ఒకవేళ (filename.కొలత() > 5 && filename.compare(filename.కొలత()-5, std::string::npos, ".json") == 0)
			command = "json";
		లేకపోతే ఒకవేళ (filename == "-")
			command = "rtlil";
		లేకపోతే ఒకవేళ (filename.ఖాళీలు())
			తిరిగి;
		లేకపోతే
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	ఒకవేళ (filename.ఖాళీలు())
		filename = "-";

	సంవర్గమానం("\n");
	ఒకవేళ (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} లేకపోతే {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, ఖాళీ, filename, command);
}

#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
స్థిర అక్షరం *readline_cmd_generator(స్థిరరాశి అక్షరం *పాఠ్యం, పూర్ణాంకం state)
{
	స్థిర std::map<std::string, Pass*>::iterator it;
	స్థిర పూర్ణాంకం len;

	ఒకవేళ (!state) {
		it = pass_రిజిస్టరు.begin();
		len = అక్షరశ్రేణి_పొడవు(పాఠ్యం);
	}

	కొరకు (; it != pass_రిజిస్టరు.చివర(); it++) {
		ఒకవేళ (it->మొదటి.compare(0, len, పాఠ్యం) == 0)
			తిరిగి strdup((it++)->మొదటి.c_str());
	}
	తిరిగి ఖాళీ;
}

స్థిర అక్షరం *readline_obj_generator(స్థిరరాశి అక్షరం *పాఠ్యం, పూర్ణాంకం state)
{
	స్థిర std::సదిశ<అక్షరం*> obj_పేర్లు;
	స్థిర కొలత_t idx;

	ఒకవేళ (!state)
	{
		idx = 0;
		obj_పేర్లు.clear();

		RTLIL::Design *design = yosys_get_design();
		పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(పాఠ్యం);

		ఒకవేళ (design->selected_చురుకైన_module.ఖాళీలు())
		{
			కొరకు (స్వయంచాలక mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				ఒకవేళ (mod_name.compare(0, len, పాఠ్యం) == 0)
					obj_పేర్లు.నెట్టు_వెనుక(strdup(mod_name.c_str()));
			}
		}
		లేకపోతే ఒకవేళ (design->module(design->selected_చురుకైన_module) != శూన్యసూచిక)
		{
			RTLIL::Module *module = design->module(design->selected_చురుకైన_module);

			కొరకు (స్వయంచాలక w : module->wires())
				ఒకవేళ (w->name.unescape().compare(0, len, పాఠ్యం) == 0)
					obj_పేర్లు.నెట్టు_వెనుక(strdup(w->name.unescape().c_str()));

			కొరకు (స్వయంచాలక &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.మొదటి);
				ఒకవేళ (mem_name.compare(0, len, పాఠ్యం) == 0)
					obj_పేర్లు.నెట్టు_వెనుక(strdup(mem_name.c_str()));
			}

			కొరకు (స్వయంచాలక cell : module->cells())
				ఒకవేళ (cell->name.unescape().compare(0, len, పాఠ్యం) == 0)
					obj_పేర్లు.నెట్టు_వెనుక(strdup(cell->name.unescape().c_str()));

			కొరకు (స్వయంచాలక &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.మొదటి);
				ఒకవేళ (proc_name.compare(0, len, పాఠ్యం) == 0)
					obj_పేర్లు.నెట్టు_వెనుక(strdup(proc_name.c_str()));
			}
		}

		std::క్రమీకరించు(obj_పేర్లు.begin(), obj_పేర్లు.చివర());
	}

	ఒకవేళ (idx < obj_పేర్లు.కొలత())
		తిరిగి strdup(obj_పేర్లు[idx++]);

	idx = 0;
	obj_పేర్లు.clear();
	తిరిగి ఖాళీ;
}

స్థిర అక్షరం **readline_completion(స్థిరరాశి అక్షరం *పాఠ్యం, పూర్ణాంకం ప్రారంభం, పూర్ణాంకం)
{
	ఒకవేళ (ప్రారంభం == 0)
		తిరిగి rl_completion_matches(పాఠ్యం, readline_cmd_generator);
	ఒకవేళ (అక్షరశ్రేణి_పోలిక(rl_పంక్తి_బఫరు, "read_", 5) && అక్షరశ్రేణి_పోలిక(rl_పంక్తి_బఫరు, "write_", 6))
		తిరిగి rl_completion_matches(పాఠ్యం, readline_obj_generator);
	తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

శూన్య shell(RTLIL::Design *design)
{
	స్థిర పూర్ణాంకం recursion_counter = 0;

	recursion_counter++;
	స్వయంచాలక guard = logger().error_throw_scope();

#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (అక్షరం*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_మాట_break_characters = (అక్షరం*)" \t\n";
#అయితే_ముగింపు

	అక్షరం *command = ఖాళీ;
#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	అయితే ((command = readline(create_prompt(design, recursion_counter))) != ఖాళీ)
	{
#లేకపోతే
	అక్షరం command_బఫరు[4096];
	అయితే (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		ఒకవేళ ((command = fgets(command_బఫరు, 4096, stdin)) == ఖాళీ)
			ఆపు;
#అయితే_ముగింపు
		ఒకవేళ (command[strspn(command, " \t\r\n")] == 0) {
#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			విడుదల(command);
#అయితే_ముగింపు
			కొనసాగు;
		}
#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		కూడు_history(command);
#అయితే_ముగింపు

		అక్షరం *p = command + strspn(command, " \t\r\n");
		ఒకవేళ (!అక్షరశ్రేణి_పోలిక(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			ఒకవేళ (*p == 0)
				ఆపు;
		}

		ప్రయత్నించు {
			log_assert(design->selection_దొంతర.కొలత() == 1);
			Pass::call(design, command);
		} పట్టుకో (log_cmd_error_exception) {
			అయితే (design->selection_దొంతర.కొలత() > 1)
				design->తీయి_selection();
			log_reset_దొంతర();
		}
		design->check();
#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		ఒకవేళ (command)
			విడుదల(command);
#అయితే_ముగింపు
	}
	ఒకవేళ (command == ఖాళీ)
		రాయి("exit\n");
#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	లేకపోతే
		విడుదల(command);
#అయితే_ముగింపు
	recursion_counter--;
}

నిర్మాణం ShellPass : బహిరంగ Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	శూన్య help() override {
		సంవర్గమానం("\n");
		సంవర్గమానం("    shell\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("This command enters the interactive command mode. This can be useful\n");
		సంవర్గమానం("in a script to interrupt the script at a certain point and allow for\n");
		సంవర్గమానం("interactive inspection or manual synthesis of the design at this point.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("The command prompt of the interactive shell indicates the current\n");
		సంవర్గమానం("selection (see 'help select'):\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("    yosys>\n");
		సంవర్గమానం("        the entire design is selected\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("    yosys*>\n");
		సంవర్గమానం("        only part of the design is selected\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("    yosys [modname]>\n");
		సంవర్గమానం("        the entire module 'modname' is selected using 'select -module modname'\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("    yosys [modname]*>\n");
		సంవర్గమానం("        only part of current module 'modname' is selected\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		సంవర్గమానం("do not terminate yosys but return to the command prompt.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("This command is the default action if nothing else has been specified\n");
		సంవర్గమానం("on the command line.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		సంవర్గమానం("\n");
	}
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) override {
		extra_వాదనలు(వాదనలు_2, 1, design, అబద్ధం);
		shell(design);
	}
} ShellPass;

#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
నిర్మాణం HistoryPass : బహిరంగ Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	శూన్య help() override {
		సంవర్గమానం("\n");
		సంవర్గమానం("    history\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("This command prints all commands in the shell history buffer. This are\n");
		సంవర్గమానం("all commands executed in an interactive session, but not the commands\n");
		సంవర్గమానం("from executed scripts.\n");
		సంవర్గమానం("\n");
	}
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) override {
		extra_వాదనలు(వాదనలు_2, 1, design, అబద్ధం);
#నిర్వచితమైతే YOSYS_ENABLE_READLINE
		కొరకు(HIST_నమోదు **list = history_list(); *list != ఖాళీ; list++)
			సంవర్గమానం("%s\n", (*list)->పంక్తి);
#లేకపోతే
		కొరకు (పూర్ణాంకం i = where_history(); history_get(i); i++)
			సంవర్గమానం("%s\n", history_get(i)->పంక్తి);
#అయితే_ముగింపు
	}
} HistoryPass;
#అయితే_ముగింపు

నిర్మాణం ScriptCmdPass : బహిరంగ Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	శూన్య help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		సంవర్గమానం("\n");
		సంవర్గమానం("    script <filename> [<from_label>:<to_label>]\n");
		సంవర్గమానం("    script -scriptwire [selection]\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("This command executes the yosys commands in the specified file (default\n");
		సంవర్గమానం("behaviour), or commands embedded in the constant text value connected to the\n");
		సంవర్గమానం("selected wires.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("In the default (file) case, the 2nd argument can be used to only execute the\n");
		సంవర్గమానం("section of the file between the specified labels. An empty from label is\n");
		సంవర్గమానం("synonymous with the beginning of the file and an empty to label is synonymous\n");
		సంవర్గమానం("with the end of the file.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("If only one label is specified (without ':') then only the block\n");
		సంవర్గమానం("marked with that label (until the next label) is executed.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		సంవర్గమానం("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		సంవర్గమానం("'-module' mode can be exited by using the 'cd' command.\n");
		సంవర్గమానం("\n");
	}
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) override
	{
		తార్కిక scriptwire = అబద్ధం;

		కొలత_t argidx;
		కొరకు (argidx = 1; argidx < వాదనలు_2.కొలత(); argidx++) {
			ఒకవేళ (వాదనలు_2[argidx] == "-scriptwire") {
				scriptwire = నిజం;
				కొనసాగు;
			}
			ఆపు;
		}
		ఒకవేళ (scriptwire) {
			extra_వాదనలు(వాదనలు_2, argidx, design);

			కొరకు (స్వయంచాలక mod : design->selected_modules())
				కొరకు (స్వయంచాలక &c : mod->connections()) {
					ఒకవేళ (!c.మొదటి.is_wire())
						కొనసాగు;
					స్వయంచాలక w = c.మొదటి.as_wire();
					ఒకవేళ (!mod->selected(w))
						కొనసాగు;
					ఒకవేళ (!c.రెండవ.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					స్వయంచాలక v = c.రెండవ.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		లేకపోతే ఒకవేళ (వాదనలు_2.కొలత() < 2)
			log_cmd_error("Missing script file.\n");
		లేకపోతే ఒకవేళ (వాదనలు_2.కొలత() == 2)
			నిరంతరం_frontend(వాదనలు_2[1], "script", design);
		లేకపోతే ఒకవేళ (వాదనలు_2.కొలత() == 3)
			నిరంతరం_frontend(వాదనలు_2[1], "script", design, &వాదనలు_2[2]);
		లేకపోతే
			extra_వాదనలు(వాదనలు_2, 2, design, అబద్ధం);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_చివర
