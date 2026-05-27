#pragma once
#include <sqlite3.h>
#include <string>

sqlite3* open_database(const std::string& path);
void close_database(sqlite3* db);