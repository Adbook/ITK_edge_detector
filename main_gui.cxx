

#include <iostream>
#include <QApplication>
#include "mainwindow.hxx"



int main(int argc, char **argv)
{



    QApplication app(argc, argv);
    app.setApplicationName("Edge Detection Filter GUI");

    MainWindow win;
    win.setWindowTitle("Edge Detection");
    win.show();

    return app.exec();
}
