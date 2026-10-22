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

#ضمن "kernel/yosys.h"
#ضمن "kernel/log.h"
#ضمن "kernel/newcelltypes.h"

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
#  include <tcl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#جيڪڏهن_پورو

#إذا defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#وإلاإذا defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#وإلا
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  إذا defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#جيڪڏهن_پورو

#إذا !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#جيڪڏهن_پورو

#إذا defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
خارجي "C" PyObject* PyInit_libyosys();
خارجي "C" PyObject* PyInit_pyosys();
#ضمن <signal.h>
#جيڪڏهن_پورو

#ضمن <limits.h>
#ضمن <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحيح yosys_xtrace = 0;
منطقي yosys_write_versions = سچ;
ثابت حرف* yosys_maybe_نسخو() {
	إذا (yosys_write_versions)
		أرجع yosys_نسخو_str;
	وإلا
		أرجع "Yosys";
}

RTLIL::Design *yosys_design = عدم;
NewCellTypes yosys_celltypes;

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = عدم;
Tcl_Interp *yosys_sdc_interp = عدم;
#جيڪڏهن_پورو

std::سيٽ<std::string> yosys_داخلا_files, yosys_output_files;

منطقي memhasher_سرگرم = خطأ;
uint32_t memhasher_rng = 123456;
std::ويڪٽر<فراغ*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقي Multithreading::سرگرم_2 = خطأ;

فراغ init_share_dirname();
فراغ init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!سرگرم_2);
	سرگرم_2 = سچ;
}

Multithreading::~Multithreading() {
	log_assert(سرگرم_2);
	سرگرم_2 = خطأ;
}

فراغ Autoidx::ensure_at_گهٽ_کان_گهٽ(صحيح v) {
	log_assert(!Multithreading::سرگرم());
	قدر = std::max(قدر, v);
}

صحيح Autoidx::عامل++(صحيح) {
	log_assert(!Multithreading::سرگرم());
	أرجع قدر++;
}

فراغ memhasher_on()
{
#إذا defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += وقت_2(عدم) << 16 ^ getpid();
#جيڪڏهن_پورو
	memhasher_store.resize(0x10000);
	memhasher_سرگرم = سچ;
}

فراغ memhasher_off()
{
	لكل (خودڪار p : memhasher_store)
		إذا (p) حرر(p);
	memhasher_store.clear();
	memhasher_سرگرم = خطأ;
}

فراغ memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	صحيح سائيز, اشاريو = (memhasher_rng >> 4) & 0xffff;
	بدل (memhasher_rng & 7) {
		حالة 0: سائيز =   16; اكسر;
		حالة 1: سائيز =  256; اكسر;
		حالة 2: سائيز = 1024; اكسر;
		حالة 3: سائيز = 4096; اكسر;
		افتراضي: سائيز = 0;
	}
	إذا (اشاريو < 16) سائيز *= 16;
	memhasher_store[اشاريو] = realloc(memhasher_store[اشاريو], سائيز);
}

