// Единица трансляции phone_book_server.cpp описывает работу сервера для телефонной книги

// Подключим библиотеку stdexcept для работы со стандартными исключениями и библиотеку
// optional для работы со случаями, когда результатом запроса может быть пустой ответ
#include <stdexcept>
#include <optional>

// Подключим заголовочный файл сервера для телефонной книги
#include "phone_book_server.h"

// Подключим пространство имён std
using namespace std;

// Подключим пространство имён базы данных для телефонной книги
using namespace phone_book_database;

// Пространство имён сервера для телефонной книги
namespace phone_book_server {

// Пространство имён функций, обрабатывающих соединения. Эти функции обрабатывают входящие запросы
// (reqest'ы) и формируют ответы (response'ы), содержа в себе логику формирования ответа, обращаясь
// к базе данных телефонной книги
namespace connection_processing_functions {

// Функция обработки запроса на добавление записи (тип 1-1)
// (клиент получает код ответа: 0 - запись с таким номером телефона уже существует,
//                              1 - запись успешно добавлена)
void AddRecordProcessingFunction(PhoneBookDatabase& database,
                                 RecordRequest* request,
                                 AddRecordResponse* response,
                                 const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на добавление записи 
    cout << "[1-1 handler #"s << handler_tag << "]: AddRecord request, name=\""s       << request->name()       << 
                                                                  "\", surname=\""s    << request->surname()    <<
                                                                  "\", patronymic=\""s << request->patronymic() <<
                                                                  "\", number=\""s     << request->number()     <<
                                                                  "\", note=\""s       << request->note()       << "\""s << endl;

    // Записываем в структуру данные для добавления записи в базу данных
    PhoneBookDatabase::Record record({request->name(),
                                      request->surname(),
                                      request->patronymic(),
                                      request->number(),
                                      request->note()});
    
    // Добавляем запись в базу данных, получаем код ответа
    // (0 - запись с таким номером телефона уже существует, 1 - запись успешно добавлена)
    size_t code = database.AddRecord(record);

    // Отсылаем клиенту код ответа
    response->set_code(code);
}

// Функция обработки запроса на удаление записи по номеру записи (тип 1-1)
// (клиент получает код ответа: 0 - записи с таким номером/id не существует,
//                              1 - запись успешно удалена)
void DeleteRecordByIdProcessingFunction(PhoneBookDatabase& database,
                                        DeleteRecordByIdRequest* request,
                                        DeleteRecordResponse* response,
                                        const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на удаление записи по номеру записи
    cout << "[1-1 handler #"s << handler_tag << "]: DeleteRecordById request, id=\""s << request->id() << "\""s << endl;

    // Удаляем запись из базы данных, получаем код ответа
    // (0 - записи с таким номером/id не существует, 1 - запись успешно удалена)
    size_t code = database.DeleteRecordById(request->id());

    // Отсылаем клиенту код ответа
    response->set_code(code);
}

// Функция обработки запроса на удаление записи по номеру телефона (тип 1-1)
// (клиент получает код ответа: 0 - записи с таким номером телефона не существует,
//                              1 - запись успешно удалена)
void DeleteRecordByNumberProcessingFunction(PhoneBookDatabase& database,
                                            DeleteRecordByNumberRequest* request,
                                            DeleteRecordResponse* response,
                                            const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на удаление записи по номеру телефона
    cout << "[1-1 handler #"s << handler_tag << "]: DeleteRecordByNumber request, number=\""s << request->number() << "\""s << endl;

    // Удаляем запись из базы данных, получаем код ответа
    // (0 - записи с таким номером телефона не существует, 1 - запись успешно удалена)
    size_t code = database.DeleteRecordByNumber(request->number());

    // Отсылаем клиенту код ответа
    response->set_code(code);
}

// Функция обработки запроса на поиск записи по номеру/id записи (тип 1-1)
// (найденная запись может быть только одна или её может не быть вовсе, тогда формируем пустой ответ с id = 0)
void FindRecordByIdProcessingFunction(PhoneBookDatabase& database,
                                      FindRecordByIdRequest* request,
                                      RecordResponse* response,
                                      const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на поиск записи по номеру/id записи
    cout << "[1-1 handler #"s << handler_tag << "]: FindRecordById request, id=\""s << request->id() << "\""s << endl;

    // Ищем запись в базе данных, если её нет - получаем nullopt
    optional<PhoneBookDatabase::RecordWithId> record = database.FindRecordById(request->id());

    // Если запись была найдена, формируем ответ клиенту с ней
    if(record.has_value()) {
        response->set_id        (record.value().id        );
        response->set_name      (record.value().name      );
        response->set_surname   (record.value().surname   );
        response->set_patronymic(record.value().patronymic);
        response->set_number    (record.value().number    );
        response->set_note      (record.value().note      );
    }

    // Иначе формируем пустую запись с id = 0, для этого ничего не надо делать
}

// Функция обработки запроса на поиск записей по имени (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsByNameProcessingFunction(PhoneBookDatabase& database,
                                         FindRecordsByNameRequest* request,
                                         vector<RecordResponse>* response,
                                         const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на поиск записи по имени
    cout << "[1-M handler #"s << handler_tag << "]: FindRecordsByName request, name=\""s << request->name() << "\""s << endl;

