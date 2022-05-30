#include "mainwindow.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->client = new Client(this);
    setCentralWidget(this->client);

    setWindowTitle("LinEmb");
    resize(1000, 600);

    createMenusActions();
}

void MainWindow::createMenusActions()
{
    actQuit = new QAction(tr("&Quitter"), this);
    actAbout = new QAction(tr("A &Propos"), this);

    QMenu* menuHelp = menuBar()->addMenu(tr("&?"));
    menuHelp->addAction(actAbout);

    connect(actQuit, &QAction::triggered, this, &QApplication::quit);
}