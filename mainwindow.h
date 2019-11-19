#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMenuBar>

namespace RustIDE
{
    class MainWindow: public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);

    private:
        QScopedPointer<QWidget> _centralWidget;
        QScopedPointer<QHBoxLayout> _boxLayout;
        QScopedPointer<QTextEdit> _textEditor;
        QScopedPointer<QMenuBar> _menuBar;

        using QMenuPtr = QScopedPointer<QMenu>;

        void assembleInterface();
        void fillMenu();
    };
}
