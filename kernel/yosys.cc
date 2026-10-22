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

#شامل "kernel/yosys.h"
#شامل "kernel/log.h"
#شامل "kernel/newcelltypes.h"

#اگر_تعریف_شدہ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
#  include <tcl.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_PLUGINS
#  include <dlfcn.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نامخلا py = pybind11;
#اگر_ختم

#اگر defined(_WIN32)
#  include <windows.h>
#  include <io.h>
#وگرنہ_اگر defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#ورنہ
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  اگر defined(YOSYS_ENABLE_SPAWN)
#    include <sys/wait.h>
#  endif
#اگر_ختم

#اگر !defined(_WIN32) && defined(YOSYS_ENABLE_GLOB)
#  include <glob.h>
#اگر_ختم

#اگر defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/sysctl.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
خارجی "C" PyObject* PyInit_libyosys();
خارجی "C" PyObject* PyInit_pyosys();
#شامل <signal.h>
#اگر_ختم

#شامل <limits.h>
#شامل <errno.h>

YOSYS_NAMESPACE_BEGIN

Autoidx autoidx(1);
صحیح yosys_xtrace = 0;
منطقی yosys_write_versions = سچ;
ثابت حرف* yosys_maybe_نسخہ() {
	اگر (yosys_write_versions)
		واپس yosys_نسخہ_str;
	ورنہ
		واپس "Yosys";
}

RTLIL::Design *yosys_design = تہی;
NewCellTypes yosys_celltypes;

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_tcl_interp = تہی;
Tcl_Interp *yosys_sdc_interp = تہی;
#اگر_ختم

std::سیٹ<std::string> yosys_داخلہ_files, yosys_output_files;

منطقی memhasher_فعال = جھوٹ;
uint32_t memhasher_rng = 123456;
std::سمتیہ<خالی*> memhasher_store;

std::string yosys_share_dirname;
std::string yosys_abc_executable;

منطقی Multithreading::فعال_2 = جھوٹ;

خالی init_share_dirname();
خالی init_abc_executable_name();

Multithreading::Multithreading() {
	log_assert(!فعال_2);
	فعال_2 = سچ;
}

Multithreading::~Multithreading() {
	log_assert(فعال_2);
	فعال_2 = جھوٹ;
}

خالی Autoidx::ensure_at_کم_سے_کم(صحیح v) {
	log_assert(!Multithreading::فعال());
	قدر = std::max(قدر, v);
}

صحیح Autoidx::عامل++(صحیح) {
	log_assert(!Multithreading::فعال());
	واپس قدر++;
}

خالی memhasher_on()
{
#اگر defined(__linux__) || defined(__FreeBSD__)
	memhasher_rng += وقت_2(تہی) << 16 ^ getpid();
#اگر_ختم
	memhasher_store.resize(0x10000);
	memhasher_فعال = سچ;
}

خالی memhasher_off()
{
	برائے (خودکار p : memhasher_store)
		اگر (p) آزاد(p);
	memhasher_store.clear();
	memhasher_فعال = جھوٹ;
}

خالی memhasher_do()
{
	memhasher_rng ^= memhasher_rng << 13;
	memhasher_rng ^= memhasher_rng >> 17;
	memhasher_rng ^= memhasher_rng << 5;

	صحیح سائز, اشاریہ = (memhasher_rng >> 4) & 0xffff;
	انتخاب (memhasher_rng & 7) {
		حالت 0: سائز =   16; توڑو;
		حالت 1: سائز =  256; توڑو;
		حالت 2: سائز = 1024; توڑو;
		حالت 3: سائز = 4096; توڑو;
		طےشدہ: سائز = 0;
	}
	اگر (اشاریہ < 16) سائز *= 16;
	memhasher_store[اشاریہ] = realloc(memhasher_store[اشاریہ], سائز);
}

خالی yosys_banner()
{
	لوگارتھم("\n");
	لوگارتھم(" /----------------------------------------------------------------------------\\\n");
	لوگارتھم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
	لوگارتھم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
	لوگارتھم(" |  Distributed under an ISC-like license, type \"license\" to see terms        |\n");
	لوگارتھم(" \\----------------------------------------------------------------------------/\n");
	لوگارتھم(" %s\n", yosys_maybe_نسخہ());
}

#اگر defined(YOSYS_ENABLE_SPAWN)
صحیح تسلسل_command(ثابت std::string &command, std::function<خالی(ثابت std::string&)> process_سطر)
{
	اگر (!process_سطر)
		واپس system(command.c_str());

	FILE *f = popen(command.c_str(), "r");
	اگر (f == خالی_اشاریہ)
		واپس -1;

	std::string سطر;
	حرف logbuf[128];
	جب (سچ) {
		اگر (fgets(logbuf, 128, f) != تہی) {
			سطر += logbuf;
			اگر (!سطر.خالی_خانے() && سطر.عقب() == '\n')
				process_سطر(سطر), سطر.clear();
		} ورنہ اگر (ferror(f) && errno == EINTR) {
			clearerr(f);
			جاری;
		} ورنہ {
			توڑو;
		}
	}
	اگر (!سطر.خالی_خانے())
		process_سطر(سطر);

	صحیح ret = pclose(f);
	اگر (ret < 0)
		واپس -2;
#اگر_تعریف_شدہ _WIN32
	واپس ret;
#ورنہ
	اگر (!WIFEXITED(ret))
		واپس -1;
	واپس WEXITSTATUS(ret);
#اگر_ختم
}
#اگر_ختم

