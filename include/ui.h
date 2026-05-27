#pragma once
#include "tour.h"
#include <string>
#include <vector>

// ───────────────── Безопасный ввод ─────────────────

int  read_int(const std::string& prompt);
std::string read_string(const std::string& prompt);
bool is_integer(const std::string& s);

// Ввод даты с валидацией, возвращает Date
Date read_date(const std::string& prompt);

// ───────────────── Таблица туров ─────────────────

// Печатает таблицу. show_index=true добавляет столбец порядкового номера
void print_tours_table(const std::vector<Tour>& tours);
