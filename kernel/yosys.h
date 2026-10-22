/* -*- c++ -*-
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


// *** NOTE TO THE READER ***
//
// Maybe you have just opened this file in the hope to learn more about the
// Yosys API. Let me congratulate you on this great decision!  ;)
//
// If you want to know how the design is represented by Yosys in the memory,
// you should read "kernel/rtlil.h".
//
// If you want to know how to register a command with Yosys, you could read
// "kernel/register.h", but it would be easier to just look at a simple
// example instead. A simple one would be "passes/cmds/log.cc".
//
// This header is very boring. It just defines some general things that
// belong nowhere else and includes the interesting headers.


#اگر_غیر_تعریف_شدہ YOSYS_H
#تعریف_کرو YOSYS_H

#شامل "kernel/yosys_common.h"

#شامل "kernel/log.h"
#شامل "kernel/rtlil.h"
#شامل "kernel/register.h"

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
ساخت Tcl_Interp;
#اگر_ختم

YOSYS_NAMESPACE_BEGIN

خالی yosys_تیاری();

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
bool yosys_already_تیاری();
#اگر_ختم

خالی yosys_shutdown();

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#اگر_ختم

خارجی RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_پروگرام_سابقہ();
ثابت حرف *create_prompt(RTLIL::Design *design, صحیح recursion_شمار, bool newline = true);
std::سمتیہ<std::string> glob_filename(ثابت std::string &filename_نمونہ);
خالی rewrite_filename(std::string &filename);

خالی تسلسل_pass(std::string command, RTLIL::Design *design = nullptr);
bool تسلسل_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_مقصد_label = nullptr);
خالی تسلسل_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
خالی shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
خارجی std::سیٹ<std::string> yosys_داخلہ_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
خارجی ثابت حرف *yosys_نسخہ_str;
خارجی ثابت حرف *yosys_git_hash_str;
خارجی ثابت حرف *yosys_build_datetime_str;
ثابت حرف* yosys_maybe_نسخہ();

// from passes/cmds/design.cc
خارجی std::map<std::string, RTLIL::Design*> saved_designs;
خارجی std::سمتیہ<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
خارجی std::map<std::string, خالی*> loaded_plugins;
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
خارجی std::map<std::string, خالی*> loaded_python_plugins;
#اگر_ختم
خارجی std::map<std::string, std::string> loaded_plugin_aliases;
خالی بھرو_plugin(std::string filename, std::سمتیہ<std::string> aliases);

خارجی std::string yosys_share_dirname;
خارجی std::string yosys_abc_executable;

YOSYS_NAMESPACE_انتہا

#اگر_ختم
