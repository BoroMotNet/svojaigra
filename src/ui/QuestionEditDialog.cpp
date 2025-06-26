#include "QuestionEditDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QMessageBox>
#include <QDebug>

QuestionEditDialog::QuestionEditDialog(MultilingualQuestionData &questionData, QWidget *parent)
    : QDialog(parent), m_questionData(questionData) {
    setupUi();
    loadData();
    QString title = m_questionData.displayNames.value("ru", m_questionData.category);
    setWindowTitle(m_questionData.category.isEmpty()
                       ? tr("Создание нового вопроса")
                       : tr("Редактирование: %1").arg(title));
}

bool QuestionEditDialog::isMarkedForDeletion() const {
    return m_markedForDeletion;
}

void QuestionEditDialog::setupUi() {
    setMinimumSize(700, 400);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QTabWidget *langTabs = new QTabWidget(this);
    QStringList languages = {"ru", "en", "be"};

    for (const QString &lang: languages) {
        QWidget *tabWidget = new QWidget();
        QFormLayout *tabLayout = new QFormLayout(tabWidget);
        tabLayout->setSpacing(10);

        QLabel *questionLabel = new QLabel(tr("Текст вопроса:"), tabWidget);
        m_questionEdits[lang] = new QTextEdit(tabWidget);
        m_questionEdits[lang]->setMinimumHeight(100);

        QLabel *answerLabel = new QLabel(tr("Ответ:"), tabWidget);
        m_answerEdits[lang] = new QLineEdit(tabWidget);

        tabLayout->addRow(questionLabel, m_questionEdits[lang]);
        tabLayout->addRow(answerLabel, m_answerEdits[lang]);
        langTabs->addTab(tabWidget, lang.toUpper());
    }

    mainLayout->addWidget(langTabs);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    QPushButton *saveButton = buttonBox->addButton(tr("Сохранить"), QDialogButtonBox::AcceptRole);
    QPushButton *cancelButton = buttonBox->addButton(tr("Отмена"), QDialogButtonBox::RejectRole);
    QPushButton *deleteButton = new QPushButton(tr("Удалить вопрос"), this);
    deleteButton->setStyleSheet("background-color: #ff4d4d; color: white;");

    if (m_questionData.points == 0) {
        deleteButton->hide();
    }

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(deleteButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(buttonBox);
    mainLayout->addLayout(bottomLayout);

    connect(saveButton, &QPushButton::clicked, this, &QuestionEditDialog::saveChanges);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(deleteButton, &QPushButton::clicked, this, &QuestionEditDialog::markForDeletion);
}

void QuestionEditDialog::loadData() {
    for (auto it = m_questionEdits.constBegin(); it != m_questionEdits.constEnd(); ++it) {
        it.value()->setText(m_questionData.questionTexts.value(it.key()));
    }
    for (auto it = m_answerEdits.constBegin(); it != m_answerEdits.constEnd(); ++it) {
        it.value()->setText(m_questionData.answerTexts.value(it.key()));
    }
}

void QuestionEditDialog::saveChanges() {
    for (auto it = m_questionEdits.constBegin(); it != m_questionEdits.constEnd(); ++it) {
        m_questionData.questionTexts[it.key()] = it.value()->toPlainText();
    }
    for (auto it = m_answerEdits.constBegin(); it != m_answerEdits.constEnd(); ++it) {
        m_questionData.answerTexts[it.key()] = it.value()->text();
    }

    m_questionData.type = Question::TextOnly;
    m_questionData.mediaPath = "";

    if (m_questionData.questionTexts["ru"].isEmpty() || m_questionData.answerTexts["ru"].isEmpty() || m_questionData.
        questionTexts["en"].isEmpty() || m_questionData.answerTexts["en"].isEmpty() || m_questionData.questionTexts[
            "be"].isEmpty() || m_questionData.answerTexts["be"].isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Текст вопроса и ответ должны быть заполнены на всех языках."));
        return;
    }
    accept();
}

void QuestionEditDialog::markForDeletion() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Удаление вопроса"),
                                  tr("Вы уверены, что хотите безвозвратно удалить этот вопрос?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_markedForDeletion = true;
        accept();
    }
}
