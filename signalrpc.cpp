// Std headers //
#include <iostream>
#include <stdio.h> // for fprintf //

// Qt headers //
#include <QTcpSocket>

// Local headers //
#include "signalclient.h"
#include "signalrpc.h"



SignalRPC::SignalRPC(const QString &host,
                     const qint16 port,
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
            p_socket->connectToHost(m_uri, m_port);
           // handleStateChange();


            connect(p_socket, SIGNAL(connected()),
                    this, SLOT(hConnected()));

            connect(p_socket, SIGNAL(disconnected()),
                    this, SLOT(hDisconnected()));

            connect(p_socket, SIGNAL(bytesWritten(qint64)),
                    this, SLOT(hBytesWritten(qint64)));

            connect(p_socket, SIGNAL(readyRead()),
                    this, SLOT(hReadyRead()));

        }
    }
}

void SignalRPC::registerClient(SignalClient *pcl)
{
    if (pcl != nullptr) {
        p_client = pcl;
    }
}

void SignalRPC::sendCommand(const QString &com)
{
    p_socket->write(com.toLocal8Bit().constData());
}


void SignalRPC::hConnected()
{
    m_state = SignalStates::SRPC_CONNECTED;
    p_socket->write("fv\n");
    emit srpcStateChanged(m_state);

}


void SignalRPC::hDisconnected()
{
    m_state = SignalStates::SRPC_DISCONNECTED;
    std::cout << "Connection lost...\n";
    handleStateChange();
}


void SignalRPC::hBytesWritten(qint64 bytes)
{
    // view the bytes written
}


void SignalRPC::hMessage(const QString& msg)
{
    //
}

void SignalRPC::hReadyRead()
{
    if (p_socket->canReadLine()) {
        std::cout << p_client->getName().toStdString() << std::endl;
        std::cout << p_socket->readAll().toStdString() << std::endl;
    }
}


//!
//! \brief Handle all AMI states and route them to the approp functions
//!
//!
void SignalRPC::handleStateChange(void)
{
    // nothing for now ...
    switch (m_state)
    {
    // retry connection
    case SignalStates::SRPC_DISCONNECTED:
        p_socket->connectToHost(m_uri, m_port);
        break;
    }

    emit srpcStateChanged(m_state);
}


