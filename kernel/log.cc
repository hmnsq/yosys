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
#پایان_اگر

#إذا defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#پایان_اگر

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

std::chrono::steady_clock::زمان_نقطه LogManager::get_initial_زمان() ثابت
{
	ساكن ثابت خودکار initial_زمان = std::chrono::steady_clock::now();
	أرجع initial_زمان;
}

فراغ (*log_error_atexit)() = اشاره_تهی;
فراغ (*log_verific_callback)(صحيح پیام_type, ثابت حرف *پیام_id, Log_منبع_Location src, ثابت حرف *پیام) = اشاره_تهی;

ساكن منطقي بعدی_print_log = درست;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقي سطر_buffered, منطقي append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	إذا (!file)
		ارم std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	إذا (سطر_buffered)
		setvbuf(file, اشاره_تهی, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	إذا (file)
		fclose(file);
}

فراغ FileLogSink::لگاریتم(ثابت Log_پیام &پیام)
{
	fputs(پیام.cached_پیام.c_str(), file);
}

فراغ FileLogSink::flush()
{
	fflush(file);
}

فراغ ConsoleLogSink::لگاریتم(ثابت Log_پیام &پیام)
{
	FILE *file = (پیام.severity == LogSeverity::Error || پیام.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(پیام.cached_پیام.c_str(), file);
}

فراغ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي StderrLogSink::should_log(ثابت Log_پیام &پیام) ثابت
{
	أرجع پیام.severity == LogSeverity::Error || پیام.severity == LogSeverity::NonFatalError ||
			(پیام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

فراغ StderrLogSink::لگاریتم(ثابت Log_پیام &پیام)
{
	fputs(پیام.cached_پیام.c_str(), stderr);
}

فراغ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

فراغ ScratchPadLogSink::لگاریتم(ثابت Log_پیام &پیام)
{
	RTLIL::Design *design = yosys_get_design();
	إذا (!design)
		أرجع;

	design->scratchpad[scratchpad].append(پیام.cached_پیام);
}

Log_پیام::Log_پیام(LogSeverity severity, Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string_view پیام_2) :
	severity(severity),
	پیشوند_2(پیشوند_2),
	format(format),
	پیام_2(پیام_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string زمان_str;
	إذا (logger().get_log_زمان())
	{
		إذا (بعدی_print_log) {
			بعدی_print_log = خطأ;
			خودکار elapsed = std::chrono::steady_clock::now() - logger().get_initial_زمان();
			خودکار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			زمان_str += stringf("[%05d.%06d] ", صحيح(us.شمار() / 1'000'000),صحيح(us.شمار() % 1'000'000));
		}

		إذا (!format.خالی() && format.پشت() == '\n')
			بعدی_print_log = درست;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		إذا (format == "%s" && !پیام_2.خالی() && پیام_2.پشت() == '\n')
			بعدی_print_log = درست;
	}
	std::string loc = !src.filename.خالی() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	cached_پیام = stringf("%s%s%s%s", زمان_str, loc, پیشوند_2, پیام_2);
}

فراغ LogManager::logv_string(LogSeverity severity, Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string str_درون) {
	اندازه_t remove_leading = 0;
	طالما (format.اندازه() > 1 && format[0] == '\n') {
		logv_string(severity, src, پیشوند_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	إذا (remove_leading > 0) {
		str_درون = str_درون.substr(remove_leading);
	}

	std::string loc = !src.filename.خالی() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	std::string str = stringf("%s%s%s",loc,پیشوند_2,str_درون);

	إذا (str.خالی())
		أرجع;

	اندازه_t nnl_pos = str.پیدا_کن_آخرین_not_از('\n');
	إذا (nnl_pos == std::string::npos)
		newline_شمار += Get_اندازه(str);
	وإلا
		newline_شمار = Get_اندازه(str) - nnl_pos - 1;

	إذا (hasher)
		hasher->update(str);

	خودکار پیام = Log_پیام(severity, src, پیشوند_2, format, str_درون);
	لكل (خودکار &sink : sinks) {
		إذا (sink->should_log(پیام))
			sink->لگاریتم(پیام);
	}

	إذا (severity == LogSeverity::سرآیند)
		str = str_درون;

	ساكن std::string linebuffer;
	ساكن منطقي warn_regex_recusion_guard = خطأ;

	إذا (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = درست;

		إذا (warn_regexes.خالی() && expect_log.خالی() && expect_پیشوند_log.خالی())
		{
			linebuffer.clear();
		}
		وإلا
		{
			linebuffer += str;

			إذا (!linebuffer.خالی() && linebuffer.پشت() == '\n') {
				لكل (خودکار &re : warn_regexes)
					إذا (std::regex_جستجو(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				لكل (خودکار &[_, قلم] : expect_log)
					إذا (std::regex_جستجو(linebuffer, قلم.الگو))
						قلم.current_شمار++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = خطأ;
	}
}

فراغ LogManager::formatted_string(LogSeverity severity, Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::فعال());

	إذا (make_debug && !is_debug(1))
		أرجع;
	logv_string(severity, src, پیشوند_2, format, std::move(str));
}

فراغ LogManager::formatted_سرآیند(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::فعال());

	spacer();
	إذا (سرآیند_شمار.اندازه() > 0)
		سرآیند_شمار.پشت()++;

	إذا (صحيح(سرآیند_شمار.اندازه()) <= verbose_level) {
		log_stderr_sink_forced = درست;
	}

	std::string سرآیند_id;

	لكل (صحيح c : سرآیند_شمار)
		سرآیند_id += stringf("%s%d", سرآیند_id.خالی() ? "" : ".", c);

	formatted_string(LogSeverity::سرآیند, Log_منبع_Location{}, stringf("%s. ", سرآیند_id), format, std::move(str));
	flush();

	إذا (hdump_all)
		hdump[سرآیند_id].درج("yosys_dump_" + سرآیند_id + ".il");

	إذا (hdump.شمار(سرآیند_id) && design != اشاره_تهی)
		لكل (خودکار &filename : hdump.at(سرآیند_id)) {
			لگاریتم("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = خطأ;
}

فراغ LogManager::formatted_warning(Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string پیام_2)
{
	log_assert(!Multithreading::فعال());

	منطقي suppressed = خطأ;

	لكل (خودکار &re : nowarn_regexes)
		إذا (std::regex_جستجو(پیام_2, re))
			suppressed = درست;

	std::string loc = !src.filename.خالی() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	إذا (suppressed)
	{
		لگاریتم("Suppressed %s%s%s", loc, پیشوند_2, پیام_2);
	}
	وإلا
	{
		صحيح bak_make_debug = make_debug;
		make_debug = 0;

		لكل (خودکار &re : werror_regexes)
			إذا (std::regex_جستجو(پیام_2, re))
				formatted_error(src, "ERROR: ", format, پیام_2);

		منطقي warning_تطابق = خطأ;
		لكل (خودکار &[_, قلم] : expect_warning)
			إذا (std::regex_جستجو(پیام_2, قلم.الگو)) {
				قلم.current_شمار++;
				warning_تطابق = درست;
			}

		std::string الگو = loc + string(پیشوند_2) + پیام_2;
		لكل (خودکار &[_, قلم] : expect_پیشوند_warning)
			إذا (std::regex_جستجو(الگو, قلم.الگو)) {
				قلم.current_شمار++;
				warning_تطابق = درست;
			}

		إذا (warnings.شمار(پیام_2))
		{
			formatted_string(LogSeverity::Info, src, پیشوند_2, format, پیام_2);
			flush();
		}
		وإلا
		{
			formatted_string(LogSeverity::Warning, src, پیشوند_2, format, پیام_2);
			flush();
			warnings.درج(پیام_2);
		}

		إذا (!warning_تطابق)
			warnings_شمار_noexpect++;
		warnings_شمار++;
		make_debug = bak_make_debug;
	}
}

فراغ LogManager::suppressed() {
	إذا (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_منبع_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

فراغ LogManager::error_with_پیشوند(LogSeverity severity, Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string پیام_2)
{
	صحيح bak_make_debug = make_debug;
	make_debug = 0;
	إذا (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, پیشوند_2, format, پیام_2);
	flush();

	make_debug = bak_make_debug;

	لكل (خودکار &[_, قلم] : expect_error)
		إذا (std::regex_جستجو(پیام_2, قلم.الگو))
			قلم.current_شمار++;

	std::string loc = !src.filename.خالی() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	std::string الگو = loc + string(پیشوند_2) + پیام_2;
	لكل (خودکار &[_, قلم] : expect_پیشوند_error)
		إذا (std::regex_جستجو(الگو, قلم.الگو))
			قلم.current_شمار++;

	errors_شمار++;
}

[[noreturn]]
فراغ LogManager::formatted_error(Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string پیام_2)
{
	error_with_پیشوند(LogSeverity::Error, src, پیشوند_2, format, پیام_2);

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
#پایان_اگر
}

فراغ LogManager::formatted_nonfatal_error(Log_منبع_Location src, std::string_view پیشوند_2, std::string_view format, std::string پیام_2)
{
	error_with_پیشوند(LogSeverity::NonFatalError, src, پیشوند_2, format, پیام_2);
}

فراغ LogManager::جمع_کن_experimental(ثابت std::string &str)
{
	إذا (experimental_ignored.شمار(str) == 0 && experimental.شمار(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.درج(str);
	}
}

فراغ LogManager::جمع_کن_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	إذا (deprecated.شمار(feature) == 0) {
		إذا (earliest_removal.خالی())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وإلا
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

فراغ log_assert_failure(ثابت حرف *عبارت, منبع_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", عبارت, location.file_name(), location.سطر());
}

فراغ log_abort_داخلی(ثابت حرف *file, صحيح سطر)
{
	log_error("Abort in %s:%d.\n", file, سطر);
}

فراغ log_yosys_abort_پیام(std::string_view file, صحيح سطر, std::string_view func, std::string_view پیام_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, سطر, func, پیام_2);
}

فراغ LogManager::formatted_cmd_error(std::string_view format, std::string پیام_2)
{
	إذا (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_منبع_Location{}, "ERROR: ", format, پیام_2);
		flush();

		ارم log_cmd_error_exception();
	}

	formatted_error(Log_منبع_Location{}, "ERROR: ", format, پیام_2);
}

فراغ LogManager::spacer()
{
	إذا (newline_شمار < 2) لگاریتم("\n");
	إذا (newline_شمار < 2) لگاریتم("\n");
}

فراغ LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت خودکار &sink) { أرجع dynamic_cast<LogSinkRef *>(sink.get()) != اشاره_تهی; });
}

فراغ LogManager::افزودن()
{
	سرآیند_شمار.افزودن_پشت(0);
}

فراغ LogManager::برداشت()
{
	سرآیند_شمار.برداشت_پشت();
	flush();
}

#إذا defined(YOSYS_ENABLE_DLOPEN)
فراغ log_backtrace(ثابت حرف *پیشوند_2, صحيح سطوح)
{
	إذا (سطوح <= 0) أرجع;

	Dl_info dli;
	فراغ *p;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #1: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #1: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 1) أرجع;

#اگر_تعریف_نشده DEBUG
	لگاریتم("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", پیشوند_2);
#وإلا
	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #2: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #2: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 2) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #3: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #3: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 3) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #4: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #4: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 4) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #5: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #5: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 5) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #6: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #6: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 6) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #7: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #7: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 7) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #8: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #8: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 8) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		لگاریتم("%sframe #9: %p %s(%p) %s(%p)\n", پیشوند_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لگاریتم("%sframe #9: ---\n", پیشوند_2);
		أرجع;
	}

	إذا (سطوح <= 9) أرجع;
#پایان_اگر
}
#وإلا
فراغ log_backtrace(ثابت حرف*, صحيح) { }
#پایان_اگر

فراغ LogManager::reset_پشته()
{
	طالما (سرآیند_شمار.اندازه() > 1)
		سرآیند_شمار.برداشت_پشت();
	flush();
}

فراغ log_dump_val_کارگر(RTLIL::IdString v) {
	إذا (v == IdString::Null)
		لگاریتم("(null)");
	وإلا إذا (ID::is_static(v))
		لگاریتم("%s", ID::unescaped_str(v));
	وإلا إذا (yosys_design && yosys_design->twines().is_زنده(v))
		لگاریتم("%s", yosys_design->twines().unescaped_str(v));
	وإلا
		لگاریتم("%s", v.handle_token());
}

فراغ log_dump_val_کارگر(RTLIL::SigSpec v) {
	لگاریتم("%s", log_signal(v));
}

فراغ log_dump_val_کارگر(RTLIL::State v) {
	لگاریتم("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقي autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_نما::Readable);
	أرجع buf.str();
}

std::string log_const(ثابت RTLIL::Const &مقدار, منطقي autoint)
{
	إذا ((مقدار.flags & RTLIL::CONST_FLAG_STRING) == 0)
		أرجع log_signal(مقدار, autoint);

	أرجع "\"" + مقدار.decode_string() + "\"";
}

فراغ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, خطأ, درست, خطأ, RTLIL_BACKEND::Dump_نما::Readable);
	لگاریتم("%s", buf.str());
}

فراغ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : اشاره_تهی, RTLIL_BACKEND::Dump_نما::Readable);
	لگاریتم("%s", buf.str());
}

فراغ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : اشاره_تهی, RTLIL_BACKEND::Dump_نما::Readable);
	لگاریتم("%s", buf.str());
}

فراغ LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_قلم> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_قلم> expect_پیشوند_log_copy, expect_پیشوند_warning_copy, expect_پیشوند_error_copy;
	std::جابجا(expect_warning_copy, expect_warning);
	std::جابجا(expect_log_copy, expect_log);
	std::جابجا(expect_error_copy, expect_error);
	std::جابجا(expect_پیشوند_warning_copy, expect_پیشوند_warning);
	std::جابجا(expect_پیشوند_log_copy, expect_پیشوند_log);
	std::جابجا(expect_پیشوند_error_copy, expect_پیشوند_error);

	خودکار check = [&](ثابت std::string نوع, std::string الگو, Log_Expected_قلم قلم) {
		إذا (قلم.current_شمار == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", نوع, الگو);
		}
		إذا (قلم.current_شمار != قلم.expected_شمار) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				نوع.c_str(), الگو.c_str(), قلم.current_شمار, قلم.expected_شمار);
		}
	};

	لكل (خودکار &[الگو, قلم] : expect_warning_copy)
		check("warning", الگو, قلم);
	لكل (خودکار &[الگو, قلم] : expect_پیشوند_warning_copy)
		check("prefixed warning", الگو, قلم);
	لكل (خودکار &[الگو, قلم] : expect_log_copy)
		check("log", الگو, قلم);
	لكل (خودکار &[الگو, قلم] : expect_پیشوند_log_copy)
		check("prefixed log", الگو, قلم);

	خودکار check_err = [&](ثابت std::string نوع, std::string الگو, Log_Expected_قلم قلم) {
		إذا (قلم.current_شمار == قلم.expected_شمار) {
			warn_regexes.clear();
			لگاریتم("Expected %s pattern '%s' found !!!\n", نوع, الگو);
			yosys_shutdown();
			#إذا defined(_MSC_VER)
				_exit(0);
			#وإلا
				_Exit(0);
			#پایان_اگر
		} وإلا {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", نوع, الگو);
		}
	};
	لكل (خودکار &[الگو, قلم] : expect_error_copy)
		check_err("error", الگو, قلم);
	لكل (خودکار &[الگو, قلم] : expect_پیشوند_error_copy)
		check_err("prefixed error", الگو, قلم);
}

