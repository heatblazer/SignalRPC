#include "ami.h"

// Std headers //
#include <iostream>
#include <stdio.h> // for fprintf //

// Qt headers //
#include <QTcpSocket>

// Local headers //
#include "amiaction.h"
#include "amiclient.h"

Ami::Ami(const QString& amiUser, const QString& amiPass, QObject* parent) :
    QObject(parent),
    m_state(AmiState::AMI_DISCONNECTED),
    m_socket(this),
    m_amiUser(amiUser),
    m_amiPass(amiPass),
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


Ami::~Ami()
{
    // TODO remove all Actions from the m_actions
}

// can`t call init more than once
// if you want to do so, rename init to reinit and remove the guard
void Ami::init(void)
{
    // check the state for furst time
    // never call init 2 times - move the bool var in the class
    if (!m_isOneTime) {
        m_isOneTime = true;
        stateChange();
    }

}


void Ami::hConnected()
{
    m_state = AmiState::AMI_CONNECTED;
    stateChange();
}


void Ami::hDisconnected()
{
    m_state = AmiState::AMI_DISCONNECTED;
    stateChange();
}


void Ami::hBytesWritten(qint64 bytes)
{
}



// sybira redowe v bufera.
// kogato cql msg e koletnat izwikwa hadleNewMsg
void Ami::hReadyRead()
{

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
}


void Ami::hLoginTimeout()
{
    m_state = AmiState::AMI_DISCONNECTED;
    m_socket.abort();
    stateChange();
}

///
/// \brief AMI::sendAction Seriaalizira Act.masg po mrevata
/// \param act
/// \return -1 ako stane gre[ka, ina`a 0
///
int Ami::sendAction(AmiAction *act)
{
    if (act == nullptr) {
        return -1;
    }

    if (m_state != AMI_READY) {
        return -1;
    }

    QByteArray data_for_send(act->getCallMsg().toString().toUtf8());
    data_for_send.append("\r\n");

    qint64 bytes = m_socket.write(data_for_send);
    if (bytes < data_for_send.size()) {
        // не мога да  пиша по мрежата.
        // дисконецтвам за да инициализирам всичко от начало
        m_socket.abort();
        m_state = AMI_DISCONNECTED;
        return -1;
    }

    m_actions.insert(act);
    std::cout << "sendAction:" << data_for_send.constData();
    return 0;
}


//!
//! \brief Handle all AMI states and route them to the approp functions
//!
//!
// wikaj tva navsqkyde kydeto smenqt m_state
void Ami::stateChange(void)
{
    switch (m_state)
    {
    case AmiState::AMI_CONNECTED:
        std::cout << "AMI CONNECTED\n";
        login();
        break;

    case AmiState::AMI_READY:
        std::cout << "AMI READY\n";
        // ready to send actions
        break;

    case AmiState::AMI_NOT_READY:
        std::cout << "AMI NOT READY\n";
        break;

    case AmiState::AMI_DISCONNECTED:
        std::cout << "AMI DISCONNECTED\n";
        m_socket.connectToHost("192.168.32.89", 5038);
        break;
    }
    emit amiStateChanged(m_state);
}


void Ami::login()
{
    QString login_str = QString("Action: Login\nUsername: %1\nSecret: %2\n\n")
            .arg(m_amiUser).arg(m_amiPass);

    m_loginTimeout.start(2000);

    qint64 res = m_socket.write(login_str.toLocal8Bit());
    if ((res < 0) || (res < login_str.length())) {
        // cant write all data
        m_state = AmiState::AMI_DISCONNECTED;
        m_socket.abort();

        stateChange();
    }
}


void Ami::handleIncomingMessage()
{
    if (m_state == AMI_READY) {
        // this is user message
        // TODO(goro,ilian) OK, we need to add more failsafe cheks...
        AmiMsg* incoming = new AmiMsg(m_dataBuffer);
        const QString action_id = incoming->actionID();
// testov blok
        {
            MsgType cmd = incoming->amiType();

        }


// Pending deprecation ...
#if 0
        // намери прилежащия Action
        QSet<AmiAction*>::iterator i;
        for(i = m_actions.begin(); i != m_actions.end(); ++i) {
            if ((*i)->getCallMsg().actionID() == action_id) {
                (*i)->p_respondMsg = incoming;  // attach  the respond
                (*i)->p_callee->handleAmiRespond(*i);

            }
        }
#endif
        // imporved the set iteration, it`s simplier and more intuitive
        QSetIterator<AmiAction*> it(m_actions);
        while (it.hasNext()) {
            AmiAction* curr = it.next();
            if (curr->getCallMsg().actionID() == action_id) {
                curr->p_respondMsg = incoming;
                curr->p_callee->handleAmiRespond(curr);
                AmiAction *pdel = curr;
                m_actions.remove(curr);
                delete pdel;
                curr = nullptr; // good practices to null it explicitly
            }
        }
    } else {    // AMI state not ready ...
        // this is posibly my login
        // TODO(goro,ilian) beter validation
        if (m_dataBuffer.contains("Authentication accepted")) {
            m_state = AMI_READY;
            stateChange();
        }
    }

    m_dataBuffer.clear();
}


