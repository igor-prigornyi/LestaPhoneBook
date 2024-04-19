// Заголовочный файл phone_book_server.h описывает работу сервера для телефонной книги

// Header guard (предотвращает повторное включение заголовочного файла)
#pragma once

// Подключим библиотеку iostream для работы стандартного потока вывода в консоль для отображения статуса
// работы сервера, библиотеку memory работы умных указателей, библиотеку string для работы со строками,
// библиотеку vector и для использования контейнера вектора, а также библиотеку thread для работы с потоками
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

// Подключим заголовочные файлы с объявлениями функционала gRPC
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

// Подключим заголовочный файл "connection.grpc.pb.h", сгенерированный из "connection.proto", который содержит 
// объявление gRPC-сервиса связи между клиентом и сервером телефонной книги. Также он внутри себя подключает
// заголовочный файл "connection.pb.h", аналогично сгенерированный из "connection.proto", который содержит
// объявления типов запросов (request'ов) и ответов (rsponse'ов) для gRPC-сервиса
#include "connection.grpc.pb.h"

// Подключим заголовочный файл базы данных для телефонной книги
#include "phone_book_database.h"

// Не будем использовать using-директивы в глобальной области видимости заголовочного файла, так как это
// приведёт к попаданию этих using-директив во все области видимости, куда будет включён заголовочный файл

