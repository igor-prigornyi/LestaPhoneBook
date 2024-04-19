# Модуль gui_dialogs описывает диалоги по добавлению, удалению и поиску записей

# Подключим модули Tkinter для реализации GUI
import tkinter
import tkinter.ttk
import tkinter.messagebox

# Подключим модуль с методами взаимодействия GUI и модулем
# отправки клиентом запросов и получения ответов от сервера
from gui_dialogs_handlers import *

# Диалог добавления новой записи
def AddRecordDialog(dialogs_manager):

    # Окно диалога
    AddRecord_dialog_window = tkinter.Toplevel(dialogs_manager.main_window)
    AddRecord_dialog_window.title('Добавление новой записи')
    AddRecord_dialog_window.geometry('485x260')
    AddRecord_dialog_window.resizable(0, 0)
    
    # Отправляем ссылку на окно диалога в менеджер диалогов
    dialogs_manager.AddRecord_dialog_window = AddRecord_dialog_window

    # При закрытии окна диалога вызываем функцию закрытия из менеджера диалогов,
    # таким образом сообщая ему, что окно диалога добавления новой записи было закрыто
    AddRecord_dialog_window.protocol('WM_DELETE_WINDOW', dialogs_manager.CloseAddRecordDialog)

    # Frame с отступом в не менее, чем 25 пикселей от границ окна
    AddRecord_dialog_window_frame = tkinter.Frame(AddRecord_dialog_window, padx=25, pady=25)
    AddRecord_dialog_window_frame.pack(expand=True)

    # Label с именем
    name_label = tkinter.Label(AddRecord_dialog_window_frame, text='Имя:')
    name_label.grid(row=1, column=1, padx=5, pady=5)

    # Поле для ввода имени
    name_entry = tkinter.Entry(AddRecord_dialog_window_frame, width=35)
    name_entry.grid(row=1, column=2, padx=5, pady=5)

    # Label с фамилией
    surname_label = tkinter.Label(AddRecord_dialog_window_frame, text='Фамилия:')
    surname_label.grid(row=2, column=1, padx=5, pady=5)

    # Поле для ввода фамилии
    surname_entry = tkinter.Entry(AddRecord_dialog_window_frame, width=35)
    surname_entry.grid(row=2, column=2, padx=5, pady=5)

    # Label с отчеством
    patronymic_label = tkinter.Label(AddRecord_dialog_window_frame, text='Отчество:')
    patronymic_label.grid(row=3, column=1, padx=5, pady=5)

    # Поле для ввода отчества
    patronymic_entry = tkinter.Entry(AddRecord_dialog_window_frame, width=35)
    patronymic_entry.grid(row=3, column=2, padx=5, pady=5)

    # Label с номером телефона
    number_label = tkinter.Label(AddRecord_dialog_window_frame, text='Номер телефона:')
    number_label.grid(row=4, column=1, padx=5, pady=5)

    # Поле для ввода номера телефона
    number_entry = tkinter.Entry(AddRecord_dialog_window_frame, width=35)
    number_entry.grid(row=4, column=2, padx=5, pady=5)

    # Label с заметкой
    note_label = tkinter.Label(AddRecord_dialog_window_frame, text='Заметка:')
    note_label.grid(row=5, column=1, padx=5, pady=5)

    # Поле для ввода заметки
    note_entry = tkinter.Entry(AddRecord_dialog_window_frame, width=35)
    note_entry.grid(row=5, column=2, padx=5, pady=5)

    # Кнопка добавления новой записи
    AddRecord_button = tkinter.Button(AddRecord_dialog_window_frame,
                                      text='Добавить новую запись', width=32,
                                      command=lambda: AddRecordDialogHandler(AddRecord_dialog_window,
                                                                             dialogs_manager.adress_entry.get(),
                                                                             name_entry.get(),
                                                                             surname_entry.get(),
                                                                             patronymic_entry.get(),
                                                                             number_entry.get(),
                                                                             note_entry.get()))
    AddRecord_button.grid(row=6, column=2, padx=5, pady=5)

