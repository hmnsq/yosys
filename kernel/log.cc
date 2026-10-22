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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/sha1/sha1.h"
#ಅನ್ತರ್ಭಾವ "backends/rtlil/rtlil_backend.h"

#ಆದರೆ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <list>

YOSYS_NAMESPACE_BEGIN

ತಾರ್ಕಿಕ log_stderr_sink_forced = ಸುಳ್ಳು;

LogManager &logger()
{
	ಸ್ಥಿರ LogManager instance;
	ಹಿಂತಿರುಗಿ instance;
}

std::chrono::steady_clock::ಸಮಯ_ಬಿಂದು LogManager::get_initial_ಸಮಯ() ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ initial_ಸಮಯ = std::chrono::steady_clock::now();
	ಹಿಂತಿರುಗಿ initial_ಸಮಯ;
}

ಶೂನ್ಯ (*log_error_atexit)() = ಶೂನ್ಯಸೂಚಕ;
ಶೂನ್ಯ (*log_verific_callback)(ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_id, Log_ಮೂಲ_Location src, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ) = ಶೂನ್ಯಸೂಚಕ;

ಸ್ಥಿರ ತಾರ್ಕಿಕ ಮುಂದಿನ_print_log = ನಿಜ;

FileLogSink::FileLogSink(ಸ್ಥಿರಾಂಕ std::string &filename, ತಾರ್ಕಿಕ ಸಾಲು_buffered, ತಾರ್ಕಿಕ append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	ಆದರೆ (!file)
		ಎಸೆ std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	ಆದರೆ (ಸಾಲು_buffered)
		setvbuf(file, ಶೂನ್ಯಸೂಚಕ, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	ಆದರೆ (file)
		fclose(file);
}

ಶೂನ್ಯ FileLogSink::ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ)
{
	fputs(ಸಂದೇಶ.cached_ಸಂದೇಶ.c_str(), file);
}

ಶೂನ್ಯ FileLogSink::flush()
{
	fflush(file);
}