// Пространство имён сервера для телефонной книги
namespace phone_book_server {

// Внутри пространства имён сервера для телефонной книги воспользуемся using-директивами
// (без подключения других пространств имён, иначе при подключении using-директивой пространства phone_book_server
// эти пространства имён тоже будут включены во все области видимости, куда будет подключено пространство phone_book_server)

// Будем использовать инструменты gRPC без префикса "grpc::"
using grpc::Server;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

// Будем использовать gRPC-сервис связи между клиентом и сервером телефонной книги,
// сгенерированный из proto-файла (объявлен в connection.grpc.pb.h) без префикса "phone_book_proto::"
using phone_book_proto::PhoneBookConnection;

// Будем использовать псевдоним "AsyncService" для PhoneBookConnection::AsyncService
using AsyncService = PhoneBookConnection::AsyncService;

// Будем использовать типы запросов (request'ы) и ответов (rsponse'ы),
// сгенерированные из proto-файла (объявлены в connection.pb.h) без префикса "phone_book_proto::"
using phone_book_proto::RecordRequest;
using phone_book_proto::RecordResponse;
using phone_book_proto::AddRecordResponse;
using phone_book_proto::DeleteRecordResponse;
using phone_book_proto::DeleteRecordByIdRequest;
using phone_book_proto::DeleteRecordByNumberRequest;
using phone_book_proto::FindRecordByIdRequest;
using phone_book_proto::FindRecordsByNameRequest;
using phone_book_proto::FindRecordsBySurnameRequest;
using phone_book_proto::FindRecordsByPatronymicRequest;
using phone_book_proto::FindRecordsByPatronymicRequest;
using phone_book_proto::FindRecordByNumberRequest;
using phone_book_proto::FindRecordsByNoteRequest;

// Будем использовать класс базы данных для телефонной книги без префикса "phone_book_database::"
using phone_book_database::PhoneBookDatabase;

// Архитектура кода сервера:
//
// Наш gRPC-сервис связи между клиентом и сервером телефонной книги использует два типа соединений:
//
// 1) Соединение типа 1-1 (one to one) для стандартных случаев
//    (например, в результате поиска по номеру телефона может найтись не более одной записи в телефонной книге):
//
//    rpc ИМЯ_СОЕДИНЕНИЯ (ТИП_ЗАПРОСА) returns (ТИП_ОТВЕТА);
//
// 2) Соединение типа 1-M (one to many) для случаев, когда необходимо отправить клиенту массив данных
//    (например, в результате поиска по фамилии в телефонной книге нашлось несколько сотен однофамильцев с разными
//    телефонными номерами, необходимо отправить все эти несколько сотен записей):
//
//    rpc ИМЯ_СОЕДИНЕНИЯ (ТИП_ЗАПРОСА) returns (stream ТИП_ОТВЕТА);
//
// Соединения типа M-1 (many to one) и M-M (many to many) не нужны для функционала нашего сервера телефонной книги.
//
// Для обработки этих двух типов соединений реализуем базовый абстрактный класс BaseConnectionHandler, в
// котором будет чисто виртуальная функция Proceed, имплементация которой будет реализована в двух наследниках
// класса - OneToOneConnectionHandler, обрабатывающем соединения типа 1-1, и OneToManyConnectionHandler,
// обрабатывающем соединения типа 1-M. Далее будем называть такие классы handler'ами.
//
// Оба эти класса будут шаблонные. В качестве шаблонных параметров они будут принимать тип запроса,
// тип ответа, указатель на функцию из функционала gRPC для инициализации соединения (регистрации
// handler'а в очереди handler'ов) и указатель на функцию, обрабатывающую соединение (эта функция будет
// обрабатывать входящий запрос и формировать ответ, содержа в себе логику формирования ответа, обращаясь
// к базе данных телефонной книги).
//
// Будем реализовывать асинхронный подход, для этого сервер будет хранить очередь handler'ов. Технически это очередь
// из void*-указателей на объекты классов OneToOneConnectionHandler и OneToManyConnectionHandler с подставленными
// шаблонными параметрами, которые будут находиться в динамической памяти (heap'е). Каждый handler будет иметь поле
// статуса, отражающее его состояние. Жизненный цикл handler'а, соответствующего определённому типу соединения, таков:
//
// 1) Вначале handler создаётся, имея статус CREATED. Затем он получает статус LISTENING и становится на прослушивание
//    порта в ожидании появления входящего запроса.
//
// 2) В какой-то момент придёт запрос, типу которого соответствует наш handler. Тогда он создаст в heap'е handler
//    своего же типа со статусом CREATED и поставит его в очередь на прослушивание порта со статусом LISTENING.
//    Наш же handler перейдёт в статус PROCESSING и займётся обработкой поступившего запроса.
//
// 3) Перейдя в статус PROCESSING, handler для соединения типа 1-1 сразу обработает пришедший запрос с помощью функции,
//    указанной в параметре шаблона, сформирует при помощи неё ответ и отправит его клиенту, а затем перейдёт в
//    статус FINISHED. А вот handler для соединения типа 1-M, перейдя в статус PROCESSING, сформирует ответ на запрос
//    в виде вектора ответов (response'ов), которые нужно будет отправить по одному, находясь в статусе PROCESSING.
//    Только после полной отправки ответа (отправки последнего элемента вектора) такой handler перейдёт в статус
//    FINISHED.
//
// 4) Перейдя в статус FINISHED, handler вызывает необходимые процедуры для закрытия соединения, а затем удаляется
//    из динамической памяти (heap'а).
//
// Вначале создадим в heap'е по одному handler'у для каждого типа соединения и зарегистрируем их в очереди, поставив
// на прослушивание порта, на котором будет работать сервер. Затем запустим цикл, в котором будем по кругу итерироваться
// по очереди handler'ов, проверяя, не произошло ли какое-либо событие, меняющее статус очередного handler'а - в таком
// случае upcats'им void*-указатель на него до указателя на базовый класс BaseConnectionHandler и вызываем функцию
// Proceed. Благодаря механизму динамического полиморфизма будет вызвана функция наследника, соответствующая стратегии
// обработки соединения определённого типа. Именно в функции Proceed и будет реализован жизненный цикл handler'а
// 1)->2)->3)->4), описанный выше.
//
// Теоретически также можно реализовать и многопоточный подход, когда обработкой handler'ов из очереди будут заниматься
// несколько потоков из Thread Pool'а. Можно, например, для каждого типа соединения выделить отдельный Thread Pool.
// Возможен и более простой вариант с запуском на отдельном потоке лишь функции обработки и формирования ответа, которая
// непосредственно обращается к базе данных телефонной книги, тогда можно воспользоваться функционалом std::async.
// В любой из подобных реализаций следует подумать о механизме синхронизации через join'ы или другие миханизмы, а также
// об ограждении участков работы с контейнерами в базе данных телефонной книги mutex'ами, чтобы избежать состояния гонки.
//
// Идея и пример асинхронной реализаций с разными типами соединений взяты из статьи: https://habr.com/ru/articles/340758/

// Пространство имён функций, обрабатывающих соединения. Эти функции обрабатывают входящие запросы
// (reqest'ы) и формируют ответы (response'ы), содержа в себе логику формирования ответа, обращаясь
// к базе данных телефонной книги
//
// Определения (definition'ы) этих функций находятся в phone_book_server.cpp
namespace connection_processing_functions {

// Функция обработки запроса на добавление записи (тип 1-1)
// (клиент получает код ответа: 0 - запись с таким номером телефона уже существует,
//                              1 - запись успешно добавлена)
void AddRecordProcessingFunction(PhoneBookDatabase&, RecordRequest*, AddRecordResponse*, const void*);

// Функция обработки запроса на удаление записи по номеру записи (тип 1-1)
// (клиент получает код ответа: 0 - записи с таким номером/id не существует,
//                              1 - запись успешно удалена)
void DeleteRecordByIdProcessingFunction(PhoneBookDatabase&, DeleteRecordByIdRequest*, DeleteRecordResponse*, const void*);

// Функция обработки запроса на удаление записи по номеру телефона (тип 1-1)
// (клиент получает код ответа: 0 - записи с таким номером телефона не существует,
//                              1 - запись успешно удалена)
void DeleteRecordByNumberProcessingFunction(PhoneBookDatabase&, DeleteRecordByNumberRequest*, DeleteRecordResponse*, const void*);

// Функция обработки запроса на поиск записи по номеру/id записи (тип 1-1)
// (найденная запись может быть только одна или её может не быть вовсе, тогда формируем пустой ответ с id = 0)
void FindRecordByIdProcessingFunction(PhoneBookDatabase&, FindRecordByIdRequest*, RecordResponse*, const void*);

// Функция обработки запроса на поиск записей по имени (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsByNameProcessingFunction(PhoneBookDatabase&, FindRecordsByNameRequest*, std::vector<RecordResponse>*, const void*);

// Функция обработки запроса на поиск записей по фамилии (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsBySurnameProcessingFunction(PhoneBookDatabase&, FindRecordsBySurnameRequest*, std::vector<RecordResponse>*, const void*);

// Функция обработки запроса на поиск записей по отчеству (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsByPatronymicProcessingFunction(PhoneBookDatabase&, FindRecordsByPatronymicRequest*, std::vector<RecordResponse>*, const void*);

// Функция обработки запроса на поиск записи по номеру телефона (тип 1-1)
// (найденная запись может быть только одна или её может не быть вовсе, тогда формируем пустой ответ с id = 0)
void FindRecordByNumberProcessingFunction(PhoneBookDatabase&, FindRecordByNumberRequest*, RecordResponse*, const void*);

// Функция обработки запроса на поиск записей по заметке (тип 1-M)
// (найденных записей может быть множество или не быть вовсе, тогда формируем пустой вектор ответов)
void FindRecordsByNoteProcessingFunction(PhoneBookDatabase&, FindRecordsByNoteRequest*, std::vector<RecordResponse>*, const void*);

}

// Класс сервера для телефонной книги
class PhoneBookServer final {
private:
    std::string ip_; // IP-адрес сервера
    uint16_t port_;  // Порт для работы сервера

