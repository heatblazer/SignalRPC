#ifndef AMI_H
#define AMI_H

// Qt headers //
#include <QObject>
#include <QTcpSocket>
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
    explicit SignalRPC(const QString& host, const qint16 port,
                        const QString& usr, const QString& pass,
                        QObject* parent = nullptr);
    virtual ~SignalRPC();

    void init(void);
    void sendCommand(const QString& com);
private:


    // I`ll emit these so the client will know I am ready
signals:
    void srpcStateChanged(SignalStates state);

private slots:
    void hMessage(const QString &msg);
    void hConnected();
    void hDisconnected();
    void hBytesWritten(qint64 bytes);
    void hReadyRead();

// handle all changes in the state and route specific actions
    void handleStateChange(void);

private:
    SignalStates    m_state;
    // be careful when using the union setting member 1 will unset member 2

    QTcpSocket* p_socket;

    QString     m_user;
    QString     m_passwd;

    QString     m_uri;
    quint16     m_port;

    QTimer      m_loginTimeout; // singleshot

    QString     m_dataBuffer;    // incomming data

    //boolean check for only one init call!
    bool        m_isOneTime;

};

#endif // AMI_H
