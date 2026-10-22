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


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/register.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
ਬਣਤਰ Tcl_Interp;
#ਜੇ_ਅੰਤ

YOSYS_NAMESPACE_BEGIN

ਖਾਲੀ yosys_ਤਿਆਰੀ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
bool yosys_already_ਤਿਆਰੀ();
#ਜੇ_ਅੰਤ

ਖਾਲੀ yosys_shutdown();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
Tcl_Interp *yosys_get_tcl_interp();
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ RTLIL::Design *yosys_design;

RTLIL::Design *yosys_get_design();
std::string proc_self_dirname();
std::string proc_share_dirname();
std::string proc_ਪ੍ਰੋਗਰਾਮ_ਅਗੇਤਰ();
ਅਚਲ ਅੱਖਰ *create_prompt(RTLIL::Design *design, ਪੂਰਨ recursion_counter, bool newline = true);
std::ਸਦਿਸ਼<std::string> glob_filename(ਅਚਲ std::string &filename_ਨਮੂਨਾ);
ਖਾਲੀ rewrite_filename(std::string &filename);

ਖਾਲੀ ਲੜੀਵਾਰ_pass(std::string command, RTLIL::Design *design = nullptr);
bool ਲੜੀਵਾਰ_frontend(std::string filename, std::string command, RTLIL::Design *design = nullptr, std::string *from_ਮੰਜ਼ਿਲ_label = nullptr);
ਖਾਲੀ ਲੜੀਵਾਰ_backend(std::string filename, std::string command, RTLIL::Design *design = nullptr);
ਖਾਲੀ shell(RTLIL::Design *design);

// journal of all input and output files read (for "yosys -E")
ਬਾਹਰੀ std::ਸੈੱਟ<std::string> yosys_ਨਿਵੇਸ਼_files, yosys_output_files;

// from kernel/version_*.o (cc source generated from Makefile)
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ *yosys_ਸੰਸਕਰਣ_str;
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ *yosys_git_hash_str;
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ *yosys_build_datetime_str;
ਅਚਲ ਅੱਖਰ* yosys_maybe_ਸੰਸਕਰਣ();

// from passes/cmds/design.cc
ਬਾਹਰੀ std::map<std::string, RTLIL::Design*> saved_designs;
ਬਾਹਰੀ std::ਸਦਿਸ਼<RTLIL::Design*> pushed_designs;

// from passes/cmds/pluginc.cc
ਬਾਹਰੀ std::map<std::string, ਖਾਲੀ*> loaded_plugins;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
ਬਾਹਰੀ std::map<std::string, ਖਾਲੀ*> loaded_python_plugins;
#ਜੇ_ਅੰਤ
ਬਾਹਰੀ std::map<std::string, std::string> loaded_plugin_aliases;
ਖਾਲੀ ਲੱਦੋ_plugin(std::string filename, std::ਸਦਿਸ਼<std::string> aliases);

ਬਾਹਰੀ std::string yosys_share_dirname;
ਬਾਹਰੀ std::string yosys_abc_executable;

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
