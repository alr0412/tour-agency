#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <set>

// ───────────────── Дата ─────────────────

struct Date {
    int day = 1, month = 1, year = 2000;

    bool valid() const {
        return year >= 2000 && month >= 1 && month <= 12 && day >= 1 && day <= 31;
    }
    bool operator<=(const Date& o) const {
        if (year != o.year) return year <= o.year;
        if (month != o.month) return month <= o.month;
        return day <= o.day;
    }
    bool operator>=(const Date& o) const {
        if (year != o.year) return year >= o.year;
        if (month != o.month) return month >= o.month;
        return day >= o.day;
    }
    std::string to_str() const;
    static Date from_str(const std::string& s); 
};

// ───────────────── Тур ─────────────────

struct Tour {
    int id = 0;
    std::string name;
    std::string country;
    int price = 0;
    Date date;
    int length = 0;   // дней
};

// ───────────────── I/O ─────────────────

std::vector<Tour> load_tours(sqlite3* db);
void save_tour(sqlite3* db, const Tour& tour);
void update_tour(sqlite3* db, const Tour& tour);
void delete_tour(sqlite3* db, int id);

// ───────────────── Вспомогательное ─────────────────

// Уникальные страны из вектора туров (отсортированные)
std::set<std::string> countries_from_tours(const std::vector<Tour>& tours);
    
// Следующий свободный ID (max + 1)
int next_id(const std::vector<Tour>& tours);
