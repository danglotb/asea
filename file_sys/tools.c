/* ------------------------------
   $Id: tools.c 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   Misc. tools
   Philippe Marquet, october 2002
   
*/

#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


/*------------------------------
  explain why exiting
  ------------------------------------------------------------*/
int
fatal(int assert, const char *fname, const char *fmt, ...)
{
    if (! assert) {
	va_list ap;
	va_start(ap, fmt);
	
	fprintf(stderr, "[Error] %s: ", fname);
	vfprintf(stderr, fmt, ap);
	fputc ('\n', stderr);

	exit(EXIT_FAILURE);
    }

    /* make gcc -W happy */
    return EXIT_FAILURE;
}

/*------------------------------
  strdup() is not ANSI.
  ------------------------------------------------------------*/
#ifdef STRDUP_MISSING

#include <string.h>

char *
strdup(const char *s)
{
    size_t siz;
    char *copy;
    
    siz = strlen(s) + 1;
    if ((copy = malloc(siz)) == NULL)
	return(NULL);
    (void)memcpy(copy, s, siz);
    return(copy);
}
#endif /* STRDUP_MISSING */
