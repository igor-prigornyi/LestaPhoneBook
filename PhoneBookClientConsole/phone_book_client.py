# Модуль phone_book_client описывает работу клиента для телефонной книги

# Подключим модуль gRPC
import grpc

# Подключим модуль connection_pb2_grpc, сгенерированный из connection.proto, который содержит функционал
# gRPC-сервиса связи между клиентом и сервером телефонной книги
import connection_pb2_grpc

# Подключим модуль connection_pb2, сгенерированный из connection.prot, который содержит описание типов
# запросов (request'ов) и ответов (rsponse'ов) для gRPC-сервиса
import connection_pb2

# Функция запроса на добавление записи (тип 1-1)
# (возвращает код ответа: 0 - запись с таким номером телефона уже существует,
#                         1 - запись успешно добавлена)
def AddRecord(adress, name, surname, patronymic, number, note):
    print('[AddRecord] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.RecordRequest(name=name, surname=surname, patronymic=patronymic, number=number, note=note)
        response = stub.AddRecord(request)

        # Возвращаем полученный код ответа на запрос
        return response.code

# Функция запроса на удаление записи по номеру/id записи (тип 1-1)
# (возвращает код ответа: 0 - записи с таким номером/id не существует,
#                         1 - запись успешно удалена)
def DeleteRecordById(adress, id):
    print('[DeleteRecordById] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.DeleteRecordByIdRequest(id=id)
        response = stub.DeleteRecordById(request)

        # Возвращаем полученный код ответа на запрос
        return response.code

# Функция запроса на удаление записи по номеру телефона (тип 1-1)
# (возвращает код ответа: 0 - записи с таким номером телефона не существует,
#                         1 - запись успешно удалена)
def DeleteRecordByNumber(adress, number):
    print('[DeleteRecordByNumber] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.DeleteRecordByNumberRequest(number=number)
        response = stub.DeleteRecordByNumber(request)

        # Возвращаем полученный код ответа на запрос
        return response.code

# Функция запроса на поиск записи по номеру/id записи (тип 1-1)
# (найденная запись может быть только одна или её может не быть вовсе, тогда возвращается None)
def FindRecordById(adress, id):
    print('[FindRecordById] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.FindRecordByIdRequest(id=id)
        response = stub.FindRecordById(request)

        # Если пришла пустая запись с id = 0, значит записи с таким номером/id не найдено, возвращаем None
        if response.id == 0: return None
        # А иначе запаковываем результаты в кортеж и возвращаем
        else: return (response.id, response.name, response.surname, response.patronymic, response.number, response.note)

# Функция запроса на поиск записей по имени (тип 1-M)
# (найденных записей может быть множество или не быть вовсе, тогда возвращается None)
def FindRecordsByName(adress, name):
    print('[FindRecordsByName] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.FindRecordsByNameRequest(name=name)
        response = stub.FindRecordsByName(request)

        # Запаковываем результаты в вектор кортежей
        result = []
        for record in response:
            result.append((record.id, record.name, record.surname, record.patronymic, record.number, record.note))

        # Если получился пустой вектор кортежей, значит записей с таким именем не найдено, возвращаем None
        if len(result) == 0: return None
        # А иначе возвращаем вектор кортежей
        else: return result

# Функция запроса на поиск записей по фамилии (тип 1-M)
# (найденных записей может быть множество или не быть вовсе, тогда возвращается None)
def FindRecordsBySurname(adress, surname):
    print('[FindRecordsBySurname] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.FindRecordsBySurnameRequest(surname=surname)
        response = stub.FindRecordsBySurname(request)

        # Запаковываем результаты в вектор кортежей
        result = []
        for record in response:
            result.append((record.id, record.name, record.surname, record.patronymic, record.number, record.note))

        # Если получился пустой вектор кортежей, значит записей с такой фамилией не найдено, возвращаем None
        if len(result) == 0: return None
        # А иначе возвращаем вектор кортежей
        else: return result

# Функция запроса на поиск записей по отчеству (тип 1-M)
# (найденных записей может быть множество или не быть вовсе, тогда возвращается None)
def FindRecordsByPatronymic(adress, patronymic):
    print('[FindRecordsByPatronymic] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.FindRecordsByPatronymicRequest(patronymic=patronymic)
        response = stub.FindRecordsByPatronymic(request)

        # Запаковываем результаты в вектор кортежей
        result = []
        for record in response:
            result.append((record.id, record.name, record.surname, record.patronymic, record.number, record.note))

        # Если получился пустой вектор кортежей, значит записей с таким отчеством не найдено, возвращаем None
        if len(result) == 0: return None
        # А иначе возвращаем вектор кортежей
        else: return result

# Функция запроса на поиск записи по номеру телефона (тип 1-1)
# (найденная запись может быть только одна или её может не быть вовсе, тогда возвращается None)
def FindRecordByNumber(adress, number):
    print('[FindRecordByNumber] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.FindRecordByNumberRequest(number=number)
        response = stub.FindRecordByNumber(request)

        # Если пришла пустая запись с id = 0, значит записи с таким номером телефона не найдено, возвращаем None
        if response.id == 0: return None
        # А иначе запаковываем результаты в кортеж и возвращаем
        else: return (response.id, response.name, response.surname, response.patronymic, response.number, response.note)

# Функция запроса на поиск записей по заметке (тип 1-M)
# (найденных записей может быть множество или не быть вовсе, тогда возвращается None)
def FindRecordsByNote(adress, note):
    print('[FindRecordsByNote] ', end='')

    # Открываем соединение, отправляем запрос и получаем ответ
    with grpc.insecure_channel(adress) as channel:
        stub = connection_pb2_grpc.PhoneBookConnectionStub(channel)
        request = connection_pb2.FindRecordsByNoteRequest(note=note)
        response = stub.FindRecordsByNote(request)

        # Запаковываем результаты в вектор кортежей
        result = []
        for record in response:
            result.append((record.id, record.name, record.surname, record.patronymic, record.number, record.note))

        # Если получился пустой вектор кортежей, значит записей с указанными словами в заметке не найдено, возвращаем None
        if len(result) == 0: return None
        # А иначе возвращаем вектор кортежей
        else: return result

# Функция вывода в консоль ответа в виде кода
def PrintResponseCode(code):
    print(f'Response code: "{code}"')

# Функция вывода в консоль ответа в записи из телефонной книги
def PrintRecord(record):

    # Если пришёл None, то значит по запросу ничего не было найдено
    if record is None: print('Response: not found')
    # Иначе выводим найденный результат
    else:
        id, name, surname, patronymic, number, note = record
        print(f'Response: id="{id}", name="{name}", surname="{surname}", patronymic="{patronymic}", number="{number}", note="{note}"')

# Функция вывода в консоль ответа в нескольких записей из телефонной книги
def PrintRecords(records):

    # Если пришёл None, то значит по запросу ничего не было найдено
    if records is None: print('Response: not found')
    # Иначе выводим найденный результат
    else:
        print(f'Response ({len(records)} records):')
        
        record_counter = 0
        for record in records:
            record_counter += 1
            id, name, surname, patronymic, number, note = record
            print(f'#{record_counter} id="{id}", name="{name}", surname="{surname}", patronymic="{patronymic}", number="{number}", note="{note}"')