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

#include <QObject>
#include <QTextStream>
#include <QRegularExpression>

class QSocketNotifier;
class QFile;

class Anonymizer : public QObject
{
    Q_OBJECT
public:
    Anonymizer(const QString &outputFileName, const QString &regex, QObject *parent = nullptr);

    ~Anonymizer();

    bool run();

private slots:
    void dataAvailable();

private:
    QFile *m_stdin;
    QFile *m_outputFile;
    QSocketNotifier *m_notifier;
    QTextStream m_outStream;
    QRegularExpression m_regex;
};

#endif // ANONYMIZER_H
