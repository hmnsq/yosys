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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/sha1/sha1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "backends/rtlil/rtlil_backend.h"

#ਜੇ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <list>

YOSYS_NAMESPACE_BEGIN

ਤਾਰਕਿਕ log_stderr_sink_forced = ਝੂਠ;

LogManager &logger()
{
	ਸਥਿਰ LogManager instance;
	ਵਾਪਸ instance;
}

std::chrono::steady_clock::ਸਮਾਂ_ਬਿੰਦੂ LogManager::get_initial_ਸਮਾਂ() ਅਚਲ
{
	ਸਥਿਰ ਅਚਲ ਸਵੈਚਾਲਿਤ initial_ਸਮਾਂ = std::chrono::steady_clock::now();
	ਵਾਪਸ initial_ਸਮਾਂ;
}

ਖਾਲੀ (*log_error_atexit)() = ਖਾਲੀ_ਸੂਚਕ;
ਖਾਲੀ (*log_verific_callback)(ਪੂਰਨ ਸੁਨੇਹਾ_type, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_id, Log_ਸਰੋਤ_Location src, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ) = ਖਾਲੀ_ਸੂਚਕ;

ਸਥਿਰ ਤਾਰਕਿਕ ਅਗਲਾ_print_log = ਸੱਚ;

FileLogSink::FileLogSink(ਅਚਲ std::string &filename, ਤਾਰਕਿਕ ਸਤਰ_buffered, ਤਾਰਕਿਕ append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	ਜੇ (!file)
		ਸੁੱਟੋ std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	ਜੇ (ਸਤਰ_buffered)
		setvbuf(file, ਖਾਲੀ_ਸੂਚਕ, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	ਜੇ (file)
		fclose(file);
}

ਖਾਲੀ FileLogSink::ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ)
{
	fputs(ਸੁਨੇਹਾ.cached_ਸੁਨੇਹਾ.c_str(), file);
}

ਖਾਲੀ FileLogSink::flush()
{
	fflush(file);
}

