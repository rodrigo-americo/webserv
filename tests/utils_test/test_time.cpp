/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_time.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 14:58:07 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/18 14:58:10 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "time.hpp"

#include <sstream>
#include <ctime>

typedef utils::time         utime;
typedef utils::time_month   month;
typedef utils::time_week_day wday;

// ── Construtor / getters básicos ──────────────────────────────────────────────

// Feature: construtor default inicializa a partir do horário atual do sistema
int test_ctor_default_matches_system_time()
{
	std::time_t now = std::time(NULL);
	struct tm *sys_tm = std::localtime(&now);

	utime t;

	// Pequena tolerância: comparar ano/mês/dia, que não mudam entre as duas chamadas
	bool ok = t.year()  == sys_tm->tm_year + 1900
		&& t.mon()   == sys_tm->tm_mon
		&& t.mday()  == sys_tm->tm_mday;
	return assert_true(ok, "test_ctor_default_matches_system_time", LINE_DATA());
}

// Feature: sec() getter/setter
int test_sec_getter_setter()
{
	utime t;
	t.sec(45);
	return assert_true(t.sec() == 45, "test_sec_getter_setter", LINE_DATA());
}

// Feature: min() getter/setter
int test_min_getter_setter()
{
	utime t;
	t.min(30);
	return assert_true(t.min() == 30, "test_min_getter_setter", LINE_DATA());
}

// Feature: hour() getter/setter
int test_hour_getter_setter()
{
	utime t;
	t.hour(14);
	return assert_true(t.hour() == 14, "test_hour_getter_setter", LINE_DATA());
}

// Feature: mday() getter/setter
int test_mday_getter_setter()
{
	utime t;
	t.mday(15);
	return assert_true(t.mday() == 15, "test_mday_getter_setter", LINE_DATA());
}

// Feature: mon() getter/setter usando o enum time_month
int test_mon_getter_setter()
{
	utime t;
	t.mon(month::MARCH);
	return assert_true(t.mon() == month::MARCH, "test_mon_getter_setter", LINE_DATA());
}

// Feature: year() getter/setter — internamente soma/subtrai 1900 (tm_year)
int test_year_getter_setter()
{
	utime t;
	t.year(1999);
	return assert_true(t.year() == 1999, "test_year_getter_setter", LINE_DATA());
}

// Feature: yday() getter/setter
int test_yday_getter_setter()
{
	utime t;
	t.yday(100);
	return assert_true(t.yday() == 100, "test_yday_getter_setter", LINE_DATA());
}

// Feature: isdst() getter/setter
int test_isdst_getter_setter()
{
	utime t;
	t.isdst(0);
	return assert_true(t.isdst() == 0, "test_isdst_getter_setter", LINE_DATA());
}

// Feature: wday() getter/setter — usando o enum time_week_day, o valor numérico
// é fielmente armazenado e devolvido (a falha está na impressão, não aqui).
int test_wday_getter_setter_numeric()
{
	utime t;
	t.wday(wday::FRIDAY);
	return assert(wday::FRIDAY, t.wday(), "test_wday_getter_setter_numeric", LINE_DATA());
}

// Feature: setters encadeáveis (fluent interface) — todos retornam `time&`
int test_setters_are_chainable()
{
	utime t;
	t.year(2024).mon(month::JUNE).mday(10).hour(8).min(15).sec(0);
	bool ok = t.year() == 2024 && t.mon() == month::JUNE && t.mday() == 10
		&& t.hour() == 8 && t.min() == 15 && t.sec() == 0;
	return assert_true(ok, "test_setters_are_chainable", LINE_DATA());
}

// ── format() ──────────────────────────────────────────────────────────────────

// Feature: format() com argumento explícito usa o formato passado
int test_format_explicit()
{
	utime t;
	t.year(2024).mon(month::MARCH).mday(5).hour(9).min(30).sec(15);
	std::string out = t.format("%Y/%m/%d").string();
	return assert_true(out == "2024/03/05", "test_format_explicit", LINE_DATA());
}

// Feature: format() sem argumento usa o formato default ("%Y-%m-%d %H:%M:%S")
int test_format_default()
{
	utime t;
	t.year(2024).mon(month::MARCH).mday(5).hour(9).min(30).sec(15);
	std::string out = t.format().string();
	return assert_true(out == "2024-03-05 09:30:15", "test_format_default", LINE_DATA());
}

// Feature: formatIso() segue o padrão ISO-8601
int test_format_iso()
{
	utime t;
	t.year(2024).mon(month::MARCH).mday(5).hour(9).min(30).sec(15);
	std::string out = t.formatIso().string();
	return assert_true(out == "2024-03-05T09:30:15", "test_format_iso", LINE_DATA());
}

