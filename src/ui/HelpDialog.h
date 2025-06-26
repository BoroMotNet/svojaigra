#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

class QTextBrowser;
class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);

private:
    void setupUi();
    void loadHelpContent();

    QTextBrowser* m_textBrowser;
};

#endif