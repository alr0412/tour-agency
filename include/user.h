#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

enum class Role { USER, ADMIN };

struct User {
    int id = 0;
    std::string login;
    std::string pass_hash;
    Role role = Role::USER;
};

// djb2-хеш
std::string hash_password(const std::string& pw);

std::vector<User> load_users(sqlite3* db);
void save_user(sqlite3* db, const User& user);
void update_user_role(sqlite3* db, int id, Role role);

// Возвращает указатель на пользователя или nullptr
User* find_user(std::vector<User>& users, const std::string& login);
