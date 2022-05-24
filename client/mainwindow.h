#pragma once

#include <QMainWindow>

#include "client.h"

class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    private:
        QAction *actAbout, *actQuit;
        Client * client = nullptr;

    public:
        MainWindow(QWidget *parent = nullptr);

    private:
        void createMenusActions();
};
