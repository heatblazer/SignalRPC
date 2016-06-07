#include "ptt.h"
#include <iostream>
#include <QDebug>



QHBoxLayout ptt::hlayout;


ptt::ptt(const QString name, QObject *parent) : QObject(parent)
{
    m_name = name;
    m_button = new QPushButton;

    // keep in mind there are 2 signlas to handle choose which one //
    connect(m_button, SIGNAL(clicked(bool)),
            this, SLOT(hClick()));
    connect(m_button, SIGNAL(pressed()),
            this, SLOT(hPress()));
    connect(m_button, SIGNAL(released()),
            this, SLOT(hReleased()));


    m_timeout.setInterval(2500);
    connect(&m_timeout, SIGNAL(timeout()),
            this, SLOT(handleTimeout()));
    m_timeout.start();

    m_button->setText(m_name);
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
    p_srpc->registerClient(this);
}

void ptt::setCommand(const QString &com)
{
    m_command = QString(com);
}

QString &ptt::getName()
{
    return m_name;
}


void ptt::hClick()
{
    p_srpc->sendCommand(m_command);
}


void ptt::hReleased()
{
}

void ptt::handleTimeout()
{
    p_srpc->sendCommand("ka\n");
}

// nothing interesting here //
void ptt::hPress()
{

}


