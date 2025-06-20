#include "./AdminEditor.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>

AdminEditor::AdminEditor(QWidget *parent) : QWidget(parent) {
    qDebug() << "AdminEditor constructor: START (without .ui file)";

    setWindowTitle(tr("Редактор Админа (без UI файла)"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QLabel *titleLabel = new QLabel(tr("Добро пожаловать в Редактор Админа!"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont font = titleLabel->font();
    font.setPointSize(16);
    font.setBold(true);
    titleLabel->setFont(font);
    mainLayout->addWidget(titleLabel);

    QLabel *contentPlaceholder = new QLabel(tr("Здесь будет содержимое для редактирования вопросов."), this);
    contentPlaceholder->setAlignment(Qt::AlignCenter);
    contentPlaceholder->setWordWrap(true);
    mainLayout->addWidget(contentPlaceholder);

    QPushButton *backButton = new QPushButton(tr("Назад"), this);
    backButton->setFixedSize(120, 40);
    mainLayout->addStretch();
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    connect(backButton, &QPushButton::clicked, this, [this]() {
        qDebug() << "AdminEditor: 'Назад' button clicked.";
        QMessageBox::information(this, tr("Действие"), tr("Вы нажали кнопку 'Назад'."));
    });


    qDebug() << "AdminEditor constructor: END (without .ui file)";
}

AdminEditor::~AdminEditor() {
    qDebug() << "AdminEditor destructor: START (without .ui file)";
    qDebug() << "AdminEditor destructor: END (without .ui file)";
}
