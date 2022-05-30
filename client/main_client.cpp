/**
 * @file   main.cpp
 * @author Lucas Gosteli et Nicolas Aubert
 * @date   24.05.2022
 * @version 0.1
 * @brief  A Linux user space program that communicates with the driver.
 * It reads the data from the driver and display them
 * Driver is located in /dev/drvTest
*/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app (argc, argv);

	MainWindow w;
    w.show();

	return app.exec();
}