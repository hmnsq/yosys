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

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#جيڪڏهن_پورو

#إذا defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#جيڪڏهن_پورو

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

std::chrono::steady_clock::وقت_نقطو LogManager::get_initial_وقت() ثابت
{
	ساكن ثابت خودڪار initial_وقت = std::chrono::steady_clock::now();
	أرجع initial_وقت;
}

فراغ (*log_error_atexit)() = خالي_اشارو;
فراغ (*log_verific_callback)(صحيح نياپو_type, ثابت حرف *پيغام_id, Log_ذريعو_Location src, ثابت حرف *نياپو) = خالي_اشارو;

ساكن منطقي ايندڙ_print_log = سچ;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقي سٽ_buffered, منطقي append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	إذا (!file)
		ارم std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	إذا (سٽ_buffered)
		setvbuf(file, خالي_اشارو, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	إذا (file)
		fclose(file);
}

فراغ FileLogSink::لاگرٿم(ثابت Log_پيغام &نياپو)
{
	fputs(نياپو.cached_نياپو.c_str(), file);
}

فراغ FileLogSink::flush()
{
	fflush(file);
}

فراغ ConsoleLogSink::لاگرٿم(ثابت Log_پيغام &نياپو)
{
	FILE *file = (نياپو.severity == LogSeverity::Error || نياپو.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(نياپو.cached_نياپو.c_str(), file);
}

فراغ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي StderrLogSink::should_log(ثابت Log_پيغام &نياپو) ثابت
{
	أرجع نياپو.severity == LogSeverity::Error || نياپو.severity == LogSeverity::NonFatalError ||
			(نياپو.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

فراغ StderrLogSink::لاگرٿم(ثابت Log_پيغام &نياپو)
{
	fputs(نياپو.cached_نياپو.c_str(), stderr);
}

فراغ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

فراغ ScratchPadLogSink::لاگرٿم(ثابت Log_پيغام &نياپو)
{
	RTLIL::Design *design = yosys_get_design();
	إذا (!design)
		أرجع;

	design->scratchpad[scratchpad].append(نياپو.cached_نياپو);
}

Log_پيغام::Log_پيغام(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string_view پيغام) :
	severity(severity),
	اڳياڙي_2(اڳياڙي_2),
	format(format),
	پيغام(پيغام),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string وقت_str;
	إذا (logger().get_log_وقت())
	{
		إذا (ايندڙ_print_log) {
			ايندڙ_print_log = خطأ;
			خودڪار elapsed = std::chrono::steady_clock::now() - logger().get_initial_وقت();
			خودڪار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			وقت_str += stringf("[%05d.%06d] ", صحيح(us.ڳڻپ() / 1'000'000),صحيح(us.ڳڻپ() % 1'000'000));
		}

		إذا (!format.خالي_جايون() && format.پٺ() == '\n')
			ايندڙ_print_log = سچ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		إذا (format == "%s" && !پيغام.خالي_جايون() && پيغام.پٺ() == '\n')
			ايندڙ_print_log = سچ;
	}
	std::string loc = !src.filename.خالي_جايون() ? stringf("%s:%d: ", src.filename, src.شروعات_سٽ) : "";
	cached_نياپو = stringf("%s%s%s%s", وقت_str, loc, اڳياڙي_2, پيغام);
}

فراغ LogManager::logv_string(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string str_اندريون) {
	سائيز_t remove_leading = 0;
	طالما (format.سائيز() > 1 && format[0] == '\n') {
		logv_string(severity, src, اڳياڙي_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	إذا (remove_leading > 0) {
		str_اندريون = str_اندريون.substr(remove_leading);
	}

	std::string loc = !src.filename.خالي_جايون() ? stringf("%s:%d: ", src.filename, src.شروعات_سٽ) : "";
	std::string str = stringf("%s%s%s",loc,اڳياڙي_2,str_اندريون);

	إذا (str.خالي_جايون())
		أرجع;

	سائيز_t nnl_pos = str.ڳوليو_آخري_not_جو('\n');
	إذا (nnl_pos == std::string::npos)
		newline_ڳڻپ += Get_سائيز(str);
	وإلا
		newline_ڳڻپ = Get_سائيز(str) - nnl_pos - 1;

	إذا (hasher)
		hasher->update(str);

	خودڪار نياپو = Log_پيغام(severity, src, اڳياڙي_2, format, str_اندريون);
	لكل (خودڪار &sink : sinks) {
		إذا (sink->should_log(نياپو))
			sink->لاگرٿم(نياپو);
	}

	إذا (severity == LogSeverity::سرو)
		str = str_اندريون;

	ساكن std::string linebuffer;
	ساكن منطقي warn_regex_recusion_guard = خطأ;

	إذا (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = سچ;

		إذا (warn_regexes.خالي_جايون() && expect_log.خالي_جايون() && expect_اڳياڙي_log.خالي_جايون())
		{
			linebuffer.clear();
		}
		وإلا
		{
			linebuffer += str;

			إذا (!linebuffer.خالي_جايون() && linebuffer.پٺ() == '\n') {
				لكل (خودڪار &re : warn_regexes)
					إذا (std::regex_ڳولا(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				لكل (خودڪار &[_, شي] : expect_log)
					إذا (std::regex_ڳولا(linebuffer, شي.نمونو))
						شي.current_ڳڻپ++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = خطأ;
	}
}

فراغ LogManager::formatted_string(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::سرگرم());

	إذا (make_debug && !is_debug(1))
		أرجع;
	logv_string(severity, src, اڳياڙي_2, format, std::move(str));
}

فراغ LogManager::formatted_سرو(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::سرگرم());

	spacer();
	إذا (سرو_ڳڻپ.سائيز() > 0)
		سرو_ڳڻپ.پٺ()++;

	إذا (صحيح(سرو_ڳڻپ.سائيز()) <= verbose_level) {
		log_stderr_sink_forced = سچ;
	}

	std::string سرو_id;

	لكل (صحيح c : سرو_ڳڻپ)
		سرو_id += stringf("%s%d", سرو_id.خالي_جايون() ? "" : ".", c);

	formatted_string(LogSeverity::سرو, Log_ذريعو_Location{}, stringf("%s. ", سرو_id), format, std::move(str));
	flush();

	إذا (hdump_all)
		hdump[سرو_id].وجهو("yosys_dump_" + سرو_id + ".il");

	إذا (hdump.ڳڻپ(سرو_id) && design != خالي_اشارو)
		لكل (خودڪار &filename : hdump.at(سرو_id)) {
			لاگرٿم("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = خطأ;
}

فراغ LogManager::formatted_warning(Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام)
{
	log_assert(!Multithreading::سرگرم());

	منطقي suppressed = خطأ;

	لكل (خودڪار &re : nowarn_regexes)
		إذا (std::regex_ڳولا(پيغام, re))
			suppressed = سچ;

	std::string loc = !src.filename.خالي_جايون() ? stringf("%s:%d: ", src.filename, src.شروعات_سٽ) : "";
	إذا (suppressed)
	{
		لاگرٿم("Suppressed %s%s%s", loc, اڳياڙي_2, پيغام);
	}
	وإلا
	{
		صحيح bak_make_debug = make_debug;
		make_debug = 0;

		لكل (خودڪار &re : werror_regexes)
			إذا (std::regex_ڳولا(پيغام, re))
				formatted_error(src, "ERROR: ", format, پيغام);

		منطقي warning_ميلاپ = خطأ;
		لكل (خودڪار &[_, شي] : expect_warning)
			إذا (std::regex_ڳولا(پيغام, شي.نمونو)) {
				شي.current_ڳڻپ++;
				warning_ميلاپ = سچ;
			}

		std::string نمونو = loc + string(اڳياڙي_2) + پيغام;
		لكل (خودڪار &[_, شي] : expect_اڳياڙي_warning)
			إذا (std::regex_ڳولا(نمونو, شي.نمونو)) {
				شي.current_ڳڻپ++;
				warning_ميلاپ = سچ;
			}

		إذا (warnings.ڳڻپ(پيغام))
		{
			formatted_string(LogSeverity::Info, src, اڳياڙي_2, format, پيغام);
			flush();
		}
		وإلا
		{
			formatted_string(LogSeverity::Warning, src, اڳياڙي_2, format, پيغام);
			flush();
			warnings.وجهو(پيغام);
		}

		إذا (!warning_ميلاپ)
			warnings_ڳڻپ_noexpect++;
		warnings_ڳڻپ++;
		make_debug = bak_make_debug;
	}
}

فراغ LogManager::suppressed() {
	إذا (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_ذريعو_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

فراغ LogManager::error_with_اڳياڙي(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام)
{
	صحيح bak_make_debug = make_debug;
	make_debug = 0;
	إذا (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, اڳياڙي_2, format, پيغام);
	flush();

	make_debug = bak_make_debug;

	لكل (خودڪار &[_, شي] : expect_error)
		إذا (std::regex_ڳولا(پيغام, شي.نمونو))
			شي.current_ڳڻپ++;

	std::string loc = !src.filename.خالي_جايون() ? stringf("%s:%d: ", src.filename, src.شروعات_سٽ) : "";
	std::string نمونو = loc + string(اڳياڙي_2) + پيغام;
	لكل (خودڪار &[_, شي] : expect_اڳياڙي_error)
		إذا (std::regex_ڳولا(نمونو, شي.نمونو))
			شي.current_ڳڻپ++;

	errors_ڳڻپ++;
}

[[noreturn]]
فراغ LogManager::formatted_error(Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام)
{
	error_with_اڳياڙي(LogSeverity::Error, src, اڳياڙي_2, format, پيغام);

	check_expected();

	إذا (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ثابت حرف *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	إذا (e && atoi(e))
		abort();

#إذا defined(_MSC_VER)
	_exit(1);
#وإلا
	_Exit(1);
#جيڪڏهن_پورو
}

فراغ LogManager::formatted_nonfatal_error(Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام)
{
	error_with_اڳياڙي(LogSeverity::NonFatalError, src, اڳياڙي_2, format, پيغام);
}

فراغ LogManager::جوڙيو_experimental(ثابت std::string &str)
{
	إذا (experimental_ignored.ڳڻپ(str) == 0 && experimental.ڳڻپ(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.وجهو(str);
	}
}

فراغ LogManager::جوڙيو_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	إذا (deprecated.ڳڻپ(feature) == 0) {
		إذا (earliest_removal.خالي_جايون())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وإلا
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

فراغ log_assert_failure(ثابت حرف *اظهار, ذريعو_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", اظهار, location.file_name(), location.سٽ());
}

فراغ log_abort_اندروني(ثابت حرف *file, صحيح سٽ)
{
	log_error("Abort in %s:%d.\n", file, سٽ);
}

فراغ log_yosys_abort_پيغام(std::string_view file, صحيح سٽ, std::string_view func, std::string_view پيغام)
{
	log_error("Abort in %s:%d (%s): %s\n", file, سٽ, func, پيغام);
}

فراغ LogManager::formatted_cmd_error(std::string_view format, std::string پيغام)
{
	إذا (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_ذريعو_Location{}, "ERROR: ", format, پيغام);
		flush();

		ارم log_cmd_error_exception();
	}

	formatted_error(Log_ذريعو_Location{}, "ERROR: ", format, پيغام);
}

فراغ LogManager::spacer()
{
	إذا (newline_ڳڻپ < 2) لاگرٿم("\n");
	إذا (newline_ڳڻپ < 2) لاگرٿم("\n");
}

فراغ LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت خودڪار &sink) { أرجع dynamic_cast<LogSinkRef *>(sink.get()) != خالي_اشارو; });
}

فراغ LogManager::ڌڪو()
{
	سرو_ڳڻپ.ڌڪو_پٺ(0);
}

فراغ LogManager::ڪڍو()
{
	سرو_ڳڻپ.ڪڍو_پٺ();
	flush();
}

#إذا defined(YOSYS_ENABLE_DLOPEN)
فراغ log_backtrace(ثابت حرف *اڳياڙي_2, صحيح سطحون)
{
	إذا (سطحون <= 0) أرجع;

	Dl_info dli;
	فراغ *p;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #1: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #1: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 1) أرجع;

#جيڪڏهن_وصف_نه_ٿيل DEBUG
	لاگرٿم("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", اڳياڙي_2);
#وإلا
	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #2: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #2: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 2) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #3: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #3: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 3) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #4: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #4: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 4) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #5: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #5: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 5) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #6: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #6: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 6) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #7: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #7: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 7) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #8: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #8: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 8) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		لاگرٿم("%sframe #9: %p %s(%p) %s(%p)\n", اڳياڙي_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لاگرٿم("%sframe #9: ---\n", اڳياڙي_2);
		أرجع;
	}

	إذا (سطحون <= 9) أرجع;
#جيڪڏهن_پورو
}
#وإلا
فراغ log_backtrace(ثابت حرف*, صحيح) { }
#جيڪڏهن_پورو

فراغ LogManager::reset_ڍير()
{
	طالما (سرو_ڳڻپ.سائيز() > 1)
		سرو_ڳڻپ.ڪڍو_پٺ();
	flush();
}

فراغ log_dump_val_ڪم_ڪندڙ(RTLIL::IdString v) {
	إذا (v == IdString::Null)
		لاگرٿم("(null)");
	وإلا إذا (ID::is_static(v))
		لاگرٿم("%s", ID::unescaped_str(v));
	وإلا إذا (yosys_design && yosys_design->twines().is_جيئرو(v))
		لاگرٿم("%s", yosys_design->twines().unescaped_str(v));
	وإلا
		لاگرٿم("%s", v.handle_token());
}

فراغ log_dump_val_ڪم_ڪندڙ(RTLIL::SigSpec v) {
	لاگرٿم("%s", log_signal(v));
}

فراغ log_dump_val_ڪم_ڪندڙ(RTLIL::State v) {
	لاگرٿم("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقي autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع buf.str();
}

std::string log_const(ثابت RTLIL::Const &قدر, منطقي autoint)
{
	إذا ((قدر.flags & RTLIL::CONST_FLAG_STRING) == 0)
		أرجع log_signal(قدر, autoint);

	أرجع "\"" + قدر.decode_string() + "\"";
}

فراغ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, خطأ, سچ, خطأ, RTLIL_BACKEND::Dump_گهڻائي::Readable);
	لاگرٿم("%s", buf.str());
}

فراغ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : خالي_اشارو, RTLIL_BACKEND::Dump_گهڻائي::Readable);
	لاگرٿم("%s", buf.str());
}

فراغ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : خالي_اشارو, RTLIL_BACKEND::Dump_گهڻائي::Readable);
	لاگرٿم("%s", buf.str());
}

فراغ LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_شي> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_شي> expect_اڳياڙي_log_copy, expect_اڳياڙي_warning_copy, expect_اڳياڙي_error_copy;
	std::مٽا_سٽا(expect_warning_copy, expect_warning);
	std::مٽا_سٽا(expect_log_copy, expect_log);
	std::مٽا_سٽا(expect_error_copy, expect_error);
	std::مٽا_سٽا(expect_اڳياڙي_warning_copy, expect_اڳياڙي_warning);
	std::مٽا_سٽا(expect_اڳياڙي_log_copy, expect_اڳياڙي_log);
	std::مٽا_سٽا(expect_اڳياڙي_error_copy, expect_اڳياڙي_error);

	خودڪار check = [&](ثابت std::string قسم, std::string نمونو, Log_Expected_شي شي) {
		إذا (شي.current_ڳڻپ == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", قسم, نمونو);
		}
		إذا (شي.current_ڳڻپ != شي.expected_ڳڻپ) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				قسم.c_str(), نمونو.c_str(), شي.current_ڳڻپ, شي.expected_ڳڻپ);
		}
	};

	لكل (خودڪار &[نمونو, شي] : expect_warning_copy)
		check("warning", نمونو, شي);
	لكل (خودڪار &[نمونو, شي] : expect_اڳياڙي_warning_copy)
		check("prefixed warning", نمونو, شي);
	لكل (خودڪار &[نمونو, شي] : expect_log_copy)
		check("log", نمونو, شي);
	لكل (خودڪار &[نمونو, شي] : expect_اڳياڙي_log_copy)
		check("prefixed log", نمونو, شي);

	خودڪار check_err = [&](ثابت std::string قسم, std::string نمونو, Log_Expected_شي شي) {
		إذا (شي.current_ڳڻپ == شي.expected_ڳڻپ) {
			warn_regexes.clear();
			لاگرٿم("Expected %s pattern '%s' found !!!\n", قسم, نمونو);
			yosys_shutdown();
			#إذا defined(_MSC_VER)
				_exit(0);
			#وإلا
				_Exit(0);
			#جيڪڏهن_پورو
		} وإلا {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", قسم, نمونو);
		}
	};
	لكل (خودڪار &[نمونو, شي] : expect_error_copy)
		check_err("error", نمونو, شي);
	لكل (خودڪار &[نمونو, شي] : expect_اڳياڙي_error_copy)
		check_err("prefixed error", نمونو, شي);
}