    // Ищем записи в базе данных, если их нет - получаем nullopt
    optional<vector<PhoneBookDatabase::RecordWithId>> records = database.FindRecordsByName(request->name());

    // Если записи были найдены, формируем ответ клиенту с ними
    if(records.has_value()) {
        for(const PhoneBookDatabase::RecordWithId& record : records.value()) {
            RecordResponse response_element;

            response_element.set_id        (record.id        );
            response_element.set_name      (record.name      );
            response_element.set_surname   (record.surname   );
            response_element.set_patronymic(record.patronymic);
            response_element.set_number    (record.number    );
            response_element.set_note      (record.note      );

            response->push_back(response_element);
        }
    }

    // Иначе формируем пустой вектор ответов, для этого ничего не надо делать
}

// Функция обработки запроса на поиск записей по фамилии (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsBySurnameProcessingFunction(PhoneBookDatabase& database,
                                            FindRecordsBySurnameRequest* request,
                                            vector<RecordResponse>* response,
                                            const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на поиск записи по фамилии
    cout << "[1-M handler #"s << handler_tag << "]: FindRecordsBySurname request, surname=\""s << request->surname() << "\""s << endl;

    // Ищем записи в базе данных, если их нет - получаем nullopt
    optional<vector<PhoneBookDatabase::RecordWithId>> records = database.FindRecordsBySurname(request->surname());

    // Если записи были найдены, формируем ответ клиенту с ними
    if(records.has_value()) {
        for(const PhoneBookDatabase::RecordWithId& record : records.value()) {
            RecordResponse response_element;

            response_element.set_id        (record.id        );
            response_element.set_name      (record.name      );
            response_element.set_surname   (record.surname   );
            response_element.set_patronymic(record.patronymic);
            response_element.set_number    (record.number    );
            response_element.set_note      (record.note      );

            response->push_back(response_element);
        }
    }

    // Иначе формируем пустой вектор ответов, для этого ничего не надо делать
}

// Функция обработки запроса на поиск записей по отчеству (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsByPatronymicProcessingFunction(PhoneBookDatabase& database,
                                               FindRecordsByPatronymicRequest* request,
                                               vector<RecordResponse>* response,
                                               const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на поиск записи по отчеству
    cout << "[1-M handler #"s << handler_tag << "]: FindRecordsByPatronymic request, patronymic=\""s << request->patronymic() << "\""s << endl;

    // Ищем записи в базе данных, если их нет - получаем nullopt
    optional<vector<PhoneBookDatabase::RecordWithId>> records = database.FindRecordsByPatronymic(request->patronymic());

    // Если записи были найдены, формируем ответ клиенту с ними
    if(records.has_value()) {
        for(const PhoneBookDatabase::RecordWithId& record : records.value()) {
            RecordResponse response_element;

            response_element.set_id        (record.id        );
            response_element.set_name      (record.name      );
            response_element.set_surname   (record.surname   );
            response_element.set_patronymic(record.patronymic);
            response_element.set_number    (record.number    );
            response_element.set_note      (record.note      );

            response->push_back(response_element);
        }
    }

    // Иначе формируем пустой вектор ответов, для этого ничего не надо делать
}

