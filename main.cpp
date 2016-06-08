#include <QApplication>
#include <QWidget>
#include <QTcpSocket>
#include <QHBoxLayout>

#include "signalrpc.h"
#include "ptt.h"



int main(int argc, char *argv[])
{
    using namespace srpc;

    QApplication a(argc, argv);

  //  srpc::SignalRPC* srpc1 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc2 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc3 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc4 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc5 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc6 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc7 = new srpc::SignalRPC("192.168.32.204","4000","","");

    srpc::SignalRPC* srpc8 = new srpc::SignalRPC("192.168.32.204", "4000", "","");

  //  srpc1->init();
    srpc2->init();
    srpc3->init();
    srpc4->init();
    srpc5->init();
    srpc6->init();
    srpc7->init();
    srpc8->init();

 //   ptt p1("KA");
    ptt p2("FV");
    ptt p3("PN");
    ptt p4("PF");

    ptt p5("12");
    ptt p6("23");
    ptt p7("34");

    ptt p8("parse error"); // expect parse error


  //  p1.registerRpc(srpc1);
    p2.registerRpc(srpc2);
    p3.registerRpc(srpc3);
    p4.registerRpc(srpc4);
    p5.registerRpc(srpc5);
    p6.registerRpc(srpc6);
    p7.registerRpc(srpc7);
    p8.registerRpc(srpc8);



   // p1.setCommand("ka\n");
    p2.setCommand("fv\n");
    p3.setCommand("pn\n");
    p4.setCommand("pf\n");
    p5.setCommand("12\n");
    p6.setCommand("23\n");
    p7.setCommand("34\n");
    p8.setCommand("asdasdsa\n");


    QWidget mw;
    mw.setLayout((QHBoxLayout*)&ptt::hlayout);
   // mw.show();


    return a.exec();
}


