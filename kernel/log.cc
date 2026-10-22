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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "libs/sha1/sha1.h"
#અન્તર્ભાવ "backends/rtlil/rtlil_backend.h"

#જો !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#જો defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <vector>
#અન્તર્ભાવ <list>

YOSYS_NAMESPACE_BEGIN

તાર્કિક log_stderr_sink_forced = ખોટું;

LogManager &logger()
{
	સ્થિર LogManager instance;
	પાછા instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() અચલ
{
	સ્થિર અચલ auto initial_time = std::chrono::steady_clock::now();
	પાછા initial_time;
}

શૂન્ય (*log_error_atexit)() = nullptr;
શૂન્ય (*log_verific_callback)(પૂર્ણાંક સંદેશ_type, અચલ અક્ષર *message_id, LogSourceLocation src, અચલ અક્ષર *સંદેશ) = nullptr;

સ્થિર તાર્કિક next_print_log = સાચું;

FileLogSink::FileLogSink(અચલ std::string &filename, તાર્કિક લીટી_buffered, તાર્કિક append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	જો (!file)
		ફેંકો std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	જો (લીટી_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	જો (file)
		fclose(file);
}

શૂન્ય FileLogSink::log(અચલ LogMessage &સંદેશ)
{
	fputs(સંદેશ.cached_સંદેશ.c_str(), file);
}

શૂન્ય FileLogSink::flush()
{
	fflush(file);
}

શૂન્ય ConsoleLogSink::log(અચલ LogMessage &સંદેશ)
{
	FILE *file = (સંદેશ.severity == LogSeverity::Error || સંદેશ.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(સંદેશ.cached_સંદેશ.c_str(), file);
}

શૂન્ય ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

તાર્કિક StderrLogSink::should_log(અચલ LogMessage &સંદેશ) અચલ
{
	પાછા સંદેશ.severity == LogSeverity::Error || સંદેશ.severity == LogSeverity::NonFatalError ||
			(સંદેશ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

શૂન્ય StderrLogSink::log(અચલ LogMessage &સંદેશ)
{
	fputs(સંદેશ.cached_સંદેશ.c_str(), stderr);
}

શૂન્ય StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

શૂન્ય ScratchPadLogSink::log(અચલ LogMessage &સંદેશ)
{
	RTLIL::Design *design = yosys_get_design();
	જો (!design)
		પાછા;

	design->scratchpad[scratchpad].append(સંદેશ.cached_સંદેશ);
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
	જો (logger().get_log_time())
	{
		જો (next_print_log) {
			next_print_log = ખોટું;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", પૂર્ણાંક(us.ગણતરી() / 1'000'000),પૂર્ણાંક(us.ગણતરી() % 1'000'000));
		}

		જો (!format.empty() && format.back() == '\n')
			next_print_log = સાચું;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		જો (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = સાચું;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_લીટી) : "";
	cached_સંદેશ = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

શૂન્ય LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	જ્યારે (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	જો (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_લીટી) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	જો (str.empty())
		પાછા;

	size_t nnl_pos = str.find_last_not_of('\n');
	જો (nnl_pos == std::string::npos)
		newline_ગણતરી += GetSize(str);
	નહીંતર
		newline_ગણતરી = GetSize(str) - nnl_pos - 1;

	જો (hasher)
		hasher->update(str);

	auto સંદેશ = LogMessage(severity, src, prefix, format, str_in);
	માટે (auto &sink : sinks) {
		જો (sink->should_log(સંદેશ))
			sink->log(સંદેશ);
	}

	જો (severity == LogSeverity::Header)
		str = str_in;

	સ્થિર std::string linebuffer;
	સ્થિર તાર્કિક warn_regex_recusion_guard = ખોટું;

	જો (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = સાચું;

		જો (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		નહીંતર
		{
			linebuffer += str;

			જો (!linebuffer.empty() && linebuffer.back() == '\n') {
				માટે (auto &re : warn_regexes)
					જો (std::regex_શોધ(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				માટે (auto &[_, item] : expect_log)
					જો (std::regex_શોધ(linebuffer, item.pattern))
						item.current_ગણતરી++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = ખોટું;
	}
}

શૂન્ય LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	જો (make_debug && !is_debug(1))
		પાછા;
	logv_string(severity, src, prefix, format, std::move(str));
}

શૂન્ય LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	જો (header_ગણતરી.size() > 0)
		header_ગણતરી.back()++;

	જો (પૂર્ણાંક(header_ગણતરી.size()) <= verbose_level) {
		log_stderr_sink_forced = સાચું;
	}

	std::string header_id;

	માટે (પૂર્ણાંક c : header_ગણતરી)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	જો (hdump_all)
		hdump[header_id].ઉમેરો("yosys_dump_" + header_id + ".il");

	જો (hdump.ગણતરી(header_id) && design != nullptr)
		માટે (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = ખોટું;
}

શૂન્ય LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	તાર્કિક suppressed = ખોટું;

	માટે (auto &re : nowarn_regexes)
		જો (std::regex_શોધ(message, re))
			suppressed = સાચું;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_લીટી) : "";
	જો (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	નહીંતર
	{
		પૂર્ણાંક bak_make_debug = make_debug;
		make_debug = 0;

		માટે (auto &re : werror_regexes)
			જો (std::regex_શોધ(message, re))
				formatted_error(src, "ERROR: ", format, message);

		તાર્કિક warning_match = ખોટું;
		માટે (auto &[_, item] : expect_warning)
			જો (std::regex_શોધ(message, item.pattern)) {
				item.current_ગણતરી++;
				warning_match = સાચું;
			}

		std::string pattern = loc + string(prefix) + message;
		માટે (auto &[_, item] : expect_prefix_warning)
			જો (std::regex_શોધ(pattern, item.pattern)) {
				item.current_ગણતરી++;
				warning_match = સાચું;
			}

		જો (warnings.ગણતરી(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		નહીંતર
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.ઉમેરો(message);
		}

		જો (!warning_match)
			warnings_ગણતરી_noexpect++;
		warnings_ગણતરી++;
		make_debug = bak_make_debug;
	}
}

શૂન્ય LogManager::suppressed() {
	જો (debug_suppressed && !make_debug) {
		constexpr અચલ અક્ષર* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

શૂન્ય LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	પૂર્ણાંક bak_make_debug = make_debug;
	make_debug = 0;
	જો (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	માટે (auto &[_, item] : expect_error)
		જો (std::regex_શોધ(message, item.pattern))
			item.current_ગણતરી++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_લીટી) : "";
	std::string pattern = loc + string(prefix) + message;
	માટે (auto &[_, item] : expect_prefix_error)
		જો (std::regex_શોધ(pattern, item.pattern))
			item.current_ગણતરી++;

	errors_ગણતરી++;
}

[[noreturn]]
શૂન્ય LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	જો (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	અચલ અક્ષર *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	જો (e && atoi(e))
		abort();

#જો defined(_MSC_VER)
	_exit(1);
#નહીંતર
	_Exit(1);
#endif
}

શૂન્ય LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

શૂન્ય LogManager::add_experimental(અચલ std::string &str)
{
	જો (experimental_ignored.ગણતરી(str) == 0 && experimental.ગણતરી(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ઉમેરો(str);
	}
}

શૂન્ય LogManager::add_deprecated(અચલ std::string &feature, અચલ std::string &earliest_removal)
{
	જો (deprecated.ગણતરી(feature) == 0) {
		જો (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		નહીંતર
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

શૂન્ય log_assert_failure(અચલ અક્ષર *પદાવલિ, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", પદાવલિ, location.file_name(), location.લીટી());
}

શૂન્ય log_abort_internal(અચલ અક્ષર *file, પૂર્ણાંક લીટી)
{
	log_error("Abort in %s:%d.\n", file, લીટી);
}

શૂન્ય log_yosys_abort_message(std::string_view file, પૂર્ણાંક લીટી, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, લીટી, func, message);
}

શૂન્ય LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	જો (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		ફેંકો log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

શૂન્ય LogManager::spacer()
{
	જો (newline_ગણતરી < 2) log("\n");
	જો (newline_ગણતરી < 2) log("\n");
}

શૂન્ય LogManager::clear_original()
{
	std::erase_if(sinks, [](અચલ auto &sink) { પાછા dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

શૂન્ય LogManager::ધકેલો()
{
	header_ગણતરી.ધકેલો_back(0);
}

શૂન્ય LogManager::કાઢો()
{
	header_ગણતરી.કાઢો_back();
	flush();
}

#જો defined(YOSYS_ENABLE_DLOPEN)
શૂન્ય log_backtrace(અચલ અક્ષર *prefix, પૂર્ણાંક levels)
{
	જો (levels <= 0) પાછા;

	Dl_info dli;
	શૂન્ય *p;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #1: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 1) પાછા;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#નહીંતર
	જો ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #2: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 2) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #3: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 3) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #4: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 4) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #5: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 5) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #6: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 6) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #7: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 7) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #8: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 8) પાછા;

	જો ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} નહીંતર {
		log("%sframe #9: ---\n", prefix);
		પાછા;
	}

	જો (levels <= 9) પાછા;
#endif
}
#નહીંતર
શૂન્ય log_backtrace(અચલ અક્ષર*, પૂર્ણાંક) { }
#endif

શૂન્ય LogManager::reset_ઢગલો()
{
	જ્યારે (header_ગણતરી.size() > 1)
		header_ગણતરી.કાઢો_back();
	flush();
}

શૂન્ય log_dump_val_worker(RTLIL::IdString v) {
	જો (v == IdString::Null)
		log("(null)");
	નહીંતર જો (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	નહીંતર જો (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	નહીંતર
		log("%s", v.handle_token());
}

શૂન્ય log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

શૂન્ય log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(અચલ RTLIL::SigSpec &sig, તાર્કિક autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	પાછા buf.str();
}

std::string log_const(અચલ RTLIL::Const &કિંમત, તાર્કિક autoint)
{
	જો ((કિંમત.flags & RTLIL::CONST_FLAG_STRING) == 0)
		પાછા log_signal(કિંમત, autoint);

	પાછા "\"" + કિંમત.decode_string() + "\"";
}

શૂન્ય log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, ખોટું, સાચું, ખોટું, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

શૂન્ય log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

શૂન્ય log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

શૂન્ય LogManager::check_expected()
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

	auto check = [&](અચલ std::string kind, std::string pattern, LogExpectedItem item) {
		જો (item.current_ગણતરી == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		જો (item.current_ગણતરી != item.expected_ગણતરી) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_ગણતરી, item.expected_ગણતરી);
		}
	};

	માટે (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	માટે (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	માટે (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	માટે (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](અચલ std::string kind, std::string pattern, LogExpectedItem item) {
		જો (item.current_ગણતરી == item.expected_ગણતરી) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#જો defined(_MSC_VER)
				_exit(0);
			#નહીંતર
				_Exit(0);
			#endif
		} નહીંતર {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	માટે (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	માટે (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

શૂન્ય LogManager::report_unexpected_error()
{
	જો (expect_no_warnings && warnings_ગણતરી_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_ગણતરી, warnings_ગણતરી - warnings_ગણતરી_noexpect);
}

શૂન્ય LogManager::add_expect(std::string type, std::string pattern, પૂર્ણાંક ગણતરી)
{
	જો (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ગણતરી);
	નહીંતર જો (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ગણતરી);
	નહીંતર જો (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ગણતરી);
	નહીંતર જો (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ગણતરી);
	નહીંતર જો (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ગણતરી);
	નહીંતર જો (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ગણતરી);
	નહીંતર log_abort();
}

શૂન્ય LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	જો (!hasher)
		પાછા {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	પાછા hash;
}

YOSYS_NAMESPACE_END