// Функция обработки запроса на поиск записи по номеру телефона (тип 1-1)
// (найденная запись может быть только одна или её может не быть вовсе, тогда формируем пустой ответ с id = 0)
void FindRecordByNumberProcessingFunction(PhoneBookDatabase& database,
                                          FindRecordByNumberRequest* request,
                                          RecordResponse* response,
                                          const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на поиск записи по номеру/id записи
    cout << "[1-1 handler #"s << handler_tag << "]: FindRecordByNumber request, number=\""s << request->number() << "\""s << endl;

    // Ищем запись в базе данных, если её нет - получаем nullopt
    optional<PhoneBookDatabase::RecordWithId> record = database.FindRecordByNumber(request->number());

    // Если запись была найдена, формируем ответ клиенту с ней
    if(record.has_value()) {
        response->set_id        (record.value().id        );
        response->set_name      (record.value().name      );
        response->set_surname   (record.value().surname   );
        response->set_patronymic(record.value().patronymic);
        response->set_number    (record.value().number    );
        response->set_note      (record.value().note      );
    }

    // Иначе формируем пустую запись с id = 0, для этого ничего не надо делать
}

// Функция обработки запроса на поиск записей по заметке (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsByNoteProcessingFunction(PhoneBookDatabase& database,
                                         FindRecordsByNoteRequest* request,
                                         vector<RecordResponse>* response,
                                         const void* handler_tag) {

    // Информируем в консоль о поступлении запроса на поиск записи по заметке
    cout << "[1-M handler #"s << handler_tag << "]: FindRecordsByNote request, note=\""s << request->note() << "\""s << endl;

    // Ищем записи в базе данных, если их нет - получаем nullopt
    optional<vector<PhoneBookDatabase::RecordWithId>> records = database.FindRecordsByNote(request->note());

    // Если записи были найдены, формируем ответ клиенту с ними
    if(records.has_value()) {
        for(const PhoneBookDatabase::RecordWithId& record : records.value()) {
            RecordResponse response_element;

            response_element.set_id        (record.id        );
            response_element.set_name      (record.name      );
            response_element.set_surname   (record.surname   );
            response_element.set_patronymic(record.patronymic);
            response_element.set_number    (record.number    );
            response_element.set_note      (record.note      );

            response->push_back(response_element);
        }
    }

    // Иначе формируем пустой вектор ответов, для этого ничего не надо делать
}

}

// Конструктор сервера принимает IP-адрес сервера, порт для работы сервера и неконстантную ссылку на базу
// данных телефонной книги, инициализирует nullptr'ом указатель на поток выполнения сервера и 
// устанавливает статус сервера в CREATED
PhoneBookServer::PhoneBookServer(const string& ip,
                                 uint16_t port,
                                 PhoneBookDatabase& database) : ip_(ip),
                                                                port_(port),
                                                                database_(database),
                                                                server_thread_(nullptr),
                                                                server_status_(ServerStatus::CREATED) { }

// Деструктор сервера
PhoneBookServer::~PhoneBookServer() {

    // Останавливаем сервер, если он имеет статус RUNNING (т.е. если функция Shutdown так и не была вызвана)
    if(server_status_ == ServerStatus::RUNNING) {
        Shutdown();
    }

    // Сохраняем данные из базы данных в файл
    database_.SaveToFile();

    // Помимо этого, умные указатели (RAII-объекты) на объект gRPC-сервера и объект очереди handler'ов соединений
    // уничтожат объекты, на которые они ссылаются, высвобождая ресурсы в heap'е
}