منطقی already_تیاری = جھوٹ;
منطقی already_shutdown = جھوٹ;

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
// Include pyosys as a package for some compatibility with wheels.
//
// This should not affect using wheels as the dylib has to actually be called
// pyosys.so for this function to be interacted with at all.
PYBIND11_MODULE(pyosys, m) {
	m.جوڑو_object("__path__", py::list());
}

// Catch uses of 'import libyosys' which can import libyosys.so, causing a ton
// of symbol collisions and overall weird behavior.
//
// This should not affect using wheels as the dylib has to actually be called
// libyosys_dummy.so for this function to be interacted with at all.
PYBIND11_MODULE(libyosys_dummy, _) {
	(خالی)_;
	پھینکو py::import_error("Change your import from 'import libyosys' to 'from pyosys import libyosys'.");
}
#اگر_ختم

خالی yosys_تیاری()
{
	اگر(already_تیاری)
		واپس;
	already_تیاری = سچ;
	already_shutdown = جھوٹ;

	StaticTwines::init();

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	// Starting Python 3.12, calling PyImport_AppendInittab on an already
	// initialized platform fails (such as when libyosys is imported
	// from a Python interpreter)
	اگر (!Py_IsInitialized()) {
		PyImport_AppendInittab((حرف*)"pyosys.libyosys", PyInit_libyosys);
		// compatibility with wheels
		PyImport_AppendInittab((حرف*)"pyosys", PyInit_pyosys);
		// prevent catastrophes
		PyImport_AppendInittab((حرف*)"libyosys", PyInit_libyosys_dummy);
		Py_Initialize();
		Py_تسلسل_Simple_String("import sys");
		اشارہ(SIGINT, SIG_DFL);
	}
#اگر_ختم

	init_share_dirname();
	init_abc_executable_name();

	Pass::init_رجسٹر();
	yosys_design = نیا RTLIL::Design;
	yosys_celltypes.static_cell_types = StaticCellTypes::categories.is_معلوم;
	log_ڈالو();
}

منطقی yosys_already_تیاری()
{
	واپس already_تیاری;
}

خالی yosys_shutdown()
{
	اگر(already_shutdown)
		واپس;
	already_تیاری = جھوٹ;
	already_shutdown = سچ;
	log_نکالو();

	Pass::مکمل_رجسٹر();

	حذف yosys_design;
	yosys_design = تہی;

	logger().clear();

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
	اگر (yosys_tcl_interp != تہی) {
		اگر (!Tcl_InterpDeleted(yosys_tcl_interp)) {
			Tcl_DeleteInterp(yosys_tcl_interp);
		}
		Tcl_Finalize();
		yosys_tcl_interp = تہی;
	}
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_PLUGINS
	برائے (خودکار &it : loaded_plugins)
		dlclose(it.دوسرا);

	loaded_plugins.clear();
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	loaded_python_plugins.clear();
#اگر_ختم
	loaded_plugin_aliases.clear();
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	Py_Finalize();
#اگر_ختم
}

ثابت std::string *create_id_سابقہ(std::string_view file, صحیح سطر, std::string_view func)
{
#اگر_تعریف_شدہ _WIN32
	سائز_t pos = file.ڈھونڈو_آخری_کا("/\\");
#ورنہ
	سائز_t pos = file.ڈھونڈو_آخری_کا('/');
#اگر_ختم
	اگر (pos != std::string_view::npos)
		file = file.substr(pos+1);

	pos = func.ڈھونڈو_آخری_کا(':');
	اگر (pos != std::string_view::npos)
		func = func.substr(pos+1);

	واپس نیا std::string(stringf("$auto$%s:%d:%s$", file, سطر, func));
}

RTLIL::Design *yosys_get_design()
{
	واپس yosys_design;
}

ثابت حرف *create_prompt(RTLIL::Design *design, صحیح recursion_شمار, منطقی newline)
{
	ساکن حرف بفر[100];
	std::string str = newline ? "\n" : "";
	اگر (recursion_شمار > 1)
		str += stringf("(%d) ", recursion_شمار);
	str += "yosys";
	اگر (!design->selected_فعال_module.خالی_خانے())
		str += stringf(" [%s]", RTLIL::unescape_id(design->twines().str(design->selected_فعال_module)).c_str());
	اگر (!design->بھرا_selection()) {
		اگر (design->selected_فعال_module.خالی_خانے())
			str += "*";
		ورنہ اگر (design->selection().selected_modules.سائز() != 1 || design->selection().selected_members.سائز() != 0 ||
				design->selection().selected_modules.گنتی(design->selected_فعال_module) == 0)
			str += "*";
	}
	snprintf(بفر, 100, "%s> ", str.c_str());
	واپس بفر;
}

std::سمتیہ<std::string> glob_filename(ثابت std::string &filename_نمونہ)
{
	std::سمتیہ<std::string> results;

#اگر defined(_WIN32) || !defined(YOSYS_ENABLE_GLOB)
	results.ڈالو_عقب(filename_نمونہ);
#ورنہ
	glob_t globbuf;

	صحیح err = glob(filename_نمونہ.c_str(), 0, تہی, &globbuf);

	اگر(err == 0) {
		برائے (سائز_t i = 0; i < globbuf.gl_pathc; i++)
			results.ڈالو_عقب(globbuf.gl_pathv[i]);
		globfree(&globbuf);
	} ورنہ {
		results.ڈالو_عقب(filename_نمونہ);
	}
#اگر_ختم

	واپس results;
}

