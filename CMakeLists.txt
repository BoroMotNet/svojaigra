cmake_minimum_required(VERSION 3.16)

project(SvojaIgra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia LinguistTools)


set(SVOJA_IGRA_SOURCES
        src/main.cpp
        src/ui/startwindow.cpp
        src/ui/MainWindow.cpp
        src/ui/AdminEditor.cpp
        src/core/Question.cpp
        src/core/Player.cpp
        src/core/GameManager.cpp
        src/core/FileManager.cpp
        src/ui/QuestionDialog.cpp
        src/ui/PlayerNameDialog.cpp
        src/ui/settingsdialog.cpp
        src/ui/QuestionEditDialog.cpp
        src/ui/QuestionEditDialog.h
        src/ui/StartWindow.ui
        src/core/UserManager.cpp
        src/core/UserManager.h
        src/ui/GameSetupDialog.cpp
        src/ui/GameSetupDialog.h
        src/ui/LeaderboardDialog.cpp
        src/ui/LeaderboardDialog.h
        src/ui/HelpDialog.cpp
        src/ui/HelpDialog.h
)


set(TRANSLATIONS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/translations")
file(COPY resources/help DESTINATION ${CMAKE_BINARY_DIR})


file(MAKE_DIRECTORY ${TRANSLATIONS_OUTPUT_DIR})

qt_add_translations(SvojaIgra
        TS_FILES
        resources/translations/svojaigra_en.ts
        resources/translations/svojaigra_be.ts
        resources/translations/svojaigra_ru.ts
        QM_OUTPUT_DIRECTORY ${TRANSLATIONS_OUTPUT_DIR}
)

add_executable(SvojaIgra
        ${SVOJA_IGRA_SOURCES}
        icon.rc
        resources/resources.qrc
)

target_link_libraries(SvojaIgra PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
)