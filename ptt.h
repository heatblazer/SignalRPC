#ifndef PTT_H
#define PTT_H
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QObject>

#include "signalrpc.h"

class ptt;

// implplementing the AmiInterface //
class ptt : public QObject, public AmiClient
{
    Q_OBJECT
public:
    explicit ptt(const QString name, QObject* parent = nullptr);
    virtual ~ptt();

    void registerAmi(SignalRPC* pRpc);    //for now register an outside ami //

private slots:
    void hClick(void);
    void hPress(void);
    void hReleased(void);

public:
    static QHBoxLayout hlayout;

private:

    SignalRPC*            p_srpc;
    QPushButton*    m_button; // pedal simulation //
    QString         m_name;
};

#endif // PTT_H
