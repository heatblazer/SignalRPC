// Std headers //
#include <iostream>
#include <stdio.h> // for fprintf //

// Qt headers //
#include <QTcpSocket>

// Local headers //
#include "ptt.h"
#include "signalclient.h"
#include "signalrpc.h"

// logger //
#include "logger.h"



namespace srpc {

SignalRPC::SignalRPC(const QString &host,
                     const QString& port,
                     const QString& usr,
                     const QString& pass, QObject* parent) :

    QObject(parent),
    m_state(SignalStates::SRPC_DISCONNECTED),
    p_socket(nullptr),
    m_uri(host),
    m_port(port),
    m_user(usr),    // unused
    m_passwd(pass), // unused
    m_loginTimeout(this),
    m_isOneTime(false)  // check for the init - don`t init 2 times
{
    m_loginTimeout.setSingleShot(true);

}


SignalRPC::~SignalRPC()
{

}

// can`t call init more than once
// if you want to do so, rename init to reinit and remove the guard
void SignalRPC::init(void)
{
    // check the state for furst time
    // never call init 2 times - move the bool var in the class
    if (!m_isOneTime) {
        m_isOneTime = true;
        p_socket = new QTcpSocket(this);
        if (p_socket == nullptr) {

        } else {
            m_state = SignalStates::SRPC_DISCONNECTED;
            p_socket->connectToHost(m_uri, (quint16)m_port.toInt());

           // connect(p_socket, SIGNAL(error(QAbstractSocket::SocketError)),
           //         this, SLOT(handleMessage(QString)));

            // handle QTcpSocket states if needed
            connect(p_socket,
                    SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                    this, SLOT(handleSocketState()));

            connect(p_socket, SIGNAL(connected()),
                    this, SLOT(handleConnected()));

            connect(p_socket, SIGNAL(disconnected()),
                    this, SLOT(handleDisconnected()));

            connect(p_socket, SIGNAL(bytesWritten(qint64)),
                    this, SLOT(handleBytesWritten(qint64)));

            connect(p_socket, SIGNAL(readyRead()),
                    this, SLOT(handleReadyRead()));
             handleStateChange();
        }
    }
}

void SignalRPC::registerClient(SignalClientIface *pcl)
{
    if (pcl != nullptr) {
        p_client = pcl;
    }
}

void SignalRPC::sendCommand(const QString &com)
{
    // a good check for the command in the final release is good
    if (p_socket->state() == QTcpSocket::ConnectedState) {
        if (p_socket->write(com.toLocal8Bit().constData()) !=
                com.size() )
        {
            ((ptt*)p_client->getClient())->m_info.m_err++;
            QString errlog = ((ptt*)p_client->getClient())->toString();
            logger::logMessage(BEGIN_LOG);
            logger::logMessage("Could not write to socket... \n");
            logger::logMessage(errlog.toLocal8Bit().constData());
            logger::logMessage(END_LOG);
        }
    } else {
        ((ptt*)p_client->getClient())->m_info.m_err++;
        QString errlog = ((ptt*)p_client->getClient())->toString();
        logger::logMessage(BEGIN_LOG);
        logger::logMessage("Socket is not connected... \n");
        logger::logMessage(errlog.toLocal8Bit().constData());
        logger::logMessage(END_LOG);
        m_state = SignalStates::SRPC_DISCONNECTED;
        handleStateChange();
    }
}


void SignalRPC::handleConnected()
{
    m_state = SignalStates::SRPC_CONNECTED;
    std::cout << "Connected!" << std::endl;
    emit srpcStateChanged(m_state);

}


void SignalRPC::handleDisconnected()
{
    m_state = SignalStates::SRPC_DISCONNECTED;

    ((ptt*)p_client->getClient())->m_info.m_disconnects++;
    QString msg(BEGIN_LOG);
    msg.append("Connection lost!\n");
    QString s = ((ptt*)p_client->getClient())->toString();
    msg.append(s);
    msg.append(END_LOG);
    logger::logMessage(msg);

    handleStateChange();
}


void SignalRPC::handleBytesWritten(qint64 bytes)
{
    // view the bytes written
    m_state = SignalStates::SRPC_READY;
    if (bytes < ((ptt*)p_client->getClient())->m_info.m_command.size()) {
        ((ptt*)p_client->getClient())->m_info.m_err++;
        QString errlog = ((ptt*)p_client->getClient())->toString();
        logger::logMessage(BEGIN_LOG);
        logger::logMessage("Could not write bytes... \n");
        logger::logMessage(errlog.toLocal8Bit().constData());
        logger::logMessage(END_LOG);
    }
    emit srpcStateChanged(m_state);
}


void SignalRPC::handleMessage(const QString& msg)
{
    // something to do to handle the received message
}

void SignalRPC::handleReadyRead()
{
    if (p_socket->canReadLine()) { // if I can read a line
        // this will get me a ptr to the implemented class by the force
        // of C++ polymorphism
        std::cout << ((ptt*)p_client->getClient())->getName().toStdString()
                  << std::endl;
        // I will read all that came to me also I need a better check
        // what I `ve read
        QByteArray b = p_socket->readLine();
        if (b.size() <= 0) {
            ((ptt*)p_client->getClient())->m_info.m_err++;
            QString errlog = ((ptt*)p_client->getClient())->toString();
            logger::logMessage(BEGIN_LOG);
            logger::logMessage("Socket read no data! \n");
            logger::logMessage(errlog.toLocal8Bit().constData());
            logger::logMessage(END_LOG);
        } else {
            // add new check to view a valid response
            // preserve the newline
            std::cout << b.toStdString() << std::endl;

            if (((ptt*)p_client)->isValidResponseFromVampire(b)) {

            } else {
                ((ptt*)p_client->getClient())->m_info.m_err++;
                QString errlog = ((ptt*)p_client->getClient())->toString();
                logger::logMessage(BEGIN_LOG);
                logger::logMessage("Read invalid data from socket\n");
                logger::logMessage(errlog.toLocal8Bit().constData());
                logger::logMessage(END_LOG);
            }
        }
        // if needed I can handle messages
    } else {
        ((ptt*)p_client->getClient())->m_info.m_err++;
        QString errlog = ((ptt*)p_client->getClient())->toString();
        logger::logMessage(BEGIN_LOG);
        logger::logMessage("Socket colud not read a response line\n");
        logger::logMessage(errlog.toLocal8Bit().constData());
        logger::logMessage(END_LOG);
    }
}


//!
//! \brief Handle all AMI states and route them to the approp functions
//!
//!
void SignalRPC::handleStateChange(void)
{

    switch (m_state)
    {
    // retry connection
    case SignalStates::SRPC_DISCONNECTED:

        p_socket->connectToHost(m_uri, (quint16) m_port.toUInt());
        if (p_socket->state() != QTcpSocket::ConnectedState) {
            m_state = SignalStates::SRPC_DISCONNECTED;
        } else {
            m_state = SignalStates::SRPC_CONNECTED;
        }

        emit srpcStateChanged(m_state);
        break;
    case SignalStates::SRPC_CONNECTED:
        // do something
        break;
    case SignalRPC::SRPC_NOT_READY:
        // do something
        break;
    case SignalRPC::SRPC_READY:
        // do something
        break;
    default:
        break;
    }

    emit srpcStateChanged(m_state);
}


/** handle the socket state if needed for extra precaution
 * @brief SignalRPC::handleSocketState
 */
void SignalRPC::handleSocketState()
{
    switch (p_socket->state())
    {
    case QTcpSocket::UnconnectedState:
        break;
    case QTcpSocket::HostLookupState:
        break;
    case QTcpSocket::ConnectingState:
        break;
    case QTcpSocket::ConnectedState:
        break;
    case QTcpSocket::BoundState:
        break;
    case QTcpSocket::ListeningState:
        break;
    case QTcpSocket::ClosingState:
        break;
    }
}


} // namespace srpc