# Диалог удаления записи
def DeleteRecordDialog(dialogs_manager):
    
    # Окно диалога
    DeleteRecord_dialog_window = tkinter.Toplevel(dialogs_manager.main_window)
    DeleteRecord_dialog_window.title('Удаление записи')
    DeleteRecord_dialog_window.geometry('500x200')
    DeleteRecord_dialog_window.resizable(0, 0)

    # Отправляем ссылку на окно диалога в менеджер диалогов
    dialogs_manager.DeleteRecord_dialog_window = DeleteRecord_dialog_window

    # При закрытии окна диалога вызываем функцию закрытия из менеджера диалогов,
    # таким образом сообщая ему, что окно диалога удаления записи было закрыто
    DeleteRecord_dialog_window.protocol('WM_DELETE_WINDOW', dialogs_manager.CloseDeleteRecordDialog)

    # Frame с отступом в не менее, чем 25 пикселей от границ окна
    DeleteRecord_dialog_window_frame = tkinter.Frame(DeleteRecord_dialog_window, padx=25, pady=25)
    DeleteRecord_dialog_window_frame.pack(expand=True)

    # Label с номером/id записи для удаления
    id_label = tkinter.Label(DeleteRecord_dialog_window_frame, text='Номер/id записи:')
    id_label.grid(row=1, column=1, padx=5, pady=5)

    # Поле для ввода номера/id записи для удаления
    id_entry = tkinter.Entry(DeleteRecord_dialog_window_frame, width=36)
    id_entry.grid(row=1, column=2, padx=5, pady=5)

    # Кнопка удаления записи по номеру/id
    DeleteRecordById_button = tkinter.Button(DeleteRecord_dialog_window_frame,
                                             text='Удалить запись по номеру/id', width=33,
                                             command=lambda: DeleteRecordByIdDialogHandler(DeleteRecord_dialog_window,
                                                                                           dialogs_manager.adress_entry.get(),
                                                                                           id_entry.get()))
    DeleteRecordById_button.grid(row=2, column=2, padx=5, pady=5)

    # Label с номером телефона записи для удаления
    number_label = tkinter.Label(DeleteRecord_dialog_window_frame, text='Номер телефона:')
    number_label.grid(row=3, column=1, padx=5, pady=5)

    # Поле для ввода номера телефона записи для удаления
    number_entry = tkinter.Entry(DeleteRecord_dialog_window_frame, width=36)
    number_entry.grid(row=3, column=2, padx=5, pady=5)

    # Кнопка удаления записи по номеру телефона
    DeleteRecordByNumber_button = tkinter.Button(DeleteRecord_dialog_window_frame,
                                                 text='Удалить запись по номеру телефона', width=33,
                                                 command=lambda: DeleteRecordByNumberDialogHandler(DeleteRecord_dialog_window,
                                                                                                   dialogs_manager.adress_entry.get(),
                                                                                                   number_entry.get()))
    DeleteRecordByNumber_button.grid(row=6, column=2, padx=5, pady=5)

