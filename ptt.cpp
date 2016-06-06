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

    m_button->setText(m_name);
    m_button->setMinimumHeight(200);
    m_button->setMinimumWidth(200);

    hlayout.addWidget(m_button);
}



ptt::~ptt()
{
}

void ptt::hClick()
{

}


void ptt::hReleased()
{
}

// nothing interesting here //
void ptt::hPress()
{
    // if you want to handle a held button
}




