#pragma once
#include "tour.h"
#include <vector>
#include <string>
#include <functional>

// Критерии сортировки
enum class SortBy { NONE, PRICE_ASC, PRICE_DESC, LENGTH_ASC, LENGTH_DESC };

struct SearchFilters {
    std::string country;   // пустая строка = любая страна
    int min_price  = 0;
    int max_price  = 999999;
    Date from_date = {1, 1, 2000};
    Date to_date   = {31, 12, 2099};
    int min_length = 1;
    int max_length = 365;
    int adults = 1;     
    int children = 0;    
    SortBy sort_by = SortBy::NONE;
};

// Подсчет цены с учетом кол-ва людей
inline int calculate_total_price(int base_price, int adults, int children)
{
    return base_price * (adults + children * 0.34);
}

// Применяет фильтры и сортировку
std::vector<Tour> apply_filters(const std::vector<Tour>& tours,
                                 const SearchFilters& f);

// Интерактивный диалог настройки фильтров (изменяет f)
// Возвращает false если пользователь хочет выйти в главное меню
bool filters_menu(SearchFilters& f, const std::vector<Tour>& tours);
