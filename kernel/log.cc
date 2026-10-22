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

#समावेश "kernel/yosys.h"
#समावेश "libs/sha1/sha1.h"
#समावेश "backends/rtlil/rtlil_backend.h"

#अगर !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#पूर्ण_अगर

#अगर defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#पूर्ण_अगर

#समावेश <stdlib.h>
#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <stdarg.h>
#समावेश <vector>
#समावेश <list>

YOSYS_NAMESPACE_BEGIN

bool log_stderr_sink_forced = false;

LogManager &logger()
{
	अटल LogManager instance;
	वापस instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() स्थिर
{
	अटल स्थिर स्वतः initial_time = std::chrono::steady_clock::now();
	वापस initial_time;
}

व्योम (*log_error_atexit)() = nullptr;
व्योम (*log_verific_callback)(पूर्णांक संदेश_type, स्थिर अक्षर *message_id, LogSourceLocation src, स्थिर अक्षर *संदेश) = nullptr;

अटल bool अग्र_print_log = true;

FileLogSink::FileLogSink(स्थिर std::string &filename, bool पंक्ति_buffered, bool append)
	: file(ख_खोलो(filename.c_str(), append ? "at" : "wt"))
{
	अगर (!file)
		फेंको std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	अगर (पंक्ति_buffered)
		रखो_भबफ(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	सफाई();
	अगर (file)
		ख_बंद(file);
}

व्योम FileLogSink::log(स्थिर LogMessage &संदेश)
{
	ख_माला_दो(संदेश.cached_संदेश.c_str(), file);
}

व्योम FileLogSink::सफाई()
{
	ख_साफ(file);
}

व्योम ConsoleLogSink::log(स्थिर LogMessage &संदेश)
{
	खाता *file = (संदेश.severity == LogSeverity::Error || संदेश.severity == LogSeverity::NonFatalError) ? मानक_त्रुटि : मानक_निकास;
	ख_माला_दो(संदेश.cached_संदेश.c_str(), file);
}

व्योम ConsoleLogSink::सफाई()
{
	ख_साफ(मानक_निकास);
	ख_साफ(मानक_त्रुटि);
}

bool StderrLogSink::should_log(स्थिर LogMessage &संदेश) स्थिर
{
	वापस संदेश.severity == LogSeverity::Error || संदेश.severity == LogSeverity::NonFatalError ||
			(संदेश.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

व्योम StderrLogSink::log(स्थिर LogMessage &संदेश)
{
	ख_माला_दो(संदेश.cached_संदेश.c_str(), मानक_त्रुटि);
}

व्योम StderrLogSink::सफाई()
{
	ख_साफ(मानक_त्रुटि);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

व्योम ScratchPadLogSink::log(स्थिर LogMessage &संदेश)
{
	RTLIL::Design *design = yosys_get_design();
	अगर (!design)
		वापस;

	design->scratchpad[scratchpad].append(संदेश.cached_संदेश);
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
	अगर (logger().get_log_time())
	{
		अगर (अग्र_print_log) {
			अग्र_print_log = false;
			स्वतः elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			स्वतः us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", पूर्णांक(us.गिनती() / 1'000'000),पूर्णांक(us.गिनती() % 1'000'000));
		}

		अगर (!format.empty() && format.back() == '\n')
			अग्र_print_log = true;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		अगर (format == "%s" && !message.empty() && message.back() == '\n')
			अग्र_print_log = true;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पंक्ति) : "";
	cached_संदेश = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

व्योम LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	माप_प्रकार remove_leading = 0;
	जबतक (format.आकार() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	अगर (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पंक्ति) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	अगर (str.empty())
		वापस;

	माप_प्रकार nnl_pos = str.find_last_not_of('\n');
	अगर (nnl_pos == std::string::npos)
		newline_गिनती += Get_आकार(str);
	अन्यथा
		newline_गिनती = Get_आकार(str) - nnl_pos - 1;

	अगर (hasher)
		hasher->update(str);

	स्वतः संदेश = LogMessage(severity, src, prefix, format, str_in);
	क्रम (स्वतः &sink : sinks) {
		अगर (sink->should_log(संदेश))
			sink->log(संदेश);
	}

	अगर (severity == LogSeverity::Header)
		str = str_in;

	अटल std::string linebuffer;
	अटल bool warn_regex_recusion_guard = false;

	अगर (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = true;

		अगर (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.सफा();
		}
		अन्यथा
		{
			linebuffer += str;

			अगर (!linebuffer.empty() && linebuffer.back() == '\n') {
				क्रम (स्वतः &re : warn_regexes)
					अगर (std::regex_खोज(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				क्रम (स्वतः &[_, item] : expect_log)
					अगर (std::regex_खोज(linebuffer, item.pattern))
						item.current_गिनती++;

				linebuffer.सफा();
			}
		}

		warn_regex_recusion_guard = false;
	}
}

व्योम LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	अगर (make_debug && !is_debug(1))
		वापस;
	logv_string(severity, src, prefix, format, std::move(str));
}

व्योम LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	अगर (header_गिनती.आकार() > 0)
		header_गिनती.back()++;

	अगर (पूर्णांक(header_गिनती.आकार()) <= verbose_level) {
		log_stderr_sink_forced = true;
	}

	std::string header_id;

	क्रम (पूर्णांक c : header_गिनती)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	सफाई();

	अगर (hdump_all)
		hdump[header_id].जोड़ो("yosys_dump_" + header_id + ".il");

	अगर (hdump.गिनती(header_id) && design != nullptr)
		क्रम (स्वतः &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = false;
}

व्योम LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	bool suppressed = false;

	क्रम (स्वतः &re : nowarn_regexes)
		अगर (std::regex_खोज(message, re))
			suppressed = true;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पंक्ति) : "";
	अगर (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	अन्यथा
	{
		पूर्णांक bak_make_debug = make_debug;
		make_debug = 0;

		क्रम (स्वतः &re : werror_regexes)
			अगर (std::regex_खोज(message, re))
				formatted_error(src, "ERROR: ", format, message);

		bool warning_match = false;
		क्रम (स्वतः &[_, item] : expect_warning)
			अगर (std::regex_खोज(message, item.pattern)) {
				item.current_गिनती++;
				warning_match = true;
			}

		std::string pattern = loc + string(prefix) + message;
		क्रम (स्वतः &[_, item] : expect_prefix_warning)
			अगर (std::regex_खोज(pattern, item.pattern)) {
				item.current_गिनती++;
				warning_match = true;
			}

		अगर (warnings.गिनती(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			सफाई();
		}
		अन्यथा
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			सफाई();
			warnings.जोड़ो(message);
		}

		अगर (!warning_match)
			warnings_गिनती_noexpect++;
		warnings_गिनती++;
		make_debug = bak_make_debug;
	}
}

व्योम LogManager::suppressed() {
	अगर (debug_suppressed && !make_debug) {
		constexpr स्थिर अक्षर* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

व्योम LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	पूर्णांक bak_make_debug = make_debug;
	make_debug = 0;
	अगर (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	सफाई();

	make_debug = bak_make_debug;

	क्रम (स्वतः &[_, item] : expect_error)
		अगर (std::regex_खोज(message, item.pattern))
			item.current_गिनती++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पंक्ति) : "";
	std::string pattern = loc + string(prefix) + message;
	क्रम (स्वतः &[_, item] : expect_prefix_error)
		अगर (std::regex_खोज(pattern, item.pattern))
			item.current_गिनती++;

	errors_गिनती++;
}

[[noreturn]]
व्योम LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	अगर (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	स्थिर अक्षर *e = दो_पर्या("YOSYS_ABORT_ON_LOG_ERROR");
	अगर (e && म_से_प(e))
		पात();

#अगर defined(_MSC_VER)
	_निकास(1);
#अन्यथा
	_Exit(1);
#पूर्ण_अगर
}

व्योम LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

व्योम LogManager::add_experimental(स्थिर std::string &str)
{
	अगर (experimental_ignored.गिनती(str) == 0 && experimental.गिनती(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.जोड़ो(str);
	}
}

व्योम LogManager::add_deprecated(स्थिर std::string &feature, स्थिर std::string &earliest_removal)
{
	अगर (deprecated.गिनती(feature) == 0) {
		अगर (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		अन्यथा
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

व्योम log_assert_failure(स्थिर अक्षर *व्यंजक, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", व्यंजक, location.file_name(), location.पंक्ति());
}

व्योम log_abort_internal(स्थिर अक्षर *file, पूर्णांक पंक्ति)
{
	log_error("Abort in %s:%d.\n", file, पंक्ति);
}

व्योम log_yosys_abort_message(std::string_view file, पूर्णांक पंक्ति, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, पंक्ति, func, message);
}

व्योम LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	अगर (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		सफाई();

		फेंको log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

व्योम LogManager::spacer()
{
	अगर (newline_गिनती < 2) log("\n");
	अगर (newline_गिनती < 2) log("\n");
}

व्योम LogManager::clear_original()
{
	std::erase_if(sinks, [](स्थिर स्वतः &sink) { वापस dynamic_cast<LogSinkRef *>(sink.लो()) != nullptr; });
}

व्योम LogManager::धकेलो()
{
	header_गिनती.धकेलो_back(0);
}

व्योम LogManager::निकालो()
{
	header_गिनती.निकालो_back();
	सफाई();
}

#अगर defined(YOSYS_ENABLE_DLOPEN)
व्योम log_backtrace(स्थिर अक्षर *prefix, पूर्णांक levels)
{
	अगर (levels <= 0) वापस;

	Dl_info dli;
	व्योम *p;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #1: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 1) वापस;

#अगर_अघोषित DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#अन्यथा
	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #2: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 2) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #3: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 3) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #4: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 4) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #5: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 5) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #6: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 6) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #7: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 7) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #8: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 8) वापस;

	अगर ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #9: ---\n", prefix);
		वापस;
	}

	अगर (levels <= 9) वापस;
#पूर्ण_अगर
}
#अन्यथा
व्योम log_backtrace(स्थिर अक्षर*, पूर्णांक) { }
#पूर्ण_अगर

व्योम LogManager::reset_ढेर()
{
	जबतक (header_गिनती.आकार() > 1)
		header_गिनती.निकालो_back();
	सफाई();
}

व्योम log_dump_val_worker(RTLIL::IdString v) {
	अगर (v == IdString::Null)
		log("(null)");
	अन्यथा अगर (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	अन्यथा अगर (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	अन्यथा
		log("%s", v.handle_token());
}

व्योम log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

व्योम log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(स्थिर RTLIL::SigSpec &sig, bool autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	वापस buf.str();
}

std::string log_const(स्थिर RTLIL::Const &मूल्य, bool autoint)
{
	अगर ((मूल्य.flags & RTLIL::CONST_FLAG_STRING) == 0)
		वापस log_signal(मूल्य, autoint);

	वापस "\"" + मूल्य.decode_string() + "\"";
}

व्योम log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, false, true, false, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

व्योम log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

व्योम log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

व्योम LogManager::check_expected()
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

	स्वतः check = [&](स्थिर std::string kind, std::string pattern, LogExpectedItem item) {
		अगर (item.current_गिनती == 0) {
			warn_regexes.सफा();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		अगर (item.current_गिनती != item.expected_गिनती) {
			warn_regexes.सफा();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_गिनती, item.expected_गिनती);
		}
	};

	क्रम (स्वतः &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	क्रम (स्वतः &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	क्रम (स्वतः &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	क्रम (स्वतः &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	स्वतः check_err = [&](स्थिर std::string kind, std::string pattern, LogExpectedItem item) {
		अगर (item.current_गिनती == item.expected_गिनती) {
			warn_regexes.सफा();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#अगर defined(_MSC_VER)
				_निकास(0);
			#अन्यथा
				_Exit(0);
			#पूर्ण_अगर
		} अन्यथा {
			warn_regexes.सफा();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	क्रम (स्वतः &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	क्रम (स्वतः &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

व्योम LogManager::report_unexpected_error()
{
	अगर (expect_no_warnings && warnings_गिनती_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_आकार(warnings),
					warnings_गिनती, warnings_गिनती - warnings_गिनती_noexpect);
}

व्योम LogManager::add_expect(std::string type, std::string pattern, पूर्णांक गिनती)
{
	अगर (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गिनती);
	अन्यथा अगर (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गिनती);
	अन्यथा अगर (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गिनती);
	अन्यथा अगर (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गिनती);
	अन्यथा अगर (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गिनती);
	अन्यथा अगर (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गिनती);
	अन्यथा log_abort();
}

व्योम LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	अगर (!hasher)
		वापस {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	वापस hash;
}

YOSYS_NAMESPACE_END