ಶೂನ್ಯ ConsoleLogSink::ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ)
{
	FILE *file = (ಸಂದೇಶ.severity == LogSeverity::Error || ಸಂದೇಶ.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(ಸಂದೇಶ.cached_ಸಂದೇಶ.c_str(), file);
}

ಶೂನ್ಯ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ತಾರ್ಕಿಕ StderrLogSink::should_log(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ.severity == LogSeverity::Error || ಸಂದೇಶ.severity == LogSeverity::NonFatalError ||
			(ಸಂದೇಶ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

ಶೂನ್ಯ StderrLogSink::ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ)
{
	fputs(ಸಂದೇಶ.cached_ಸಂದೇಶ.c_str(), stderr);
}

ಶೂನ್ಯ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

ಶೂನ್ಯ ScratchPadLogSink::ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ)
{
	RTLIL::Design *design = yosys_get_design();
	ಆದರೆ (!design)
		ಹಿಂತಿರುಗಿ;

	design->scratchpad[scratchpad].append(ಸಂದೇಶ.cached_ಸಂದೇಶ);
}

Log_ಸಂದೇಶ::Log_ಸಂದೇಶ(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string_view ಸಂದೇಶ_2) :
	severity(severity),
	ಪೂರ್ವಪ್ರತ್ಯಯ_2(ಪೂರ್ವಪ್ರತ್ಯಯ_2),
	format(format),
	ಸಂದೇಶ_2(ಸಂದೇಶ_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string ಸಮಯ_str;
	ಆದರೆ (logger().get_log_ಸಮಯ())
	{
		ಆದರೆ (ಮುಂದಿನ_print_log) {
			ಮುಂದಿನ_print_log = ಸುಳ್ಳು;
			ಸ್ವಯಂಚಾಲಿತ elapsed = std::chrono::steady_clock::now() - logger().get_initial_ಸಮಯ();
			ಸ್ವಯಂಚಾಲಿತ us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			ಸಮಯ_str += stringf("[%05d.%06d] ", ಪೂರ್ಣಾಂಕ(us.ಎಣಿಕೆ_2() / 1'000'000),ಪೂರ್ಣಾಂಕ(us.ಎಣಿಕೆ_2() % 1'000'000));
		}

		ಆದರೆ (!format.ಖಾಲಿಗಳು() && format.ಹಿಂಭಾಗ() == '\n')
			ಮುಂದಿನ_print_log = ನಿಜ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		ಆದರೆ (format == "%s" && !ಸಂದೇಶ_2.ಖಾಲಿಗಳು() && ಸಂದೇಶ_2.ಹಿಂಭಾಗ() == '\n')
			ಮುಂದಿನ_print_log = ನಿಜ;
	}
	std::string loc = !src.filename.ಖಾಲಿಗಳು() ? stringf("%s:%d: ", src.filename, src.ಆರಂಭ_ಸಾಲು) : "";
	cached_ಸಂದೇಶ = stringf("%s%s%s%s", ಸಮಯ_str, loc, ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಸಂದೇಶ_2);
}

ಶೂನ್ಯ LogManager::logv_string(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string str_ಒಳ) {
	ಗಾತ್ರ_t remove_leading = 0;
	ಆಗ (format.ಗಾತ್ರ_2() > 1 && format[0] == '\n') {
		logv_string(severity, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	ಆದರೆ (remove_leading > 0) {
		str_ಒಳ = str_ಒಳ.substr(remove_leading);
	}

	std::string loc = !src.filename.ಖಾಲಿಗಳು() ? stringf("%s:%d: ", src.filename, src.ಆರಂಭ_ಸಾಲು) : "";
	std::string str = stringf("%s%s%s",loc,ಪೂರ್ವಪ್ರತ್ಯಯ_2,str_ಒಳ);

	ಆದರೆ (str.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ;

	ಗಾತ್ರ_t nnl_pos = str.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_not_ದ('\n');
	ಆದರೆ (nnl_pos == std::string::npos)
		newline_ಎಣಿಕೆ += Get_ಗಾತ್ರ(str);
	ಇಲ್ಲದಿದ್ದರೆ
		newline_ಎಣಿಕೆ = Get_ಗಾತ್ರ(str) - nnl_pos - 1;

	ಆದರೆ (hasher)
		hasher->update(str);

	ಸ್ವಯಂಚಾಲಿತ ಸಂದೇಶ = Log_ಸಂದೇಶ(severity, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, str_ಒಳ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sink : sinks) {
		ಆದರೆ (sink->should_log(ಸಂದೇಶ))
			sink->ಲಘುಗಣಕ(ಸಂದೇಶ);
	}

	ಆದರೆ (severity == LogSeverity::ಶೀರ್ಷಿಕೆ)
		str = str_ಒಳ;

	ಸ್ಥಿರ std::string linebuffer;
	ಸ್ಥಿರ ತಾರ್ಕಿಕ warn_regex_recusion_guard = ಸುಳ್ಳು;

	ಆದರೆ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = ನಿಜ;

		ಆದರೆ (warn_regexes.ಖಾಲಿಗಳು() && expect_log.ಖಾಲಿಗಳು() && expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log.ಖಾಲಿಗಳು())
		{
			linebuffer.clear();
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			linebuffer += str;

			ಆದರೆ (!linebuffer.ಖಾಲಿಗಳು() && linebuffer.ಹಿಂಭಾಗ() == '\n') {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &re : warn_regexes)
					ಆದರೆ (std::regex_ಹುಡುಕು(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, ಅಂಶ] : expect_log)
					ಆದರೆ (std::regex_ಹುಡುಕು(linebuffer, ಅಂಶ.ನಮೂನೆ))
						ಅಂಶ.current_ಎಣಿಕೆ++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = ಸುಳ್ಳು;
	}
}

ಶೂನ್ಯ LogManager::formatted_string(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::ಸಕ್ರಿಯ());

	ಆದರೆ (make_debug && !is_debug(1))
		ಹಿಂತಿರುಗಿ;
	logv_string(severity, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, std::move(str));
}

ಶೂನ್ಯ LogManager::formatted_ಶೀರ್ಷಿಕೆ(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::ಸಕ್ರಿಯ());

	spacer();
	ಆದರೆ (ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ಗಾತ್ರ_2() > 0)
		ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ಹಿಂಭಾಗ()++;

	ಆದರೆ (ಪೂರ್ಣಾಂಕ(ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ಗಾತ್ರ_2()) <= verbose_level) {
		log_stderr_sink_forced = ನಿಜ;
	}

	std::string ಶೀರ್ಷಿಕೆ_id;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c : ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ)
		ಶೀರ್ಷಿಕೆ_id += stringf("%s%d", ಶೀರ್ಷಿಕೆ_id.ಖಾಲಿಗಳು() ? "" : ".", c);

	formatted_string(LogSeverity::ಶೀರ್ಷಿಕೆ, Log_ಮೂಲ_Location{}, stringf("%s. ", ಶೀರ್ಷಿಕೆ_id), format, std::move(str));
	flush();

	ಆದರೆ (hdump_all)
		hdump[ಶೀರ್ಷಿಕೆ_id].ಸೇರಿಸು("yosys_dump_" + ಶೀರ್ಷಿಕೆ_id + ".il");

	ಆದರೆ (hdump.ಎಣಿಕೆ_2(ಶೀರ್ಷಿಕೆ_id) && design != ಶೂನ್ಯಸೂಚಕ)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &filename : hdump.at(ಶೀರ್ಷಿಕೆ_id)) {
			ಲಘುಗಣಕ("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = ಸುಳ್ಳು;
}

ಶೂನ್ಯ LogManager::formatted_warning(Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2)
{
	log_assert(!Multithreading::ಸಕ್ರಿಯ());

	ತಾರ್ಕಿಕ suppressed = ಸುಳ್ಳು;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &re : nowarn_regexes)
		ಆದರೆ (std::regex_ಹುಡುಕು(ಸಂದೇಶ_2, re))
			suppressed = ನಿಜ;

	std::string loc = !src.filename.ಖಾಲಿಗಳು() ? stringf("%s:%d: ", src.filename, src.ಆರಂಭ_ಸಾಲು) : "";
	ಆದರೆ (suppressed)
	{
		ಲಘುಗಣಕ("Suppressed %s%s%s", loc, ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಸಂದೇಶ_2);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		ಪೂರ್ಣಾಂಕ bak_make_debug = make_debug;
		make_debug = 0;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &re : werror_regexes)
			ಆದರೆ (std::regex_ಹುಡುಕು(ಸಂದೇಶ_2, re))
				formatted_error(src, "ERROR: ", format, ಸಂದೇಶ_2);

		ತಾರ್ಕಿಕ warning_ಹೊಂದಿಕೆ = ಸುಳ್ಳು;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, ಅಂಶ] : expect_warning)
			ಆದರೆ (std::regex_ಹುಡುಕು(ಸಂದೇಶ_2, ಅಂಶ.ನಮೂನೆ)) {
				ಅಂಶ.current_ಎಣಿಕೆ++;
				warning_ಹೊಂದಿಕೆ = ನಿಜ;
			}

		std::string ನಮೂನೆ = loc + string(ಪೂರ್ವಪ್ರತ್ಯಯ_2) + ಸಂದೇಶ_2;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, ಅಂಶ] : expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning)
			ಆದರೆ (std::regex_ಹುಡುಕು(ನಮೂನೆ, ಅಂಶ.ನಮೂನೆ)) {
				ಅಂಶ.current_ಎಣಿಕೆ++;
				warning_ಹೊಂದಿಕೆ = ನಿಜ;
			}

		ಆದರೆ (warnings.ಎಣಿಕೆ_2(ಸಂದೇಶ_2))
		{
			formatted_string(LogSeverity::Info, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, ಸಂದೇಶ_2);
			flush();
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			formatted_string(LogSeverity::Warning, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, ಸಂದೇಶ_2);
			flush();
			warnings.ಸೇರಿಸು(ಸಂದೇಶ_2);
		}

		ಆದರೆ (!warning_ಹೊಂದಿಕೆ)
			warnings_ಎಣಿಕೆ_noexpect++;
		warnings_ಎಣಿಕೆ++;
		make_debug = bak_make_debug;
	}
}