فراغ LogManager::report_unexpected_error()
{
	إذا (expect_no_warnings && warnings_شمار_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_اندازه(warnings),
					warnings_شمار, warnings_شمار - warnings_شمار_noexpect);
}

فراغ LogManager::جمع_کن_expect(std::string type, std::string الگو, صحيح شمار)
{
	إذا (type == "error")
		expect_error[الگو] = Log_Expected_قلم(YS_REGEX_COMPILE(الگو), شمار);
	وإلا إذا (type == "prefix-error")
		expect_پیشوند_error[الگو] = Log_Expected_قلم(YS_REGEX_COMPILE(الگو), شمار);
	وإلا إذا (type == "warning")
		expect_warning[الگو] = Log_Expected_قلم(YS_REGEX_COMPILE(الگو), شمار);
	وإلا إذا (type == "prefix-warning")
		expect_پیشوند_warning[الگو] = Log_Expected_قلم(YS_REGEX_COMPILE(الگو), شمار);
	وإلا إذا (type == "log")
		expect_log[الگو] = Log_Expected_قلم(YS_REGEX_COMPILE(الگو), شمار);
	وإلا إذا (type == "prefix-log")
		expect_پیشوند_log[الگو] = Log_Expected_قلم(YS_REGEX_COMPILE(الگو), شمار);
	وإلا log_abort();
}

فراغ LogManager::شروع_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::پایان_hasher()
{
	إذا (!hasher)
		أرجع {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	أرجع hash;
}

YOSYS_NAMESPACE_انتها