# Диалог поиска записи
def FindRecordDialog(dialogs_manager):
    
    # Окно диалога
    FindRecord_dialog_window = tkinter.Toplevel(dialogs_manager.main_window)
    FindRecord_dialog_window.title('Поиск записи')
    FindRecord_dialog_window.geometry('1020x550')
    FindRecord_dialog_window.resizable(0, 0)

    # Отправляем ссылку на окно диалога в менеджер диалогов
    dialogs_manager.FindRecord_dialog_window = FindRecord_dialog_window

    # При закрытии окна диалога вызываем функцию закрытия из менеджера диалогов,
    # таким образом сообщая ему, что окно диалога поиска записи было закрыто
    FindRecord_dialog_window.protocol('WM_DELETE_WINDOW', dialogs_manager.CloseFindRecordDialog)

    # Frame с отступом в не менее, чем 25 пикселей от границ окна
    FindRecord_dialog_window_frame = tkinter.Frame(FindRecord_dialog_window, padx=25, pady=25)
    FindRecord_dialog_window_frame.pack(expand=True)

    # Столбцы таблицы для отображения результатов поиска
    data_columns = ('id', 'name', 'surname', 'patronymic', 'number', 'note')

    # Таблица для отображения результатов поиска
    data_tree = tkinter.ttk.Treeview(FindRecord_dialog_window_frame, columns=data_columns, show='headings')
    data_tree.grid(row=7, column=1, columnspan=3, padx=5, pady=15)

    # Заголовки столбцов таблицы для отображения результатов поиска
    data_tree.heading('id',         text='Номер/id',       anchor=tkinter.W)
    data_tree.heading('name',       text='Имя',            anchor=tkinter.W)
    data_tree.heading('surname',    text='Фамилия',        anchor=tkinter.W)
    data_tree.heading('patronymic', text='Отчество',       anchor=tkinter.W)
    data_tree.heading('number',     text='Номер телефона', anchor=tkinter.W)
    data_tree.heading('note',       text='Заметка',        anchor=tkinter.W)

    # Размеры столбцов таблицы для отображения результатов поиска
    data_tree.column("#1", width=80)
    data_tree.column("#2", width=140)
    data_tree.column("#3", width=140)
    data_tree.column("#4", width=140)
    data_tree.column("#5", width=180)
    data_tree.column("#6", width=240)

    # Вертикальную полоса прокрутки таблицы для отображения результатов поиска
    data_tree_scrollbar = tkinter.ttk.Scrollbar(FindRecord_dialog_window_frame, orient=tkinter.VERTICAL, command=data_tree.yview)
    data_tree.configure(yscroll=data_tree_scrollbar.set)
    data_tree_scrollbar.grid(row=7, column=4, padx=5, pady=15, sticky='ns')

    # Label с номером/id записи для поиска
    id_label = tkinter.Label(FindRecord_dialog_window_frame, text='Номер/id записи:')
    id_label.grid(row=1, column=1, padx=5, pady=5)

    # Поле для ввода номера/id записи для поиска
    id_entry = tkinter.Entry(FindRecord_dialog_window_frame, width=55)
    id_entry.grid(row=1, column=2, padx=5, pady=5)

    # Кнопка поиска записи по номеру/id
    FindRecordById_button = tkinter.Button(FindRecord_dialog_window_frame,
                                           text='Найти запись по номеру/id', width=33,
                                           command=lambda: FindRecordByIdDialogHandler(FindRecord_dialog_window,
                                                                                       data_tree,
                                                                                       dialogs_manager.adress_entry.get(),
                                                                                       id_entry.get()))
    FindRecordById_button.grid(row=1, column=3, columnspan=2, padx=5, pady=5)

    # Label с именем для поиска записей
    name_label = tkinter.Label(FindRecord_dialog_window_frame, text='Имя:')
    name_label.grid(row=2, column=1, padx=5, pady=5)

    # Поле для ввода имени для поиска записей
    name_entry = tkinter.Entry(FindRecord_dialog_window_frame, width=55)
    name_entry.grid(row=2, column=2, padx=5, pady=5)

    # Кнопка поиска записей по имени
    FindRecordByName_button = tkinter.Button(FindRecord_dialog_window_frame,
                                             text='Найти записи по имени', width=33,
                                             command=lambda: FindRecordByNameDialogHandler(FindRecord_dialog_window,
                                                                                           data_tree,
                                                                                           dialogs_manager.adress_entry.get(),
                                                                                           name_entry.get()))
    FindRecordByName_button.grid(row=2, column=3, columnspan=2, padx=5, pady=5)

    # Label с фамилией для поиска записей
    surname_label = tkinter.Label(FindRecord_dialog_window_frame, text='Фамилия:')
    surname_label.grid(row=3, column=1, padx=5, pady=5)

    # Поле для ввода фамилии для поиска записей
    surname_entry = tkinter.Entry(FindRecord_dialog_window_frame, width=55)
    surname_entry.grid(row=3, column=2, padx=5, pady=5)

    # Кнопка поиска записей по фамилии
    FindRecordBySurname_button = tkinter.Button(FindRecord_dialog_window_frame,
                                                text='Найти записи по фамилии', width=33,
                                                command=lambda: FindRecordBySurnameDialogHandler(FindRecord_dialog_window,
                                                                                                 data_tree,
                                                                                                 dialogs_manager.adress_entry.get(),
                                                                                                 surname_entry.get()))
    FindRecordBySurname_button.grid(row=3, column=3, columnspan=2, padx=5, pady=5)

    # Label с отчеством для поиска записей
    patronymic_label = tkinter.Label(FindRecord_dialog_window_frame, text='Отчество:')
    patronymic_label.grid(row=4, column=1, padx=5, pady=5)

    # Поле для ввода отчества для поиска записей
    patronymic_entry = tkinter.Entry(FindRecord_dialog_window_frame, width=55)
    patronymic_entry.grid(row=4, column=2, padx=5, pady=5)

    # Кнопка поиска записей по отчеству
    FindRecordByPatronymic_button = tkinter.Button(FindRecord_dialog_window_frame,
                                                   text='Найти записи по отчеству', width=33,
                                                   command=lambda: FindRecordByPatronymicDialogHandler(FindRecord_dialog_window,
                                                                                                       data_tree,
                                                                                                       dialogs_manager.adress_entry.get(),
                                                                                                       patronymic_entry.get()))
    FindRecordByPatronymic_button.grid(row=4, column=3, columnspan=2, padx=5, pady=5)

    # Label с номером телефона для поиска записи
    number_label = tkinter.Label(FindRecord_dialog_window_frame, text='Номер телефона:')
    number_label.grid(row=5, column=1, padx=5, pady=5)

    # Поле для ввода номера телефона для поиска записи
    number_entry = tkinter.Entry(FindRecord_dialog_window_frame, width=55)
    number_entry.grid(row=5, column=2, padx=5, pady=5)

    # Кнопка поиска записи по номеру телефона
    FindRecordByNumber_button = tkinter.Button(FindRecord_dialog_window_frame,
                                               text='Найти запись по номеру телефона', width=33,
                                               command=lambda: FindRecordByNumberDialogHandler(FindRecord_dialog_window,
                                                                                               data_tree,
                                                                                               dialogs_manager.adress_entry.get(),
                                                                                               number_entry.get()))
    FindRecordByNumber_button.grid(row=5, column=3, columnspan=2, padx=5, pady=5)

    # Label с заметкой для поиска записей
    note_label = tkinter.Label(FindRecord_dialog_window_frame, text='Заметка:')
    note_label.grid(row=6, column=1, padx=5, pady=5)

    # Поле для ввода заметки для поиска записей
    note_entry = tkinter.Entry(FindRecord_dialog_window_frame, width=55)
    note_entry.grid(row=6, column=2, padx=5, pady=5)

    # Кнопка поиска записей по заметке
    FindRecordByNote_button = tkinter.Button(FindRecord_dialog_window_frame,
                                             text='Найти записи по заметке', width=33,
                                             command=lambda: FindRecordByNoteDialogHandler(FindRecord_dialog_window,
                                                                                           data_tree,
                                                                                           dialogs_manager.adress_entry.get(),
                                                                                           note_entry.get()))
    FindRecordByNote_button.grid(row=6, column=3, columnspan=2, padx=5, pady=5)

    