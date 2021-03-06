# The client writes a message to Sys::Syslog unix method.
# The syslogd writes it into a file and through a pipe.
# The syslogd passes it via UDP to the loghost.
# The server receives the message on its UDP socket.
# Find the message in client, file, pipe, syslogd, server log.
# Check that the file log contains the hostname and message.

use strict;
use warnings;
use Sys::Hostname;

(my $host = hostname()) =~ s/\..*//;  # short name

our %args = (
    client => {
	logsock => { type => "unix" },
    },
    file => {
	loggrep => qr/ $host syslogd-regress\[\d+\]: /. get_testlog(),
    },
);

1;
