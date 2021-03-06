/* Bidekkida - Log file anonymizer
 *
 * Copyright (c) 2017 Buschtrommel/Matthias Fehring <kontakt@buschmann23.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANONYMIZER_H
#define ANONYMIZER_H

#include <QTextStream>
#include <QRegularExpression>
#include <QFile>
#include <QObject>
#include <QMap>

extern "C" {
#include <syslog.h>
}

class QSocketNotifier;

class Anonymizer : public QObject
{
    Q_OBJECT
public:
    Anonymizer(const QString &outputFileName, const QString &ipregex, QObject* parent = nullptr);

    enum Backend {
        File = 0,
        Syslog = 1,
        Journal = 2
    };

    enum Priority {
        Emergency = LOG_EMERG,
        Alert = LOG_ALERT,
        Critical = LOG_CRIT,
        Error = LOG_ERR,
        Warning = LOG_WARNING,
        Notice = LOG_NOTICE,
        Informational = LOG_INFO,
        Debug = LOG_DEBUG
    };

    ~Anonymizer();

    bool run();

    void setBackend(Backend backend);
    void setBackend(const QString &_backend);
    void setIdentifier(const QString &identifier);
    void setPriority(Priority priority);
    void setPriority(const QString &_priority);
    void setPriorityRegex(const QString &regex);
    void setPriorityMap(const QString &prios);

private slots:
    void dataAvailable();

private:
    QFile m_stdin;
    QFile m_outputFile;
    QSocketNotifier *m_notifier = nullptr;
    QTextStream m_outStream;
    QRegularExpression m_ipregex;
    QRegularExpression m_prioregex;
    Backend m_backend = File;
    QString m_identifier;
    Priority m_priority = Informational;
    QMap<QString,int> m_prioMap;
    bool m_anonymizeIp = false;
    bool m_extractPrio = false;
};

#endif // ANONYMIZER_H
