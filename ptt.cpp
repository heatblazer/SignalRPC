#include "ptt.h"

#include <iostream>

#include "logger.h"

#define IS_DIGIT(a) \
    (((a) >= '0') && ((a) <= '9'))

using namespace srpc;

QHBoxLayout ptt::hlayout;
QTimer ptt::m_timeout3;

ptt::ptt(const QString name, VampireResp vs, QObject *parent) : QObject(parent)
, m_vstate(vs)
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


    m_timeout1.setInterval(2000);
    connect(&m_timeout1, SIGNAL(timeout()),
            this, SLOT(handleTimeout1()));
    m_timeout1.start();


    m_timeout2.setInterval(1000);
    connect(&m_timeout2, SIGNAL(timeout()),
            this, SLOT(handleTimeout2()));
    m_timeout2.start();

    // log to file eacch 2 hours
    m_timeout3.setInterval(2*3600000);
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

void ptt::handleTimeout1()
{
    p_srpc->sendCommand("ka\n");
}


void ptt::handleTimeout2()
{
    p_srpc->sendCommand(m_info.m_command);
}

void ptt::handleTimeout3()
{
    QString log;
    log.append("\n---------------------------");
    log.append("Timed log: ");
    log.append(QDateTime::currentDateTime().toString());
    log.append("---------------------------\n");

    logger::logMessage(log);


}

bool ptt::isValidResponseFromVampire(const QString& data)
{
    switch (m_vstate) {
    case KA:
        if (QString::compare(data, "ok\n", Qt::CaseInsensitive)==0) {
            return true;
        }
    case FV:
        if (QString::compare(data, "1.0\n", Qt::CaseInsensitive) == 0) {
            return true;
        }
    case PN:
    case PF:
        if (QString::compare(data, "ok_ptt\n", Qt::CaseInsensitive) == 0) {
            return true;
        }
    case DTMF:
        if (QString::compare(data, "ok_tone\n", Qt::CaseInsensitive)==0){
            return true;
        }
#if 0
        if (IS_DIGIT(data.toLocal8Bit().constData()[0]) &&
            IS_DIGIT(data.toLocal8Bit().constData()[1]) &&
            data.toLocal8Bit().constData()[0] != data.toLocal8Bit().constData()[1]){
            return true;
        }
#endif
    case BUSY:
        if (QString::compare(data, "busy\n", Qt::CaseInsensitive)==0) {
            return true;
        }
    case PARSE_ERROR:
        if (QString::compare(data, "parse error\n", Qt::CaseInsensitive)) {
            return true;
        }

    default:
        return false;
    }
}

// nothing interesting here //
void ptt::hPress()
{

}


