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

}

// Функция разделения строки на слова через пробел
// (возвращает вектор string_view, ссылающихся на оригинальную строку)
vector<string_view> SplitIntoWords(string_view str) {

    // Вектор с разделёнными словами
    vector<string_view> words;

    // В бесконечном цикле ищем следующий пробел в строке и откусываем от неё часть от начала
    // до пробела, таким образом разделяя строку на слова через пробел, и так до тех пор, пока
    // не дойдём до конца строки
    while (true) {
        const auto space = str.find(' ');
        words.push_back(str.substr(0, space));

        if (space == str.npos) break;
        else str.remove_prefix(space + 1);
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

    // Пример строки: <records_count="13" last_record_id="18">

    // Находим все позици символа кавычек в строке
    auto positions = detail::FindAllPositionsOfCharInString(str, '\"');

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

    // Пример строки: <id="2" name="Александр" surname="Петров" patronymic="Иванович" number="+79754213275" note="C++ junior developer">

    // Находим все позици символа кавычек в строке
    auto positions = detail::FindAllPositionsOfCharInString(str, '\"');

    // Получаем номер/id записи
    size_t id  = stoi(string(str.substr(positions[0] + 1, positions[1] - positions[0] - 1)));

    // Получаем имя
    string name(str.substr(positions[2] + 1, positions[3] - positions[2] - 1));

    // Получаем фамилию
    string surname(str.substr(positions[4] + 1, positions[5] - positions[4] - 1));

    // Получаем отчество
    string patronymic(str.substr(positions[6] + 1, positions[7] - positions[6] - 1));

    // Получаем номер телефона
    string number(str.substr(positions[8] + 1, positions[9] - positions[8] - 1));

    // Получаем заметку
    string note(str.substr(positions[10] + 1, positions[11] - positions[10] - 1));

    // Возвращаем результат
    return {id, name, surname, patronymic, number, note};
}

}