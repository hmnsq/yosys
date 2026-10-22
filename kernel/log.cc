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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/sha1/sha1.h"
#अन्तर्भाव "backends/rtlil/rtlil_backend.h"

#यदि !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#यदि_अन्तः

#यदि defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#यदि_अन्तः

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <vector>
#अन्तर्भाव <list>

YOSYS_NAMESPACE_BEGIN

तार्किक log_stderr_sink_forced = असत्य;

LogManager &logger()
{
	स्थिर LogManager instance;
	प्रत्यागम instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ध्रुव
{
	स्थिर ध्रुव स्वचालित initial_time = std::chrono::steady_clock::now();
	प्रत्यागम initial_time;
}

शून्य (*log_error_atexit)() = शून्यसूचकः;
शून्य (*log_verific_callback)(पूर्णाङ्क सन्देशः_type, ध्रुव वर्ण *सन्देशः_id, LogSourceLocation src, ध्रुव वर्ण *सन्देशः) = शून्यसूचकः;

स्थिर तार्किक अग्र_print_log = सत्य;

FileLogSink::FileLogSink(ध्रुव std::string &filename, तार्किक पङ्क्तिः_buffered, तार्किक append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	यदि (!file)
		क्षिप std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	यदि (पङ्क्तिः_buffered)
		setvbuf(file, शून्यसूचकः, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	यदि (file)
		fclose(file);
}

शून्य FileLogSink::लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः)
{
	fputs(सन्देशः.cached_सन्देशः.c_str(), file);
}

शून्य FileLogSink::flush()
{
	fflush(file);
}

शून्य ConsoleLogSink::लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः)
{
	FILE *file = (सन्देशः.severity == LogSeverity::Error || सन्देशः.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(सन्देशः.cached_सन्देशः.c_str(), file);
}

शून्य ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

तार्किक StderrLogSink::should_log(ध्रुव Log_सन्देशः &सन्देशः) ध्रुव
{
	प्रत्यागम सन्देशः.severity == LogSeverity::Error || सन्देशः.severity == LogSeverity::NonFatalError ||
			(सन्देशः.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

शून्य StderrLogSink::लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः)
{
	fputs(सन्देशः.cached_सन्देशः.c_str(), stderr);
}

शून्य StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

शून्य ScratchPadLogSink::लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः)
{
	RTLIL::Design *design = yosys_get_design();
	यदि (!design)
		प्रत्यागम;

	design->scratchpad[scratchpad].append(सन्देशः.cached_सन्देशः);
}

Log_सन्देशः::Log_सन्देशः(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string_view सन्देशः_2) :
	severity(severity),
	उपसर्गः_2(उपसर्गः_2),
	format(format),
	सन्देशः_2(सन्देशः_2),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string time_str;
	यदि (logger().get_log_time())
	{
		यदि (अग्र_print_log) {
			अग्र_print_log = असत्य;
			स्वचालित elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			स्वचालित us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", पूर्णाङ्क(us.गणनम्() / 1'000'000),पूर्णाङ्क(us.गणनम्() % 1'000'000));
		}

		यदि (!format.रिक्तानि() && format.पृष्ठभागः() == '\n')
			अग्र_print_log = सत्य;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		यदि (format == "%s" && !सन्देशः_2.रिक्तानि() && सन्देशः_2.पृष्ठभागः() == '\n')
			अग्र_print_log = सत्य;
	}
	std::string loc = !src.filename.रिक्तानि() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	cached_सन्देशः = stringf("%s%s%s%s", time_str, loc, उपसर्गः_2, सन्देशः_2);
}

