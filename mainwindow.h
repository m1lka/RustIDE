#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLabel>
#include <QTextCursor>
#include <QShortcut>
#include <QString>

namespace RustIDE
{
    class MainWindow: public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);

    private:
        enum class ZoomType
        {
            In = 0,
            Out = 1,
            None = 2
        };

        QScopedPointer<QWidget> _centralWidget;
        QScopedPointer<QHBoxLayout> _boxLayout;
        QScopedPointer<QTextEdit> _textEditor;
        QScopedPointer<QMenuBar> _menuBar;

        QTextCursor _cursor;
        QScopedPointer<QLabel> _cursorePositionLabel;

        QScopedPointer<QShortcut> _scZoomInText;
        QScopedPointer<QShortcut> _scZoomOutText;
        QScopedPointer<QLabel> _textScaleLabel;

        QScopedPointer<QShortcut> _scCreateNewFile;
        QScopedPointer<QShortcut> _scOpenFile;
        QScopedPointer<QShortcut> _scSaveFile;
        QScopedPointer<QShortcut> _scSaveFileAs;

        QScopedPointer<QShortcut> _scUndo;
        QScopedPointer<QShortcut> _scRedo;


        const int _minZoomPercent;
        const int _stepZoomPercent;
        const QString _newFilename;

        int _currentZoomPercent;

        QString _windowTitle;
        QString _currentFileName;
        bool _currentFileSaved;

        void assembleInterface();
        void fillMenu();
        void setupStatusBar();
        void setupCursorPosition();
        void setupZoomText();
        void setupBaseShortcut();

        bool eventFilter(QObject *obj, QEvent *event) override;
        bool event(QEvent *event) override;

    private Q_SLOTS:
        void updateCursorPositionInStatusBar();

        void updateZoomAndStatusBar(ZoomType zoomFunctor = ZoomType::None);

        void updateWindowTitle(bool currentFileSaved = true);

        void createNewFile();
        void openFile();
        void saveFile();
        void saveFileAs();

        void exitApp(QEvent *event = nullptr);
    };
}
