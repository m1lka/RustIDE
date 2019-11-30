#pragma once

#include <QTextEdit>

namespace RustIDE::Ui
{
    class TextEdit : public QTextEdit
    {
    public:
        TextEdit(QWidget *parent = nullptr);

    private:
        TextEdit *t;
    };
}

