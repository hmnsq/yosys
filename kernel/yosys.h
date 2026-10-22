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


#यदि_अपरिभाषितम् YOSYS_H
#परिभाषय YOSYS_H

#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/register.h"

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
संरचना Tcl_Interp;
#यदि_अन्तः

YOSYS_NAMESPACE_BEGIN

शून्य yosys_सज्जता();

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
bool yosys_already_सज्जता();
#यदि_अन्तः

शून्य yosys_shutdown();

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#यदि_अन्तः

बाह्य RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_क्रमादेशः_उपसर्गः();
ध्रुव वर्ण *create_prompt(RTLIL::Design *design, पूर्णाङ्क recursion_गणक, bool newline = true);
std::सदिशम्<std::string> glob_filename(ध्रुव std::string &filename_आदर्शः);
शून्य rewrite_filename(std::string &filename);

शून्य सततम्_pass(std::string command, RTLIL::Design *design = nullptr);
bool सततम्_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_गन्तव्यम्_label = nullptr);
शून्य सततम्_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
शून्य shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
बाह्य std::समुच्चयः<std::string> yosys_निवेशः_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
बाह्य ध्रुव वर्ण *yosys_संस्करणम्_str;
बाह्य ध्रुव वर्ण *yosys_git_hash_str;
बाह्य ध्रुव वर्ण *yosys_build_datetime_str;
ध्रुव वर्ण* yosys_maybe_संस्करणम्();

// from passes/cmds/design.cc
बाह्य std::map<std::string, RTLIL::Design*> saved_designs;
बाह्य std::सदिशम्<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
बाह्य std::map<std::string, शून्य*> loaded_plugins;
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
बाह्य std::map<std::string, शून्य*> loaded_python_plugins;
#यदि_अन्तः
बाह्य std::map<std::string, std::string> loaded_plugin_aliases;
शून्य आरोपय_plugin(std::string filename, std::सदिशम्<std::string> aliases);

बाह्य std::string yosys_share_dirname;
बाह्य std::string yosys_abc_executable;

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
