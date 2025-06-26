#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include "ui/StartWindow.h"
#include "core/FileManager.h"

int main(int argc, char *argv[]) {
    system("chcp 1251");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("BorovikIvan");
    QCoreApplication::setApplicationName("SvojaIgra");

    app.setStyle(QStyleFactory::create("Fusion"));

    const QString darkTheme = R"(
        QWidget {
            background-color: #2b2b2b;
            color: #f0f0f0;
            font-family: Arial, sans-serif;
            border: none;
        }

        QMainWindow, QDialog {
            background-color: #3c3f41;
        }

        /* Стили для кнопок */
        QPushButton {
            background-color: #555555;
            color: #f0f0f0;
            border: 1px solid #666666;
            padding: 8px 16px;
            border-radius: 4px;
            font-size: 16px;
        }

        QPushButton:hover {
            background-color: #6a6a6a;
            border: 1px solid #777777;
        }

        QPushButton:pressed {
            background-color: #4a4a4a;
        }

        QPushButton:disabled {
            background-color: #404040;
            color: #888888;
            border-color: #555555;
        }

        /* Стили для полей ввода */
        QLineEdit, QTextEdit {
            background-color: #2b2b2b;
            color: #f0f0f0;
            border: 1px solid #666666;
            border-radius: 4px;
            padding: 5px;
        }

        /* Стили для заголовков */
        QLabel {
            color: #f0f0f0;
            background-color: transparent;
        }

        /* Стили для MessageBox */
        QMessageBox {
            background-color: #3c3f41;
        }

        QMessageBox QLabel {
            color: #f0f0f0;
        }

        /* Стили для ScrollArea */
        QScrollArea {
            border: none;
            background-color: #2b2b2b;
        }

        QScrollArea QWidget {
             background-color: #2b2b2b;
        }

        /* Стили для выпадающих списков */
        QComboBox {
            border: 1px solid #666666;
            border-radius: 3px;
            padding: 1px 18px 1px 3px;
            min-width: 6em;
            background-color: #555555;
        }
        QComboBox:on { /* a QComboBox is open */
            border: 1px solid #777777;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 15px;
            border-left-width: 1px;
            border-left-color: #666666;
            border-left-style: solid;
            border-top-right-radius: 3px;
            border-bottom-right-radius: 3px;
        }
        QComboBox QAbstractItemView {
            border: 1px solid #666666;
            selection-background-color: #6a6a6a;
            background-color: #2b2b2b;
        }
    )";

    app.setStyleSheet(darkTheme);

    FileManager::initQuestionsDirectory();

    QString lang = FileManager::loadLanguageSetting();
    QTranslator appTranslator;
    QString translationsPath = QCoreApplication::applicationDirPath() + "/translations";
    if (appTranslator.load("svojaigra_" + lang, translationsPath)) {
        app.installTranslator(&appTranslator);
    }
    QTranslator qtTranslator;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (qtTranslator.load("qtbase_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
#endif

    StartWindow startWindow;
    startWindow.setWindowIcon(QIcon(":/icons/icon.ico"));
    startWindow.showFullScreen();

    return app.exec();
}