    // Умный указатель на объект gRPC-сервера 
    // (при вызове деструктора сервера для телефонной книги объект также будет уничтожен)
    std::unique_ptr<Server> server_;

    // Умный указатель на объект очереди handler'ов соединений
    // (при вызове деструктора сервера для телефонной книги объект также будет уничтожен)
    std::unique_ptr<ServerCompletionQueue> handlers_queue_;

    // Сервис асинхронной gRPC-коммуникации
    AsyncService service_;

    // Возможные статусы (состояния) сервера
    enum class ServerStatus {
        CREATED,       // Объект сервера был только что создан и ожидает вызова функции запуска
        RUNNING,       // Сервер работает
        SHUTTING_DOWN, // Сервер останавливается
        SHUTDOWNED     // Сервер остановлен
    }; 

    // Статус сервера
    ServerStatus server_status_;

    // Указатель на поток выполнения сервера
    std::thread* server_thread_;

    // Неконстантая ссылка на базу данных телефонной книги
    PhoneBookDatabase& database_;

public:
    // Конструктор сервера принимает IP-адрес сервера, порт для работы сервера и неконстантную ссылку на базу
    // данных телефонной книги, инициализирует nullptr'ом указатель на поток выполнения сервера и 
    // устанавливает статус сервера в CREATED
    //
    // (определение/definition этой функции находится в phone_book_server.cpp)
    explicit PhoneBookServer(const std::string& ip, uint16_t port, PhoneBookDatabase& database);

    // Деструктор сервера
    // (определение/definition этой функции находится в phone_book_server.cpp)
    ~PhoneBookServer();

    // Функция запуска сервера в отдельном потоке
    // (определение/definition этой функции находится в phone_book_server.cpp)
    void RunInNewThread();

