#include "mainwindow.h"
#include <QException>
#include <QMenu>
#include <QDebug>
#include <QApplication>
#include <QStatusBar>
#include <QWheelEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

namespace RustIDE
{
    MainWindow::MainWindow(QWidget* parent):
        QMainWindow(parent),
        _minZoomPercent(20),
        _stepZoomPercent(10),
        _newFilename("new"),
        _currentZoomPercent(100),
        _windowTitle("RustIDE"),
        _currentFileSaved(true)
    {
        this->setMinimumSize(800, 600);
        this->setWindowTitle(_windowTitle);

        _settingsFilename = QString(QApplication::applicationDirPath() + "/settings.ini");
        assembleInterface();

        if (loadSettings())
            openFileForName(_currentFileName);
        else
            createNewFile();

        updateWindowTitle(true);
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
        connect(_textEditor.get(), &QTextEdit::textChanged, this, [&](){ updateWindowTitle(false); });

        setupBaseShortcut();
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
        connect(_scZoomInText.get(), &QShortcut::activated, this, [this](){ updateZoomAndStatusBar(ZoomType::In); });

        _scZoomOutText.reset(new QShortcut(_centralWidget.get()));
        _scZoomOutText->setKey(Qt::CTRL + Qt::Key_Minus);
        connect(_scZoomOutText.get(), &QShortcut::activated, this, [this](){ updateZoomAndStatusBar(ZoomType::Out); });

        updateZoomAndStatusBar();
    }

