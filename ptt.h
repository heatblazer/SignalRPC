#ifndef PTT_H
#define PTT_H
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QObject>


#include "defs.h" // for the vmp states
#include "signalrpc.h"
#include "signalclient.h"

namespace srpc {
class ptt;
class SignalRPC;

class ptt : public QObject, public SignalClientIface
{
    Q_OBJECT
public:
    struct {
        QString                 m_name;
        QString                 m_command;
        quint64                 m_err;
        quint64                 m_disconnects;
    } m_info;

public:
                // name of the button // vamp enum  // qparent
    explicit ptt(const QString name, VampireResp vs, QObject* parent = nullptr);
    virtual ~ptt();

    void registerRpc(srpc::SignalRPC* pRpc);    //for now register an outside ami //
    void setCommand(const QString& com);    // register command to the button

    // from signal client
    virtual SignalClientIface* getClient();
    virtual QString    toString(void);

    QString&    getName(void);

private slots:
    void hClick(void);
    void hPress(void);
    void hReleased(void);
    void handleTimeout1(void);
    void handleTimeout2(void);

public:
    // if the response from the vmp is good
    bool isValidResponseFromVampire(const QString& data);

public:
    static QHBoxLayout hlayout;


private:

    // private field for inner classes

private:

    SignalRPC*              p_srpc;
    QPushButton*            m_button;

    QTimer                  m_timeout1;
    QTimer                  m_timeout2;

    VampireResp             m_vstate;

};

} // namespace srpc
#endif // PTT_H
