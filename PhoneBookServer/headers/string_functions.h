// Заголовочный файл string_functions.h содержит в себе функции для работы со строками

// Header guard (предотвращает повторное включение заголовочного файла)
#pragma once

// Подключим библиотеку string для работы со строками, библиотеку tuple для работы
// с кортежами и библиотеку vector для использования контейнеров вектора
#include <string>
#include <tuple>
#include <vector>

// Не будем использовать using-директивы в глобальной области видимости заголовочного файла, так как это
// приведёт к попаданию этих using-директив во все области видимости, куда будет включён заголовочный файл

// Пространство имён для функций для работы со строками
namespace string_functions {

// Пространство имён для вспомогательных объектов и функций
namespace detail {

// Функция нахождения всех позиций вхождения символа в строку
std::vector<size_t> FindAllPositionsOfCharInString(std::string_view str, char c);

// Функция преобразования "&quot;" в кавычки в строке
std::string ConverteAmpersandSequencesToQuotes(std::string_view str);

// Функция преобразования кавычек в "&quot;" в строке
std::string ConverteQuotesToAmpersandSequences(std::string_view str);

}

// Функция разделения строки на слова через символы-сепараторы
// (знаки препинания ".", "?", "!", ".", ":", ",", ";", кавычки, скобки "()", "[]", "{}" и пробел " ")
// (возвращает вектор string_view, ссылающихся на оригинальную строку)
std::vector<std::string_view> SplitIntoWords(std::string_view str);

// Функция сортировки и удаления дубликатов в векторе слов
// (возвращает отсортированный вектор слов с удалёнными дубликатами, string_view в возвращённом
// векторе ссылаются на те же string'и, что и в оригинальном векторе, переданным в качестве
// аргумента функции)
std::vector<std::string_view> SortAndRemoveDuplicates(std::vector<std::string_view> words);

// Функция парсинга строки с информацией о числе записей в базе данных и номере/id последней записи
// (используется при загрузке данных из файла в базу данных)
std::pair<size_t, size_t> ParseInfoStringFromFile(std::string_view str);

// Функция парсинга строки с записью для базы данных
// (используется при загрузке данных из файла в базу данных)
std::tuple<size_t, std::string, std::string, 
                   std::string, std::string, std::string> ParseRecordStringFromFile(std::string_view str);

// Функция упаковки строки с информацией о числе записей в базе данных и номере/id последней записи
// (используется при сохранении данных из базы в файл)
std::string PackInfoStringForFile(size_t records_count, size_t last_record_id);

// Функция упаковки строки с записью для базы данных
// (используется при сохранении данных из базы в файл)
std::string PackRecordStringForFile(size_t id, std::string_view name,       std::string_view surname,
                                               std::string_view patronymic, std::string_view number, std::string_view note);

}