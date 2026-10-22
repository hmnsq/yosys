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

#شامل "kernel/yosys.h"
#شامل "libs/sha1/sha1.h"
#شامل "backends/rtlil/rtlil_backend.h"

#اگر !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#اگر_ختم

#اگر defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#اگر_ختم

#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <stdarg.h>
#شامل <vector>
#شامل <list>

YOSYS_NAMESPACE_BEGIN

منطقی log_stderr_sink_forced = جھوٹ;

LogManager &logger()
{
	ساکن LogManager instance;
	واپس instance;
}

std::chrono::steady_clock::وقت_نقطہ LogManager::get_initial_وقت() ثابت
{
	ساکن ثابت خودکار initial_وقت = std::chrono::steady_clock::now();
	واپس initial_وقت;
}

خالی (*log_error_atexit)() = خالی_اشاریہ;
خالی (*log_verific_callback)(صحیح پیغام_type, ثابت حرف *پیغام_id, Log_ماخذ_Location src, ثابت حرف *پیغام) = خالی_اشاریہ;

ساکن منطقی اگلا_print_log = سچ;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقی سطر_buffered, منطقی append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	اگر (!file)
		پھینکو std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	اگر (سطر_buffered)
		setvbuf(file, خالی_اشاریہ, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	اگر (file)
		fclose(file);
}

خالی FileLogSink::لوگارتھم(ثابت Log_پیغام &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), file);
}

خالی FileLogSink::flush()
{
	fflush(file);
}

خالی ConsoleLogSink::لوگارتھم(ثابت Log_پیغام &پیغام)
{
	FILE *file = (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(پیغام.cached_پیغام.c_str(), file);
}

خالی ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقی StderrLogSink::should_log(ثابت Log_پیغام &پیغام) ثابت
{
	واپس پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

خالی StderrLogSink::لوگارتھم(ثابت Log_پیغام &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), stderr);
}

خالی StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

خالی ScratchPadLogSink::لوگارتھم(ثابت Log_پیغام &پیغام)
{
	RTLIL::Design *design = yosys_get_design();
	اگر (!design)
		واپس;

	design->scratchpad[scratchpad].append(پیغام.cached_پیغام);
}

Log_پیغام::Log_پیغام(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string_view پیغام_2) :
	severity(severity),
	سابقہ_2(سابقہ_2),
	format(format),
	پیغام_2(پیغام_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string وقت_str;
	اگر (logger().get_log_وقت())
	{
		اگر (اگلا_print_log) {
			اگلا_print_log = جھوٹ;
			خودکار elapsed = std::chrono::steady_clock::now() - logger().get_initial_وقت();
			خودکار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			وقت_str += stringf("[%05d.%06d] ", صحیح(us.گنتی() / 1'000'000),صحیح(us.گنتی() % 1'000'000));
		}

		اگر (!format.خالی_خانے() && format.عقب() == '\n')
			اگلا_print_log = سچ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		اگر (format == "%s" && !پیغام_2.خالی_خانے() && پیغام_2.عقب() == '\n')
			اگلا_print_log = سچ;
	}
	std::string loc = !src.filename.خالی_خانے() ? stringf("%s:%d: ", src.filename, src.آغاز_سطر) : "";
	cached_پیغام = stringf("%s%s%s%s", وقت_str, loc, سابقہ_2, پیغام_2);
}

