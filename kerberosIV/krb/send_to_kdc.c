/*
 * This software may now be redistributed outside the US.
 *
 * $Source: /scm/cvs/src/kerberosIV/krb/Attic/send_to_kdc.c,v $
 *
 * $Locker:  $
 */

/* 
  Copyright (C) 1989 by the Massachusetts Institute of Technology

   Export of this software from the United States of America is assumed
   to require a specific license from the United States Government.
   It is the responsibility of any person or organization contemplating
   export to obtain such a license before exporting.

WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
distribute this software and its documentation for any purpose and
without fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright notice and
this permission notice appear in supporting documentation, and that
the name of M.I.T. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  M.I.T. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without express
or implied warranty.

  */

#include "krb_locl.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define S_AD_SZ sizeof(struct sockaddr_in)

static int krb_udp_port = 0;

/* CLIENT_KRB_TIMEOUT indicates the time to wait before
 * retrying a server.  It's defined in "krb.h".
 */
static struct timeval timeout = { CLIENT_KRB_TIMEOUT, 0};
static char *prog = "send_to_kdc";
static send_recv(KTEXT pkt, KTEXT rpkt, int f, struct sockaddr_in *_to, struct hostent *addrs);

/*
 * This file contains two routines, send_to_kdc() and send_recv().
 * send_recv() is a static routine used by send_to_kdc().
 */

/*
 * send_to_kdc() sends a message to the Kerberos authentication
 * server(s) in the given realm and returns the reply message.
 * The "pkt" argument points to the message to be sent to Kerberos;
 * the "rpkt" argument will be filled in with Kerberos' reply.
 * The "realm" argument indicates the realm of the Kerberos server(s)
 * to transact with.  If the realm is null, the local realm is used.
 *
 * If more than one Kerberos server is known for a given realm,
 * different servers will be queried until one of them replies.
 * Several attempts (retries) are made for each server before
 * giving up entirely.
 *
 * If an answer was received from a Kerberos host, KSUCCESS is
 * returned.  The following errors can be returned:
 *
 * SKDC_CANT    - can't get local realm
 *              - can't find "kerberos" in /etc/services database
 *              - can't open socket
 *              - can't bind socket
 *              - all ports in use
 *              - couldn't find any Kerberos host
 *
 * SKDC_RETRY   - couldn't get an answer from any Kerberos server,
 *		  after several retries
 */