ಶೂನ್ಯ LogManager::suppressed() {
	ಆದರೆ (debug_suppressed && !make_debug) {
		constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_ಮೂಲ_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

ಶೂನ್ಯ LogManager::error_with_ಪೂರ್ವಪ್ರತ್ಯಯ(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2)
{
	ಪೂರ್ಣಾಂಕ bak_make_debug = make_debug;
	make_debug = 0;
	ಆದರೆ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, ಸಂದೇಶ_2);
	flush();

	make_debug = bak_make_debug;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, ಅಂಶ] : expect_error)
		ಆದರೆ (std::regex_ಹುಡುಕು(ಸಂದೇಶ_2, ಅಂಶ.ನಮೂನೆ))
			ಅಂಶ.current_ಎಣಿಕೆ++;

	std::string loc = !src.filename.ಖಾಲಿಗಳು() ? stringf("%s:%d: ", src.filename, src.ಆರಂಭ_ಸಾಲು) : "";
	std::string ನಮೂನೆ = loc + string(ಪೂರ್ವಪ್ರತ್ಯಯ_2) + ಸಂದೇಶ_2;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, ಅಂಶ] : expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error)
		ಆದರೆ (std::regex_ಹುಡುಕು(ನಮೂನೆ, ಅಂಶ.ನಮೂನೆ))
			ಅಂಶ.current_ಎಣಿಕೆ++;

	errors_ಎಣಿಕೆ++;
}

