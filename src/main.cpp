#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QStringLiteral>
#include "ui/StartWindow.h"
#include "core/FileManager.h"

int main(int argc, char *argv[]) {
    system("chcp 1251");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Borovik Ivan T-391");
    QCoreApplication::setApplicationName("Своя Игра");

    qDebug() << "--- НАЧАЛО ДИАГНОСТИКИ ЛОКАЛИЗАЦИИ ---";

    QString lang = FileManager::loadLanguageSetting();
    qDebug() << "[INFO] Язык, загруженный из настроек:" << lang;

    QString workingDir = QCoreApplication::applicationDirPath();
    qDebug() << "[INFO] Рабочая директория приложения:" << workingDir;

    QString translationsPath = workingDir + "/translations";
    qDebug() << "[CHECK] Полный путь к папке переводов:" << translationsPath;

    QDir translationsDir(translationsPath);
    if (translationsDir.exists()) {
        qDebug() << "[OK] Папка 'translations' существует.";
    } else {
        qWarning() << "[ERROR] Папка 'translations' НЕ СУЩЕСТВУЕТ по указанному пути!";
    }

    QString qmFileName = "svojaigra_" + lang + ".qm";
    QString fullQmPath = translationsPath + "/" + qmFileName;
    qDebug() << "[CHECK] Полный путь к файлу перевода:" << fullQmPath;

    if (QFile::exists(fullQmPath)) {
        qDebug() << "[OK] Файл" << qmFileName << "существует.";
    } else {
        qWarning() << QStringLiteral("[ERROR] Файл \"%1\" НЕ НАЙДЕН по указанному пути!").arg(qmFileName);
    }

    QTranslator appTranslator;
    if (appTranslator.load("svojaigra_" + lang, translationsPath)) {
        app.installTranslator(&appTranslator);
        qDebug() << "[OK] Файл перевода успешно загружен с диска!";
    } else {
        qWarning() << "[ERROR] Не удалось загрузить файл перевода!";
    }

    qDebug() << "--- КОНЕЦ ДИАГНОСТИКИ ЛОКАЛИЗАЦИИ ---";


    QTranslator qtTranslator;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (qtTranslator.load("qtbase_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
#endif

    QSettings settings;

    QColor backgroundColor = QColor(settings.value("#1e1e1e").toString());
    QPalette palette = app.palette();
    palette.setColor(QPalette::Window, backgroundColor);
    palette.setColor(QPalette::WindowText, Qt::white);
    app.setPalette(palette);

    app.setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);


    StartWindow startWindow;
    startWindow.showFullScreen();

    return app.exec();
}
