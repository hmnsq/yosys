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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "libs/sha1/sha1.h"
#அந்தர்பாவ "backends/rtlil/rtlil_backend.h"

#எனில் !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#எனில் defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <vector>
#அந்தர்பாவ <list>

YOSYS_NAMESPACE_BEGIN

தருக்க log_stderr_sink_forced = பொய்;

LogManager &logger()
{
	நிலையான LogManager instance;
	திரும்பு instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() மாறிலி
{
	நிலையான மாறிலி auto initial_time = std::chrono::steady_clock::now();
	திரும்பு initial_time;
}

வெறுமை (*log_error_atexit)() = nullptr;
வெறுமை (*log_verific_callback)(முழு செய்தி_type, மாறிலி எழுத்து *message_id, LogSourceLocation src, மாறிலி எழுத்து *செய்தி) = nullptr;

நிலையான தருக்க next_print_log = உண்மை;

FileLogSink::FileLogSink(மாறிலி std::string &filename, தருக்க வரி_buffered, தருக்க append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	எனில் (!file)
		எறி std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	எனில் (வரி_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	எனில் (file)
		fclose(file);
}

வெறுமை FileLogSink::log(மாறிலி LogMessage &செய்தி)
{
	fputs(செய்தி.cached_செய்தி.c_str(), file);
}

வெறுமை FileLogSink::flush()
{
	fflush(file);
}

வெறுமை ConsoleLogSink::log(மாறிலி LogMessage &செய்தி)
{
	FILE *file = (செய்தி.severity == LogSeverity::Error || செய்தி.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(செய்தி.cached_செய்தி.c_str(), file);
}

வெறுமை ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

தருக்க StderrLogSink::should_log(மாறிலி LogMessage &செய்தி) மாறிலி
{
	திரும்பு செய்தி.severity == LogSeverity::Error || செய்தி.severity == LogSeverity::NonFatalError ||
			(செய்தி.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

வெறுமை StderrLogSink::log(மாறிலி LogMessage &செய்தி)
{
	fputs(செய்தி.cached_செய்தி.c_str(), stderr);
}

வெறுமை StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

வெறுமை ScratchPadLogSink::log(மாறிலி LogMessage &செய்தி)
{
	RTLIL::Design *design = yosys_get_design();
	எனில் (!design)
		திரும்பு;

	design->scratchpad[scratchpad].append(செய்தி.cached_செய்தி);
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
	எனில் (logger().get_log_time())
	{
		எனில் (next_print_log) {
			next_print_log = பொய்;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", முழு(us.மொத்தம்() / 1'000'000),முழு(us.மொத்தம்() % 1'000'000));
		}

		எனில் (!format.empty() && format.back() == '\n')
			next_print_log = உண்மை;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		எனில் (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = உண்மை;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_வரி) : "";
	cached_செய்தி = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

வெறுமை LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	போது (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	எனில் (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_வரி) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	எனில் (str.empty())
		திரும்பு;

	size_t nnl_pos = str.find_last_not_of('\n');
	எனில் (nnl_pos == std::string::npos)
		newline_மொத்தம் += GetSize(str);
	இல்லையெனில்
		newline_மொத்தம் = GetSize(str) - nnl_pos - 1;

	எனில் (hasher)
		hasher->update(str);

	auto செய்தி = LogMessage(severity, src, prefix, format, str_in);
	க்கு (auto &sink : sinks) {
		எனில் (sink->should_log(செய்தி))
			sink->log(செய்தி);
	}

	எனில் (severity == LogSeverity::Header)
		str = str_in;

	நிலையான std::string linebuffer;
	நிலையான தருக்க warn_regex_recusion_guard = பொய்;

	எனில் (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = உண்மை;

		எனில் (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		இல்லையெனில்
		{
			linebuffer += str;

			எனில் (!linebuffer.empty() && linebuffer.back() == '\n') {
				க்கு (auto &re : warn_regexes)
					எனில் (std::regex_தேடல்(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				க்கு (auto &[_, item] : expect_log)
					எனில் (std::regex_தேடல்(linebuffer, item.pattern))
						item.current_மொத்தம்++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = பொய்;
	}
}

வெறுமை LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	எனில் (make_debug && !is_debug(1))
		திரும்பு;
	logv_string(severity, src, prefix, format, std::move(str));
}

வெறுமை LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	எனில் (header_மொத்தம்.size() > 0)
		header_மொத்தம்.back()++;

	எனில் (முழு(header_மொத்தம்.size()) <= verbose_level) {
		log_stderr_sink_forced = உண்மை;
	}

	std::string header_id;

	க்கு (முழு c : header_மொத்தம்)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	எனில் (hdump_all)
		hdump[header_id].செருகு("yosys_dump_" + header_id + ".il");

	எனில் (hdump.மொத்தம்(header_id) && design != nullptr)
		க்கு (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = பொய்;
}

வெறுமை LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	தருக்க suppressed = பொய்;

	க்கு (auto &re : nowarn_regexes)
		எனில் (std::regex_தேடல்(message, re))
			suppressed = உண்மை;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_வரி) : "";
	எனில் (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	இல்லையெனில்
	{
		முழு bak_make_debug = make_debug;
		make_debug = 0;

		க்கு (auto &re : werror_regexes)
			எனில் (std::regex_தேடல்(message, re))
				formatted_error(src, "ERROR: ", format, message);

		தருக்க warning_match = பொய்;
		க்கு (auto &[_, item] : expect_warning)
			எனில் (std::regex_தேடல்(message, item.pattern)) {
				item.current_மொத்தம்++;
				warning_match = உண்மை;
			}

		std::string pattern = loc + string(prefix) + message;
		க்கு (auto &[_, item] : expect_prefix_warning)
			எனில் (std::regex_தேடல்(pattern, item.pattern)) {
				item.current_மொத்தம்++;
				warning_match = உண்மை;
			}

		எனில் (warnings.மொத்தம்(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		இல்லையெனில்
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.செருகு(message);
		}

		எனில் (!warning_match)
			warnings_மொத்தம்_noexpect++;
		warnings_மொத்தம்++;
		make_debug = bak_make_debug;
	}
}

வெறுமை LogManager::suppressed() {
	எனில் (debug_suppressed && !make_debug) {
		constexpr மாறிலி எழுத்து* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

வெறுமை LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	முழு bak_make_debug = make_debug;
	make_debug = 0;
	எனில் (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	க்கு (auto &[_, item] : expect_error)
		எனில் (std::regex_தேடல்(message, item.pattern))
			item.current_மொத்தம்++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_வரி) : "";
	std::string pattern = loc + string(prefix) + message;
	க்கு (auto &[_, item] : expect_prefix_error)
		எனில் (std::regex_தேடல்(pattern, item.pattern))
			item.current_மொத்தம்++;

	errors_மொத்தம்++;
}

[[noreturn]]
வெறுமை LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	எனில் (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	மாறிலி எழுத்து *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	எனில் (e && atoi(e))
		abort();

#எனில் defined(_MSC_VER)
	_exit(1);
#இல்லையெனில்
	_Exit(1);
#endif
}

வெறுமை LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

வெறுமை LogManager::add_experimental(மாறிலி std::string &str)
{
	எனில் (experimental_ignored.மொத்தம்(str) == 0 && experimental.மொத்தம்(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.செருகு(str);
	}
}

வெறுமை LogManager::add_deprecated(மாறிலி std::string &feature, மாறிலி std::string &earliest_removal)
{
	எனில் (deprecated.மொத்தம்(feature) == 0) {
		எனில் (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		இல்லையெனில்
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

வெறுமை log_assert_failure(மாறிலி எழுத்து *கோவை, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", கோவை, location.file_name(), location.வரி());
}

வெறுமை log_abort_internal(மாறிலி எழுத்து *file, முழு வரி)
{
	log_error("Abort in %s:%d.\n", file, வரி);
}

வெறுமை log_yosys_abort_message(std::string_view file, முழு வரி, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, வரி, func, message);
}

வெறுமை LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	எனில் (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		எறி log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

வெறுமை LogManager::spacer()
{
	எனில் (newline_மொத்தம் < 2) log("\n");
	எனில் (newline_மொத்தம் < 2) log("\n");
}

வெறுமை LogManager::clear_original()
{
	std::erase_if(sinks, [](மாறிலி auto &sink) { திரும்பு dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

வெறுமை LogManager::தள்ளு()
{
	header_மொத்தம்.தள்ளு_back(0);
}

வெறுமை LogManager::எடு()
{
	header_மொத்தம்.எடு_back();
	flush();
}

#எனில் defined(YOSYS_ENABLE_DLOPEN)
வெறுமை log_backtrace(மாறிலி எழுத்து *prefix, முழு levels)
{
	எனில் (levels <= 0) திரும்பு;

	Dl_info dli;
	வெறுமை *p;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #1: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 1) திரும்பு;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#இல்லையெனில்
	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #2: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 2) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #3: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 3) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #4: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 4) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #5: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 5) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #6: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 6) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #7: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 7) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #8: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 8) திரும்பு;

	எனில் ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} இல்லையெனில் {
		log("%sframe #9: ---\n", prefix);
		திரும்பு;
	}

	எனில் (levels <= 9) திரும்பு;
#endif
}
#இல்லையெனில்
வெறுமை log_backtrace(மாறிலி எழுத்து*, முழு) { }
#endif

வெறுமை LogManager::reset_அடுக்கு()
{
	போது (header_மொத்தம்.size() > 1)
		header_மொத்தம்.எடு_back();
	flush();
}

வெறுமை log_dump_val_worker(RTLIL::IdString v) {
	எனில் (v == IdString::Null)
		log("(null)");
	இல்லையெனில் எனில் (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	இல்லையெனில் எனில் (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	இல்லையெனில்
		log("%s", v.handle_token());
}

வெறுமை log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

வெறுமை log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(மாறிலி RTLIL::SigSpec &sig, தருக்க autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு buf.str();
}

std::string log_const(மாறிலி RTLIL::Const &மதிப்பு, தருக்க autoint)
{
	எனில் ((மதிப்பு.flags & RTLIL::CONST_FLAG_STRING) == 0)
		திரும்பு log_signal(மதிப்பு, autoint);

	திரும்பு "\"" + மதிப்பு.decode_string() + "\"";
}

வெறுமை log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, பொய், உண்மை, பொய், RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

வெறுமை log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

வெறுமை log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

வெறுமை LogManager::check_expected()
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

	auto check = [&](மாறிலி std::string kind, std::string pattern, LogExpectedItem item) {
		எனில் (item.current_மொத்தம் == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		எனில் (item.current_மொத்தம் != item.expected_மொத்தம்) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_மொத்தம், item.expected_மொத்தம்);
		}
	};

	க்கு (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	க்கு (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	க்கு (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	க்கு (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](மாறிலி std::string kind, std::string pattern, LogExpectedItem item) {
		எனில் (item.current_மொத்தம் == item.expected_மொத்தம்) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#எனில் defined(_MSC_VER)
				_exit(0);
			#இல்லையெனில்
				_Exit(0);
			#endif
		} இல்லையெனில் {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	க்கு (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	க்கு (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

வெறுமை LogManager::report_unexpected_error()
{
	எனில் (expect_no_warnings && warnings_மொத்தம்_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_மொத்தம், warnings_மொத்தம் - warnings_மொத்தம்_noexpect);
}

வெறுமை LogManager::add_expect(std::string type, std::string pattern, முழு மொத்தம்)
{
	எனில் (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), மொத்தம்);
	இல்லையெனில் எனில் (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), மொத்தம்);
	இல்லையெனில் எனில் (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), மொத்தம்);
	இல்லையெனில் எனில் (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), மொத்தம்);
	இல்லையெனில் எனில் (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), மொத்தம்);
	இல்லையெனில் எனில் (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), மொத்தம்);
	இல்லையெனில் log_abort();
}

வெறுமை LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	எனில் (!hasher)
		திரும்பு {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	திரும்பு hash;
}

YOSYS_NAMESPACE_END