خالی LogManager::logv_string(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string str_اندر) {
	سائز_t remove_leading = 0;
	جب (format.سائز() > 1 && format[0] == '\n') {
		logv_string(severity, src, سابقہ_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	اگر (remove_leading > 0) {
		str_اندر = str_اندر.substr(remove_leading);
	}

	std::string loc = !src.filename.خالی_خانے() ? stringf("%s:%d: ", src.filename, src.آغاز_سطر) : "";
	std::string str = stringf("%s%s%s",loc,سابقہ_2,str_اندر);

	اگر (str.خالی_خانے())
		واپس;

	سائز_t nnl_pos = str.ڈھونڈو_آخری_not_کا('\n');
	اگر (nnl_pos == std::string::npos)
		newline_گنتی += Get_سائز(str);
	ورنہ
		newline_گنتی = Get_سائز(str) - nnl_pos - 1;

	اگر (hasher)
		hasher->update(str);

	خودکار پیغام = Log_پیغام(severity, src, سابقہ_2, format, str_اندر);
	برائے (خودکار &sink : sinks) {
		اگر (sink->should_log(پیغام))
			sink->لوگارتھم(پیغام);
	}

	اگر (severity == LogSeverity::سرخی)
		str = str_اندر;

	ساکن std::string linebuffer;
	ساکن منطقی warn_regex_recusion_guard = جھوٹ;

	اگر (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = سچ;

		اگر (warn_regexes.خالی_خانے() && expect_log.خالی_خانے() && expect_سابقہ_log.خالی_خانے())
		{
			linebuffer.clear();
		}
		ورنہ
		{
			linebuffer += str;

			اگر (!linebuffer.خالی_خانے() && linebuffer.عقب() == '\n') {
				برائے (خودکار &re : warn_regexes)
					اگر (std::regex_تلاش(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				برائے (خودکار &[_, شے_2] : expect_log)
					اگر (std::regex_تلاش(linebuffer, شے_2.نمونہ))
						شے_2.current_گنتی++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = جھوٹ;
	}
}

خالی LogManager::formatted_string(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::فعال());

	اگر (make_debug && !is_debug(1))
		واپس;
	logv_string(severity, src, سابقہ_2, format, std::move(str));
}

خالی LogManager::formatted_سرخی(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::فعال());

	spacer();
	اگر (سرخی_گنتی.سائز() > 0)
		سرخی_گنتی.عقب()++;

	اگر (صحیح(سرخی_گنتی.سائز()) <= verbose_level) {
		log_stderr_sink_forced = سچ;
	}

	std::string سرخی_id;

	برائے (صحیح c : سرخی_گنتی)
		سرخی_id += stringf("%s%d", سرخی_id.خالی_خانے() ? "" : ".", c);

	formatted_string(LogSeverity::سرخی, Log_ماخذ_Location{}, stringf("%s. ", سرخی_id), format, std::move(str));
	flush();

	اگر (hdump_all)
		hdump[سرخی_id].داخل_کرو("yosys_dump_" + سرخی_id + ".il");

	اگر (hdump.گنتی(سرخی_id) && design != خالی_اشاریہ)
		برائے (خودکار &filename : hdump.at(سرخی_id)) {
			لوگارتھم("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = جھوٹ;
}

خالی LogManager::formatted_warning(Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2)
{
	log_assert(!Multithreading::فعال());

	منطقی suppressed = جھوٹ;

	برائے (خودکار &re : nowarn_regexes)
		اگر (std::regex_تلاش(پیغام_2, re))
			suppressed = سچ;

	std::string loc = !src.filename.خالی_خانے() ? stringf("%s:%d: ", src.filename, src.آغاز_سطر) : "";
	اگر (suppressed)
	{
		لوگارتھم("Suppressed %s%s%s", loc, سابقہ_2, پیغام_2);
	}
	ورنہ
	{
		صحیح bak_make_debug = make_debug;
		make_debug = 0;

		برائے (خودکار &re : werror_regexes)
			اگر (std::regex_تلاش(پیغام_2, re))
				formatted_error(src, "ERROR: ", format, پیغام_2);

		منطقی warning_مطابقت = جھوٹ;
		برائے (خودکار &[_, شے_2] : expect_warning)
			اگر (std::regex_تلاش(پیغام_2, شے_2.نمونہ)) {
				شے_2.current_گنتی++;
				warning_مطابقت = سچ;
			}

		std::string نمونہ = loc + string(سابقہ_2) + پیغام_2;
		برائے (خودکار &[_, شے_2] : expect_سابقہ_warning)
			اگر (std::regex_تلاش(نمونہ, شے_2.نمونہ)) {
				شے_2.current_گنتی++;
				warning_مطابقت = سچ;
			}

		اگر (warnings.گنتی(پیغام_2))
		{
			formatted_string(LogSeverity::Info, src, سابقہ_2, format, پیغام_2);
			flush();
		}
		ورنہ
		{
			formatted_string(LogSeverity::Warning, src, سابقہ_2, format, پیغام_2);
			flush();
			warnings.داخل_کرو(پیغام_2);
		}

		اگر (!warning_مطابقت)
			warnings_گنتی_noexpect++;
		warnings_گنتی++;
		make_debug = bak_make_debug;
	}
}

خالی LogManager::suppressed() {
	اگر (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_ماخذ_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

خالی LogManager::error_with_سابقہ(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2)
{
	صحیح bak_make_debug = make_debug;
	make_debug = 0;
	اگر (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, سابقہ_2, format, پیغام_2);
	flush();

	make_debug = bak_make_debug;

	برائے (خودکار &[_, شے_2] : expect_error)
		اگر (std::regex_تلاش(پیغام_2, شے_2.نمونہ))
			شے_2.current_گنتی++;

	std::string loc = !src.filename.خالی_خانے() ? stringf("%s:%d: ", src.filename, src.آغاز_سطر) : "";
	std::string نمونہ = loc + string(سابقہ_2) + پیغام_2;
	برائے (خودکار &[_, شے_2] : expect_سابقہ_error)
		اگر (std::regex_تلاش(نمونہ, شے_2.نمونہ))
			شے_2.current_گنتی++;

	errors_گنتی++;
}

[[noreturn]]
خالی LogManager::formatted_error(Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2)
{
	error_with_سابقہ(LogSeverity::Error, src, سابقہ_2, format, پیغام_2);

	check_expected();

	اگر (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ثابت حرف *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	اگر (e && atoi(e))
		abort();

#اگر defined(_MSC_VER)
	_exit(1);
#ورنہ
	_Exit(1);
#اگر_ختم
}

خالی LogManager::formatted_nonfatal_error(Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2)
{
	error_with_سابقہ(LogSeverity::NonFatalError, src, سابقہ_2, format, پیغام_2);
}

خالی LogManager::جوڑو_experimental(ثابت std::string &str)
{
	اگر (experimental_ignored.گنتی(str) == 0 && experimental.گنتی(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.داخل_کرو(str);
	}
}

خالی LogManager::جوڑو_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	اگر (deprecated.گنتی(feature) == 0) {
		اگر (earliest_removal.خالی_خانے())
			log_warning("Feature '%s' is deprecated.\n", feature);
		ورنہ
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

خالی log_assert_failure(ثابت حرف *اظہاریہ, ماخذ_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", اظہاریہ, location.file_name(), location.سطر());
}

خالی log_abort_اندرونی(ثابت حرف *file, صحیح سطر)
{
	log_error("Abort in %s:%d.\n", file, سطر);
}

خالی log_yosys_abort_پیغام(std::string_view file, صحیح سطر, std::string_view func, std::string_view پیغام_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, سطر, func, پیغام_2);
}

خالی LogManager::formatted_cmd_error(std::string_view format, std::string پیغام_2)
{
	اگر (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_ماخذ_Location{}, "ERROR: ", format, پیغام_2);
		flush();

		پھینکو log_cmd_error_exception();
	}

	formatted_error(Log_ماخذ_Location{}, "ERROR: ", format, پیغام_2);
}

خالی LogManager::spacer()
{
	اگر (newline_گنتی < 2) لوگارتھم("\n");
	اگر (newline_گنتی < 2) لوگارتھم("\n");
}

خالی LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت خودکار &sink) { واپس dynamic_cast<LogSinkRef *>(sink.get()) != خالی_اشاریہ; });
}

خالی LogManager::ڈالو()
{
	سرخی_گنتی.ڈالو_عقب(0);
}

خالی LogManager::نکالو()
{
	سرخی_گنتی.نکالو_عقب();
	flush();
}

#اگر defined(YOSYS_ENABLE_DLOPEN)
خالی log_backtrace(ثابت حرف *سابقہ_2, صحیح سطحیں)
{
	اگر (سطحیں <= 0) واپس;

	Dl_info dli;
	خالی *p;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #1: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #1: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 1) واپس;

#اگر_غیر_تعریف_شدہ DEBUG
	لوگارتھم("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", سابقہ_2);
#ورنہ
	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #2: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #2: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 2) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #3: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #3: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 3) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #4: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #4: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 4) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #5: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #5: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 5) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #6: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #6: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 6) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #7: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #7: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 7) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #8: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #8: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 8) واپس;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #9: %p %s(%p) %s(%p)\n", سابقہ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ورنہ {
		لوگارتھم("%sframe #9: ---\n", سابقہ_2);
		واپس;
	}

	اگر (سطحیں <= 9) واپس;
#اگر_ختم
}
#ورنہ
خالی log_backtrace(ثابت حرف*, صحیح) { }
#اگر_ختم

خالی LogManager::reset_ڈھیر()
{
	جب (سرخی_گنتی.سائز() > 1)
		سرخی_گنتی.نکالو_عقب();
	flush();
}

خالی log_dump_val_کارکن(RTLIL::IdString v) {
	اگر (v == IdString::Null)
		لوگارتھم("(null)");
	ورنہ اگر (ID::is_static(v))
		لوگارتھم("%s", ID::unescaped_str(v));
	ورنہ اگر (yosys_design && yosys_design->twines().is_زندہ(v))
		لوگارتھم("%s", yosys_design->twines().unescaped_str(v));
	ورنہ
		لوگارتھم("%s", v.handle_token());
}

خالی log_dump_val_کارکن(RTLIL::SigSpec v) {
	لوگارتھم("%s", log_signal(v));
}

خالی log_dump_val_کارکن(RTLIL::State v) {
	لوگارتھم("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقی autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_منوال::Readable);
	واپس buf.str();
}

std::string log_const(ثابت RTLIL::Const &قدر, منطقی autoint)
{
	اگر ((قدر.flags & RTLIL::CONST_FLAG_STRING) == 0)
		واپس log_signal(قدر, autoint);

	واپس "\"" + قدر.decode_string() + "\"";
}

خالی log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, جھوٹ, سچ, جھوٹ, RTLIL_BACKEND::Dump_منوال::Readable);
	لوگارتھم("%s", buf.str());
}

خالی log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : خالی_اشاریہ, RTLIL_BACKEND::Dump_منوال::Readable);
	لوگارتھم("%s", buf.str());
}

خالی log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : خالی_اشاریہ, RTLIL_BACKEND::Dump_منوال::Readable);
	لوگارتھم("%s", buf.str());
}

