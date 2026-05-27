#include "filter.h"
#include "ui.h"
#include <iostream>
#include <algorithm>
#include <vector>

// Применение фильтров

std::vector<Tour> apply_filters(const std::vector<Tour> &tours,
                                const SearchFilters &f)
{
    std::vector<Tour> result;
    for (const auto &t : tours)
    {
        if (!f.country.empty() && t.country != f.country)
            continue;
        if (t.price < f.min_price || t.price > f.max_price)
            continue;
        if (t.length < f.min_length || t.length > f.max_length)
            continue;
        if (!(t.date >= f.from_date && t.date <= f.to_date))
            continue;
        result.push_back(t);
    }

    switch (f.sort_by)
    {
    case SortBy::PRICE_ASC:
        std::sort(result.begin(), result.end(),
                  [](const Tour &a, const Tour &b)
                  { return a.price < b.price; });
        break;
    case SortBy::PRICE_DESC:
        std::sort(result.begin(), result.end(),
                  [](const Tour &a, const Tour &b)
                  { return a.price > b.price; });
        break;
    case SortBy::LENGTH_ASC:
        std::sort(result.begin(), result.end(),
                  [](const Tour &a, const Tour &b)
                  { return a.length < b.length; });
        break;
    case SortBy::LENGTH_DESC:
        std::sort(result.begin(), result.end(),
                  [](const Tour &a, const Tour &b)
                  { return a.length > b.length; });
        break;
    default:
        break;
    }
    return result;
}

// helpers

static void set_country(SearchFilters &f, const std::vector<Tour> &tours)
{
    auto countries = countries_from_tours(tours);
    int i = 1;
    std::cout << "  0) Любая страна\n";
    for (const auto &c : countries)
        std::cout << "  " << i++ << ") " << c << '\n';
    int ch = read_int("> ");
    if (ch == 0)
    {
        f.country = "";
        return;
    }
    i = 1;
    for (const auto &c : countries)
    {
        if (i++ == ch)
        {
            f.country = c;
            return;
        }
    }
    std::cout << "  [!] Неверный выбор. Страна не изменена.\n";
}

static void set_price(SearchFilters &f)
{
    Currency cur = get_current_currency();
    std::string cur_symbol = currency_symbol(cur);
    
    int min_local = std::max(0, read_int("  Минимальная цена (" + cur_symbol + "): "));
    int max_local = read_int("  Максимальная цена (" + cur_symbol + "): ");
    
    if (max_local < min_local)
    {
        std::cout << "  [!] Максимум меньше минимума — установлен равным минимуму.\n";
        max_local = min_local;
    }
    
    // Конвертация в рубли для хранения в фильтре
    f.min_price = convert_to_rub(min_local, cur);
    f.max_price = convert_to_rub(max_local, cur);
}

static void set_dates(SearchFilters &f)
{
    f.from_date = read_date("  Дата вылета НЕ РАНЬШЕ");
    f.to_date = read_date("  Дата вылета НЕ ПОЗЖЕ");
    if (!(f.from_date <= f.to_date))
    {
        std::cout << "  [!] Конечная дата раньше начальной — диапазон сброшен.\n";
        f.from_date = {1, 1, 2000};
        f.to_date = {31, 12, 2099};
    }
}

static void set_length(SearchFilters &f)
{
    f.min_length = std::max(1, read_int("  Минимальная продолжительность (дней): "));
    f.max_length = read_int("  Максимальная продолжительность (дней): ");
    if (f.max_length < f.min_length)
    {
        std::cout << "  [!] Максимум < минимума — установлен равным минимуму.\n";
        f.max_length = f.min_length;
    }
}

static void set_sort(SearchFilters &f)
{
    std::cout << "  1) Цена ↑\n  2) Цена ↓\n  3) Длительность ↑\n  4) Длительность ↓\n  0) Без сортировки\n";
    switch (read_int("> "))
    {
    case 1:
        f.sort_by = SortBy::PRICE_ASC;
        break;
    case 2:
        f.sort_by = SortBy::PRICE_DESC;
        break;
    case 3:
        f.sort_by = SortBy::LENGTH_ASC;
        break;
    case 4:
        f.sort_by = SortBy::LENGTH_DESC;
        break;
    default:
        f.sort_by = SortBy::NONE;
    }
}

static std::string sort_label(SortBy s)
{
    switch (s)
    {
    case SortBy::PRICE_ASC:
        return "Цена ↑";
    case SortBy::PRICE_DESC:
        return "Цена ↓";
    case SortBy::LENGTH_ASC:
        return "Длит. ↑";
    case SortBy::LENGTH_DESC:
        return "Длит. ↓";
    default:
        return "нет";
    }
}

// меню фильтров

bool filters_menu(SearchFilters &f, const std::vector<Tour> &tours)
{
    while (true)
    {
        std::string country_label = f.country.empty() ? "любая" : f.country;
        std::cout << "\n── Фильтры поиска ──────────────────────────────\n"
                  << "  1) Страна:          " << country_label << '\n'
                  << "  2) Бюджет (" << currency_symbol(get_current_currency()) << "):      от " << convert_price(f.min_price) << " до " << convert_price(f.max_price)<< '\n'
                  << "  3) Дата вылета:     " << f.from_date.to_str()
                  << " — " << f.to_date.to_str() << '\n'
                  << "  4) Продолжительность(дн.):  от " << f.min_length << " до " << f.max_length << '\n'
                  << "  5) Сортировка:      " << sort_label(f.sort_by) << '\n'
                  << "────────────────────────────────────────────────\n"
                  << "  6) Найти туры\n"
                  << "  7) Сбросить фильтры\n"
                  << "  0) Назад в главное меню\n"
                  << "────────────────────────────────────────────────\n> ";

        switch (read_int(""))
        {
        case 1:
            set_country(f, tours);
            break;
        case 2:
            set_price(f);
            break;
        case 3:
            set_dates(f);
            break;
        case 4:
            set_length(f);
            break;
        case 5:
            set_sort(f);
            break;
        case 6:
        {
            auto res = apply_filters(tours, f);
            if (res.empty())
                std::cout << "\n  Туров по заданным критериям не найдено.\n";
            else
            {
                std::cout << "\n  Найдено туров: " << res.size() << '\n';
                print_tours_table(res);
            }
            break;
        }
        case 7:
            f = SearchFilters{};
            std::cout << "  Фильтры сброшены.\n";
            break;
        case 0:
            return false;
        default:
            std::cout << "  [!] Неверный пункт меню.\n";
        }
    }
}
