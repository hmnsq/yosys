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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"

#ଅନ୍ତର୍ଭାଵ "kernel/log.h"
#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ "kernel/register.h"

#ifdef YOSYS_ENABLE_TCL
ସଂରଚନା Tcl_Interp;
#endif

YOSYS_NAMESPACE_BEGIN

ଶୂନ୍ୟ yosys_setup();

#ifdef YOSYS_ENABLE_PYTHON
bool yosys_already_setup();
#endif

ଶୂନ୍ୟ yosys_shutdown();

#ifdef YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#endif

ବାହ୍ୟ RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_program_prefix();
ଅଚଳ ଅକ୍ଷର *create_prompt(RTLIL::Design *design, ପୂର୍ଣ୍ଣସଂଖ୍ୟା recursion_counter, bool newline = true);
std::vector<std::string> glob_filename(ଅଚଳ std::string &filename_pattern);
ଶୂନ୍ୟ rewrite_filename(std::string &filename);

ଶୂନ୍ୟ run_pass(std::string command, RTLIL::Design *design = nullptr);
bool run_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_to_label = nullptr);
ଶୂନ୍ୟ run_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
ଶୂନ୍ୟ shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
ବାହ୍ୟ std::set<std::string> yosys_ନିବେଶ_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର *yosys_version_str;
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର *yosys_git_hash_str;
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର *yosys_build_datetime_str;
ଅଚଳ ଅକ୍ଷର* yosys_maybe_version();

// from passes/cmds/design.cc
ବାହ୍ୟ std::map<std::string, RTLIL::Design*> saved_designs;
ବାହ୍ୟ std::vector<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
ବାହ୍ୟ std::map<std::string, ଶୂନ୍ୟ*> loaded_plugins;
#ifdef YOSYS_ENABLE_PYTHON
ବାହ୍ୟ std::map<std::string, ଶୂନ୍ୟ*> loaded_python_plugins;
#endif
ବାହ୍ୟ std::map<std::string, std::string> loaded_plugin_aliases;
ଶୂନ୍ୟ load_plugin(std::string filename, std::vector<std::string> aliases);

ବାହ୍ୟ std::string yosys_share_dirname;
ବାହ୍ୟ std::string yosys_abc_executable;

YOSYS_NAMESPACE_END

#endif
