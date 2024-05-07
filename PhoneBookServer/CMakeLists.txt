cmake_minimum_required(VERSION 3.8)

project(PhoneBookServer C CXX)

include(Common.cmake)

# Подключение proto-файла
get_filename_component(phone_book_proto "../protos/connection.proto" ABSOLUTE)
get_filename_component(phone_book_proto_path "${phone_book_proto}" PATH)

# Сгенерированные из proto-файла файлы
set(phone_book_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/connection.pb.cc")
set(phone_book_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/connection.pb.h")
set(phone_book_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/connection.grpc.pb.cc")
set(phone_book_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/connection.grpc.pb.h")
add_custom_command(
    OUTPUT "${phone_book_proto_srcs}" "${phone_book_proto_hdrs}" "${phone_book_grpc_srcs}" "${phone_book_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${phone_book_proto_path}"
         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
         "${phone_book_proto}"
    DEPENDS "${phone_book_proto}")

# Подключение сгенерированных из proto-файла файлов
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(phone_book_grpc_proto
            ${phone_book_grpc_srcs}
            ${phone_book_grpc_hdrs}
            ${phone_book_proto_srcs}
            ${phone_book_proto_hdrs})
target_link_libraries(phone_book_grpc_proto
                      ${_REFLECTION}
                      ${_GRPC_GRPCPP}
                      ${_PROTOBUF_LIBPROTOBUF})

# Добавляем директорию с заголовочными файлами для включения
include_directories("headers")

# Функции для работы со строками (string_functions.cpp)
add_library(string_functions
            "headers/string_functions.h"
            "sources/string_functions.cpp")

# База данных для телефонной книги (phone_book_database.cpp)
add_library(phone_book_database
            "headers/phone_book_database.h"
            "sources/phone_book_database.cpp")
target_link_libraries(phone_book_database
                      string_functions)

# Сервер для телефонной книги (phone_book_server.cpp)
add_library(phone_book_server
            "headers/phone_book_server.h"
            "sources/phone_book_server.cpp")
target_link_libraries(phone_book_server
                      phone_book_database
                      phone_book_grpc_proto
                      ${_REFLECTION}
                      ${_GRPC_GRPCPP}
                      ${_PROTOBUF_LIBPROTOBUF})

# Сборка исполняемого файла
add_executable(main "sources/main.cpp")
target_link_libraries(main
                      phone_book_server
                      phone_book_database)