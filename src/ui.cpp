#include "ui.h"
#include <iostream>
#include <sstream>

static Currency current_currency = Currency::RUB;

// Работа с валютами
std::string currency_symbol(Currency cur)
{
    switch (cur)
    {
        case Currency::RUB: return "₽";
        case Currency::BYN: return "BYN";
        case Currency::USD: return "$";
        case Currency::EUR: return "€";
        default: return "₽";
    }
}

void change_currency()
{
    std::cout << "\n  Выберите валюту:\n"
              << "  1) Российский рубль (₽)\n"
              << "  2) Белорусский рубль (BYN)\n"
              << "  3) Доллар США ($)\n"
              << "  4) Евро (€)\n"
              << "  0) Отмена\n> ";
    int ch = read_int("");
    switch (ch)
    {
        case 1: current_currency = Currency::RUB; break;
        case 2: current_currency = Currency::BYN; break;
        case 3: current_currency = Currency::USD; break;
        case 4: current_currency = Currency::EUR; break;
        default: std::cout << "  Валюта не изменена.\n"; return;
    }
    std::cout << "  Валюта изменена.\n";
}

Currency get_current_currency()
{
    return current_currency;
}

int convert_price(int price_rub)
{
    // Курсы
    switch (get_current_currency())
    {
        case Currency::RUB: return price_rub;
        case Currency::BYN: return price_rub / 26;     
        case Currency::USD: return price_rub / 71;   
        case Currency::EUR: return price_rub / 83;    
        default: return price_rub;
    }
}

int convert_to_rub(int amount, Currency cur)
{
    switch (cur)
    {
        case Currency::RUB: return amount;
        case Currency::BYN: return amount * 26;     
        case Currency::USD: return amount * 71;    
        case Currency::EUR: return amount * 83;    
        default: return amount;
    }
}

// Ввод

bool is_integer(const std::string &s)
{
    if (s.empty())
        return false;
    size_t start = (s[0] == '-') ? 1 : 0;
    if (start == s.size())
        return false;
    for (size_t i = start; i < s.size(); ++i)
        if (!isdigit(static_cast<unsigned char>(s[i])))
            return false;
    return true;
}

int read_int(const std::string &prompt)
{
    std::string input;
    while (true)
    {
        if (!prompt.empty())
            std::cout << prompt;
        std::getline(std::cin, input);
        if (is_integer(input))
            return std::stoi(input);
        std::cout << "  [!] Введите целое число.\n";
    }
}

std::string read_string(const std::string &prompt)
{
    std::string input;
    while (true)
    {
        if (!prompt.empty())
            std::cout << prompt;
        std::getline(std::cin, input);
        if (!input.empty())
            return input;
        std::cout << "  [!] Строка не может быть пустой.\n";
    }
}

Date read_date(const std::string &prompt)
{
    while (true)
    {
        std::string s = read_string(prompt + " (дд-мм-гггг): ");
        Date d = Date::from_str(s);
        if (d.valid())
            return d;
        std::cout << "  [!] Некорректная дата. Формат: дд-мм-гггг (например 15-06-2026)\n";
    }
}

// ───────────────── Таблица ─────────────────

// Считает количество Unicode-кодпоинтов в UTF-8 строке
static int utf8_len(const std::string &s)
{
    int count = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80)
            ++count; // не continuation byte
    return count;
}

// Обрезает UTF-8 строку до max кодпоинтов
static std::string utf8_substr(const std::string &s, int max_cp)
{
    int count = 0;
    size_t i = 0;
    while (i < s.size() && count < max_cp)
    {
        unsigned char c = s[i];
        if ((c & 0x80) == 0)
        {
            ++i;
        } // 1-byte
        else if ((c & 0xE0) == 0xC0)
        {
            i += 2;
        } // 2-byte
        else if ((c & 0xF0) == 0xE0)
        {
            i += 3;
        } // 3-byte
        else
        {
            i += 4;
        } // 4-byte
        ++count;
    }
    return s.substr(0, i);
}

// Форматирует строку по ширине в символах
static std::string fmt(const std::string &s, int w)
{
    int len = utf8_len(s);
    if (len >= w)
        return utf8_substr(s, w - 1) + ".";
    return s + std::string(w - len, ' ');
}

void print_tours_table(const std::vector<Tour> &tours)
{
    if (tours.empty())
    {
        std::cout << "  (список пуст)\n";
        return;
    }

    // ширины колонок в символах
    const int W_ID = 4, W_NAME = 38, W_COUNTRY = 12,
              W_PRICE = 11, W_DATE = 11, W_LEN = 5;
    const int TOTAL = W_ID + W_NAME + W_COUNTRY + W_PRICE + W_DATE + W_LEN + 13;

    auto sep = [&](char c)
    { std::cout << std::string(TOTAL, c) << '\n'; };

    auto row = [&](const std::string &id, const std::string &name,
                   const std::string &country, const std::string &price,
                   const std::string &date, const std::string &len)
    {
        std::cout << "| " << fmt(id, W_ID)
                  << "| " << fmt(name, W_NAME)
                  << "| " << fmt(country, W_COUNTRY)
                  << "| " << fmt(price, W_PRICE)
                  << "| " << fmt(date, W_DATE)
                  << "| " << fmt(len, W_LEN) << "|\n";
    };

    sep('=');
    std::string price_header = "ЦЕНА (" + currency_symbol(get_current_currency()) + ")";
    row("ID", "НАЗВАНИЕ ТУРА", "СТРАНА", price_header, "ДАТА", "ДНЕЙ");
    sep('=');
    for (size_t i = 0; i < tours.size(); ++i)
    {
        const auto &t = tours[i];
        row(std::to_string(t.id), t.name, t.country,
            std::to_string(convert_price(t.price)) + " " + currency_symbol(get_current_currency()),
            t.date.to_str(), std::to_string(t.length));
        if (i + 1 < tours.size())
            sep('-');
    }
    sep('=');
}
