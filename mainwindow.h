#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLabel>
#include <QTextCursor>
#include <QShortcut>

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

        QTextCursor _cursor;
        QScopedPointer<QLabel> _cursorePositionLabel;

        QScopedPointer<QShortcut> _scZoomInText;
        QScopedPointer<QShortcut> _scZoomOutText;
        QScopedPointer<QLabel> _textScaleLabel;
        int _percentZoomText;

        void assembleInterface();
        void fillMenu();
        void setupStatusBar();
        void setupCursorePosition();
        void setupZoomText();


        bool eventFilter(QObject *obj, QEvent *event) override;

        enum class ZoomType
        {
            In = 0,
            Out = 1,
            OnlyUpdate = 3
        };

    private Q_SLOTS:
        void updateCursorPositionInStatusBar();

        void updateZoomTextAndStatusBar(ZoomType zoomFunctor = ZoomType::OnlyUpdate);
    };
}