خالی LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_شے> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_شے> expect_سابقہ_log_copy, expect_سابقہ_warning_copy, expect_سابقہ_error_copy;
	std::تبادلہ(expect_warning_copy, expect_warning);
	std::تبادلہ(expect_log_copy, expect_log);
	std::تبادلہ(expect_error_copy, expect_error);
	std::تبادلہ(expect_سابقہ_warning_copy, expect_سابقہ_warning);
	std::تبادلہ(expect_سابقہ_log_copy, expect_سابقہ_log);
	std::تبادلہ(expect_سابقہ_error_copy, expect_سابقہ_error);

	خودکار check = [&](ثابت std::string قسم, std::string نمونہ, Log_Expected_شے شے_2) {
		اگر (شے_2.current_گنتی == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", قسم, نمونہ);
		}
		اگر (شے_2.current_گنتی != شے_2.expected_گنتی) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				قسم.c_str(), نمونہ.c_str(), شے_2.current_گنتی, شے_2.expected_گنتی);
		}
	};

	برائے (خودکار &[نمونہ, شے_2] : expect_warning_copy)
		check("warning", نمونہ, شے_2);
	برائے (خودکار &[نمونہ, شے_2] : expect_سابقہ_warning_copy)
		check("prefixed warning", نمونہ, شے_2);
	برائے (خودکار &[نمونہ, شے_2] : expect_log_copy)
		check("log", نمونہ, شے_2);
	برائے (خودکار &[نمونہ, شے_2] : expect_سابقہ_log_copy)
		check("prefixed log", نمونہ, شے_2);

	خودکار check_err = [&](ثابت std::string قسم, std::string نمونہ, Log_Expected_شے شے_2) {
		اگر (شے_2.current_گنتی == شے_2.expected_گنتی) {
			warn_regexes.clear();
			لوگارتھم("Expected %s pattern '%s' found !!!\n", قسم, نمونہ);
			yosys_shutdown();
			#اگر defined(_MSC_VER)
				_exit(0);
			#ورنہ
				_Exit(0);
			#اگر_ختم
		} ورنہ {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", قسم, نمونہ);
		}
	};
	برائے (خودکار &[نمونہ, شے_2] : expect_error_copy)
		check_err("error", نمونہ, شے_2);
	برائے (خودکار &[نمونہ, شے_2] : expect_سابقہ_error_copy)
		check_err("prefixed error", نمونہ, شے_2);
}

خالی LogManager::report_unexpected_error()
{
	اگر (expect_no_warnings && warnings_گنتی_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_سائز(warnings),
					warnings_گنتی, warnings_گنتی - warnings_گنتی_noexpect);
}

خالی LogManager::جوڑو_expect(std::string type, std::string نمونہ, صحیح گنتی)
{
	اگر (type == "error")
		expect_error[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	ورنہ اگر (type == "prefix-error")
		expect_سابقہ_error[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	ورنہ اگر (type == "warning")
		expect_warning[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	ورنہ اگر (type == "prefix-warning")
		expect_سابقہ_warning[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	ورنہ اگر (type == "log")
		expect_log[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	ورنہ اگر (type == "prefix-log")
		expect_سابقہ_log[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	ورنہ log_abort();
}

خالی LogManager::آغاز_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::اختتام_hasher()
{
	اگر (!hasher)
		واپس {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	واپس hash;
}

YOSYS_NAMESPACE_انتہا
