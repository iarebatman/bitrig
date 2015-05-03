/*	$OpenBSD: ps.h,v 1.10 2015/05/03 06:23:28 guenther Exp $	*/
/*	$NetBSD: ps.h,v 1.11 1995/09/29 21:57:03 cgd Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ps.h	8.1 (Berkeley) 5/31/93
 */

#include <sys/queue.h>
#include <sys/resource.h>
#include <sys/sysctl.h>


#define	UNLIMITED	0	/* unlimited terminal width */
enum type {
	INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64
};

/* Variables. */
typedef struct varent {
	SIMPLEQ_ENTRY(varent) entries;
	struct var *var;
} VARENT;

SIMPLEQ_HEAD(varent_list, varent);

typedef struct var {
	char	*name;		/* name(s) of variable */
	char	*header;	/* default header */
	char	*alias;		/* aliases */
#define	COMM	0x01		/* needs exec arguments and environment (XXX) */
#define	LJUST	0x02		/* left adjust on output (trailing blanks) */
#define	USER	0x04		/* needs user structure */
#define	INF127	0x08		/* 127 = infinity: if > 127, print 127. */
#define	NLIST	0x10		/* needs nlist info from kernel */
	u_int	flag;
				/* output routine */
	void	(*oproc)(const struct kinfo_proc *, VARENT *);
	short	width;		/* printing width */
	char	parsed;		/* have we been parsed yet? (avoid dupes) */
	/*
	 * The following (optional) elements are hooks for passing information
	 * to the generic output routine, pvar(), which prints simple elements
	 * from struct kinfo_proc
	 */
	int	off;		/* offset in structure */
	enum	type type;	/* type of element */
	char	*fmt;		/* printf format */
	char	*time;		/* time format */
	/*
	 * glue to link selected fields together
	 */
} VAR;

#include "extern.h"
