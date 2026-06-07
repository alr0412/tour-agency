#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

sqlite3* open_database(const std::string& path);
void close_database(sqlite3* db);
std::vector<unsigned char> read_image_file(const std::string& filepath);