خالی rewrite_filename(std::string &filename)
{
	اگر (filename.compare(0, 1, "\"") == 0 && filename.compare(Get_سائز(filename)-1, std::string::npos, "\"") == 0)
		filename = filename.substr(1, Get_سائز(filename)-2);
	اگر (filename.compare(0, 2, "+/") == 0)
		filename = proc_share_dirname() + filename.substr(2);
#اگر_غیر_تعریف_شدہ _WIN32
	اگر (filename.compare(0, 2, "~/") == 0)
		filename = filename.replace(0, 1, getenv("HOME"));
#اگر_ختم
}

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL

// defined in tclapi.cc
خارجی صحیح yosys_tcl_interp_init(Tcl_Interp *interp);

خارجی Tcl_Interp *yosys_get_tcl_interp()
{
	اگر (yosys_tcl_interp == تہی) {
		yosys_tcl_interp = Tcl_CreateInterp();
		yosys_tcl_interp_init(yosys_tcl_interp);
	}
	واپس yosys_tcl_interp;
}

// Also see SdcPass
ساخت TclPass : عوامی Pass {
	TclPass() : Pass("tcl", "execute a TCL script file") { }
	خالی help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لوگارتھم("\n");
		لوگارتھم("    tcl <filename> [args]\n");
		لوگارتھم("\n");
		لوگارتھم("This command executes the tcl commands in the specified file.\n");
		لوگارتھم("Use 'yosys cmd' to run the yosys command 'cmd' from tcl.\n");
		لوگارتھم("\n");
		لوگارتھم("The tcl command 'yosys -import' can be used to import all yosys\n");
		لوگارتھم("commands directly as tcl commands to the tcl shell. Yosys commands\n");
		لوگارتھم("'proc' and 'rename' are wrapped to tcl commands 'procs' and 'renames'\n");
		لوگارتھم("in order to avoid a name collision with the built in commands.\n");
		لوگارتھم("\n");
		لوگارتھم("If any arguments are specified, these arguments are provided to the script via\n");
		لوگارتھم("the standard $argc and $argv variables.\n");
		لوگارتھم("\n");
		لوگارتھم("Note, tcl will not receive the output of any yosys command. If the output\n");
		لوگارتھم("of the tcl commands are needed, use the yosys command 'tee -s result.string'\n");
		لوگارتھم("to redirect yosys's output to the 'result.string' scratchpad value.\n");
		لوگارتھم("The 'result.string' value is then used as the tcl output value of the command.\n");
		لوگارتھم("\n");
	}
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *) override {
		اگر (دلائل_2.سائز() < 2)
			log_cmd_error("Missing script file.\n");

		std::سمتیہ<Tcl_Obj*> script_دلائل;
		برائے (خودکار it = دلائل_2.begin() + 2; it != دلائل_2.انتہا(); ++it)
			script_دلائل.ڈالو_عقب(Tcl_NewStringObj((*it).c_str(), (*it).سائز()));

		Tcl_Interp *interp = yosys_get_tcl_interp();
		Tcl_Preserve(interp);
		Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argc", 4), تہی, Tcl_NewIntObj(script_دلائل.سائز()), 0);
		Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv", 4), تہی, Tcl_NewListObj(script_دلائل.سائز(), script_دلائل.ڈیٹا()), 0);
		Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv0", 5), تہی, Tcl_NewStringObj(دلائل_2[1].c_str(), دلائل_2[1].سائز()), 0);
		اگر (Tcl_EvalFile(interp, دلائل_2[1].c_str()) != TCL_OK)
			log_cmd_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتیجہ(interp));
		Tcl_Release(interp);
	}
} TclPass;

#اگر_ختم

