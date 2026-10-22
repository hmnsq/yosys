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


#अगर_अघोषित YOSYS_H
#घोषणा YOSYS_H

#समावेश "kernel/yosys_common.h"

#समावेश "kernel/log.h"
#समावेश "kernel/rtlil.h"
#समावेश "kernel/register.h"

#अगर_घोषित YOSYS_ENABLE_TCL
काष्ठा Tcl_Interp;
#पूर्ण_अगर

YOSYS_NAMESPACE_BEGIN

व्योम yosys_setup();

#अगर_घोषित YOSYS_ENABLE_PYTHON
bool yosys_already_setup();
#पूर्ण_अगर

व्योम yosys_shutdown();

#अगर_घोषित YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#पूर्ण_अगर

बाह्य RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_program_prefix();
स्थिर अक्षर *create_prompt(RTLIL::Design *design, पूर्णांक recursion_गणक, bool newline = true);
std::vector<std::string> glob_filename(स्थिर std::string &filename_pattern);
व्योम rewrite_filename(std::string &filename);

व्योम run_pass(std::string command, RTLIL::Design *design = nullptr);
bool run_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_to_label = nullptr);
व्योम run_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
व्योम shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
बाह्य std::set<std::string> yosys_निवेश_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
बाह्य स्थिर अक्षर *yosys_version_str;
बाह्य स्थिर अक्षर *yosys_git_hash_str;
बाह्य स्थिर अक्षर *yosys_build_datetime_str;
स्थिर अक्षर* yosys_maybe_version();

// from passes/cmds/design.cc
बाह्य std::map<std::string, RTLIL::Design*> saved_designs;
बाह्य std::vector<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
बाह्य std::map<std::string, व्योम*> loaded_plugins;
#अगर_घोषित YOSYS_ENABLE_PYTHON
बाह्य std::map<std::string, व्योम*> loaded_python_plugins;
#पूर्ण_अगर
बाह्य std::map<std::string, std::string> loaded_plugin_aliases;
व्योम load_plugin(std::string filename, std::vector<std::string> aliases);

बाह्य std::string yosys_share_dirname;
बाह्य std::string yosys_abc_executable;

YOSYS_NAMESPACE_END

#पूर्ण_अगर
