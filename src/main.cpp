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

#include <QCoreApplication>
#include <QSettings>

#include <signal.h>

#include "config.h"
#include "anonymizer.h"

void catchUnixSignals(const std::vector<int> &quitSignals, const std::vector<int> &ignoreSignals = std::vector<int>())
{
    auto quitHandler = [](int sig) -> void {
        qDebug("Stopping bidekkida (user requested signal %d)", sig);
        QCoreApplication::quit();
    };

    for (int sig : ignoreSignals) {
        signal(sig, SIG_IGN);
    }

    for (int sig : quitSignals) {
        signal(sig, quitHandler);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});

    app.setOrganizationName(QStringLiteral("Buschtrommel"));
    app.setOrganizationDomain(QStringLiteral("buschmann23.de"));
    app.setApplicationName(QStringLiteral("bidekkida"));
    app.setApplicationVersion(QStringLiteral(BIDEKKIDA_VERSION));

    QSettings *settings = new QSettings(QStringLiteral(BIDEKKIDA_CONFDIR) + QLatin1String("/bidekkida.conf"), QSettings::IniFormat);

    const QString group = (argc > 1) ? QString::fromLatin1(argv[1]) : QStringLiteral("default");

    settings->beginGroup(group);
    const QString logFileName = settings->value(QStringLiteral("logfile"), QStringLiteral("/var/log/apache2/access_log")).toString();
    const QString ipregex = settings->value(QStringLiteral("ipregex")).toString();
    const QString priorityregex = settings->value(QStringLiteral("priorityregex")).toString();

    const QString _backend = settings->value(QStringLiteral("backend"), QStringLiteral("file")).toString();
    Anonymizer::Backend backend = Anonymizer::File;
    if (_backend == QLatin1String("syslog")) {
        backend = Anonymizer::Syslog;
#ifdef WITH_SYSTEMD
    } else if (_backend == QLatin1String("journal")) {
        backend = Anonymizer::Journal;
#endif
    } else {
        backend = Anonymizer::File;
    }

    const QString identifier = settings->value(QStringLiteral("identifier"), group).toString();

    const QString _priority = settings->value(QStringLiteral("priority"), QStringLiteral("6")).toString();
    Anonymizer::Priority priority = Anonymizer::Informational;
    if ((_priority == QLatin1String("0")) || (QString::compare(_priority, QLatin1String("emergency"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Emergency;
    } else if ((_priority == QLatin1String("1")) || (QString::compare(_priority, QLatin1String("alert"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Alert;
    } else if ((_priority == QLatin1String("2")) || (QString::compare(_priority, QLatin1String("critical"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Critical;
    } else if ((_priority == QLatin1String("3")) || (QString::compare(_priority, QLatin1String("error"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Error;
    } else if ((_priority == QLatin1String("4")) || (QString::compare(_priority, QLatin1String("warning"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Warning;
    } else if ((_priority == QLatin1String("5")) || (QString::compare(_priority, QLatin1String("notice"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Notice;
    } else if ((_priority == QLatin1String("6")) || (QString::compare(_priority, QLatin1String("informational"), Qt::CaseInsensitive) == 0)) {
        priority = Anonymizer::Informational;
    } else {
        priority = Anonymizer::Debug;
    }

    settings->endGroup();

    delete settings;

    Anonymizer anon(logFileName, ipregex);
    anon.setBackend(backend);
    anon.setIdentifier(identifier);
    anon.setPriority(priority);
    anon.setPriorityRegex(priorityregex);
    if (!anon.run()) {
        return 2;
    }

    return app.exec();
}
