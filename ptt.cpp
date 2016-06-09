#include "ptt.h"
#include <iostream>
#include <QDebug>


using namespace srpc;

QHBoxLayout ptt::hlayout;


ptt::ptt(const QString name, QObject *parent) : QObject(parent)
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

// nothing interesting here //
void ptt::hPress()
{

}


