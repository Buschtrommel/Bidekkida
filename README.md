# bidekkida

Tiny tool to anonymize and/or redirect log file entries to different backends. Anonymization can
be used to anonymize personal data like IP addresses in log files. Currently it supports redirecting
the log data into files, syslog or systemd journal. It is mainly targeted to Apache web server log
files and written in C++/Qt.

Bidekkida is the old high german word for cover, veil.

## Usage
bidekkida can be used as piped command for Apache log files as in the following example:

    <VirtualHost *:80>

        CustomLog "|/usr/local/bin/bidekkida myhost" combined

    </VirtualHost>

The example above uses a configuration named *myhost* that will be read from the configuration file
at `/etc/bidekkida.conf`. Without a configuration, bidekkida will use the default values. There are
currently the following configuration options that can be set for as many configurations as needed:
* backend - The backend used to write the log data, default: file. Possible values:
  * file - writes the output into the file defined by *logfile*
  * syslog - writes the output into the default syslog
  * journal - writes the output into the systemd journal
* logfile - The file where anonymized data is written to if backend is *file*, default: /var/log/apache2/access_log
* identifier - Syslog identifier used for the *syslog* and *journal* backend, default: name of the configuration group
* ipregex - The regular expression to find the IP address in the log line, empty by default. If set, the found IP address will be anonymized.
* priority - The default syslog priority when writing to *syslog* or *journal* backend. Default: 6/Informational. Possible values:
  * emergency or 0
  * alert or 1
  * critical or 2
  * error or 3
  * warning or 4
  * notice or 5
  * informational or 6
  * debug or 7
* priorityregex - The regular expression to find the log priority in the log line, empty by default. Can be used togehter with *prioritymap*. If not set or priority can not be found, the default *priority* will be used.
* prioritymap - A string that creates a map of strings to integers used to map log priority to syslog priority values, empty by default. Can also be the keyword *apache* to use the default apache values for the log priority. Use it together with the *priorityregex* to set the priority for *syslog* and *journal* backend. Example string: `emerg,0;alert,1;crit,2;error,3;...`

You can create configurations for different hosts by simply creating new sections in the configuration file
at `/etc/bidekkida.conf`. (If the configuration file does not exist, simply create it.)
This is an example configuration file:

    [default]
    logfile=/var/log/apache2/access_log
    ipregex="^([a-f0-9:\\.]{3,39})\\s.*"

    [myhost]
    logfile=/var/log/apache2/myhost-access_log

    [otherhost]
    backend=journal
    priority=informational
    priorityregex="^\\[\\w*:(.*)\\]\\s\\[.*"
    prioritymap=apache

The configuration example above defines default values that will overwrite the hard coded default values as
well as an additional configuration named *myhost*. *myhost* uses a different log file, but will use the default
regular expression. *otherhost* uses the systemd journal backend and extracts the log priority but will not extract
and anonymize IP addresses.

Internally QRegularExpression is used. See the [documentation](http://doc.qt.io/qt-5/qregularexpression.html#details)
of that class to learn more about the possible regular expressions. Especially note that backslashes have to be masked
by another backslash in QRegularExpression.

## License

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

