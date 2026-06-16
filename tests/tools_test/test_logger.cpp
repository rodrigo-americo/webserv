/* ************************************************************************** */
/*  test_logger.cpp — unit tests for the Logger subsystem                     */
/* ************************************************************************** */

#include "test_utils.hpp"

#include "singleton.hpp"
#include "utils.hpp"
#include "text.hpp"

#include "LoggerRecord.hpp"
#include "LoggerFormatter.hpp"
#include "LoggerDestine.hpp"
#include "LoggerDestineOstream.hpp"
#include "LoggerDestineFile.hpp"
#include "Logger.hpp"

#include <sstream>
#include <fstream>
#include <cstdio>
#include <unistd.h>

// ── helpers ──────────────────────────────────────────────────────────────────

// Capture writes to a fd (STDOUT/STDERR), run fn(), return captured string.
static std::string capture_fd(int target_fd, void (*fn)())
{
	int pipefd[2];
	::pipe(pipefd);
	int saved = ::dup(target_fd);
	::dup2(pipefd[1], target_fd);
	::close(pipefd[1]);

	fn();

	::dup2(saved, target_fd);
	::close(saved);

	char buf[4096] = {};
	ssize_t n = ::read(pipefd[0], buf, sizeof(buf) - 1);
	::close(pipefd[0]);
	(void)n;
	return std::string(buf);
}

static LoggerRecord make_record(LoggerLevel::type lvl, const std::string &msg)
{
	return LoggerRecord(lvl, utils::str(msg), utils::str("test.cpp"), 42, utils::time());
}

// ── LoggerRecord ──────────────────────────────────────────────────────────────

int test_record_stores_level()
{
	LoggerRecord rec = make_record(LoggerLevel::WARN, "oops");
	return assert_true(rec.level == LoggerLevel::FATAL, "test_record_stores_level", LINE_DATA());
}

int test_record_stores_msg()
{
	LoggerRecord rec = make_record(LoggerLevel::INFO, "hello world");
	return assert_true(rec.msg == utils::str("hello world"), "test_record_stores_msg", LINE_DATA());
}

int test_record_stores_file_and_line()
{
	LoggerRecord rec = make_record(LoggerLevel::DEBUG, "x");
	return assert_true(rec.file == utils::str("test.cpp") && rec.line == 42,
					"test_record_stores_file_and_line", LINE_DATA());
}

int test_record_default_level()
{
	LoggerRecord rec;
	return assert_true(rec.level == LoggerLevel::TRACE, "test_record_default_level", LINE_DATA());
}

// ── LoggerLevel ordering ──────────────────────────────────────────────────────

int test_level_ordering()
{
	bool ok =   LoggerLevel::TRACE < LoggerLevel::DEBUG
			&& LoggerLevel::DEBUG < LoggerLevel::INFO
			&& LoggerLevel::INFO  < LoggerLevel::WARN
			&& LoggerLevel::WARN  < LoggerLevel::ERROR
			&& LoggerLevel::ERROR < LoggerLevel::FATAL;
	return assert_true(ok, "test_level_ordering", LINE_DATA());
}

// ── LoggerFormatterNoStyle ────────────────────────────────────────────────────

int test_formatter_nostyle_contains_msg()
{
	LoggerFormatterNoStyle fmt;
	std::string out = fmt.format(make_record(LoggerLevel::INFO, "my message")).string();
	return assert_true(out.find("my message") != std::string::npos,
					"test_formatter_nostyle_contains_msg", LINE_DATA());
}

int test_formatter_nostyle_contains_file_line()
{
	LoggerFormatterNoStyle fmt;
	std::string out = fmt.format(make_record(LoggerLevel::INFO, "x")).string();
	return assert_true(out.find("test.cpp:42") != std::string::npos,
					"test_formatter_nostyle_contains_file_line", LINE_DATA());
}