int
send_to_kdc(pkt, rpkt, realm)
	KTEXT pkt;
	KTEXT rpkt;
	char *realm;
{
    int i, f;
    int no_host; /* was a kerberos host found? */
    int retry;
    int n_hosts;
    int retval;
    struct sockaddr_in to;
    struct hostent *host, *hostlist;
    char *cp;
    char krbhst[MAX_HSTNM];
    char lrealm[REALM_SZ];

    /*
     * If "realm" is non-null, use that, otherwise get the
     * local realm.
     */
    if (realm)
	(void) strcpy(lrealm, realm);
    else
	if (krb_get_lrealm(lrealm,1)) {
	    if (krb_debug)
		fprintf(stderr, "%s: can't get local realm\n", prog);
	    return(SKDC_CANT);
	}
    if (krb_debug)
        printf("lrealm is %s\n", lrealm);
    if (krb_udp_port == 0) {
        register struct servent *sp;
        if ((sp = getservbyname("kerberos","udp")) == 0) {
            if (krb_debug)
                fprintf(stderr, "%s: Can't get kerberos/udp service\n",
                        prog);
            krb_udp_port = 750; /* Was return(SKDC_CANT); */
        }
        else
	    krb_udp_port = sp->s_port;
        if (krb_debug)
            printf("krb_udp_port is %d\n", krb_udp_port);
    }
    bzero((char *)&to, S_AD_SZ);
    hostlist = (struct hostent *) malloc(sizeof(struct hostent));
    if (!hostlist)
        return (/*errno */SKDC_CANT);
    if ((f = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        if (krb_debug)
            fprintf(stderr,"%s: Can't open socket\n", prog);
        return(SKDC_CANT);
    }
    /* from now on, exit through rtn label for cleanup */

    no_host = 1;
    /* get an initial allocation */
    n_hosts = 0;
    bzero((char *)&hostlist[n_hosts], sizeof(struct hostent));
    for (i = 1; krb_get_krbhst(krbhst, lrealm, i) == KSUCCESS; ++i) {
        if (krb_debug) {
            printf("Getting host entry for %s...",krbhst);
            (void) fflush(stdout);
        }
        host = gethostbyname(krbhst);
        if (krb_debug) {
            printf("%s.\n",
                   host ? "Got it" : "Didn't get it");
            (void) fflush(stdout);
        }
        if (!host)
            continue;
        no_host = 0;    /* found at least one */
        n_hosts++;
        /* preserve host network address to check later
         * (would be better to preserve *all* addresses,
         * take care of that later)
         */
        hostlist = (struct hostent *)
            realloc((char *)hostlist,
                    (unsigned)
                    sizeof(struct hostent)*(n_hosts+1));
        if (!hostlist)
            return /*errno */SKDC_CANT;
        bcopy((char *)host, (char *)&hostlist[n_hosts-1],
              sizeof(struct hostent));
        host = &hostlist[n_hosts-1];
        cp = malloc((unsigned)host->h_length);
        if (!cp) {
            retval = /*errno */SKDC_CANT;
            goto rtn;
        }
        bcopy((char *)host->h_addr, cp, host->h_length);
/* At least Sun OS version 3.2 (or worse) and Ultrix version 2.2
   (or worse) only return one name ... */
#if defined(h_addr)
        host->h_addr_list = (char **)malloc(2*sizeof(char *));
        if (!host->h_addr_list) {
            retval = /*errno */SKDC_CANT;
            goto rtn;
        }
        host->h_addr_list[1] = NULL;
#endif /* defined(h_addr) */
        host->h_addr = cp;
        bzero((char *)&hostlist[n_hosts],
              sizeof(struct hostent));
        to.sin_family = host->h_addrtype;
        bcopy(host->h_addr, (char *)&to.sin_addr,
              host->h_length);
        to.sin_port = krb_udp_port;
        if (send_recv(pkt, rpkt, f, &to, hostlist)) {
            retval = KSUCCESS;
            goto rtn;
        }
        if (krb_debug) {
            printf("Timeout, error, or wrong descriptor\n");
            (void) fflush(stdout);
        }
    }
    if (no_host) {
	if (krb_debug)
	    fprintf(stderr, "%s: can't find any Kerberos host.\n",
		    prog);
        retval = SKDC_CANT;
        goto rtn;
    }
    /* retry each host in sequence */
    for (retry = 0; retry < CLIENT_KRB_RETRY; ++retry) {
        for (host = hostlist; host->h_name != (char *)NULL; host++) {
            to.sin_family = host->h_addrtype;
            bcopy(host->h_addr, (char *)&to.sin_addr,
                  host->h_length);
            if (send_recv(pkt, rpkt, f, &to, hostlist)) {
                retval = KSUCCESS;
                goto rtn;
            }
        }
    }
    retval = SKDC_RETRY;
rtn:
    (void) close(f);
    if (hostlist) {
        register struct hostent *hp;
        for (hp = hostlist; hp->h_name; hp++)
#if defined(h_addr)
            if (hp->h_addr_list) {
#endif /* defined(h_addr) */
                if (hp->h_addr)
                    free(hp->h_addr);
#if defined(h_addr)
                free((char *)hp->h_addr_list);
            }
#endif /* defined(h_addr) */
        free((char *)hostlist);
    }
    return(retval);
}

/*
 * try to send out and receive message.
 * return 1 on success, 0 on failure
 */

static int
send_recv(pkt, rpkt, f, _to, addrs)
	KTEXT pkt;
	KTEXT rpkt;
	int f;
	struct sockaddr_in *_to;
	struct hostent *addrs;
{
    fd_set readfds;
    register struct hostent *hp;
    struct sockaddr_in from;
    int sin_size;
    int numsent;

    if (krb_debug) {
        if (_to->sin_family == AF_INET)
            printf("Sending message to %s...",
                   inet_ntoa(_to->sin_addr));
        else
            printf("Sending message...");
        (void) fflush(stdout);
    }
    if ((numsent = sendto(f,(char *)(pkt->dat), pkt->length, 0, 
			  (struct sockaddr *)_to,
                          S_AD_SZ)) != pkt->length) {
        if (krb_debug)
            printf("sent only %d/%d\n",numsent, pkt->length);
        return 0;
    }
    if (krb_debug) {
        printf("Sent\nWaiting for reply...");
        (void) fflush(stdout);
    }
    FD_ZERO(&readfds);
    FD_SET(f, &readfds);
    errno = 0;
    /* select - either recv is ready, or timeout */
    /* see if timeout or error or wrong descriptor */
    if (select(f + 1, &readfds, (fd_set *)0, (fd_set *)0, &timeout) < 1
        || !FD_ISSET(f, &readfds)) {
        if (krb_debug) {
	    long rfds;
	    bcopy(&readfds, &rfds, sizeof(rfds));
            fprintf(stderr, "select failed: readfds=%lx",
                    rfds);
            perror("");
        }
        return 0;
    }
    sin_size = sizeof(from);
    if (recvfrom(f, (char *)(rpkt->dat), sizeof(rpkt->dat), 0,
		 (struct sockaddr *)&from, &sin_size)
        < 0) {
        if (krb_debug)
            perror("recvfrom");
        return 0;
    }
    if (krb_debug) {
        printf("received packet from %s\n", inet_ntoa(from.sin_addr));
        fflush(stdout);
    }
    for (hp = addrs; hp->h_name != (char *)NULL; hp++) {
        if (!bcmp(hp->h_addr, (char *)&from.sin_addr.s_addr,
                  hp->h_length)) {
            if (krb_debug) {
                printf("Received it\n");
                (void) fflush(stdout);
            }
            return 1;
        }
        if (krb_debug)
            fprintf(stderr,
                    "packet not from %lx\n",
                    ntohl(hp->h_addr));
    }
    if (krb_debug)
        fprintf(stderr, "%s: received packet from wrong host! (%x)\n",
                "send_to_kdc(send_rcv)", (int)from.sin_addr.s_addr);
    return 0;
}
