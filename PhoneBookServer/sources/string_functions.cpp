// Единица трансляции string_functions.cpp содержит в себе функции для работы со строками

// Подключим библиотеку algorithm для использования стандартных алгоритмов
#include <algorithm>

// Подключим заголовочный файл с функциями для работы со строками
#include "string_functions.h"

// Подключим пространство имён std
using namespace std;

// Пространство имён для функций для работы со строками
namespace string_functions {

// Пространство имён для вспомогательных объектов и функций
namespace detail {

// Функция нахождения всех позиций вхождения символа в строку
vector<size_t> FindAllPositionsOfCharInString(string_view str, char c) {

    // Вектор позиций вхождения символа в строку
    vector<size_t> positions;
    
    // Пробегаем в цикле всю строку по символам
    for(size_t pos = 0; pos < str.size(); ++pos) {

        // Если символ совпадает с нужным, добавляем его позицию в вектор позиций
        if(str[pos] == c) positions.push_back(pos);
    }

    // Возвращаем вектор позиций вхождения символа в строку
    return positions;
}

// Функция преобразования "&quot;" в кавычки в строке
string ConverteAmpersandSequencesToQuotes(string_view str) {

    // Если "&quot;" вообще не встречаются в строке, возвращаем её копию
    if (str.find("&quot;"s) == str.npos) return string(str);

    // Иначе пробегаем по строке и формируем такую же, но с "&quot;", заменёнными на кавычки
    string result = ""s;

    while (true) {
        size_t quot_pos = str.find("&quot;"s);
        result += str.substr(0, quot_pos);

        if (quot_pos == str.npos) break;
        else {
            result += "\""s;
            str.remove_prefix(quot_pos + 6);
        }
    }

    // Возвращаем результат
    return result;
}

// Функция преобразования кавычек в "&quot;" в строке
string ConverteQuotesToAmpersandSequences(string_view str) {

    // Если кавычки вообще не встречаются в строке, возвращаем её копию
    if (str.find('\"') == str.npos) return string(str);

    // Иначе пробегаем по строке и формируем такую же, но с кавычками, заменёнными на "&quot;"
    string result = ""s;

    while (true) {
        size_t quot_pos = str.find("\""s);
        result += str.substr(0, quot_pos);

        if (quot_pos == str.npos) break;
        else {
            result += "&quot;"s;
            str.remove_prefix(quot_pos + 1);
        }
    }

    // Возвращаем результат
    return result;
}

}

// Функция разделения строки на слова через символы-сепараторы
// (знаки препинания ".", "?", "!", ".", ":", ",", ";", кавычки, скобки "()", "[]", "{}" и пробел " ")
// (возвращает вектор string_view, ссылающихся на оригинальную строку)
vector<string_view> SplitIntoWords(string_view str) {

    // Вектор с разделёнными словами
    vector<string_view> words;

    // Символы-сепараторы:
    const static string separator_chars = "!?.:,;\"()[]{} "s;

    // В бесконечном цикле ищем следующий символ-сепаратор в строке и откусываем от неё часть от начала
    // до символа-сепаратора, таким образом разделяя строку на слова через символы-сепараторы, и так до
    // тех пор, пока не дойдём до конца строки
    while (true) {
        size_t separator_pos = str.find_first_of(separator_chars);
        string_view word = str.substr(0, separator_pos);

        if (!word.empty()) words.push_back(str.substr(0, separator_pos));
        if (separator_pos == str.npos) break;
        else str.remove_prefix(separator_pos + 1);
    }

    // Возвращаем вектор с разделёнными словами
    return words;
}

// Функция сортировки и удаления дубликатов в векторе слов
// (возвращает отсортированный вектор слов с удалёнными дубликатами, string_view в возвращённом
// векторе ссылаются на те же string'и, что и в оригинальном векторе, переданным в качестве
// аргумента функции)
vector<string_view> SortAndRemoveDuplicates(vector<string_view> words) {

    // Сортируем вектор слов
    sort(words.begin(), words.end());

    // Удаляем повтояющиеся подряд идущие слова
    const auto words_end_border = unique(words.begin(), words.end());

    // Отсекаем хвост в векторе слов
    words.erase(words_end_border, words.end());

    // Возвращаем отсортированный вектор слов с удалёнными дубликатами
    return words;
}

// Функция парсинга строки с информацией о числе записей в базе данных и номере/id последней записи
// (используется при загрузке данных из файла в базу данных)
pair<size_t, size_t> ParseInfoStringFromFile(string_view str) {

    // Пример строки: <records_count="42" last_record_id="53">

    // Для удобства подключим внутри функции пространство имён detail
    using namespace detail;

    // Находим все позици символа кавычек в строке
    auto positions = FindAllPositionsOfCharInString(str, '\"');

    // Получаем число записей в базе данных
    size_t records_count  = stoi(string(str.substr(positions[0] + 1, positions[1] - positions[0] - 1)));

    // Получаем номер/id последней записи
    size_t last_record_id = stoi(string(str.substr(positions[2] + 1, positions[3] - positions[2] - 1)));

    // Возвращаем результат
    return {records_count, last_record_id};
}

// Функция парсинга строки с записью для базы данных
// (используется при загрузке данных из файла в базу данных)
tuple<size_t, string, string, string, string, string> ParseRecordStringFromFile(string_view str) {

    // Пример строки: <id="2" name="Александр" surname="Петров" patronymic="Иванович" number="+79754213275" note="C++ junior developer at &quotLesta Games&quot">

    // Для удобства подключим внутри функции пространство имён detail
    using namespace detail;

    // Находим все позици символа кавычек в строке
    auto positions = FindAllPositionsOfCharInString(str, '\"');

    // Получаем номер/id записи
    size_t id  = stoi(string(str.substr(positions[0] + 1,
                                        positions[1] - positions[0] - 1)));

    // Получаем имя (заменяя "&quot;" на кавычки)
    string name = ConverteAmpersandSequencesToQuotes(str.substr(positions[2] + 1,
                                                                positions[3] - positions[2] - 1));

    // Получаем фамилию (заменяя "&quot;" на кавычки)
    string surname = ConverteAmpersandSequencesToQuotes(str.substr(positions[4] + 1,
                                                                   positions[5] - positions[4] - 1));

    // Получаем отчество (заменяя "&quot;" на кавычки)
    string patronymic = ConverteAmpersandSequencesToQuotes(str.substr(positions[6] + 1,
                                                                      positions[7] - positions[6] - 1));

    // Получаем номер телефона (заменяя "&quot;" на кавычки)
    string number = ConverteAmpersandSequencesToQuotes(str.substr(positions[8] + 1,
                                                                  positions[9] - positions[8] - 1));

    // Получаем заметку (заменяя "&quot;" на кавычки)
    string note = ConverteAmpersandSequencesToQuotes(str.substr(positions[10] + 1,
                                                                positions[11] - positions[10] - 1));

    // Возвращаем результат
    return {id, name, surname, patronymic, number, note};
}

// Функция упаковки строки с информацией о числе записей в базе данных и номере/id последней записи
// (используется при сохранении данных из базы в файл)
string PackInfoStringForFile(size_t records_count, size_t last_record_id) {

    // Пример строки-результата: <records_count="42" last_record_id="53">

    // Упаковываем информацию в строку
    string result = "<records_count=\""s + to_string(records_count) + "\" last_record_id=\""s + to_string(last_record_id) + "\">"s;

    // Возвращаем результат
    return result;
}

// Функция упаковки строки с записью для базы данных
// (используется при сохранении данных из базы в файл)
string PackRecordStringForFile(size_t id, string_view name, string_view surname, string_view patronymic, string_view number, string_view note) {

    // Пример строки-результата: <id="2" name="Александр" surname="Петров" patronymic="Иванович" number="+79754213275" note="C++ junior developer at &quotLesta Games&quot">

    // Для удобства подключим внутри функции пространство имён detail
    using namespace detail;

    // Упаковываем информацию в строку, заменяя в имени/фамилии/отчестве/номере телефона/заметке кавычки на "&quot;"
    string result = "<id=\""s           + to_string(id) +
                    "\" name=\""s       + ConverteQuotesToAmpersandSequences(name)       +
                    "\" surname=\""s    + ConverteQuotesToAmpersandSequences(surname)    +
                    "\" patronymic=\""s + ConverteQuotesToAmpersandSequences(patronymic) +
                    "\" number=\""s     + ConverteQuotesToAmpersandSequences(number)     +
                    "\" note=\""s       + ConverteQuotesToAmpersandSequences(note)       +
                    "\">"s;

    // Возвращаем результат
    return result;
}

}