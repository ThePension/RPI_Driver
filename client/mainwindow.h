#pragma once

#include <QMainWindow>

class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    private:
        QAction *actAbout, *actQuit;
        
    public:
        MainWindow(QWidget *parent = nullptr);

    private:
        void createMenusActions();
};