[[noreturn]]
ಶೂನ್ಯ LogManager::formatted_error(Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2)
{
	error_with_ಪೂರ್ವಪ್ರತ್ಯಯ(LogSeverity::Error, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, ಸಂದೇಶ_2);

	check_expected();

	ಆದರೆ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	ಆದರೆ (e && atoi(e))
		abort();

#ಆದರೆ defined(_MSC_VER)
	_exit(1);
#ಇಲ್ಲದಿದ್ದರೆ
	_Exit(1);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ LogManager::formatted_nonfatal_error(Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2)
{
	error_with_ಪೂರ್ವಪ್ರತ್ಯಯ(LogSeverity::NonFatalError, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, format, ಸಂದೇಶ_2);
}

ಶೂನ್ಯ LogManager::ಕೂಡಿಸು_experimental(ಸ್ಥಿರಾಂಕ std::string &str)
{
	ಆದರೆ (experimental_ignored.ಎಣಿಕೆ_2(str) == 0 && experimental.ಎಣಿಕೆ_2(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ಸೇರಿಸು(str);
	}
}

ಶೂನ್ಯ LogManager::ಕೂಡಿಸು_deprecated(ಸ್ಥಿರಾಂಕ std::string &feature, ಸ್ಥಿರಾಂಕ std::string &earliest_removal)
{
	ಆದರೆ (deprecated.ಎಣಿಕೆ_2(feature) == 0) {
		ಆದರೆ (earliest_removal.ಖಾಲಿಗಳು())
			log_warning("Feature '%s' is deprecated.\n", feature);
		ಇಲ್ಲದಿದ್ದರೆ
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

ಶೂನ್ಯ log_assert_failure(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಭಿವ್ಯಕ್ತಿ, ಮೂಲ_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", ಅಭಿವ್ಯಕ್ತಿ, location.file_name(), location.ಸಾಲು());
}

ಶೂನ್ಯ log_abort_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು)
{
	log_error("Abort in %s:%d.\n", file, ಸಾಲು);
}

ಶೂನ್ಯ log_yosys_abort_ಸಂದೇಶ(std::string_view file, ಪೂರ್ಣಾಂಕ ಸಾಲು, std::string_view func, std::string_view ಸಂದೇಶ_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, ಸಾಲು, func, ಸಂದೇಶ_2);
}

ಶೂನ್ಯ LogManager::formatted_cmd_error(std::string_view format, std::string ಸಂದೇಶ_2)
{
	ಆದರೆ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_ಮೂಲ_Location{}, "ERROR: ", format, ಸಂದೇಶ_2);
		flush();

		ಎಸೆ log_cmd_error_exception();
	}

	formatted_error(Log_ಮೂಲ_Location{}, "ERROR: ", format, ಸಂದೇಶ_2);
}

ಶೂನ್ಯ LogManager::spacer()
{
	ಆದರೆ (newline_ಎಣಿಕೆ < 2) ಲಘುಗಣಕ("\n");
	ಆದರೆ (newline_ಎಣಿಕೆ < 2) ಲಘುಗಣಕ("\n");
}

ಶೂನ್ಯ LogManager::clear_original()
{
	std::erase_if(sinks, [](ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &sink) { ಹಿಂತಿರುಗಿ dynamic_cast<LogSinkRef *>(sink.get()) != ಶೂನ್ಯಸೂಚಕ; });
}

ಶೂನ್ಯ LogManager::ತಳ್ಳು()
{
	ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ತಳ್ಳು_ಹಿಂಭಾಗ(0);
}

