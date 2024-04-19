# Модуль gui_dialogs_handlers описывает методы взаимодействия GUI и
# модуля отправки клиентом запросов и получения ответов от сервера

# Подключим модули Tkinter для реализации GUI
import tkinter
import tkinter.ttk
import tkinter.messagebox

# Подключим модуль клиента для телефонной книги
from phone_book_client import *

# Функция очистки таблицы результатов поиска
def CleanTable(table):
    for item in table.get_children(): table.delete(item)

# Функция добавления записи в таблицу результатов поиска
def AddRecordInTable(table, record):
    table.insert('', tkinter.END, values=record)

# Функция запуска действия по добавлению новой записи
def AddRecordDialogHandler(dialog_window, adress, name, surname, patronymic, number, note):

    # Делаем запрос на добавление новой записи
    try: respose_code = AddRecord(adress, name, surname, patronymic,  number, note)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Информируем о результате запроса, исходя из кода ответа сервера
        if respose_code: tkinter.messagebox.showinfo   ('Информация', f'Запись успешно добавлена!', parent=dialog_window)
        else:            tkinter.messagebox.showwarning('Информация', f'Запись с таким номером телефона уже существует!', parent=dialog_window)

# Функция запуска действия по удалению записи по номеру/id
def DeleteRecordByIdDialogHandler(dialog_window, adress, id):

    # Пробуем преобразовать номер/id записи из типа str в тип int
    try: id = int(id)
    # В случае невалидного значения выдаём ошибку и прерываем действие
    except:
        tkinter.messagebox.showerror('Ошибка', f'Значение "{id}" не является валидным номером/id записи!', parent=dialog_window)
        return
    
    # Делаем запрос на удаление записи по номеру/id
    try: respose_code = DeleteRecordById(adress, id)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Информируем о результате запроса, исходя из кода ответа сервера
        if respose_code: tkinter.messagebox.showinfo   ('Информация', f'Запись успешно удалена!', parent=dialog_window)
        else:            tkinter.messagebox.showwarning('Информация', f'Записи с таким номером/id не существует!', parent=dialog_window)

# Функция запуска действия по удалению записи по номеру телефона
def DeleteRecordByNumberDialogHandler(dialog_window, adress, number):
    
    # Делаем запрос на удаление записи по номеру телефона
    try: respose_code = DeleteRecordByNumber(adress, number)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Информируем о результате запроса, исходя из кода ответа сервера
        if respose_code: tkinter.messagebox.showinfo   ('Информация', f'Запись успешно удалена!', parent = dialog_window)
        else:            tkinter.messagebox.showwarning('Информация', f'Записи с таким номером телефона не существует!', parent=dialog_window)

# Функция запуска действия по поиску записи по номеру/id
def FindRecordByIdDialogHandler(dialog_window, table, adress, id):

    # Пробуем преобразовать номер/id записи из типа str в тип int
    try: id = int(id)
    # В случае невалидного значения выдаём ошибку и прерываем действие
    except:
        tkinter.messagebox.showerror('Ошибка', f'Значение "{id}" не является валидным номером/id записи!', parent=dialog_window)
        return

    # Делаем запрос на поиск записи по номеру/id
    try: record = FindRecordById(adress, id)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Если записи с таким номером/id не нашлось
        if record is None:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Информируем, что записи с таким номером/id не нашлось
            tkinter.messagebox.showwarning('Информация', f'Записи с номером/id "{id}" не существует!', parent=dialog_window)

        # Если запись с таким номером/id нашлась
        else:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Добавляем найденную запись в таблицу результатов поиска
            AddRecordInTable(table, record)

            # Информируем, что запись с таким номером/id найдена
            tkinter.messagebox.showinfo('Информация', f'Запись с номером/id "{id}" найдена!', parent=dialog_window)

# Функция запуска действия по поиску записей по имени
def FindRecordByNameDialogHandler(dialog_window, table, adress, name):
    
    # Делаем запрос на поиск записей по имени
    try: records = FindRecordsByName(adress, name)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Если записей с таким именем не нашлось
        if records is None:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Информируем, что записей с таким именем не нашлось
            tkinter.messagebox.showwarning('Информация', f'Записей с именем "{name}" не найдено!', parent=dialog_window)

        # Если записи с таким именем нашлись
        else:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Добавляем найденные записи в таблицу результатов поиска
            for record in records:
                AddRecordInTable(table, record)

            # Информируем, что записи с таким именем найдены
            tkinter.messagebox.showinfo('Информация', f'Найдено {len(records)} записей с именем "{name}"!', parent=dialog_window)

