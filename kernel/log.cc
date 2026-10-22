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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "libs/sha1/sha1.h"
#అన్తర్భావ "backends/rtlil/rtlil_backend.h"

#ఒకవేళ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#అయితే_ముగింపు

#ఒకవేళ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#అయితే_ముగింపు

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <vector>
#అన్తర్భావ <list>

YOSYS_NAMESPACE_BEGIN

తార్కిక log_stderr_sink_forced = అబద్ధం;

LogManager &logger()
{
	స్థిర LogManager instance;
	తిరిగి instance;
}

std::chrono::steady_clock::సమయం_బిందువు LogManager::get_initial_సమయం() స్థిరరాశి
{
	స్థిర స్థిరరాశి స్వయంచాలక initial_సమయం = std::chrono::steady_clock::now();
	తిరిగి initial_సమయం;
}

శూన్య (*log_error_atexit)() = శూన్యసూచిక;
శూన్య (*log_verific_callback)(పూర్ణాంకం సందేశం_type, స్థిరరాశి అక్షరం *సందేశం_id, Log_ఆధారం_Location src, స్థిరరాశి అక్షరం *సందేశం) = శూన్యసూచిక;

స్థిర తార్కిక తదుపరి_print_log = నిజం;

FileLogSink::FileLogSink(స్థిరరాశి std::string &filename, తార్కిక పంక్తి_buffered, తార్కిక append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	ఒకవేళ (!file)
		విసురు std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	ఒకవేళ (పంక్తి_buffered)
		setvbuf(file, శూన్యసూచిక, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	ఒకవేళ (file)
		fclose(file);
}

శూన్య FileLogSink::సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం)
{
	fputs(సందేశం.cached_సందేశం.c_str(), file);
}

శూన్య FileLogSink::flush()
{
	fflush(file);
}

