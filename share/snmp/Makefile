# $OpenBSD: Makefile,v 1.3 2012/02/23 03:54:38 joel Exp $

IFILES=	OPENBSD-SNMPD-CONF.txt OPENBSD-BASE-MIB.txt
IFILES+= OPENBSD-MEM-MIB.txt OPENBSD-SENSORS-MIB.txt
IFILES+= OPENBSD-CARP-MIB.txt OPENBSD-PF-MIB.txt

all clean cleandir depend lint obj tags: _SUBDIRUSE

realinstall:
	${INSTALL} -c -m 0444 ${IFILES} ${DESTDIR}${BINDIR}/snmp/mibs

.include <bsd.prog.mk>