ಶೂನ್ಯ LogManager::ತೆಗೆ_2()
{
	ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ತೆಗೆ_ಹಿಂಭಾಗ();
	flush();
}

#ಆದರೆ defined(YOSYS_ENABLE_DLOPEN)
ಶೂನ್ಯ log_backtrace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪೂರ್ಣಾಂಕ ಮಟ್ಟಗಳು)
{
	ಆದರೆ (ಮಟ್ಟಗಳು <= 0) ಹಿಂತಿರುಗಿ;

	Dl_info dli;
	ಶೂನ್ಯ *p;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #1: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #1: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 1) ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUG
	ಲಘುಗಣಕ("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #2: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #2: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 2) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #3: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #3: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 3) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #4: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #4: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 4) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #5: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #5: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 5) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #6: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #6: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 6) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #7: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #7: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 7) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #8: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #8: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 8) ಹಿಂತಿರುಗಿ;

	ಆದರೆ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		ಲಘುಗಣಕ("%sframe #9: %p %s(%p) %s(%p)\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("%sframe #9: ---\n", ಪೂರ್ವಪ್ರತ್ಯಯ_2);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ಮಟ್ಟಗಳು <= 9) ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
}
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ log_backtrace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಪೂರ್ಣಾಂಕ) { }
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ LogManager::reset_ರಾಶಿ()
{
	ಆಗ (ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ಗಾತ್ರ_2() > 1)
		ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ.ತೆಗೆ_ಹಿಂಭಾಗ();
	flush();
}

ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(RTLIL::IdString v) {
	ಆದರೆ (v == IdString::Null)
		ಲಘುಗಣಕ("(null)");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ID::is_static(v))
		ಲಘುಗಣಕ("%s", ID::unescaped_str(v));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (yosys_design && yosys_design->twines().is_ಜೀವಂತ(v))
		ಲಘುಗಣಕ("%s", yosys_design->twines().unescaped_str(v));
	ಇಲ್ಲದಿದ್ದರೆ
		ಲಘುಗಣಕ("%s", v.handle_token());
}

ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(RTLIL::SigSpec v) {
	ಲಘುಗಣಕ("%s", log_signal(v));
}

ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(RTLIL::State v) {
	ಲಘುಗಣಕ("%s", log_signal(v));
}

std::string log_signal(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, ತಾರ್ಕಿಕ autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ buf.str();
}

std::string log_const(ಸ್ಥಿರಾಂಕ RTLIL::Const &ಮೌಲ್ಯ, ತಾರ್ಕಿಕ autoint)
{
	ಆದರೆ ((ಮೌಲ್ಯ.flags & RTLIL::CONST_FLAG_STRING) == 0)
		ಹಿಂತಿರುಗಿ log_signal(ಮೌಲ್ಯ, autoint);

	ಹಿಂತಿರುಗಿ "\"" + ಮೌಲ್ಯ.decode_string() + "\"";
}

ಶೂನ್ಯ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, ಸುಳ್ಳು, ನಿಜ, ಸುಳ್ಳು, RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಲಘುಗಣಕ("%s", buf.str());
}

ಶೂನ್ಯ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : ಶೂನ್ಯಸೂಚಕ, RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಲಘುಗಣಕ("%s", buf.str());
}

ಶೂನ್ಯ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : ಶೂನ್ಯಸೂಚಕ, RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಲಘುಗಣಕ("%s", buf.str());
}

