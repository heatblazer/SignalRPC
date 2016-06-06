#include <QApplication>
#include <QWidget>
#include <QTcpSocket>
#include <QHBoxLayout>

#include "signalrpc.h"
#include "ptt.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);




    // create a test ami clients //
    //ptt p1("ptt 1");
    ptt p1("ptt 1");
    ptt p2("ptt 2");
    ptt p3("ptt 3");
    ptt p4("ptt 4");

    // get a handle to the ami //

    QWidget mw;
    mw.setLayout((QHBoxLayout*)&ptt::hlayout);
    mw.show();


    return a.exec();
}