    // Функция остановки сервера
    // (определение/definition этой функции находится в phone_book_server.cpp)
    void Shutdown();

private:
    // Функция инициализации очереди handler'ов
    // (определение/definition этой функции находится в cpp-файле)
    void HandlerQueueInitialization();

    // Функция циклического итерирования по очереди handler'ов (основной цикл сервера)
    // (определение/definition этой функции находится в cpp-файле)
    void HandlerQueueLoop();

    // Базовый абстрактный класс handler'а соединения
    class BaseConnectionHandler {
    public:
        // Конструктор принимает сырые указатели на сервис асинхронной gRPC-коммуникации и очередь handler'ов,
        // а также константную ссылку на статус сервера и неконстантную ссылку на базу данных телефонной книги
        // и создаёт handler со статусом CREATED
        BaseConnectionHandler(AsyncService* service,
                              ServerCompletionQueue* handlers_queue,
                              const ServerStatus& server_status,
                              PhoneBookDatabase& database) : service_(service),
                                                             handlers_queue_(handlers_queue),
                                                             server_status_(server_status),
                                                             database_(database),
                                                             status_(ConnectionStatus::CREATED) { }

        // Публичный виртуальный деструктор необходим для корректного удаления наследников
        virtual ~BaseConnectionHandler() { }

        // Чисто виртуальная функция обработки запроса handler'ом, её имплементацию требуется определить в наследниках
        virtual void Proceed() = 0;

    protected:
        // Возможные статусы (состояния) handler'а
        enum class ConnectionStatus {
            CREATED,    // Подключение создано
            LISTENING,  // В ожидании входящего запроса
            PROCESSING, // Запрос обрабатывается
            FINISHED,   // Запрос обработан
            ABORTED     // Подключение прервано
        };

        AsyncService* service_;                 // Сырой указатель на сервис асинхронной gRPC-коммуникации
        ServerCompletionQueue* handlers_queue_; // Сырой указатель на очередь handler'ов
        ServerContext ctx_;                     // Параметры соединения
        ConnectionStatus status_;               // Статус handler'а

        const ServerStatus& server_status_;     // Константная ссылка на статус сервера
        PhoneBookDatabase& database_;           // Неконстантная сылка на базу данных телефонной книги
    };

    // Класс handler'а соединения типа 1-1, наследуется от базового класса handler'а
    // Параметры шаблона: тип запроса (request'а), тип ответа (response'а), указатель на функцию инициализации соединения,
    // указатель на функцию обработки соединения типа 1-1
    //
    // (поскольку класс шаблонный, поместим definition'ы его методов прямо в header-файле, иначе возникнет ошибка при
    // инстанцировании с определёнными шаблонными параметрами, что приведёт к ошибкам на этапе линкови - definition'ы
    // функций с нужными подставленными шаблонными параметрами не будут найдены ни в одной единице трансляции)

    template <typename RequestType,   // Тип запроса (request'а)
              typename ResponseType,  // Тип ответа  (response'а)
              auto ConnectionRegistrationFunction, // Указатель на функцию инициализации соединения из функционала gRPC
                                                   // (эта функция осуществляет регистрацию handler'а в очереди handler'ов)
              auto ConnectionProcessingFunction>   // Указатель на функцию обработки соединения
                                                   // (эта функция осуществляет обработку входящего запроса и формирует ответ,
                                                   // содержа в себе логику формирования ответа, обращаясь к базе данных
                                                   // телефонной книги)

    class OneToOneConnectionHandler : public BaseConnectionHandler {
    private:
        RequestType  request_;  // Запрос (вместо RequestType  будет подставлен тип запроса)
        ResponseType response_; // Ответ  (вместо ResponseType будет подставлен тип ответа )

        ServerAsyncResponseWriter<ResponseType> responder_; // Асинхронный респондер для соединения типа 1-1

    public:
        // Конструктор принимает сырые указатели на сервис асинхронной gRPC-коммуникации и очередь handler'ов, а также
        // константную ссылку на статус сервера и неконстантную ссылку на базу данных телефонной книги. Конструктор вызывает
        // конструктор базового класса, который создаёт hanlder со статусом CREATED, затем конструктор связывает асинхронный
        // респондер и параметры соединения (в нашем случае дефолтные), затем конструктор вызывает функцию обработки запроса
        // handler'ом
        OneToOneConnectionHandler(AsyncService* service,
                                  ServerCompletionQueue* handlers_queue,
                                  const ServerStatus& server_status,
                                  PhoneBookDatabase& database) : BaseConnectionHandler(service,
                                                                                       handlers_queue,
                                                                                       server_status,
                                                                                       database),
                                                                 responder_(&ctx_) {

            // В результате первого вызова функции обработки запроса handler'ом, наш handler получит статус LISTENING, будет
            // добавлен в очередь handler'ов и поставлен на прослушивание порта в ожидании появления входящего соединения,
            // соответствующему типу handler'a
            Proceed();
        }