#اگر defined(__linux__) || defined(__CYGWIN__) || defined(__gnu_hurd__)
std::string proc_self_dirname()
{
	std::string راستہ_2(4096, '\0');
	ssize_t buflen = -1;
	// Double until sucess, while avoiding endless loop.  Give up
	// when symlink is longer than 4096*(2^30) = 4398046511104
	// bytes.
	برائے (صحیح tries = 30; 0 < tries; tries--) {
		buflen = readlink("/proc/self/exe", راستہ_2.ڈیٹا(), راستہ_2.سائز());
		اگر (buflen < (ssize_t)راستہ_2.سائز())
			توڑو;
		ورنہ
			راستہ_2.resize(راستہ_2.سائز() * 2);
	}
	اگر (buflen < 0) {
		log_error("readlink(\"/proc/self/exe\") failed: %s\n", strerror(errno));
		راستہ_2.resize(0);
	} ورنہ {
		جب (buflen > 0 && راستہ_2[buflen-1] != '/')
			buflen--;
		راستہ_2.resize(buflen);
	}
	واپس راستہ_2;
}
#وگرنہ_اگر defined(__FreeBSD__) || defined(__NetBSD__)
std::string proc_self_dirname()
{
#اگر_تعریف_شدہ __NetBSD__
	صحیح mib[4] = {CTL_KERN, KERN_PROC_دلائل, getpid(), KERN_PROC_PATHNAME};
#ورنہ
	صحیح mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
#اگر_ختم
	سائز_t buflen;
	حرف *بفر;
	std::string راستہ_2;
	اگر (sysctl(mib, 4, تہی, &buflen, تہی, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	بفر = (حرف*)حافظہ_دو(buflen);
	اگر (بفر == تہی)
		log_error("malloc failed: %s\n", strerror(errno));
	اگر (sysctl(mib, 4, بفر, &buflen, تہی, 0) != 0)
		log_error("sysctl failed: %s\n", strerror(errno));
	جب (buflen > 0 && بفر[buflen-1] != '/')
		buflen--;
	راستہ_2.تفویض(بفر, buflen);
	آزاد(بفر);
	واپس راستہ_2;
}
#وگرنہ_اگر defined(__APPLE__)
std::string proc_self_dirname()
{
	حرف *راستہ_2 = تہی;
	uint32_t buflen = 0;
	جب (NSGet_Executable_راستہ(راستہ_2, &buflen) != 0)
		راستہ_2 = (حرف *) realloc((خالی *) راستہ_2, buflen);
	حرف resolved[راستہ_MAX];
	اگر (realpath(راستہ_2, resolved) != تہی) {
		آزاد(راستہ_2);
		راستہ_2 = strdup(resolved);
		buflen = لڑی_لمبائی(راستہ_2);
	}
	جب (buflen > 0 && راستہ_2[buflen-1] != '/')
		buflen--;
	std::string str(راستہ_2, buflen);
	آزاد(راستہ_2);
	واپس str;
}
#وگرنہ_اگر defined(_WIN32)
std::string proc_self_dirname()
{
	std::wstring wbinpath(4096, L'\0');
	اگر (!GetModuleFileNameW(0, &wbinpath[0], wbinpath.سائز()))
	fprintf(stderr, "GetModuleFileNameW() failed.\n");
	wbinpath.resize(wbinpath.rfind(L'\\') + 1); // remove filename
	std::string ubinpath;
	ubinpath.resize(Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wbinpath.ڈیٹا(), wbinpath.سائز(), تہی, 0, تہی, تہی));
	اگر (Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wbinpath.ڈیٹا(), wbinpath.سائز(), &ubinpath[0], ubinpath.سائز(), تہی, تہی) == 0)
	fprintf(stderr, "WideCharToMultiByte() failed.\n");
	واپس ubinpath;

}
#وگرنہ_اگر defined(__wasm)
std::string proc_self_dirname()
{
	واپس "/";
}
#وگرنہ_اگر defined(__OpenBSD__) || defined(__HAIKU__)
حرف yosys_راستہ[راستہ_MAX];
حرف *yosys_argv0;

std::string proc_self_dirname(خالی)
{
	حرف buf[راستہ_MAX + 1] = "", *راستہ_2, *p;
	// if case argv[0] contains a valid path, return it
	اگر (لڑی_لمبائی(yosys_راستہ) > 0) {
		p = strrchr(yosys_راستہ, '/');
		snprintf(buf, حجم buf, "%*s/", (صحیح)(yosys_راستہ - p), yosys_راستہ);
		واپس buf;
	}
	// if argv[0] does not, reconstruct the path out of $PATH
	راستہ_2 = strdup(getenv("PATH"));
	اگر (!راستہ_2)
		log_error("getenv(\"PATH\") failed: %s\n",  strerror(errno));
	برائے (p = strtok(راستہ_2, ":"); p; p = strtok(تہی, ":")) {
		snprintf(buf, حجم buf, "%s/%s", p, yosys_argv0);
		اگر (access(buf, X_OK) == 0) {
			*(strrchr(buf, '/') + 1) = '\0';
			آزاد(راستہ_2);
			واپس buf;
		}
	}
	آزاد(راستہ_2);
	log_error("Can't determine yosys executable path.\n");
	واپس تہی;
}
#ورنہ
	#غلطی "Don't know how to determine process executable base path!"
#اگر_ختم

#اگر defined(__wasm)
خالی init_share_dirname()
{
	yosys_share_dirname = "/share/";
}
#ورنہ
خالی init_share_dirname()
{
#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	اگر (PyObject_HasAttrString(sys_obj, "_pyosys_share_dirname")) {
		PyObject *share_راستہ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_share_dirname");
		ثابت حرف *share_راستہ = PyUnicode_AsUTF8(share_راستہ_obj);
		yosys_share_dirname = std::string(share_راستہ);
		واپس;
	}
#  endif
	std::string proc_self_راستہ = proc_self_dirname();
#  اگر defined(_WIN32) && !defined(YOSYS_WIN32_UNIX_DIR)
	std::string proc_share_راستہ = proc_self_راستہ + "share\\";
	اگر (check_directory_exists(proc_share_راستہ, سچ)) {
		yosys_share_dirname = proc_share_راستہ;
		واپس;
	}
	proc_share_راستہ = proc_self_راستہ + "..\\share\\";
	اگر (check_directory_exists(proc_share_راستہ, سچ)) {
		yosys_share_dirname = proc_share_راستہ;
		واپس;
	}
#  ورنہ
	std::string proc_share_راستہ = proc_self_راستہ + "share/";
	اگر (check_directory_exists(proc_share_راستہ, سچ)) {
		yosys_share_dirname = proc_share_راستہ;
		واپس;
	}
	proc_share_راستہ = proc_self_راستہ + "../share/" + proc_پروگرام_سابقہ()+ "yosys/";
	اگر (check_directory_exists(proc_share_راستہ, سچ)) {
		yosys_share_dirname = proc_share_راستہ;
		واپس;
	}
	proc_share_راستہ = YOSYS_DATDIR "/";
	اگر (check_directory_exists(proc_share_راستہ, سچ)) {
		yosys_share_dirname = proc_share_راستہ;
		واپس;
	}
