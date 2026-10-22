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

#অন্তর্ভাব "kernel/yosys_common.h"

#অন্তর্ভাব "kernel/log.h"
#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব "kernel/register.h"

#ifdef YOSYS_ENABLE_TCL
সংরচনা Tcl_Interp;
#endif

YOSYS_NAMESPACE_BEGIN

শূন্য yosys_setup();

#ifdef YOSYS_ENABLE_PYTHON
bool yosys_already_setup();
#endif

শূন্য yosys_shutdown();

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#endif

বাহ্য RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_program_prefix();
ধ্রুব বর্ণ *create_prompt(RTLIL::Design *design, পূর্ণাঙ্ক recursion_counter, bool newline = true);
std::vector<std::string> glob_filename(ধ্রুব std::string &filename_pattern);
শূন্য rewrite_filename(std::string &filename);

শূন্য run_pass(std::string command, RTLIL::Design *design = nullptr);
bool run_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_to_label = nullptr);
শূন্য run_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
শূন্য shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
বাহ্য std::set<std::string> yosys_প্রবেশ_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
বাহ্য ধ্রুব বর্ণ *yosys_version_str;
বাহ্য ধ্রুব বর্ণ *yosys_git_hash_str;
বাহ্য ধ্রুব বর্ণ *yosys_build_datetime_str;
ধ্রুব বর্ণ* yosys_maybe_version();

// from passes/cmds/design.cc
বাহ্য std::map<std::string, RTLIL::Design*> saved_designs;
বাহ্য std::vector<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
বাহ্য std::map<std::string, শূন্য*> loaded_plugins;
#ifdef YOSYS_ENABLE_PYTHON
বাহ্য std::map<std::string, শূন্য*> loaded_python_plugins;
#endif
বাহ্য std::map<std::string, std::string> loaded_plugin_aliases;
শূন্য load_plugin(std::string filename, std::vector<std::string> aliases);

বাহ্য std::string yosys_share_dirname;
বাহ্য std::string yosys_abc_executable;

YOSYS_NAMESPACE_END

#endif
