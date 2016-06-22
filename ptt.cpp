#include "ptt.h"

#include <iostream>

#include "logger.h"
#define HOURS 1
#define IS_DIGIT(a) \
    (((a) >= '0') && ((a) <= '9'))

using namespace srpc;

QHBoxLayout ptt::hlayout;
QTimer ptt::m_timeout3;
unsigned long long ptt::m_requestCounter = 0;
unsigned long long ptt::m_respCounter = 0;


ptt::ptt(const QString name, int time_ka,
         int time_cmd, QObject *parent) : QObject(parent)

{
    m_info.m_name = name;
    m_info.m_err = m_info.m_disconnects = 0;
    m_button = new QPushButton;

    // keep in mind there are 2 signlas to handle choose which one //
    connect(m_button, SIGNAL(clicked(bool)),
            this, SLOT(hClick()));
    connect(m_button, SIGNAL(pressed()),
            this, SLOT(hPress()));
    connect(m_button, SIGNAL(released()),
            this, SLOT(hReleased()));


    m_timeoutKA.setInterval(time_ka);
    connect(&m_timeoutKA, SIGNAL(timeout()),
            this, SLOT(handleTimeoutKA()));
    m_timeoutKA.start();


    m_timeoutPN.setInterval(time_cmd);
    connect(&m_timeoutPN, SIGNAL(timeout()),
            this, SLOT(handleTimeoutPN()));
    m_timeoutPN.start();

    m_timeoutPF.setInterval(time_cmd*20);
    connect(&m_timeoutPF, SIGNAL(timeout()),
            this, SLOT(handleTimeoutPF()));
    m_timeoutPF.start();


    // log to file eacch 2 hours
    m_timeout3.setInterval(HOURS*3600000);
   // m_timeout3.setInterval(10000);
    connect(&m_timeout3, SIGNAL(timeout()),
            this, SLOT(handleTimeout3()));
    m_timeout3.start();


    m_button->setText(m_info.m_name);
    m_button->setMinimumHeight(200);
    m_button->setMinimumWidth(200);

    hlayout.addWidget(m_button);
}



ptt::~ptt()
{
}


void ptt::registerRpc(SignalRPC *pRpc)
{
    p_srpc = pRpc;
    p_srpc->registerClient((ptt*)this);
}

void ptt::setCommand(const QString &com)
{
    m_info.m_command = QString(com);
}

SignalClientIface* ptt::getClient()
{
    return this;
}

// return a dump of the obj
QString ptt::toString()
{
    QString s;
    s.append("[command: (");
    s.append(m_info.m_command);
    s.append(")]\n");
    s.append("[name: ");
    s.append(m_info.m_name);
    s.append("]\n");
    s.append("[reading errors: ");
    s.append(QString::number(m_info.m_err));
    s.append("]\n");
    s.append("[disconnect errors: ");
    s.append(QString::number(m_info.m_disconnects));
    s.append("]\n");
    return s;
}

QString &ptt::getName()
{
    return this->m_info.m_name;
}


void ptt::hClick()
{
    p_srpc->sendCommand(m_info.m_command);
}


void ptt::hReleased()
{
}

void ptt::handleTimeoutKA()
{
    m_requestCounter++;
    p_srpc->sendCommand("ka\n");
}


void ptt::handleTimeoutPN()
{
    m_requestCounter++;
    p_srpc->sendCommand("pn\n");
}


void ptt::handleTimeoutPF()
{
    m_requestCounter++;
    p_srpc->sendCommand("pf\n");

}

void ptt::handleTimeout3()
{
    QString log;
    log.append("\n---------------------------");
    log.append("Timed log: ");
    log.append(QString("Requests sent for the past 1 hour: %1\n")
               .arg(m_requestCounter));
    log.append(QDateTime::currentDateTime().toString());
    log.append("---------------------------\n");

    logger::logMessage(log);


}

bool ptt::isValidResponseFromVampire(const QString& data)
{

    if (QString::compare(data, "ok_ka\n", Qt::CaseInsensitive)==0) {
        return true;
    }
    if (QString::compare(data, "1.1\n", Qt::CaseInsensitive) == 0) {
        return true;
    }
    if (QString::compare(data, "ok_ptt\n", Qt::CaseInsensitive) == 0) {
        return true;
    }
    if (QString::compare(data, "ok_tone\n", Qt::CaseInsensitive)==0){
        return true;
    }

    if (QString::compare(data, "busy\n", Qt::CaseInsensitive)==0) {
            return true;
    }

    if (QString::compare(data, "busy_ptt\n", Qt::CaseInsensitive)==0){
        return true;
    }

    if (QString::compare(data, "parse error\n", Qt::CaseInsensitive)) {
        return true;
    }

    return false;

}

// nothing interesting here //
void ptt::hPress()
{
    // don`t handle presses for now, maybe closing socket
    p_srpc->closeConnection();
}


