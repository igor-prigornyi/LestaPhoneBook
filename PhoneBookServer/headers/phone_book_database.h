// Заголовочный файл phone_book_database.h описывает работу базы данных для телефонной книги,
// которая используется в сервере для телефонной книги, предоставляя необходимый функционал
// по хранению, изменению и поиску данных

// Header guard (предотвращает повторное включение заголовочного файла)
#pragma once

// Подключим библиотеку optional для работы со случаями, когда результатом запроса к базе данных
// может быть пустой ответ, библиотеку string для работы со строками, библиотеки vector, map и set
// для использования контейнеров вектора, словаря и множества
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <set>

// Не будем использовать using-директивы в глобальной области видимости заголовочного файла, так как это
// приведёт к попаданию этих using-директив во все области видимости, куда будет включён заголовочный файл

// Пространство имён базы данных для телефонной книги
namespace phone_book_database {

// Архитектура кода базы данных:
//
// Класс базы данных для телефонной книги снабжён двумя структурами для описания записи в базе: Record 
// и RecordWithId. Первая структура хранит в себе имя, фамилию, отчество, телефонный номер и заметку,
// вторая помимо этого ещё и номер/id записи. Внутри класса базы данных хранение данных осуществляется
// с помощью структуры Record, а структура RecordWithId используется лишь для возврата результатов
// запросов из функций поиска записей по какому-либо критерию.
//
// Физически данные хранятся в словаре (map'е) "Номер/id записи -> записи":
//    map<size_t, Record> records_;
//
// Также для быстрого поиска записей по имени/фамилии/отчеству/номеру телефона введены следующие словари:
//
// 1) Словарь "Имя -> Номер/id записи":
//    map<string_view, set<size_t>> name_to_records_;
//
// 2) Словарь "Фамилия -> Номер/id записи":
//    map<string_view, set<size_t>> surname_to_records_;
//
// 3) Словарь "Отчество -> Номер/id записи":
//    map<string_view, set<size_t>> patronymic_to_records_;
//
// 4) Словарь "Номер телефона -> Номер/id записи":
//    map<string_view, size_t> number_to_record_;
//
// Ключами во всех этих словарях (map'ах) являются не строки (string'и), а ссылки на строки (string_view),
// которые физически хранятся в контейнере records_, что уменьшает объём используемой базой данной оперативной
// памяти. Необходимо следить за тем, чтобы string_view при инициализации ссылались именно на строки из
// контейнера records_, а при удалении записей, вначале необходимо удалять содержимое контейнеров, где
// ключами являются именно string_view, а потом уже удалять содержимое контейнера records_, иначе произойдёт
// инвалидация ссылок на строки. Также при удалении записи через методы DeleteRecordById и DeleteRecordByNumber
// необходимо перевешивать string_view в ключах и значениях на string'и других записей, иначе также произойдёт
// инвалидация ссылок на строки. 
//
// Замечание: возможно, в некоторых случаях можно (стоит) использовать не std::map, реализованный на основе
// бинарного дерева поиска, а std::unordered_map, реализованный на основе hash-таблицы.
//
// Замечание: в случае реализации параллельной работы handler'ов, обрабатывающих соединения с клиентами
// (например, с помощью Thread Pool'а), необходимо огородить участки работы с контейнерами mutex'ами,
// чтобы избежать состояния гонки. 
//
// Для поиска записей по содержимому заметки будем использовать механизм ранжирования записей по TF-IDF
// (TF - Term Frequency, IDF - Inverse Document Frequency), выдавая в качестве ответа на запрос набор
// записей, отсортированных по уменьшению релевантности TF-IDF. Для хранения значений TF будем использовать
// два словаря:
//
// 5) Словарь "Слово в заметках -> Номер/id записи -> Частота TF":
//    map<string_view, map<size_t, double>> note_word_to_record_freqs_;
//
// 6) Словарь "Номер/id записи -> Слова в заметках"
//    map<size_t, set<string_view>> record_to_note_words_;
//
// Статья про статистическую меру TF-IDF: https://ru.wikipedia.org/wiki/TF-IDF
//
// Вспомогательные словари 1)-4) и 5)-6) дополняются информацией в момент добавлений новой записи через метод
// AddRecord. В момент удаления записи через методы DeleteRecordById и DeleteRecordByNumber данные, касающиеся
// удаляемой записи, удаляются и из вспомогательных словарей 1)-4) и 5)-6). Значение IDF будет вычисляться в
// момент поиска записей по содержанию заметок через метод FindRecordsByNote.
//
// У каждой записи есть её уникальный номер/id, который служит ключом в контейнере records_. Также, вообще
// говоря, уникальным идентификатором является и телефонный номер, который не может повторяться у двух
// разных записей (он сам по себе уже мог бы быть хорошим hash'ом, если бы мы использовали unordered_map'ы).
// Для контроля выдачи уникальных номеров/id добавляемым записям есть поле last_record_id_, которое
// содержит значение номера/id последнего добавленного документа и инкрементируется при добавлении нового
// документа в базу данных.
//
// Для хранения состояния базы данных в перерывах между перезапусками сервера реализованы методы загрузки
// данных в базу из файла (LoadFromFile) и сохранения данных из базы в файл (SaveToFile). Значение поля
// last_record_id_ также будет храниться в файле.

// Класс базы данных для телефонной книги
class PhoneBookDatabase final {
public:
	// Структура записи в телефонной книге
	struct Record {
		std::string name;       // Имя
		std::string surname;    // Фамилия
		std::string patronymic; // Отчество
		std::string number;     // Телефонный номер
		std::string note;       // Заметка
	};

