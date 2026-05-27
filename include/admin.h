#pragma once
#include "tour.h"
#include "user.h"
#include <sqlite3.h>

void admin_add_tour(sqlite3* db, std::vector<Tour>& tours);
void admin_edit_tour(sqlite3* db, std::vector<Tour>& tours);
void admin_delete_tour(sqlite3* db, std::vector<Tour>& tours);
void admin_list_users(const std::vector<User>& users);
void admin_change_role(sqlite3* db, std::vector<User>& users);
