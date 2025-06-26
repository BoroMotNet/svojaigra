#ifndef LEADERBOARDDIALOG_H
#define LEADERBOARDDIALOG_H

#include <QDialog>

class QTableWidget;

class LeaderboardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LeaderboardDialog(QWidget *parent = nullptr);

private:
    void setupUi();
    void populateTable();

    QTableWidget* m_tableWidget;
};

#endif