        // Имплементация функции обработки запроса handler'ом для соединения типа 1-1
        virtual void Proceed() override {
            // Для удобства подключим внутри функции пространство имён std
            using namespace std;

            // Если сервер находится не в состоянии RUNNING, а handler в состоянии CREATED или LISTENING, т.е. сервер
            // находится в  процессе остановки, а handler в состоянии ожидания запроса, то переводим handler в статус
            // ABORTED и завершаем обработку соединения, дабы высвободить все возможные ресурсы при остановке сервера
            if (server_status_ != ServerStatus::RUNNING && (status_ == ConnectionStatus::CREATED ||
                                                            status_ == ConnectionStatus::LISTENING)) {
                status_ = ConnectionStatus::ABORTED;

                // Удаляем handler из heap'а и завершаем обработку
                delete this; return;
            }

            // Замечание: в теории, те запросы, которые успели поступить в обработку ещё до отключения сервера (когда таковое
            // произойдёт), будут иметь статус PROCESSING и будут дообработаны, а ответы будут отправлены клиентам. Действительно
            // ли так происходит на практике, выяснить не удалось, так как обмен информацией идёт настолько быстро, что не удётся
            // подловить момент, когда запрос только поступил, а ответ ещё не сформирован, чтобы в этот момент попытаться
            // смоделировать ситуацию остановки сервера. Возможно, стоит попробовать на большом объёме данных с очень большим
            // запросом.

            // Если handler только что создан и имеет статус CREATED
            if (status_ == ConnectionStatus::CREATED) {

                // Информируем в консоль о создании нового handler'а для соединения типа 1-1
                cout << "[1-1 handler #"s << this << "]: New handler for 1-1 connection"s << endl;
                
                // Регистрируем handler в очереди handler'ов с помощью переданной по указателю в параметрах шаблона функции
                // из функционала gRPC, передавая ей в качестве уникального идентификатора handler'а void*-указатель на него
                (service_->*ConnectionRegistrationFunction)(&ctx_, &request_, &responder_, handlers_queue_, handlers_queue_, this);

                // Переводим handler в статус LISTENING, обработка соединения handler'ом начнётся, когда до этого handler'а
                // дойдёт очередь на следующем проходе по очереди handler'ов в функции HandlerQueueLoop (функция циклического
                // итерирования по очереди handler'ов)
                status_ = ConnectionStatus::LISTENING;
            }
            // Если handler в процессе обработки соединения и имеет статус LISTENING
            else if (status_ == ConnectionStatus::LISTENING) {

                // Информируем в консоль о том, что наш handler увидел входящий запрос, соответствующего ему типа, открыл
                // соединение и принял этот запрос
                cout << "[1-1 handler #"s << this << "]: Handler has opened 1-1 connection and received the request ("s << sizeof(request_) << " bytes)"s << endl;

                // Переводим наш handler в статус PROCESSING
                status_ = ConnectionStatus::PROCESSING;

                // Создаём в heap'е handler такого же типа (он сразу же в вызове конструктора будет зарегистрирован в очереди
                // handler'ов и поставлен на прослушивание порта, получив вначале статус CREATED, а затем LISTENING), который
                // сменит наш handler на посту и будет находиться в ожидании появления нового входящего соединения,
                // соответствующему типу нашего handler'a. Наш же handler далее займётся обработкой текущего соедиения
                new OneToOneConnectionHandler<RequestType,
                                              ResponseType,
                                              ConnectionRegistrationFunction,
                                              ConnectionProcessingFunction>(service_,
                                                                            handlers_queue_,
                                                                            server_status_,
                                                                            database_);

                // Вызываем функцию обработки соединения, переданную в параметрах шаблона. Эта функция будет осуществлять
                // обработку входящего запроса и формировать ответ, обращаясь к базе данных телефонной книги
                ConnectionProcessingFunction(database_, &request_, &response_, this);

                // Информируем в консоль о том, что наш handler сформировал ответ и начал его отправку клиенту
                cout << "[1-1 handler #"s << this << "]: Sending response ... ("s << sizeof(response_) << " bytes)"s << endl;
                
                // Отправляем сформированный ответ клиенту и снимаем responder нашего handler'а с соединения, закрывая его
                responder_.Finish(response_, Status::OK, this);

                // Замечание: здесь можно выдать exception в случае неуспешной отправки ответа клиенту. При выдачи exception'а
                // начнётся раскрутка stack'а до ближайшего catch'а, куда будет передана информация о выданном exception'е. В
                // таком случае стоит продумать механизм разрыва текущего соединения и уведомления очереди handler'ов о том, что
                // наш текущий handler погиб в неравном бою под Химмельсдорфом. Возможно, в самом классе ServerCompletionQueue
                // реализован какой-либо механизм, позволяющий понять, что void*-указатель, указывающий на удалённый handler,
                // инвалидировался. Иначе стоит продумать механизм уведомления очереди handler'ов о том, что текущий handler
                // вышел из боя.
                //
                // Также необходимо будет реализовать купирование подобной ситуации и на стороне клиента, иначе его приложение
                // рискует упасть. Возможно, фреймворк gRPC сам генерирует в таком случае exception'ы и/или купирует подобные
                // ситуации. Также стоит задуматься над нестандартными ситуациями и их купированием и в других местах, где ведётся
                // работа с соединением.
                
                // Информируем в консоль о том, что наш handler успешно отправил ответ клиенту
                cout << "[1-1 handler #"s << this << "]: Response has been sent ("s << sizeof(response_) << " bytes)"s << endl;

                // Переводим handler в статус FINISHED
                status_ = ConnectionStatus::FINISHED;
            }
            // В остальных случаях handler завершил работу и имеет статус FINISHED
            else {

                // Проверяем, что handler действительно имеет статус FINISHED
                GPR_ASSERT(status_ == ConnectionStatus::FINISHED);

                // Информируем в консоль о завершении работы handler'а для соединения типа 1-1
                cout << "[1-1 handler #"s << this << "]: Handler for 1-1 connection has finished"s << endl;

                // Удаляем handler из heap'а и завершаем обработку
                delete this; return;
            }
        }

