// Заголовочный файл linux_getch_function.h содержит в себе функцию getch для Linux,
// аналогичную функции getch() из conio.h в Windows

// Поскольку все объявленные здесь объекты и функции будут определены (иметь definition'ы)
// в header-файле, то они будут иметь ключевое слово inline, чтобы не нарушать one
// definition rule (они будут встраиваемыми)

// Header guard (предотвращает повторное включение заголовочного файла)
#pragma once

// Подключим библиотеку termios.h, содержащую функционал по настройке ввода-вывода
// в терминал (консоль). Внимание: это C-style библиотека
#include <termios.h>

// Подключим библиотеку cstdio, содержащую функционал по вводу-выводу в терминал
// (консоль). Внимание: это C-style библиотека
#include <cstdio>

// Пространство имён для имплементации функции getch для Linux
namespace linux_getch_function {

// Пространство имён для вспомогательных объектов и функций
namespace detail {

// Структура, содержащая предыдущие параметры ввода-вывода в терминал
//
// (это C-style структура, поэтому для объявления необходимо ключевое
// слово struct перед типом, также сделаем этот объект статическим и
// inline'овым, дабы не нарушать one definition rule, такой приём доступен,
// начиная со стандарта С++17)
inline static struct termios Previous_terminal_input_output_settings;

// Структура, содержащая текущие параметры ввода-вывода в терминал
//
// (это C-style структура, поэтому для объявления необходимо ключевое
// слово struct перед типом, также сделаем этот объект статическим и
// inline'овым, дабы не нарушать one definition rule, такой приём доступен,
// начиная со стандарта С++17)
inline static struct termios Current_terminal_input_output_settings;

// Функция установки новых параметров ввода-вывода в терминал
inline void Init_new_terminal_input_output_settings() {

    // Забираем текущие параметры ввода-вывода в терминал и записываем в Previous
    tcgetattr(0, &Previous_terminal_input_output_settings);         

    // Копируем эти параметры из Previous в Current
    Current_terminal_input_output_settings = Previous_terminal_input_output_settings;

    // Current-параметры будут отличаться отключенной буферизацией ввода-вывода
    // (disabled buffered input-output) и отключенным выводом в консоль вводимой
    // информации (no echo mode)
    Current_terminal_input_output_settings.c_lflag &= ~ICANON;
    Current_terminal_input_output_settings.c_lflag &= ~ECHO;

    // Устанавливаем в качестве параметров ввода-вывода в терминал Current-параметры
    tcsetattr(0, TCSANOW, &Current_terminal_input_output_settings);
}

// Функция установки предыдущих ввода-вывода в терминал
inline void Restore_original_terminal_input_output_settings() {
    // Устанавливаем в качестве параметров ввода-вывода в терминал Previous-параметры
    tcsetattr(0, TCSANOW, &Previous_terminal_input_output_settings);
}

}

// Функция getch, считывающая из терминала один вводимый символ (без отображения этого
// символа в терминале), аналогичная функции getch() из conio.h в Windows
inline char getch() {

    // Вводимый символ
    char ch;

    // Отключаем у терминала буферизацию ввода-вывода и отображение вводимых данных
    detail::Init_new_terminal_input_output_settings();

    // Считываем один символ при помощи getchar
    ch = getchar();

    // Возвращаем старые параметры ввода-вывода у терминала
    detail::Restore_original_terminal_input_output_settings();

    // Возвращаем полученный символ
    return ch;
}

}