int test_formatter_nostyle_ends_with_newline()
{
	LoggerFormatterNoStyle fmt;
	std::string out = fmt.format(make_record(LoggerLevel::INFO, "x")).string();
	return assert_true(!out.empty() && out.back() == '\n',
					"test_formatter_nostyle_ends_with_newline", LINE_DATA());
}

// ── LoggerFormatter coloured variants ────────────────────────────────────────

int test_formatter_trace_contains_msg()
{
	LoggerFormatterTrace fmt;
	std::string out = fmt.format(make_record(LoggerLevel::TRACE, "trace msg")).string();
	return assert_true(out.find("trace msg") != std::string::npos,
					"test_formatter_trace_contains_msg", LINE_DATA());
}

int test_formatter_debug_contains_msg()
{
	LoggerFormatterDebug fmt;
	std::string out = fmt.format(make_record(LoggerLevel::DEBUG, "debug msg")).string();
	return assert_true(out.find("debug msg") != std::string::npos,
					"test_formatter_debug_contains_msg", LINE_DATA());
}

int test_formatter_info_contains_msg()
{
	LoggerFormatterInfo fmt;
	std::string out = fmt.format(make_record(LoggerLevel::INFO, "info msg")).string();
	return assert_true(out.find("info msg") != std::string::npos,
					"test_formatter_info_contains_msg", LINE_DATA());
}

int test_formatter_warn_contains_msg()
{
	LoggerFormatterWarn fmt;
	std::string out = fmt.format(make_record(LoggerLevel::WARN, "warn msg")).string();
	return assert_true(out.find("warn msg") != std::string::npos,
					"test_formatter_warn_contains_msg", LINE_DATA());
}

int test_formatter_error_contains_msg()
{
	LoggerFormatterError fmt;
	std::string out = fmt.format(make_record(LoggerLevel::ERROR, "error msg")).string();
	return assert_true(out.find("error msg") != std::string::npos,
					"test_formatter_error_contains_msg", LINE_DATA());
}

int test_formatter_fatal_contains_msg()
{
	LoggerFormatterFatal fmt;
	std::string out = fmt.format(make_record(LoggerLevel::FATAL, "fatal msg")).string();
	return assert_true(out.find("fatal msg") != std::string::npos,
					"test_formatter_fatal_contains_msg", LINE_DATA());
}

// ── LoggerDestine — isInRange ─────────────────────────────────────────────────

int test_destine_range_accepts_min()
{
	LoggerDestineOstream d(std::cout, LoggerLevel::INFO, LoggerLevel::WARN);
	return assert_true(d.isInRange(LoggerLevel::INFO), "test_destine_range_accepts_min", LINE_DATA());
}

int test_destine_range_accepts_max()
{
	LoggerDestineOstream d(std::cout, LoggerLevel::INFO, LoggerLevel::WARN);
	return assert_true(d.isInRange(LoggerLevel::WARN), "test_destine_range_accepts_max", LINE_DATA());
}

int test_destine_range_rejects_below()
{
	LoggerDestineOstream d(std::cout, LoggerLevel::INFO, LoggerLevel::WARN);
	return assert_true(!d.isInRange(LoggerLevel::TRACE), "test_destine_range_rejects_below", LINE_DATA());
}

int test_destine_range_rejects_above()
{
	LoggerDestineOstream d(std::cout, LoggerLevel::INFO, LoggerLevel::WARN);
	return assert_true(!d.isInRange(LoggerLevel::ERROR), "test_destine_range_rejects_above", LINE_DATA());
}

int test_destine_single_level_accepts()
{
	LoggerDestineOstream d(std::cout, LoggerLevel::ERROR);
	return assert_true( d.isInRange(LoggerLevel::ERROR) &&
					!d.isInRange(LoggerLevel::WARN)  &&
					!d.isInRange(LoggerLevel::FATAL),
					"test_destine_single_level_accepts", LINE_DATA());
}

// ── LoggerDestineOstream — write ──────────────────────────────────────────────

