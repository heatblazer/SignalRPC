#ifndef AMI_H
#define AMI_H

// Qt headers //
#include <QObject>

//use the union rule
#include <QTcpSocket>
#include <QUdpSocket>

#include <QTimer>


class SignalRPC : public QObject
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
    explicit SignalRPC(const QString& amiUser, const QString& amiPass,
                 QObject* parent = nullptr);
    virtual ~SignalRPC();

    void init(void);

private:
    void login();
    void handleIncomingMessage();

    // I`ll emit these so the client will know I am ready
signals:
    void srpcStateChanged(SignalStates state);

private slots:
    void hConnected();
    void hDisconnected();
    void hBytesWritten(qint64 bytes);
    void hReadyRead();
    void hLoginTimeout();

// handle all changes in the state and route specific actions
    void handleStateChange(void);

private:
    SignalRPC    m_state;
    // be careful when using the union setting member 1 will unset member 2

    union {
        QTcpSocket  tcp;
        QUdpSocket udp;
    } m_socket;

    QString     m_user;
    QString     m_passwd;
    QTimer      m_loginTimeout; // singleshot

    QString     m_dataBuffer;    // incomming data

    //boolean check for only one init call!
    bool        m_isOneTime;

};

#endif // AMI_H
