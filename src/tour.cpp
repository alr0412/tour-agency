#include "tour.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// Date

std::string Date::to_str() const
{
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d-%02d-%04d", day, month, year);
    return buf;
}

Date Date::from_str(const std::string &s)
{
    Date d;
    // Формат: ДД-ММ-ГГГГ
    if (s.size() == 10 && s[2] == '-' && s[5] == '-')
    {
        try
        {
            d.day   = std::stoi(s.substr(0, 2));
            d.month = std::stoi(s.substr(3, 2));
            d.year  = std::stoi(s.substr(6, 4));
        }
        catch (...) {}
    }
    return d;
}

// I/O

std::vector<Tour> load_tours(sqlite3* db)
{
    std::vector<Tour> tours;
    const char* sql = "SELECT id, name, country, price, date, length FROM tours;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Tour t;
            t.id      = sqlite3_column_int(stmt, 0);
            t.name    = (const char*)(sqlite3_column_text(stmt, 1));
            t.country = (const char*)(sqlite3_column_text(stmt, 2));
            t.price   = sqlite3_column_int(stmt, 3);
            t.date    = Date::from_str(
                (const char*)(sqlite3_column_text(stmt, 4))
            );
            t.length  = sqlite3_column_int(stmt, 5);
            tours.push_back(t);
        }
        sqlite3_finalize(stmt);
    }
    return tours;
}

void save_tour(sqlite3* db, const Tour& tour)
{
    const char* sql = "INSERT INTO tours (name, country, price, date, length) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, tour.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, tour.country.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, tour.price);
        std::string date_str = tour.date.to_str();  // просто to_str()
        sqlite3_bind_text(stmt, 4, date_str.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, tour.length);
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void update_tour(sqlite3* db, const Tour& tour)
{
    const char* sql = "UPDATE tours SET name=?, country=?, price=?, date=?, length=? WHERE id=?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, tour.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, tour.country.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, tour.price);
        std::string date_str = tour.date.to_str();  // просто to_str()
        sqlite3_bind_text(stmt, 4, date_str.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, tour.length);
        sqlite3_bind_int(stmt, 6, tour.id);
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void delete_tour(sqlite3* db, int id)
{
    const char* sql = "DELETE FROM tours WHERE id=?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
// Helpers

std::set<std::string> countries_from_tours(const std::vector<Tour> &tours)
{
    std::set<std::string> s;
    for (const auto &t : tours)
        s.insert(t.country);
    return s;
}

int next_id(const std::vector<Tour> &tours)
{
    int mx = 0;
    for (const auto &t : tours)
        mx = std::max(mx, t.id);
    return mx + 1;
}