ಶೂನ್ಯ LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_ಅಂಶ> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_ಅಂಶ> expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log_copy, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning_copy, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error_copy;
	std::ಅದಲುಬದಲು(expect_warning_copy, expect_warning);
	std::ಅದಲುಬದಲು(expect_log_copy, expect_log);
	std::ಅದಲುಬದಲು(expect_error_copy, expect_error);
	std::ಅದಲುಬದಲು(expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning_copy, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning);
	std::ಅದಲುಬದಲು(expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log_copy, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log);
	std::ಅದಲುಬದಲು(expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error_copy, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error);

	ಸ್ವಯಂಚಾಲಿತ check = [&](ಸ್ಥಿರಾಂಕ std::string ಬಗೆ, std::string ನಮೂನೆ, Log_Expected_ಅಂಶ ಅಂಶ) {
		ಆದರೆ (ಅಂಶ.current_ಎಣಿಕೆ == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", ಬಗೆ, ನಮೂನೆ);
		}
		ಆದರೆ (ಅಂಶ.current_ಎಣಿಕೆ != ಅಂಶ.expected_ಎಣಿಕೆ) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				ಬಗೆ.c_str(), ನಮೂನೆ.c_str(), ಅಂಶ.current_ಎಣಿಕೆ, ಅಂಶ.expected_ಎಣಿಕೆ);
		}
	};

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ನಮೂನೆ, ಅಂಶ] : expect_warning_copy)
		check("warning", ನಮೂನೆ, ಅಂಶ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ನಮೂನೆ, ಅಂಶ] : expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning_copy)
		check("prefixed warning", ನಮೂನೆ, ಅಂಶ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ನಮೂನೆ, ಅಂಶ] : expect_log_copy)
		check("log", ನಮೂನೆ, ಅಂಶ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ನಮೂನೆ, ಅಂಶ] : expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log_copy)
		check("prefixed log", ನಮೂನೆ, ಅಂಶ);

	ಸ್ವಯಂಚಾಲಿತ check_err = [&](ಸ್ಥಿರಾಂಕ std::string ಬಗೆ, std::string ನಮೂನೆ, Log_Expected_ಅಂಶ ಅಂಶ) {
		ಆದರೆ (ಅಂಶ.current_ಎಣಿಕೆ == ಅಂಶ.expected_ಎಣಿಕೆ) {
			warn_regexes.clear();
			ಲಘುಗಣಕ("Expected %s pattern '%s' found !!!\n", ಬಗೆ, ನಮೂನೆ);
			yosys_shutdown();
			#ಆದರೆ defined(_MSC_VER)
				_exit(0);
			#ಇಲ್ಲದಿದ್ದರೆ
				_Exit(0);
			#ಆದರೆ_ಅಂತ್ಯ
		} ಇಲ್ಲದಿದ್ದರೆ {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", ಬಗೆ, ನಮೂನೆ);
		}
	};
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ನಮೂನೆ, ಅಂಶ] : expect_error_copy)
		check_err("error", ನಮೂನೆ, ಅಂಶ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ನಮೂನೆ, ಅಂಶ] : expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error_copy)
		check_err("prefixed error", ನಮೂನೆ, ಅಂಶ);
}

ಶೂನ್ಯ LogManager::report_unexpected_error()
{
	ಆದರೆ (expect_no_warnings && warnings_ಎಣಿಕೆ_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_ಗಾತ್ರ(warnings),
					warnings_ಎಣಿಕೆ, warnings_ಎಣಿಕೆ - warnings_ಎಣಿಕೆ_noexpect);
}

ಶೂನ್ಯ LogManager::ಕೂಡಿಸು_expect(std::string type, std::string ನಮೂನೆ, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
	ಆದರೆ (type == "error")
		expect_error[ನಮೂನೆ] = Log_Expected_ಅಂಶ(YS_REGEX_COMPILE(ನಮೂನೆ), ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "prefix-error")
		expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error[ನಮೂನೆ] = Log_Expected_ಅಂಶ(YS_REGEX_COMPILE(ನಮೂನೆ), ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "warning")
		expect_warning[ನಮೂನೆ] = Log_Expected_ಅಂಶ(YS_REGEX_COMPILE(ನಮೂನೆ), ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "prefix-warning")
		expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning[ನಮೂನೆ] = Log_Expected_ಅಂಶ(YS_REGEX_COMPILE(ನಮೂನೆ), ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "log")
		expect_log[ನಮೂನೆ] = Log_Expected_ಅಂಶ(YS_REGEX_COMPILE(ನಮೂನೆ), ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "prefix-log")
		expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log[ನಮೂನೆ] = Log_Expected_ಅಂಶ(YS_REGEX_COMPILE(ನಮೂನೆ), ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ log_abort();
}

ಶೂನ್ಯ LogManager::ಆರಂಭ_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::ಮುಕ್ತಾಯ_hasher()
{
	ಆದರೆ (!hasher)
		ಹಿಂತಿರುಗಿ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	ಹಿಂತಿರುಗಿ hash;
}

YOSYS_NAMESPACE_ಕೊನೆ
