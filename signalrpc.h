#ifndef AMI_H
#define AMI_H

// Qt headers //
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class AmiAction;

class Ami : public QObject
{
    Q_OBJECT
// ami states
    enum AmiState {
        AMI_DISCONNECTED,   // tcp/tls soket is disconnected
        AMI_CONNECTED,      // tcp/tls socket is connected

        AMI_READY,          // ami login is passed an ready for actions
        AMI_NOT_READY,      // ami login not passed
    };


public:
    explicit Ami(const QString& amiUser, const QString& amiPass,
                 QObject* parent = nullptr);
    virtual ~Ami();

    void init(void);
    int sendAction(AmiAction* sendAction);

private:
    void login();
    void handleIncomingMessage();

    // I`ll emit these so the client will know I am ready
signals:
// if we want to notify the world about this, also emit it where needed
    void amiStateChanged(AmiState state);

private slots:
    void hConnected();
    void hDisconnected();
    void hBytesWritten(qint64 bytes);
    void hReadyRead();
    void hLoginTimeout();

// handle all changes in the state and route specific actions
    void stateChange(void);


private:
    AmiState    m_state;
    QTcpSocket  m_socket;
    QString     m_amiUser;
    QString     m_amiPass;
    QTimer      m_loginTimeout; // singleshot

    QString     m_dataBuffer;    // incomming data
    QSet<AmiAction*> m_actions;

    //boolean check for only one init call!
    bool        m_isOneTime;

};

#endif // AMI_H
