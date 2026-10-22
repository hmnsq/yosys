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

#ضمن "kernel/yosys.h"
#ضمن "libs/sha1/sha1.h"
#ضمن "backends/rtlil/rtlil_backend.h"

#𐤀𐤌 !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#𐤀𐤌 defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <stdarg.h>
#ضمن <vector>
#ضمن <list>

YOSYS_NAMESPACE_BEGIN

منطقي log_stderr_sink_forced = خطأ;

LogManager &logger()
{
	ساكن LogManager instance;
	أرجع instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ثابت
{
	ساكن ثابت auto initial_time = std::chrono::steady_clock::now();
	أرجع initial_time;
}

فراغ (*log_error_atexit)() = nullptr;
فراغ (*log_verific_callback)(صحيح msg_type, ثابت حرف *message_id, LogSourceLocation src, ثابت حرف *msg) = nullptr;

ساكن منطقي next_print_log = true;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقي line_buffered, منطقي append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	𐤀𐤌 (!file)
		ارم std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	𐤀𐤌 (line_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	𐤀𐤌 (file)
		fclose(file);
}

فراغ FileLogSink::log(ثابت LogMessage &msg)
{
	fputs(msg.cached_msg.c_str(), file);
}

فراغ FileLogSink::flush()
{
	fflush(file);
}

فراغ ConsoleLogSink::log(ثابت LogMessage &msg)
{
	FILE *file = (msg.severity == LogSeverity::Error || msg.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(msg.cached_msg.c_str(), file);
}

فراغ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي StderrLogSink::should_log(ثابت LogMessage &msg) ثابت
{
	أرجع msg.severity == LogSeverity::Error || msg.severity == LogSeverity::NonFatalError ||
			(msg.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

فراغ StderrLogSink::log(ثابت LogMessage &msg)
{
	fputs(msg.cached_msg.c_str(), stderr);
}

فراغ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

فراغ ScratchPadLogSink::log(ثابت LogMessage &msg)
{
	RTLIL::Design *design = yosys_get_design();
	𐤀𐤌 (!design)
		أرجع;

	design->scratchpad[scratchpad].append(msg.cached_msg);
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
	𐤀𐤌 (logger().get_log_time())
	{
		𐤀𐤌 (next_print_log) {
			next_print_log = خطأ;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", صحيح(us.count() / 1'000'000),صحيح(us.count() % 1'000'000));
		}

		𐤀𐤌 (!format.empty() && format.back() == '\n')
			next_print_log = true;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		𐤀𐤌 (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = true;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	cached_msg = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

فراغ LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	طالما (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	𐤀𐤌 (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	𐤀𐤌 (str.empty())
		أرجع;

	size_t nnl_pos = str.find_last_not_of('\n');
	𐤀𐤌 (nnl_pos == std::string::npos)
		newline_count += GetSize(str);
	وإلا
		newline_count = GetSize(str) - nnl_pos - 1;

	𐤀𐤌 (hasher)
		hasher->update(str);

	auto msg = LogMessage(severity, src, prefix, format, str_in);
	لكل (auto &sink : sinks) {
		𐤀𐤌 (sink->should_log(msg))
			sink->log(msg);
	}

	𐤀𐤌 (severity == LogSeverity::Header)
		str = str_in;

	ساكن std::string linebuffer;
	ساكن منطقي warn_regex_recusion_guard = خطأ;

	𐤀𐤌 (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = true;

		𐤀𐤌 (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		وإلا
		{
			linebuffer += str;

			𐤀𐤌 (!linebuffer.empty() && linebuffer.back() == '\n') {
				لكل (auto &re : warn_regexes)
					𐤀𐤌 (std::regex_search(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				لكل (auto &[_, item] : expect_log)
					𐤀𐤌 (std::regex_search(linebuffer, item.pattern))
						item.current_count++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = خطأ;
	}
}

فراغ LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	𐤀𐤌 (make_debug && !is_debug(1))
		أرجع;
	logv_string(severity, src, prefix, format, std::move(str));
}

فراغ LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	𐤀𐤌 (header_count.size() > 0)
		header_count.back()++;

	𐤀𐤌 (صحيح(header_count.size()) <= verbose_level) {
		log_stderr_sink_forced = true;
	}

	std::string header_id;

	لكل (صحيح c : header_count)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	𐤀𐤌 (hdump_all)
		hdump[header_id].insert("yosys_dump_" + header_id + ".il");

	𐤀𐤌 (hdump.count(header_id) && design != nullptr)
		لكل (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = خطأ;
}

فراغ LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	منطقي suppressed = خطأ;

	لكل (auto &re : nowarn_regexes)
		𐤀𐤌 (std::regex_search(message, re))
			suppressed = true;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	𐤀𐤌 (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	وإلا
	{
		صحيح bak_make_debug = make_debug;
		make_debug = 0;

		لكل (auto &re : werror_regexes)
			𐤀𐤌 (std::regex_search(message, re))
				formatted_error(src, "ERROR: ", format, message);

		منطقي warning_match = خطأ;
		لكل (auto &[_, item] : expect_warning)
			𐤀𐤌 (std::regex_search(message, item.pattern)) {
				item.current_count++;
				warning_match = true;
			}

		std::string pattern = loc + string(prefix) + message;
		لكل (auto &[_, item] : expect_prefix_warning)
			𐤀𐤌 (std::regex_search(pattern, item.pattern)) {
				item.current_count++;
				warning_match = true;
			}

		𐤀𐤌 (warnings.count(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		وإلا
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.insert(message);
		}

		𐤀𐤌 (!warning_match)
			warnings_count_noexpect++;
		warnings_count++;
		make_debug = bak_make_debug;
	}
}

فراغ LogManager::suppressed() {
	𐤀𐤌 (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

فراغ LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	صحيح bak_make_debug = make_debug;
	make_debug = 0;
	𐤀𐤌 (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	لكل (auto &[_, item] : expect_error)
		𐤀𐤌 (std::regex_search(message, item.pattern))
			item.current_count++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	std::string pattern = loc + string(prefix) + message;
	لكل (auto &[_, item] : expect_prefix_error)
		𐤀𐤌 (std::regex_search(pattern, item.pattern))
			item.current_count++;

	errors_count++;
}

[[noreturn]]
فراغ LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	𐤀𐤌 (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ثابت حرف *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	𐤀𐤌 (e && atoi(e))
		abort();

#𐤀𐤌 defined(_MSC_VER)
	_exit(1);
#وإلا
	_Exit(1);
#endif
}

فراغ LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

فراغ LogManager::add_experimental(ثابت std::string &str)
{
	𐤀𐤌 (experimental_ignored.count(str) == 0 && experimental.count(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.insert(str);
	}
}

فراغ LogManager::add_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	𐤀𐤌 (deprecated.count(feature) == 0) {
		𐤀𐤌 (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وإلا
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

فراغ log_assert_failure(ثابت حرف *expr, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", expr, location.file_name(), location.line());
}

فراغ log_abort_internal(ثابت حرف *file, صحيح line)
{
	log_error("Abort in %s:%d.\n", file, line);
}

فراغ log_yosys_abort_message(std::string_view file, صحيح line, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, line, func, message);
}

فراغ LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	𐤀𐤌 (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		ارم log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

فراغ LogManager::spacer()
{
	𐤀𐤌 (newline_count < 2) log("\n");
	𐤀𐤌 (newline_count < 2) log("\n");
}

فراغ LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت auto &sink) { أرجع dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

فراغ LogManager::push()
{
	header_count.push_back(0);
}

فراغ LogManager::pop()
{
	header_count.pop_back();
	flush();
}

#𐤀𐤌 defined(YOSYS_ENABLE_DLOPEN)
فراغ log_backtrace(ثابت حرف *prefix, صحيح levels)
{
	𐤀𐤌 (levels <= 0) أرجع;

	Dl_info dli;
	فراغ *p;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #1: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 1) أرجع;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#وإلا
	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #2: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 2) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #3: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 3) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #4: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 4) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #5: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 5) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #6: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 6) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #7: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 7) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #8: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 8) أرجع;

	𐤀𐤌 ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #9: ---\n", prefix);
		أرجع;
	}

	𐤀𐤌 (levels <= 9) أرجع;
#endif
}
#وإلا
فراغ log_backtrace(ثابت حرف*, صحيح) { }
#endif

فراغ LogManager::reset_stack()
{
	طالما (header_count.size() > 1)
		header_count.pop_back();
	flush();
}

فراغ log_dump_val_worker(RTLIL::IdString v) {
	𐤀𐤌 (v == IdString::Null)
		log("(null)");
	وإلا 𐤀𐤌 (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	وإلا 𐤀𐤌 (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	وإلا
		log("%s", v.handle_token());
}

فراغ log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

فراغ log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقي autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	أرجع buf.str();
}

std::string log_const(ثابت RTLIL::Const &value, منطقي autoint)
{
	𐤀𐤌 ((value.flags & RTLIL::CONST_FLAG_STRING) == 0)
		أرجع log_signal(value, autoint);

	أرجع "\"" + value.decode_string() + "\"";
}

فراغ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, خطأ, true, خطأ, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

فراغ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

فراغ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

فراغ LogManager::check_expected()
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

	auto check = [&](ثابت std::string kind, std::string pattern, LogExpectedItem item) {
		𐤀𐤌 (item.current_count == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		𐤀𐤌 (item.current_count != item.expected_count) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_count, item.expected_count);
		}
	};

	لكل (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	لكل (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	لكل (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	لكل (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ثابت std::string kind, std::string pattern, LogExpectedItem item) {
		𐤀𐤌 (item.current_count == item.expected_count) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#𐤀𐤌 defined(_MSC_VER)
				_exit(0);
			#وإلا
				_Exit(0);
			#endif
		} وإلا {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	لكل (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	لكل (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

فراغ LogManager::report_unexpected_error()
{
	𐤀𐤌 (expect_no_warnings && warnings_count_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_count, warnings_count - warnings_count_noexpect);
}

فراغ LogManager::add_expect(std::string type, std::string pattern, صحيح count)
{
	𐤀𐤌 (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	وإلا 𐤀𐤌 (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	وإلا 𐤀𐤌 (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	وإلا 𐤀𐤌 (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	وإلا 𐤀𐤌 (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	وإلا 𐤀𐤌 (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	وإلا log_abort();
}

فراغ LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	𐤀𐤌 (!hasher)
		أرجع {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	أرجع hash;
}

YOSYS_NAMESPACE_END
