#ifndef ADMINEDITOR_H
#define ADMINEDITOR_H

#include <QWidget>
namespace Ui {
    class AdminEditor;
}

class AdminEditor : public QWidget {
    Q_OBJECT

public:
    explicit AdminEditor(QWidget *parent = nullptr);

    ~AdminEditor();

private:
    Ui::AdminEditor *ui;
};

#endif