int test_destine_ostream_writes_when_in_range()
{
	std::ostringstream oss;
	{
		LoggerDestineOstream d(oss, LoggerLevel::INFO, LoggerLevel::FATAL);
		d.write(make_record(LoggerLevel::ERROR, "ostream write test"));
	}
	return assert_true(oss.str().find("ostream write test") != std::string::npos,
					"test_destine_ostream_writes_when_in_range", LINE_DATA());
}

int test_destine_ostream_silent_when_out_of_range()
{
	std::ostringstream oss;
	{
		LoggerDestineOstream d(oss, LoggerLevel::WARN, LoggerLevel::FATAL);
		d.write(make_record(LoggerLevel::DEBUG, "should not appear"));
	}
	return assert_true(oss.str().empty(),
					"test_destine_ostream_silent_when_out_of_range", LINE_DATA());
}

// ── LoggerDestine — custom formatter ──────────────────────────────────────────

struct PrefixFormatter : public LoggerFormatter
{
	utils::str format(const LoggerRecord &rec)
	{
		return utils::str("PREFIX:" + rec.msg.string() + "\n");
	}
};

int test_destine_custom_formatter_used_for_level()
{
	std::ostringstream oss;
	{
		LoggerDestineOstream d(oss, LoggerLevel::TRACE, LoggerLevel::FATAL);
		d.addFormatter(LoggerLevel::DEBUG, new PrefixFormatter());
		d.write(make_record(LoggerLevel::DEBUG, "custom"));
	}
	return assert_true(oss.str().find("PREFIX:custom") != std::string::npos,
					"test_destine_custom_formatter_used_for_level", LINE_DATA());
}

int test_destine_other_levels_use_default_formatter()
{
	std::ostringstream oss;
	{
		LoggerDestineOstream d(oss, LoggerLevel::TRACE, LoggerLevel::FATAL);
		d.addFormatter(LoggerLevel::DEBUG, new PrefixFormatter());
		d.write(make_record(LoggerLevel::INFO, "info without prefix"));
	}
	return assert_true(oss.str().find("PREFIX:") == std::string::npos,
					"test_destine_other_levels_use_default_formatter", LINE_DATA());
}

// ── LoggerDestineFile ─────────────────────────────────────────────────────────

int test_destine_file_creates_file()
{
	const std::string path = "/tmp/logger_test_create.log";
	std::remove(path.c_str());
	{ LoggerDestineFile d(path, LoggerLevel::TRACE, LoggerLevel::FATAL); }
	std::ifstream f(path.c_str());
	bool exists = f.good();
	std::remove(path.c_str());
	return assert_true(exists, "test_destine_file_creates_file", LINE_DATA());
}

int test_destine_file_writes_message()
{
	const std::string path = "/tmp/logger_test_write.log";
	{
		LoggerDestineFile d(path, LoggerLevel::TRACE, LoggerLevel::FATAL);
		d.write(make_record(LoggerLevel::INFO, "file write test"));
	}
	std::ifstream f(path.c_str());
	std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	std::remove(path.c_str());
	return assert_true(content.find("file write test") != std::string::npos,
					"test_destine_file_writes_message", LINE_DATA());
}

int test_destine_file_appends()
{
	const std::string path = "/tmp/logger_test_append.log";
	{
		LoggerDestineFile d(path, LoggerLevel::TRACE, LoggerLevel::FATAL);
		d.write(make_record(LoggerLevel::INFO, "first"));
		d.write(make_record(LoggerLevel::INFO, "second"));
	}
	std::ifstream f(path.c_str());
	std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	std::remove(path.c_str());
	return assert_true(content.find("first") != std::string::npos &&
					content.find("second") != std::string::npos,
					"test_destine_file_appends", LINE_DATA());
}

int test_destine_file_silent_out_of_range()
{
	const std::string path = "/tmp/logger_test_oor.log";
	{
		LoggerDestineFile d(path, LoggerLevel::ERROR, LoggerLevel::FATAL);
		d.write(make_record(LoggerLevel::DEBUG, "should not be here"));
	}
	std::ifstream f(path.c_str());
	std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	std::remove(path.c_str());
	return assert_true(content.find("should not be here") == std::string::npos,
					"test_destine_file_silent_out_of_range", LINE_DATA());
}

