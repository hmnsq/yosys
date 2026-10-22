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


#ifndef YOSYS_H
#define YOSYS_H

#ضمن "kernel/yosys_common.h"

#ضمن "kernel/log.h"
#ضمن "kernel/rtlil.h"
#ضمن "kernel/register.h"

#ifdef YOSYS_ENABLE_TCL
بنية Tcl_Interp;
#endif

YOSYS_NAMESPACE_BEGIN

فراغ yosys_setup();

#ifdef YOSYS_ENABLE_PYTHON
bool yosys_already_setup();
#endif

فراغ yosys_shutdown();

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#endif

خارجي RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_program_prefix();
ثابت حرف *create_prompt(RTLIL::Design *design, صحيح recursion_counter, bool newline = true);
std::vector<std::string> glob_filename(ثابت std::string &filename_pattern);
فراغ rewrite_filename(std::string &filename);

فراغ run_pass(std::string command, RTLIL::Design *design = nullptr);
bool run_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_to_label = nullptr);
فراغ run_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
فراغ shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
خارجي std::set<std::string> yosys_شغرو_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
خارجي ثابت حرف *yosys_version_str;
خارجي ثابت حرف *yosys_git_hash_str;
خارجي ثابت حرف *yosys_build_datetime_str;
ثابت حرف* yosys_maybe_version();

// from passes/cmds/design.cc
خارجي std::map<std::string, RTLIL::Design*> saved_designs;
خارجي std::vector<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
خارجي std::map<std::string, فراغ*> loaded_plugins;
#ifdef YOSYS_ENABLE_PYTHON
خارجي std::map<std::string, فراغ*> loaded_python_plugins;
#endif
خارجي std::map<std::string, std::string> loaded_plugin_aliases;
فراغ load_plugin(std::string filename, std::vector<std::string> aliases);

خارجي std::string yosys_share_dirname;
خارجي std::string yosys_abc_executable;

YOSYS_NAMESPACE_END

#endif
