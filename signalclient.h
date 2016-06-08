#ifndef SIGNALCLIENT_H
#define SIGNALCLIENT_H

#include <QString>

// interface class to implement in the ptt
namespace srpc {

class SignalClientIface
{
public:
    SignalClientIface();
    virtual ~SignalClientIface() = 0;
    virtual SignalClientIface* getClient(void) = 0;

};

} // namespace srpc

#endif // SIGNALCLIENT_H
