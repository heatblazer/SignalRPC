#include "logger.h"


QFile logger::m_logfile;

void logger::logMessage(const QString &msg)
{
    static bool isOnetime = false;
    QString txt;

    if(!isOnetime) {
        isOnetime = true;
        QDateTime date;
        (void) date;
        QString lname("log_");
        lname.append(date.currentDateTime().toString());
        m_logfile.setFileName(lname);
    }

    txt = msg;
    if (!m_logfile.isOpen()) {
        m_logfile.open(QIODevice::WriteOnly|QIODevice::Append);
    }
    QTextStream ts(&m_logfile);

    ts << txt << endl << flush;

}
