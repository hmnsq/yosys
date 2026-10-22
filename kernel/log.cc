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
#د_که_پای

#إذا defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#د_که_پای

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

std::chrono::steady_clock::وخت_ټکی LogManager::get_initial_وخت() ثابت
{
	ساكن ثابت خپلکاری initial_وخت = std::chrono::steady_clock::now();
	أرجع initial_وخت;
}

فراغ (*log_error_atexit)() = تش_اشاره;
فراغ (*log_verific_callback)(صحيح پیغام_type, ثابت حرف *پیغام_id, Log_سرچینه_Location src, ثابت حرف *پیغام) = تش_اشاره;

ساكن منطقي بل_print_log = ریښتیا;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقي کرښه_buffered, منطقي append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	إذا (!file)
		ارم std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	إذا (کرښه_buffered)
		setvbuf(file, تش_اشاره, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	إذا (file)
		fclose(file);
}

فراغ FileLogSink::لوګاریتم(ثابت Log_پیغام &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), file);
}

فراغ FileLogSink::flush()
{
	fflush(file);
}

فراغ ConsoleLogSink::لوګاریتم(ثابت Log_پیغام &پیغام)
{
	FILE *file = (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(پیغام.cached_پیغام.c_str(), file);
}

فراغ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي StderrLogSink::should_log(ثابت Log_پیغام &پیغام) ثابت
{
	أرجع پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

فراغ StderrLogSink::لوګاریتم(ثابت Log_پیغام &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), stderr);
}

فراغ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

فراغ ScratchPadLogSink::لوګاریتم(ثابت Log_پیغام &پیغام)
{
	RTLIL::Design *design = yosys_get_design();
	إذا (!design)
		أرجع;

	design->scratchpad[scratchpad].append(پیغام.cached_پیغام);
}

Log_پیغام::Log_پیغام(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string_view پیغام_2) :
	severity(severity),
	مختاړی_2(مختاړی_2),
	format(format),
	پیغام_2(پیغام_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string وخت_str;
	إذا (logger().get_log_وخت())
	{
		إذا (بل_print_log) {
			بل_print_log = خطأ;
			خپلکاری elapsed = std::chrono::steady_clock::now() - logger().get_initial_وخت();
			خپلکاری us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			وخت_str += stringf("[%05d.%06d] ", صحيح(us.شمېر() / 1'000'000),صحيح(us.شمېر() % 1'000'000));
		}

		إذا (!format.تشه() && format.شا() == '\n')
			بل_print_log = ریښتیا;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		إذا (format == "%s" && !پیغام_2.تشه() && پیغام_2.شا() == '\n')
			بل_print_log = ریښتیا;
	}
	std::string loc = !src.filename.تشه() ? stringf("%s:%d: ", src.filename, src.پیل_کرښه) : "";
	cached_پیغام = stringf("%s%s%s%s", وخت_str, loc, مختاړی_2, پیغام_2);
}

