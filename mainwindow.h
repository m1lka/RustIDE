#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLabel>
#include <QTextDocument>
#include <QTextCursor>

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

        QScopedPointer<QTextDocument> _document;
        QTextCursor _cursor;
        QScopedPointer<QLabel> _statusBarLabel;

        void assembleInterface();
        void setupStatusBar();
        void fillMenu();

    private Q_SLOTS:
        void updateStatusBar();
    };
}