	// Структура записи в телефонной книге c полем с номером/id записи
	// (можно сделать через наследование от Record, но тогда поле id будет последним, а хотелось бы сделать
	// его первым для удобного вызова конструктора в формате {id, name, surname, patronymic, number, note})
	struct RecordWithId
	{
		size_t id;              // Номер/id записи
		std::string name;       // Имя
		std::string surname;    // Фамилия
		std::string patronymic; // Отчество
		std::string number;     // Телефонный номер
		std::string note;       // Заметка
	};
	
private:
    // Имя файла с базой данных телефонной книги
    std::string database_file_name_;

	// Словарь "Номер/id записи -> записи"
	// (именно в этом контейнере хранятся строковые данные, в остальных лишь ссылки (string_view))
	std::map<size_t, Record> records_;

	// Словарь "Имя -> Номер/id записи"
	// (используется для быстрого поиска записей по имени)
	std::map<std::string_view, std::set<size_t>> name_to_records_;

	// Словарь "Фамилия -> Номер/id записи"
	// (используется для быстрого поиска записей по фамилии)
	std::map<std::string_view, std::set<size_t>> surname_to_records_;

	// Словарь "Отчество -> Номер/id записи"
	// (используется для быстрого поиска записей по отчеству)
	std::map<std::string_view, std::set<size_t>> patronymic_to_records_;

	// Словарь "Номер телефона -> Номер/id записи"
	// (используется для быстрого поиска записей по номеру телефона)
	std::map<std::string_view, size_t> number_to_record_;

	// Словарь "Слово в заметках -> Номер/id записи -> Частота TF"
	// (используется для быстрого поиска записей по содержимому заметки и получения выборки, ранжированной по TF-IDF)
	std::map<std::string_view, std::map<size_t, double>> note_word_to_record_freqs_;

	// Словарь "Номер/id записи -> Слова в заметках"
	// (используется для быстрого поиска записей по содержимому заметки и получения выборки, ранжированной по TF-IDF)
	std::map<size_t, std::set<std::string_view>> record_to_note_words_;

	// Номер/id последней записи
	size_t last_record_id_;
	
public:
    // Конструктор базы данных принимает имя файла (полное имя с путём до файла) с базой данных телефонной книги
    // (определение/definition этой функции находится в phone_book_database.cpp)
    explicit PhoneBookDatabase(const std::string& database_file_name);

    // Функция загрузки данных в базу из файла
    // (определение/definition этой функции находится в phone_book_database.cpp)
    void LoadFromFile();

    // Функция сохранения данных из базы в файл
    // (определение/definition этой функции находится в phone_book_database.cpp)
    void SaveToFile() const;

    // Функция добавления записи
    // (возвращает код ответа: 0 - запись с таким номером телефона уже существует,
    //                         1 - запись успешно добавлена)
    //
    // (определение/definition этой функции находится в phone_book_database.cpp)
	size_t AddRecord(const Record& record);

	// Функция удаления записи по её номеру/id
    // (возвращает код ответа: 0 - записи с таким номером/id не существует,
    //                         1 - запись успешно удалена)
    //
    // (определение/definition этой функции находится в phone_book_database.cpp)
	size_t DeleteRecordById(size_t record_id);

    // Функция удаления записи по номеру телефона
    // (возвращает код ответа: 0 - записи с таким номером телефона не существует,
    //                         1 - запись успешно удалена)
    //
    // (определение/definition этой функции находится в phone_book_database.cpp)
	size_t DeleteRecordByNumber(const std::string& number);

    // Функция поиска записи по номеру/id записи
    // (найденная запись может быть только одна или её может не быть вовсе, тогда возвращает nullopt)
    //
    // (определение/definition этой функции находится в phone_book_database.cpp)
	std::optional<RecordWithId> FindRecordById(size_t id) const;

    // Функция поиска записей по имени
	// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
	//
    // (определение/definition этой функции находится в phone_book_database.cpp)
	std::optional<std::vector<RecordWithId>> FindRecordsByName(const std::string& name) const;

	// Функция поиска записей по фамилии
	// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
	//
    // (определение/definition этой функции находится в phone_book_database.cpp)
	std::optional<std::vector<RecordWithId>> FindRecordsBySurname(const std::string& surname) const;

	// Функция поиска записей по отчеству
	// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
	//
    // (определение/definition этой функции находится в phone_book_database.cpp)
	std::optional<std::vector<RecordWithId>> FindRecordsByPatronymic(const std::string& patronymic) const;

	// Функция поиска записи по номеру телефона
	// (найденная запись может быть только одна или её может не быть вовсе, тогда возвращает nullopt)
	//
    // (определение/definition этой функции находится в phone_book_database.cpp)
	std::optional<RecordWithId> FindRecordByNumber(const std::string& number) const;

	// Функция поиска записей по содержанию заметок
	// (найденных записей может быть множество или не быть вовсе, тогда возвращает nullopt)
	//
    // (определение/definition этой функции находится в phone_book_database.cpp)
	std::optional<std::vector<RecordWithId>> FindRecordsByNote(const std::string& note) const;

private:
	// Функция добавления записи с фиксированным номером/id
    // (возвращает код ответа: 0 - запись с таким номером телефона уже существует,
    //                         1 - запись успешно добавлена)
    //
    // (определение/definition этой функции находится в phone_book_database.cpp)
	size_t AddRecordById(size_t record_id, const Record& record);

	// Функция вычисления частоты IDF слова
	// (нужна для работы функции поиска записей по содержанию заметок)
	//
	// (определение/definition этой функции находится в phone_book_database.cpp)
    double ComputeWordInverseDocumentFreq(std::string_view word) const;
};

}