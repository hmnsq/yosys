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


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/register.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
ರಚನೆ Tcl_Interp;
#ಆದರೆ_ಅಂತ್ಯ

YOSYS_NAMESPACE_BEGIN

ಶೂನ್ಯ yosys_ಸಿದ್ಧತೆ();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
bool yosys_already_ಸಿದ್ಧತೆ();
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ yosys_shutdown();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_ಕಾರ್ಯಕ್ರಮ_ಪೂರ್ವಪ್ರತ್ಯಯ();
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *create_prompt(RTLIL::Design *design, ಪೂರ್ಣಾಂಕ recursion_counter, bool newline = true);
std::ಸದಿಶ<std::string> glob_filename(ಸ್ಥಿರಾಂಕ std::string &filename_ನಮೂನೆ);
ಶೂನ್ಯ rewrite_filename(std::string &filename);

ಶೂನ್ಯ ಸತತ_pass(std::string command, RTLIL::Design *design = nullptr);
bool ಸತತ_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_ಗಮ್ಯ_label = nullptr);
ಶೂನ್ಯ ಸತತ_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
ಶೂನ್ಯ shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
ಬಾಹ್ಯ std::ಗಣ<std::string> yosys_ಒಳಹರಿವು_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *yosys_ಆವೃತ್ತಿ_str;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *yosys_git_hash_str;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *yosys_build_datetime_str;
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* yosys_maybe_ಆವೃತ್ತಿ();

// from passes/cmds/design.cc
ಬಾಹ್ಯ std::map<std::string, RTLIL::Design*> saved_designs;
ಬಾಹ್ಯ std::ಸದಿಶ<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
ಬಾಹ್ಯ std::map<std::string, ಶೂನ್ಯ*> loaded_plugins;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
ಬಾಹ್ಯ std::map<std::string, ಶೂನ್ಯ*> loaded_python_plugins;
#ಆದರೆ_ಅಂತ್ಯ
ಬಾಹ್ಯ std::map<std::string, std::string> loaded_plugin_aliases;
ಶೂನ್ಯ ತುಂಬು_plugin(std::string filename, std::ಸದಿಶ<std::string> aliases);

ಬಾಹ್ಯ std::string yosys_share_dirname;
ಬಾಹ್ಯ std::string yosys_abc_executable;

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
