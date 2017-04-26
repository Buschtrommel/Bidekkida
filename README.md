# bidekkida

Tiny tool to anonymize log file entries that can contain personal data like IP addresses.
Currently mainly targeted to Apache web server log files. It is written in C++/Qt.

Bidekkida is the old high german word for cover, veil.

## Usage
bidekkida can be used as piped command for Apache log files as in the following example:

    <VirtualHost *:80>
    
        CustomLog "|/usr/local/bin/bidekkida myhost" combined
    
    </VirtualHost>
    
The example above uses a configuration named *myhost* that will be read from the configuration file
at `/etc/bidekkida.conf`. Without a configuration, bidekkida will use the default values. There are
currently two configuration options that can be set for as many configurations as needed:
* logfile - the file where anonymized data is written to, default: /var/log/apache2/access_log
* regex - the regular expression to find the IP address in the log line, default: ^([a-f0-9:\\.]{3,39})\\s.*

You can create configurations for different hosts by simply creating new sections in the configuration file
at `/etc/bidekkida.conf`. (If the configuration file does not exist, simply create it.)
This is an example configuration file:

    [default]
    logfile=/var/log/apache2/access_log
    regex="^([a-f0-9:\\.]{3,39})\\s.*"
    
    [myhost]
    logfile=/var/log/apache2/myhost-access_log
    
The configuration example above defines default values that will overwrite the hard coded default values as
well as an additional configuration named *myhost*. *myhost* uses a different log file, but will use the default
regular expression.

The hard coded regular expression should work fine with the default combined log file format for Apache CustomLog.
If you are using a different format, you have to adapt the regular expression to your logging format - or adapt your
log file format.

Internally QRegularExpression is used to find the IP address. See the [documentation](http://doc.qt.io/qt-5/qregularexpression.html#details) 
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

