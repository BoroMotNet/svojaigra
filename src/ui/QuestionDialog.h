#ifndef QUESTIONDIALOG_H
#define QUESTIONDIALOG_H

#include <QDialog>
#include "../core/Question.h"

class QLabel;
class QLineEdit;

class QuestionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionDialog(const Question& question, QWidget *parent = nullptr);
    QString getAnswer() const;

private:
    QLineEdit* m_answerEdit;
};

#endif // QUESTIONDIALOG_H