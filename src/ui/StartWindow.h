#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include "MainWindow.h"

namespace Ui {
    class StartWindow;
}

class MainWindow;
class AdminEditor;

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private slots:
    void handleStart();
    void handleSettings();
    void handleEdit();

private:
    Ui::StartWindow *ui;

    MainWindow  *gameWindow;
    AdminEditor *adminWindow;
};

#endif // STARTWINDOW_H