    void MainWindow::setupBaseShortcut()
    {
        _scCreateNewFile.reset(new QShortcut(_centralWidget.get()));
        _scCreateNewFile->setKey(Qt::CTRL + Qt::Key_T);
        connect(_scCreateNewFile.get(), &QShortcut::activated, this, &MainWindow::createNewFile);

        _scOpenFile.reset(new QShortcut(_centralWidget.get()));
        _scOpenFile->setKey(Qt::CTRL + Qt::Key_O);
        connect(_scOpenFile.get(), &QShortcut::activated, this, &MainWindow::openFile);

        _scSaveFile.reset(new QShortcut(_centralWidget.get()));
        _scSaveFile->setKey(Qt::CTRL + Qt::Key_S);
        connect(_scSaveFile.get(), &QShortcut::activated, this, &MainWindow::saveFile);

        _scSaveFileAs.reset(new QShortcut(_centralWidget.get()));
        _scSaveFileAs->setKey(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
        connect(_scSaveFileAs.get(), &QShortcut::activated, this, &MainWindow::saveFileAs);

        _scUndo.reset(new QShortcut(_centralWidget.get()));
        _scUndo->setKey(Qt::CTRL + Qt::Key_Z);
        connect(_scUndo.get(), &QShortcut::activated, _textEditor.get(), &QTextEdit::undo);

        _scRedo.reset(new QShortcut(_centralWidget.get()));
        _scRedo->setKey(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
        connect(_scRedo.get(), &QShortcut::activated, _textEditor.get(), &QTextEdit::undo);
    }

    void MainWindow::fillMenu()
    {
        if(_menuBar == nullptr)
            throw std::runtime_error("Не инициализирован _menuBar");

        auto curMenu =_menuBar->addMenu("Файл");
        curMenu->addAction("Создать", this, &MainWindow::createNewFile);
        curMenu->addAction("Открыть", this, &MainWindow::openFile);
        curMenu->addAction("Сохранить", this, &MainWindow::saveFile);
        curMenu->addAction("Сохранить как...", this, &MainWindow::saveFileAs);
        curMenu->addAction("Закрыть", this, &MainWindow::closeFile);
        curMenu->addAction("Выход", this, [&]() { exitApp(); });

        curMenu = _menuBar->addMenu("Правка");
        curMenu->addAction("Отменить", _textEditor.get(), &QTextEdit::undo);
        curMenu->addAction("Вернуть", _textEditor.get(), &QTextEdit::redo);
        curMenu->addSeparator();
        curMenu->addAction("Вырезать", _textEditor.get(), &QTextEdit::cut);
        curMenu->addAction("Копировать", _textEditor.get(), &QTextEdit::copy);
        curMenu->addAction("Вставить", _textEditor.get(), &QTextEdit::paste);

        curMenu->addAction("Удалить");
        curMenu->addAction("Выделить все", _textEditor.get(), &QTextEdit::selectAll);

        curMenu = _menuBar->addMenu("Вид");
        {
            auto subMenu = curMenu->addMenu("Масштаб текста");
            subMenu->addAction("Увеличить", this, [&](){ updateZoomAndStatusBar(ZoomType::In); });
            subMenu->addAction("Уменьшить", this, [&](){ updateZoomAndStatusBar(ZoomType::Out); });
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

    void MainWindow::updateZoomAndStatusBar(ZoomType zoomFunctor)
    {
        switch (zoomFunctor)
        {
        case ZoomType::In:
            _currentZoomPercent += _stepZoomPercent;
            _textEditor->zoomIn();
            break;

        case ZoomType::Out:
            _currentZoomPercent -= _stepZoomPercent;
            if(_currentZoomPercent <= _minZoomPercent)
                _currentZoomPercent = _minZoomPercent;
            else
                _textEditor->zoomOut();
            break;

        case ZoomType::None:
            break;
        }

        _textScaleLabel->setText("Масштаб: " + QString::number(_currentZoomPercent) + "%");
    }

	void MainWindow::saveSettings()
	{
        if(_currentFileName == _newFilename)
            return;

        QSettings settings(_settingsFilename, QSettings::IniFormat);
        settings.setValue("lastOpenedFilename", _currentFileName);
	}

	bool MainWindow::loadSettings()
	{
        QSettings settings(_settingsFilename, QSettings::IniFormat);
		_currentFileName = settings.value("lastOpenedFilename").toString();
		return !_currentFileName.isEmpty();
	}

    void MainWindow::openFileForName(QString filename)
	{
		QFile file(filename);
		_currentFileName = filename;

		if (!file.open(QIODevice::ReadOnly | QFile::Text))
		{
			QMessageBox::warning(this, "Предупреждение", "Невозможно открыть файл: " + file.errorString());
			_currentFileName.clear();
			return;
		}

		QTextStream in(&file);
		QString text = in.readAll();
		_textEditor->setText(text);
		file.close();
	}


    bool MainWindow::eventFilter(QObject *obj, QEvent *event)
    {
        if(obj == _textEditor.get() && event->type() == QEvent::Wheel)
        {
            auto wheel = static_cast<QWheelEvent*>(event);
            if(wheel->modifiers() == Qt::ControlModifier)
            {
                if(wheel->delta() > 0)
                    updateZoomAndStatusBar(ZoomType::In);
                else
                    updateZoomAndStatusBar(ZoomType::Out);

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

    bool MainWindow::event(QEvent *event)
    {
        if(event->type() == QEvent::Close)
        {
            exitApp(event);
            return true;
        }

        return QMainWindow::event(event);
    }

    void MainWindow::updateWindowTitle(bool currentFileSaved)
    {
        _currentFileSaved = currentFileSaved;

        auto newTitle = _windowTitle + ": [" + _currentFileName +
                (!_currentFileSaved? "*]": "]");

        this->setWindowTitle(newTitle);
    }

    void MainWindow::createNewFile()
    {
        // пока что так

        if(!_currentFileSaved)
        {
            saveFile();
        }

        _textEditor->clear();
        _currentFileName = _newFilename;

        updateWindowTitle();
    }

    void MainWindow::openFile()
    {
        if(!_currentFileSaved)
            saveFile();

        QString filename = QFileDialog::getOpenFileName(this, "Открыть файл");

        openFileForName(filename);

        updateWindowTitle();
    }

    void MainWindow::saveFile()
    {
        if(_currentFileName == _newFilename)
        {
            saveFileAs();
            return;
        }

        QString fileName;

        if (_currentFileName.isEmpty()) {
            fileName = QFileDialog::getSaveFileName(this, "Сохранить");
            _currentFileName = fileName;
        } else {
            fileName = _currentFileName;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "Предупреждение", "Невозможно сохранить файл: " + file.errorString());
            return;
        }

        QTextStream out(&file);
        QString text = _textEditor->toPlainText();
        out << text;

        file.close();

        updateWindowTitle();
    }

    void MainWindow::saveFileAs()
    {
        if(_textEditor->toPlainText().size() == 0)
            return;

        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить как...");
        QFile file(fileName);

        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "Предупреждение", "Невозможно сохранить файл: " + file.errorString());
            return;
        }

        _currentFileName = fileName;
        QTextStream out(&file);
        QString text = _textEditor->toPlainText();
        out << text;
        file.close();

        updateWindowTitle();
    }

    void MainWindow::closeFile()
    {
        saveFile();

        _textEditor->setText("");
        _currentFileName = _newFilename;
        updateWindowTitle(true);
    }

    void MainWindow::exitApp(QEvent *event)
    {
        if(!_currentFileSaved)
        {
            QMessageBox question(_centralWidget.get());
            question.setText("Выход");
            question.setInformativeText("Вы хотите сохранить изменения в файле?");
            question.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            question.setIcon(QMessageBox::Information);
            question.setDefaultButton(QMessageBox::Cancel);
            auto res = question.exec();

            switch (res)
            {
            case QMessageBox::Yes:
                saveFile();
                break;
            case QMessageBox::No:
                break;
            case QMessageBox::Cancel:
                if(event)
                    event->ignore();
                return;
            }
        }

		saveSettings();
        QApplication::quit();
    }
}
