// Единица трансляции main.cpp содержит входную точку программы и реализует интерфейс

// Подключим библиотеку iostream для работы стандартного потока вывода в консоль для
// отображения статуса работы сервера, библиотеку string для работы со строками и
// библиотеку chrono для работы со временем
#include <iostream>
#include <string>
#include <chrono>

// Подключим заголовочный файл с функцией getch для Linux
#include "linux_getch_function.h"

// Подключим заголовочный файл сервера для телефонной книги
#include "phone_book_server.h"

// Подключим заголовочный файл базы данных для телефонной книги
// (вообще говоря, он подключен внутри phone_book_server.h, поэтому необязательно)
#include "phone_book_database.h"

// Подключим пространства имён std и chrono_literals
using namespace std;
using namespace std::chrono_literals;

// Входная точка приложения сервера для телефонной книги
int main(int argc, char** argv) {

    // Имя файла с базой данных телефонной книги
    const string database_name = "database.db"s; 

    // IP-адрес сервера телефонной книги
    const string server_ip = "0.0.0.0"s;

    // Порт для работы сервера телефонной книги
    const uint16_t server_port = 50051; 

    // Создаём базу данных телефонной книги, загружая данные из файла
    phone_book_database::PhoneBookDatabase database(database_name);

    // Создаём сервер телефонной книги, передавая ему IP-адрес и порт
    phone_book_server::PhoneBookServer server(server_ip, server_port, database);

    // Запускаем сервер в отдельном потоке
    server.RunInNewThread();

    // Ожидаем 500мс, пока сервер запустится
    this_thread::sleep_for(500ms);

    // В основном потоке будем ожидать от оператора сервера нажатия кнопки ESC или пробела для
    // остановки работы сервера, заблокировав выполнение потока функцией getch
    char interruption_key; while(true) {

        // Информируем в консоль, что для остановки работы сервера необходимо нажать ESC или пробел
        cout << "[To shutdown the server press ESC or SPACE]" << endl;

        // Замечание: может получиться так, что сервер, работающий на параллельном потоке, выведет
        // в консоль что-либо в момент, когда функция getch изменит параметры ввода-вывода в терминал.
        // Вообще, стоило бы написать GUI и сделать отдельную кнопку для остановки сервера
        interruption_key = linux_getch_function::getch();

        // Проверяем, не была ли нажата кнопка ESC (код 27) или пробел (код 32),
        // и прерываем блокирование основного потока
        if (interruption_key == 27 || interruption_key == 32) {
            break;
        }
    }

    // Информируем в консоль о том, что была нажата кнопка ESC или пробел
    cout << "[ESC or SPACE was pressed, shutting down the server ...]"s << endl;

    // Останавливаем сервер
    server.Shutdown();

    // Ссохранение данных из базы данных телефонной книги в файл реализовано в деструкторе PhoneBookDatabase,
    // однако можно вызвать и вручную:
    // database.SaveToFile();

    return 0;
}