#ifndef SIGNALCLIENT_H
#define SIGNALCLIENT_H

#include <QString>

// interface class to implement in the ptt

class SignalClient
{
public:
    SignalClient();
    virtual ~SignalClient() = 0;

    virtual QString& getName(void) = 0;

};


#endif // SIGNALCLIENT_H
