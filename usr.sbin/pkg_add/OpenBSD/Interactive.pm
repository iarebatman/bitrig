# ex:ts=8 sw=4:
# $OpenBSD: Interactive.pm,v 1.19 2014/12/29 07:37:27 jasper Exp $
#
# Copyright (c) 2005-2007 Marc Espie <espie@openbsd.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF

use strict;
use warnings;

package OpenBSD::Interactive;

sub new
{
	my ($class, $state) = @_;
	bless {
	    state => $state,
	    always => 0,
	}, $class;
}

sub ask_list
{
	my ($self, $prompt, @values) = @_;
	if ($self->{always}) {
		return $values[0];
	}

	$self->{state}->errsay('#1', $prompt);
	my $i = 0;
	for my $v (@values) {
		$self->{state}->errsay("#1\t#2: #3", 
		    $i == 0 ? "a" : "", $i, $v);
		$i++;
	}
LOOP:
	$self->{state}->errprint("Your choice: ");
	my $result = <STDIN>;
	unless (defined $result) {
		$self->{state}->errsay("");
		return $values[0];
	}
	chomp $result;
	if ($result eq '') {
		return $values[0];
	}
	if ($result eq 'a') {
		$self->{always} = 1;
		return $values[0];
	}
	if ($result =~ m/^\d+$/o) {
		if ($result >= 0 && $result < @values) {
			return $values[$result];
		}
		$self->{state}->errsay("invalid numeric value !");
		goto LOOP;
	}
	if (grep { $result eq $_ } @values) {
		return $result;
	} else {
		$self->{state}->errsay("Ambiguous value !");
		goto LOOP;
	}
}

sub confirm
{
	my ($self, $prompt, $yesno) = @_;
	if ($self->{always}) {
		return 1;
	}
LOOP2:
	$self->{state}->errprint("#1 ? [#2/a] ", 
	    $prompt, $yesno ? "Y/n" : "y/N");

	my $result = <STDIN>;
	unless(defined $result) {
		$self->{state}->errsay("");
		return $yesno;
	}
	chomp $result;
	$result =~ s/\s+//go;
	$result =~ tr/A-Z/a-z/;
	if ($result eq 'yes' or $result eq 'y') {
		return 1;
	}
	if ($result eq 'no' or $result eq 'n') {
		return 0;
	}
	if ($result eq 'a') {
		$self->{always} = 1;
		return 1;
	}
	if ($result eq '') {
		return $yesno;
	}
	$self->{state}->errsay("Ambiguous answer");
	goto LOOP2;
}

sub is_interactive
{
	return 1;
}

1;