ਖਾਲੀ ConsoleLogSink::ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ)
{
	FILE *file = (ਸੁਨੇਹਾ.severity == LogSeverity::Error || ਸੁਨੇਹਾ.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(ਸੁਨੇਹਾ.cached_ਸੁਨੇਹਾ.c_str(), file);
}

ਖਾਲੀ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ਤਾਰਕਿਕ StderrLogSink::should_log(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) ਅਚਲ
{
	ਵਾਪਸ ਸੁਨੇਹਾ.severity == LogSeverity::Error || ਸੁਨੇਹਾ.severity == LogSeverity::NonFatalError ||
			(ਸੁਨੇਹਾ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

ਖਾਲੀ StderrLogSink::ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ)
{
	fputs(ਸੁਨੇਹਾ.cached_ਸੁਨੇਹਾ.c_str(), stderr);
}

ਖਾਲੀ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

ਖਾਲੀ ScratchPadLogSink::ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ)
{
	RTLIL::Design *design = yosys_get_design();
	ਜੇ (!design)
		ਵਾਪਸ;

	design->scratchpad[scratchpad].append(ਸੁਨੇਹਾ.cached_ਸੁਨੇਹਾ);
}

Log_ਸੁਨੇਹਾ::Log_ਸੁਨੇਹਾ(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string_view ਸੁਨੇਹਾ_2) :
	severity(severity),
	ਅਗੇਤਰ_2(ਅਗੇਤਰ_2),
	format(format),
	ਸੁਨੇਹਾ_2(ਸੁਨੇਹਾ_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string ਸਮਾਂ_str;
	ਜੇ (logger().get_log_ਸਮਾਂ())
	{
		ਜੇ (ਅਗਲਾ_print_log) {
			ਅਗਲਾ_print_log = ਝੂਠ;
			ਸਵੈਚਾਲਿਤ elapsed = std::chrono::steady_clock::now() - logger().get_initial_ਸਮਾਂ();
			ਸਵੈਚਾਲਿਤ us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			ਸਮਾਂ_str += stringf("[%05d.%06d] ", ਪੂਰਨ(us.ਗਿਣਤੀ_2() / 1'000'000),ਪੂਰਨ(us.ਗਿਣਤੀ_2() % 1'000'000));
		}

		ਜੇ (!format.ਖਾਲੀ_ਥਾਵਾਂ() && format.ਪਿੱਠ() == '\n')
			ਅਗਲਾ_print_log = ਸੱਚ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		ਜੇ (format == "%s" && !ਸੁਨੇਹਾ_2.ਖਾਲੀ_ਥਾਵਾਂ() && ਸੁਨੇਹਾ_2.ਪਿੱਠ() == '\n')
			ਅਗਲਾ_print_log = ਸੱਚ;
	}
	std::string loc = !src.filename.ਖਾਲੀ_ਥਾਵਾਂ() ? stringf("%s:%d: ", src.filename, src.ਸ਼ੁਰੂ_ਸਤਰ) : "";
	cached_ਸੁਨੇਹਾ = stringf("%s%s%s%s", ਸਮਾਂ_str, loc, ਅਗੇਤਰ_2, ਸੁਨੇਹਾ_2);
}

ਖਾਲੀ LogManager::logv_string(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string str_ਵਿੱਚ) {
	ਮਾਪ_t remove_leading = 0;
	ਜਦੋਂ (format.ਮਾਪ() > 1 && format[0] == '\n') {
		logv_string(severity, src, ਅਗੇਤਰ_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	ਜੇ (remove_leading > 0) {
		str_ਵਿੱਚ = str_ਵਿੱਚ.substr(remove_leading);
	}

	std::string loc = !src.filename.ਖਾਲੀ_ਥਾਵਾਂ() ? stringf("%s:%d: ", src.filename, src.ਸ਼ੁਰੂ_ਸਤਰ) : "";
	std::string str = stringf("%s%s%s",loc,ਅਗੇਤਰ_2,str_ਵਿੱਚ);

	ਜੇ (str.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ;

	ਮਾਪ_t nnl_pos = str.ਲੱਭੋ_ਆਖਰੀ_not_ਦਾ('\n');
	ਜੇ (nnl_pos == std::string::npos)
		newline_ਗਿਣਤੀ += Get_ਮਾਪ(str);
	ਨਹੀਂਤਾਂ
		newline_ਗਿਣਤੀ = Get_ਮਾਪ(str) - nnl_pos - 1;

	ਜੇ (hasher)
		hasher->update(str);

	ਸਵੈਚਾਲਿਤ ਸੁਨੇਹਾ = Log_ਸੁਨੇਹਾ(severity, src, ਅਗੇਤਰ_2, format, str_ਵਿੱਚ);
	ਲਈ (ਸਵੈਚਾਲਿਤ &sink : sinks) {
		ਜੇ (sink->should_log(ਸੁਨੇਹਾ))
			sink->ਲਘੂਗਣਕ(ਸੁਨੇਹਾ);
	}

	ਜੇ (severity == LogSeverity::ਸਿਰਲੇਖ)
		str = str_ਵਿੱਚ;

	ਸਥਿਰ std::string linebuffer;
	ਸਥਿਰ ਤਾਰਕਿਕ warn_regex_recusion_guard = ਝੂਠ;

	ਜੇ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = ਸੱਚ;

		ਜੇ (warn_regexes.ਖਾਲੀ_ਥਾਵਾਂ() && expect_log.ਖਾਲੀ_ਥਾਵਾਂ() && expect_ਅਗੇਤਰ_log.ਖਾਲੀ_ਥਾਵਾਂ())
		{
			linebuffer.clear();
		}
		ਨਹੀਂਤਾਂ
		{
			linebuffer += str;

			ਜੇ (!linebuffer.ਖਾਲੀ_ਥਾਵਾਂ() && linebuffer.ਪਿੱਠ() == '\n') {
				ਲਈ (ਸਵੈਚਾਲਿਤ &re : warn_regexes)
					ਜੇ (std::regex_ਖੋਜ(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				ਲਈ (ਸਵੈਚਾਲਿਤ &[_, ਵਸਤੂ_2] : expect_log)
					ਜੇ (std::regex_ਖੋਜ(linebuffer, ਵਸਤੂ_2.ਨਮੂਨਾ))
						ਵਸਤੂ_2.current_ਗਿਣਤੀ++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = ਝੂਠ;
	}
}

ਖਾਲੀ LogManager::formatted_string(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::ਸਰਗਰਮ());

	ਜੇ (make_debug && !is_debug(1))
		ਵਾਪਸ;
	logv_string(severity, src, ਅਗੇਤਰ_2, format, std::move(str));
}

ਖਾਲੀ LogManager::formatted_ਸਿਰਲੇਖ(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::ਸਰਗਰਮ());

	spacer();
	ਜੇ (ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਮਾਪ() > 0)
		ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਪਿੱਠ()++;

	ਜੇ (ਪੂਰਨ(ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਮਾਪ()) <= verbose_level) {
		log_stderr_sink_forced = ਸੱਚ;
	}

	std::string ਸਿਰਲੇਖ_id;

	ਲਈ (ਪੂਰਨ c : ਸਿਰਲੇਖ_ਗਿਣਤੀ)
		ਸਿਰਲੇਖ_id += stringf("%s%d", ਸਿਰਲੇਖ_id.ਖਾਲੀ_ਥਾਵਾਂ() ? "" : ".", c);

	formatted_string(LogSeverity::ਸਿਰਲੇਖ, Log_ਸਰੋਤ_Location{}, stringf("%s. ", ਸਿਰਲੇਖ_id), format, std::move(str));
	flush();

	ਜੇ (hdump_all)
		hdump[ਸਿਰਲੇਖ_id].ਪਾਓ("yosys_dump_" + ਸਿਰਲੇਖ_id + ".il");

	ਜੇ (hdump.ਗਿਣਤੀ_2(ਸਿਰਲੇਖ_id) && design != ਖਾਲੀ_ਸੂਚਕ)
		ਲਈ (ਸਵੈਚਾਲਿਤ &filename : hdump.at(ਸਿਰਲੇਖ_id)) {
			ਲਘੂਗਣਕ("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = ਝੂਠ;
}

ਖਾਲੀ LogManager::formatted_warning(Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2)
{
	log_assert(!Multithreading::ਸਰਗਰਮ());

	ਤਾਰਕਿਕ suppressed = ਝੂਠ;

	ਲਈ (ਸਵੈਚਾਲਿਤ &re : nowarn_regexes)
		ਜੇ (std::regex_ਖੋਜ(ਸੁਨੇਹਾ_2, re))
			suppressed = ਸੱਚ;

	std::string loc = !src.filename.ਖਾਲੀ_ਥਾਵਾਂ() ? stringf("%s:%d: ", src.filename, src.ਸ਼ੁਰੂ_ਸਤਰ) : "";
	ਜੇ (suppressed)
	{
		ਲਘੂਗਣਕ("Suppressed %s%s%s", loc, ਅਗੇਤਰ_2, ਸੁਨੇਹਾ_2);
	}
	ਨਹੀਂਤਾਂ
	{
		ਪੂਰਨ bak_make_debug = make_debug;
		make_debug = 0;

		ਲਈ (ਸਵੈਚਾਲਿਤ &re : werror_regexes)
			ਜੇ (std::regex_ਖੋਜ(ਸੁਨੇਹਾ_2, re))
				formatted_error(src, "ERROR: ", format, ਸੁਨੇਹਾ_2);

		ਤਾਰਕਿਕ warning_ਮੇਲ = ਝੂਠ;
		ਲਈ (ਸਵੈਚਾਲਿਤ &[_, ਵਸਤੂ_2] : expect_warning)
			ਜੇ (std::regex_ਖੋਜ(ਸੁਨੇਹਾ_2, ਵਸਤੂ_2.ਨਮੂਨਾ)) {
				ਵਸਤੂ_2.current_ਗਿਣਤੀ++;
				warning_ਮੇਲ = ਸੱਚ;
			}

		std::string ਨਮੂਨਾ = loc + string(ਅਗੇਤਰ_2) + ਸੁਨੇਹਾ_2;
		ਲਈ (ਸਵੈਚਾਲਿਤ &[_, ਵਸਤੂ_2] : expect_ਅਗੇਤਰ_warning)
			ਜੇ (std::regex_ਖੋਜ(ਨਮੂਨਾ, ਵਸਤੂ_2.ਨਮੂਨਾ)) {
				ਵਸਤੂ_2.current_ਗਿਣਤੀ++;
				warning_ਮੇਲ = ਸੱਚ;
			}

		ਜੇ (warnings.ਗਿਣਤੀ_2(ਸੁਨੇਹਾ_2))
		{
			formatted_string(LogSeverity::Info, src, ਅਗੇਤਰ_2, format, ਸੁਨੇਹਾ_2);
			flush();
		}
		ਨਹੀਂਤਾਂ
		{
			formatted_string(LogSeverity::Warning, src, ਅਗੇਤਰ_2, format, ਸੁਨੇਹਾ_2);
			flush();
			warnings.ਪਾਓ(ਸੁਨੇਹਾ_2);
		}

		ਜੇ (!warning_ਮੇਲ)
			warnings_ਗਿਣਤੀ_noexpect++;
		warnings_ਗਿਣਤੀ++;
		make_debug = bak_make_debug;
	}
}

ਖਾਲੀ LogManager::suppressed() {
	ਜੇ (debug_suppressed && !make_debug) {
		constexpr ਅਚਲ ਅੱਖਰ* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_ਸਰੋਤ_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

ਖਾਲੀ LogManager::error_with_ਅਗੇਤਰ(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2)
{
	ਪੂਰਨ bak_make_debug = make_debug;
	make_debug = 0;
	ਜੇ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, ਅਗੇਤਰ_2, format, ਸੁਨੇਹਾ_2);
	flush();

	make_debug = bak_make_debug;

	ਲਈ (ਸਵੈਚਾਲਿਤ &[_, ਵਸਤੂ_2] : expect_error)
		ਜੇ (std::regex_ਖੋਜ(ਸੁਨੇਹਾ_2, ਵਸਤੂ_2.ਨਮੂਨਾ))
			ਵਸਤੂ_2.current_ਗਿਣਤੀ++;

	std::string loc = !src.filename.ਖਾਲੀ_ਥਾਵਾਂ() ? stringf("%s:%d: ", src.filename, src.ਸ਼ੁਰੂ_ਸਤਰ) : "";
	std::string ਨਮੂਨਾ = loc + string(ਅਗੇਤਰ_2) + ਸੁਨੇਹਾ_2;
	ਲਈ (ਸਵੈਚਾਲਿਤ &[_, ਵਸਤੂ_2] : expect_ਅਗੇਤਰ_error)
		ਜੇ (std::regex_ਖੋਜ(ਨਮੂਨਾ, ਵਸਤੂ_2.ਨਮੂਨਾ))
			ਵਸਤੂ_2.current_ਗਿਣਤੀ++;

	errors_ਗਿਣਤੀ++;
}

[[noreturn]]
ਖਾਲੀ LogManager::formatted_error(Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2)
{
	error_with_ਅਗੇਤਰ(LogSeverity::Error, src, ਅਗੇਤਰ_2, format, ਸੁਨੇਹਾ_2);

	check_expected();

	ਜੇ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ਅਚਲ ਅੱਖਰ *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	ਜੇ (e && atoi(e))
		abort();

#ਜੇ defined(_MSC_VER)
	_exit(1);
#ਨਹੀਂਤਾਂ
	_Exit(1);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ LogManager::formatted_nonfatal_error(Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2)
{
	error_with_ਅਗੇਤਰ(LogSeverity::NonFatalError, src, ਅਗੇਤਰ_2, format, ਸੁਨੇਹਾ_2);
}

ਖਾਲੀ LogManager::ਜੋੜੋ_experimental(ਅਚਲ std::string &str)
{
	ਜੇ (experimental_ignored.ਗਿਣਤੀ_2(str) == 0 && experimental.ਗਿਣਤੀ_2(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ਪਾਓ(str);
	}
}

ਖਾਲੀ LogManager::ਜੋੜੋ_deprecated(ਅਚਲ std::string &feature, ਅਚਲ std::string &earliest_removal)
{
	ਜੇ (deprecated.ਗਿਣਤੀ_2(feature) == 0) {
		ਜੇ (earliest_removal.ਖਾਲੀ_ਥਾਵਾਂ())
			log_warning("Feature '%s' is deprecated.\n", feature);
		ਨਹੀਂਤਾਂ
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

ਖਾਲੀ log_assert_failure(ਅਚਲ ਅੱਖਰ *ਵਿਅੰਜਕ, ਸਰੋਤ_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", ਵਿਅੰਜਕ, location.file_name(), location.ਸਤਰ());
}

ਖਾਲੀ log_abort_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ)
{
	log_error("Abort in %s:%d.\n", file, ਸਤਰ);
}

ਖਾਲੀ log_yosys_abort_ਸੁਨੇਹਾ(std::string_view file, ਪੂਰਨ ਸਤਰ, std::string_view func, std::string_view ਸੁਨੇਹਾ_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, ਸਤਰ, func, ਸੁਨੇਹਾ_2);
}

ਖਾਲੀ LogManager::formatted_cmd_error(std::string_view format, std::string ਸੁਨੇਹਾ_2)
{
	ਜੇ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_ਸਰੋਤ_Location{}, "ERROR: ", format, ਸੁਨੇਹਾ_2);
		flush();

		ਸੁੱਟੋ log_cmd_error_exception();
	}

	formatted_error(Log_ਸਰੋਤ_Location{}, "ERROR: ", format, ਸੁਨੇਹਾ_2);
}

ਖਾਲੀ LogManager::spacer()
{
	ਜੇ (newline_ਗਿਣਤੀ < 2) ਲਘੂਗਣਕ("\n");
	ਜੇ (newline_ਗਿਣਤੀ < 2) ਲਘੂਗਣਕ("\n");
}

ਖਾਲੀ LogManager::clear_original()
{
	std::erase_if(sinks, [](ਅਚਲ ਸਵੈਚਾਲਿਤ &sink) { ਵਾਪਸ dynamic_cast<LogSinkRef *>(sink.get()) != ਖਾਲੀ_ਸੂਚਕ; });
}

ਖਾਲੀ LogManager::ਧੱਕੋ()
{
	ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਧੱਕੋ_ਪਿੱਠ(0);
}

ਖਾਲੀ LogManager::ਕੱਢੋ()
{
	ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਕੱਢੋ_ਪਿੱਠ();
	flush();
}

#ਜੇ defined(YOSYS_ENABLE_DLOPEN)
ਖਾਲੀ log_backtrace(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ_2, ਪੂਰਨ ਪੱਧਰ)
{
	ਜੇ (ਪੱਧਰ <= 0) ਵਾਪਸ;

	Dl_info dli;
	ਖਾਲੀ *p;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #1: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #1: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 1) ਵਾਪਸ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUG
	ਲਘੂਗਣਕ("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", ਅਗੇਤਰ_2);
#ਨਹੀਂਤਾਂ
	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #2: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #2: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 2) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #3: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #3: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 3) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #4: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #4: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 4) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #5: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #5: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 5) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #6: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #6: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 6) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #7: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #7: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 7) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #8: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #8: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 8) ਵਾਪਸ;

	ਜੇ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		ਲਘੂਗਣਕ("%sframe #9: %p %s(%p) %s(%p)\n", ਅਗੇਤਰ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("%sframe #9: ---\n", ਅਗੇਤਰ_2);
		ਵਾਪਸ;
	}

	ਜੇ (ਪੱਧਰ <= 9) ਵਾਪਸ;
#ਜੇ_ਅੰਤ
}
#ਨਹੀਂਤਾਂ
ਖਾਲੀ log_backtrace(ਅਚਲ ਅੱਖਰ*, ਪੂਰਨ) { }
#ਜੇ_ਅੰਤ

ਖਾਲੀ LogManager::reset_ਢੇਰ()
{
	ਜਦੋਂ (ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਮਾਪ() > 1)
		ਸਿਰਲੇਖ_ਗਿਣਤੀ.ਕੱਢੋ_ਪਿੱਠ();
	flush();
}

ਖਾਲੀ log_dump_val_ਕਾਮਾ(RTLIL::IdString v) {
	ਜੇ (v == IdString::Null)
		ਲਘੂਗਣਕ("(null)");
	ਨਹੀਂਤਾਂ ਜੇ (ID::is_static(v))
		ਲਘੂਗਣਕ("%s", ID::unescaped_str(v));
	ਨਹੀਂਤਾਂ ਜੇ (yosys_design && yosys_design->twines().is_ਜੀਵਤ(v))
		ਲਘੂਗਣਕ("%s", yosys_design->twines().unescaped_str(v));
	ਨਹੀਂਤਾਂ
		ਲਘੂਗਣਕ("%s", v.handle_token());
}

ਖਾਲੀ log_dump_val_ਕਾਮਾ(RTLIL::SigSpec v) {
	ਲਘੂਗਣਕ("%s", log_signal(v));
}

ਖਾਲੀ log_dump_val_ਕਾਮਾ(RTLIL::State v) {
	ਲਘੂਗਣਕ("%s", log_signal(v));
}

std::string log_signal(ਅਚਲ RTLIL::SigSpec &sig, ਤਾਰਕਿਕ autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ buf.str();
}

std::string log_const(ਅਚਲ RTLIL::Const &ਮੁੱਲ, ਤਾਰਕਿਕ autoint)
{
	ਜੇ ((ਮੁੱਲ.flags & RTLIL::CONST_FLAG_STRING) == 0)
		ਵਾਪਸ log_signal(ਮੁੱਲ, autoint);

	ਵਾਪਸ "\"" + ਮੁੱਲ.decode_string() + "\"";
}

ਖਾਲੀ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, ਝੂਠ, ਸੱਚ, ਝੂਠ, RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਲਘੂਗਣਕ("%s", buf.str());
}

ਖਾਲੀ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : ਖਾਲੀ_ਸੂਚਕ, RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਲਘੂਗਣਕ("%s", buf.str());
}

ਖਾਲੀ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : ਖਾਲੀ_ਸੂਚਕ, RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਲਘੂਗਣਕ("%s", buf.str());
}

ਖਾਲੀ LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_ਵਸਤੂ> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_ਵਸਤੂ> expect_ਅਗੇਤਰ_log_copy, expect_ਅਗੇਤਰ_warning_copy, expect_ਅਗੇਤਰ_error_copy;
	std::ਅਦਲਾ_ਬਦਲੀ(expect_warning_copy, expect_warning);
	std::ਅਦਲਾ_ਬਦਲੀ(expect_log_copy, expect_log);
	std::ਅਦਲਾ_ਬਦਲੀ(expect_error_copy, expect_error);
	std::ਅਦਲਾ_ਬਦਲੀ(expect_ਅਗੇਤਰ_warning_copy, expect_ਅਗੇਤਰ_warning);
	std::ਅਦਲਾ_ਬਦਲੀ(expect_ਅਗੇਤਰ_log_copy, expect_ਅਗੇਤਰ_log);
	std::ਅਦਲਾ_ਬਦਲੀ(expect_ਅਗੇਤਰ_error_copy, expect_ਅਗੇਤਰ_error);

	ਸਵੈਚਾਲਿਤ check = [&](ਅਚਲ std::string ਕਿਸਮ, std::string ਨਮੂਨਾ, Log_Expected_ਵਸਤੂ ਵਸਤੂ_2) {
		ਜੇ (ਵਸਤੂ_2.current_ਗਿਣਤੀ == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", ਕਿਸਮ, ਨਮੂਨਾ);
		}
		ਜੇ (ਵਸਤੂ_2.current_ਗਿਣਤੀ != ਵਸਤੂ_2.expected_ਗਿਣਤੀ) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				ਕਿਸਮ.c_str(), ਨਮੂਨਾ.c_str(), ਵਸਤੂ_2.current_ਗਿਣਤੀ, ਵਸਤੂ_2.expected_ਗਿਣਤੀ);
		}
	};

	ਲਈ (ਸਵੈਚਾਲਿਤ &[ਨਮੂਨਾ, ਵਸਤੂ_2] : expect_warning_copy)
		check("warning", ਨਮੂਨਾ, ਵਸਤੂ_2);
	ਲਈ (ਸਵੈਚਾਲਿਤ &[ਨਮੂਨਾ, ਵਸਤੂ_2] : expect_ਅਗੇਤਰ_warning_copy)
		check("prefixed warning", ਨਮੂਨਾ, ਵਸਤੂ_2);
	ਲਈ (ਸਵੈਚਾਲਿਤ &[ਨਮੂਨਾ, ਵਸਤੂ_2] : expect_log_copy)
		check("log", ਨਮੂਨਾ, ਵਸਤੂ_2);
	ਲਈ (ਸਵੈਚਾਲਿਤ &[ਨਮੂਨਾ, ਵਸਤੂ_2] : expect_ਅਗੇਤਰ_log_copy)
		check("prefixed log", ਨਮੂਨਾ, ਵਸਤੂ_2);

	ਸਵੈਚਾਲਿਤ check_err = [&](ਅਚਲ std::string ਕਿਸਮ, std::string ਨਮੂਨਾ, Log_Expected_ਵਸਤੂ ਵਸਤੂ_2) {
		ਜੇ (ਵਸਤੂ_2.current_ਗਿਣਤੀ == ਵਸਤੂ_2.expected_ਗਿਣਤੀ) {
			warn_regexes.clear();
			ਲਘੂਗਣਕ("Expected %s pattern '%s' found !!!\n", ਕਿਸਮ, ਨਮੂਨਾ);
			yosys_shutdown();
			#ਜੇ defined(_MSC_VER)
				_exit(0);
			#ਨਹੀਂਤਾਂ
				_Exit(0);
			#ਜੇ_ਅੰਤ
		} ਨਹੀਂਤਾਂ {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", ਕਿਸਮ, ਨਮੂਨਾ);
		}
	};
	ਲਈ (ਸਵੈਚਾਲਿਤ &[ਨਮੂਨਾ, ਵਸਤੂ_2] : expect_error_copy)
		check_err("error", ਨਮੂਨਾ, ਵਸਤੂ_2);
	ਲਈ (ਸਵੈਚਾਲਿਤ &[ਨਮੂਨਾ, ਵਸਤੂ_2] : expect_ਅਗੇਤਰ_error_copy)
		check_err("prefixed error", ਨਮੂਨਾ, ਵਸਤੂ_2);
}

ਖਾਲੀ LogManager::report_unexpected_error()
{
	ਜੇ (expect_no_warnings && warnings_ਗਿਣਤੀ_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_ਮਾਪ(warnings),
					warnings_ਗਿਣਤੀ, warnings_ਗਿਣਤੀ - warnings_ਗਿਣਤੀ_noexpect);
}

ਖਾਲੀ LogManager::ਜੋੜੋ_expect(std::string type, std::string ਨਮੂਨਾ, ਪੂਰਨ ਗਿਣਤੀ_2)
{
	ਜੇ (type == "error")
		expect_error[ਨਮੂਨਾ] = Log_Expected_ਵਸਤੂ(YS_REGEX_COMPILE(ਨਮੂਨਾ), ਗਿਣਤੀ_2);
	ਨਹੀਂਤਾਂ ਜੇ (type == "prefix-error")
		expect_ਅਗੇਤਰ_error[ਨਮੂਨਾ] = Log_Expected_ਵਸਤੂ(YS_REGEX_COMPILE(ਨਮੂਨਾ), ਗਿਣਤੀ_2);
	ਨਹੀਂਤਾਂ ਜੇ (type == "warning")
		expect_warning[ਨਮੂਨਾ] = Log_Expected_ਵਸਤੂ(YS_REGEX_COMPILE(ਨਮੂਨਾ), ਗਿਣਤੀ_2);
	ਨਹੀਂਤਾਂ ਜੇ (type == "prefix-warning")
		expect_ਅਗੇਤਰ_warning[ਨਮੂਨਾ] = Log_Expected_ਵਸਤੂ(YS_REGEX_COMPILE(ਨਮੂਨਾ), ਗਿਣਤੀ_2);
	ਨਹੀਂਤਾਂ ਜੇ (type == "log")
		expect_log[ਨਮੂਨਾ] = Log_Expected_ਵਸਤੂ(YS_REGEX_COMPILE(ਨਮੂਨਾ), ਗਿਣਤੀ_2);
	ਨਹੀਂਤਾਂ ਜੇ (type == "prefix-log")
		expect_ਅਗੇਤਰ_log[ਨਮੂਨਾ] = Log_Expected_ਵਸਤੂ(YS_REGEX_COMPILE(ਨਮੂਨਾ), ਗਿਣਤੀ_2);
	ਨਹੀਂਤਾਂ log_abort();
}

ਖਾਲੀ LogManager::ਸ਼ੁਰੂ_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::ਅੰਤ_hasher()
{
	ਜੇ (!hasher)
		ਵਾਪਸ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	ਵਾਪਸ hash;
}

YOSYS_NAMESPACE_ਸਿਰਾ
