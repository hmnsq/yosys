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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/sha1/sha1.h"
#अन्तर्भाव "backends/rtlil/rtlil_backend.h"

#यदि !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#यदि defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <vector>
#अन्तर्भाव <list>

YOSYS_NAMESPACE_BEGIN

तार्किक log_stderr_sink_forced = असत्य;

LogManager &logger()
{
	स्थिर LogManager instance;
	फर्काऊ instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() अचर
{
	स्थिर अचर स्वतः initial_time = std::chrono::steady_clock::now();
	फर्काऊ initial_time;
}

शून्य (*log_error_atexit)() = nullptr;
शून्य (*log_verific_callback)(पूर्णाङ्क सन्देश_type, अचर वर्ण *message_id, LogSourceLocation src, अचर वर्ण *सन्देश) = nullptr;

स्थिर तार्किक next_print_log = सत्य;

FileLogSink::FileLogSink(अचर std::string &filename, तार्किक पङ्क्ति_buffered, तार्किक append)
	: file(ख_खोलो(filename.c_str(), append ? "at" : "wt"))
{
	यदि (!file)
		क्षिप std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	यदि (पङ्क्ति_buffered)
		रखो_भबफ(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	यदि (file)
		ख_बंद(file);
}

शून्य FileLogSink::log(अचर LogMessage &सन्देश)
{
	ख_माला_दो(सन्देश.cached_सन्देश.c_str(), file);
}

शून्य FileLogSink::flush()
{
	ख_साफ(file);
}

शून्य ConsoleLogSink::log(अचर LogMessage &सन्देश)
{
	खाता *file = (सन्देश.severity == LogSeverity::Error || सन्देश.severity == LogSeverity::NonFatalError) ? मानक_त्रुटि : मानक_निकास;
	ख_माला_दो(सन्देश.cached_सन्देश.c_str(), file);
}

शून्य ConsoleLogSink::flush()
{
	ख_साफ(मानक_निकास);
	ख_साफ(मानक_त्रुटि);
}

तार्किक StderrLogSink::should_log(अचर LogMessage &सन्देश) अचर
{
	फर्काऊ सन्देश.severity == LogSeverity::Error || सन्देश.severity == LogSeverity::NonFatalError ||
			(सन्देश.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

शून्य StderrLogSink::log(अचर LogMessage &सन्देश)
{
	ख_माला_दो(सन्देश.cached_सन्देश.c_str(), मानक_त्रुटि);
}

शून्य StderrLogSink::flush()
{
	ख_साफ(मानक_त्रुटि);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

शून्य ScratchPadLogSink::log(अचर LogMessage &सन्देश)
{
	RTLIL::Design *design = yosys_get_design();
	यदि (!design)
		फर्काऊ;

	design->scratchpad[scratchpad].append(सन्देश.cached_सन्देश);
}

LogMessage::LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message) :
	severity(severity),
	prefix(prefix),
	format(format),
	message(message),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string time_str;
	यदि (logger().get_log_time())
	{
		यदि (next_print_log) {
			next_print_log = असत्य;
			स्वतः elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			स्वतः us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", पूर्णाङ्क(us.गन्ती() / 1'000'000),पूर्णाङ्क(us.गन्ती() % 1'000'000));
		}

		यदि (!format.empty() && format.back() == '\n')
			next_print_log = सत्य;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		यदि (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = सत्य;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्ति) : "";
	cached_सन्देश = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

शून्य LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	माप_प्रकार remove_leading = 0;
	जबसम्म (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	यदि (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्ति) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	यदि (str.empty())
		फर्काऊ;

	माप_प्रकार nnl_pos = str.find_last_not_of('\n');
	यदि (nnl_pos == std::string::npos)
		newline_गन्ती += GetSize(str);
	अन्यथा
		newline_गन्ती = GetSize(str) - nnl_pos - 1;

	यदि (hasher)
		hasher->update(str);

	स्वतः सन्देश = LogMessage(severity, src, prefix, format, str_in);
	लागि (स्वतः &sink : sinks) {
		यदि (sink->should_log(सन्देश))
			sink->log(सन्देश);
	}

	यदि (severity == LogSeverity::Header)
		str = str_in;

	स्थिर std::string linebuffer;
	स्थिर तार्किक warn_regex_recusion_guard = असत्य;

	यदि (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = सत्य;

		यदि (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		अन्यथा
		{
			linebuffer += str;

			यदि (!linebuffer.empty() && linebuffer.back() == '\n') {
				लागि (स्वतः &re : warn_regexes)
					यदि (std::regex_खोज(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				लागि (स्वतः &[_, item] : expect_log)
					यदि (std::regex_खोज(linebuffer, item.pattern))
						item.current_गन्ती++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = असत्य;
	}
}

शून्य LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	यदि (make_debug && !is_debug(1))
		फर्काऊ;
	logv_string(severity, src, prefix, format, std::move(str));
}

शून्य LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	यदि (header_गन्ती.size() > 0)
		header_गन्ती.back()++;

	यदि (पूर्णाङ्क(header_गन्ती.size()) <= verbose_level) {
		log_stderr_sink_forced = सत्य;
	}

	std::string header_id;

	लागि (पूर्णाङ्क c : header_गन्ती)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	यदि (hdump_all)
		hdump[header_id].घुसाउ("yosys_dump_" + header_id + ".il");

	यदि (hdump.गन्ती(header_id) && design != nullptr)
		लागि (स्वतः &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = असत्य;
}

शून्य LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	तार्किक suppressed = असत्य;

	लागि (स्वतः &re : nowarn_regexes)
		यदि (std::regex_खोज(message, re))
			suppressed = सत्य;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्ति) : "";
	यदि (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	अन्यथा
	{
		पूर्णाङ्क bak_make_debug = make_debug;
		make_debug = 0;

		लागि (स्वतः &re : werror_regexes)
			यदि (std::regex_खोज(message, re))
				formatted_error(src, "ERROR: ", format, message);

		तार्किक warning_match = असत्य;
		लागि (स्वतः &[_, item] : expect_warning)
			यदि (std::regex_खोज(message, item.pattern)) {
				item.current_गन्ती++;
				warning_match = सत्य;
			}

		std::string pattern = loc + string(prefix) + message;
		लागि (स्वतः &[_, item] : expect_prefix_warning)
			यदि (std::regex_खोज(pattern, item.pattern)) {
				item.current_गन्ती++;
				warning_match = सत्य;
			}

		यदि (warnings.गन्ती(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		अन्यथा
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.घुसाउ(message);
		}

		यदि (!warning_match)
			warnings_गन्ती_noexpect++;
		warnings_गन्ती++;
		make_debug = bak_make_debug;
	}
}

शून्य LogManager::suppressed() {
	यदि (debug_suppressed && !make_debug) {
		constexpr अचर वर्ण* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

शून्य LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	पूर्णाङ्क bak_make_debug = make_debug;
	make_debug = 0;
	यदि (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	लागि (स्वतः &[_, item] : expect_error)
		यदि (std::regex_खोज(message, item.pattern))
			item.current_गन्ती++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्ति) : "";
	std::string pattern = loc + string(prefix) + message;
	लागि (स्वतः &[_, item] : expect_prefix_error)
		यदि (std::regex_खोज(pattern, item.pattern))
			item.current_गन्ती++;

	errors_गन्ती++;
}

[[noreturn]]
शून्य LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	यदि (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	अचर वर्ण *e = दो_पर्या("YOSYS_ABORT_ON_LOG_ERROR");
	यदि (e && म_से_प(e))
		पात();

#यदि defined(_MSC_VER)
	_exit(1);
#अन्यथा
	_Exit(1);
#endif
}

शून्य LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

शून्य LogManager::add_experimental(अचर std::string &str)
{
	यदि (experimental_ignored.गन्ती(str) == 0 && experimental.गन्ती(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.घुसाउ(str);
	}
}

शून्य LogManager::add_deprecated(अचर std::string &feature, अचर std::string &earliest_removal)
{
	यदि (deprecated.गन्ती(feature) == 0) {
		यदि (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		अन्यथा
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

शून्य log_assert_failure(अचर वर्ण *अभिव्यक्ति, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", अभिव्यक्ति, location.file_name(), location.पङ्क्ति());
}

शून्य log_abort_internal(अचर वर्ण *file, पूर्णाङ्क पङ्क्ति)
{
	log_error("Abort in %s:%d.\n", file, पङ्क्ति);
}

शून्य log_yosys_abort_message(std::string_view file, पूर्णाङ्क पङ्क्ति, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, पङ्क्ति, func, message);
}

शून्य LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	यदि (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		क्षिप log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

शून्य LogManager::spacer()
{
	यदि (newline_गन्ती < 2) log("\n");
	यदि (newline_गन्ती < 2) log("\n");
}

शून्य LogManager::clear_original()
{
	std::erase_if(sinks, [](अचर स्वतः &sink) { फर्काऊ dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

शून्य LogManager::थप()
{
	header_गन्ती.थप_back(0);
}

शून्य LogManager::झिक()
{
	header_गन्ती.झिक_back();
	flush();
}

#यदि defined(YOSYS_ENABLE_DLOPEN)
शून्य log_backtrace(अचर वर्ण *prefix, पूर्णाङ्क levels)
{
	यदि (levels <= 0) फर्काऊ;

	Dl_info dli;
	शून्य *p;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #1: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 1) फर्काऊ;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#अन्यथा
	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #2: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 2) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #3: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 3) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #4: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 4) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #5: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 5) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #6: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 6) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #7: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 7) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #8: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 8) फर्काऊ;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #9: ---\n", prefix);
		फर्काऊ;
	}

	यदि (levels <= 9) फर्काऊ;
#endif
}
#अन्यथा
शून्य log_backtrace(अचर वर्ण*, पूर्णाङ्क) { }
#endif

शून्य LogManager::reset_थाक()
{
	जबसम्म (header_गन्ती.size() > 1)
		header_गन्ती.झिक_back();
	flush();
}

शून्य log_dump_val_worker(RTLIL::IdString v) {
	यदि (v == IdString::Null)
		log("(null)");
	अन्यथा यदि (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	अन्यथा यदि (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	अन्यथा
		log("%s", v.handle_token());
}

शून्य log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

शून्य log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(अचर RTLIL::SigSpec &sig, तार्किक autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	फर्काऊ buf.str();
}

std::string log_const(अचर RTLIL::Const &मान, तार्किक autoint)
{
	यदि ((मान.flags & RTLIL::CONST_FLAG_STRING) == 0)
		फर्काऊ log_signal(मान, autoint);

	फर्काऊ "\"" + मान.decode_string() + "\"";
}

शून्य log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, असत्य, सत्य, असत्य, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

शून्य log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

शून्य log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

शून्य LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, LogExpectedItem> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, LogExpectedItem> expect_prefix_log_copy, expect_prefix_warning_copy, expect_prefix_error_copy;
	std::swap(expect_warning_copy, expect_warning);
	std::swap(expect_log_copy, expect_log);
	std::swap(expect_error_copy, expect_error);
	std::swap(expect_prefix_warning_copy, expect_prefix_warning);
	std::swap(expect_prefix_log_copy, expect_prefix_log);
	std::swap(expect_prefix_error_copy, expect_prefix_error);

	स्वतः check = [&](अचर std::string kind, std::string pattern, LogExpectedItem item) {
		यदि (item.current_गन्ती == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		यदि (item.current_गन्ती != item.expected_गन्ती) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_गन्ती, item.expected_गन्ती);
		}
	};

	लागि (स्वतः &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	लागि (स्वतः &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	लागि (स्वतः &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	लागि (स्वतः &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	स्वतः check_err = [&](अचर std::string kind, std::string pattern, LogExpectedItem item) {
		यदि (item.current_गन्ती == item.expected_गन्ती) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#यदि defined(_MSC_VER)
				_exit(0);
			#अन्यथा
				_Exit(0);
			#endif
		} अन्यथा {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	लागि (स्वतः &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	लागि (स्वतः &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

शून्य LogManager::report_unexpected_error()
{
	यदि (expect_no_warnings && warnings_गन्ती_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_गन्ती, warnings_गन्ती - warnings_गन्ती_noexpect);
}

शून्य LogManager::add_expect(std::string type, std::string pattern, पूर्णाङ्क गन्ती)
{
	यदि (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गन्ती);
	अन्यथा यदि (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गन्ती);
	अन्यथा यदि (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गन्ती);
	अन्यथा यदि (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गन्ती);
	अन्यथा यदि (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गन्ती);
	अन्यथा यदि (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गन्ती);
	अन्यथा log_abort();
}

शून्य LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	यदि (!hasher)
		फर्काऊ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	फर्काऊ hash;
}

YOSYS_NAMESPACE_END