static void helper_bad_path_write()
{
	// Constructor attempts to open the file and logs the error to stderr.
	LoggerDestineFile d("/no_such_dir/bad.log", LoggerLevel::TRACE, LoggerLevel::FATAL);
	(void)d;
}

int test_destine_file_bad_path_logs_to_stderr()
{
	std::string captured = capture_fd(STDERR_FILENO, helper_bad_path_write);
	return assert_true(captured.find("cannot open file") != std::string::npos,
					"test_destine_file_bad_path_logs_to_stderr", LINE_DATA());
}

// ── Logger (singleton) tests ──────────────────────────────────────────────────
//
// The Logger singleton keeps every added destine alive for the process lifetime.
// To avoid use-after-free, we back each capture ostream with a static buffer
// that also lives for the process lifetime.
// We use a new static oss for each test (by index), reset before the test.

// Storage pool: one static oss per Logger test (enough for all tests below).
static std::ostringstream g_oss[8];

static void reset_oss(int idx) { g_oss[idx].str(""); g_oss[idx].clear(); }

// Feature: Logger::log() drops messages below _min_level
int test_logger_filters_below_min_level()
{
	reset_oss(0);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::WARN);
	l.addDestine(new LoggerDestineOstream(g_oss[0], LoggerLevel::TRACE, LoggerLevel::FATAL));

	l.log(LoggerLevel::DEBUG, "dropped", "f.cpp", 1, utils::time());

	return assert_true(g_oss[0].str().empty(), "test_logger_filters_below_min_level", LINE_DATA());
}

// Feature: Logger::log() passes messages at _min_level
int test_logger_passes_at_min_level()
{
	reset_oss(1);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::WARN);
	l.addDestine(new LoggerDestineOstream(g_oss[1], LoggerLevel::TRACE, LoggerLevel::FATAL));

	l.log(LoggerLevel::WARN, "passes", "f.cpp", 1, utils::time());

	return assert_true(g_oss[1].str().find("passes") != std::string::npos,
					"test_logger_passes_at_min_level", LINE_DATA());
}

// Feature: Logger::log() passes messages above _min_level
int test_logger_passes_above_min_level()
{
	reset_oss(2);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::INFO);
	l.addDestine(new LoggerDestineOstream(g_oss[2], LoggerLevel::TRACE, LoggerLevel::FATAL));

	l.log(LoggerLevel::FATAL, "fatal passes", "f.cpp", 1, utils::time());

	return assert_true(g_oss[2].str().find("fatal passes") != std::string::npos,
					"test_logger_passes_above_min_level", LINE_DATA());
}

// Feature: static level helpers (levelTrace…levelFatal) set the min level
int test_logger_static_level_helpers()
{
	reset_oss(3);
	Logger::levelError();
	Logger &l = Logger::getInstance();
	l.addDestine(new LoggerDestineOstream(g_oss[3], LoggerLevel::TRACE, LoggerLevel::FATAL));

	l.log(LoggerLevel::WARN,  "suppressed", "f.cpp", 1, utils::time());
	l.log(LoggerLevel::FATAL, "visible",    "f.cpp", 1, utils::time());

	bool ok = g_oss[3].str().find("suppressed") == std::string::npos
		&& g_oss[3].str().find("visible")    != std::string::npos;
	return assert_true(ok, "test_logger_static_level_helpers", LINE_DATA());
}

// ── Logger macros ─────────────────────────────────────────────────────────────

int test_macro_log_info()
{
	reset_oss(4);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::TRACE);
	l.addDestine(new LoggerDestineOstream(g_oss[4], LoggerLevel::TRACE, LoggerLevel::FATAL));

	LOG_INFO("macro info test");

	return assert_true(g_oss[4].str().find("macro info test") != std::string::npos,
					"test_macro_log_info", LINE_DATA());
}

