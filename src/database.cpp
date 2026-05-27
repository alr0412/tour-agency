#include <database.h>
#include <iostream>

sqlite3* open_database(const std::string& path){
    sqlite3* db;
    int return_code = sqlite3_open(path.c_str(), &db);
    if(return_code!=SQLITE_OK){
        std::cerr << "Ошибка открытия БД: " << sqlite3_errmsg(db) << '\n';
        return nullptr;
    }

    sqlite3_exec(db, "PRAGMA encoding = 'UTF-8';", nullptr, nullptr, nullptr);
    
     const char* create_tables = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT NOT NULL UNIQUE,
            pass_hash TEXT NOT NULL,
            role TEXT NOT NULL DEFAULT 'USER'
        );
        
        CREATE TABLE IF NOT EXISTS tours (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            country TEXT NOT NULL,
            price INTEGER NOT NULL,
            date TEXT NOT NULL,
            length INTEGER NOT NULL
        );
        
        INSERT OR IGNORE INTO users (login, pass_hash, role) 
        VALUES ('admin', 'f12fc8e', 'ADMIN');
    )";

    char* err_msg = nullptr;
    return_code = sqlite3_exec(db, create_tables, nullptr, nullptr, &err_msg);

    if(return_code!=SQLITE_OK){
        std::cerr << "Ошибка создания таблиц: " << err_msg << '\n';
        sqlite3_free(err_msg);
    }

    return db;
}

void close_database(sqlite3* db)
{
    if (db) sqlite3_close(db);
}