فراغ yosys_banner()
{
	لاگرٿم("\n");
	لاگرٿم(" /----------------------------------------------------------------------------\\\n");
	لاگرٿم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	لاگرٿم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	لاگرٿم(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	لاگرٿم(" \\----------------------------------------------------------------------------/\n");
	لاگرٿم(" %s\n", yosys_maybe_نسخو());
}

#إذا defined(YOSYS_ENABLE_SPAWN)
صحيح لاڳيتو_command(ثابت std::string &command, std::function<فراغ(ثابت std::string&)> process_سٽ)
{
	إذا (!process_سٽ)
		أرجع system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	إذا (f == خالي_اشارو)
		أرجع -1;

	std::string سٽ;
	حرف logbuf[128];
	طالما (سچ) {
		إذا (fgets(logbuf, 128, f) != عدم) {
			سٽ += logbuf;
			إذا (!سٽ.خالي_جايون() && سٽ.پٺ() == '\n')
				process_سٽ(سٽ), سٽ.clear();
		} وإلا إذا (ferror(f) && errno == EINTR) {
			clearerr(f);
			استمر;
		} وإلا {
			اكسر;
		}
	}
	إذا (!سٽ.خالي_جايون())
		process_سٽ(سٽ);

	صحيح ret = pclose(f);
	إذا (ret < 0)
		أرجع -2;
#جيڪڏهن_وصف_ٿيل _WIN32
	أرجع ret;
#وإلا
	إذا (!WIFEXITED(ret))
		أرجع -1;
	أرجع WEXITSTATUS(ret);
#جيڪڏهن_پورو
}
#جيڪڏهن_پورو

منطقي already_تياري = خطأ;
منطقي already_shutdown = خطأ;

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.جوڙيو_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(فراغ)_;
	ارم py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#جيڪڏهن_پورو

فراغ yosys_تياري()
{
	إذا(already_تياري)
		أرجع;
	already_تياري = سچ;
	already_shutdown = خطأ;

	StaticTwines::init();

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	إذا (!Py_IsInitialized()) {
		PyImport_AppendInittab((حرف*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((حرف*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((حرف*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_لاڳيتو_Simple_String("import sys");
		إشارة(SIGINT, SIG_DFL);
	}
#جيڪڏهن_پورو

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_رجسٽر();
	yosys_design = جديد RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_معلوم;
	log_ڌڪو();
}

منطقي yosys_already_تياري()
{
	أرجع already_تياري;
}

فراغ yosys_shutdown()
{
	إذا(already_shutdown)
		أرجع;
	already_تياري = خطأ;
	already_shutdown = سچ;
	log_ڪڍو();

	Pass::پورو_رجسٽر();

	ميٽيو yosys_design;
	yosys_design = عدم;

	logger().clear();

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
	إذا (yosys_tcl_interp != عدم) {
		إذا (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = عدم;
	}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PLUGINS
	لكل (خودڪار &it : loaded_plugins)
		dlclose(it.ٻيو);

	loaded_plugins.clear();
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#جيڪڏهن_پورو
	loaded_plugin_aliases.clear();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	Py_Finalize();
#جيڪڏهن_پورو
}

ثابت std::string *create_id_اڳياڙي(std::string_view file, صحيح سٽ, std::string_view func)
{
#جيڪڏهن_وصف_ٿيل _WIN32
	سائيز_t pos = file.ڳوليو_آخري_جو("/\\");
#وإلا
	سائيز_t pos = file.ڳوليو_آخري_جو('/');
#جيڪڏهن_پورو
	إذا (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.ڳوليو_آخري_جو(':');
	إذا (pos != std::string_view::npos)
		func = func.substr(pos+1);

	أرجع جديد std::string(stringf("$auto$%s:%d:%s$", file, سٽ, func));
}

RTLIL::Design *yosys_get_design()
{
	أرجع yosys_design;
}

ثابت حرف *create_prompt(RTLIL::Design *design, صحيح recursion_counter, منطقي newline)
{
	ساكن حرف بفر[100];
	std::string str = newline ? "\n" : "";
	إذا (recursion_counter > 1)
		str += stringf("(%d) ", recursion_counter);
	str += "yosys";
	إذا (!design->selected_سرگرم_module.خالي_جايون())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_سرگرم_module)).c_str());
	إذا (!design->ڀريل_selection()) {
		إذا (design->selected_سرگرم_module.خالي_جايون())
			str += "*";
		وإلا إذا (design->selection().selected_modules.سائيز() != 1 || design->selection().selected_members.سائيز() != 0 ||
				design->selection().selected_modules.ڳڻپ(design->selected_سرگرم_module) == 0)
			str += "*";
	}
	snprintf(بفر, 100, "%s> ", str.c_str());
	أرجع بفر;
}

std::ويڪٽر<std::string> glob_filename(ثابت std::string &filename_نمونو)
{
	std::ويڪٽر<std::string> results;

#إذا defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ڌڪو_پٺ(filename_نمونو);
#وإلا
	glob_t globbuf;

	صحيح err = glob(filename_نمونو.c_str(), 0, عدم, &globbuf);

	إذا(err == 0) {
		لكل (سائيز_t i = 0; i < globbuf.gl_pathc; i++)
			results.ڌڪو_پٺ(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} وإلا {
		results.ڌڪو_پٺ(filename_نمونو);
	}
#جيڪڏهن_پورو

	أرجع results;
}

فراغ rewrite_filename(std::string &filename)
{
	إذا (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_سائيز(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_سائيز(filename)-2);
	إذا (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#جيڪڏهن_وصف_نه_ٿيل _WIN32
	إذا (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL

// defined in tclapi.cc
خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);

خارجي Tcl_Interp *yosys_get_tcl_interp()
{
	إذا (yosys_tcl_interp == عدم) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	أرجع yosys_tcl_interp;
}

// Also see SdcPass
بنية TclPass : عام Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	فراغ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لاگرٿم("\n");
		لاگرٿم("    tcl <filename> [args]\n");
		لاگرٿم("\n");
		لاگرٿم("This command executes the tcl commands in the specified file.\n");
		لاگرٿم("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		لاگرٿم("\n");
		لاگرٿم("The tcl command 'yosys -import' can be used to import all yosys\n");
		لاگرٿم("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		لاگرٿم("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		لاگرٿم("in order to avoid a name collision with the built in commands.\n");
		لاگرٿم("\n");
		لاگرٿم("If any arguments are specified, these arguments are provided to the script via\n");
		لاگرٿم("the standard $argc and $argv variables.\n");
		لاگرٿم("\n");
		لاگرٿم("Note, tcl will not receive the output of any yosys command. If the output\n");
		لاگرٿم("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		لاگرٿم("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		لاگرٿم("The 'result.string' value is then used as the tcl output value of the command.\n");
		لاگرٿم("\n");
	}
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *) override {
		إذا (دليلون_2.سائيز() < 2)
			log_cmd_error("Missing script file.\n");

		std::ويڪٽر<Tcl_Obj*> script_دليلون;
		لكل (خودڪار it = دليلون_2.begin() + 2; it != دليلون_2.پڇاڙي(); ++it)
			script_دليلون.ڌڪو_پٺ(Tcl_NewStringObj((*it).c_str(), (*it).سائيز()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_سيٽ_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(script_دليلون.سائيز()), 0);
		Tcl_Obj_سيٽ_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(script_دليلون.سائيز(), script_دليلون.معلومات()), 0);
		Tcl_Obj_سيٽ_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(دليلون_2[1].c_str(), دليلون_2[1].سائيز()), 0);
		إذا (Tcl_EvalFile(interp, دليلون_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتيجو(interp));
		Tcl_Release(interp);
	}
} TclPass;

#جيڪڏهن_پورو

#إذا defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string رستو_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	لكل (صحيح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", رستو_2.معلومات(), رستو_2.سائيز());
		إذا (buflen < (ssize_t)رستو_2.سائيز())
			اكسر;
		وإلا
			رستو_2.resize(رستو_2.سائيز() * 2);
	}
	إذا (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		رستو_2.resize(0);
	} وإلا {
		طالما (buflen > 0 && رستو_2[buflen-1] != '/')
			buflen--;
		رستو_2.resize(buflen);
	}
	أرجع رستو_2;
}
#وإلاإذا defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#جيڪڏهن_وصف_ٿيل __NetBSD__
	صحيح mib[4] = {CTL_KERN, KERN_PROC_دليلون, getpid(), KERN_PROC_PATHNAME};
#وإلا
	صحيح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#جيڪڏهن_پورو
	سائيز_t buflen;
	حرف *بفر;
	std::string رستو_2;
	إذا (sysctl(mib, 4, عدم, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	بفر = (حرف*)يادگيري_ڏيو(buflen);
	إذا (بفر == عدم)
		log_error("malloc failed: %s\n", strerror(errno));
	إذا (sysctl(mib, 4, بفر, &buflen, عدم, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	طالما (buflen > 0 && بفر[buflen-1] != '/')
		buflen--;
	رستو_2.سونپيو(بفر, buflen);
	حرر(بفر);
	أرجع رستو_2;
}
#وإلاإذا defined(__APPLE__)
std::string proc_self_dirname()
{
	حرف *رستو_2 = عدم;
	uint32_t buflen = 0;
	طالما (NSGet_Executable_رستو(رستو_2, &buflen) != 0)
		رستو_2 = (حرف *) realloc((فراغ *) رستو_2, buflen);
	حرف resolved[رستو_MAX];
	إذا (realpath(رستو_2, resolved) != عدم) {
		حرر(رستو_2);
		رستو_2 = strdup(resolved);
		buflen = لڙي_ڊيگهه(رستو_2);
	}
	طالما (buflen > 0 && رستو_2[buflen-1] != '/')
		buflen--;
	std::string str(رستو_2, buflen);
	حرر(رستو_2);
	أرجع str;
}
#وإلاإذا defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	إذا (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.سائيز()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wbinpath.معلومات(), wbinpath.سائيز(), عدم, 0, عدم, عدم));
	إذا (Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wbinpath.معلومات(), wbinpath.سائيز(), &ubinpath[0], ubinpath.سائيز(), عدم, عدم) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	أرجع ubinpath;

}
#وإلاإذا defined(__wasm)
std::string proc_self_dirname()
{
	أرجع "/";
}
#وإلاإذا defined(__OpenBSD__) || defined(__HAIKU__)
حرف yosys_رستو[رستو_MAX];
حرف *yosys_argv0;

std::string proc_self_dirname(فراغ)
{
	حرف buf[رستو_MAX + 1] = "", *رستو_2, *p;
	// if case argv[0] contains a valid path, return it
	إذا (لڙي_ڊيگهه(yosys_رستو) > 0) {
		p = strrchr(yosys_رستو, '/');
		snprintf(buf, حجم buf, "%*s/", (صحيح)(yosys_رستو - p), yosys_رستو);
		أرجع buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	رستو_2 = strdup(getenv("PATH"));
	إذا (!رستو_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	لكل (p = strtok(رستو_2, ":"); p; p = strtok(عدم, ":")) {
		snprintf(buf, حجم buf, "%s/%s", p, yosys_argv0);
		إذا (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			حرر(رستو_2);
			أرجع buf;
		}
	}
	حرر(رستو_2);
	log_error("Can't determine yosys executable path.\n");
	أرجع عدم;
}
#وإلا
	#غلطي "Don't know how to determine process executable base path!"
#جيڪڏهن_پورو

#إذا defined(__wasm)
فراغ init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#وإلا
فراغ init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_رستو_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت حرف *share_رستو = PyUnicode_AsUTF8(share_رستو_obj);
		yosys_share_dirname = std::string(share_رستو);
		أرجع;
	}
#  endif
	std::string proc_self_رستو = proc_self_dirname();
#  إذا defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_رستو = proc_self_رستو + "share\\";
	إذا (check_directory_exists(proc_share_رستو, سچ)) {
		yosys_share_dirname = proc_share_رستو;
		أرجع;
	}
	proc_share_رستو = proc_self_رستو + "..\\share\\";
	إذا (check_directory_exists(proc_share_رستو, سچ)) {
		yosys_share_dirname = proc_share_رستو;
		أرجع;
	}
#  وإلا
	std::string proc_share_رستو = proc_self_رستو + "share/";
	إذا (check_directory_exists(proc_share_رستو, سچ)) {
		yosys_share_dirname = proc_share_رستو;
		أرجع;
	}
	proc_share_رستو = proc_self_رستو + "../share/" + proc_پروگرام_اڳياڙي()+ "yosys/";
	إذا (check_directory_exists(proc_share_رستو, سچ)) {
		yosys_share_dirname = proc_share_رستو;
		أرجع;
	}
	proc_share_رستو = YOSYS_DATDIR "/";
	إذا (check_directory_exists(proc_share_رستو, سچ)) {
		yosys_share_dirname = proc_share_رستو;
		أرجع;
	}
#  endif
}
#جيڪڏهن_پورو

فراغ init_abc_executable_name()
{
#جيڪڏهن_وصف_ٿيل ABCEXTERNAL
	std::string exe_file;
	إذا (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} وإلا {
		yosys_abc_executable = ABCEXTERNAL;
	}
#وإلا
	yosys_abc_executable = proc_self_dirname() + proc_پروگرام_اڳياڙي()+ "yosys-abc";
#  ifdef _WIN32
	إذا (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_پروگرام_اڳياڙي() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_پروگرام_اڳياڙي() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	إذا (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_رستو_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت حرف *abc_رستو = PyUnicode_AsUTF8(abc_رستو_obj);
		yosys_abc_executable = std::string(abc_رستو);
	}
#  endif
#جيڪڏهن_پورو
}

std::string proc_share_dirname()
{
	إذا (yosys_share_dirname.خالي_جايون())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	أرجع yosys_share_dirname;
}

std::string proc_پروگرام_اڳياڙي()
{
	أرجع YOSYS_پروگرام_اڳياڙي;
}

منطقي fgetline(FILE *f, std::string &بفر)
{
	بفر = "";
	حرف بلاڪ[4096];
	طالما (1) {
		إذا (fgets(بلاڪ, 4096, f) == عدم)
			أرجع خطأ;
		بفر += بلاڪ;
		إذا (بفر.سائيز() > 0 && (بفر[بفر.سائيز()-1] == '\n' ||  بفر[بفر.سائيز()-1] == '\r')) {
			طالما (بفر.سائيز() > 0 && (بفر[بفر.سائيز()-1] == '\n' ||  بفر[بفر.سائيز()-1] == '\r'))
				بفر.resize(بفر.سائيز()-1);
			أرجع سچ;
		}
	}
}

ساكن فراغ handle_label(std::string &command, منطقي &from_منزل_سرگرم, ثابت std::string &لاڳيتو_from, ثابت std::string &لاڳيتو_منزل)
{
	صحيح pos = 0;
	std::string label;

	طالما (pos < Get_سائيز(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	إذا (pos < Get_سائيز(command) && command[pos] == '#')
		أرجع;

	طالما (pos < Get_سائيز(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	إذا (Get_سائيز(label) > 1 && label.پٺ() == ':')
	{
		label = label.substr(0, Get_سائيز(label)-1);
		command = command.substr(pos);

		إذا (label == لاڳيتو_from)
			from_منزل_سرگرم = سچ;
		وإلا إذا (label == لاڳيتو_منزل || (لاڳيتو_from == لاڳيتو_منزل && !لاڳيتو_from.خالي_جايون()))
			from_منزل_سرگرم = خطأ;
	}
}

منطقي لاڳيتو_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_منزل_label)
{
	إذا (design == خالي_اشارو)
		design = yosys_design;

	إذا (command == "auto") {
	  std::string filename_trim = filename;

	  خودڪار has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
	    إذا (filename.سائيز() >= extension.سائيز()) {
	      أرجع filename.compare(filename.سائيز() - extension.سائيز(), extension.سائيز(), extension) == 0;
	    }
	    أرجع خطأ;
	  };

	  إذا (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.سائيز() - 3);
	  }

	  إذا (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } وإلا إذا (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } وإلا إذا (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } وإلا إذا (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } وإلا إذا (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } وإلا إذا (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } وإلا إذا (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } وإلا إذا (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } وإلا إذا (filename == "-") {
	    command = "script";
	  } وإلا {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	إذا (command == "script")
	{
		std::string لاڳيتو_from, لاڳيتو_منزل;
		منطقي from_منزل_سرگرم = سچ;

		إذا (from_منزل_label != عدم) {
			سائيز_t pos = from_منزل_label->ڳوليو(':');
			إذا (pos == std::string::npos) {
				لاڳيتو_from = *from_منزل_label;
				لاڳيتو_منزل = *from_منزل_label;
			} وإلا {
				لاڳيتو_from = from_منزل_label->substr(0, pos);
				لاڳيتو_منزل = from_منزل_label->substr(pos+1);
			}
			from_منزل_سرگرم = لاڳيتو_from.خالي_جايون();
		}

		لاگرٿم("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		إذا (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_داخلا_files.وجهو(filename);
		}

		إذا (f == عدم)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		صحيح backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		حاول {
			std::string command;
			طالما (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				طالما (!command.خالي_جايون() && command[command.سائيز()-1] == '\\') {
					std::string ايندڙ_سٽ;
					إذا (!fgetline(f, ايندڙ_سٽ))
						اكسر;
					Frontend::current_script_lineno++;
					command.resize(command.سائيز()-1);
					command += ايندڙ_سٽ;
				}
				handle_label(command, from_منزل_سرگرم, لاڳيتو_from, لاڳيتو_منزل);
				إذا (from_منزل_سرگرم) {
					Pass::call(design, command);
					design->check();
				}
			}

			إذا (!command.خالي_جايون()) {
				handle_label(command, from_منزل_سرگرم, لاڳيتو_from, لاڳيتو_منزل);
				إذا (from_منزل_سرگرم) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		امسك (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			ارم;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		إذا (filename != "-")
			fclose(f);

		أرجع سچ;
	}

	إذا (command == "tcl") {
		Pass::call(design, ويڪٽر<string>({command, filename}));
		أرجع سچ;
	}

	لاگرٿم("\n");
	إذا (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} وإلا {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	إذا (command[0] == ' ') {
		خودڪار argv = ورهايو_ٽوڪن("read" + command);
		argv.ڌڪو_پٺ(filename);
		Pass::call(design, argv);
	} وإلا
		Frontend::frontend_call(design, عدم, filename, command);

	design->check();
	أرجع خطأ;
}

فراغ لاڳيتو_pass(std::string command, RTLIL::Design *design)
{
	إذا (design == خالي_اشارو)
		design = yosys_design;

	لاگرٿم("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

فراغ لاڳيتو_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	إذا (design == خالي_اشارو)
		design = yosys_design;

	إذا (command == "auto") {
		إذا (filename.سائيز() > 2 && filename.compare(filename.سائيز()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		وإلا إذا (filename.سائيز() > 3 && filename.compare(filename.سائيز()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		وإلا إذا (filename.سائيز() > 3 && filename.compare(filename.سائيز()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		وإلا إذا (filename.سائيز() > 3 && filename.compare(filename.سائيز()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		وإلا إذا (filename.سائيز() > 4 && filename.compare(filename.سائيز()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		وإلا إذا (filename.سائيز() > 5 && filename.compare(filename.سائيز()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		وإلا إذا (filename.سائيز() > 5 && filename.compare(filename.سائيز()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		وإلا إذا (filename.سائيز() > 5 && filename.compare(filename.سائيز()-5, std::string::npos, ".json") == 0)
			command = "json";
		وإلا إذا (filename == "-")
			command = "rtlil";
		وإلا إذا (filename.خالي_جايون())
			أرجع;
		وإلا
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	إذا (filename.خالي_جايون())
		filename = "-";

	لاگرٿم("\n");
	إذا (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} وإلا {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, عدم, filename, command);
}

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ساكن حرف *readline_cmd_generator(ثابت حرف *متن, صحيح state)
{
	ساكن std::map<std::string, Pass*>::iterator it;
	ساكن صحيح len;

	إذا (!state) {
		it = pass_رجسٽر.begin();
		len = لڙي_ڊيگهه(متن);
	}

	لكل (; it != pass_رجسٽر.پڇاڙي(); it++) {
		إذا (it->پهريون.compare(0, len, متن) == 0)
			أرجع strdup((it++)->پهريون.c_str());
	}
	أرجع عدم;
}

ساكن حرف *readline_obj_generator(ثابت حرف *متن, صحيح state)
{
	ساكن std::ويڪٽر<حرف*> obj_نالا;
	ساكن سائيز_t idx;

	إذا (!state)
	{
		idx = 0;
		obj_نالا.clear();

		RTLIL::Design *design = yosys_get_design();
		صحيح len = لڙي_ڊيگهه(متن);

		إذا (design->selected_سرگرم_module.خالي_جايون())
		{
			لكل (خودڪار mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				إذا (mod_name.compare(0, len, متن) == 0)
					obj_نالا.ڌڪو_پٺ(strdup(mod_name.c_str()));
			}
		}
		وإلا إذا (design->module(design->selected_سرگرم_module) != خالي_اشارو)
		{
			RTLIL::Module *module = design->module(design->selected_سرگرم_module);

			لكل (خودڪار w : module->wires())
				إذا (w->name.unescape().compare(0, len, متن) == 0)
					obj_نالا.ڌڪو_پٺ(strdup(w->name.unescape().c_str()));

			لكل (خودڪار &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.پهريون);
				إذا (mem_name.compare(0, len, متن) == 0)
					obj_نالا.ڌڪو_پٺ(strdup(mem_name.c_str()));
			}

			لكل (خودڪار cell : module->cells())
				إذا (cell->name.unescape().compare(0, len, متن) == 0)
					obj_نالا.ڌڪو_پٺ(strdup(cell->name.unescape().c_str()));

			لكل (خودڪار &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.پهريون);
				إذا (proc_name.compare(0, len, متن) == 0)
					obj_نالا.ڌڪو_پٺ(strdup(proc_name.c_str()));
			}
		}

		std::ترتيب_ڏيو(obj_نالا.begin(), obj_نالا.پڇاڙي());
	}

	إذا (idx < obj_نالا.سائيز())
		أرجع strdup(obj_نالا[idx++]);

	idx = 0;
	obj_نالا.clear();
	أرجع عدم;
}

ساكن حرف **readline_completion(ثابت حرف *متن, صحيح شروعات, صحيح)
{
	إذا (شروعات == 0)
		أرجع rl_completion_matches(متن, readline_cmd_generator);
	إذا (لڙي_ڀيٽ(rl_سٽ_بفر, "read_", 5) && لڙي_ڀيٽ(rl_سٽ_بفر, "write_", 6))
		أرجع rl_completion_matches(متن, readline_obj_generator);
	أرجع عدم;
}
#جيڪڏهن_پورو

فراغ shell(RTLIL::Design *design)
{
	ساكن صحيح recursion_counter = 0;

	recursion_counter++;
	خودڪار guard = logger().error_throw_scope();

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (حرف*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_لفظ_break_characters = (حرف*)" \t\n";
#جيڪڏهن_پورو

	حرف *command = عدم;
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	طالما ((command = readline(create_prompt(design, recursion_counter))) != عدم)
	{
#وإلا
	حرف command_بفر[4096];
	طالما (1)
	{
		fputs(create_prompt(design, recursion_counter), stdout);
		fflush(stdout);
		إذا ((command = fgets(command_بفر, 4096, stdin)) == عدم)
			اكسر;
#جيڪڏهن_پورو
		إذا (command[strspn(command, " \t\r\n")] == 0) {
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			حرر(command);
#جيڪڏهن_پورو
			استمر;
		}
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		جوڙيو_history(command);
#جيڪڏهن_پورو

		حرف *p = command + strspn(command, " \t\r\n");
		إذا (!لڙي_ڀيٽ(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			إذا (*p == 0)
				اكسر;
		}

		حاول {
			log_assert(design->selection_ڍير.سائيز() == 1);
			Pass::call(design, command);
		} امسك (log_cmd_error_exception) {
			طالما (design->selection_ڍير.سائيز() > 1)
				design->ڪڍو_selection();
			log_reset_ڍير();
		}
		design->check();
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		إذا (command)
			حرر(command);
#جيڪڏهن_پورو
	}
	إذا (command == عدم)
		لکو("exit\n");
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	وإلا
		حرر(command);
#جيڪڏهن_پورو
	recursion_counter--;
}

بنية ShellPass : عام Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	فراغ help() override {
		لاگرٿم("\n");
		لاگرٿم("    shell\n");
		لاگرٿم("\n");
		لاگرٿم("This command enters the interactive command mode. This can be useful\n");
		لاگرٿم("in a script to interrupt the script at a certain point and allow for\n");
		لاگرٿم("interactive inspection or manual synthesis of the design at this point.\n");
		لاگرٿم("\n");
		لاگرٿم("The command prompt of the interactive shell indicates the current\n");
		لاگرٿم("selection (see 'help select'):\n");
		لاگرٿم("\n");
		لاگرٿم("    yosys>\n");
		لاگرٿم("        the entire design is selected\n");
		لاگرٿم("\n");
		لاگرٿم("    yosys*>\n");
		لاگرٿم("        only part of the design is selected\n");
		لاگرٿم("\n");
		لاگرٿم("    yosys [modname]>\n");
		لاگرٿم("        the entire module 'modname' is selected using 'select -module modname'\n");
		لاگرٿم("\n");
		لاگرٿم("    yosys [modname]*>\n");
		لاگرٿم("        only part of current module 'modname' is selected\n");
		لاگرٿم("\n");
		لاگرٿم("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		لاگرٿم("do not terminate yosys but return to the command prompt.\n");
		لاگرٿم("\n");
		لاگرٿم("This command is the default action if nothing else has been specified\n");
		لاگرٿم("on the command line.\n");
		لاگرٿم("\n");
		لاگرٿم("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		لاگرٿم("\n");
	}
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) override {
		extra_دليلون(دليلون_2, 1, design, خطأ);
		shell(design);
	}
} ShellPass;

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
بنية HistoryPass : عام Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	فراغ help() override {
		لاگرٿم("\n");
		لاگرٿم("    history\n");
		لاگرٿم("\n");
		لاگرٿم("This command prints all commands in the shell history buffer. This are\n");
		لاگرٿم("all commands executed in an interactive session, but not the commands\n");
		لاگرٿم("from executed scripts.\n");
		لاگرٿم("\n");
	}
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) override {
		extra_دليلون(دليلون_2, 1, design, خطأ);
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_READLINE
		لكل(HIST_داخلا **list = history_list(); *list != عدم; list++)
			لاگرٿم("%s\n", (*list)->سٽ);
#وإلا
		لكل (صحيح i = where_history(); history_get(i); i++)
			لاگرٿم("%s\n", history_get(i)->سٽ);
#جيڪڏهن_پورو
	}
} HistoryPass;
#جيڪڏهن_پورو

بنية ScriptCmdPass : عام Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	فراغ help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لاگرٿم("\n");
		لاگرٿم("    script <filename> [<from_label>:<to_label>]\n");
		لاگرٿم("    script -scriptwire [selection]\n");
		لاگرٿم("\n");
		لاگرٿم("This command executes the yosys commands in the specified file (default\n");
		لاگرٿم("behaviour), or commands embedded in the constant text value connected to the\n");
		لاگرٿم("selected wires.\n");
		لاگرٿم("\n");
		لاگرٿم("In the default (file) case, the 2nd argument can be used to only execute the\n");
		لاگرٿم("section of the file between the specified labels. An empty from label is\n");
		لاگرٿم("synonymous with the beginning of the file and an empty to label is synonymous\n");
		لاگرٿم("with the end of the file.\n");
		لاگرٿم("\n");
		لاگرٿم("If only one label is specified (without ':') then only the block\n");
		لاگرٿم("marked with that label (until the next label) is executed.\n");
		لاگرٿم("\n");
		لاگرٿم("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		لاگرٿم("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		لاگرٿم("'-module' mode can be exited by using the 'cd' command.\n");
		لاگرٿم("\n");
	}
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) override
	{
		منطقي scriptwire = خطأ;

		سائيز_t argidx;
		لكل (argidx = 1; argidx < دليلون_2.سائيز(); argidx++) {
			إذا (دليلون_2[argidx] == "-scriptwire") {
				scriptwire = سچ;
				استمر;
			}
			اكسر;
		}
		إذا (scriptwire) {
			extra_دليلون(دليلون_2, argidx, design);

			لكل (خودڪار mod : design->selected_modules())
				لكل (خودڪار &c : mod->connections()) {
					إذا (!c.پهريون.is_wire())
						استمر;
					خودڪار w = c.پهريون.as_wire();
					إذا (!mod->selected(w))
						استمر;
					إذا (!c.ٻيو.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					خودڪار v = c.ٻيو.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		وإلا إذا (دليلون_2.سائيز() < 2)
			log_cmd_error("Missing script file.\n");
		وإلا إذا (دليلون_2.سائيز() == 2)
			لاڳيتو_frontend(دليلون_2[1], "script", design);
		وإلا إذا (دليلون_2.سائيز() == 3)
			لاڳيتو_frontend(دليلون_2[1], "script", design, &دليلون_2[2]);
		وإلا
			extra_دليلون(دليلون_2, 2, design, خطأ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_پڇاڙي
