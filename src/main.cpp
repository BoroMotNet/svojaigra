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

int main(int argc, char *argv[])
{
    system("chcp 1251");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Borovik Ivan T-391");
    QCoreApplication::setApplicationName("Своя Игра");

    qDebug() << "--- ПАЧАТАК ДЫЯГНОСТЫКІ ЛАКАЛІЗАЦЫІ ---";

    QString lang = FileManager::loadLanguageSetting();
    qDebug() << "[INFO] Мова, загружаная з налад:" << lang;

    QString workingDir = QCoreApplication::applicationDirPath();
    qDebug() << "[INFO] Рабочая дырэкторыя праграмы:" << workingDir;

    QString translationsPath = workingDir + "/translations";
    qDebug() << "[CHECK] Поўны шлях да папкі перакладаў:" << translationsPath;

    QDir translationsDir(translationsPath);
    if (translationsDir.exists()) {
        qDebug() << "[OK] Папка 'translations' існуе.";
    } else {
        qWarning() << "[ERROR] Папка 'translations' НЕ ІСНУЕ па ўказаным шляху!";
    }

    QString qmFileName = "svojaigra_" + lang + ".qm";
    QString fullQmPath = translationsPath + "/" + qmFileName;
    qDebug() << "[CHECK] Поўны шлях да файла перакладу:" << fullQmPath;

    if (QFile::exists(fullQmPath)) {
        qDebug() << "[OK] Файл" << qmFileName << "існуе.";
    } else {
        qWarning() << QStringLiteral("[ERROR] Файл \"%1\" НЕ ЗНОЙДЗЕНЫ па ўказаным шляху!").arg(qmFileName);
    }

    QTranslator appTranslator;
    if (appTranslator.load("svojaigra_" + lang, translationsPath)) {
        app.installTranslator(&appTranslator);
        qDebug() << "[OK] Файл перакладу паспяхова загружаны з дыска!";
    } else {
        qWarning() << "[ERROR] Спроба загрузкі файла не атрымалася!";
    }

    qDebug() << "--- КАНЕЦ ДЫЯГНОСТЫКІ ЛАКАЛІЗАЦЫІ ---";

    QTranslator qtTranslator;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (qtTranslator.load("qtbase_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
    #endif

    StartWindow startWindow;
    startWindow.showFullScreen();

    return app.exec();
}