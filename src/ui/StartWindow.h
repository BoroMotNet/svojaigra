#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QWidget>

class QPushButton;
class QVBoxLayout;
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
    QPushButton *startButton;
    QPushButton *settingsButton;
    QPushButton *editButton;
    QPushButton *exitButton;
    QVBoxLayout *mainLayout;

    MainWindow  *gameWindow;
    AdminEditor *adminWindow;
};

#endif // STARTWINDOW_H