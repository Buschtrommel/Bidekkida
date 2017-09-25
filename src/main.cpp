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

    settings->beginGroup(QStringLiteral("default"));
    QString logFileName = settings->value(QStringLiteral("logfile"), QStringLiteral("/var/log/apache2/access_log")).toString();
    QString ipregex = settings->value(QStringLiteral("ipregex")).toString();
    QString priorityregex = settings->value(QStringLiteral("priorityregex")).toString();
    QString backend = settings->value(QStringLiteral("backend"), QStringLiteral("file")).toString();
    QString identifier = settings->value(QStringLiteral("identifier"), QCoreApplication::applicationName()).toString();
    QString priority = settings->value(QStringLiteral("priority"), QStringLiteral("6")).toString();
    QString priorityMap = settings->value(QStringLiteral("prioritymap")).toString();
    settings->endGroup();


    if (argc > 1) {
        const QString group = QString::fromUtf8(argv[1]);

        settings->beginGroup(group);
        logFileName = settings->value(QStringLiteral("logfile"), logFileName).toString();
        ipregex = settings->value(QStringLiteral("ipregex"), ipregex).toString();
        priorityregex = settings->value(QStringLiteral("priorityregex"), priorityregex).toString();
        backend = settings->value(QStringLiteral("backend"), backend).toString();
        identifier = settings->value(QStringLiteral("identifier"), group).toString();
        priority = settings->value(QStringLiteral("priority"), priority).toString();
        priorityMap = settings->value(QStringLiteral("prioritymap"), priorityMap).toString();
        settings->endGroup();
    }

    delete settings;

    Anonymizer anon(logFileName, ipregex);
    anon.setBackend(backend);
    anon.setIdentifier(identifier);
    anon.setPriority(priority);
    anon.setPriorityRegex(priorityregex);
    anon.setPriorityMap(priorityMap);
    if (!anon.run()) {
        return 2;
    }

    return app.exec();
}
