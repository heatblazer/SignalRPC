#ifndef LOGGER_H
#define LOGGER_H
#include <QtCore>


#define BEGIN_LOG "\n-------- B E G I N --------\n"
#define END_LOG "\n-------- E N D ------------\n"


class logger
{
public:
    static void logMessage(const QString& msg);


private:

    static QFile  m_logfile;
};


#endif // LOGGER_H
