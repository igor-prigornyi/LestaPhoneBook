# Модуль gui_dialogs_manager описывает класс менеджера диалогов
# (осуществляет взаимодействия между окнами приложения)

# Подключим модуль Tkinter для реализации GUI
import tkinter

# Подключим модуль с диалогами по добавлению, удалению и поиску записей
from gui_dialogs import *

# Класс менеджера диалогов
class DialogsManager:

    # Конструктор захватывает ссылку на главное окно приложения и поле для ввода
    # полного адреса сервера с портом (важно захватить именно поле, а не его значение,
    # так как во время работы приложения пользователь может поменять значение поля)
    def __init__(self, main_window, adress_entry):
        self.main_window  = main_window
        self.adress_entry = adress_entry

        # Ссылки на окна диалогов (вначале None)
        self.AddRecord_dialog_window    = None
        self.DeleteRecord_dialog_window = None
        self.FindRecord_dialog_window   = None

    # Метод, который вызывается при нажатии кнопки "Добавить запись" в главном окне
    def OpenAddRecordDialog(self):
        # Если окно с диалогом добавления записи не открыто, то открываем его
        if self.AddRecord_dialog_window is None: AddRecordDialog(self)
        # А если окно с диалогом добавления записи открыто, переместим фокус на него
        else: self.AddRecord_dialog_window.focus_force()

    # Метод, который вызывается при закрытии диалога добавления записи
    def CloseAddRecordDialog(self):
        # Закрываем окно и присваиваем ссылке на него значение None
        self.AddRecord_dialog_window.destroy()
        self.AddRecord_dialog_window = None

    # Метод, который вызывается при нажатии кнопки "Удалить запись" в главном окне
    def OpenDeleteRecordDialog(self):
        # Если окно с диалогом удаления записи не открыто, то открываем его
        if self.DeleteRecord_dialog_window is None: DeleteRecordDialog(self)
        # А если окно с диалогом удаления записи открыто, переместим фокус на него
        else: self.DeleteRecord_dialog_window.focus_force()

    # Метод, который вызывается при закрытии диалога удаления записи
    def CloseDeleteRecordDialog(self):
        # Закрываем окно и присваиваем ссылке на него значение None
        self.DeleteRecord_dialog_window.destroy()
        self.DeleteRecord_dialog_window = None

    # Метод, который вызывается при нажатии кнопки "Найти запись" в главном окне
    def OpenFindRecordDialog(self):
        # Если окно с диалогом поиска записи не открыто, то открываем его
        if self.FindRecord_dialog_window is None: FindRecordDialog(self)
        # А если окно с диалогом поиска записи открыто, переместим фокус на него
        else: self.FindRecord_dialog_window.focus_force()

    # Метод, который вызывается при закрытии диалога поиска записи
    def CloseFindRecordDialog(self):
        # Закрываем окно и присваиваем ссылке на него значение None
        self.FindRecord_dialog_window.destroy()
        self.FindRecord_dialog_window = None