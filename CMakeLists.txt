cmake_minimum_required(VERSION 3.14)
project(lab7)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Собираем библиотеку из всех .cpp в src/
file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_library(${PROJECT_NAME}_lib ${SRC_FILES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC include)

# Основной исполняемый файл (со случайной генерацией)
add_executable(${PROJECT_NAME}_exe main.cpp)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE ${PROJECT_NAME}_lib)

# Второй исполняемый файл (с загрузкой из файла)
add_executable(${PROJECT_NAME}_2_exe main2.cpp)
target_link_libraries(${PROJECT_NAME}_2_exe PRIVATE ${PROJECT_NAME}_lib)

# Тесты
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}_tests test/test.cpp)
target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE
        ${PROJECT_NAME}_lib
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)

# Копирование файла npc.txt в директорию сборки
configure_file(npc.txt ${CMAKE_CURRENT_BINARY_DIR}/npc.txt COPYONLY)
configure_file(npc.txt ${CMAKE_CURRENT_BINARY_DIR}/../npc.txt COPYONLY)

# Установка целевых файлов
install(TARGETS ${PROJECT_NAME}_exe ${PROJECT_NAME}_2_exe ${PROJECT_NAME}_tests
        RUNTIME DESTINATION bin)

# Создание удобных псевдонимов
add_custom_target(run
    DEPENDS ${PROJECT_NAME}_exe
    COMMAND ./${PROJECT_NAME}_exe
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run2
    DEPENDS ${PROJECT_NAME}_2_exe
    COMMAND ./${PROJECT_NAME}_2_exe
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_tests
    DEPENDS ${PROJECT_NAME}_tests
    COMMAND ./${PROJECT_NAME}_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Информация о целях сборки
message(STATUS "Build targets available:")
message(STATUS "  - ${PROJECT_NAME}_exe: Main program with random generation")
message(STATUS "  - ${PROJECT_NAME}_2_exe: Program with file loading")
message(STATUS "  - ${PROJECT_NAME}_tests: Run unit tests")
message(STATUS "  - run: Build and run main program")
message(STATUS "  - run2: Build and run file loading program")
message(STATUS "  - run_tests: Build and run tests")