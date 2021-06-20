/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  util.c - miscellaneous utility code for libtar
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include <internal.h>

#include <stdio.h>
#include <sys/param.h>
#include <errno.h>

#ifdef STDC_HEADERS
# include <string.h>
#endif


/* hashing function for pathnames */
int
path_hashfunc(char *key, int numbuckets)
{
	char buf[MAXPATHLEN];
	char *p;

	strcpy(buf, key);
	p = basename(buf);

	return (((unsigned int)p[0]) % numbuckets);
}


/* matching function for dev_t's */
int
dev_match(dev_t *dev1, dev_t *dev2)
{
	return !memcmp(dev1, dev2, sizeof(dev_t));
}


/* matching function for ino_t's */
int
ino_match(ino_t *ino1, ino_t *ino2)
{
	return !memcmp(ino1, ino2, sizeof(ino_t));
}


/* hashing function for dev_t's */
int
dev_hash(dev_t *dev)
{
	return *dev % 16;
}


/* hashing function for ino_t's */
int
ino_hash(ino_t *inode)
{
	return *inode % 256;
}


/*
** mkdirhier() - create all directories in a given path
** returns:
**	0			success
**	1			all directories already exist
**	-1 (and sets errno)	error
*/
int
mkdirhier(char *path)
{
	char src[MAXPATHLEN], dst[MAXPATHLEN] = "";
	char *dirp, *nextp = src;
	int retval = 1;

	if (strlcpy(src, path, sizeof(src)) > sizeof(src))
	{
		errno = ENAMETOOLONG;
		return -1;
	}

	if (path[0] == '/')
		strcpy(dst, "/");

	while ((dirp = strsep(&nextp, "/")) != NULL)
	{
		if (*dirp == '\0')
			continue;

		if (dst[0] != '\0')
			strcat(dst, "/");
		strcat(dst, dirp);

		if (mkdir(dst, 0777) == -1)
		{
			if (errno != EEXIST)
				return -1;
		}
		else
			retval = 0;
	}

	return retval;
}


/* calculate header checksum */
int
th_crc_calc(TAR *t)
{
	int i, sum = 0;

	for (i = 0; i < T_BLOCKSIZE; i++)
		sum += ((unsigned char *)(&(t->th_buf)))[i];
	for (i = 0; i < 8; i++)
		sum += (' ' - (unsigned char)t->th_buf.chksum[i]);

	return sum;
}


/* calculate a signed header checksum */
int
th_signed_crc_calc(TAR *t)
{
	int i, sum = 0;

	for (i = 0; i < T_BLOCKSIZE; i++)
		sum += ((signed char *)(&(t->th_buf)))[i];
	for (i = 0; i < 8; i++)
		sum += (' ' - (signed char)t->th_buf.chksum[i]);

	return sum;
}


/* string-octal to integer conversion */
off_t
oct_to_int_bounds(char *oct, int len, off_t min, off_t max)
{
	long long i;
	if (oct[0] & 0x80) {
		// common base 256 extension

		// sign-extend
		i = (oct[0] & 0x40) ? (off_t)-1 << ((len - 1) * 8) : 0;
		// remove sign from first byte
		i |= (off_t)(oct[0] & 0x7f) << ((len - 1) * 8);
		// base 256
		for (int w = 1; w < len; w ++) {
			i |= (off_t)oct[w] << ((len - 1 - w) * 8);
		}
	} else {
		if (sscanf(oct, "%llo", &i) == 0) {
			i = 0;
		}
	}
	return (i >= min && i <= max) ? i : 0;
}

/* integer to NULL-terminated string-octal conversion */
void
int_to_oct(off_t num, char * oct, int octlen)
{
	int_to_oct_nonull(num, oct, octlen - 1);
	oct[octlen - 1] = '\0';
}


/* integer to string-octal conversion, no NULL */
void
int_to_oct_nonull(off_t num, char *oct, size_t octlen)
{
	if (num >= 0 && num <= 077777777777) {
		snprintf(oct, octlen, "%*llo", (int)(octlen - 1), (unsigned long long)num);
		oct[octlen - 1] = ' ';
	} else {
		// common base 256 extension
		while (octlen) {
			octlen --;
			oct[octlen] = num & 0xff;
			num >>= 8;
		}
		oct[0] |= 0x80;
	}
}


