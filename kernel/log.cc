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
#جے_انت

#إذا defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#جے_انت

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

std::chrono::steady_clock::ویلا_نقطہ LogManager::get_initial_ویلا() ثابت
{
	ساكن ثابت خودکار initial_ویلا = std::chrono::steady_clock::now();
	أرجع initial_ویلا;
}

فراغ (*log_error_atexit)() = خالی_سوچک;
فراغ (*log_verific_callback)(صحيح پیغام_type, ثابت حرف *سنیہا_id, Log_سروت_Location src, ثابت حرف *پیغام) = خالی_سوچک;

ساكن منطقي اگلا_print_log = سچ;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقي سطر_buffered, منطقي append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	إذا (!file)
		ارم std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	إذا (سطر_buffered)
		setvbuf(file, خالی_سوچک, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	إذا (file)
		fclose(file);
}

فراغ FileLogSink::لوگارتھم(ثابت Log_سنیہا &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), file);
}

فراغ FileLogSink::flush()
{
	fflush(file);
}

فراغ ConsoleLogSink::لوگارتھم(ثابت Log_سنیہا &پیغام)
{
	FILE *file = (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(پیغام.cached_پیغام.c_str(), file);
}

فراغ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي StderrLogSink::should_log(ثابت Log_سنیہا &پیغام) ثابت
{
	أرجع پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

فراغ StderrLogSink::لوگارتھم(ثابت Log_سنیہا &پیغام)
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

فراغ ScratchPadLogSink::لوگارتھم(ثابت Log_سنیہا &پیغام)
{
	RTLIL::Design *design = yosys_get_design();
	إذا (!design)
		أرجع;

	design->scratchpad[scratchpad].append(پیغام.cached_پیغام);
}

Log_سنیہا::Log_سنیہا(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string_view سنیہا) :
	severity(severity),
	اگیتر_2(اگیتر_2),
	format(format),
	سنیہا(سنیہا),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string ویلا_str;
	إذا (logger().get_log_ویلا())
	{
		إذا (اگلا_print_log) {
			اگلا_print_log = خطأ;
			خودکار elapsed = std::chrono::steady_clock::now() - logger().get_initial_ویلا();
			خودکار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			ویلا_str += stringf("[%05d.%06d] ", صحيح(us.گنتی() / 1'000'000),صحيح(us.گنتی() % 1'000'000));
		}

		إذا (!format.خالی_تھاواں() && format.پٹھ() == '\n')
			اگلا_print_log = سچ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		إذا (format == "%s" && !سنیہا.خالی_تھاواں() && سنیہا.پٹھ() == '\n')
			اگلا_print_log = سچ;
	}
	std::string loc = !src.filename.خالی_تھاواں() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	cached_پیغام = stringf("%s%s%s%s", ویلا_str, loc, اگیتر_2, سنیہا);
}

فراغ LogManager::logv_string(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string str_وچ) {
	ناپ_t remove_leading = 0;
	طالما (format.ناپ() > 1 && format[0] == '\n') {
		logv_string(severity, src, اگیتر_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	إذا (remove_leading > 0) {
		str_وچ = str_وچ.substr(remove_leading);
	}

	std::string loc = !src.filename.خالی_تھاواں() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	std::string str = stringf("%s%s%s",loc,اگیتر_2,str_وچ);

	إذا (str.خالی_تھاواں())
		أرجع;

	ناپ_t nnl_pos = str.لبھو_آخری_not_دا('\n');
	إذا (nnl_pos == std::string::npos)
		newline_گنتی += Get_ناپ(str);
	وإلا
		newline_گنتی = Get_ناپ(str) - nnl_pos - 1;

	إذا (hasher)
		hasher->update(str);

	خودکار پیغام = Log_سنیہا(severity, src, اگیتر_2, format, str_وچ);
	لكل (خودکار &sink : sinks) {
		إذا (sink->should_log(پیغام))
			sink->لوگارتھم(پیغام);
	}

	إذا (severity == LogSeverity::سرخی)
		str = str_وچ;

	ساكن std::string linebuffer;
	ساكن منطقي warn_regex_recusion_guard = خطأ;

	إذا (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = سچ;

		إذا (warn_regexes.خالی_تھاواں() && expect_log.خالی_تھاواں() && expect_اگیتر_log.خالی_تھاواں())
		{
			linebuffer.clear();
		}
		وإلا
		{
			linebuffer += str;

			إذا (!linebuffer.خالی_تھاواں() && linebuffer.پٹھ() == '\n') {
				لكل (خودکار &re : warn_regexes)
					إذا (std::regex_تلاش(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				لكل (خودکار &[_, شے] : expect_log)
					إذا (std::regex_تلاش(linebuffer, شے.نمونہ))
						شے.current_گنتی++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = خطأ;
	}
}

فراغ LogManager::formatted_string(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::سرگرم());

	إذا (make_debug && !is_debug(1))
		أرجع;
	logv_string(severity, src, اگیتر_2, format, std::move(str));
}

فراغ LogManager::formatted_سرخی(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::سرگرم());

	spacer();
	إذا (سرخی_گنتی.ناپ() > 0)
		سرخی_گنتی.پٹھ()++;

	إذا (صحيح(سرخی_گنتی.ناپ()) <= verbose_level) {
		log_stderr_sink_forced = سچ;
	}

	std::string سرخی_id;

	لكل (صحيح c : سرخی_گنتی)
		سرخی_id += stringf("%s%d", سرخی_id.خالی_تھاواں() ? "" : ".", c);

	formatted_string(LogSeverity::سرخی, Log_سروت_Location{}, stringf("%s. ", سرخی_id), format, std::move(str));
	flush();

	إذا (hdump_all)
		hdump[سرخی_id].داخل_کرو("yosys_dump_" + سرخی_id + ".il");

	إذا (hdump.گنتی(سرخی_id) && design != خالی_سوچک)
		لكل (خودکار &filename : hdump.at(سرخی_id)) {
			لوگارتھم("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = خطأ;
}

فراغ LogManager::formatted_warning(Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا)
{
	log_assert(!Multithreading::سرگرم());

	منطقي suppressed = خطأ;

	لكل (خودکار &re : nowarn_regexes)
		إذا (std::regex_تلاش(سنیہا, re))
			suppressed = سچ;

	std::string loc = !src.filename.خالی_تھاواں() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	إذا (suppressed)
	{
		لوگارتھم("Suppressed %s%s%s", loc, اگیتر_2, سنیہا);
	}
	وإلا
	{
		صحيح bak_make_debug = make_debug;
		make_debug = 0;

		لكل (خودکار &re : werror_regexes)
			إذا (std::regex_تلاش(سنیہا, re))
				formatted_error(src, "ERROR: ", format, سنیہا);

		منطقي warning_میل = خطأ;
		لكل (خودکار &[_, شے] : expect_warning)
			إذا (std::regex_تلاش(سنیہا, شے.نمونہ)) {
				شے.current_گنتی++;
				warning_میل = سچ;
			}

		std::string نمونہ = loc + string(اگیتر_2) + سنیہا;
		لكل (خودکار &[_, شے] : expect_اگیتر_warning)
			إذا (std::regex_تلاش(نمونہ, شے.نمونہ)) {
				شے.current_گنتی++;
				warning_میل = سچ;
			}

		إذا (warnings.گنتی(سنیہا))
		{
			formatted_string(LogSeverity::Info, src, اگیتر_2, format, سنیہا);
			flush();
		}
		وإلا
		{
			formatted_string(LogSeverity::Warning, src, اگیتر_2, format, سنیہا);
			flush();
			warnings.داخل_کرو(سنیہا);
		}

		إذا (!warning_میل)
			warnings_گنتی_noexpect++;
		warnings_گنتی++;
		make_debug = bak_make_debug;
	}
}

فراغ LogManager::suppressed() {
	إذا (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_سروت_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

فراغ LogManager::error_with_اگیتر(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا)
{
	صحيح bak_make_debug = make_debug;
	make_debug = 0;
	إذا (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, اگیتر_2, format, سنیہا);
	flush();

	make_debug = bak_make_debug;

	لكل (خودکار &[_, شے] : expect_error)
		إذا (std::regex_تلاش(سنیہا, شے.نمونہ))
			شے.current_گنتی++;

	std::string loc = !src.filename.خالی_تھاواں() ? stringf("%s:%d: ", src.filename, src.شروع_سطر) : "";
	std::string نمونہ = loc + string(اگیتر_2) + سنیہا;
	لكل (خودکار &[_, شے] : expect_اگیتر_error)
		إذا (std::regex_تلاش(نمونہ, شے.نمونہ))
			شے.current_گنتی++;

	errors_گنتی++;
}

[[noreturn]]
فراغ LogManager::formatted_error(Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا)
{
	error_with_اگیتر(LogSeverity::Error, src, اگیتر_2, format, سنیہا);

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
#جے_انت
}

فراغ LogManager::formatted_nonfatal_error(Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا)
{
	error_with_اگیتر(LogSeverity::NonFatalError, src, اگیتر_2, format, سنیہا);
}

فراغ LogManager::جوڑو_experimental(ثابت std::string &str)
{
	إذا (experimental_ignored.گنتی(str) == 0 && experimental.گنتی(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.داخل_کرو(str);
	}
}

فراغ LogManager::جوڑو_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	إذا (deprecated.گنتی(feature) == 0) {
		إذا (earliest_removal.خالی_تھاواں())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وإلا
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

فراغ log_assert_failure(ثابت حرف *اظہاریہ, سروت_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", اظہاریہ, location.file_name(), location.سطر());
}

فراغ log_abort_اندرونی(ثابت حرف *file, صحيح سطر)
{
	log_error("Abort in %s:%d.\n", file, سطر);
}

فراغ log_yosys_abort_سنیہا(std::string_view file, صحيح سطر, std::string_view func, std::string_view سنیہا)
{
	log_error("Abort in %s:%d (%s): %s\n", file, سطر, func, سنیہا);
}

فراغ LogManager::formatted_cmd_error(std::string_view format, std::string سنیہا)
{
	إذا (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_سروت_Location{}, "ERROR: ", format, سنیہا);
		flush();

		ارم log_cmd_error_exception();
	}

	formatted_error(Log_سروت_Location{}, "ERROR: ", format, سنیہا);
}

فراغ LogManager::spacer()
{
	إذا (newline_گنتی < 2) لوگارتھم("\n");
	إذا (newline_گنتی < 2) لوگارتھم("\n");
}

فراغ LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت خودکار &sink) { أرجع dynamic_cast<LogSinkRef *>(sink.get()) != خالی_سوچک; });
}

فراغ LogManager::ڈالو()
{
	سرخی_گنتی.ڈالو_پٹھ(0);
}

فراغ LogManager::نکالو()
{
	سرخی_گنتی.نکالو_پٹھ();
	flush();
}

#إذا defined(YOSYS_ENABLE_DLOPEN)
فراغ log_backtrace(ثابت حرف *اگیتر_2, صحيح پدھراں)
{
	إذا (پدھراں <= 0) أرجع;

	Dl_info dli;
	فراغ *p;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #1: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #1: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 1) أرجع;

#جے_بنا_تعریف DEBUG
	لوگارتھم("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", اگیتر_2);
#وإلا
	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #2: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #2: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 2) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #3: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #3: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 3) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #4: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #4: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 4) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #5: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #5: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 5) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #6: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #6: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 6) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #7: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #7: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 7) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #8: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #8: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 8) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		لوگارتھم("%sframe #9: %p %s(%p) %s(%p)\n", اگیتر_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		لوگارتھم("%sframe #9: ---\n", اگیتر_2);
		أرجع;
	}

	إذا (پدھراں <= 9) أرجع;
#جے_انت
}
#وإلا
فراغ log_backtrace(ثابت حرف*, صحيح) { }
#جے_انت

فراغ LogManager::reset_ڈھیر()
{
	طالما (سرخی_گنتی.ناپ() > 1)
		سرخی_گنتی.نکالو_پٹھ();
	flush();
}

فراغ log_dump_val_کامہ(RTLIL::IdString v) {
	إذا (v == IdString::Null)
		لوگارتھم("(null)");
	وإلا إذا (ID::is_static(v))
		لوگارتھم("%s", ID::unescaped_str(v));
	وإلا إذا (yosys_design && yosys_design->twines().is_جیوندا(v))
		لوگارتھم("%s", yosys_design->twines().unescaped_str(v));
	وإلا
		لوگارتھم("%s", v.handle_token());
}

فراغ log_dump_val_کامہ(RTLIL::SigSpec v) {
	لوگارتھم("%s", log_signal(v));
}

فراغ log_dump_val_کامہ(RTLIL::State v) {
	لوگارتھم("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقي autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_منوال::Readable);
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
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, خطأ, سچ, خطأ, RTLIL_BACKEND::Dump_منوال::Readable);
	لوگارتھم("%s", buf.str());
}

فراغ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : خالی_سوچک, RTLIL_BACKEND::Dump_منوال::Readable);
	لوگارتھم("%s", buf.str());
}

فراغ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : خالی_سوچک, RTLIL_BACKEND::Dump_منوال::Readable);
	لوگارتھم("%s", buf.str());
}

فراغ LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_شے> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_شے> expect_اگیتر_log_copy, expect_اگیتر_warning_copy, expect_اگیتر_error_copy;
	std::وٹاندرا(expect_warning_copy, expect_warning);
	std::وٹاندرا(expect_log_copy, expect_log);
	std::وٹاندرا(expect_error_copy, expect_error);
	std::وٹاندرا(expect_اگیتر_warning_copy, expect_اگیتر_warning);
	std::وٹاندرا(expect_اگیتر_log_copy, expect_اگیتر_log);
	std::وٹاندرا(expect_اگیتر_error_copy, expect_اگیتر_error);

	خودکار check = [&](ثابت std::string قسم, std::string نمونہ, Log_Expected_شے شے) {
		إذا (شے.current_گنتی == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", قسم, نمونہ);
		}
		إذا (شے.current_گنتی != شے.expected_گنتی) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				قسم.c_str(), نمونہ.c_str(), شے.current_گنتی, شے.expected_گنتی);
		}
	};

	لكل (خودکار &[نمونہ, شے] : expect_warning_copy)
		check("warning", نمونہ, شے);
	لكل (خودکار &[نمونہ, شے] : expect_اگیتر_warning_copy)
		check("prefixed warning", نمونہ, شے);
	لكل (خودکار &[نمونہ, شے] : expect_log_copy)
		check("log", نمونہ, شے);
	لكل (خودکار &[نمونہ, شے] : expect_اگیتر_log_copy)
		check("prefixed log", نمونہ, شے);

	خودکار check_err = [&](ثابت std::string قسم, std::string نمونہ, Log_Expected_شے شے) {
		إذا (شے.current_گنتی == شے.expected_گنتی) {
			warn_regexes.clear();
			لوگارتھم("Expected %s pattern '%s' found !!!\n", قسم, نمونہ);
			yosys_shutdown();
			#إذا defined(_MSC_VER)
				_exit(0);
			#وإلا
				_Exit(0);
			#جے_انت
		} وإلا {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", قسم, نمونہ);
		}
	};
	لكل (خودکار &[نمونہ, شے] : expect_error_copy)
		check_err("error", نمونہ, شے);
	لكل (خودکار &[نمونہ, شے] : expect_اگیتر_error_copy)
		check_err("prefixed error", نمونہ, شے);
}

فراغ LogManager::report_unexpected_error()
{
	إذا (expect_no_warnings && warnings_گنتی_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_ناپ(warnings),
					warnings_گنتی, warnings_گنتی - warnings_گنتی_noexpect);
}

فراغ LogManager::جوڑو_expect(std::string type, std::string نمونہ, صحيح گنتی)
{
	إذا (type == "error")
		expect_error[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	وإلا إذا (type == "prefix-error")
		expect_اگیتر_error[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	وإلا إذا (type == "warning")
		expect_warning[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	وإلا إذا (type == "prefix-warning")
		expect_اگیتر_warning[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	وإلا إذا (type == "log")
		expect_log[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	وإلا إذا (type == "prefix-log")
		expect_اگیتر_log[نمونہ] = Log_Expected_شے(YS_REGEX_COMPILE(نمونہ), گنتی);
	وإلا log_abort();
}

فراغ LogManager::شروع_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::انت_hasher()
{
	إذا (!hasher)
		أرجع {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	أرجع hash;
}

YOSYS_NAMESPACE_سرا