        // Деструктор информирует в консоль об удалении handler'а из heap'а
        ~OneToOneConnectionHandler() {
            // Для удобства подключим внутри функции пространство имён std
            using namespace std;

            // Информируем в консоль об удалении handler'а из heap'а
            cout << "[1-1 handler #"s << this << "]: Handler for 1-1 connection was deleted from heap"s << endl;
        }
    };

    // Класс handler'а соединения типа 1-M, наследуется от базового класса handler'а
    // Параметры шаблона: тип запроса (request'а), тип ответа (response'а), указатель на функцию инициализации соединения,
    // указатель на функцию обработки соединения
    //
    // (поскольку класс шаблонный, поместим definition'ы его методов прямо в header-файле, иначе возникнет ошибка при
    // инстанцировании с определёнными шаблонными параметрами, что приведёт к ошибкам на этапе линкови - definition'ы
    // функций с нужными подставленными шаблонными параметрами не будут найдены ни в одной единице трансляции)

    template <typename RequestType,   // Тип запроса (request'а)
              typename ResponseType,  // Тип ответа  (response'а)
              auto ConnectionRegistrationFunction, // Указатель на функцию инициализации соединения из функционала gRPC
                                                   // (эта функция осуществляет регистрацию handler'а в очереди handler'ов)
              auto ConnectionProcessingFunction>   // Указатель на функцию обработки соединения типа 1-M
                                                   // (эта функция осуществляет обработку входящего запроса и формирует ответ,
                                                   // содержа в себе логику формирования ответа, обращаясь к базе данных
                                                   // телефонной книги)

    class OneToManyConnectionHandler : public BaseConnectionHandler{
    private:
        RequestType request_;                // Запрос         (вместо RequestType  будет подставлен тип запроса)
        std::vector<ResponseType> response_; // Вектор ответов (вместо ResponseType будет подставлен тип ответа )

        ServerAsyncWriter<ResponseType> responder_; // Асинхронный респондер для соединения типа 1-M

        size_t responder_counter_; // Счётчик для итерации по вектору ответов при отправке оного клиенту
        size_t bytes_counter_;     // Счётчик отправленных клиенту байт