فراغ LogManager::logv_string(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string str_دننه) {
	اندازه_t remove_leading = 0;
	طالما (format.اندازه() > 1 && format[0] == '\n') {
		logv_string(severity, src, مختاړی_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	إذا (remove_leading > 0) {
		str_دننه = str_دننه.substr(remove_leading);
	}

	std::string loc = !src.filename.تشه() ? stringf("%s:%d: ", src.filename, src.پیل_کرښه) : "";
	std::string str = stringf("%s%s%s",loc,مختاړی_2,str_دننه);

	إذا (str.تشه())
		أرجع;

	اندازه_t nnl_pos = str.پیدا_کړه_وروستی_not_د('\n');
	إذا (nnl_pos == std::string::npos)
		newline_شمېر += Get_اندازه(str);
	وإلا
		newline_شمېر = Get_اندازه(str) - nnl_pos - 1;

	إذا (hasher)
		hasher->update(str);

	خپلکاری پیغام = Log_پیغام(severity, src, مختاړی_2, format, str_دننه);
	لكل (خپلکاری &sink : sinks) {
		إذا (sink->should_log(پیغام))
			sink->لوګاریتم(پیغام);
	}

	إذا (severity == LogSeverity::سرلیک)
		str = str_دننه;

	ساكن std::string linebuffer;
	ساكن منطقي warn_regex_recusion_guard = خطأ;

	إذا (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = ریښتیا;

		إذا (warn_regexes.تشه() && expect_log.تشه() && expect_مختاړی_log.تشه())
		{
			linebuffer.clear();
		}
		وإلا
		{
			linebuffer += str;

			إذا (!linebuffer.تشه() && linebuffer.شا() == '\n') {
				لكل (خپلکاری &re : warn_regexes)
					إذا (std::regex_پلټنه(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				لكل (خپلکاری &[_, توکی] : expect_log)
					إذا (std::regex_پلټنه(linebuffer, توکی.نمونه))
						توکی.current_شمېر++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = خطأ;
	}
}

فراغ LogManager::formatted_string(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::فعال());

	إذا (make_debug && !is_debug(1))
		أرجع;
	logv_string(severity, src, مختاړی_2, format, std::move(str));
}

فراغ LogManager::formatted_سرلیک(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::فعال());

	spacer();
	إذا (سرلیک_شمېر.اندازه() > 0)
		سرلیک_شمېر.شا()++;

	إذا (صحيح(سرلیک_شمېر.اندازه()) <= verbose_level) {
		log_stderr_sink_forced = ریښتیا;
	}

	std::string سرلیک_id;

	لكل (صحيح c : سرلیک_شمېر)
		سرلیک_id += stringf("%s%d", سرلیک_id.تشه() ? "" : ".", c);

	formatted_string(LogSeverity::سرلیک, Log_سرچینه_Location{}, stringf("%s. ", سرلیک_id), format, std::move(str));
	flush();

	إذا (hdump_all)
		hdump[سرلیک_id].ورننباسه("yosys_dump_" + سرلیک_id + ".il");

	إذا (hdump.شمېر(سرلیک_id) && design != تش_اشاره)
		لكل (خپلکاری &filename : hdump.at(سرلیک_id)) {
			لوګاریتم("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = خطأ;
}

فراغ LogManager::formatted_warning(Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2)
{
	log_assert(!Multithreading::فعال());

	منطقي suppressed = خطأ;

	لكل (خپلکاری &re : nowarn_regexes)
		إذا (std::regex_پلټنه(پیغام_2, re))
			suppressed = ریښتیا;

	std::string loc = !src.filename.تشه() ? stringf("%s:%d: ", src.filename, src.پیل_کرښه) : "";
	إذا (suppressed)
	{
		لوګاریتم("Suppressed %s%s%s", loc, مختاړی_2, پیغام_2);
	}
	وإلا
	{
		صحيح bak_make_debug = make_debug;
		make_debug = 0;

		لكل (خپلکاری &re : werror_regexes)
			إذا (std::regex_پلټنه(پیغام_2, re))
				formatted_error(src, "ERROR: ", format, پیغام_2);

		منطقي warning_سمون = خطأ;
		لكل (خپلکاری &[_, توکی] : expect_warning)
			إذا (std::regex_پلټنه(پیغام_2, توکی.نمونه)) {
				توکی.current_شمېر++;
				warning_سمون = ریښتیا;
			}

		std::string نمونه = loc + string(مختاړی_2) + پیغام_2;
		لكل (خپلکاری &[_, توکی] : expect_مختاړی_warning)
			إذا (std::regex_پلټنه(نمونه, توکی.نمونه)) {
				توکی.current_شمېر++;
				warning_سمون = ریښتیا;
			}

		إذا (warnings.شمېر(پیغام_2))
		{
			formatted_string(LogSeverity::Info, src, مختاړی_2, format, پیغام_2);
			flush();
		}
		وإلا
		{
			formatted_string(LogSeverity::Warning, src, مختاړی_2, format, پیغام_2);
			flush();
			warnings.ورننباسه(پیغام_2);
		}

		إذا (!warning_سمون)
			warnings_شمېر_noexpect++;
		warnings_شمېر++;
		make_debug = bak_make_debug;
	}
}

فراغ LogManager::suppressed() {
	إذا (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_سرچینه_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

فراغ LogManager::error_with_مختاړی(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2)
{
	صحيح bak_make_debug = make_debug;
	make_debug = 0;
	إذا (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, مختاړی_2, format, پیغام_2);
	flush();

	make_debug = bak_make_debug;

	لكل (خپلکاری &[_, توکی] : expect_error)
		إذا (std::regex_پلټنه(پیغام_2, توکی.نمونه))
			توکی.current_شمېر++;

	std::string loc = !src.filename.تشه() ? stringf("%s:%d: ", src.filename, src.پیل_کرښه) : "";
	std::string نمونه = loc + string(مختاړی_2) + پیغام_2;
	لكل (خپلکاری &[_, توکی] : expect_مختاړی_error)
		إذا (std::regex_پلټنه(نمونه, توکی.نمونه))
			توکی.current_شمېر++;

	errors_شمېر++;
}

[[noreturn]]
فراغ LogManager::formatted_error(Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2)
{
	error_with_مختاړی(LogSeverity::Error, src, مختاړی_2, format, پیغام_2);

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
#د_که_پای
}

فراغ LogManager::formatted_nonfatal_error(Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2)
{
	error_with_مختاړی(LogSeverity::NonFatalError, src, مختاړی_2, format, پیغام_2);
}

فراغ LogManager::جمع_کړه_experimental(ثابت std::string &str)
{
	إذا (experimental_ignored.شمېر(str) == 0 && experimental.شمېر(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ورننباسه(str);
	}
}

فراغ LogManager::جمع_کړه_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	إذا (deprecated.شمېر(feature) == 0) {
		إذا (earliest_removal.تشه())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وإلا
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

فراغ log_assert_failure(ثابت حرف *بیان, سرچینه_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", بیان, location.file_name(), location.کرښه());
}

فراغ log_abort_دننی(ثابت حرف *file, صحيح کرښه)
{
	log_error("Abort in %s:%d.\n", file, کرښه);
}

فراغ log_yosys_abort_پیغام(std::string_view file, صحيح کرښه, std::string_view func, std::string_view پیغام_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, کرښه, func, پیغام_2);
}

فراغ LogManager::formatted_cmd_error(std::string_view format, std::string پیغام_2)
{
	إذا (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_سرچینه_Location{}, "ERROR: ", format, پیغام_2);
		flush();

		ارم log_cmd_error_exception();
	}

	formatted_error(Log_سرچینه_Location{}, "ERROR: ", format, پیغام_2);
}

فراغ LogManager::spacer()
{
	إذا (newline_شمېر < 2) لوګاریتم("\n");
	إذا (newline_شمېر < 2) لوګاریتم("\n");
}

فراغ LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت خپلکاری &sink) { أرجع dynamic_cast<LogSinkRef *>(sink.get()) != تش_اشاره; });
}

فراغ LogManager::ټېل()
{
	سرلیک_شمېر.ټېل_شا(0);
}

فراغ LogManager::راوباسه()
{
	سرلیک_شمېر.راوباسه_شا();
	flush();
}

#إذا defined(YOSYS_ENABLE_DLOPEN)
فراغ log_backtrace(ثابت حرف *مختاړی_2, صحيح پوړونه)
{
	إذا (پوړونه <= 0) أرجع;

	Dl_info dli;
	فراغ *p;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #1: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #1: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 1) أرجع;

#که_نه_تعریف_شوی DEBUG
	لوګاریتم("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", مختاړی_2);
#وإلا
	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #2: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #2: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 2) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #3: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #3: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 3) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #4: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #4: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 4) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #5: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #5: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 5) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #6: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #6: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 6) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #7: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #7: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 7) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #8: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #8: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 8) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		لوګاریتم("%sframe #9: %p %s(%p) %s(%p)\n", مختاړی_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوګاریتم("%sframe #9: ---\n", مختاړی_2);
		أرجع;
	}

	إذا (پوړونه <= 9) أرجع;
#د_که_پای
}
#وإلا
فراغ log_backtrace(ثابت حرف*, صحيح) { }
#د_که_پای

فراغ LogManager::reset_ډېری()
{
	طالما (سرلیک_شمېر.اندازه() > 1)
		سرلیک_شمېر.راوباسه_شا();
	flush();
}

فراغ log_dump_val_کارګر(RTLIL::IdString v) {
	إذا (v == IdString::Null)
		لوګاریتم("(null)");
	وإلا إذا (ID::is_static(v))
		لوګاریتم("%s", ID::unescaped_str(v));
	وإلا إذا (yosys_design && yosys_design->twines().is_ژوندی(v))
		لوګاریتم("%s", yosys_design->twines().unescaped_str(v));
	وإلا
		لوګاریتم("%s", v.handle_token());
}

فراغ log_dump_val_کارګر(RTLIL::SigSpec v) {
	لوګاریتم("%s", log_signal(v));
}

فراغ log_dump_val_کارګر(RTLIL::State v) {
	لوګاریتم("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقي autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع buf.str();
}

std::string log_const(ثابت RTLIL::Const &ارزښت, منطقي autoint)
{
	إذا ((ارزښت.flags & RTLIL::CONST_FLAG_STRING) == 0)
		أرجع log_signal(ارزښت, autoint);

	أرجع "\"" + ارزښت.decode_string() + "\"";
}

فراغ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, خطأ, ریښتیا, خطأ, RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	لوګاریتم("%s", buf.str());
}

فراغ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : تش_اشاره, RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	لوګاریتم("%s", buf.str());
}

فراغ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : تش_اشاره, RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	لوګاریتم("%s", buf.str());
}

فراغ LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_توکی> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_توکی> expect_مختاړی_log_copy, expect_مختاړی_warning_copy, expect_مختاړی_error_copy;
	std::بدلون(expect_warning_copy, expect_warning);
	std::بدلون(expect_log_copy, expect_log);
	std::بدلون(expect_error_copy, expect_error);
	std::بدلون(expect_مختاړی_warning_copy, expect_مختاړی_warning);
	std::بدلون(expect_مختاړی_log_copy, expect_مختاړی_log);
	std::بدلون(expect_مختاړی_error_copy, expect_مختاړی_error);

	خپلکاری check = [&](ثابت std::string ډول, std::string نمونه, Log_Expected_توکی توکی) {
		إذا (توکی.current_شمېر == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", ډول, نمونه);
		}
		إذا (توکی.current_شمېر != توکی.expected_شمېر) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				ډول.c_str(), نمونه.c_str(), توکی.current_شمېر, توکی.expected_شمېر);
		}
	};

	لكل (خپلکاری &[نمونه, توکی] : expect_warning_copy)
		check("warning", نمونه, توکی);
	لكل (خپلکاری &[نمونه, توکی] : expect_مختاړی_warning_copy)
		check("prefixed warning", نمونه, توکی);
	لكل (خپلکاری &[نمونه, توکی] : expect_log_copy)
		check("log", نمونه, توکی);
	لكل (خپلکاری &[نمونه, توکی] : expect_مختاړی_log_copy)
		check("prefixed log", نمونه, توکی);

	خپلکاری check_err = [&](ثابت std::string ډول, std::string نمونه, Log_Expected_توکی توکی) {
		إذا (توکی.current_شمېر == توکی.expected_شمېر) {
			warn_regexes.clear();
			لوګاریتم("Expected %s pattern '%s' found !!!\n", ډول, نمونه);
			yosys_shutdown();
			#إذا defined(_MSC_VER)
				_exit(0);
			#وإلا
				_Exit(0);
			#د_که_پای
		} وإلا {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", ډول, نمونه);
		}
	};
	لكل (خپلکاری &[نمونه, توکی] : expect_error_copy)
		check_err("error", نمونه, توکی);
	لكل (خپلکاری &[نمونه, توکی] : expect_مختاړی_error_copy)
		check_err("prefixed error", نمونه, توکی);
}

