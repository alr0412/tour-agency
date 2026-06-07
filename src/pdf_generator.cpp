#include "tour.h"
#include <hpdf.h>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>

std::string utf8_to_cp1251(const std::string& utf8_str) {
    // uTF-8 в широкую строку
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(utf8_str);
    
    // Конвертация широкиз строк в CP1251
    std::string result;
    for (wchar_t wc : wide) {
        if (wc >= 0x0410 && wc <= 0x044F) { // Диапазон кириллицы
            // Кириллица в CP1251
            static const int cp1251_cyrillic[0x50] = {
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
                0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
                0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF 
            };
            int idx = wc - 0x0410;
            if (idx >= 0 && idx < 0x50) {
                result.push_back(static_cast<char>(cp1251_cyrillic[idx]));
            } else {
                result.push_back('?');
            }
        } else if (wc < 0x80) {
            result.push_back(static_cast<char>(wc)); // ASCII
        } else {
            result.push_back('?');
        }
    }
    return result;
}

void pdf_error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    (void)user_data;
    std::cerr << "PDF ошибка: " << error_no << " / " << detail_no << std::endl;
}

bool generate_tour_pdf(const Tour& tour, const std::string& filename) {
    HPDF_Doc pdf = HPDF_New(pdf_error_handler, nullptr);
    if (!pdf) {
        std::cerr << "Ошибка при создании PDF" << std::endl;
        return false;
    }

    // Загрузка шрифта
    const char* font_path = "arial.ttf";
    const char* font_name = HPDF_LoadTTFontFromFile(pdf, font_path, HPDF_TRUE);
    HPDF_Font cyrillic_font = nullptr;
    if (font_name) {
        // CP1251 кодировка
        cyrillic_font = HPDF_GetFont(pdf, font_name, "CP1251");
        if (!cyrillic_font) {
            cyrillic_font = HPDF_GetFont(pdf, font_name, nullptr);
        }
    }
    if (!cyrillic_font) {
        std::cerr << "  [!] Не удалось загрузить шрифт с кириллицей." << std::endl;
        cyrillic_font = HPDF_GetFont(pdf, "Helvetica", nullptr);
    }

    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    // Title
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, cyrillic_font, 18);
    std::string title = utf8_to_cp1251("Тур #" + std::to_string(tour.id));
    HPDF_Page_TextOut(page, 50, 800, title.c_str());
    HPDF_Page_EndText(page);

    // Info
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, cyrillic_font, 12);
    int y = 750;
    std::string name = utf8_to_cp1251("Название: " + tour.name);
    HPDF_Page_TextOut(page, 50, y, name.c_str()); y -= 30;
    std::string country = utf8_to_cp1251("Страна: " + tour.country);
    HPDF_Page_TextOut(page, 50, y, country.c_str()); y -= 30;
    std::string price = utf8_to_cp1251("Цена: " + std::to_string(tour.price) + " руб.");
    HPDF_Page_TextOut(page, 50, y, price.c_str()); y -= 30;
    std::string date = utf8_to_cp1251("Дата вылета: " + tour.date.to_str());
    HPDF_Page_TextOut(page, 50, y, date.c_str()); y -= 30;
    std::string length = utf8_to_cp1251("Длина: " + std::to_string(tour.length) + " дней");
    HPDF_Page_TextOut(page, 50, y, length.c_str()); y -= 30;
    HPDF_Page_EndText(page);

    // Photo (unchanged)
    if (!tour.photo.empty()) {
        std::string temp_file = "temp_tour_photo.jpg";
        std::ofstream fout(temp_file, std::ios::binary);
        fout.write(reinterpret_cast<const char*>(tour.photo.data()), tour.photo.size());
        fout.close();

        HPDF_Image image = HPDF_LoadJpegImageFromFile(pdf, temp_file.c_str());
        if (!image) {
            image = HPDF_LoadPngImageFromFile(pdf, temp_file.c_str());
        }
        if (image) {
            HPDF_Page_DrawImage(page, image, 400, 600, 150, 100);
        }
        std::remove(temp_file.c_str());
    }

    HPDF_SaveToFile(pdf, filename.c_str());
    HPDF_Free(pdf);
    return true;
}