శూన్య ConsoleLogSink::సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం)
{
	FILE *file = (సందేశం.severity == LogSeverity::Error || సందేశం.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(సందేశం.cached_సందేశం.c_str(), file);
}

శూన్య ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

తార్కిక StderrLogSink::should_log(స్థిరరాశి Log_సందేశం &సందేశం) స్థిరరాశి
{
	తిరిగి సందేశం.severity == LogSeverity::Error || సందేశం.severity == LogSeverity::NonFatalError ||
			(సందేశం.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

శూన్య StderrLogSink::సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం)
{
	fputs(సందేశం.cached_సందేశం.c_str(), stderr);
}

శూన్య StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

శూన్య ScratchPadLogSink::సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం)
{
	RTLIL::Design *design = yosys_get_design();
	ఒకవేళ (!design)
		తిరిగి;

	design->scratchpad[scratchpad].append(సందేశం.cached_సందేశం);
}

Log_సందేశం::Log_సందేశం(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string_view సందేశం_2) :
	severity(severity),
	ఉపసర్గ_2(ఉపసర్గ_2),
	format(format),
	సందేశం_2(సందేశం_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string సమయం_str;
	ఒకవేళ (logger().get_log_సమయం())
	{
		ఒకవేళ (తదుపరి_print_log) {
			తదుపరి_print_log = అబద్ధం;
			స్వయంచాలక elapsed = std::chrono::steady_clock::now() - logger().get_initial_సమయం();
			స్వయంచాలక us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			సమయం_str += stringf("[%05d.%06d] ", పూర్ణాంకం(us.లెక్క() / 1'000'000),పూర్ణాంకం(us.లెక్క() % 1'000'000));
		}

		ఒకవేళ (!format.ఖాళీలు() && format.వెనుక() == '\n')
			తదుపరి_print_log = నిజం;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		ఒకవేళ (format == "%s" && !సందేశం_2.ఖాళీలు() && సందేశం_2.వెనుక() == '\n')
			తదుపరి_print_log = నిజం;
	}
	std::string loc = !src.filename.ఖాళీలు() ? stringf("%s:%d: ", src.filename, src.ప్రారంభం_పంక్తి) : "";
	cached_సందేశం = stringf("%s%s%s%s", సమయం_str, loc, ఉపసర్గ_2, సందేశం_2);
}

శూన్య LogManager::logv_string(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string str_లోపల) {
	కొలత_t remove_leading = 0;
	అయితే (format.కొలత() > 1 && format[0] == '\n') {
		logv_string(severity, src, ఉపసర్గ_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	ఒకవేళ (remove_leading > 0) {
		str_లోపల = str_లోపల.substr(remove_leading);
	}

	std::string loc = !src.filename.ఖాళీలు() ? stringf("%s:%d: ", src.filename, src.ప్రారంభం_పంక్తి) : "";
	std::string str = stringf("%s%s%s",loc,ఉపసర్గ_2,str_లోపల);

	ఒకవేళ (str.ఖాళీలు())
		తిరిగి;

	కొలత_t nnl_pos = str.కనుగొను_చివరి_not_యొక్క('\n');
	ఒకవేళ (nnl_pos == std::string::npos)
		newline_లెక్క += Get_కొలత(str);
	లేకపోతే
		newline_లెక్క = Get_కొలత(str) - nnl_pos - 1;

	ఒకవేళ (hasher)
		hasher->update(str);

	స్వయంచాలక సందేశం = Log_సందేశం(severity, src, ఉపసర్గ_2, format, str_లోపల);
	కొరకు (స్వయంచాలక &sink : sinks) {
		ఒకవేళ (sink->should_log(సందేశం))
			sink->సంవర్గమానం(సందేశం);
	}

	ఒకవేళ (severity == LogSeverity::శీర్షిక)
		str = str_లోపల;

	స్థిర std::string linebuffer;
	స్థిర తార్కిక warn_regex_recusion_guard = అబద్ధం;

	ఒకవేళ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = నిజం;

		ఒకవేళ (warn_regexes.ఖాళీలు() && expect_log.ఖాళీలు() && expect_ఉపసర్గ_log.ఖాళీలు())
		{
			linebuffer.clear();
		}
		లేకపోతే
		{
			linebuffer += str;

			ఒకవేళ (!linebuffer.ఖాళీలు() && linebuffer.వెనుక() == '\n') {
				కొరకు (స్వయంచాలక &re : warn_regexes)
					ఒకవేళ (std::regex_శోధన(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				కొరకు (స్వయంచాలక &[_, అంశం] : expect_log)
					ఒకవేళ (std::regex_శోధన(linebuffer, అంశం.నమూనా))
						అంశం.current_లెక్క++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = అబద్ధం;
	}
}

శూన్య LogManager::formatted_string(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::చురుకైన());

	ఒకవేళ (make_debug && !is_debug(1))
		తిరిగి;
	logv_string(severity, src, ఉపసర్గ_2, format, std::move(str));
}

శూన్య LogManager::formatted_శీర్షిక(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::చురుకైన());

	spacer();
	ఒకవేళ (శీర్షిక_లెక్క.కొలత() > 0)
		శీర్షిక_లెక్క.వెనుక()++;

	ఒకవేళ (పూర్ణాంకం(శీర్షిక_లెక్క.కొలత()) <= verbose_level) {
		log_stderr_sink_forced = నిజం;
	}

	std::string శీర్షిక_id;

	కొరకు (పూర్ణాంకం c : శీర్షిక_లెక్క)
		శీర్షిక_id += stringf("%s%d", శీర్షిక_id.ఖాళీలు() ? "" : ".", c);

	formatted_string(LogSeverity::శీర్షిక, Log_ఆధారం_Location{}, stringf("%s. ", శీర్షిక_id), format, std::move(str));
	flush();

	ఒకవేళ (hdump_all)
		hdump[శీర్షిక_id].చొప్పించు("yosys_dump_" + శీర్షిక_id + ".il");

	ఒకవేళ (hdump.లెక్క(శీర్షిక_id) && design != శూన్యసూచిక)
		కొరకు (స్వయంచాలక &filename : hdump.at(శీర్షిక_id)) {
			సంవర్గమానం("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = అబద్ధం;
}

శూన్య LogManager::formatted_warning(Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2)
{
	log_assert(!Multithreading::చురుకైన());

	తార్కిక suppressed = అబద్ధం;

	కొరకు (స్వయంచాలక &re : nowarn_regexes)
		ఒకవేళ (std::regex_శోధన(సందేశం_2, re))
			suppressed = నిజం;

	std::string loc = !src.filename.ఖాళీలు() ? stringf("%s:%d: ", src.filename, src.ప్రారంభం_పంక్తి) : "";
	ఒకవేళ (suppressed)
	{
		సంవర్గమానం("Suppressed %s%s%s", loc, ఉపసర్గ_2, సందేశం_2);
	}
	లేకపోతే
	{
		పూర్ణాంకం bak_make_debug = make_debug;
		make_debug = 0;

		కొరకు (స్వయంచాలక &re : werror_regexes)
			ఒకవేళ (std::regex_శోధన(సందేశం_2, re))
				formatted_error(src, "ERROR: ", format, సందేశం_2);

		తార్కిక warning_సరిపోలిక = అబద్ధం;
		కొరకు (స్వయంచాలక &[_, అంశం] : expect_warning)
			ఒకవేళ (std::regex_శోధన(సందేశం_2, అంశం.నమూనా)) {
				అంశం.current_లెక్క++;
				warning_సరిపోలిక = నిజం;
			}

		std::string నమూనా = loc + string(ఉపసర్గ_2) + సందేశం_2;
		కొరకు (స్వయంచాలక &[_, అంశం] : expect_ఉపసర్గ_warning)
			ఒకవేళ (std::regex_శోధన(నమూనా, అంశం.నమూనా)) {
				అంశం.current_లెక్క++;
				warning_సరిపోలిక = నిజం;
			}

		ఒకవేళ (warnings.లెక్క(సందేశం_2))
		{
			formatted_string(LogSeverity::Info, src, ఉపసర్గ_2, format, సందేశం_2);
			flush();
		}
		లేకపోతే
		{
			formatted_string(LogSeverity::Warning, src, ఉపసర్గ_2, format, సందేశం_2);
			flush();
			warnings.చొప్పించు(సందేశం_2);
		}

		ఒకవేళ (!warning_సరిపోలిక)
			warnings_లెక్క_noexpect++;
		warnings_లెక్క++;
		make_debug = bak_make_debug;
	}
}

శూన్య LogManager::suppressed() {
	ఒకవేళ (debug_suppressed && !make_debug) {
		constexpr స్థిరరాశి అక్షరం* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, Log_ఆధారం_Location{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

శూన్య LogManager::error_with_ఉపసర్గ(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2)
{
	పూర్ణాంకం bak_make_debug = make_debug;
	make_debug = 0;
	ఒకవేళ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, ఉపసర్గ_2, format, సందేశం_2);
	flush();

	make_debug = bak_make_debug;

	కొరకు (స్వయంచాలక &[_, అంశం] : expect_error)
		ఒకవేళ (std::regex_శోధన(సందేశం_2, అంశం.నమూనా))
			అంశం.current_లెక్క++;

	std::string loc = !src.filename.ఖాళీలు() ? stringf("%s:%d: ", src.filename, src.ప్రారంభం_పంక్తి) : "";
	std::string నమూనా = loc + string(ఉపసర్గ_2) + సందేశం_2;
	కొరకు (స్వయంచాలక &[_, అంశం] : expect_ఉపసర్గ_error)
		ఒకవేళ (std::regex_శోధన(నమూనా, అంశం.నమూనా))
			అంశం.current_లెక్క++;

	errors_లెక్క++;
}

[[noreturn]]
శూన్య LogManager::formatted_error(Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2)
{
	error_with_ఉపసర్గ(LogSeverity::Error, src, ఉపసర్గ_2, format, సందేశం_2);

	check_expected();

	ఒకవేళ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	స్థిరరాశి అక్షరం *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	ఒకవేళ (e && atoi(e))
		abort();

#ఒకవేళ defined(_MSC_VER)
	_exit(1);
#లేకపోతే
	_Exit(1);
#అయితే_ముగింపు
}

శూన్య LogManager::formatted_nonfatal_error(Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2)
{
	error_with_ఉపసర్గ(LogSeverity::NonFatalError, src, ఉపసర్గ_2, format, సందేశం_2);
}

శూన్య LogManager::కూడు_experimental(స్థిరరాశి std::string &str)
{
	ఒకవేళ (experimental_ignored.లెక్క(str) == 0 && experimental.లెక్క(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.చొప్పించు(str);
	}
}

శూన్య LogManager::కూడు_deprecated(స్థిరరాశి std::string &feature, స్థిరరాశి std::string &earliest_removal)
{
	ఒకవేళ (deprecated.లెక్క(feature) == 0) {
		ఒకవేళ (earliest_removal.ఖాళీలు())
			log_warning("Feature '%s' is deprecated.\n", feature);
		లేకపోతే
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

శూన్య log_assert_failure(స్థిరరాశి అక్షరం *సమాసం, ఆధారం_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", సమాసం, location.file_name(), location.పంక్తి());
}

శూన్య log_abort_అంతర్గత(స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి)
{
	log_error("Abort in %s:%d.\n", file, పంక్తి);
}

శూన్య log_yosys_abort_సందేశం(std::string_view file, పూర్ణాంకం పంక్తి, std::string_view func, std::string_view సందేశం_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, పంక్తి, func, సందేశం_2);
}

శూన్య LogManager::formatted_cmd_error(std::string_view format, std::string సందేశం_2)
{
	ఒకవేళ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, Log_ఆధారం_Location{}, "ERROR: ", format, సందేశం_2);
		flush();

		విసురు log_cmd_error_exception();
	}

	formatted_error(Log_ఆధారం_Location{}, "ERROR: ", format, సందేశం_2);
}

శూన్య LogManager::spacer()
{
	ఒకవేళ (newline_లెక్క < 2) సంవర్గమానం("\n");
	ఒకవేళ (newline_లెక్క < 2) సంవర్గమానం("\n");
}

శూన్య LogManager::clear_original()
{
	std::erase_if(sinks, [](స్థిరరాశి స్వయంచాలక &sink) { తిరిగి dynamic_cast<LogSinkRef *>(sink.get()) != శూన్యసూచిక; });
}

శూన్య LogManager::నెట్టు()
{
	శీర్షిక_లెక్క.నెట్టు_వెనుక(0);
}

శూన్య LogManager::తీయి()
{
	శీర్షిక_లెక్క.తీయి_వెనుక();
	flush();
}

#ఒకవేళ defined(YOSYS_ENABLE_DLOPEN)
శూన్య log_backtrace(స్థిరరాశి అక్షరం *ఉపసర్గ_2, పూర్ణాంకం స్థాయిలు)
{
	ఒకవేళ (స్థాయిలు <= 0) తిరిగి;

	Dl_info dli;
	శూన్య *p;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #1: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #1: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 1) తిరిగి;

#నిర్వచితం_కాకపోతే DEBUG
	సంవర్గమానం("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", ఉపసర్గ_2);
#లేకపోతే
	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #2: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #2: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 2) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #3: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #3: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 3) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #4: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #4: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 4) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #5: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #5: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 5) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #6: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #6: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 6) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #7: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #7: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 7) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #8: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #8: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 8) తిరిగి;

	ఒకవేళ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		సంవర్గమానం("%sframe #9: %p %s(%p) %s(%p)\n", ఉపసర్గ_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} లేకపోతే {
		సంవర్గమానం("%sframe #9: ---\n", ఉపసర్గ_2);
		తిరిగి;
	}

	ఒకవేళ (స్థాయిలు <= 9) తిరిగి;
#అయితే_ముగింపు
}
#లేకపోతే
శూన్య log_backtrace(స్థిరరాశి అక్షరం*, పూర్ణాంకం) { }
#అయితే_ముగింపు

శూన్య LogManager::reset_దొంతర()
{
	అయితే (శీర్షిక_లెక్క.కొలత() > 1)
		శీర్షిక_లెక్క.తీయి_వెనుక();
	flush();
}

శూన్య log_dump_val_కార్మికుడు(RTLIL::IdString v) {
	ఒకవేళ (v == IdString::Null)
		సంవర్గమానం("(null)");
	లేకపోతే ఒకవేళ (ID::is_static(v))
		సంవర్గమానం("%s", ID::unescaped_str(v));
	లేకపోతే ఒకవేళ (yosys_design && yosys_design->twines().is_సజీవ(v))
		సంవర్గమానం("%s", yosys_design->twines().unescaped_str(v));
	లేకపోతే
		సంవర్గమానం("%s", v.handle_token());
}

శూన్య log_dump_val_కార్మికుడు(RTLIL::SigSpec v) {
	సంవర్గమానం("%s", log_signal(v));
}

శూన్య log_dump_val_కార్మికుడు(RTLIL::State v) {
	సంవర్గమానం("%s", log_signal(v));
}

std::string log_signal(స్థిరరాశి RTLIL::SigSpec &sig, తార్కిక autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి buf.str();
}

std::string log_const(స్థిరరాశి RTLIL::Const &విలువ, తార్కిక autoint)
{
	ఒకవేళ ((విలువ.flags & RTLIL::CONST_FLAG_STRING) == 0)
		తిరిగి log_signal(విలువ, autoint);

	తిరిగి "\"" + విలువ.decode_string() + "\"";
}

శూన్య log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, అబద్ధం, నిజం, అబద్ధం, RTLIL_BACKEND::Dump_బహుళకం::Readable);
	సంవర్గమానం("%s", buf.str());
}

శూన్య log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : శూన్యసూచిక, RTLIL_BACKEND::Dump_బహుళకం::Readable);
	సంవర్గమానం("%s", buf.str());
}

శూన్య log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : శూన్యసూచిక, RTLIL_BACKEND::Dump_బహుళకం::Readable);
	సంవర్గమానం("%s", buf.str());
}

శూన్య LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, Log_Expected_అంశం> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, Log_Expected_అంశం> expect_ఉపసర్గ_log_copy, expect_ఉపసర్గ_warning_copy, expect_ఉపసర్గ_error_copy;
	std::మార్పిడి(expect_warning_copy, expect_warning);
	std::మార్పిడి(expect_log_copy, expect_log);
	std::మార్పిడి(expect_error_copy, expect_error);
	std::మార్పిడి(expect_ఉపసర్గ_warning_copy, expect_ఉపసర్గ_warning);
	std::మార్పిడి(expect_ఉపసర్గ_log_copy, expect_ఉపసర్గ_log);
	std::మార్పిడి(expect_ఉపసర్గ_error_copy, expect_ఉపసర్గ_error);

	స్వయంచాలక check = [&](స్థిరరాశి std::string రకం, std::string నమూనా, Log_Expected_అంశం అంశం) {
		ఒకవేళ (అంశం.current_లెక్క == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", రకం, నమూనా);
		}
		ఒకవేళ (అంశం.current_లెక్క != అంశం.expected_లెక్క) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				రకం.c_str(), నమూనా.c_str(), అంశం.current_లెక్క, అంశం.expected_లెక్క);
		}
	};

	కొరకు (స్వయంచాలక &[నమూనా, అంశం] : expect_warning_copy)
		check("warning", నమూనా, అంశం);
	కొరకు (స్వయంచాలక &[నమూనా, అంశం] : expect_ఉపసర్గ_warning_copy)
		check("prefixed warning", నమూనా, అంశం);
	కొరకు (స్వయంచాలక &[నమూనా, అంశం] : expect_log_copy)
		check("log", నమూనా, అంశం);
	కొరకు (స్వయంచాలక &[నమూనా, అంశం] : expect_ఉపసర్గ_log_copy)
		check("prefixed log", నమూనా, అంశం);

	స్వయంచాలక check_err = [&](స్థిరరాశి std::string రకం, std::string నమూనా, Log_Expected_అంశం అంశం) {
		ఒకవేళ (అంశం.current_లెక్క == అంశం.expected_లెక్క) {
			warn_regexes.clear();
			సంవర్గమానం("Expected %s pattern '%s' found !!!\n", రకం, నమూనా);
			yosys_shutdown();
			#ఒకవేళ defined(_MSC_VER)
				_exit(0);
			#లేకపోతే
				_Exit(0);
			#అయితే_ముగింపు
		} లేకపోతే {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", రకం, నమూనా);
		}
	};
	కొరకు (స్వయంచాలక &[నమూనా, అంశం] : expect_error_copy)
		check_err("error", నమూనా, అంశం);
	కొరకు (స్వయంచాలక &[నమూనా, అంశం] : expect_ఉపసర్గ_error_copy)
		check_err("prefixed error", నమూనా, అంశం);
}