// Функция запуска сервера в отдельном потоке
void PhoneBookServer::RunInNewThread() {

    // Если сервер имеет статус RUNNING, значит он уже запущен, выдаём exception
    if (server_status_ == ServerStatus::RUNNING) {
        throw logic_error("Server is already running"s);
    }
    // Если сервер имеет статус SHUTTING_DOWN, значит он останавливается, выдаём exception
    if (server_status_ == ServerStatus::SHUTTING_DOWN) {
        throw logic_error("Server is shutting down"s);
    }
    // Если сервер имеет статус SHUTDOWNED, значит он был остановлен, выдаём exception
    //
    // (для запуска сервера вновь необхолимо создать новый объект типа PhoneBookServer, ведь
    // старый должен высвободить ресурсы путём вызова деструктора и уничтожения объектов,
    // содержащихся в его полях по значению и по умным указателям)
    if (server_status_ == ServerStatus::SHUTTING_DOWN) {
        throw logic_error("Server was shutdowned"s);
    }

    // В остальных случаях запускаем сервер

    // Полный адрес сервера с портом
    string address = ip_ + ":"s + to_string(port_);

    // Информируем в консоль о начале запуска сервера на указанном адресе
    cout << "[Starting server listening on "s << address << " ...]"s << endl;

    // Factory/builder для создания gRPC-сервера
    ServerBuilder builder;

    // Будем прослушивать входящие подключения на указанном порту без аутентификации 
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());

    // Зарегистрируем наш сервис асинхронной gRPC-коммуникации за нашим gRPC-сервером
    builder.RegisterService(&service_);

    // Зарегистрируем нашу очередь handler'ов за нашим gRPC-сервером
    handlers_queue_ = builder.AddCompletionQueue();

    // Создаём и запускаем gRPC-сервер
    server_ = builder.BuildAndStart();

    // Меняем статус сервера на RUNNING
    server_status_ = ServerStatus::RUNNING;

    // Вызываем функцию инициализации очереди handler'ов
    HandlerQueueInitialization();

    // Вызываем функцию циклического итерирования по очереди handler'ов (для данного экземпляра
    // класса сервера) в отдельном потоке и записываем указатель на него в server_thread_
    server_thread_ = new thread(&PhoneBookServer::HandlerQueueLoop, this);
}

// Функция остановки сервера
void PhoneBookServer::Shutdown() {
    // Информируем в консоль о том, что начата остановка работы сервера
    cout << "[Shutdowning the server ...]"s << endl;

    // Устанавливаем статус сервера в SHUTTING_DOWN
    server_status_ = ServerStatus::SHUTTING_DOWN;

    server_->Shutdown();         // Остановка gRPC-сервера
    handlers_queue_->Shutdown(); // Остановка очереди handler'ов соединений

    // Устанавливаем статус сервера в SHUTDOWNED
    server_status_ = ServerStatus::SHUTDOWNED;
}

// Функция инициализации очереди handler'ов
void PhoneBookServer::HandlerQueueInitialization() {
    // Подключаем пространство имён с функциями, обрабатывающими соединения
    using namespace connection_processing_functions;

    // Создаём в heap'е по одному handler'у для каждого типа соединения. При вызове конструктора они автоматически
    // будут отправляться в очередь handler'ов, связываться с сервисом асинхронной gRPC-коммуникации и становиться
    // на прослушивание порта
    //
    // Замечание: возможно, генерацию кода по созданию handler'ов можно запустить при помощи макросов

    // Информируем в консоль о начале создания первых handler'ов для обработки всех типов соединений
    cout << "[Creating first handlers for each connection type ...]"s << endl;

    // Создаём первый handler для обработок запросов AddRecord (тип 1-1)
    new OneToOneConnectionHandler <RecordRequest,
                                   AddRecordResponse,
                                   &AsyncService::RequestAddRecord,
                                   AddRecordProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов DeleteRecordById (тип 1-1)
    new OneToOneConnectionHandler <DeleteRecordByIdRequest,
                                   DeleteRecordResponse,
                                   &AsyncService::RequestDeleteRecordById,
                                   DeleteRecordByIdProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов DeleteRecordByNumber (тип 1-1)
    new OneToOneConnectionHandler <DeleteRecordByNumberRequest,
                                   DeleteRecordResponse,
                                   &AsyncService::RequestDeleteRecordByNumber,
                                   DeleteRecordByNumberProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов FindRecordById (тип 1-1)
    new OneToOneConnectionHandler <FindRecordByIdRequest,
                                   RecordResponse,
                                   &AsyncService::RequestFindRecordById,
                                   FindRecordByIdProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов FindRecordsByName (тип 1-M)
    new OneToManyConnectionHandler <FindRecordsByNameRequest,
                                    RecordResponse,
                                    &AsyncService::RequestFindRecordsByName,
                                    FindRecordsByNameProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов FindRecordsBySurname (тип 1-M)
    new OneToManyConnectionHandler <FindRecordsBySurnameRequest,
                                    RecordResponse,
                                    &AsyncService::RequestFindRecordsBySurname,
                                    FindRecordsBySurnameProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов FindRecordsByPatronymic (тип 1-M)
    new OneToManyConnectionHandler <FindRecordsByPatronymicRequest,
                                    RecordResponse,
                                    &AsyncService::RequestFindRecordsByPatronymic,
                                    FindRecordsByPatronymicProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов FindRecordByNumber (тип 1-1)
    new OneToOneConnectionHandler <FindRecordByNumberRequest,
                                   RecordResponse,
                                   &AsyncService::RequestFindRecordByNumber,
                                   FindRecordByNumberProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Создаём первый handler для обработок запросов FindRecordsByNote (тип 1-M)
    new OneToManyConnectionHandler <FindRecordsByNoteRequest,
                                    RecordResponse,
                                    &AsyncService::RequestFindRecordsByNote,
                                    FindRecordsByNoteProcessingFunction>(&service_, handlers_queue_.get(), server_status_, database_);

    // Информируем в консоль об успешном создании первых handler'ов для обработки всех типов соединений
    cout << "[The first handlers were created for each connection type]"s << endl;
}

