# Подключим модуль для логирования
import logging

# Подключим модуль клиента для телефонной книги
from phone_book_client import *

# Функция входной точки клиента для телефонной книги
def main():
    # Будем вести логирование в файл
    logging.basicConfig(level=logging.INFO, filename='client_log.log', filemode='w')
    
    # Полный адрес сервера с портом
    adress = 'localhost:50051'

    # Тестирование и демонстрация работы функционала
    # Замечание: можно написать отдельные Unit-test'ы с assert'ами
    
    # Добавим шесть записей с именем Александр (id = 1, 2, 3, 4, 5, 6)
    PrintResponseCode(AddRecord(adress, 'Александр', 'Иванов',   'Петрович',       '+79743102164', 'C++ intern developer'))    # id = 1
    PrintResponseCode(AddRecord(adress, 'Александр', 'Петров',   'Иванович',       '+79754213275', 'C++ junior developer'))    # id = 2
    PrintResponseCode(AddRecord(adress, 'Александр', 'Сидоров',  'Николаевич',     '+79765324386', 'C++ middle developer'))    # id = 3
    PrintResponseCode(AddRecord(adress, 'Александр', 'Смирнов',  'Константинович', '+79776435497', 'C++ senior developer'))    # id = 4
    PrintResponseCode(AddRecord(adress, 'Александр', 'Соколов',  'Алексеевич',     '+79787546508', 'Python senior developer')) # id = 5
    PrintResponseCode(AddRecord(adress, 'Александр', 'Васильев', 'Павлович',       '+79798657619', 'Rust middle developer'  )) # id = 6

    # Просим добавить запись с таким же номером телефона, как у первого Александра (это сделать не получится)
    PrintResponseCode(AddRecord(adress, 'Сергей', 'Буркатовский', 'Борисович', '+79743102164', 'Boss of the gym'))

    # Просим найти запись с id = 4
    PrintRecord(FindRecordById(adress, 4))

    # Просим найти запись с id = 404 (такой записи не существует)
    PrintRecord(FindRecordById(adress, 404))

    # Просим найти все записи с именем Александр, найдутся с id = 1, 2, 3, 4, 5, 6
    PrintRecords(FindRecordsByName(adress, 'Александр'))

    # Удалим три записи с id = 1, 3, 6
    PrintResponseCode(DeleteRecordById(adress, 1))
    PrintResponseCode(DeleteRecordById(adress, 3))
    PrintResponseCode(DeleteRecordById(adress, 6))

    # Удалим запись с id = 404 (такой записи не существует)
    PrintResponseCode(DeleteRecordById(adress, 404))

    # Просим найти все записи с именем Александр, найдутся с id = 2, 4, 5
    PrintRecords(FindRecordsByName(adress, 'Александр'))

    # Добавим шесть записей с фамилией Куликов (id = 7, 8, 9, 10, 11, 12)
    PrintResponseCode(AddRecord(adress, 'Антон',    'Куликов', 'Денисович' , '+79638245371', 'UX/UI designer' ))               # id = 7
    PrintResponseCode(AddRecord(adress, 'Артём',    'Куликов', 'Максимович', '+79649356482', 'Level designer' ))               # id = 8
    PrintResponseCode(AddRecord(adress, 'Андрей',   'Куликов', 'Дмитриевич', '+79650467593', '3ds Max artist' ))               # id = 9
    PrintResponseCode(AddRecord(adress, 'Алексей',  'Куликов', 'Семёнович',  '+79661578604', '3ds Max rendering specialist' )) # id = 10
    PrintResponseCode(AddRecord(adress, 'Арсений',  'Куликов', 'Евгеньевич', '+79672689715', '3ds Max plugin developer' ))     # id = 11
    PrintResponseCode(AddRecord(adress, 'Анатолий', 'Куликов', 'Михайлович', '+79683790826', 'Havok developer' ))              # id = 12

    # Просим найти все записи с фамилией Куликов, найдутся с id = 7, 8, 9, 10, 11, 12
    PrintRecords(FindRecordsBySurname(adress, 'Куликов'))

    # Удалим запись с номером +79638245371 (т.е. с id = 8)
    PrintResponseCode(DeleteRecordByNumber(adress, '+79649356482'))

    # Удалим запись с номером +79661578604 (т.е. с id = 10)
    PrintResponseCode(DeleteRecordByNumber(adress, '+79661578604'))

    # Удалим запись с номером 88005553535 (такой записи не существует)
    PrintResponseCode(DeleteRecordByNumber(adress, '88005553535'))

    # Просим найти все записи с фамилией Куликов, найдутся с id = 7, 9, 11, 12
    PrintRecords(FindRecordsBySurname(adress, 'Куликов'))

    # Добавим шесть записей с отчеством Сергеевна (id = 13, 14, 15, 16, 17, 18)
    PrintResponseCode(AddRecord(adress, 'Анна',      'Лебедева', 'Сергеевна', '+79847358427', 'Product manager'              )) # id = 13
    PrintResponseCode(AddRecord(adress, 'Алина',     'Жукова',   'Сергеевна', '+79858469538', 'PR manager'                   )) # id = 14
    PrintResponseCode(AddRecord(adress, 'Ксения',    'Зайцева',  'Сергеевна', '+79869570649', 'Photoshop artist'             )) # id = 15
    PrintResponseCode(AddRecord(adress, 'София',     'Волкова',  'Сергеевна', '+79870681750', 'Data scientist'               )) # id = 16
    PrintResponseCode(AddRecord(adress, 'Мария',     'Тарасова', 'Сергеевна', '+79881792861', 'Data engineer'                )) # id = 17
    PrintResponseCode(AddRecord(adress, 'Анастасия', 'Фомина',   'Сергеевна', '+79892803972', 'C++ teamlead senior developer')) # id = 18

    # Просим найти все записи с отчеством Сергеевна, найдутся с id = 13, 14, 15, 16, 17, 18
    PrintRecords(FindRecordsByPatronymic(adress, 'Сергеевна'))

    # Просим найти все записи, в заметках которых есть слова из запроса "C++ teamlead senior developer"
    #
    # В порядке уменьшения релевантности по TF-IDF должны найтись записи с
    # id = 18 (C++ teamlead senior developer; 4 общих слова: С++, teamlead, senior, developer)
    # id = 4  (C++ senior developer;          3 общих слова: С++, senior, developer)
    # id = 2  (C++ junior developer;          2 общих слова: С++, developer)
    # id = 5  (Python senior developer;       2 общих слова: senior, developer)
    # id = 12 (Havok developer;               1 общих слова: developer)
    # id = 11 (3ds Max plugin developer;      1 общих слова: developer)
    #
    # Замечание: у записи с id = 12 значение TF больше, т.к. в ней меньше слов, чем у записи с id = 11,
    # поэтому в выборке она идёт раньше, чем запись с id = 11
    #
    # Замечание: можно также реализовать функционал со стоп-словами (предлоги, частицы и т.д., слова которые нужно
    # игнорировать) и минус-словами (записи, где в заметке встречаются такие слова, необходимо исключить из выборки),
    # записи с одинаковой релевантностью (релевантностями, которые отличаются только в 6 знаке после запятой) можно
    # сортировать между собой по какому-нибудь другому признаку, например по имени/фамилии/номеру телефона и т.д.
    #
    # Замечание: следующие записи не будут найдены, так как их удалили до момента поиска
    # id = 1 (C++ intern developer)
    # id = 3 (C++ middle developer)
    # id = 6 (Rust middle developer)
    PrintRecords(FindRecordsByNote(adress, 'C++ teamlead senior developer'))

    # Просим найти все записи, в заметках которых есть слова из запроса "Likes artillery" (таких записей не существует)
    PrintRecords(FindRecordsByNote(adress, 'Likes artillery'))

# Входная точка приложения клиента для телефонной книги
if __name__ == '__main__':
    main()