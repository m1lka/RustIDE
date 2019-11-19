#include "mainwindow.h"
#include <QException>
#include <QMenu>
#include <QDebug>
#include <QApplication>

namespace RustIDE
{
    MainWindow::MainWindow(QWidget* parent):
        QMainWindow(parent)
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
        } catch (const std::exception& ex)
        {
            qDebug() << ex.what();
        }

        _boxLayout->setMenuBar(_menuBar.get());
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
        curMenu->addAction("Вырезать");
        curMenu->addAction("Копировать");
        curMenu->addAction("Вставить");
        curMenu->addAction("Удалить");
        curMenu->addAction("Выделить все");

        curMenu = _menuBar->addMenu("Вид");
        curMenu->addAction("Масштаб текста");

        curMenu = _menuBar->addMenu("Поиск");
        curMenu->addAction("Найти");
        curMenu->addAction("Замена");

        curMenu = _menuBar->addMenu("Свойства");
        curMenu->addAction("О программе");
    }
}