शून्य LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string str_अन्तर्) {
	आकार_t remove_leading = 0;
	यावत् (format.आकार() > 1 && format[0] == '\n') {
		logv_string(severity, src, उपसर्गः_2, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	यदि (remove_leading > 0) {
		str_अन्तर् = str_अन्तर्.substr(remove_leading);
	}

	std::string loc = !src.filename.रिक्तानि() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	std::string str = stringf("%s%s%s",loc,उपसर्गः_2,str_अन्तर्);

	यदि (str.रिक्तानि())
		प्रत्यागम;

	आकार_t nnl_pos = str.find_चरमः_not_सम्बन्धि('\n');
	यदि (nnl_pos == std::string::npos)
		newline_गणनम् += Get_आकार(str);
	अन्यथा
		newline_गणनम् = Get_आकार(str) - nnl_pos - 1;

	यदि (hasher)
		hasher->update(str);

	स्वचालित सन्देशः = Log_सन्देशः(severity, src, उपसर्गः_2, format, str_अन्तर्);
	प्रति (स्वचालित &sink : sinks) {
		यदि (sink->should_log(सन्देशः))
			sink->लघुगणकः(सन्देशः);
	}

	यदि (severity == LogSeverity::शीर्षकम्)
		str = str_अन्तर्;

	स्थिर std::string linebuffer;
	स्थिर तार्किक warn_regex_recusion_guard = असत्य;

	यदि (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = सत्य;

		यदि (warn_regexes.रिक्तानि() && expect_log.रिक्तानि() && expect_उपसर्गः_log.रिक्तानि())
		{
			linebuffer.clear();
		}
		अन्यथा
		{
			linebuffer += str;

			यदि (!linebuffer.रिक्तानि() && linebuffer.पृष्ठभागः() == '\n') {
				प्रति (स्वचालित &re : warn_regexes)
					यदि (std::regex_अन्वेषणम्(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				प्रति (स्वचालित &[_, item] : expect_log)
					यदि (std::regex_अन्वेषणम्(linebuffer, item.आदर्शः))
						item.current_गणनम्++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = असत्य;
	}
}

शून्य LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string str)
{
	log_assert(!Multithreading::सक्रिय());

	यदि (make_debug && !is_debug(1))
		प्रत्यागम;
	logv_string(severity, src, उपसर्गः_2, format, std::move(str));
}

शून्य LogManager::formatted_शीर्षकम्(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::सक्रिय());

	spacer();
	यदि (शीर्षकम्_गणनम्.आकार() > 0)
		शीर्षकम्_गणनम्.पृष्ठभागः()++;

	यदि (पूर्णाङ्क(शीर्षकम्_गणनम्.आकार()) <= verbose_level) {
		log_stderr_sink_forced = सत्य;
	}

	std::string शीर्षकम्_id;

	प्रति (पूर्णाङ्क c : शीर्षकम्_गणनम्)
		शीर्षकम्_id += stringf("%s%d", शीर्षकम्_id.रिक्तानि() ? "" : ".", c);

	formatted_string(LogSeverity::शीर्षकम्, LogSourceLocation{}, stringf("%s. ", शीर्षकम्_id), format, std::move(str));
	flush();

	यदि (hdump_all)
		hdump[शीर्षकम्_id].निवेशय("yosys_dump_" + शीर्षकम्_id + ".il");

	यदि (hdump.गणनम्(शीर्षकम्_id) && design != शून्यसूचकः)
		प्रति (स्वचालित &filename : hdump.at(शीर्षकम्_id)) {
			लघुगणकः("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = असत्य;
}

शून्य LogManager::formatted_warning(LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2)
{
	log_assert(!Multithreading::सक्रिय());

	तार्किक suppressed = असत्य;

	प्रति (स्वचालित &re : nowarn_regexes)
		यदि (std::regex_अन्वेषणम्(सन्देशः_2, re))
			suppressed = सत्य;

	std::string loc = !src.filename.रिक्तानि() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	यदि (suppressed)
	{
		लघुगणकः("Suppressed %s%s%s", loc, उपसर्गः_2, सन्देशः_2);
	}
	अन्यथा
	{
		पूर्णाङ्क bak_make_debug = make_debug;
		make_debug = 0;

		प्रति (स्वचालित &re : werror_regexes)
			यदि (std::regex_अन्वेषणम्(सन्देशः_2, re))
				formatted_error(src, "ERROR: ", format, सन्देशः_2);

		तार्किक warning_मेलनम् = असत्य;
		प्रति (स्वचालित &[_, item] : expect_warning)
			यदि (std::regex_अन्वेषणम्(सन्देशः_2, item.आदर्शः)) {
				item.current_गणनम्++;
				warning_मेलनम् = सत्य;
			}

		std::string आदर्शः = loc + string(उपसर्गः_2) + सन्देशः_2;
		प्रति (स्वचालित &[_, item] : expect_उपसर्गः_warning)
			यदि (std::regex_अन्वेषणम्(आदर्शः, item.आदर्शः)) {
				item.current_गणनम्++;
				warning_मेलनम् = सत्य;
			}

		यदि (warnings.गणनम्(सन्देशः_2))
		{
			formatted_string(LogSeverity::Info, src, उपसर्गः_2, format, सन्देशः_2);
			flush();
		}
		अन्यथा
		{
			formatted_string(LogSeverity::Warning, src, उपसर्गः_2, format, सन्देशः_2);
			flush();
			warnings.निवेशय(सन्देशः_2);
		}

		यदि (!warning_मेलनम्)
			warnings_गणनम्_noexpect++;
		warnings_गणनम्++;
		make_debug = bak_make_debug;
	}
}

शून्य LogManager::suppressed() {
	यदि (debug_suppressed && !make_debug) {
		constexpr ध्रुव वर्ण* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

शून्य LogManager::error_with_उपसर्गः(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2)
{
	पूर्णाङ्क bak_make_debug = make_debug;
	make_debug = 0;
	यदि (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, उपसर्गः_2, format, सन्देशः_2);
	flush();

	make_debug = bak_make_debug;

	प्रति (स्वचालित &[_, item] : expect_error)
		यदि (std::regex_अन्वेषणम्(सन्देशः_2, item.आदर्शः))
			item.current_गणनम्++;

	std::string loc = !src.filename.रिक्तानि() ? stringf("%s:%d: ", src.filename, src.start_पङ्क्तिः) : "";
	std::string आदर्शः = loc + string(उपसर्गः_2) + सन्देशः_2;
	प्रति (स्वचालित &[_, item] : expect_उपसर्गः_error)
		यदि (std::regex_अन्वेषणम्(आदर्शः, item.आदर्शः))
			item.current_गणनम्++;

	errors_गणनम्++;
}

[[noreturn]]
शून्य LogManager::formatted_error(LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2)
{
	error_with_उपसर्गः(LogSeverity::Error, src, उपसर्गः_2, format, सन्देशः_2);

	check_expected();

	यदि (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ध्रुव वर्ण *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	यदि (e && atoi(e))
		abort();

#यदि defined(_MSC_VER)
	_exit(1);
#अन्यथा
	_Exit(1);
#यदि_अन्तः
}

शून्य LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2)
{
	error_with_उपसर्गः(LogSeverity::NonFatalError, src, उपसर्गः_2, format, सन्देशः_2);
}

शून्य LogManager::सङ्कलय_experimental(ध्रुव std::string &str)
{
	यदि (experimental_ignored.गणनम्(str) == 0 && experimental.गणनम्(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.निवेशय(str);
	}
}

शून्य LogManager::सङ्कलय_deprecated(ध्रुव std::string &feature, ध्रुव std::string &earliest_removal)
{
	यदि (deprecated.गणनम्(feature) == 0) {
		यदि (earliest_removal.रिक्तानि())
			log_warning("Feature '%s' is deprecated.\n", feature);
		अन्यथा
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

शून्य log_assert_failure(ध्रुव वर्ण *व्यञ्जकम्, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", व्यञ्जकम्, location.file_name(), location.पङ्क्तिः());
}

शून्य log_abort_आन्तरिकम्(ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः)
{
	log_error("Abort in %s:%d.\n", file, पङ्क्तिः);
}

शून्य log_yosys_abort_सन्देशः(std::string_view file, पूर्णाङ्क पङ्क्तिः, std::string_view func, std::string_view सन्देशः_2)
{
	log_error("Abort in %s:%d (%s): %s\n", file, पङ्क्तिः, func, सन्देशः_2);
}

शून्य LogManager::formatted_cmd_error(std::string_view format, std::string सन्देशः_2)
{
	यदि (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, सन्देशः_2);
		flush();

		क्षिप log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, सन्देशः_2);
}

शून्य LogManager::spacer()
{
	यदि (newline_गणनम् < 2) लघुगणकः("\n");
	यदि (newline_गणनम् < 2) लघुगणकः("\n");
}

शून्य LogManager::clear_original()
{
	std::erase_if(sinks, [](ध्रुव स्वचालित &sink) { प्रत्यागम dynamic_cast<LogSinkRef *>(sink.get()) != शून्यसूचकः; });
}

शून्य LogManager::प्रक्षिप()
{
	शीर्षकम्_गणनम्.प्रक्षिप_पृष्ठभागः(0);
}

शून्य LogManager::निष्कासय()
{
	शीर्षकम्_गणनम्.निष्कासय_पृष्ठभागः();
	flush();
}

#यदि defined(YOSYS_ENABLE_DLOPEN)
शून्य log_backtrace(ध्रुव वर्ण *उपसर्गः_2, पूर्णाङ्क स्तराः)
{
	यदि (स्तराः <= 0) प्रत्यागम;

	Dl_info dli;
	शून्य *p;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #1: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #1: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 1) प्रत्यागम;

#यदि_अपरिभाषितम् DEBUG
	लघुगणकः("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", उपसर्गः_2);
#अन्यथा
	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #2: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #2: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 2) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #3: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #3: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 3) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #4: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #4: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 4) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #5: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #5: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 5) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #6: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #6: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 6) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #7: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #7: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 7) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #8: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #8: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 8) प्रत्यागम;

	यदि ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		लघुगणकः("%sframe #9: %p %s(%p) %s(%p)\n", उपसर्गः_2, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		लघुगणकः("%sframe #9: ---\n", उपसर्गः_2);
		प्रत्यागम;
	}

	यदि (स्तराः <= 9) प्रत्यागम;
#यदि_अन्तः
}
#अन्यथा
शून्य log_backtrace(ध्रुव वर्ण*, पूर्णाङ्क) { }
#यदि_अन्तः

शून्य LogManager::reset_राशिः()
{
	यावत् (शीर्षकम्_गणनम्.आकार() > 1)
		शीर्षकम्_गणनम्.निष्कासय_पृष्ठभागः();
	flush();
}

शून्य log_dump_val_कार्यकर्ता(RTLIL::IdString v) {
	यदि (v == IdString::Null)
		लघुगणकः("(null)");
	अन्यथा यदि (ID::is_static(v))
		लघुगणकः("%s", ID::unescaped_str(v));
	अन्यथा यदि (yosys_design && yosys_design->twines().is_जीवितम्(v))
		लघुगणकः("%s", yosys_design->twines().unescaped_str(v));
	अन्यथा
		लघुगणकः("%s", v.handle_token());
}

शून्य log_dump_val_कार्यकर्ता(RTLIL::SigSpec v) {
	लघुगणकः("%s", log_signal(v));
}

शून्य log_dump_val_कार्यकर्ता(RTLIL::State v) {
	लघुगणकः("%s", log_signal(v));
}

std::string log_signal(ध्रुव RTLIL::SigSpec &sig, तार्किक autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम buf.str();
}

std::string log_const(ध्रुव RTLIL::Const &मानम्, तार्किक autoint)
{
	यदि ((मानम्.flags & RTLIL::CONST_FLAG_STRING) == 0)
		प्रत्यागम log_signal(मानम्, autoint);

	प्रत्यागम "\"" + मानम्.decode_string() + "\"";
}

शून्य log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, असत्य, सत्य, असत्य, RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	लघुगणकः("%s", buf.str());
}

शून्य log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : शून्यसूचकः, RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	लघुगणकः("%s", buf.str());
}

शून्य log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : शून्यसूचकः, RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	लघुगणकः("%s", buf.str());
}

शून्य LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, LogExpectedItem> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, LogExpectedItem> expect_उपसर्गः_log_copy, expect_उपसर्गः_warning_copy, expect_उपसर्गः_error_copy;
	std::swap(expect_warning_copy, expect_warning);
	std::swap(expect_log_copy, expect_log);
	std::swap(expect_error_copy, expect_error);
	std::swap(expect_उपसर्गः_warning_copy, expect_उपसर्गः_warning);
	std::swap(expect_उपसर्गः_log_copy, expect_उपसर्गः_log);
	std::swap(expect_उपसर्गः_error_copy, expect_उपसर्गः_error);

	स्वचालित check = [&](ध्रुव std::string विधा, std::string आदर्शः, LogExpectedItem item) {
		यदि (item.current_गणनम् == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", विधा, आदर्शः);
		}
		यदि (item.current_गणनम् != item.expected_गणनम्) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				विधा.c_str(), आदर्शः.c_str(), item.current_गणनम्, item.expected_गणनम्);
		}
	};

	प्रति (स्वचालित &[आदर्शः, item] : expect_warning_copy)
		check("warning", आदर्शः, item);
	प्रति (स्वचालित &[आदर्शः, item] : expect_उपसर्गः_warning_copy)
		check("prefixed warning", आदर्शः, item);
	प्रति (स्वचालित &[आदर्शः, item] : expect_log_copy)
		check("log", आदर्शः, item);
	प्रति (स्वचालित &[आदर्शः, item] : expect_उपसर्गः_log_copy)
		check("prefixed log", आदर्शः, item);

	स्वचालित check_err = [&](ध्रुव std::string विधा, std::string आदर्शः, LogExpectedItem item) {
		यदि (item.current_गणनम् == item.expected_गणनम्) {
			warn_regexes.clear();
			लघुगणकः("Expected %s pattern '%s' found !!!\n", विधा, आदर्शः);
			yosys_shutdown();
			#यदि defined(_MSC_VER)
				_exit(0);
			#अन्यथा
				_Exit(0);
			#यदि_अन्तः
		} अन्यथा {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", विधा, आदर्शः);
		}
	};
	प्रति (स्वचालित &[आदर्शः, item] : expect_error_copy)
		check_err("error", आदर्शः, item);
	प्रति (स्वचालित &[आदर्शः, item] : expect_उपसर्गः_error_copy)
		check_err("prefixed error", आदर्शः, item);
}

शून्य LogManager::report_unexpected_error()
{
	यदि (expect_no_warnings && warnings_गणनम्_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_आकार(warnings),
					warnings_गणनम्, warnings_गणनम् - warnings_गणनम्_noexpect);
}

शून्य LogManager::सङ्कलय_expect(std::string type, std::string आदर्शः, पूर्णाङ्क गणनम्)
{
	यदि (type == "error")
		expect_error[आदर्शः] = LogExpectedItem(YS_REGEX_COMPILE(आदर्शः), गणनम्);
	अन्यथा यदि (type == "prefix-error")
		expect_उपसर्गः_error[आदर्शः] = LogExpectedItem(YS_REGEX_COMPILE(आदर्शः), गणनम्);
	अन्यथा यदि (type == "warning")
		expect_warning[आदर्शः] = LogExpectedItem(YS_REGEX_COMPILE(आदर्शः), गणनम्);
	अन्यथा यदि (type == "prefix-warning")
		expect_उपसर्गः_warning[आदर्शः] = LogExpectedItem(YS_REGEX_COMPILE(आदर्शः), गणनम्);
	अन्यथा यदि (type == "log")
		expect_log[आदर्शः] = LogExpectedItem(YS_REGEX_COMPILE(आदर्शः), गणनम्);
	अन्यथा यदि (type == "prefix-log")
		expect_उपसर्गः_log[आदर्शः] = LogExpectedItem(YS_REGEX_COMPILE(आदर्शः), गणनम्);
	अन्यथा log_abort();
}

शून्य LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	यदि (!hasher)
		प्रत्यागम {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	प्रत्यागम hash;
}

YOSYS_NAMESPACE_अन्तः