    public:
        // Конструктор принимает сырые указатели на сервис асинхронной gRPC-коммуникации и очередь handler'ов, а также
        // константную ссылку на статус сервера и неконстантную ссылку на базу данных телефонной книги. Конструктор вызывает
        // конструктор базового класса, который создаёт hanlder со статусом CREATED, затем конструктор связывает асинхронный
        // респондер и параметры соединения (в нашем случае дефолтные), после чего конструктор устанавливает на ноль счётчик
        // итераций по вектору ответов при отправке оного клиенту, устанавливает на ноль счётчик отправленных клиенту байт и,
        // наконец, вызывает функцию обработки запроса handler'ом
        OneToManyConnectionHandler(AsyncService* service,
                                   ServerCompletionQueue* handlers_queue,
                                   const ServerStatus& server_status,
                                   PhoneBookDatabase& database) : BaseConnectionHandler(service,
                                                                                        handlers_queue,
                                                                                        server_status,
                                                                                        database),
                                                                  responder_(&ctx_),
                                                                  responder_counter_(0),
                                                                  bytes_counter_(0) {

            // В результате первого вызова функции обработки запроса handler'ом, наш handler получит статус LISTENING, будет
            // добавлен в очередь handler'ов и поставлен на прослушивание порта в ожидании появления входящего соединения,
            // соответствующему типу handler'a
            Proceed();
        }