శూన్య LogManager::report_unexpected_error()
{
	ఒకవేళ (expect_no_warnings && warnings_లెక్క_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_కొలత(warnings),
					warnings_లెక్క, warnings_లెక్క - warnings_లెక్క_noexpect);
}

శూన్య LogManager::కూడు_expect(std::string type, std::string నమూనా, పూర్ణాంకం లెక్క)
{
	ఒకవేళ (type == "error")
		expect_error[నమూనా] = Log_Expected_అంశం(YS_REGEX_COMPILE(నమూనా), లెక్క);
	లేకపోతే ఒకవేళ (type == "prefix-error")
		expect_ఉపసర్గ_error[నమూనా] = Log_Expected_అంశం(YS_REGEX_COMPILE(నమూనా), లెక్క);
	లేకపోతే ఒకవేళ (type == "warning")
		expect_warning[నమూనా] = Log_Expected_అంశం(YS_REGEX_COMPILE(నమూనా), లెక్క);
	లేకపోతే ఒకవేళ (type == "prefix-warning")
		expect_ఉపసర్గ_warning[నమూనా] = Log_Expected_అంశం(YS_REGEX_COMPILE(నమూనా), లెక్క);
	లేకపోతే ఒకవేళ (type == "log")
		expect_log[నమూనా] = Log_Expected_అంశం(YS_REGEX_COMPILE(నమూనా), లెక్క);
	లేకపోతే ఒకవేళ (type == "prefix-log")
		expect_ఉపసర్గ_log[నమూనా] = Log_Expected_అంశం(YS_REGEX_COMPILE(నమూనా), లెక్క);
	లేకపోతే log_abort();
}

శూన్య LogManager::ప్రారంభం_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::ముగింపు_hasher()
{
	ఒకవేళ (!hasher)
		తిరిగి {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	తిరిగి hash;
}

YOSYS_NAMESPACE_చివర