// Feature: formatLog() é igual ao formato default ("%Y-%m-%d %H:%M:%S")
int test_format_log()
{
	utime t;
	t.year(2024).mon(month::MARCH).mday(5).hour(9).min(30).sec(15);
	std::string out = t.formatLog().string();
	return assert_true(out == "2024-03-05 09:30:15", "test_format_log", LINE_DATA());
}

// Feature: formatEmail() segue o padrão RFC tipo "Wed, 05 Mar 2024 09:30:15"
// (depende de wday estar consistente — testamos uma data cujo wday real
// bate com year/mon/mday calculados via normalize()+mktime)
int test_format_email()
{
	utime t;
	t.year(2024).mon(month::MARCH).mday(5).hour(9).min(30).sec(15);
	t.normalize(); // recalcula tm_wday corretamente via mktime
	std::string out = t.formatEmail().string();
	// 2024-03-05 é uma terça-feira
	return assert_true(out == "Tue, 05 Mar 2024 09:30:15", "test_format_email", LINE_DATA());
}

// ── setFormat() ───────────────────────────────────────────────────────────────

// Feature: setFormat() altera o formato usado por format() sem argumento
int test_setFormat_changes_default()
{
	utime t;
	t.year(2024).mon(month::JANUARY).mday(1);
	t.setFormat("%Y");
	return assert_true(t.format().string() == "2024", "test_setFormat_changes_default", LINE_DATA());
}

// Feature: setFormat() com string vazia é ignorado (formato permanece o anterior)
int test_setFormat_empty_is_ignored()
{
	utime t;
	t.year(2024).mon(month::JANUARY).mday(1);
	t.setFormat("%Y");          // formato válido primeiro
	t.setFormat("");            // deve ser ignorado
	return assert_true(t.format().string() == "2024",
					"test_setFormat_empty_is_ignored", LINE_DATA());
}

int test_setFormat_trailing_whitespace_not_trimmed()
{
	utime t;
	t.year(2024).mon(month::JANUARY).mday(1);
	t.setFormat("%Y   "); // contém espaços à direita
	std::string out = t.format().string();
	// Comportamento atual: "2024   " (espaços preservados, vieram do format string)
	return assert_true(out == "2024   ",
					"test_setFormat_trailing_whitespace_not_trimmed", LINE_DATA());
}

// Feature: setFormat() com string apenas de espaços é corretamente rejeitado
// (esse caso específico funciona porque o `first == last` do trim() detecta
// "tudo espaço" e zera a string via clear(), independentemente do bug de
// trailing — ver SKILL str.hpp).
int test_setFormat_only_whitespace_is_ignored()
{
	utime t;
	t.year(2024).mon(month::JANUARY).mday(1);
	t.setFormat("%Y");     // formato válido primeiro
	t.setFormat("   ");    // só espaços -> deve ser ignorado
	return assert_true(t.format().string() == "2024",
					"test_setFormat_only_whitespace_is_ignored", LINE_DATA());
}


int test_objects_are_not_equal_after_year_change()
{
	utime a;
	utime b = a;
	b.year(a.year() + 50);

	return assert_true(a != b,
					"test_objects_are_not_equal_after_year_change", LINE_DATA());
}

int test_diff_is_not_zero_after_year_change()
{
	utime a;
	utime b = a;
	b.year(a.year() + 50);

	double diff = b - a;
	return assert_true(diff != 0.0,
					"test_diff_is_not_zero_after_year_change", LINE_DATA());
}

// Feature: chamar normalize() explicitamente corrige o comportamento acima —
// confirma que o mecanismo EXISTE e funciona, só não é automático.
int test_normalize_fixes_comparison_after_field_change()
{
	utime a;
	utime b = a;
	b.year(a.year() + 50);
	b.normalize();

	bool ok = (a != b) && (b.year() == a.year() + 50);
	return assert_true(ok, "test_normalize_fixes_comparison_after_field_change", LINE_DATA());
}

// Feature: normalize() recalcula corretamente tm_wday/tm_yday via mktime
// para uma data conhecida (2024-01-01 é uma segunda-feira, yday 0).
int test_normalize_recalculates_wday_and_yday()
{
	utime t;
	t.year(2024).mon(month::JANUARY).mday(1);
	t.normalize();

	// 2024-01-01 -> tm_wday = 1 (segunda), tm_yday = 0
	bool ok = (int)t.wday() == 1 && t.yday() == 0;
	return assert_true(ok, "test_normalize_recalculates_wday_and_yday", LINE_DATA());
}

// ── Operadores de comparação (sem alteração de campos, apenas timestamps reais) ──

// Feature: duas instâncias copiadas do mesmo timestamp são iguais
int test_operator_eq_same_timestamp()
{
	utime a;
	utime b = a;
	return assert_true(a == b, "test_operator_eq_same_timestamp", LINE_DATA());
}

// Feature: operator!= é o complemento de operator==
int test_operator_neq()
{
	utime a;
	utime b = a;
	b.year(a.year() + 1);
	b.normalize();
	return assert_true(a != b, "test_operator_neq", LINE_DATA());
}