فراغ LogManager::report_unexpected_error()
{
	إذا (expect_no_warnings && warnings_ڳڻپ_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_سائيز(warnings),
					warnings_ڳڻپ, warnings_ڳڻپ - warnings_ڳڻپ_noexpect);
}

فراغ LogManager::جوڙيو_expect(std::string type, std::string نمونو, صحيح ڳڻپ)
{
	إذا (type == "error")
		expect_error[نمونو] = Log_Expected_شي(YS_REGEX_COMPILE(نمونو), ڳڻپ);
	وإلا إذا (type == "prefix-error")
		expect_اڳياڙي_error[نمونو] = Log_Expected_شي(YS_REGEX_COMPILE(نمونو), ڳڻپ);
	وإلا إذا (type == "warning")
		expect_warning[نمونو] = Log_Expected_شي(YS_REGEX_COMPILE(نمونو), ڳڻپ);
	وإلا إذا (type == "prefix-warning")
		expect_اڳياڙي_warning[نمونو] = Log_Expected_شي(YS_REGEX_COMPILE(نمونو), ڳڻپ);
	وإلا إذا (type == "log")
		expect_log[نمونو] = Log_Expected_شي(YS_REGEX_COMPILE(نمونو), ڳڻپ);
	وإلا إذا (type == "prefix-log")
		expect_اڳياڙي_log[نمونو] = Log_Expected_شي(YS_REGEX_COMPILE(نمونو), ڳڻپ);
	وإلا log_abort();
}

فراغ LogManager::شروعات_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::پڄاڻي_hasher()
{
	إذا (!hasher)
		أرجع {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	أرجع hash;
}

YOSYS_NAMESPACE_پڇاڙي
