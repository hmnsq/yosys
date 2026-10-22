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

#यदि_परिभाषितम् YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
#  include <tcl.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
नामाकाश py = pybind11;
#यदि_अन्तः

#यदि defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#नोचेद्यदि defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#अन्यथा
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  यदि defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#यदि_अन्तः

#यदि !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#यदि_अन्तः

#यदि defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
बाह्य "C" PyObject* PyInit_libyosys();
बाह्य "C" PyObject* PyInit_pyosys();
#अन्तर्भाव <signal.h>
#यदि_अन्तः

#अन्तर्भाव <limits.h>
#अन्तर्भाव <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
पूर्णाङ्क yosys_xtrace = 0;
तार्किक yosys_write_versions = सत्य;
ध्रुव वर्ण* yosys_maybe_संस्करणम्() {
	यदि (yosys_write_versions)
		प्रत्यागम yosys_संस्करणम्_str;
	अन्यथा
		प्रत्यागम "Yosys";
}

RTLIL::Design *yosys_design = रिक्त;
NewCellTypes yosys_celltypes;

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = रिक्त;
Tcl_Interp *yosys_sdc_interp = रिक्त;
#यदि_अन्तः

std::समुच्चयः<std::string> yosys_निवेशः_files, yosys_output_files;

तार्किक memhasher_सक्रिय = असत्य;
uint32_t memhasher_rng = 123456;
std::सदिशम्<शून्य*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

तार्किक Multithreading::सक्रिय_2 = असत्य;

शून्य init_share_dirname();
शून्य init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!सक्रिय_2);
	सक्रिय_2 = सत्य;
}

Multithreading::~Multithreading() {
	log_assert(सक्रिय_2);
	सक्रिय_2 = असत्य;
}

शून्य Autoidx::ensure_at_न्यूनतमम्(पूर्णाङ्क v) {
	log_assert(!Multithreading::सक्रिय());
	मानम् = std::max(मानम्, v);
}

पूर्णाङ्क Autoidx::संक्रियकः++(पूर्णाङ्क) {
	log_assert(!Multithreading::सक्रिय());
	प्रत्यागम मानम्++;
}

शून्य memhasher_on()
{
#यदि defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += time(रिक्त) << 16 ^ getpid();
#यदि_अन्तः
	memhasher_store.resize(0x10000);
	memhasher_सक्रिय = सत्य;
}

शून्य memhasher_off()
{
	प्रति (स्वचालित p : memhasher_store)
		यदि (p) मुक्त(p);
	memhasher_store.clear();
	memhasher_सक्रिय = असत्य;
}

शून्य memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	पूर्णाङ्क आकार, सूचकांक = (memhasher_rng >> 4) & 0xffff;
	विकल्प (memhasher_rng & 7) {
		पक्ष 0: आकार =   16; भङ्ग;
		पक्ष 1: आकार =  256; भङ्ग;
		पक्ष 2: आकार = 1024; भङ्ग;
		पक्ष 3: आकार = 4096; भङ्ग;
		स्वभाव: आकार = 0;
	}
	यदि (सूचकांक < 16) आकार *= 16;
	memhasher_store[सूचकांक] = realloc(memhasher_store[सूचकांक], आकार);
}

