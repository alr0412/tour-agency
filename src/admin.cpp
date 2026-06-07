#include "admin.h"
#include "ui.h"
#include <iostream>
#include <algorithm>
#include "database.h"
// Работа с турами

void admin_add_tour(sqlite3* db, std::vector<Tour> &tours)
{
    Tour t;
    t.id = next_id(tours);
    t.name = read_string("  Название тура: ");
    t.country = read_string("  Страна: ");

    Currency cur = get_current_currency();
    std::string cur_symbol = currency_symbol(cur);
    int price_local = std::max(0, read_int("  Цена (" + cur_symbol + "): "));
    t.price = convert_to_rub(price_local, cur);

    t.date = read_date("  Дата вылета");
    t.length = std::max(1, read_int("  Продолжительность (дней): "));

    std::string photo_path;
    std::cout << "  Путь к фото (Enter чтобы пропустить): ";
    std::cin >> std::ws;
    std::getline(std::cin, photo_path);
    if (!photo_path.empty()) {
        t.photo = read_image_file(photo_path);
        if (t.photo.empty())
            std::cout << "  [!] Не удалось прочитать файл.\n";
    }
    
    tours.push_back(t);
    save_tour(db, t);
    std::cout << "  - Тур добавлен с ID = " << t.id << ".\n";
}

void admin_edit_tour(sqlite3* db, std::vector<Tour> &tours)
{
    int id = read_int("  ID тура для редактирования: ");
    auto it = std::find_if(tours.begin(), tours.end(),
                           [id](const Tour &t) { return t.id == id; });
    if (it == tours.end())
    {
        std::cout << "  [!] Тур с ID=" << id << " не найден.\n";
        return;
    }
    print_tours_table({*it});

    std::cout << "  Что изменить?\n"
              << "  1) Название\n  2) Страна\n  3) Цена\n"
              << "  4) Дата вылета\n  5) Продолжительность\n"
              << "  6) Фото\n  0) Отмена\n> ";
    switch (read_int(""))
    {
    case 1: it->name = read_string("  Новое название: "); break;
    case 2: it->country = read_string("  Новая страна: "); break;
    case 3: 
    {
        Currency cur = get_current_currency();
        std::string cur_symbol = currency_symbol(cur);
        int price_local = std::max(0, read_int("  Новая цена (" + cur_symbol + "): "));
        it->price = convert_to_rub(price_local, cur);
        break;
    }
    case 4: it->date = read_date("  Новая дата"); break;
    case 5: it->length = std::max(1, read_int("  Новая продолжительность: ")); break;
    case 6: {
        std::string photo_path;
        std::cout << "  Путь к новому фото (Enter чтобы удалить текущее): ";
        std::cin >> std::ws;
        std::getline(std::cin, photo_path);
        if (photo_path.empty()) {
            it->photo.clear();
            std::cout << "  - Фото удалено.\n";
        } else {
            std::vector<unsigned char> new_photo = read_image_file(photo_path);
            if (!new_photo.empty()) {
                it->photo = std::move(new_photo);
                std::cout << "  - Фото заменено.\n";
            } else {
                std::cout << "  [!] Не удалось прочитать файл, фото не изменено.\n";
            }
        }
        break;
    }
    default: std::cout << "  Отменено.\n"; return;
    }
    update_tour(db, *it);
    std::cout << "  - Тур обновлён.\n";
}

void admin_delete_tour(sqlite3* db, std::vector<Tour> &tours)
{
    int id = read_int("  ID тура для удаления: ");
    auto it = std::find_if(tours.begin(), tours.end(),
                           [id](const Tour &t) { return t.id == id; });
    if (it == tours.end())
    {
        std::cout << "  [!] Тур с ID=" << id << " не найден.\n";
        return;
    }
    print_tours_table({*it});
    std::cout << "  Подтвердите удаление: 1) Удалить  0) Отмена\n> ";
    if (read_int("") == 1)
    {
        delete_tour(db, id);    // ← раньше было save_tours(tours, path)
        tours.erase(it);
        std::cout << "  - Тур удалён.\n";
    }
    else
    {
        std::cout << "  Отменено.\n";
    }
}

// Работа с пользователями

void admin_list_users(const std::vector<User> &users)
{
    std::cout << "\n  Зарегистрированные пользователи:\n";
    std::cout << "  " << std::string(30, '-') << '\n';
    for (const auto &u : users)
    {
        std::cout << "  " << u.login
                  << " [" << (u.role == Role::ADMIN ? "ADMIN" : "USER") << "]\n";
    }
    std::cout << "  " << std::string(30, '-') << '\n';
}

void admin_change_role(sqlite3* db, std::vector<User> &users)
{
    admin_list_users(users);
    std::string login = read_string("  Логин пользователя: ");
    User *u = find_user(users, login);
    if (!u)
    {
        std::cout << "  [!] Пользователь не найден.\n";
        return;
    }
    std::cout << "  Текущая роль: "
              << (u->role == Role::ADMIN ? "ADMIN" : "USER") << '\n';
    std::cout << "  Новая роль: 1) USER  2) ADMIN  0) Отмена\n> ";
    int ch = read_int("");
    if (ch == 1) u->role = Role::USER;
    else if (ch == 2) u->role = Role::ADMIN;
    else { std::cout << "  Отменено.\n"; return; }
    
    update_user_role(db, u->id, u->role);
    std::cout << "  - Роль изменена.\n";
}
