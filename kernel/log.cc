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

#जइ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#जइ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <vector>
#अन्तर्भाव <list>

YOSYS_NAMESPACE_BEGIN

तार्किक log_stderr_sink_forced = मिच्छ;

LogManager &logger()
{
	स्थिर LogManager instance;
	प्रत्यागम instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ध्रुव
{
	स्थिर ध्रुव auto initial_time = std::chrono::steady_clock::now();
	प्रत्यागम initial_time;
}

शून्य (*log_error_atexit)() = nullptr;
शून्य (*log_verific_callback)(पूर्णाङ्क सन्देशः_type, ध्रुव वर्ण *message_id, LogSourceLocation src, ध्रुव वर्ण *सन्देशः) = nullptr;

स्थिर तार्किक next_print_log = सच्च;

FileLogSink::FileLogSink(ध्रुव std::string &filename, तार्किक पङ्क्तिः_buffered, तार्किक append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	जइ (!file)
		क्षिप std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	जइ (पङ्क्तिः_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	जइ (file)
		fclose(file);
}

शून्य FileLogSink::log(ध्रुव LogMessage &सन्देशः)
{
	fputs(सन्देशः.cached_सन्देशः.c_str(), file);
}

शून्य FileLogSink::flush()
{
	fflush(file);
}

शून्य ConsoleLogSink::log(ध्रुव LogMessage &सन्देशः)
{
	FILE *file = (सन्देशः.severity == LogSeverity::Error || सन्देशः.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(सन्देशः.cached_सन्देशः.c_str(), file);
}

शून्य ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

तार्किक StderrLogSink::should_log(ध्रुव LogMessage &सन्देशः) ध्रुव
{
	प्रत्यागम सन्देशः.severity == LogSeverity::Error || सन्देशः.severity == LogSeverity::NonFatalError ||
			(सन्देशः.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

शून्य StderrLogSink::log(ध्रुव LogMessage &सन्देशः)
{
	fputs(सन्देशः.cached_सन्देशः.c_str(), stderr);
}

शून्य StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

शून्य ScratchPadLogSink::log(ध्रुव LogMessage &सन्देशः)
{
	RTLIL::Design *design = yosys_get_design();
	जइ (!design)
		प्रत्यागम;

	design->scratchpad[scratchpad].append(सन्देशः.cached_सन्देशः);
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
	जइ (logger().get_log_time())
	{
		जइ (next_print_log) {
			next_print_log = मिच्छ;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", पूर्णाङ्क(us.गणनम्() / 1'000'000),पूर्णाङ्क(us.गणनम्() % 1'000'000));
		}

		जइ (!format.empty() && format.back() == '\n')
			next_print_log = सच्च;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		जइ (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = सच्च;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	cached_सन्देशः = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

शून्य LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	जाव (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	जइ (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	जइ (str.empty())
		प्रत्यागम;

	size_t nnl_pos = str.find_last_not_of('\n');
	जइ (nnl_pos == std::string::npos)
		newline_गणनम् += GetSize(str);
	अण्णहा
		newline_गणनम् = GetSize(str) - nnl_pos - 1;

	जइ (hasher)
		hasher->update(str);

	auto सन्देशः = LogMessage(severity, src, prefix, format, str_in);
	प्रति (auto &sink : sinks) {
		जइ (sink->should_log(सन्देशः))
			sink->log(सन्देशः);
	}

	जइ (severity == LogSeverity::Header)
		str = str_in;

	स्थिर std::string linebuffer;
	स्थिर तार्किक warn_regex_recusion_guard = मिच्छ;

	जइ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = सच्च;

		जइ (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		अण्णहा
		{
			linebuffer += str;

			जइ (!linebuffer.empty() && linebuffer.back() == '\n') {
				प्रति (auto &re : warn_regexes)
					जइ (std::regex_अन्वेषणम्(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				प्रति (auto &[_, item] : expect_log)
					जइ (std::regex_अन्वेषणम्(linebuffer, item.pattern))
						item.current_गणनम्++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = मिच्छ;
	}
}

शून्य LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	जइ (make_debug && !is_debug(1))
		प्रत्यागम;
	logv_string(severity, src, prefix, format, std::move(str));
}

शून्य LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	जइ (header_गणनम्.size() > 0)
		header_गणनम्.back()++;

	जइ (पूर्णाङ्क(header_गणनम्.size()) <= verbose_level) {
		log_stderr_sink_forced = सच्च;
	}

	std::string header_id;

	प्रति (पूर्णाङ्क c : header_गणनम्)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	जइ (hdump_all)
		hdump[header_id].निवेशय("yosys_dump_" + header_id + ".il");

	जइ (hdump.गणनम्(header_id) && design != nullptr)
		प्रति (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = मिच्छ;
}

शून्य LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	तार्किक suppressed = मिच्छ;

	प्रति (auto &re : nowarn_regexes)
		जइ (std::regex_अन्वेषणम्(message, re))
			suppressed = सच्च;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	जइ (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	अण्णहा
	{
		पूर्णाङ्क bak_make_debug = make_debug;
		make_debug = 0;

		प्रति (auto &re : werror_regexes)
			जइ (std::regex_अन्वेषणम्(message, re))
				formatted_error(src, "ERROR: ", format, message);

		तार्किक warning_match = मिच्छ;
		प्रति (auto &[_, item] : expect_warning)
			जइ (std::regex_अन्वेषणम्(message, item.pattern)) {
				item.current_गणनम्++;
				warning_match = सच्च;
			}

		std::string pattern = loc + string(prefix) + message;
		प्रति (auto &[_, item] : expect_prefix_warning)
			जइ (std::regex_अन्वेषणम्(pattern, item.pattern)) {
				item.current_गणनम्++;
				warning_match = सच्च;
			}

		जइ (warnings.गणनम्(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		अण्णहा
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.निवेशय(message);
		}

		जइ (!warning_match)
			warnings_गणनम्_noexpect++;
		warnings_गणनम्++;
		make_debug = bak_make_debug;
	}
}

शून्य LogManager::suppressed() {
	जइ (debug_suppressed && !make_debug) {
		constexpr ध्रुव वर्ण* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

शून्य LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	पूर्णाङ्क bak_make_debug = make_debug;
	make_debug = 0;
	जइ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	प्रति (auto &[_, item] : expect_error)
		जइ (std::regex_अन्वेषणम्(message, item.pattern))
			item.current_गणनम्++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	std::string pattern = loc + string(prefix) + message;
	प्रति (auto &[_, item] : expect_prefix_error)
		जइ (std::regex_अन्वेषणम्(pattern, item.pattern))
			item.current_गणनम्++;

	errors_गणनम्++;
}

[[noreturn]]
शून्य LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	जइ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ध्रुव वर्ण *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	जइ (e && atoi(e))
		abort();

#जइ defined(_MSC_VER)
	_exit(1);
#अण्णहा
	_Exit(1);
#endif
}

शून्य LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

शून्य LogManager::add_experimental(ध्रुव std::string &str)
{
	जइ (experimental_ignored.गणनम्(str) == 0 && experimental.गणनम्(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.निवेशय(str);
	}
}

शून्य LogManager::add_deprecated(ध्रुव std::string &feature, ध्रुव std::string &earliest_removal)
{
	जइ (deprecated.गणनम्(feature) == 0) {
		जइ (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		अण्णहा
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

शून्य log_assert_failure(ध्रुव वर्ण *व्यञ्जकम्, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", व्यञ्जकम्, location.file_name(), location.पङ्क्तिः());
}

शून्य log_abort_internal(ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः)
{
	log_error("Abort in %s:%d.\n", file, पङ्क्तिः);
}

शून्य log_yosys_abort_message(std::string_view file, पूर्णाङ्क पङ्क्तिः, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, पङ्क्तिः, func, message);
}

शून्य LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	जइ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		क्षिप log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

शून्य LogManager::spacer()
{
	जइ (newline_गणनम् < 2) log("\n");
	जइ (newline_गणनम् < 2) log("\n");
}

शून्य LogManager::clear_original()
{
	std::erase_if(sinks, [](ध्रुव auto &sink) { प्रत्यागम dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

शून्य LogManager::प्रक्षिप()
{
	header_गणनम्.प्रक्षिप_back(0);
}

शून्य LogManager::निष्कासय()
{
	header_गणनम्.निष्कासय_back();
	flush();
}

#जइ defined(YOSYS_ENABLE_DLOPEN)
शून्य log_backtrace(ध्रुव वर्ण *prefix, पूर्णाङ्क levels)
{
	जइ (levels <= 0) प्रत्यागम;

	Dl_info dli;
	शून्य *p;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #1: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 1) प्रत्यागम;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#अण्णहा
	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #2: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 2) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #3: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 3) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #4: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 4) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #5: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 5) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #6: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 6) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #7: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 7) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #8: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 8) प्रत्यागम;

	जइ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अण्णहा {
		log("%sframe #9: ---\n", prefix);
		प्रत्यागम;
	}

	जइ (levels <= 9) प्रत्यागम;
#endif
}
#अण्णहा
शून्य log_backtrace(ध्रुव वर्ण*, पूर्णाङ्क) { }
#endif

शून्य LogManager::reset_राशिः()
{
	जाव (header_गणनम्.size() > 1)
		header_गणनम्.निष्कासय_back();
	flush();
}

शून्य log_dump_val_worker(RTLIL::IdString v) {
	जइ (v == IdString::Null)
		log("(null)");
	अण्णहा जइ (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	अण्णहा जइ (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	अण्णहा
		log("%s", v.handle_token());
}

शून्य log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

शून्य log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ध्रुव RTLIL::SigSpec &sig, तार्किक autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम buf.str();
}

std::string log_const(ध्रुव RTLIL::Const &मानम्, तार्किक autoint)
{
	जइ ((मानम्.flags & RTLIL::CONST_FLAG_STRING) == 0)
		प्रत्यागम log_signal(मानम्, autoint);

	प्रत्यागम "\"" + मानम्.decode_string() + "\"";
}

शून्य log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, मिच्छ, सच्च, मिच्छ, RTLIL_BACKEND::DumpMode::Readable);
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

	auto check = [&](ध्रुव std::string kind, std::string pattern, LogExpectedItem item) {
		जइ (item.current_गणनम् == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		जइ (item.current_गणनम् != item.expected_गणनम्) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_गणनम्, item.expected_गणनम्);
		}
	};

	प्रति (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	प्रति (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	प्रति (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	प्रति (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ध्रुव std::string kind, std::string pattern, LogExpectedItem item) {
		जइ (item.current_गणनम् == item.expected_गणनम्) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#जइ defined(_MSC_VER)
				_exit(0);
			#अण्णहा
				_Exit(0);
			#endif
		} अण्णहा {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	प्रति (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	प्रति (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

शून्य LogManager::report_unexpected_error()
{
	जइ (expect_no_warnings && warnings_गणनम्_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_गणनम्, warnings_गणनम् - warnings_गणनम्_noexpect);
}

शून्य LogManager::add_expect(std::string type, std::string pattern, पूर्णाङ्क गणनम्)
{
	जइ (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गणनम्);
	अण्णहा जइ (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गणनम्);
	अण्णहा जइ (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गणनम्);
	अण्णहा जइ (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गणनम्);
	अण्णहा जइ (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गणनम्);
	अण्णहा जइ (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), गणनम्);
	अण्णहा log_abort();
}

शून्य LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	जइ (!hasher)
		प्रत्यागम {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	प्रत्यागम hash;
}

YOSYS_NAMESPACE_END