#  endif
}
#اگر_ختم

خالی init_abc_executable_name()
{
#اگر_تعریف_شدہ ABCEXTERNAL
	std::string exe_file;
	اگر (std::getenv("ABC")) {
		yosys_abc_executable = std::getenv("ABC");
	} ورنہ {
		yosys_abc_executable = ABCEXTERNAL;
	}
#ورنہ
	yosys_abc_executable = proc_self_dirname() + proc_پروگرام_سابقہ()+ "yosys-abc";
#  ifdef _WIN32
	اگر (!check_file_exists(yosys_abc_executable + ".exe") && check_file_exists(proc_self_dirname() + "..\\" + proc_پروگرام_سابقہ() + "yosys-abc.exe"))
		yosys_abc_executable = proc_self_dirname() + "..\\" + proc_پروگرام_سابقہ() + "yosys-abc";
#  endif

#  ifdef YOSYS_ENABLE_PYTHON
	PyObject *sys_obj = PyImport_ImportModule("sys");

	اگر (PyObject_HasAttrString(sys_obj, "_pyosys_abc")) {
		PyObject *abc_راستہ_obj = PyObject_GetAttrString(sys_obj, "_pyosys_abc");
		ثابت حرف *abc_راستہ = PyUnicode_AsUTF8(abc_راستہ_obj);
		yosys_abc_executable = std::string(abc_راستہ);
	}
#  endif
#اگر_ختم
}

std::string proc_share_dirname()
{
	اگر (yosys_share_dirname.خالی_خانے())
		log_error("init_share_dirname: unable to determine share/ directory!\n");
	واپس yosys_share_dirname;
}

std::string proc_پروگرام_سابقہ()
{
	واپس YOSYS_پروگرام_سابقہ;
}

منطقی fgetline(FILE *f, std::string &بفر)
{
	بفر = "";
	حرف بلاک[4096];
	جب (1) {
		اگر (fgets(بلاک, 4096, f) == تہی)
			واپس جھوٹ;
		بفر += بلاک;
		اگر (بفر.سائز() > 0 && (بفر[بفر.سائز()-1] == '\n' ||  بفر[بفر.سائز()-1] == '\r')) {
			جب (بفر.سائز() > 0 && (بفر[بفر.سائز()-1] == '\n' ||  بفر[بفر.سائز()-1] == '\r'))
				بفر.resize(بفر.سائز()-1);
			واپس سچ;
		}
	}
}

ساکن خالی handle_label(std::string &command, منطقی &from_مقصد_فعال, ثابت std::string &تسلسل_from, ثابت std::string &تسلسل_مقصد)
{
	صحیح pos = 0;
	std::string label;

	جب (pos < Get_سائز(command) && (command[pos] == ' ' || command[pos] == '\t'))
		pos++;

	اگر (pos < Get_سائز(command) && command[pos] == '#')
		واپس;

	جب (pos < Get_سائز(command) && command[pos] != ' ' && command[pos] != '\t' && command[pos] != '\r' && command[pos] != '\n')
		label += command[pos++];

	اگر (Get_سائز(label) > 1 && label.عقب() == ':')
	{
		label = label.substr(0, Get_سائز(label)-1);
		command = command.substr(pos);

		اگر (label == تسلسل_from)
			from_مقصد_فعال = سچ;
		ورنہ اگر (label == تسلسل_مقصد || (تسلسل_from == تسلسل_مقصد && !تسلسل_from.خالی_خانے()))
			from_مقصد_فعال = جھوٹ;
	}
}

