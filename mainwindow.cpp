#include "mainwindow.h"
#include <QException>
#include <QMenu>
#include <QDebug>
#include <QApplication>
#include <QStatusBar>
#include <QWheelEvent>

namespace RustIDE
{
    MainWindow::MainWindow(QWidget* parent):
        QMainWindow(parent),
        _minPercentZoomText(20),
        _stepPercentZoomText(10),
        _percentZoomText(100)
    {
        this->setMinimumSize(800, 600);

        assembleInterface();
    }

    void MainWindow::assembleInterface()
    {
        _centralWidget.reset(new QWidget(this));
        setCentralWidget(_centralWidget.get());

        _boxLayout.reset(new QHBoxLayout(_centralWidget.get()));
        _centralWidget.get()->setLayout(_boxLayout.get());
        _centralWidget->setLayout(_boxLayout.get());

        _textEditor.reset(new QTextEdit(_centralWidget.get()));
        _boxLayout->addWidget(_textEditor.get());

        _menuBar.reset(new QMenuBar(_centralWidget.get()));

        try
        {
            fillMenu();
        }
        catch (const std::exception& ex)
        {
            qDebug() << ex.what();
        }

        _boxLayout->setMenuBar(_menuBar.get());

        setupStatusBar();

        _textEditor->installEventFilter(this);
    }

    void MainWindow::setupStatusBar()
    {
        setupZoomText();

        setupCursorPosition();
    }

    void MainWindow::setupCursorPosition()
    {
        _cursorePositionLabel.reset(new QLabel(statusBar()));
        statusBar()->addPermanentWidget(_cursorePositionLabel.get());

        connect(_textEditor.get(), &QTextEdit::textChanged, this, &MainWindow::updateCursorPositionInStatusBar);
        connect(_textEditor.get(), &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPositionInStatusBar);

        updateCursorPositionInStatusBar();
    }

    void MainWindow::setupZoomText()
    {
        _textScaleLabel.reset(new QLabel(statusBar()));
        statusBar()->addPermanentWidget(_textScaleLabel.get());

        _scZoomInText.reset(new QShortcut(_centralWidget.get()));
        _scZoomInText->setKey(Qt::CTRL + Qt::Key_Plus);
        connect(_scZoomInText.get(), &QShortcut::activated, this, [this](){updateZoomTextAndStatusBar(ZoomType::In);});

        _scZoomOutText.reset(new QShortcut(_centralWidget.get()));
        _scZoomOutText->setKey(Qt::CTRL + Qt::Key_Minus);
        connect(_scZoomOutText.get(), &QShortcut::activated, this, [this](){updateZoomTextAndStatusBar(ZoomType::Out);});

        updateZoomTextAndStatusBar();
    }

    void MainWindow::fillMenu()
    {
        if(_menuBar == nullptr)
            throw std::runtime_error("Не инициализирован _menuBar");

        auto curMenu =_menuBar->addMenu("Файл");
        curMenu->addAction("Создать");
        curMenu->addAction("Открыть");
        curMenu->addAction("Сохранить");
        curMenu->addAction("Сохранить как...");
        curMenu->addAction("Выход", QApplication::quit);

        curMenu = _menuBar->addMenu("Правка");
        curMenu->addAction("Вырезать", _textEditor.get(), &QTextEdit::cut);
        curMenu->addAction("Копировать", _textEditor.get(), &QTextEdit::copy);
        curMenu->addAction("Вставить", _textEditor.get(), &QTextEdit::paste);

        curMenu->addAction("Удалить");
        curMenu->addAction("Выделить все", _textEditor.get(), &QTextEdit::selectAll);

        curMenu = _menuBar->addMenu("Вид");
        {
            auto subMenu = curMenu->addMenu("Масштаб текста");
            subMenu->addAction("Увеличить", this, [&](){updateZoomTextAndStatusBar(ZoomType::In);});
            subMenu->addAction("Уменьшить", this, [&](){updateZoomTextAndStatusBar(ZoomType::Out);});
        }
        curMenu = _menuBar->addMenu("Поиск");
        curMenu->addAction("Найти");
        curMenu->addAction("Замена");

        curMenu = _menuBar->addMenu("Свойства");
        curMenu->addAction("О программе");
    }

    void MainWindow::updateCursorPositionInStatusBar()
    {
        auto cursore = _textEditor->textCursor();
        _cursorePositionLabel->setText("Ln: " + QString::number(cursore.blockNumber() + 1) + ", " +
                                 "Col: " + QString::number(cursore.columnNumber() + 1));
    }

    void MainWindow::updateZoomTextAndStatusBar(ZoomType zoomFunctor)
    {
        switch (zoomFunctor)
        {
        case ZoomType::In:
            _percentZoomText += _stepPercentZoomText;
            _textEditor->zoomIn();
            break;

        case ZoomType::Out:
            _percentZoomText -= _stepPercentZoomText;
            if(_percentZoomText <= _minPercentZoomText)
                _percentZoomText = _minPercentZoomText;
            else
                _textEditor->zoomOut();
            break;

        case ZoomType::None:
            break;
        }

        _textScaleLabel->setText("Масштаб: " + QString::number(_percentZoomText) + "%");
    }

    bool MainWindow::eventFilter(QObject *obj, QEvent *event)
    {
        if(obj == _textEditor.get() && event->type() == QEvent::Wheel)
        {
            auto wheel = static_cast<QWheelEvent*>(event);
            if(wheel->modifiers() == Qt::ControlModifier)
            {
                if(wheel->delta() > 0)
                    updateZoomTextAndStatusBar(ZoomType::In);
                else
                    updateZoomTextAndStatusBar(ZoomType::Out);
                return true;
            }
            else
                return false;
        }
        else
        {
            return QMainWindow::eventFilter(obj, event);
        }
    }
}