// Функция циклического итерирования по очереди handler'ов (основной цикл сервера)
void PhoneBookServer::HandlerQueueLoop() {
    // Информируем в консоль о запуске основного цикла сервера
    cout << "[The main server loop has started]"s << endl;

    // Указатель (tag) на handler для итерирования по очереди (выступает в роли итератора)
    void* handler_iterator_tag;

    // Вспомогательный флаг для указателя (tag'а) на handler
    bool event_ok;
        
    // Итерируемся по handler'ам в очереди запросов в бесконечном цикле
    while (handlers_queue_->Next(&handler_iterator_tag, &event_ok)) {

        // Вызываем у очереди handler'ов метод Next, который блокирует выполнение цикла до тех пор, пока в очереди handler'ов
        // не произойдёт какое-либо событие, т.е. event (например, от клиента пришёл запрос определённого типа, необходимо, чтобы
        // handler соответствующего типа, стоявший ранее на прослушивании порта, создал себе handler для прослушивания порта на
        // замену, а сам перешёл в статус PROCESSING и начал обрабатывать соединение с клиентом).
        //
        // Как только событие происходит, метод Next записывает в handler_iterator_tag void*-указатель на handler, для которого
        // надо совершить действие. При возникновении event'а метод Next возвращает true, таким образом мы итерируемся по очереди
        // handler'ов при возникновении event'ов. При вызове метода Shutdown для остановки сервера, метод Next увидит event с
        // остановкой работы очереди handler'ов и вернёт false, тогда мы выйдем из цикла.
        //
        // Также метод Next получает указатель на булево значение event_ok, куда записывает false, если у клиента на
        // writer'е request'а был вызван метод Finish, а иначе true. Такой функционал нужен для поддержки соединений
        // типа M-1 и M-M, которые не используются в сервере для телефонной книги.
        
        // После того, как метод Next отработал и разблокировал дальнейшее выполнение цикла, записав в handler_iterator_tag
        // void*-указатель на hander, для которого произошёл event, необходимо вызвать у этого handler'а функцию обработки.
        //
        // Upcats'им указатель на этот handler до указателя на базовый класс BaseConnectionHandler и вызываем у него метод
        // обработки Proceed. Благодаря механизму динамического полиморфизма будет вызван метод наследника (классов
        // OneToOneConnectionHandler или OneToManyConnectionHandler с подставленными параметрами шаблона), т.е. функция
        // обработки для handler'а конкретного типа соединения
        static_cast<BaseConnectionHandler*>(handler_iterator_tag)->Proceed();
    }

    // Если мы вышли из цикла, значит метод Next вернул false, а это значит, что сервер был остановлен
    
    // Информируем в консоль о том, что сервер был остановлен
    cout << "[The server has been shutdowned]"s << endl;
}

}