منطقی تسلسل_frontend(std::string filename, std::string command, RTLIL::Design *design, std::string *from_مقصد_label)
{
	اگر (design == خالی_اشاریہ)
		design = yosys_design;

	اگر (command == "auto") {
	  std::string filename_trim = filename;

	  خودکار has_extension = [](ثابت std::string& filename, ثابت std::string& extension) {
	    اگر (filename.سائز() >= extension.سائز()) {
	      واپس filename.compare(filename.سائز() - extension.سائز(), extension.سائز(), extension) == 0;
	    }
	    واپس جھوٹ;
	  };

	  اگر (has_extension(filename_trim, ".gz")) {
	    filename_trim.erase(filename_trim.سائز() - 3);
	  }

	  اگر (has_extension(filename_trim, ".v")) {
	    command = " -vlog2k";
	  } ورنہ اگر (has_extension(filename_trim, ".sv")) {
	    command = " -sv";
	  } ورنہ اگر (has_extension(filename_trim, ".vhd") || has_extension(filename_trim, ".vhdl")) {
	    command = " -vhdl";
	  } ورنہ اگر (has_extension(filename_trim, ".blif") || has_extension(filename_trim, ".eblif")) {
	    command = "blif";
	  } ورنہ اگر (has_extension(filename_trim, ".json")) {
	    command = "json";
	  } ورنہ اگر (has_extension(filename_trim, ".il")) {
	    command = "rtlil";
	  } ورنہ اگر (has_extension(filename_trim, ".ys")) {
	    command = "script";
	  } ورنہ اگر (has_extension(filename_trim, ".tcl")) {
	    command = "tcl";
	  } ورنہ اگر (filename == "-") {
	    command = "script";
	  } ورنہ {
	    log_error("Can't guess frontend for input file `%s' (missing -f option)!\n", filename);
	  }
	}

	اگر (command == "script")
	{
		std::string تسلسل_from, تسلسل_مقصد;
		منطقی from_مقصد_فعال = سچ;

		اگر (from_مقصد_label != تہی) {
			سائز_t pos = from_مقصد_label->ڈھونڈو(':');
			اگر (pos == std::string::npos) {
				تسلسل_from = *from_مقصد_label;
				تسلسل_مقصد = *from_مقصد_label;
			} ورنہ {
				تسلسل_from = from_مقصد_label->substr(0, pos);
				تسلسل_مقصد = from_مقصد_label->substr(pos+1);
			}
			from_مقصد_فعال = تسلسل_from.خالی_خانے();
		}

		لوگارتھم("\n");
		log_comment("-- Executing script file `%s' --\n", filename);

		FILE *f = stdin;

		اگر (filename != "-") {
			f = fopen(filename.c_str(), "r");
			yosys_داخلہ_files.داخل_کرو(filename);
		}

		اگر (f == تہی)
			log_error("Can't open script file `%s' for reading: %s\n", filename, strerror(errno));

		FILE *backup_script_file = Frontend::current_script_file;
		std::string backup_script_filename = Frontend::current_script_filename;
		صحیح backup_script_lineno = Frontend::current_script_lineno;
		Frontend::current_script_file = f;
		Frontend::current_script_filename = filename;
		Frontend::current_script_lineno = 0;

		کوشش {
			std::string command;
			جب (fgetline(f, command)) {
				Frontend::current_script_lineno++;
				جب (!command.خالی_خانے() && command[command.سائز()-1] == '\\') {
					std::string اگلا_سطر;
					اگر (!fgetline(f, اگلا_سطر))
						توڑو;
					Frontend::current_script_lineno++;
					command.resize(command.سائز()-1);
					command += اگلا_سطر;
				}
				handle_label(command, from_مقصد_فعال, تسلسل_from, تسلسل_مقصد);
				اگر (from_مقصد_فعال) {
					Pass::call(design, command);
					design->check();
				}
			}

			اگر (!command.خالی_خانے()) {
				handle_label(command, from_مقصد_فعال, تسلسل_from, تسلسل_مقصد);
				اگر (from_مقصد_فعال) {
					Frontend::current_script_lineno++;
					Pass::call(design, command);
					design->check();
				}
			}
		}
		پکڑو (...) {
			Frontend::current_script_file = backup_script_file;
			Frontend::current_script_filename = backup_script_filename;
			Frontend::current_script_lineno = backup_script_lineno;
			پھینکو;
		}

		Frontend::current_script_file = backup_script_file;
		Frontend::current_script_filename = backup_script_filename;
		Frontend::current_script_lineno = backup_script_lineno;

		اگر (filename != "-")
			fclose(f);

		واپس سچ;
	}

	اگر (command == "tcl") {
		Pass::call(design, سمتیہ<string>({command, filename}));
		واپس سچ;
	}

	لوگارتھم("\n");
	اگر (filename == "-") {
		log_comment("-- Parsing stdin using frontend `%s' --\n", command);
	} ورنہ {
		log_comment("-- Parsing `%s' using frontend `%s' --\n", filename, command);
	}

	اگر (command[0] == ' ') {
		خودکار argv = بانٹو_علامات("read" + command);
		argv.ڈالو_عقب(filename);
		Pass::call(design, argv);
	} ورنہ
		Frontend::frontend_call(design, تہی, filename, command);

	design->check();
	واپس جھوٹ;
}

خالی تسلسل_pass(std::string command, RTLIL::Design *design)
{
	اگر (design == خالی_اشاریہ)
		design = yosys_design;

	لوگارتھم("\n");
	log_comment("-- Running command `%s' --\n", command);

	Pass::call(design, command);
}

خالی تسلسل_backend(std::string filename, std::string command, RTLIL::Design *design)
{
	اگر (design == خالی_اشاریہ)
		design = yosys_design;

	اگر (command == "auto") {
		اگر (filename.سائز() > 2 && filename.compare(filename.سائز()-2, std::string::npos, ".v") == 0)
			command = "verilog";
		ورنہ اگر (filename.سائز() > 3 && filename.compare(filename.سائز()-3, std::string::npos, ".sv") == 0)
			command = "verilog -sv";
		ورنہ اگر (filename.سائز() > 3 && filename.compare(filename.سائز()-3, std::string::npos, ".il") == 0)
			command = "rtlil";
		ورنہ اگر (filename.سائز() > 3 && filename.compare(filename.سائز()-3, std::string::npos, ".cc") == 0)
			command = "cxxrtl";
		ورنہ اگر (filename.سائز() > 4 && filename.compare(filename.سائز()-4, std::string::npos, ".aig") == 0)
			command = "aiger";
		ورنہ اگر (filename.سائز() > 5 && filename.compare(filename.سائز()-5, std::string::npos, ".blif") == 0)
			command = "blif";
		ورنہ اگر (filename.سائز() > 5 && filename.compare(filename.سائز()-5, std::string::npos, ".edif") == 0)
			command = "edif";
		ورنہ اگر (filename.سائز() > 5 && filename.compare(filename.سائز()-5, std::string::npos, ".json") == 0)
			command = "json";
		ورنہ اگر (filename == "-")
			command = "rtlil";
		ورنہ اگر (filename.خالی_خانے())
			واپس;
		ورنہ
			log_error("Can't guess backend for output file `%s' (missing -b option)!\n", filename);
	}

	اگر (filename.خالی_خانے())
		filename = "-";

	لوگارتھم("\n");
	اگر (filename == "-") {
		log_comment("-- Writing to stdout using backend `%s' --\n", command);
	} ورنہ {
		log_comment("-- Writing to `%s' using backend `%s' --\n", filename, command);
	}

	Backend::backend_call(design, تہی, filename, command);
}

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ساکن حرف *readline_cmd_generator(ثابت حرف *متن, صحیح state)
{
	ساکن std::map<std::string, Pass*>::iterator it;
	ساکن صحیح len;

	اگر (!state) {
		it = pass_رجسٹر.begin();
		len = لڑی_لمبائی(متن);
	}

	برائے (; it != pass_رجسٹر.انتہا(); it++) {
		اگر (it->پہلا.compare(0, len, متن) == 0)
			واپس strdup((it++)->پہلا.c_str());
	}
	واپس تہی;
}

