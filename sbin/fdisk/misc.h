/*	$OpenBSD: misc.h,v 1.30 2015/03/30 17:11:49 krw Exp $	*/

/*
 * Copyright (c) 1997 Tobias Weingartner
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _MISC_H
#define _MISC_H

struct unit_type {
	char	*abbr;
	int64_t	 conversion;
	char	*lname;
};
extern struct unit_type unit_types[];
#define SECTORS	1	/* units are bytes/sectors/kbytes/mbytes/gbytes */

/* Prototypes */
int unit_lookup(char *);
int ask_cmd(char **, char **);
uint32_t ask_num(const char *, uint32_t, uint32_t, uint32_t);
int ask_pid(int, int, int);
char *ask_string(const char *, const char *);
int ask_yn(const char *);
uint64_t getuint64(char *, uint64_t, uint64_t);
void BN_to_CHS(uint32_t, uint32_t *, uint32_t *, uint32_t *);
uint32_t CHS_to_BN(uint32_t, uint32_t, uint32_t);
uint32_t crc32(const unsigned char *, const uint32_t);

#endif /* _MISC_H */
