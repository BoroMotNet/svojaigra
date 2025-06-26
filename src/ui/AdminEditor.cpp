#include "AdminEditor.h"
#include "ui_AdminEditor.h"
#include "../core/FileManager.h"
#include "QuestionEditDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QMessageBox>
#include <QIcon>
#include <QStyle>
#include <QDebug>
#include <algorithm>

AdminEditor::AdminEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminEditor),
    m_isDirty(false)
{
    ui->setupUi(this);
    setupUi();
    loadAllQuestions();
    populateBoard();
}

AdminEditor::~AdminEditor()
{
    delete ui;
}

void AdminEditor::setupUi() {
    setWindowTitle(tr("Редактор вопросов"));
    this->setMinimumSize(1024, 768);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel(tr("Редактор игровых вопросов"), this);
    titleLabel->setFont(QFont("Arial", 18, QFont::Bold));

    m_saveButton = new QPushButton(tr("Сохранить все изменения"), this);
    m_saveButton->setFont(QFont("Arial", 12));
    m_saveButton->setIcon(QIcon::fromTheme("document-save", style()->standardIcon(QStyle::SP_DialogSaveButton)));

    m_exitButton = new QPushButton(tr("Выход в главное меню"), this);
    m_exitButton->setFont(QFont("Arial", 12));
    m_exitButton->setIcon(QIcon::fromTheme("application-exit", style()->standardIcon(QStyle::SP_DialogCancelButton)));

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_saveButton);
    topLayout->addWidget(m_exitButton);
    mainLayout->addLayout(topLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(m_scrollArea);

    connect(m_saveButton, &QPushButton::clicked, this, &AdminEditor::saveAllChanges);
    connect(m_exitButton, &QPushButton::clicked, this, &AdminEditor::handleExit);
}

void AdminEditor::loadAllQuestions() {
    m_allQuestions = FileManager::loadAllQuestionsForEditor();
}

void AdminEditor::populateBoard() {
    qDebug() << "Заполняю доску редактора. Загружено категорий:" << m_allQuestions.size();

    QWidget* oldBoardContainer = m_scrollArea->takeWidget();
    if (oldBoardContainer) {
        oldBoardContainer->deleteLater();
    }

    QWidget* boardContainer = new QWidget();
    QGridLayout* boardLayout = new QGridLayout(boardContainer);
    boardLayout->setSpacing(10);

    int col = 0;
    for (auto it_cat = m_allQuestions.constBegin(); it_cat != m_allQuestions.constEnd(); ++it_cat) {
        const QString& category = it_cat.key();

        QLabel* categoryLabel = new QLabel(category, boardContainer);
        categoryLabel->setAlignment(Qt::AlignCenter);
        categoryLabel->setFont(QFont("Arial", 14, QFont::Bold));
        categoryLabel->setWordWrap(true);
        boardLayout->addWidget(categoryLabel, 0, col);

        int row = 1;
        QList<int> pointsList = it_cat.value().keys();
        std::sort(pointsList.begin(), pointsList.end());

        for (int points : pointsList) {
            QWidget* questionWidget = createQuestionWidget(category, points);
            boardLayout->addWidget(questionWidget, row, col);
            row++;
        }

        QPushButton* addButton = new QPushButton(QIcon::fromTheme("list-add", style()->standardIcon(QStyle::SP_FileDialogNewFolder)), tr(" Добавить"), boardContainer);
        connect(addButton, &QPushButton::clicked, this, [this, category](){ addNewQuestion(category); });
        boardLayout->addWidget(addButton, row, col, Qt::AlignTop);

        col++;
    }

    boardLayout->setRowStretch(boardLayout->rowCount(), 1);
    boardLayout->setColumnStretch(boardLayout->columnCount(), 1);

    m_scrollArea->setWidget(boardContainer);
}


QWidget* AdminEditor::createQuestionWidget(const QString& category, int points) {
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(5);

    QLabel* pointsLabel = new QLabel(QString::number(points), widget);
    pointsLabel->setFont(QFont("Arial", 20, QFont::Bold));

    QPushButton* editButton = new QPushButton(widget);
    editButton->setFixedSize(32, 32);
    editButton->setIcon(style()->standardIcon(QStyle::SP_DriveHDIcon));
    editButton->setToolTip(tr("Редактировать вопрос"));

    layout->addStretch();
    layout->addWidget(pointsLabel);
    layout->addWidget(editButton);
    layout->addStretch();

    connect(editButton, &QPushButton::clicked, this, [this, category, points](){ openQuestionEditor(category, points);
            });

    return widget;
}

void AdminEditor::openQuestionEditor(const QString& category, int points) {
    if (!m_allQuestions.contains(category) || !m_allQuestions[category].contains(points)) return;

    QuestionEditDialog dialog(m_allQuestions[category][points], this);
    if (dialog.exec() == QDialog::Accepted) {
        m_isDirty = true;
        populateBoard();

        if (dialog.isMarkedForDeletion()) {
             m_allQuestions[category].remove(points);
             if (m_allQuestions[category].isEmpty()) {
                m_allQuestions.remove(category);
             }
            QMessageBox::information(this, tr("Успешно"), tr("Вопрос был удален. Не забудьте сохранить изменения."));
        } else {
            QMessageBox::information(this, tr("Успешно"), tr("Вопрос обновлен. Не забудьте сохранить изменения."));
        }
        populateBoard();
    }
}

void AdminEditor::addNewQuestion(const QString& category) {
    int maxPoints = 0;
    if(m_allQuestions.contains(category)) {
        for(int points : m_allQuestions[category].keys()) {
            if (points > maxPoints) maxPoints = points;
        }
    }
    int newPoints = (maxPoints / 100 + 1) * 100;

    MultilingualQuestionData newData;
    newData.category = category;
    newData.points = newPoints;
    newData.type = Question::TextOnly;

    QuestionEditDialog dialog(newData, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_isDirty = true;
        m_allQuestions[newData.category][newData.points] = newData;
        populateBoard();
    }
}

void AdminEditor::saveAllChanges() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Сохранение"),
                                  tr("Вы уверены, что хотите сохранить все изменения в файлы вопросов?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        bool success = FileManager::saveAllQuestionsFromEditor(m_allQuestions);
        if (success) {
            m_isDirty = false;
            QMessageBox::information(this, tr("Успех"), tr("Все изменения успешно сохранены."));
        } else {
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось сохранить изменения в файлы. Проверьте консоль на наличие ошибок."));
        }
    }
}

void AdminEditor::handleExit()
{
    this->close();
}

void AdminEditor::closeEvent(QCloseEvent *event)
{
    if (m_isDirty) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Несохраненные изменения"),
                                      tr("У вас есть несохраненные изменения. Вы уверены, что хотите выйти?\nВсе несохраненные данные будут потеряны."),
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            event->ignore();
        } else {
            event->accept();
            if (parentWidget()) {
                parentWidget()->show();
            }
        }
    } else {
        event->accept();
    }
}