# Функция запуска действия по поиску записей по фамилии
def FindRecordBySurnameDialogHandler(dialog_window, table, adress, surname):
    
    # Делаем запрос на поиск записей по фамилии
    try: records = FindRecordsBySurname(adress, surname)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Если записей с такой фамилией не нашлось
        if records is None:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Информируем, что записей с такой фамилией не нашлось
            tkinter.messagebox.showwarning('Информация', f'Записей с фамилией "{surname}" не найдено!', parent=dialog_window)

        # Если записи с такой фамилией нашлись
        else:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Добавляем найденные записи в таблицу результатов поиска
            for record in records:
                AddRecordInTable(table, record)

            # Информируем, что записи с такой фамилией найдены
            tkinter.messagebox.showinfo('Информация', f'Найдено {len(records)} записей с фамилией "{surname}"!', parent=dialog_window)

# Функция запуска действия по поиску записей по отчеству
def FindRecordByPatronymicDialogHandler(dialog_window, table, adress, patronymic):
    
    # Делаем запрос на поиск записей по отчеству
    try: records = FindRecordsByPatronymic(adress, patronymic)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Если записей с таким отчеством не нашлось
        if records is None:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Информируем, что записей с таким отчеством не нашлось
            tkinter.messagebox.showwarning('Информация', f'Записей с отчеством "{patronymic}" не найдено!', parent=dialog_window)

        # Если записи с таким отчеством нашлись
        else:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Добавляем найденные записи в таблицу результатов поиска
            for record in records:
                AddRecordInTable(table, record)

            # Информируем, что записи с таким отчеством найдены
            tkinter.messagebox.showinfo('Информация', f'Найдено {len(records)} записей с отчеством "{patronymic}"!', parent=dialog_window)

# Функция запуска действия по поиску записи по номеру телефона
def FindRecordByNumberDialogHandler(dialog_window, table, adress, number):
    
    # Делаем запрос на поиск записи по номеру телефона
    try: record = FindRecordByNumber(adress, number)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Если записи с таким номером телефона не нашлось
        if record is None:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Информируем, что записи с таким номером телефона не нашлось
            tkinter.messagebox.showwarning('Информация', f'Записи с номером телефона "{number}" не существует!', parent=dialog_window)

        # Если запись с таким номером телефона нашлась
        else:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Добавляем найденную запись в таблицу результатов поиска
            AddRecordInTable(table, record)

            # Информируем, что запись с таким номером телефона найдена
            tkinter.messagebox.showinfo('Информация', f'Запись с номером телефона "{number}" найдена!', parent=dialog_window)

# Функция запуска действия по поиску записей по заметке
def FindRecordByNoteDialogHandler(dialog_window, table, adress, note):
    
    # Делаем запрос на поиск записей по заметке
    try: records = FindRecordsByNote(adress, note)
    # В случае исключения (ошибка подключения к серверу) выдаём ошибку
    except: tkinter.messagebox.showerror('Ошибка', f'Не удалось отправить запрос! Возможно, сервер недоступен.', parent=dialog_window)
    # В случае успешного соединения выдаём результат запроса
    else:
        # Если записей с содержимым заметки по запросу не нашлось
        if records is None:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Информируем, что записей с содержимым заметки по запросу не нашлось
            tkinter.messagebox.showwarning('Информация', f'Записей с содержимым "{note}" в заметке не найдено!', parent=dialog_window)

        # Если записи с содержимым заметки по запросу нашлись
        else:
            # Очищаем таблицу результатов поиска
            CleanTable(table)

            # Добавляем найденные записи в таблицу результатов поиска
            for record in records:
                AddRecordInTable(table, record)

            # Информируем, что записей с содержимым заметки по запросу найдены
            tkinter.messagebox.showinfo('Информация', f'Найдено {len(records)} записей с содержимым "{note}" в заметке!', parent=dialog_window)