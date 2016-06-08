#ifndef AMI_H
#define AMI_H

// interface to implement
#include "handlersiface.h"

// Qt headers //
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

namespace srpc {
// interface class for the client and it`s implementation
class SignalClientIface;

class SignalRPC : public QObject, public HandlersIface
{
    Q_OBJECT
// ami states
    enum SignalStates {
        SRPC_DISCONNECTED,
        SRPC_CONNECTED,

        SRPC_READY,
        SRPC_NOT_READY,
    };


public:
    explicit SignalRPC(const QString& host, const QString &port,
                        const QString& usr, const QString& pass,
                        QObject* parent = nullptr);
    virtual ~SignalRPC();

    void init(void);

    void sendCommand(const QString& com);

    virtual void registerClient(SignalClientIface *pcl);

private:
    // I`ll emit these so the client will know I am ready
signals:
    void srpcStateChanged(SignalStates state);

private slots:

// these are to be implemented from Handelr Iface forcibly!
// I want the user to implement these!!!
    virtual void handleMessage(const QString &msg);
    virtual void handleConnected();
    virtual void handleDisconnected();
    virtual void handleBytesWritten(qint64 bytes);
    virtual void handleReadyRead();
    virtual void handleStateChange(void);

private:
    SignalStates    m_state;
    // be careful when using the union setting member 1 will unset member 2

    QTcpSocket*     p_socket;

    // by the polimorhism power I can cast it to upper class
    SignalClientIface*   p_client;

    QString     m_user;
    QString     m_passwd;

    QString     m_uri;
    QString     m_port;

    QTimer      m_loginTimeout; // singleshot

    QString     m_dataBuffer;    // incomming data

    //boolean check for only one init call!
    bool        m_isOneTime;

};
} // namespace srpc

#endif // AMI_H