فراغ LogManager::report_unexpected_error()
{
	إذا (expect_no_warnings && warnings_شمېر_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_اندازه(warnings),
					warnings_شمېر, warnings_شمېر - warnings_شمېر_noexpect);
}

فراغ LogManager::جمع_کړه_expect(std::string type, std::string نمونه, صحيح شمېر)
{
	إذا (type == "error")
		expect_error[نمونه] = Log_Expected_توکی(YS_REGEX_COMPILE(نمونه), شمېر);
	وإلا إذا (type == "prefix-error")
		expect_مختاړی_error[نمونه] = Log_Expected_توکی(YS_REGEX_COMPILE(نمونه), شمېر);
	وإلا إذا (type == "warning")
		expect_warning[نمونه] = Log_Expected_توکی(YS_REGEX_COMPILE(نمونه), شمېر);
	وإلا إذا (type == "prefix-warning")
		expect_مختاړی_warning[نمونه] = Log_Expected_توکی(YS_REGEX_COMPILE(نمونه), شمېر);
	وإلا إذا (type == "log")
		expect_log[نمونه] = Log_Expected_توکی(YS_REGEX_COMPILE(نمونه), شمېر);
	وإلا إذا (type == "prefix-log")
		expect_مختاړی_log[نمونه] = Log_Expected_توکی(YS_REGEX_COMPILE(نمونه), شمېر);
	وإلا log_abort();
}

فراغ LogManager::پیل_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::پای_hasher()
{
	إذا (!hasher)
		أرجع {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	أرجع hash;
}

YOSYS_NAMESPACE_پای_ټکی