ساکن حرف *readline_obj_generator(ثابت حرف *متن, صحیح state)
{
	ساکن std::سمتیہ<حرف*> obj_نام;
	ساکن سائز_t idx;

	اگر (!state)
	{
		idx = 0;
		obj_نام.clear();

		RTLIL::Design *design = yosys_get_design();
		صحیح len = لڑی_لمبائی(متن);

		اگر (design->selected_فعال_module.خالی_خانے())
		{
			برائے (خودکار mod : design->modules()) {
				std::string mod_name = mod->name.unescape();
				اگر (mod_name.compare(0, len, متن) == 0)
					obj_نام.ڈالو_عقب(strdup(mod_name.c_str()));
			}
		}
		ورنہ اگر (design->module(design->selected_فعال_module) != خالی_اشاریہ)
		{
			RTLIL::Module *module = design->module(design->selected_فعال_module);

			برائے (خودکار w : module->wires())
				اگر (w->name.unescape().compare(0, len, متن) == 0)
					obj_نام.ڈالو_عقب(strdup(w->name.unescape().c_str()));

			برائے (خودکار &it : module->memories) {
				std::string mem_name = design->twines().unescaped_str(it.پہلا);
				اگر (mem_name.compare(0, len, متن) == 0)
					obj_نام.ڈالو_عقب(strdup(mem_name.c_str()));
			}

			برائے (خودکار cell : module->cells())
				اگر (cell->name.unescape().compare(0, len, متن) == 0)
					obj_نام.ڈالو_عقب(strdup(cell->name.unescape().c_str()));

			برائے (خودکار &it : module->processes) {
				std::string proc_name = design->twines().unescaped_str(it.پہلا);
				اگر (proc_name.compare(0, len, متن) == 0)
					obj_نام.ڈالو_عقب(strdup(proc_name.c_str()));
			}
		}

		std::ترتیب_دو(obj_نام.begin(), obj_نام.انتہا());
	}

	اگر (idx < obj_نام.سائز())
		واپس strdup(obj_نام[idx++]);

	idx = 0;
	obj_نام.clear();
	واپس تہی;
}

ساکن حرف **readline_completion(ثابت حرف *متن, صحیح آغاز, صحیح)
{
	اگر (آغاز == 0)
		واپس rl_completion_matches(متن, readline_cmd_generator);
	اگر (لڑی_موازنہ(rl_سطر_بفر, "read_", 5) && لڑی_موازنہ(rl_سطر_بفر, "write_", 6))
		واپس rl_completion_matches(متن, readline_obj_generator);
	واپس تہی;
}
#اگر_ختم

