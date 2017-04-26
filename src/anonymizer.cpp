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

#include "anonymizer.h"
#include <QSocketNotifier>
#include <QFile>
#include <iostream>
#include <QRegularExpressionMatch>
#include <QVector>
#include <QStringRef>
#include <QStringBuilder>

Anonymizer::Anonymizer(const QString &outputFileName, const QString &regex, QObject *parent) :
    QObject(parent),
    m_stdin(new QFile(this)),
    m_outputFile(new QFile(outputFileName, this))
{
    m_regex.setPattern(regex);
}


Anonymizer::~Anonymizer()
{
}



bool Anonymizer::run()
{
    if (!m_stdin->open(stdin, QFile::ReadOnly|QFile::Text)) {
        qDebug("Failed to open standard input");
        return false;
    }
    qDebug("Successfully opened standard input.");

    if (!m_outputFile->open(QFile::WriteOnly|QFile::Append|QFile::Text)) {
        qDebug("Failed to open output file: %s", m_outputFile->fileName().toUtf8().constData());
        return false;
    }
    qDebug("Successfully opened file to write: %s", m_outputFile->fileName().toUtf8().constData());

    m_notifier = new QSocketNotifier(m_stdin->handle(), QSocketNotifier::Read, this);

    if (!m_notifier->isEnabled()) {
        qDebug("Failed to enable socket notifier.");
        return false;
    }

    if (!connect(m_notifier, &QSocketNotifier::activated, this, &Anonymizer::dataAvailable)) {
        qDebug("Failed to connect to QSocketNotifier::activated signal.");
        return false;
    }

    m_outStream.setDevice(m_outputFile);
    return true;
}



void Anonymizer::dataAvailable()
{
    QString s(m_stdin->readLine());
    const QRegularExpressionMatch match = m_regex.match(s);
    if (Q_LIKELY(match.hasMatch())) {
        const QString origAddress = match.captured(1);
        QString cloakedAddress;
        if (origAddress.contains(QChar('.'))) {
            QVector<QStringRef> parts = origAddress.splitRef(QChar('.'));
            cloakedAddress = parts.at(0) % QChar('.') % parts.at(1) % QLatin1String(".0.0");
        } else {
            if (Q_LIKELY(origAddress.size() > 3)) {
                QVector<QStringRef> parts = origAddress.splitRef(QChar(':'), QString::SkipEmptyParts);
                if (Q_LIKELY(parts.size() > 3)) {
                    cloakedAddress = parts.at(0) % QChar(':') % parts.at(1) % QChar(':') % parts.at(2) % QLatin1String("::");
                } else {
                    cloakedAddress = origAddress;
                }
            } else {
                cloakedAddress = origAddress;
            }
        }
        s.replace(origAddress, cloakedAddress);
    }
    m_outStream << s;
    flush(m_outStream);
}