// Feature: operator< / operator> com timestamps reais distintos
int test_operator_lt_gt()
{
	utime a;
	utime b = a;
	b.year(a.year() + 1);
	b.normalize();
	return assert_true(a < b && b > a, "test_operator_lt_gt", LINE_DATA());
}

// Feature: operator<= / operator>= incluem igualdade
int test_operator_lte_gte_equal()
{
	utime a;
	utime b = a;
	return assert_true(a <= b && a >= b, "test_operator_lte_gte_equal", LINE_DATA());
}

// Feature: operator- retorna a diferença em segundos (std::difftime)
int test_operator_minus_positive_diff()
{
	utime a;
	utime b = a;
	b.sec(a.sec() + 10);
	b.normalize();
	double diff = b - a;
	return assert_true(diff == 10.0, "test_operator_minus_positive_diff", LINE_DATA());
}

int test_weekday_stream_operator_aligned()
{
	std::ostringstream oss;
	oss << wday::type(wday::SUNDAY);
	return assert_true(oss.str() == "SUNDAY",
					"test_weekday_stream_operator_aligned", LINE_DATA());
}

int test_weekday_stream_operator_aligned_friday()
{
	std::ostringstream oss;
	oss << wday::type(wday::FRIDAY);
	return assert_true(oss.str() == "FRIDAY",
					"test_weekday_stream_operator_aligned_friday", LINE_DATA());
}

int test_weekday_stream_operator_aligned_saturday_wraps()
{
	std::ostringstream oss;
	oss << wday::type(wday::SATURDAY);
	return assert_true(oss.str() == "SATURDAY",
					"test_weekday_stream_operator_aligned_saturday_wraps", LINE_DATA());
}

int test_month_stream_operator_is_aligned()
{
	std::ostringstream oss;
	oss << month::type(month::MARCH);
	return assert_true(oss.str() == "MARCH", "test_month_stream_operator_is_aligned", LINE_DATA());
}

int test_month_stream_operator_first_and_last()
{
	std::ostringstream oss1, oss2;
	oss1 << month::type(month::JANUARY);
	oss2 << month::type(month::DECEMBER);
	return assert_true(oss1.str() == "JANUARY" && oss2.str() == "DECEMBER",
					"test_month_stream_operator_first_and_last", LINE_DATA());
}

// ── operator<<(ostream&, time) ───────────────────────────────────────────────

// Feature: operator<< para `time` delega para format() (formato default)
int test_time_stream_operator_uses_format()
{
	utime t;
	t.year(2024).mon(month::MARCH).mday(5).hour(9).min(30).sec(15);
	std::ostringstream oss;
	oss << t;
	return assert_true(oss.str() == "2024-03-05 09:30:15",
					"test_time_stream_operator_uses_format", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== Construtor / getters básicos ===\n";
	failures += test_ctor_default_matches_system_time();
	failures += test_sec_getter_setter();
	failures += test_min_getter_setter();
	failures += test_hour_getter_setter();
	failures += test_mday_getter_setter();
	failures += test_mon_getter_setter();
	failures += test_year_getter_setter();
	failures += test_yday_getter_setter();
	failures += test_isdst_getter_setter();
	failures += test_wday_getter_setter_numeric();
	failures += test_setters_are_chainable();

	// std::cout << "\n=== format() ===\n";
	failures += test_format_explicit();
	failures += test_format_default();
	failures += test_format_iso();
	failures += test_format_log();
	failures += test_format_email();

	// std::cout << "\n=== setFormat() ===\n";
	failures += test_setFormat_changes_default();
	failures += test_setFormat_empty_is_ignored();
	failures += test_setFormat_trailing_whitespace_not_trimmed();
	failures += test_setFormat_only_whitespace_is_ignored();

	// std::cout << "\n=== normalize() e dependência para comparação/diferença ===\n";
	failures += test_objects_are_not_equal_after_year_change();
	failures += test_diff_is_not_zero_after_year_change();
	failures += test_normalize_fixes_comparison_after_field_change();
	failures += test_normalize_recalculates_wday_and_yday();

	// std::cout << "\n=== Operadores de comparação ===\n";
	failures += test_operator_eq_same_timestamp();
	failures += test_operator_neq();
	failures += test_operator_lt_gt();
	failures += test_operator_lte_gte_equal();
	failures += test_operator_minus_positive_diff();

	// std::cout << "\n=== BUG: array time_week_day_str desalinhado ===\n";
	failures += test_weekday_stream_operator_aligned();
	failures += test_weekday_stream_operator_aligned_friday();
	failures += test_weekday_stream_operator_aligned_saturday_wraps();
	failures += test_month_stream_operator_is_aligned();
	failures += test_month_stream_operator_first_and_last();

	// std::cout << "\n=== operator<<(ostream&, time) ===\n";
	failures += test_time_stream_operator_uses_format();

	messages::print();

	return failures;
}
