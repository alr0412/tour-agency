#include "tour.h"
#include "user.h"
#include "ui.h"
#include "filter.h"
#include "admin.h"
#include "database.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

static sqlite3* g_db = nullptr;

// Авторизация

static User *do_login(std::vector<User> &users)
{
    std::string login = read_string("  Логин: ");
    std::string password = read_string("  Пароль: ");
    User *u = find_user(users, login);
    if (u && u->pass_hash == hash_password(password))
    {
        std::cout << "  Добро пожаловать, " << login
                  << "! Роль: " << (u->role == Role::ADMIN ? "ADMIN" : "USER") << '\n';
        return u;
    }
    std::cout << "  [!] Неверный логин или пароль.\n";
    return nullptr;
}

static void do_register(std::vector<User> &users)
{
    std::string login = read_string("  Придумайте логин: ");
    if (find_user(users, login))
    {
        std::cout << "  [!] Такой логин уже занят.\n";
        return;
    }
    std::string password = read_string("  Придумайте пароль: ");
    
    User new_user;
    new_user.login = login;
    new_user.pass_hash = hash_password(password);
    new_user.role = Role::USER;
    
    save_user(g_db, new_user);
    users.push_back(new_user);
    
    std::cout << "  - Регистрация успешна! Роль: USER\n";
}

static User *auth_loop(std::vector<User> &users)
{
    while (true)
    {
        std::cout << "\n── Авторизация ────────────────────────────────\n"
                  << "  1) Войти\n  2) Зарегистрироваться\n  0) Выйти из программы\n"
                  << "───────────────────────────────────────────────\n> ";
        int ch = read_int("");
        if (ch == 0)
        {
            std::cout << "До свидания!\n";
            return nullptr;
        }
        if (ch == 1)
        {
            User *u = do_login(users);
            if (u)
                return u;
        }
        else if (ch == 2)
        {
            do_register(users);
        }
        else
        {
            std::cout << "  [!] Неверный пункт.\n";
        }
    }
}

// Главное меню

static void main_menu(User &user, std::vector<User> &users,
                      std::vector<Tour> &tours)
{
    SearchFilters filters;

    while (true)
    {
        bool is_admin = (user.role == Role::ADMIN);

        std::string dashes;
        int n = user.login.length() < 28 ? 28 - (int)user.login.length() : 0;
        for (int i = 0; i < n; i++)
            dashes += "─";
        std::cout << "\n─── Главное меню [" << user.login << "] " << dashes << "\n"
                  << "  1) Поиск тура (с фильтрами)\n"
                  << "  2) Показать все туры\n"
                  << "  3) Сменить валюту\n";
        if (is_admin)
        {
            std::cout
                << "─── Администрирование ──────────────────────────\n"
                << "  4) Добавить тур\n"
                << "  5) Редактировать тур\n"
                << "  6) Удалить тур\n"
                << "  7) Список пользователей\n"
                << "  8) Изменить роль пользователя\n";
        }
        std::cout << "────────────────────────────────────────────────\n"
                  << "  0) Выйти из аккаунта\n"
                  << "────────────────────────────────────────────────\n> ";

        int ch = read_int("");
        switch (ch)
        {
        case 0:
            std::cout << "  Выход из аккаунта.\n";
            return;
        case 1:
            filters_menu(filters, tours);
            break;
        case 2:
            print_tours_table(tours, 1,0);
            break;
        case 3:
            change_currency();
            break;
        case 4: 
            if (is_admin)
                admin_add_tour(g_db, tours);
            else
                std::cout << "  [!] Нет доступа.\n";
            break;
        case 5:
            if (is_admin)
                admin_edit_tour(g_db, tours);
            else
                std::cout << "  [!] Нет доступа.\n";
            break;
        case 6:
            if (is_admin)
                admin_delete_tour(g_db, tours);
            else
                std::cout << "  [!] Нет доступа.\n";
            break;
        case 7:
            if (is_admin)
                admin_list_users(users);
            else
                std::cout << "  [!] Нет доступа.\n";
            break;
        case 8:
            if (is_admin)
                admin_change_role(g_db, users);
            else
                std::cout << "  [!] Нет доступа.\n";
            break;
        default:
            std::cout << "  [!] Неверный пункт меню.\n";
        }
    }
}

// main

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "ru_RU.UTF-8");

    std::cout << "╔═════════════════════════════════════════════╗\n"
              << "║    СИСТЕМА ПОДБОРА ТУРИСТИЧЕСКИХ ПУТЁВОК    ║\n"
              << "╚═════════════════════════════════════════════╝\n";

    g_db = open_database("tourism.db");
    if (!g_db) return 1;

    // Загрузка данных
    std::vector<User> users = load_users(g_db);
    std::vector<Tour> tours = load_tours(g_db);
    
    // Главный цикл: авторизация, меню, снова авторизация
    while (true)
    {
        User *current = auth_loop(users);
        if (!current)
            break; // пользователь выбрал «Выйти из программы»
        main_menu(*current, users, tours);
        // после выхода из аккаунта снова предлагаем войти
    }

    std::cout << "До свидания!\n";
    close_database(g_db);
    return 0;
}
