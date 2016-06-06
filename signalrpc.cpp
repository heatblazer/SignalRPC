// Std headers //
#include <iostream>
#include <stdio.h> // for fprintf //

// Qt headers //
#include <QTcpSocket>

// Local headers //
#include "signalrpc.h"



SignalRPC::SignalRPC(const QString& usr, const QString& pass, QObject* parent) :
    QObject(parent),
    m_state(SignalStates::SRPC_DISCONNECTED),
    m_socket(this),
    m_user(usr),
    m_passwd(pass),
    m_loginTimeout(this),
    m_isOneTime(false)  // check for the init - don`t init 2 times
{
    m_loginTimeout.setSingleShot(true);

    // I am interested IF m_socket was created, I don`t want asserts with    //
    // tryung to access not created socket  or a null pointer                //
    // I will create the socket in the init since constructor can`t return a //
    // value, and I might need to return error if socket can`t be allocated  //
    // handle socket signals then route the AMI //
    connect(&m_socket, SIGNAL(connected()),
            this, SLOT(hConnected()));

    connect(&m_socket, SIGNAL(disconnected()),
            this, SLOT(hDisconnected()));

    connect(&m_socket, SIGNAL(bytesWritten(qint64)),
            this, SLOT(hBytesWritten(qint64)));

    connect(&m_socket, SIGNAL(readyRead()),
            this, SLOT(hReadyRead()));
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
        handleStateChange();
    }

}


void SignalRPC::hConnected()
{
    m_state = SignalStates::SRPC_CONNECTED;
    emit srpcStateChanged(m_state);
}


void SignalRPC::hDisconnected()
{
    m_state = SignalStates::SRPC_DISCONNECTED;
    emit srpcStateChanged(m_state);
}


void SignalRPC::hBytesWritten(qint64 bytes)
{

}


void SignalRPC::hReadyRead()
{
#if 0
    const QString ami_new_line = "\r\n";
    char local_buff[512];
    while (m_socket.canReadLine()) {
        // read all //
        // or if needed read line by line depends what do we need //
        qint64 l = m_socket.readLine(local_buff, 512);
        if (l!= -1) {
            if (local_buff == ami_new_line) {
                handleIncomingMessage();
            } else {
                m_dataBuffer.append(local_buff);
            }
        }
    }
#endif
}


//!
//! \brief Handle all AMI states and route them to the approp functions
//!
//!
void SignalRPC::handleStateChange(void)
{
    switch (m_state)
    {

    }

    emit srpcStateChanged(m_state);
}


