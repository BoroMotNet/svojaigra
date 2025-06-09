cmake_minimum_required(VERSION 3.16)

project(SvojaIgra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia LinguistTools)

qt_add_translations(SvojaIgra # Название вашего проекта (цели)
        # Список файлов перевода, которые мы хотим создать
        TS_FILES
        resources/translations/svojaigra_en.ts
        resources/translations/svojaigra_be.ts
        resources/translations/svojaigra_ru.ts

        SOURCES
        src/main.cpp
        src/ui/StartWindow.cpp
        src/ui/StartWindow.h
        src/ui/MainWindow.cpp
        src/ui/MainWindow.h
        src/ui/AdminEditor.cpp
        src/ui/AdminEditor.h
)

add_executable(SvojaIgra
        src/main.cpp
        src/ui/StartWindow.h
        src/ui/StartWindow.cpp
        src/ui/MainWindow.h   
        src/ui/MainWindow.cpp 
        src/ui/AdminEditor.h  
        src/ui/AdminEditor.cpp
        src/core/Question.cpp
        src/core/Player.cpp
        src/core/GameManager.cpp
        src/core/FileManager.h
        src/core/GameManager.h
        src/core/Player.h
        src/ui/QuestionDialog.cpp
        src/ui/QuestionDialog.h
        src/core/FileManager.cpp
        resources/resources.qrc
        src/ui/SettingsDialog.cpp
        src/ui/SettingsDialog.h
        src/ui/PlayerNameDialog.cpp
        src/ui/PlayerNameDialog.h
)

target_link_libraries(SvojaIgra PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
)