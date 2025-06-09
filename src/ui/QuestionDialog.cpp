#include "./QuestionDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

QuestionDialog::QuestionDialog(const Question& question, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(question.category + " - " + QString::number(question.points));
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* questionLabel = new QLabel(question.questionText, this);
    questionLabel->setFont(QFont("Arial", 18));
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignCenter);

    m_answerEdit = new QLineEdit(this);
    m_answerEdit->setFont(QFont("Arial", 14));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    layout->addWidget(questionLabel, 1);
    layout->addWidget(m_answerEdit);
    layout->addWidget(buttonBox);
}

QString QuestionDialog::getAnswer() const
{
    return m_answerEdit->text();
}