शून्य yosys_banner()
{
	लघुगणकः("\n");
	लघुगणकः(" /----------------------------------------------------------------------------\\\n");
	लघुगणकः(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	लघुगणकः(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	लघुगणकः(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	लघुगणकः(" \\----------------------------------------------------------------------------/\n");
	लघुगणकः(" %s\n", yosys_maybe_संस्करणम्());
}

#यदि defined(YOSYS_ENABLE_SPAWN)
पूर्णाङ्क सततम्_command(ध्रुव std::string &command, std::function<शून्य(ध्रुव std::string&)> process_पङ्क्तिः)
{
	यदि (!process_पङ्क्तिः)
		प्रत्यागम system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	यदि (f == शून्यसूचकः)
		प्रत्यागम -1;

	std::string पङ्क्तिः;
	वर्ण logbuf[128];
	यावत् (सत्य) {
		यदि (fgets(logbuf, 128, f) != रिक्त) {
			पङ्क्तिः += logbuf;
			यदि (!पङ्क्तिः.रिक्तानि() && पङ्क्तिः.पृष्ठभागः() == '\n')
				process_पङ्क्तिः(पङ्क्तिः), पङ्क्तिः.clear();
		} अन्यथा यदि (ferror(f) && errno == EINTR) {
			clearerr(f);
			अनुवर्त;
		} अन्यथा {
			भङ्ग;
		}
	}
	यदि (!पङ्क्तिः.रिक्तानि())
		process_पङ्क्तिः(पङ्क्तिः);

	पूर्णाङ्क ret = pclose(f);
	यदि (ret < 0)
		प्रत्यागम -2;
#यदि_परिभाषितम् _WIN32
	प्रत्यागम ret;
#अन्यथा
	यदि (!WIFEXITED(ret))
		प्रत्यागम -1;
	प्रत्यागम WEXITSTATUS(ret);
#यदि_अन्तः
}
#यदि_अन्तः

तार्किक already_सज्जता = असत्य;
तार्किक already_shutdown = असत्य;

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.सङ्कलय_object("__path__", py::list());
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
#यदि_अन्तः

शून्य yosys_सज्जता()
{
	यदि(already_सज्जता)
		प्रत्यागम;
	already_सज्जता = सत्य;
	already_shutdown = असत्य;

	StaticTwines::init();

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	यदि (!Py_IsInitialized()) {
		PyImport_AppendInittab((वर्ण*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((वर्ण*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((वर्ण*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_सततम्_Simple_String("import sys");
		सङ्केत(SIGINT, SIG_DFL);
	}
#यदि_अन्तः

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_पञ्जिकाकोशः();
	yosys_design = नूतन RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_ज्ञातम्;
	log_प्रक्षिप();
}

तार्किक yosys_already_सज्जता()
{
	प्रत्यागम already_सज्जता;
}

शून्य yosys_shutdown()
{
	यदि(already_shutdown)
		प्रत्यागम;
	already_सज्जता = असत्य;
	already_shutdown = सत्य;
	log_निष्कासय();

	Pass::समाप्तम्_पञ्जिकाकोशः();

	विलोपय yosys_design;
	yosys_design = रिक्त;

	logger().clear();

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
	यदि (yosys_tcl_interp != रिक्त) {
		यदि (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = रिक्त;
	}
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_PLUGINS
	प्रति (स्वचालित &it : loaded_plugins)
		dlclose(it.second);

	loaded_plugins.clear();
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#यदि_अन्तः
	loaded_plugin_aliases.clear();
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	Py_Finalize();
#यदि_अन्तः
}

ध्रुव std::string *create_id_उपसर्गः(std::string_view file, पूर्णाङ्क पङ्क्तिः, std::string_view func)
{
#यदि_परिभाषितम् _WIN32
	आकार_t pos = file.find_चरमः_सम्बन्धि("/\\");
#अन्यथा
	आकार_t pos = file.find_चरमः_सम्बन्धि('/');
#यदि_अन्तः
	यदि (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.find_चरमः_सम्बन्धि(':');
	यदि (pos != std::string_view::npos)
		func = func.substr(pos+1);

	प्रत्यागम नूतन std::string(stringf("$auto$%s:%d:%s$", file, पङ्क्तिः, func));
}

RTLIL::Design *yosys_get_design()
{
	प्रत्यागम yosys_design;
}

ध्रुव वर्ण *create_prompt(RTLIL::Design *design, पूर्णाङ्क recursion_गणक, तार्किक newline)
{
	स्थिर वर्ण बफर[100];
	std::string str = newline ? "\n" : "";
	यदि (recursion_गणक > 1)
		str += stringf("(%d) ", recursion_गणक);
	str += "yosys";
	यदि (!design->selected_सक्रिय_module.रिक्तानि())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_सक्रिय_module)).c_str());
	यदि (!design->पूर्णम्_selection()) {
		यदि (design->selected_सक्रिय_module.रिक्तानि())
			str += "*";
		अन्यथा यदि (design->selection().selected_modules.आकार() != 1 || design->selection().selected_members.आकार() != 0 ||
				design->selection().selected_modules.गणनम्(design->selected_सक्रिय_module) == 0)
			str += "*";
	}
	snprintf(बफर, 100, "%s> ", str.c_str());
	प्रत्यागम बफर;
}

std::सदिशम्<std::string> glob_filename(ध्रुव std::string &filename_आदर्शः)
{
	std::सदिशम्<std::string> results;

#यदि defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.प्रक्षिप_पृष्ठभागः(filename_आदर्शः);
#अन्यथा
	glob_t globbuf;

	पूर्णाङ्क err = glob(filename_आदर्शः.c_str(), 0, रिक्त, &globbuf);

	यदि(err == 0) {
		प्रति (आकार_t i = 0; i < globbuf.gl_pathc; i++)
			results.प्रक्षिप_पृष्ठभागः(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} अन्यथा {
		results.प्रक्षिप_पृष्ठभागः(filename_आदर्शः);
	}
#यदि_अन्तः

	प्रत्यागम results;
}

शून्य rewrite_filename(std::string &filename)
{
	यदि (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_आकार(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_आकार(filename)-2);
	यदि (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#यदि_अपरिभाषितम् _WIN32
	यदि (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#यदि_अन्तः
}

#यदि_परिभाषितम् YOSYS_ENABLE_TCL

// defined in tclapi.cc
बाह्य पूर्णाङ्क yosys_tcl_interp_init(Tcl_Interp *interp);

बाह्य Tcl_Interp *yosys_get_tcl_interp()
{
	यदि (yosys_tcl_interp == रिक्त) {
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
		लघुगणकः("\n");
		लघुगणकः("    tcl <filename> [args]\n");
		लघुगणकः("\n");
		लघुगणकः("This command executes the tcl commands in the specified file.\n");
		लघुगणकः("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		लघुगणकः("\n");
		लघुगणकः("The tcl command 'yosys -import' can be used to import all yosys\n");
		लघुगणकः("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		लघुगणकः("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		लघुगणकः("in order to avoid a name collision with the built in commands.\n");
		लघुगणकः("\n");
		लघुगणकः("If any arguments are specified, these arguments are provided to the script via\n");
		लघुगणकः("the standard $argc and $argv variables.\n");
		लघुगणकः("\n");
		लघुगणकः("Note, tcl will not receive the output of any yosys command. If the output\n");
		लघुगणकः("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		लघुगणकः("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		लघुगणकः("The 'result.string' value is then used as the tcl output value of the command.\n");
		लघुगणकः("\n");
	}
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *) override {
		यदि (तर्काः_2.आकार() < 2)
			log_cmd_error("Missing script file.\n");

		std::सदिशम्<Tcl_Obj*> script_तर्काः;
		प्रति (स्वचालित it = तर्काः_2.begin() + 2; it != तर्काः_2.अन्तः(); ++it)
			script_तर्काः.प्रक्षिप_पृष्ठभागः(Tcl_NewStringObj((*it).c_str(), (*it).आकार()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_समुच्चयः_Var2(interp, Tcl_NewStringObj("argc", 4), रिक्त, Tcl_NewIntObj(script_तर्काः.आकार()), 0);
		Tcl_Obj_समुच्चयः_Var2(interp, Tcl_NewStringObj("argv", 4), रिक्त, Tcl_NewListObj(script_तर्काः.आकार(), script_तर्काः.दत्त()), 0);
		Tcl_Obj_समुच्चयः_Var2(interp, Tcl_NewStringObj("argv0", 5), रिक्त, Tcl_NewStringObj(तर्काः_2[1].c_str(), तर्काः_2[1].आकार()), 0);
		यदि (Tcl_EvalFile(interp, तर्काः_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_परिणाम(interp));
		Tcl_Release(interp);
	}
} TclPass;

#यदि_अन्तः

#यदि defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string मार्गः_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	प्रति (पूर्णाङ्क tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", मार्गः_2.दत्त(), मार्गः_2.आकार());
		यदि (buflen < (ssize_t)मार्गः_2.आकार())
			भङ्ग;
		अन्यथा
			मार्गः_2.resize(मार्गः_2.आकार() * 2);
	}
	यदि (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		मार्गः_2.resize(0);
	} अन्यथा {
		यावत् (buflen > 0 && मार्गः_2[buflen-1] != '/')
			buflen--;
		मार्गः_2.resize(buflen);
	}
	प्रत्यागम मार्गः_2;
}
#नोचेद्यदि defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#यदि_परिभाषितम् __NetBSD__
	पूर्णाङ्क mib[4] = {CTL_KERN, KERN_PROC_तर्काः, getpid(), KERN_PROC_PATHNAME};
#अन्यथा
	पूर्णाङ्क mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#यदि_अन्तः
	आकार_t buflen;
	वर्ण *बफर;
	std::string मार्गः_2;
	यदि (sysctl(mib, 4, रिक्त, &buflen, रिक्त, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	बफर = (वर्ण*)स्मृतिं_देहि(buflen);
	यदि (बफर == रिक्त)
		log_error("malloc failed: %s\n", strerror(errno));
	यदि (sysctl(mib, 4, बफर, &buflen, रिक्त, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	यावत् (buflen > 0 && बफर[buflen-1] != '/')
		buflen--;
	मार्गः_2.नियोजय(बफर, buflen);
	मुक्त(बफर);
	प्रत्यागम मार्गः_2;
}
#नोचेद्यदि defined(__APPLE__)
std::string proc_self_dirname()
{
	वर्ण *मार्गः_2 = रिक्त;
	uint32_t buflen = 0;
	यावत् (NSGet_Executable_मार्गः(मार्गः_2, &buflen) != 0)
		मार्गः_2 = (वर्ण *) realloc((शून्य *) मार्गः_2, buflen);
	वर्ण resolved[मार्गः_MAX];
	यदि (realpath(मार्गः_2, resolved) != रिक्त) {
		मुक्त(मार्गः_2);
		मार्गः_2 = strdup(resolved);
		buflen = वाक्यदैर्घ्यम्(मार्गः_2);
	}
	यावत् (buflen > 0 && मार्गः_2[buflen-1] != '/')
		buflen--;
	std::string str(मार्गः_2, buflen);
	मुक्त(मार्गः_2);
	प्रत्यागम str;
}
#नोचेद्यदि defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	यदि (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.आकार()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, wbinpath.दत्त(), wbinpath.आकार(), रिक्त, 0, रिक्त, रिक्त));
	यदि (Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, wbinpath.दत्त(), wbinpath.आकार(), &ubinpath[0], ubinpath.आकार(), रिक्त, रिक्त) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	प्रत्यागम ubinpath;

}
#नोचेद्यदि defined(__wasm)
std::string proc_self_dirname()
{
	प्रत्यागम "/";
}
#नोचेद्यदि defined(__OpenBSD__) || defined(__HAIKU__)
वर्ण yosys_मार्गः[मार्गः_MAX];
वर्ण *yosys_argv0;

std::string proc_self_dirname(शून्य)
{
	वर्ण buf[मार्गः_MAX + 1] = "", *मार्गः_2, *p;
	// if case argv[0] contains a valid path, return it
	यदि (वाक्यदैर्घ्यम्(yosys_मार्गः) > 0) {
		p = strrchr(yosys_मार्गः, '/');
		snprintf(buf, परिमाण buf, "%*s/", (पूर्णाङ्क)(yosys_मार्गः - p), yosys_मार्गः);
		प्रत्यागम buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	मार्गः_2 = strdup(getenv("PATH"));
	यदि (!मार्गः_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	प्रति (p = strtok(मार्गः_2, ":"); p; p = strtok(रिक्त, ":")) {
		snprintf(buf, परिमाण buf, "%s/%s", p, yosys_argv0);
		यदि (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			मुक्त(मार्गः_2);
			प्रत्यागम buf;
		}
	}
	मुक्त(मार्गः_2);
	log_error("Can't determine yosys executable path.\n");
	प्रत्यागम रिक्त;
}
#अन्यथा
	#दोषः "Don't know how to determine process executable base path!"
#यदि_अन्तः

#यदि defined(__wasm)
शून्य init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#अन्यथा
शून्य init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	यदि (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_मार्गः_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ध्रुव वर्ण *share_मार्गः = PyUnicode_AsUTF8(share_मार्गः_obj);
		yosys_share_dirname = std::string(share_मार्गः);
		प्रत्यागम;
	}
#  endif
	std::string proc_self_मार्गः = proc_self_dirname();
#  यदि defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_मार्गः = proc_self_मार्गः + "share\\";
	यदि (check_directory_exists(proc_share_मार्गः, सत्य)) {
		yosys_share_dirname = proc_share_मार्गः;
		प्रत्यागम;
	}
	proc_share_मार्गः = proc_self_मार्गः + "..\\share\\";
	यदि (check_directory_exists(proc_share_मार्गः, सत्य)) {
		yosys_share_dirname = proc_share_मार्गः;
		प्रत्यागम;
	}
#  अन्यथा
	std::string proc_share_मार्गः = proc_self_मार्गः + "share/";
	यदि (check_directory_exists(proc_share_मार्गः, सत्य)) {
		yosys_share_dirname = proc_share_मार्गः;
		प्रत्यागम;
	}
	proc_share_मार्गः = proc_self_मार्गः + "../share/" + proc_क्रमादेशः_उपसर्गः()+ "yosys/";
	यदि (check_directory_exists(proc_share_मार्गः, सत्य)) {
		yosys_share_dirname = proc_share_मार्गः;
		प्रत्यागम;
	}
	proc_share_मार्गः = YOSYS_DATDIR "/";
	यदि (check_directory_exists(proc_share_मार्गः, सत्य)) {
		yosys_share_dirname = proc_share_मार्गः;
		प्रत्यागम;
	}
#  endif
}
#यदि_अन्तः

शून्य init_abc_executable_name()
{
#यदि_परिभाषितम् ABCEXTERNAL
	std::string exe_file;
	यदि (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} अन्यथा {
		yosys_abc_executable = ABCEXTERNAL;
	}
#अन्यथा
	yosys_abc_executable = proc_self_dirname() + proc_क्रमादेशः_उपसर्गः()+ "yosys-abc";
#  ifdef _WIN32
	यदि (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_क्रमादेशः_उपसर्गः() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_क्रमादेशः_उपसर्गः() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	यदि (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_मार्गः_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ध्रुव वर्ण *abc_मार्गः = PyUnicode_AsUTF8(abc_मार्गः_obj);
		yosys_abc_executable = std::string(abc_मार्गः);
	}
#  endif
#यदि_अन्तः
}

std::string proc_share_dirname()
{
	यदि (yosys_share_dirname.रिक्तानि())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	प्रत्यागम yosys_share_dirname;
}

std::string proc_क्रमादेशः_उपसर्गः()
{
	प्रत्यागम YOSYS_क्रमादेशः_उपसर्गः;
}

तार्किक fgetline(FILE *f, std::string &बफर)
{
	बफर = "";
	वर्ण खण्डम्[4096];
	यावत् (1) {
		यदि (fgets(खण्डम्, 4096, f) == रिक्त)
			प्रत्यागम असत्य;
		बफर += खण्डम्;
		यदि (बफर.आकार() > 0 && (बफर[बफर.आकार()-1] == '\n' ||  बफर[बफर.आकार()-1] == '\r')) {
			यावत् (बफर.आकार() > 0 && (बफर[बफर.आकार()-1] == '\n' ||  बफर[बफर.आकार()-1] == '\r'))
				बफर.resize(बफर.आकार()-1);
			प्रत्यागम सत्य;
		}
	}
}

स्थिर शून्य handle_label(std::string &command, तार्किक &from_गन्तव्यम्_सक्रिय, ध्रुव std::string &सततम्_from, ध्रुव std::string &सततम्_गन्तव्यम्)
{
	पूर्णाङ्क pos = 0;
	std::string label;

	यावत् (pos < Get_आकार(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	यदि (pos < Get_आकार(command) && command[pos] == '#')
		प्रत्यागम;

	यावत् (pos < Get_आकार(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	यदि (Get_आकार(label) > 1 && label.पृष्ठभागः() == ':')
	{
		label = label.substr(0, Get_आकार(label)-1);
		command = command.substr(pos);

		यदि (label == सततम्_from)
			from_गन्तव्यम्_सक्रिय = सत्य;
		अन्यथा यदि (label == सततम्_गन्तव्यम् || (सततम्_from == सततम्_गन्तव्यम् && !सततम्_from.रिक्तानि()))
			from_गन्तव्यम्_सक्रिय = असत्य;
	}
}

तार्किक सततम्_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_गन्तव्यम्_label)
{
	यदि (design == शून्यसूचकः)
		design = yosys_design;

	यदि (command == "auto") {
	  std::string filename_trim = filename;

	  स्वचालित has_extension = [](ध्रुव std::string& filename, ध्रुव std::string& extension) {
	    यदि (filename.आकार() >= extension.आकार()) {
	      प्रत्यागम filename.compare(filename.आकार() - extension.आकार(), extension.आकार(), extension) == 0;
	    }
	    प्रत्यागम असत्य;
	  };

	  यदि (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.आकार() - 3);
	  }

	  यदि (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } अन्यथा यदि (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } अन्यथा यदि (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } अन्यथा यदि (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } अन्यथा यदि (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } अन्यथा यदि (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } अन्यथा यदि (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } अन्यथा यदि (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } अन्यथा यदि (filename == "-") {
	    command = "script";
	  } अन्यथा {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	यदि (command == "script")
	{
		std::string सततम्_from, सततम्_गन्तव्यम्;
		तार्किक from_गन्तव्यम्_सक्रिय = सत्य;

		यदि (from_गन्तव्यम्_label != रिक्त) {
			आकार_t pos = from_गन्तव्यम्_label->find(':');
			यदि (pos == std::string::npos) {
				सततम्_from = *from_गन्तव्यम्_label;
				सततम्_गन्तव्यम् = *from_गन्तव्यम्_label;
			} अन्यथा {
				सततम्_from = from_गन्तव्यम्_label->substr(0, pos);
				सततम्_गन्तव्यम् = from_गन्तव्यम्_label->substr(pos+1);
			}
			from_गन्तव्यम्_सक्रिय = सततम्_from.रिक्तानि();
		}

		लघुगणकः("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		यदि (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_निवेशः_files.निवेशय(filename);
		}

		यदि (f == रिक्त)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		पूर्णाङ्क backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		यत्न {
			std::string command;
			यावत् (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				यावत् (!command.रिक्तानि() && command[command.आकार()-1] == '\\') {
					std::string अग्र_पङ्क्तिः;
					यदि (!fgetline(f, अग्र_पङ्क्तिः))
						भङ्ग;
					Frontend::current_script_lineno++;
					command.resize(command.आकार()-1);
					command += अग्र_पङ्क्तिः;
				}
				handle_label(command, from_गन्तव्यम्_सक्रिय, सततम्_from, सततम्_गन्तव्यम्);
				यदि (from_गन्तव्यम्_सक्रिय) {
					Pass::call(design, command);
					design->check();
				}
			}

			यदि (!command.रिक्तानि()) {
				handle_label(command, from_गन्तव्यम्_सक्रिय, सततम्_from, सततम्_गन्तव्यम्);
				यदि (from_गन्तव्यम्_सक्रिय) {
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

		यदि (filename != "-")
			fclose(f);

		प्रत्यागम सत्य;
	}

	यदि (command == "tcl") {
		Pass::call(design, सदिशम्<string>({command, filename}));
		प्रत्यागम सत्य;
	}

	लघुगणकः("\n");
	यदि (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} अन्यथा {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	यदि (command[0] == ' ') {
		स्वचालित argv = विभज_tokens("read" + command);
		argv.प्रक्षिप_पृष्ठभागः(filename);
		Pass::call(design, argv);
	} अन्यथा
		Frontend::frontend_call(design, रिक्त, filename, command);

	design->check();
	प्रत्यागम असत्य;
}

शून्य सततम्_pass(std::string command, RTLIL::Design *design)
{
	यदि (design == शून्यसूचकः)
		design = yosys_design;

	लघुगणकः("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

शून्य सततम्_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	यदि (design == शून्यसूचकः)
		design = yosys_design;

	यदि (command == "auto") {
		यदि (filename.आकार() > 2 && filename.compare(filename.आकार()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		अन्यथा यदि (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		अन्यथा यदि (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		अन्यथा यदि (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		अन्यथा यदि (filename.आकार() > 4 && filename.compare(filename.आकार()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		अन्यथा यदि (filename.आकार() > 5 && filename.compare(filename.आकार()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		अन्यथा यदि (filename.आकार() > 5 && filename.compare(filename.आकार()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		अन्यथा यदि (filename.आकार() > 5 && filename.compare(filename.आकार()-5, std::string::npos, ".json") == 0)
			command = "json";
		अन्यथा यदि (filename == "-")
			command = "rtlil";
		अन्यथा यदि (filename.रिक्तानि())
			प्रत्यागम;
		अन्यथा
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	यदि (filename.रिक्तानि())
		filename = "-";

	लघुगणकः("\n");
	यदि (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} अन्यथा {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, रिक्त, filename, command);
}

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
स्थिर वर्ण *readline_cmd_generator(ध्रुव वर्ण *text, पूर्णाङ्क state)
{
	स्थिर std::map<std::string, Pass*>::iterator it;
	स्थिर पूर्णाङ्क len;

	यदि (!state) {
		it = pass_पञ्जिकाकोशः.begin();
		len = वाक्यदैर्घ्यम्(text);
	}

	प्रति (; it != pass_पञ्जिकाकोशः.अन्तः(); it++) {
		यदि (it->first.compare(0, len, text) == 0)
			प्रत्यागम strdup((it++)->first.c_str());
	}
	प्रत्यागम रिक्त;
}

स्थिर वर्ण *readline_obj_generator(ध्रुव वर्ण *text, पूर्णाङ्क state)
{
	स्थिर std::सदिशम्<वर्ण*> obj_नामानि;
	स्थिर आकार_t idx;

	यदि (!state)
	{
		idx = 0;
		obj_नामानि.clear();

		RTLIL::Design *design = yosys_get_design();
		पूर्णाङ्क len = वाक्यदैर्घ्यम्(text);

		यदि (design->selected_सक्रिय_module.रिक्तानि())
		{
			प्रति (स्वचालित mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				यदि (mod_name.compare(0, len, text) == 0)
					obj_नामानि.प्रक्षिप_पृष्ठभागः(strdup(mod_name.c_str()));
			}
		}
		अन्यथा यदि (design->module(design->selected_सक्रिय_module) != शून्यसूचकः)
		{
			RTLIL::Module *module = design->module(design->selected_सक्रिय_module);

			प्रति (स्वचालित w : module->wires())
				यदि (w->name.unescape().compare(0, len, text) == 0)
					obj_नामानि.प्रक्षिप_पृष्ठभागः(strdup(w->name.unescape().c_str()));

			प्रति (स्वचालित &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.first);
				यदि (mem_name.compare(0, len, text) == 0)
					obj_नामानि.प्रक्षिप_पृष्ठभागः(strdup(mem_name.c_str()));
			}

			प्रति (स्वचालित cell : module->cells())
				यदि (cell->name.unescape().compare(0, len, text) == 0)
					obj_नामानि.प्रक्षिप_पृष्ठभागः(strdup(cell->name.unescape().c_str()));

			प्रति (स्वचालित &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.first);
				यदि (proc_name.compare(0, len, text) == 0)
					obj_नामानि.प्रक्षिप_पृष्ठभागः(strdup(proc_name.c_str()));
			}
		}

		std::क्रमय(obj_नामानि.begin(), obj_नामानि.अन्तः());
	}

	यदि (idx < obj_नामानि.आकार())
		प्रत्यागम strdup(obj_नामानि[idx++]);

	idx = 0;
	obj_नामानि.clear();
	प्रत्यागम रिक्त;
}

स्थिर वर्ण **readline_completion(ध्रुव वर्ण *text, पूर्णाङ्क start, पूर्णाङ्क)
{
	यदि (start == 0)
		प्रत्यागम rl_completion_matches(text, readline_cmd_generator);
	यदि (वाक्ये_तुलय(rl_पङ्क्तिः_बफर, "read_", 5) && वाक्ये_तुलय(rl_पङ्क्तिः_बफर, "write_", 6))
		प्रत्यागम rl_completion_matches(text, readline_obj_generator);
	प्रत्यागम रिक्त;
}
#यदि_अन्तः

शून्य shell(RTLIL::Design *design)
{
	स्थिर पूर्णाङ्क recursion_गणक = 0;

	recursion_गणक++;
	स्वचालित guard = logger().error_throw_scope();

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (वर्ण*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_word_break_characters = (वर्ण*)" \t\n";
#यदि_अन्तः

	वर्ण *command = रिक्त;
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	यावत् ((command = readline(create_prompt(design, recursion_गणक))) != रिक्त)
	{
#अन्यथा
	वर्ण command_बफर[4096];
	यावत् (1)
	{
		fputs(create_prompt(design, recursion_गणक), stdout);
		fflush(stdout);
		यदि ((command = fgets(command_बफर, 4096, stdin)) == रिक्त)
			भङ्ग;
#यदि_अन्तः
		यदि (command[strspn(command, " \t\r\n")] == 0) {
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			मुक्त(command);
#यदि_अन्तः
			अनुवर्त;
		}
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		सङ्कलय_history(command);
#यदि_अन्तः

		वर्ण *p = command + strspn(command, " \t\r\n");
		यदि (!वाक्ये_तुलय(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			यदि (*p == 0)
				भङ्ग;
		}

		यत्न {
			log_assert(design->selection_राशिः.आकार() == 1);
			Pass::call(design, command);
		} गृहाण (log_cmd_error_exception) {
			यावत् (design->selection_राशिः.आकार() > 1)
				design->निष्कासय_selection();
			log_reset_राशिः();
		}
		design->check();
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		यदि (command)
			मुक्त(command);
#यदि_अन्तः
	}
	यदि (command == रिक्त)
		प्रारूप_लिख("exit\n");
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	अन्यथा
		मुक्त(command);
#यदि_अन्तः
	recursion_गणक--;
}

संरचना ShellPass : सार्वजनिक Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	शून्य help() override {
		लघुगणकः("\n");
		लघुगणकः("    shell\n");
		लघुगणकः("\n");
		लघुगणकः("This command enters the interactive command mode. This can be useful\n");
		लघुगणकः("in a script to interrupt the script at a certain point and allow for\n");
		लघुगणकः("interactive inspection or manual synthesis of the design at this point.\n");
		लघुगणकः("\n");
		लघुगणकः("The command prompt of the interactive shell indicates the current\n");
		लघुगणकः("selection (see 'help select'):\n");
		लघुगणकः("\n");
		लघुगणकः("    yosys>\n");
		लघुगणकः("        the entire design is selected\n");
		लघुगणकः("\n");
		लघुगणकः("    yosys*>\n");
		लघुगणकः("        only part of the design is selected\n");
		लघुगणकः("\n");
		लघुगणकः("    yosys [modname]>\n");
		लघुगणकः("        the entire module 'modname' is selected using 'select -module modname'\n");
		लघुगणकः("\n");
		लघुगणकः("    yosys [modname]*>\n");
		लघुगणकः("        only part of current module 'modname' is selected\n");
		लघुगणकः("\n");
		लघुगणकः("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		लघुगणकः("do not terminate yosys but return to the command prompt.\n");
		लघुगणकः("\n");
		लघुगणकः("This command is the default action if nothing else has been specified\n");
		लघुगणकः("on the command line.\n");
		लघुगणकः("\n");
		लघुगणकः("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		लघुगणकः("\n");
	}
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) override {
		extra_तर्काः(तर्काः_2, 1, design, असत्य);
		shell(design);
	}
} ShellPass;

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
संरचना HistoryPass : सार्वजनिक Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	शून्य help() override {
		लघुगणकः("\n");
		लघुगणकः("    history\n");
		लघुगणकः("\n");
		लघुगणकः("This command prints all commands in the shell history buffer. This are\n");
		लघुगणकः("all commands executed in an interactive session, but not the commands\n");
		लघुगणकः("from executed scripts.\n");
		लघुगणकः("\n");
	}
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) override {
		extra_तर्काः(तर्काः_2, 1, design, असत्य);
#यदि_परिभाषितम् YOSYS_ENABLE_READLINE
		प्रति(HIST_प्रविष्टिः **list = history_list(); *list != रिक्त; list++)
			लघुगणकः("%s\n", (*list)->पङ्क्तिः);
#अन्यथा
		प्रति (पूर्णाङ्क i = where_history(); history_get(i); i++)
			लघुगणकः("%s\n", history_get(i)->पङ्क्तिः);
#यदि_अन्तः
	}
} HistoryPass;
#यदि_अन्तः

संरचना ScriptCmdPass : सार्वजनिक Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	शून्य help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		लघुगणकः("\n");
		लघुगणकः("    script <filename> [<from_label>:<to_label>]\n");
		लघुगणकः("    script -scriptwire [selection]\n");
		लघुगणकः("\n");
		लघुगणकः("This command executes the yosys commands in the specified file (default\n");
		लघुगणकः("behaviour), or commands embedded in the constant text value connected to the\n");
		लघुगणकः("selected wires.\n");
		लघुगणकः("\n");
		लघुगणकः("In the default (file) case, the 2nd argument can be used to only execute the\n");
		लघुगणकः("section of the file between the specified labels. An empty from label is\n");
		लघुगणकः("synonymous with the beginning of the file and an empty to label is synonymous\n");
		लघुगणकः("with the end of the file.\n");
		लघुगणकः("\n");
		लघुगणकः("If only one label is specified (without ':') then only the block\n");
		लघुगणकः("marked with that label (until the next label) is executed.\n");
		लघुगणकः("\n");
		लघुगणकः("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		लघुगणकः("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		लघुगणकः("'-module' mode can be exited by using the 'cd' command.\n");
		लघुगणकः("\n");
	}
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) override
	{
		तार्किक scriptwire = असत्य;

		आकार_t argidx;
		प्रति (argidx = 1; argidx < तर्काः_2.आकार(); argidx++) {
			यदि (तर्काः_2[argidx] == "-scriptwire") {
				scriptwire = सत्य;
				अनुवर्त;
			}
			भङ्ग;
		}
		यदि (scriptwire) {
			extra_तर्काः(तर्काः_2, argidx, design);

			प्रति (स्वचालित mod : design->selected_modules())
				प्रति (स्वचालित &c : mod->connections()) {
					यदि (!c.first.is_wire())
						अनुवर्त;
					स्वचालित w = c.first.as_wire();
					यदि (!mod->selected(w))
						अनुवर्त;
					यदि (!c.second.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					स्वचालित v = c.second.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		अन्यथा यदि (तर्काः_2.आकार() < 2)
			log_cmd_error("Missing script file.\n");
		अन्यथा यदि (तर्काः_2.आकार() == 2)
			सततम्_frontend(तर्काः_2[1], "script", design);
		अन्यथा यदि (तर्काः_2.आकार() == 3)
			सततम्_frontend(तर्काः_2[1], "script", design, &तर्काः_2[2]);
		अन्यथा
			extra_तर्काः(तर्काः_2, 2, design, असत्य);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_अन्तः
