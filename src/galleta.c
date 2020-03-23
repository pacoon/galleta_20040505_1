/*
 * Copyright (C) 2003, by Keith J. Jones.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define STRSIZE		1000

#ifdef CYGWIN
ssize_t pread( int d, void *buf, size_t nbytes, off_t offset) {
  lseek( d, offset, SEEK_SET );
  read( d, buf, nbytes );
}
#endif

//
/* Backwards 8 byte ASCII Hex to time_t */
//
time_t win_time_to_unix( unsigned long high, unsigned long low ) {
  double dbl;
  time_t total;

  dbl = ((double)high)*(pow(2,32));
  dbl += (double)(low);

  if ( dbl==0 ) {
    return 0;
  }

  dbl *= 1.0e-7;
  dbl -= 11644473600;

  total = (double)dbl;

  return total;
}

//
/* This function prepares a string for nice output */
//
int printablestring( char *str ) {
  int i;

  i = 0;
  while ( str[i] != '\0' ) {
    if ( (unsigned char)str[i] < 32 || (unsigned char)str[i] > 127 ) {
      str[i] = ' ';
    }
    i++; 
  }
  return 0;
}

//
/* This function prints the usage message */
//
void usage( void ) {
  printf("\nUsage:  galleta [options] <filename>\n" );
  printf("\t-d Field Delimiter (TAB by default)\n" );
  printf("\n\n");
}

//
/* MAIN function */
//
int main( int argc, char **argv ) {
  int cookie_file;
  int i, j;
  int res;
  int opt;
  unsigned long ctimehigh, ctimelow, etimehigh, etimelow;
  char delim[10];
  int eof = 0;
  char chr;
  char site[STRSIZE], variable[STRSIZE], value[STRSIZE], ctime1[STRSIZE], ctime2[STRSIZE], etime1[STRSIZE], etime2[STRSIZE], flags[STRSIZE];
  time_t ctime;
  time_t etime;
  char ascctime[26], ascetime[26];
  int year, mon;
  struct tm *ctimetm, *etimetm;

  if (argc < 2) {
    usage();
    exit( -2 );
  }

  strcpy( delim, "\t" );

  printf("Cookie File: %s\n\n", argv[argc-1]);
  cookie_file = open( argv[argc-1], O_RDONLY, 0 );

  if ( cookie_file <= 0 ) { 
    printf("ERROR - The cookie file cannot be opened!\n\n");
    usage();
    exit( -3 ); 
  }

  while ((opt = getopt( argc, argv, "d:")) != -1) {
    switch(opt) {
      case 'd':
        strncpy( delim, optarg, 10 );
        break;

      default:
        usage();
        exit(-1);
    }
  }

  printf( "SITE%sVARIABLE%sVALUE%sCREATION TIME%sEXPIRE TIME%sFLAGS\n", delim, delim, delim, delim, delim );

  i = 0;

  while (eof == 0) {

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      variable[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    variable[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      value[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    value[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      site[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    site[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      flags[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    flags[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      etime1[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    etime1[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      etime2[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    etime2[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      ctime1[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    ctime1[j] = '\0';

    j = 0;
    res = pread( cookie_file, &chr, 1, i );
    while (res == 1 && chr != 0x0A) {
      ctime2[j] = chr;
      i++;
      j++;
      res = pread( cookie_file, &chr, 1, i );
    }
    i++;
    ctime2[j] = '\0';

    i = i+2;
    res = pread( cookie_file, &chr, 1, i );
    if (res == 0) {
      eof = 1;
    }
    ctimelow = strtoul( ctime1, (char **)NULL, 10 );
    ctimehigh = strtoul( ctime2, (char **)NULL, 10 );
    etimelow = strtoul( etime1, (char **)NULL, 10 );
    etimehigh = strtoul( etime2, (char **)NULL, 10 );

    ctime = win_time_to_unix( ctimehigh, ctimelow );
    etime = win_time_to_unix( etimehigh, etimelow );

    ctimetm = localtime( &ctime );
    year = ctimetm->tm_year + 1900;
    mon = ctimetm->tm_mon + 1;
    sprintf( ascctime, "%02d/%02d/%02d %02d:%02d:%02d", mon, ctimetm->tm_mday, year, ctimetm->tm_hour, ctimetm->tm_min, ctimetm->tm_sec );

    etimetm = localtime( &etime );
    year = etimetm->tm_year + 1900;
    mon = etimetm->tm_mon + 1;
    sprintf( ascetime, "%02d/%02d/%02d %02d:%02d:%02d", mon, etimetm->tm_mday, year, etimetm->tm_hour, etimetm->tm_min, etimetm->tm_sec );

    printf( "%s%s%s%s%s%s%s%s%s%s%s\n", site, delim, variable, delim, value, delim, ascctime, delim, ascetime, delim, flags ); 
  }

  close (cookie_file);
}
