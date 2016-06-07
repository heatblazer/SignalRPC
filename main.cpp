#include <QApplication>
#include <QWidget>
#include <QTcpSocket>
#include <QHBoxLayout>

#include "signalrpc.h"
#include "ptt.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SignalRPC* srpc = new SignalRPC("192.168.32.204",
                                    4000,
                                    "",
                                    "");

    srpc->init();

    ptt p1("KA");
    ptt p2("FV");
    ptt p3("PN");
    ptt p4("PF");


    p1.registerRpc(srpc);
    p2.registerRpc(srpc);
    p3.registerRpc(srpc);
    p4.registerRpc(srpc);


    p1.setCommand("ka\n");
    p2.setCommand("fv\n");
    p3.setCommand("pn\n");
    p4.setCommand("pf\n");


    // get a handle to the ami //

    QWidget mw;
    mw.setLayout((QHBoxLayout*)&ptt::hlayout);
    mw.show();


    return a.exec();
}


