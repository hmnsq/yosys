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

#जर !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#जर defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

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
	परत instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() स्थिरांक
{
	स्थिर स्थिरांक स्वतः initial_time = std::chrono::steady_clock::now();
	परत initial_time;
}

शून्य (*log_error_atexit)() = nullptr;
शून्य (*log_verific_callback)(पूर्णांक संदेश_type, स्थिरांक अक्षर *message_id, LogSourceLocation src, स्थिरांक अक्षर *संदेश) = nullptr;

स्थिर तार्किक next_print_log = सत्य;

FileLogSink::FileLogSink(स्थिरांक std::string &filename, तार्किक ओळ_buffered, तार्किक append)
	: file(ख_खोलो(filename.c_str(), append ? "at" : "wt"))
{
	जर (!file)
		फेका std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	जर (ओळ_buffered)
		रखो_भबफ(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	जर (file)
		ख_बंद(file);
}

शून्य FileLogSink::log(स्थिरांक LogMessage &संदेश)
{
	ख_माला_दो(संदेश.cached_संदेश.c_str(), file);
}

शून्य FileLogSink::flush()
{
	ख_साफ(file);
}

शून्य ConsoleLogSink::log(स्थिरांक LogMessage &संदेश)
{
	खाता *file = (संदेश.severity == LogSeverity::Error || संदेश.severity == LogSeverity::NonFatalError) ? मानक_त्रुटि : मानक_निकास;
	ख_माला_दो(संदेश.cached_संदेश.c_str(), file);
}

शून्य ConsoleLogSink::flush()
{
	ख_साफ(मानक_निकास);
	ख_साफ(मानक_त्रुटि);
}

तार्किक StderrLogSink::should_log(स्थिरांक LogMessage &संदेश) स्थिरांक
{
	परत संदेश.severity == LogSeverity::Error || संदेश.severity == LogSeverity::NonFatalError ||
			(संदेश.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

शून्य StderrLogSink::log(स्थिरांक LogMessage &संदेश)
{
	ख_माला_दो(संदेश.cached_संदेश.c_str(), मानक_त्रुटि);
}

शून्य StderrLogSink::flush()
{
	ख_साफ(मानक_त्रुटि);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

शून्य ScratchPadLogSink::log(स्थिरांक LogMessage &संदेश)
{
	RTLIL::Design *design = yosys_get_design();
	जर (!design)
		परत;

	design->scratchpad[scratchpad].append(संदेश.cached_संदेश);
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
	जर (logger().get_log_time())
	{
		जर (next_print_log) {
			next_print_log = असत्य;
			स्वतः elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			स्वतः us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", पूर्णांक(us.मोजणी() / 1'000'000),पूर्णांक(us.मोजणी() % 1'000'000));
		}

		जर (!format.empty() && format.back() == '\n')
			next_print_log = सत्य;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		जर (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = सत्य;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ओळ) : "";
	cached_संदेश = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

शून्य LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	माप_प्रकार remove_leading = 0;
	पर्यंत (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	जर (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ओळ) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	जर (str.empty())
		परत;

	माप_प्रकार nnl_pos = str.find_last_not_of('\n');
	जर (nnl_pos == std::string::npos)
		newline_मोजणी += GetSize(str);
	अन्यथा
		newline_मोजणी = GetSize(str) - nnl_pos - 1;

	जर (hasher)
		hasher->update(str);

	स्वतः संदेश = LogMessage(severity, src, prefix, format, str_in);
	साठी (स्वतः &sink : sinks) {
		जर (sink->should_log(संदेश))
			sink->log(संदेश);
	}

	जर (severity == LogSeverity::Header)
		str = str_in;

	स्थिर std::string linebuffer;
	स्थिर तार्किक warn_regex_recusion_guard = असत्य;

	जर (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = सत्य;

		जर (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		अन्यथा
		{
			linebuffer += str;

			जर (!linebuffer.empty() && linebuffer.back() == '\n') {
				साठी (स्वतः &re : warn_regexes)
					जर (std::regex_शोध(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				साठी (स्वतः &[_, item] : expect_log)
					जर (std::regex_शोध(linebuffer, item.pattern))
						item.current_मोजणी++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = असत्य;
	}
}

शून्य LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	जर (make_debug && !is_debug(1))
		परत;
	logv_string(severity, src, prefix, format, std::move(str));
}

शून्य LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	जर (header_मोजणी.size() > 0)
		header_मोजणी.back()++;

	जर (पूर्णांक(header_मोजणी.size()) <= verbose_level) {
		log_stderr_sink_forced = सत्य;
	}

	std::string header_id;

	साठी (पूर्णांक c : header_मोजणी)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	जर (hdump_all)
		hdump[header_id].घाला("yosys_dump_" + header_id + ".il");

	जर (hdump.मोजणी(header_id) && design != nullptr)
		साठी (स्वतः &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = असत्य;
}

शून्य LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	तार्किक suppressed = असत्य;

	साठी (स्वतः &re : nowarn_regexes)
		जर (std::regex_शोध(message, re))
			suppressed = सत्य;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ओळ) : "";
	जर (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	अन्यथा
	{
		पूर्णांक bak_make_debug = make_debug;
		make_debug = 0;

		साठी (स्वतः &re : werror_regexes)
			जर (std::regex_शोध(message, re))
				formatted_error(src, "ERROR: ", format, message);

		तार्किक warning_match = असत्य;
		साठी (स्वतः &[_, item] : expect_warning)
			जर (std::regex_शोध(message, item.pattern)) {
				item.current_मोजणी++;
				warning_match = सत्य;
			}

		std::string pattern = loc + string(prefix) + message;
		साठी (स्वतः &[_, item] : expect_prefix_warning)
			जर (std::regex_शोध(pattern, item.pattern)) {
				item.current_मोजणी++;
				warning_match = सत्य;
			}

		जर (warnings.मोजणी(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		अन्यथा
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.घाला(message);
		}

		जर (!warning_match)
			warnings_मोजणी_noexpect++;
		warnings_मोजणी++;
		make_debug = bak_make_debug;
	}
}

शून्य LogManager::suppressed() {
	जर (debug_suppressed && !make_debug) {
		constexpr स्थिरांक अक्षर* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

शून्य LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	पूर्णांक bak_make_debug = make_debug;
	make_debug = 0;
	जर (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	साठी (स्वतः &[_, item] : expect_error)
		जर (std::regex_शोध(message, item.pattern))
			item.current_मोजणी++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ओळ) : "";
	std::string pattern = loc + string(prefix) + message;
	साठी (स्वतः &[_, item] : expect_prefix_error)
		जर (std::regex_शोध(pattern, item.pattern))
			item.current_मोजणी++;

	errors_मोजणी++;
}

[[noreturn]]
शून्य LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	जर (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	स्थिरांक अक्षर *e = दो_पर्या("YOSYS_ABORT_ON_LOG_ERROR");
	जर (e && म_से_प(e))
		पात();

#जर defined(_MSC_VER)
	_exit(1);
#अन्यथा
	_Exit(1);
#endif
}

शून्य LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

शून्य LogManager::add_experimental(स्थिरांक std::string &str)
{
	जर (experimental_ignored.मोजणी(str) == 0 && experimental.मोजणी(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.घाला(str);
	}
}

शून्य LogManager::add_deprecated(स्थिरांक std::string &feature, स्थिरांक std::string &earliest_removal)
{
	जर (deprecated.मोजणी(feature) == 0) {
		जर (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		अन्यथा
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

शून्य log_assert_failure(स्थिरांक अक्षर *अभिव्यक्ती, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", अभिव्यक्ती, location.file_name(), location.ओळ());
}

शून्य log_abort_internal(स्थिरांक अक्षर *file, पूर्णांक ओळ)
{
	log_error("Abort in %s:%d.\n", file, ओळ);
}

शून्य log_yosys_abort_message(std::string_view file, पूर्णांक ओळ, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, ओळ, func, message);
}

शून्य LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	जर (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		फेका log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

शून्य LogManager::spacer()
{
	जर (newline_मोजणी < 2) log("\n");
	जर (newline_मोजणी < 2) log("\n");
}

शून्य LogManager::clear_original()
{
	std::erase_if(sinks, [](स्थिरांक स्वतः &sink) { परत dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

शून्य LogManager::ढकला()
{
	header_मोजणी.ढकला_back(0);
}

शून्य LogManager::काढा()
{
	header_मोजणी.काढा_back();
	flush();
}

#जर defined(YOSYS_ENABLE_DLOPEN)
शून्य log_backtrace(स्थिरांक अक्षर *prefix, पूर्णांक levels)
{
	जर (levels <= 0) परत;

	Dl_info dli;
	शून्य *p;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #1: ---\n", prefix);
		परत;
	}

	जर (levels <= 1) परत;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#अन्यथा
	जर ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #2: ---\n", prefix);
		परत;
	}

	जर (levels <= 2) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #3: ---\n", prefix);
		परत;
	}

	जर (levels <= 3) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #4: ---\n", prefix);
		परत;
	}

	जर (levels <= 4) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #5: ---\n", prefix);
		परत;
	}

	जर (levels <= 5) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #6: ---\n", prefix);
		परत;
	}

	जर (levels <= 6) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #7: ---\n", prefix);
		परत;
	}

	जर (levels <= 7) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #8: ---\n", prefix);
		परत;
	}

	जर (levels <= 8) परत;

	जर ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} अन्यथा {
		log("%sframe #9: ---\n", prefix);
		परत;
	}

	जर (levels <= 9) परत;
#endif
}
#अन्यथा
शून्य log_backtrace(स्थिरांक अक्षर*, पूर्णांक) { }
#endif

शून्य LogManager::reset_चळत()
{
	पर्यंत (header_मोजणी.size() > 1)
		header_मोजणी.काढा_back();
	flush();
}

शून्य log_dump_val_worker(RTLIL::IdString v) {
	जर (v == IdString::Null)
		log("(null)");
	अन्यथा जर (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	अन्यथा जर (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	अन्यथा
		log("%s", v.handle_token());
}

शून्य log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

शून्य log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(स्थिरांक RTLIL::SigSpec &sig, तार्किक autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	परत buf.str();
}

std::string log_const(स्थिरांक RTLIL::Const &मूल्य, तार्किक autoint)
{
	जर ((मूल्य.flags & RTLIL::CONST_FLAG_STRING) == 0)
		परत log_signal(मूल्य, autoint);

	परत "\"" + मूल्य.decode_string() + "\"";
}

शून्य log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, असत्य, सत्य, असत्य, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

शून्य log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

शून्य log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

शून्य LogManager::check_expected()
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

	स्वतः check = [&](स्थिरांक std::string kind, std::string pattern, LogExpectedItem item) {
		जर (item.current_मोजणी == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		जर (item.current_मोजणी != item.expected_मोजणी) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_मोजणी, item.expected_मोजणी);
		}
	};

	साठी (स्वतः &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	साठी (स्वतः &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	साठी (स्वतः &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	साठी (स्वतः &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	स्वतः check_err = [&](स्थिरांक std::string kind, std::string pattern, LogExpectedItem item) {
		जर (item.current_मोजणी == item.expected_मोजणी) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#जर defined(_MSC_VER)
				_exit(0);
			#अन्यथा
				_Exit(0);
			#endif
		} अन्यथा {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	साठी (स्वतः &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	साठी (स्वतः &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

शून्य LogManager::report_unexpected_error()
{
	जर (expect_no_warnings && warnings_मोजणी_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_मोजणी, warnings_मोजणी - warnings_मोजणी_noexpect);
}

शून्य LogManager::add_expect(std::string type, std::string pattern, पूर्णांक मोजणी)
{
	जर (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), मोजणी);
	अन्यथा जर (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), मोजणी);
	अन्यथा जर (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), मोजणी);
	अन्यथा जर (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), मोजणी);
	अन्यथा जर (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), मोजणी);
	अन्यथा जर (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), मोजणी);
	अन्यथा log_abort();
}

शून्य LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	जर (!hasher)
		परत {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	परत hash;
}

YOSYS_NAMESPACE_END
