#include "user.h"
#include <fstream>
#include <sstream>
#include <cstdint>

std::string hash_password(const std::string &pw)
{
    uint32_t h = 5381;
    for (unsigned char c : pw)
        h = ((h << 5) + h) + c;
    std::ostringstream ss;
    ss << std::hex << h;
    return ss.str();
}

std::vector<User> load_users(sqlite3* db)
{
    std::vector<User> users;
    const char* sql = "SELECT id, login, pass_hash, role FROM users;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            User u;
            u.id        = sqlite3_column_int(stmt, 0);
            u.login     = (const char*)(sqlite3_column_text(stmt, 1));
            u.pass_hash = (const char*)(sqlite3_column_text(stmt, 2));
            
            std::string role_str = (const char*)(sqlite3_column_text(stmt, 3));
            u.role = (role_str == "ADMIN") ? Role::ADMIN : Role::USER;
            
            users.push_back(u);
        }
        sqlite3_finalize(stmt);
    }
    return users;
}

void save_user(sqlite3* db, const User& user)
{
    const char* sql = "INSERT INTO users (login, pass_hash, role) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, user.login.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, user.pass_hash.c_str(), -1, SQLITE_TRANSIENT);
        std::string role_str = (user.role == Role::ADMIN) ? "ADMIN" : "USER";
        sqlite3_bind_text(stmt, 3, role_str.c_str(), -1, SQLITE_TRANSIENT);
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void update_user_role(sqlite3* db, int id, Role role)
{
    const char* sql = "UPDATE users SET role = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        std::string role_str = (role == Role::ADMIN) ? "ADMIN" : "USER";
        sqlite3_bind_text(stmt, 1, role_str.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, id);
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

User *find_user(std::vector<User> &users, const std::string &login)
{
    for (auto &u : users)
        if (u.login == login)
            return &u;
    return nullptr;
}
