# Модуль gui_main описывает главный интерфейс приложения

# Подключим модуль Tkinter для реализации GUI
import tkinter

# Подключим модуль c менеджером диалогов, который реализует класс,
# осуществляющий взаимодействия между окнами приложения
from gui_dialogs_manager import * 

# Главный интерфейс приложения
def MainInterface():

    # Главное окно приложения
    main_window = tkinter.Tk()
    main_window.title('Lesta Phone Book Client')
    main_window.geometry('300x280')
    main_window.resizable(0, 0)

    # Frame с отступом в не менее, чем 25 пикселей от границ окна
    main_window_frame = tkinter.Frame(main_window, padx=25, pady=25)
    main_window_frame.pack(expand=True)
    
    # Label с полным адресом сервера с портом
    adress_label = tkinter.Label(main_window_frame, text='Полный адрес сервера с портом:')
    adress_label.grid(row=1, column=1, padx=5, pady=5)

    # Поле для ввода полного адреса сервера с портом
    adress_entry = tkinter.Entry(main_window_frame, width=28)
    adress_entry.insert(0, 'localhost:50051')
    adress_entry.grid(row=2, column=1, padx=5, pady=5)

    # Менеджер диалогов
    dialogs_manager = DialogsManager(main_window, adress_entry)

    # Кнопка добавления записи
    # (открывает диалог добавления новой записи, если он не открыт в данный момент)
    AddRecord_button = tkinter.Button(main_window_frame,
                                      text='Добавить запись', width=25,
                                      command=dialogs_manager.OpenAddRecordDialog)
    AddRecord_button.grid(row=3, column=1, padx=5, pady=5)

    # Кнопка удаления записи
    # (открывает диалог удаления записи, если он не открыт в данный момент)
    DeleteRecord_button = tkinter.Button(main_window_frame,
                                         text='Удалить запись', width=25,
                                         command=dialogs_manager.OpenDeleteRecordDialog)
    DeleteRecord_button.grid(row=4, column=1, padx=5, pady=5)
    
    # Кнопка поиска записи
    # (открывает диалог поиска записи, если он не открыт в данный момент)
    FindRecord_button = tkinter.Button(main_window_frame,
                                       text='Найти запись', width=25,
                                       command=dialogs_manager.OpenFindRecordDialog)
    FindRecord_button.grid(row=5, column=1, padx=5, pady=5)

    # Кнопка выхода из приложения
    FindRecord_button = tkinter.Button(main_window_frame,
                                       text='Выйти', width=25,
                                       command=main_window.destroy)
    FindRecord_button.grid(row=6, column=1, padx=5, pady=5)

    # Запускаем главный цикл 
    main_window.mainloop()