خالی shell(RTLIL::Design *design)
{
	ساکن صحیح recursion_شمار = 0;

	recursion_شمار++;
	خودکار guard = logger().error_throw_scope();

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	rl_readline_name = (حرف*)"yosys";
	rl_attempted_completion_function = readline_completion;
	rl_basic_لفظ_break_characters = (حرف*)" \t\n";
#اگر_ختم

	حرف *command = تہی;
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	جب ((command = readline(create_prompt(design, recursion_شمار))) != تہی)
	{
#ورنہ
	حرف command_بفر[4096];
	جب (1)
	{
		fputs(create_prompt(design, recursion_شمار), stdout);
		fflush(stdout);
		اگر ((command = fgets(command_بفر, 4096, stdin)) == تہی)
			توڑو;
#اگر_ختم
		اگر (command[strspn(command, " \t\r\n")] == 0) {
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
			آزاد(command);
#اگر_ختم
			جاری;
		}
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		جوڑو_history(command);
#اگر_ختم

		حرف *p = command + strspn(command, " \t\r\n");
		اگر (!لڑی_موازنہ(p, "exit", 4)) {
			p += 4;
			p += strspn(p, " \t\r\n");
			اگر (*p == 0)
				توڑو;
		}

		کوشش {
			log_assert(design->selection_ڈھیر.سائز() == 1);
			Pass::call(design, command);
		} پکڑو (log_cmd_error_exception) {
			جب (design->selection_ڈھیر.سائز() > 1)
				design->نکالو_selection();
			log_reset_ڈھیر();
		}
		design->check();
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
		اگر (command)
			آزاد(command);
#اگر_ختم
	}
	اگر (command == تہی)
		لکھو("exit\n");
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ورنہ
		آزاد(command);
#اگر_ختم
	recursion_شمار--;
}

ساخت ShellPass : عوامی Pass {
	ShellPass() : Pass("shell", "enter interactive command mode") { }
	خالی help() override {
		لوگارتھم("\n");
		لوگارتھم("    shell\n");
		لوگارتھم("\n");
		لوگارتھم("This command enters the interactive command mode. This can be useful\n");
		لوگارتھم("in a script to interrupt the script at a certain point and allow for\n");
		لوگارتھم("interactive inspection or manual synthesis of the design at this point.\n");
		لوگارتھم("\n");
		لوگارتھم("The command prompt of the interactive shell indicates the current\n");
		لوگارتھم("selection (see 'help select'):\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys>\n");
		لوگارتھم("        the entire design is selected\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys*>\n");
		لوگارتھم("        only part of the design is selected\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys [modname]>\n");
		لوگارتھم("        the entire module 'modname' is selected using 'select -module modname'\n");
		لوگارتھم("\n");
		لوگارتھم("    yosys [modname]*>\n");
		لوگارتھم("        only part of current module 'modname' is selected\n");
		لوگارتھم("\n");
		لوگارتھم("When in interactive shell, some errors (e.g. invalid command arguments)\n");
		لوگارتھم("do not terminate yosys but return to the command prompt.\n");
		لوگارتھم("\n");
		لوگارتھم("This command is the default action if nothing else has been specified\n");
		لوگارتھم("on the command line.\n");
		لوگارتھم("\n");
		لوگارتھم("Press Ctrl-D or type 'exit' to leave the interactive shell.\n");
		لوگارتھم("\n");
	}
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) override {
		extra_دلائل(دلائل_2, 1, design, جھوٹ);
		shell(design);
	}
} ShellPass;

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ساخت HistoryPass : عوامی Pass {
	HistoryPass() : Pass("history", "show last interactive commands") { }
	خالی help() override {
		لوگارتھم("\n");
		لوگارتھم("    history\n");
		لوگارتھم("\n");
		لوگارتھم("This command prints all commands in the shell history buffer. This are\n");
		لوگارتھم("all commands executed in an interactive session, but not the commands\n");
		لوگارتھم("from executed scripts.\n");
		لوگارتھم("\n");
	}
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) override {
		extra_دلائل(دلائل_2, 1, design, جھوٹ);
#اگر_تعریف_شدہ YOSYS_ENABLE_READLINE
		برائے(HIST_اندراج **list = history_list(); *list != تہی; list++)
			لوگارتھم("%s\n", (*list)->سطر);
#ورنہ
		برائے (صحیح i = where_history(); history_get(i); i++)
			لوگارتھم("%s\n", history_get(i)->سطر);
#اگر_ختم
	}
} HistoryPass;
#اگر_ختم

ساخت ScriptCmdPass : عوامی Pass {
	ScriptCmdPass() : Pass("script", "execute commands from file or wire") { }
	خالی help() override {
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		لوگارتھم("\n");
		لوگارتھم("    script <filename> [<from_label>:<to_label>]\n");
		لوگارتھم("    script -scriptwire [selection]\n");
		لوگارتھم("\n");
		لوگارتھم("This command executes the yosys commands in the specified file (default\n");
		لوگارتھم("behaviour), or commands embedded in the constant text value connected to the\n");
		لوگارتھم("selected wires.\n");
		لوگارتھم("\n");
		لوگارتھم("In the default (file) case, the 2nd argument can be used to only execute the\n");
		لوگارتھم("section of the file between the specified labels. An empty from label is\n");
		لوگارتھم("synonymous with the beginning of the file and an empty to label is synonymous\n");
		لوگارتھم("with the end of the file.\n");
		لوگارتھم("\n");
		لوگارتھم("If only one label is specified (without ':') then only the block\n");
		لوگارتھم("marked with that label (until the next label) is executed.\n");
		لوگارتھم("\n");
		لوگارتھم("In \"-scriptwire\" mode, the commands on the selected wire(s) will be executed\n");
		لوگارتھم("in the scope of (and thus, relative to) the wires' owning module(s). This\n");
		لوگارتھم("'-module' mode can be exited by using the 'cd' command.\n");
		لوگارتھم("\n");
	}
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) override
	{
		منطقی scriptwire = جھوٹ;

		سائز_t argidx;
		برائے (argidx = 1; argidx < دلائل_2.سائز(); argidx++) {
			اگر (دلائل_2[argidx] == "-scriptwire") {
				scriptwire = سچ;
				جاری;
			}
			توڑو;
		}
		اگر (scriptwire) {
			extra_دلائل(دلائل_2, argidx, design);

			برائے (خودکار mod : design->selected_modules())
				برائے (خودکار &c : mod->connections()) {
					اگر (!c.پہلا.is_wire())
						جاری;
					خودکار w = c.پہلا.as_wire();
					اگر (!mod->selected(w))
						جاری;
					اگر (!c.دوسرا.is_fully_const())
						log_error("RHS of selected wire %s.%s is not constant.\n", mod, w);
					خودکار v = c.دوسرا.as_const();
					Pass::call_on_module(design, mod, v.decode_string());
				}
		}
		ورنہ اگر (دلائل_2.سائز() < 2)
			log_cmd_error("Missing script file.\n");
		ورنہ اگر (دلائل_2.سائز() == 2)
			تسلسل_frontend(دلائل_2[1], "script", design);
		ورنہ اگر (دلائل_2.سائز() == 3)
			تسلسل_frontend(دلائل_2[1], "script", design, &دلائل_2[2]);
		ورنہ
			extra_دلائل(دلائل_2, 2, design, جھوٹ);
	}
} ScriptCmdPass;

YOSYS_NAMESPACE_انتہا