int test_macro_log_error_captures_source_location()
{
	reset_oss(5);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::TRACE);
	l.addDestine(new LoggerDestineOstream(g_oss[5], LoggerLevel::TRACE, LoggerLevel::FATAL));

	LOG_ERROR("location check");

	return assert_true(g_oss[5].str().find("test_logger.cpp") != std::string::npos,
					"test_macro_log_error_captures_source_location", LINE_DATA());
}

int test_macro_log_trace_suppressed()
{
	reset_oss(6);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::DEBUG);
	l.addDestine(new LoggerDestineOstream(g_oss[6], LoggerLevel::TRACE, LoggerLevel::FATAL));

	LOG_TRACE("trace should be gone");

	return assert_true(g_oss[6].str().find("trace should be gone") == std::string::npos,
					"test_macro_log_trace_suppressed", LINE_DATA());
}

int test_macro_stream_expression()
{
	reset_oss(7);
	Logger &l = Logger::getInstance();
	l.setMinLevel(LoggerLevel::TRACE);
	l.addDestine(new LoggerDestineOstream(g_oss[7], LoggerLevel::TRACE, LoggerLevel::FATAL));

	int val = 99;
	LOG_DEBUG("value=" << val);

	return assert_true(g_oss[7].str().find("value=99") != std::string::npos,
					"test_macro_stream_expression", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main()
{
	int failures = 0;

	std::cout << "\n=== LoggerRecord ===\n";
	failures += test_record_stores_level();
	failures += test_record_stores_msg();
	failures += test_record_stores_file_and_line();
	failures += test_record_default_level();

	std::cout << "\n=== LoggerLevel ordering ===\n";
	failures += test_level_ordering();

	std::cout << "\n=== LoggerFormatterNoStyle ===\n";
	failures += test_formatter_nostyle_contains_msg();
	failures += test_formatter_nostyle_contains_file_line();
	failures += test_formatter_nostyle_ends_with_newline();

	std::cout << "\n=== LoggerFormatter coloured variants ===\n";
	failures += test_formatter_trace_contains_msg();
	failures += test_formatter_debug_contains_msg();
	failures += test_formatter_info_contains_msg();
	failures += test_formatter_warn_contains_msg();
	failures += test_formatter_error_contains_msg();
	failures += test_formatter_fatal_contains_msg();

	std::cout << "\n=== LoggerDestine — isInRange ===\n";
	failures += test_destine_range_accepts_min();
	failures += test_destine_range_accepts_max();
	failures += test_destine_range_rejects_below();
	failures += test_destine_range_rejects_above();
	failures += test_destine_single_level_accepts();

	std::cout << "\n=== LoggerDestineOstream — write ===\n";
	failures += test_destine_ostream_writes_when_in_range();
	failures += test_destine_ostream_silent_when_out_of_range();

	std::cout << "\n=== LoggerDestine — custom formatter ===\n";
	failures += test_destine_custom_formatter_used_for_level();
	failures += test_destine_other_levels_use_default_formatter();

	std::cout << "\n=== LoggerDestineFile ===\n";
	failures += test_destine_file_creates_file();
	failures += test_destine_file_writes_message();
	failures += test_destine_file_appends();
	failures += test_destine_file_silent_out_of_range();
	failures += test_destine_file_bad_path_logs_to_stderr();

	std::cout << "\n=== Logger singleton — filtering ===\n";
	failures += test_logger_filters_below_min_level();
	failures += test_logger_passes_at_min_level();
	failures += test_logger_passes_above_min_level();
	failures += test_logger_static_level_helpers();

	std::cout << "\n=== Logger macros ===\n";
	failures += test_macro_log_info();
	failures += test_macro_log_error_captures_source_location();
	failures += test_macro_log_trace_suppressed();
	failures += test_macro_stream_expression();

	std::cout << "\n──────────────────────────────────────────\n";
	if (failures == 0)
		std::cout << "\033[32mAll tests passed.\033[0m\n";
	else
		std::cout << "\033[31m" << failures << " test(s) failed.\033[0m\n";

	return failures;
}
