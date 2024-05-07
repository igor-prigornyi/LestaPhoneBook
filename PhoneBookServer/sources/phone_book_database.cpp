// Единица трансляции phone_book_database.cpp описывает работу базы данных для телефонной книги,
// которая используется в сервере для телефонной книги, предоставляя необходимый функционал
// по хранению, изменению и поиску данных

// Подключим библиотеку iostream для работы стандартного потока вывода в консоль для отображения статуса
// работы базы данных, библиотеку fstream для работы с потоком ввода-вывода в файл, библиотеку cmath для
// использования математических функций (требуется функция логарифма) и библиотеку algorithm для
// использования стандартных алгоритмов
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

// Подключим заголовочный файл базы данных для телефонной книги
#include "phone_book_database.h"

// Подключаем заголовочный файл с функциями для работы со строками
#include "string_functions.h"

// Подключим пространство имён std
using namespace std;

// Пространство имён базы данных для телефонной книги
namespace phone_book_database {

// Конструктор базы данных принимает имя файла (полное имя с путём до файла) с базой данных телефонной книги,
// запоминает это имя и загружает данные в базу из файла
PhoneBookDatabase::PhoneBookDatabase(const string& database_file_name) : database_file_name_(database_file_name) {
    LoadFromFile();
}

// Функция загрузки данных в базу из файла
void PhoneBookDatabase::LoadFromFile() {

    // Информируем в консоль о начале загрузке данных в базу из файла
    cout << "[Starting loading data from \""s << database_file_name_ << "\" into the database ...]"s << endl;

    // Открываем файл для чтения
    ifstream database_file(database_file_name_);

    // Если файла с базой данных нету
    if(!database_file) {

        // Информируем в консоль о невозможности открыть файл с базой данных,
        // будем работать с пустой базой данных (нужно для первого запуска сервера)
        cout << "[Can't open \""s << database_file_name_ << "\", database is empty]"s << endl;

        // Номером/id последней записи будет id = 0 (т.е. записей ещё нету)
        last_record_id_ = 0;

        // На этом всё
        return;
    }

    // Замечание: для того, чтобы избежать проблемы с кавычками в имени/фамилии/отчестве/номере телефона/заметке,
    // при сохранении данных из базы в файл все кавычки заменяются на "&quot;", а при чтении данных из файла в
    // базу данных последовательности "&quot;" заменяются обратно на кавычки

    // Буферная строка
    string buffer;

    // Читаем в буфер первую строку с информацией о числе записей в базе данных и номере/id последней записи
    getline(database_file, buffer);

    // Парсим строку с информацией о числе записей в базе данных и номере/id последней записи
    auto database_info = string_functions::ParseInfoStringFromFile(buffer);

    // Записываем число записей в базе данных 
    size_t number_of_records = database_info.first;

    // Записываем номер/id последней записи
    last_record_id_ = database_info.second;

    // В цикле читаем все записи из файла
    for(size_t i = 0; i < number_of_records; ++i) {

        // Читаем в буфер очередную строку с очередной записью для базы данных
        getline(database_file, buffer);

        // Парсим строку с очередной записью для базы данных
        auto record_info = string_functions::ParseRecordStringFromFile(buffer);

        // Номер/id записи, которую необходимо добавить
        size_t record_id = get<0>(record_info);

        // Запись, которую необходимо добавить
        Record record({get<1>(record_info), get<2>(record_info), get<3>(record_info), get<4>(record_info), get<5>(record_info)});

        // Добавляем запись в базу данных
        AddRecordById(record_id, record);
    }

    // Закрываем файл
    database_file.close();
    
    // Информируем в консоль об успешной загрузке данных в базу из файла
    cout << "[Data from \""s << database_file_name_ << "\" has been loaded into the database]"s << endl;
}

// Функция сохранения данных из базы в файл
void PhoneBookDatabase::SaveToFile() const {

    // Информируем в консоль о начале сохранения данных из базы в файл
    cout << "[Starting saving data from database to \""s << database_file_name_ << "\" ...]"s << endl;

    // Открываем файл для записи
    ofstream database_file(database_file_name_);

    // Замечание: для того, чтобы избежать проблемы с кавычками в имени/фамилии/отчестве/номере телефона/заметке,
    // при сохранении данных из базы в файл все кавычки заменяются на "&quot;", а при чтении данных из файла в
    // базу данных последовательности "&quot;" заменяются обратно на кавычки

    // Записываем информацию о числе записей в базе данных и номере/id последней записи
    database_file << string_functions::PackInfoStringForFile(records_.size(), last_record_id_) << endl;

    // Записываем каждую запись и базы данных в файл
    for(const auto& [record_id, record] : records_) {
        database_file << string_functions::PackRecordStringForFile(record_id,
                                                                   record.name,
                                                                   record.surname,
                                                                   record.patronymic,
                                                                   record.number,
                                                                   record.note) << endl;
    }

    // Закрываем файл
    database_file.close();

    // Информируем в консоль об успешном сохранении данных из базы в файл
    cout << "[Data from database has been saved to \""s << database_file_name_ << "\"]"s << endl;
}

// Функция добавления записи с фиксированным номером/id
// (возвращает код ответа: 0 - запись с таким номером телефона уже существует,
//                         1 - запись успешно добавлена)
size_t PhoneBookDatabase::AddRecordById(size_t record_id, const Record& record) {

    // Если запись с таким номером телефона уже существует в базе данных, возвращаем код ответа - 0
    if(number_to_record_.count(record.number)) {
        return 0;
    }

    // Добавляем данные в словарь "Номер/id записи -> записи"
    // (именно в этом контейнере хранятся строковые данные, в остальных лишь ссылки (string_view))
    records_[record_id] = record;

    // Для остальных словарей в качестве ключа важно использовать именно ту строку, на которую будет
    // ссылаться string_view, т.е. строку из контейнера records_

    // Добавляем данные в словарь "Имя -> Номер/id записи" (для поиска записей по имени)
    name_to_records_[records_[record_id].name].insert(record_id);

    // Добавляем данные в словарь "Фамилия -> Номер/id записи" (для поиска записей по фамилии)
    surname_to_records_[records_[record_id].surname].insert(record_id);

    // Добавляем данные в словарь "Отчество -> Номер/id записи" (для поиска записей по отчеству)
    patronymic_to_records_[records_[record_id].patronymic].insert(record_id);

    // Добавляем данные в словарь "Номер телефона -> Номер/id записи" (для поиска записей по номеру телефона)
    number_to_record_[records_[record_id].number] = record_id;

    // Для поиска записей по содержимому заметки и получения выборки, ранжированной по TF-IDF,
    // необходимо добавить данные о словах, содержащихся в заметке записи, для чего вычислим
    // TF (Term Frequency) каждого слова в заметке записи по формуле:
    //
    // TF = Число вхождений (упоминаний) слова в заметке / Число слов в заметке
    // (https://ru.wikipedia.org/wiki/TF-IDF)

    // Вначале разделим заметку в записи на отдельные слова через символы-сепараторы
    // (знаки препинания ".", "?", "!", ".", ":", ",", ";", кавычки, скобки "()", "[]", "{}" и пробел " ")
    vector<string_view> note_words = string_functions::SplitIntoWords(records_[record_id].note);

    // Вычислим константу inv_word_count = 1 / Число слов в заметке
    const double inv_word_count = 1.0 / static_cast<double>(note_words.size());

    // Пробежимся по всем словам в заметке записи
    for (string_view word : note_words) {

        // Внесём данные с TF слова в словарь "Слово в заметках -> Номер/id записи -> Частота TF" следующим образом:
        // будем добавлять каждый раз к значению TF слова (вначале инициализировано нулём) ранее вычисленную константу
        // inv_word_count = 1 / Число слов в заметке
        note_word_to_record_freqs_[word][record_id] += inv_word_count;

        // Также внесём слово в словарь "Номер/id записи -> Слова в заметках"
        record_to_note_words_[record_id].insert(word);
    }

    // Значение IDF (Inverse Document Frequency) будет вычисляться в момент
    // поиска записей по содержанию заметок

    // Возвращаем код ответа - 1
    return 1;
}

// Функция добавления записи
// (возвращает код ответа: 0 - запись с таким номером телефона уже существует,
//                         1 - запись успешно добавлена)
size_t PhoneBookDatabase::AddRecord(const Record& record) {

    // Пробуем добавить запись в базу данных с номером/id на 1 большим, чем последний номер/id записи в базе

    // Если запись успешно добавлена
    if(AddRecordById(last_record_id_ + 1, record)) {

        // Икрементируем номер/id последней записи
        ++last_record_id_;

        // Возвращаем код ответа - 1
        return 1;
    }
    // Если запись не была добавлена (в базе данных уже есть запись с таким же телефонным номером)
    else {

        // Возвращаем код ответа - 0
        return 0;
    }
}

// Функция удаления записи по её номеру/id
// (возвращает код ответа: 0 - записи с таким номером/id не существует,
//                         1 - запись успешно удалена)
size_t PhoneBookDatabase::DeleteRecordById(size_t record_id) {

    // Если записи с таким номером/id не существует в базе данных, возвращаем код ответа - 0
    if(!records_.count(record_id)) {
        return 0;
    }

    // Удаляем запись из базы данных. Вначале стоит удалить упоминание записи из тех словарей, где ключом
    // является string_view, который ссылается на строки из контейнера records_, а затем уже сами данные
    // из контейнера records_, иначе зависимые string_view инвалидируются и удаление будет невозможно

    // Удаляем данные из словаря "Имя -> Номер/id записи"
    name_to_records_[records_[record_id].name].erase(record_id);
    
    // Если не осталось других записей с таким же именем
    if(name_to_records_[records_[record_id].name].empty()) {

        // Удаляем упоминание этого имени из базы данных
        name_to_records_.erase(records_[record_id].name);
    }
    // А если остались и другие записи с таким же именем
    else {

        // Необходимо перевесить string_view ключа с именем удаляемой записи на string какой-нибудь другой 
        // записи с таким же именем, иначе в случае, если удаляется именно та запись, на которую ссылается
        // string_view ключа с именем, ключ будет инвалидирован
        //
        // Пример: клиент добавил в базу данных телефонной книги две записи с Александром Ивановым и
        // Александром Петровым. В контейнере name_to_records_ ключ "Александр" являлся string_view,
        // который ссылался на string Александра Иванова, который лежит в контейнере records_. Если
        // удалить запись с Александром Ивановым, то string_view ключа "Александр" в name_to_records_
        // будет инвалидирован, и никаких  других Александров больше найти не получится. В таком случае
        // следует перевесить string_view на string оставшегося Александра Петрова

        // Номер/id какой-нибудь другой записи с таким же именем (будет выбрана запись с наименьшим номером/id)
        size_t another_record_id_with_same_name = *name_to_records_[records_[record_id].name].begin();

        // String_view, который будет ссылаться на string какой-нибудь другой записи с таким же именем
        string_view new_name_key = records_[another_record_id_with_same_name].name;

        // Вынимаем из словаря узел с нашем именем
        auto map_node_handler = name_to_records_.extract(records_[record_id].name);

        // Изменяем ключ у этого узла на string_view, ссылающийся на string какой-нибудь другой записи с таким же именем
        map_node_handler.key() = new_name_key;

        // Возвращаем узел обратно в словарь (так как у insert есть перегрузка с rvalue-ссылкой,
        // передадим узел через std::move, дабы не копировать его содержимое, а переместить)
        name_to_records_.insert(move(map_node_handler));
    }

    // Удаляем данные из словаря "Фамилия -> Номер/id записи"
    surname_to_records_[records_[record_id].surname].erase(record_id);

    // Если не осталось других записей с такой же фамилией
    if(surname_to_records_[records_[record_id].surname].empty()) {

        // Удаляем упоминание этой фамилии из базы данных
        surname_to_records_.erase(records_[record_id].surname);
    }
    // А если остались и другие записи с такой же фамилией
    else {

        // Необходимо перевесить string_view ключа с фамилией удаляемой записи на string какой-нибудь другой 
        // записи с такой же фамилии, иначе в случае, если удаляется именно та запись, на которую ссылается
        // string_view ключа с фамилией, ключ будет инвалидирован

        // Номер/id какой-нибудь другой записи с такой же фамилией (будет выбрана запись с наименьшим номером/id)
        size_t another_record_id_with_same_surname = *surname_to_records_[records_[record_id].surname].begin();

        // String_view, который будет ссылаться на string какой-нибудь другой записи с такой же фамилией
        string_view new_surname_key = records_[another_record_id_with_same_surname].surname;

        // Вынимаем из словаря узел с нашей фамилией
        auto map_node_handler = surname_to_records_.extract(records_[record_id].surname);

        // Изменяем ключ у этого узла на string_view, ссылающийся на string какой-нибудь другой записи с такой же фамилией
        map_node_handler.key() = new_surname_key;

        // Возвращаем узел обратно в словарь (так как у insert есть перегрузка с rvalue-ссылкой,
        // передадим узел через std::move, дабы не копировать его содержимое, а переместить)
        surname_to_records_.insert(move(map_node_handler));
    }

    // Удаляем данные из словаря "Отчество -> Номер/id записи"
    patronymic_to_records_[records_[record_id].patronymic].erase(record_id);

    // Если не осталось других записей с таким же отчеством
    if(patronymic_to_records_[records_[record_id].patronymic].empty()) {
        // Удаляем упоминание этого отчества из базы данных
        patronymic_to_records_.erase(records_[record_id].patronymic);
    }
    // А если остались и другие записи с таким же отчеством
    else {

        // Необходимо перевесить string_view ключа с отчеством удаляемой записи на string какой-нибудь другой 
        // записи с таким же отчеством, иначе в случае, если удаляется именно та запись, на которую ссылается
        // string_view ключа с отчеством, ключ будет инвалидирован

        // Номер/id какой-нибудь другой записи с таким же отчеством (будет выбрана запись с наименьшим номером/id)
        size_t another_record_id_with_same_patronymic = *patronymic_to_records_[records_[record_id].patronymic].begin();

        // String_view, который будет ссылаться на string какой-нибудь другой записи с таким же отчеством
        string_view new_patronymic_key = records_[another_record_id_with_same_patronymic].patronymic;

        // Вынимаем из словаря узел с нашем отчеством
        auto map_node_handler = patronymic_to_records_.extract(records_[record_id].patronymic);

        // Изменяем ключ у этого узла на string_view, ссылающийся на string какой-нибудь другой записи с таким же отчеством
        map_node_handler.key() = new_patronymic_key;

        // Возвращаем узел обратно в словарь (так как у insert есть перегрузка с rvalue-ссылкой,
        // передадим узел через std::move, дабы не копировать его содержимое, а переместить)
        patronymic_to_records_.insert(move(map_node_handler));
    }

    // Удаляем данные из словаря "Номер телефона -> Номер/id записи"
    number_to_record_.erase(records_[record_id].number);

    // Теперь необходимо удалить данные о встречающихся в заметке к удаляемой записи словах из словарей
    // "Номер/id записи -> Слова в заметках" и Слово в заметках -> Номер/id записи -> Частота TF"

    // Слова, которые встречаются в заметке к удаляемой записи
    set<string_view> note_words_in_record(record_to_note_words_[record_id].begin(),
                                          record_to_note_words_[record_id].end());

    // Удаляем данные из словаря "Номер/id записи -> Слова в заметках"
    record_to_note_words_.erase(record_id);

    // Удаляем данные из словаря "Слово в заметках -> Номер/id записи -> Частота TF",
    // для чего пробегаем все слова, встречавшиеся в заметке к удаляемой записи
    for(string_view word : note_words_in_record) {

        // Для каждого слова удаляем упоминание о том, что оно встречалось в заметках к удаляемой записи
        note_word_to_record_freqs_[word].erase(record_id);

        // Если так вышло, что слово больше не встречается в заметках ни к какой другой записи
        if(note_word_to_record_freqs_[word].empty()) {

            // Удаляем упоминание об этом слове из базы данных
            note_word_to_record_freqs_.erase(word);
        }
        // А если остались и другие записи с таким же словом в заметках
        else {
            // Необходимо перевесить string_view ключа со словом в заметке удаляемой записи на string какой-нибудь другой 
            // записи с таким же словом в заметках, иначе в случае, если удаляется именно та запись, на которую ссылается
            // string_view ключа со словом, ключ будет инвалидирован

            // Номер/id какой-нибудь другой записи с таким же словом в заметках (будет выбрана запись с наименьшим номером/id)
            size_t another_record_id_with_same_note_word = (*note_word_to_record_freqs_[word].begin()).first;

            // String_view, который будет ссылаться на string какой-нибудь другой записи с таким же словом в заметках
            string_view new_note_word_key = *record_to_note_words_[another_record_id_with_same_note_word].find(word);

            // Вынимаем из словаря узел с нашем словом в заметках
            auto map_node_handler = note_word_to_record_freqs_.extract(word);

            // Изменяем ключ у этого узла на string_view, ссылающийся на string какой-нибудь другой записи с таким же словом в заметках
            map_node_handler.key() = new_note_word_key;

            // Возвращаем узел обратно в словарь (так как у insert есть перегрузка с rvalue-ссылкой,
            // передадим узел через std::move, дабы не копировать его содержимое, а переместить)
            note_word_to_record_freqs_.insert(move(map_node_handler));
        }
    }  

    // И вот теперь уже можно удалить данные из словаря "Номер/id записи -> записи", в котором непосредственно
    // хранятся строковые данные, на которые ссылались string_view во вспомогательных словарях
    records_.erase(record_id);

    // Возвращаем код ответа - 1
    return 1;
}

// Функция удаления записи по номеру телефона
// (возвращает код ответа: 0 - записи с таким номером телефона не существует,
//                         1 - запись успешно удалена)
size_t PhoneBookDatabase::DeleteRecordByNumber(const string& number) {

    // Если записи с таким номером телефона не существует в базе данных, возвращаем код ответа - 0
    if(!number_to_record_.count(number)) {
        return 0;
    }

    // Получаем номер/id записи в базе данных
    size_t record_id = number_to_record_.at(number);

    // Вызываем функцию удаления записи по номеру/id записи
    return DeleteRecordById(record_id);
}

// Функция поиска записи по номеру/id записи
// (найденная запись может быть только одна или её может не быть вовсе, тогда возвращает nullopt)
optional<PhoneBookDatabase::RecordWithId> PhoneBookDatabase::FindRecordById(size_t id) const {

    // Если записи с таким номером/id не существует в базе данных, возвращаем nullopt
    if(!records_.count(id)) {
        return nullopt;
    }

    // Получаем константную ссылку на запись в базе данных
    const Record& record = records_.at(id);

    // Возвращаем запись вместе с её номером/id в базе данных
    return RecordWithId({id, record.name, record.surname, record.patronymic, record.number, record.note});
}

// Функция поиска записей по имени
// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
optional<vector<PhoneBookDatabase::RecordWithId>> PhoneBookDatabase::FindRecordsByName(const string& name) const {

    // Если записей с таким именем не существует в базе данных, возвращаем nullopt
    if(!name_to_records_.count(name)) {
        return nullopt;
    }

    // Вектор найденных записей с указанным именем
    vector<RecordWithId> result;

    // Проходим в цикле по номерам/id всех записей, содержащих указанное имя
    for(const size_t id : name_to_records_.at(name)) {

        // Получаем константную ссылку на запись в базе данных
        const Record& record = records_.at(id);

        // Добавляем запись вместе с её номером/id в базе данных в вектор найденных записей
        result.push_back({id, record.name, record.surname, record.patronymic, record.number, record.note});
    }

    // Возвращаем вектор найденных записей
    return result;
}

// Функция поиска записей по фамилии
// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
optional<vector<PhoneBookDatabase::RecordWithId>> PhoneBookDatabase::FindRecordsBySurname(const string& surname) const {

    // Если записей с такой фамилией не существует в базе данных, возвращаем nullopt
    if(!surname_to_records_.count(surname)) {
        return nullopt;
    }

    // Вектор найденных записей с указанной фамилией
    vector<RecordWithId> result;

    // Проходим в цикле по номерам/id всех записей, содержащих указанную фамилию
    for(const size_t id : surname_to_records_.at(surname)) {

        // Получаем константную ссылку на запись в базе данных
        const Record& record = records_.at(id);

        // Добавляем запись вместе с её номером/id в базе данных в вектор найденных записей
        result.push_back({id, record.name, record.surname, record.patronymic, record.number, record.note});
    }

    // Возвращаем вектор найденных записей
    return result;
}

// Функция поиска записей по отчеству
// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
optional<vector<PhoneBookDatabase::RecordWithId>> PhoneBookDatabase::FindRecordsByPatronymic(const string& patronymic) const {

    // Если записей с таким отчеством не существует в базе данных, возвращаем nullopt
    if(!patronymic_to_records_.count(patronymic)) {
        return nullopt;
    }

    // Вектор найденных записей с указанным отчеством
    vector<RecordWithId> result;

    // Проходим в цикле по номерам/id всех записей, содержащих указанное отчество
    for(const size_t id : patronymic_to_records_.at(patronymic)) {

        // Получаем константную ссылку на запись в базе данных
        const Record& record = records_.at(id);

        // Добавляем запись вместе с её номером/id в базе данных в вектор найденных записей
        result.push_back({id, record.name, record.surname, record.patronymic, record.number, record.note});
    }

    // Возвращаем вектор найденных записей
    return result;
}

// Функция поиска записи по номеру телефона
// (найденная запись может быть только одна или её может не быть вовсе, тогда возвращает nullopt)
optional<PhoneBookDatabase::RecordWithId> PhoneBookDatabase::FindRecordByNumber(const string& number) const {

    // Если записи с таким номером телефона не существует в базе данных, возвращаем nullopt
    if(!number_to_record_.count(number)) {
        return nullopt;
    }

    // Получаем номер/id записи в базе данных
    size_t record_id = number_to_record_.at(number);

    // Получаем константную ссылку на запись в базе данных
    const Record& record = records_.at(record_id);

    // Возвращаем запись вместе с её номером/id в базе данных
    return RecordWithId({record_id, record.name, record.surname, record.patronymic, record.number, record.note});
}

// Функция поиска записей по содержанию заметок
// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
optional<vector<PhoneBookDatabase::RecordWithId>> PhoneBookDatabase::FindRecordsByNote(const string& note) const {

    // Для поиска записей по содержимому заметки будем использовать механизм ранжирования записей по TF-IDF
    // (TF - Term Frequency, IDF - Inverse Document Frequency), статья про статистическую меру TF-IDF:
    // https://ru.wikipedia.org/wiki/TF-IDF

    // Словарь "Номер/id записи -> Релевантность по TF-IDF"
    map<size_t, double> record_to_relevance;

    // Разделим содержание заметки note на отдельные слова, отсортируем и удалим дубликаты
    vector<string_view> words = string_functions::SortAndRemoveDuplicates(string_functions::SplitIntoWords(note));

    // Пробежим все слова, заметки с наличием которых надо найти
    for (string_view word : words) {

        // Если слова нету в словаре "Слово в заметках -> Номер/id записи -> Частота TF", значит
        // нету записей, где это слово встречается в заметке, пропускаем его
        if (!note_word_to_record_freqs_.count(word)) {
            continue;
        }

        // Вычисляем частоту IDF (Inverse Document Frequency) для слова
        const double inverse_record_freq = ComputeWordInverseDocumentFreq(word);

        // Если слово встречается в заметке какой-либо записи, добавляем номер/id этой записи в словарь
        // "Номер/id записи -> Релевантность по TF-IDF" для отбора записей по релевантности, для этого
        // перебираем в цикле все записи, где встречается конкретное слово
        for (const auto& [record_id, term_freq] : note_word_to_record_freqs_.at(word)) {

            // Добавляем в релевантность документа TF * IDF совпавшего слова в заметке
            record_to_relevance[record_id] += term_freq * inverse_record_freq;
        }
    }

    // Замечание: можно также реализовать функционал со стоп-словами (предлоги, частицы и т.д., слова которые нужно
    // игнорировать) и минус-словами (записи, где в заметке встречаются такие слова, необходимо исключить из выборки)

    // Вектор найденных записей с упоминанием в заметках необходимых слов (пара "Документ, релевантность по TF-IDF")
    vector<pair<RecordWithId, double>> matched_records;

    // Заполняем его на основе словаря "Номер/id записи -> Релевантность по TF-IDF"
    for (const auto [record_id, relevance] : record_to_relevance) {

        // Константная ссылка на запись
        const Record& record = records_.at(record_id);

        // Добавляем запись в вектор
        matched_records.push_back({{record_id, record.name, record.surname, record.patronymic, record.number, record.note}, relevance});
    }

    // Если вектор найденных записей с упоминанием в заметках необходимых слов оказался пустым, значит записей,
    // содержащих в заметках необходимые слова, не найдено - возвращаем nullopt
    if (matched_records.empty()) {
        return nullopt;
    }

    // Сортируем найденные записи по убыванию релевантности по TF-IDF
    sort(matched_records.begin(), matched_records.end(),
        [](const pair<RecordWithId, double>& lhs, const pair<RecordWithId, double>& rhs) {
            return lhs.second > rhs.second;
        });

    // Замечание: записи с одинаковой релевантностью (релевантностями, которые отличаются только в 6 знаке после запятой) 
    // можно сортировать между собой по какому-нибудь другому признаку, например по имени/фамилии/номеру телефона и т.д.
    //
    // Замечание: если найденных записей будет очень много, можно отсечь топ из фиксированного
    // числа записей и сформировать ответ только с ними:
    //
    // const size_t MAX_RESULT_RECORDS_COUNT = 10;
    //
    // if (matched_records.size() > MAX_RESULT_RECORDS_COUNT) {
    //     matched_records.resize(MAX_RESULT_RECORDS_COUNT);
    // }

    // Итоговый вектор найденных записей с упоминанием в заметках необходимых слов
    // (идентичен вектору matched_records, но без значений релевантности по TF-IDF)
    vector<RecordWithId> result(matched_records.size());

    // Стягиваем у вектора matched_records структуры RecordWithId через std::move
    //
    // Замечание: я надеюсь, что у структуры RecordWithId будет сгенерированный по умолчанию move-конструктор
    // RecordWithId(RecordWithId&& rvalue), и тяжеловесные строки с именем, фамилией, отчеством, номером телефона
    // и заметкой будут перемещены в новый вектор, а не просто скопированы, это должно немного улучшить производительность
    for(size_t i = 0; i < matched_records.size(); ++i) {
        result[i] = move(matched_records[i].first);
    }

    // Возвращаем вектор найденных записей
    return result;
}

// Функция вычисления частоты IDF слова
// (нужна для работы функции поиска записей по содержанию заметок)
double PhoneBookDatabase::ComputeWordInverseDocumentFreq(string_view word) const {

    // Частота IDF (Inverse Document Frequency) для слова вычисляется по формуле:
    //
    // IDF = log(Число записей в базе данных / Число записей, где слово встречается в заметке)
    // (https://ru.wikipedia.org/wiki/TF-IDF)

    return log(static_cast<double>(records_.size()) /
               static_cast<double>(note_word_to_record_freqs_.at(word).size()));
}

}