        // Имплементация функции обработки запроса handler'ом для соединения типа 1-M
        virtual void Proceed() override {
            // Для удобства подключим внутри функции пространство имён std
            using namespace std;

            // Если сервер находится не в состоянии RUNNING, а handler в состоянии CREATED или LISTENING, т.е. сервер
            // находится в  процессе остановки, а handler в состоянии ожидания запроса, то переводим handler в статус
            // ABORTED и завершаем обработку соединения, дабы высвободить все возможные ресурсы при остановке сервера
            if (server_status_ != ServerStatus::RUNNING && (status_ == ConnectionStatus::CREATED ||
                                                            status_ == ConnectionStatus::LISTENING)) {
                status_ = ConnectionStatus::ABORTED;

                // Удаляем handler из heap'а и завершаем обработку
                delete this; return;
            }

            // Замечание: в теории, те запросы, которые успели поступить в обработку ещё до отключения сервера (когда таковое
            // произойдёт), будут иметь статус PROCESSING и будут дообработаны, а ответы будут отправлены клиентам. Действительно
            // ли так происходит на практике, выяснить не удалось, так как обмен информацией идёт настолько быстро, что не удётся
            // подловить момент, когда запрос только поступил, а ответ ещё не сформирован, чтобы в этот момент попытаться
            // смоделировать ситуацию остановки сервера. Возможно, стоит попробовать на большом объёме данных с очень большим
            // запросом.

            // Если handler только что создан и имеет статус CREATED
            if (status_ == ConnectionStatus::CREATED) {
                // Информируем в консоль о создании нового handler'а для соединения типа 1-M
                cout << "[1-M handler #"s << this << "]: New handler for 1-M connection"s << endl;

                // Регистрируем handler в очереди handler'ов с помощью переданной по указателю в параметрах шаблона функции
                // из функционала gRPC, передавая ей в качестве уникального идентификатора handler'а void*-указатель на него
                (service_->*ConnectionRegistrationFunction)(&ctx_, &request_, &responder_, handlers_queue_, handlers_queue_, this);

                // Переводим handler в статус LISTENING, обработка соединения handler'ом начнётся, когда до этого handler'а
                // дойдёт очередь на следующем проходе по очереди handler'ов в функции HandlerQueueLoop (функция циклического
                // итерирования по очереди handler'ов)
                status_ = ConnectionStatus::LISTENING;
            }
            // Если handler в процессе обработки соединения и имеет статус LISTENING или PROCESSING
            else if (status_ == ConnectionStatus::LISTENING || status_ == ConnectionStatus::PROCESSING) {

                // Если наш handler имеет статус LISTENING, значит наш handler только начал обрабатывать текущее соединение.
                // Необходимо создать в heap'е handler такого же типа, который сменит наш handler на посту и будет находиться
                // в ожидании появления нового входящего соединения, соответствующему типу нашего handler'a, пока наш handler
                // будет обрабатывать текущее соединение
                if(status_ == ConnectionStatus::LISTENING) {

                    // Информируем в консоль о том, что наш handler увидел входящий запрос, соответствующего ему типа, открыл
                    // соединение и принял этот запрос
                    cout << "[1-M handler #"s << this << "]: Handler has opened 1-M connection and received the request ("s << sizeof(request_) << " bytes)"s << endl;

                    // Переводим наш handler в статус PROCESSING
                    status_ = ConnectionStatus::PROCESSING;

                    // Создаём в heap'е handler такого же типа (он сразу же в вызове конструктора будет зарегистрирован в очереди
                    // handler'ов и поставлен на прослушивание порта, получив вначале статус CREATED, а затем LISTENING), который
                    // сменит наш handler на посту и будет находиться в ожидании появления нового входящего соединения,
                    // соответствующему типу нашего handler'a. Наш же handler далее займётся обработкой текущего соедиения
                    new OneToManyConnectionHandler<RequestType,
                                                   ResponseType,
                                                   ConnectionRegistrationFunction,
                                                   ConnectionProcessingFunction>(service_,
                                                                                 handlers_queue_,
                                                                                 server_status_,
                                                                                 database_);

                    // После перевода нашего handler'а в статус PROCESSING мы уже не попадём в этот блок, поэтому handler на замену
                    // будет создан лишь однократно

                    // Вызываем функцию обработки соединения, переданную в параметрах шаблона. Эта функция будет осуществлять
                    // обработку входящего запроса и, обращаясь к базе данных телефонной книги, формировать вектор ответов,
                    // элементы которого будут последовательно отправляться клиенту
                    ConnectionProcessingFunction(database_, &request_, &response_, this);

                    // Так как статус нашего handler'а переведён в PROCESSING, и мы уже не попадём в этот блок, функция
                    // обработки соединения, формирующая вектор ответов, будет вызвана лишь однократно
                }

                // После того, как вектор ответов сформирован, необходимо последовательно отправить все его элементы клиенту.
                // Будем отправлять очередной элемент каждый раз, когда до нашего handler'а будет доходить очередь на очередном
                // проходе по очереди handler'ов в функции HandlerQueueLoop (функция циклического итерирования по очереди handler'ов).
                // Для этого будем использовать счётчик отправок, который при создании handler'а инициализируется нулём, а при каждой
                // отправке очередного элемента вектора ответов клиенту будет инкрементироваться до тех пор, пока не дойдёт до размера
                // вектора ответов, после чего отправка завершится.
                
                // Если счётчик отправок меньше размера вектора ответов, необходимо отправить клиенту очередной элемент вектора ответов
                if(responder_counter_ < response_.size()) {

                    // Информируем в консоль о том, что наш handler отправляет клиенту очередной элемент вектора ответов
                    cout << "[1-M handler #"s << this << "]: Sending response (part "s << responder_counter_ + 1 << "/"s << response_.size() << ")"s << endl;

                    // Отправляем очередной элемент вектора ответов клиенту
                    responder_.Write(response_[responder_counter_], this);

                    // Замечание: здесь можно выдать exception в случае неуспешной отправки очередного элемента вектора ответов клиенту

                    // Инкрементируем счётчик счётчик отправок
                    ++responder_counter_;

                    // Добавляем число отправленных байт к счётчику байт
                    bytes_counter_ += sizeof(response_[responder_counter_]);
                }
                // Иначе счётчик отправок достиг размера вектора ответов, необходимо завершить отправку клиенту вектора ответов
                else {

                    // Информируем в консоль о том, что наш handler успешно завершил отправку клиенту вектора ответов
                    cout << "[1-M handler #"s << this << "]: Response has been fully sent ("s << bytes_counter_ << " bytes)"s << endl;

                    // Cнимаем responder нашего handler'а с соединения, закрывая текущее соединение
                    responder_.Finish(Status(), this);

                    // Замечание: здесь можно выдать exception в случае неуспешной попытки закрыть соединение

                    // Переводим handler в статус FINISHED
                    status_ = ConnectionStatus::FINISHED;
                }
            }
            // В остальных случаях handler завершил работу и имеет статус FINISHED
            else {

                // Проверяем, что handler действительно имеет статус FINISHED
                GPR_ASSERT(status_ == ConnectionStatus::FINISHED);

                // Информируем в консоль о завершении работы handler'а для соединения типа 1-M
                cout << "[1-M handler #"s << this << "]: Handler for 1-M connection has finished"s << endl;

                // Удаляем handler из heap'а и завершаем обработку
                delete this; return;
            }
        }

        // Деструктор информирует в консоль об удалении handler'а из heap'а
        ~OneToManyConnectionHandler() {
            // Для удобства подключим внутри функции пространство имён std
            using namespace std;

            // Информируем в консоль об удалении handler'а из heap'а
            cout << "[1-M handler #"s << this << "]: Handler for 1-M connection was deleted from heap"s << endl;
        }
    };
};

}