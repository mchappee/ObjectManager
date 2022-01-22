/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright © Matthew Chappee <matthew@orasoft.org>
 *             Paul Flinders <paul@orasoft.org>
 *             Brent Gilmore <brent@orasoft.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/objectman.h"

char *sse(char *in)  /*Strip spaces from end of string*/
{
    int i;

    i=strlen(in)-1;
    while(in[i]==' '&&i>-1)i--;
    in[i+1]=0;
    return in;
}


char *ssb(char *in)  /*Strip spaces from beginning of string*/
{
    int i,len;

    i=0;
    len=strlen(in);
    while(in[i]==' '&&i<len)i++;
    memmove(in,in+i,len-i);
    in[len-i]=0;
    return in;
}        

char *nosemi(char *in)  /*Strip semis from end of string*/
{
    int i;

    i=strlen(in)-1;
    while(in[i]==';'&&i>-1)i--;
    in[i+1]=0;
    return in;
}


char *nocrlf(char *in)  /*Strip \r\n from end of string*/
{
    int i;

    i=strlen(in)-1;
    while((in[i]=='\r' || in[i]=='\n') &&i>-1)i--;
    in[i+1]=0;
    return in;
}

/* Revised by Adrian Miranda for egcs/Solaris */
char *strupr(char *p)
{
    char *s;

    s=p;
    while(*p){
	*p=toupper(*p);
	++p;
    }

    return s;

}

/* Revised by Adrian Miranda for egcs/Solaris */
char *strlower(char *p)
{
    char *s;

    s=p;
    while(*p){
	*p=tolower(*p);
	++p;
    }

    return s;

}


char *
strnocasestr(
    char *str1, 
    char *str2)
{
char *ptr1;
char *ptr2;
char *position;

position = str1;
while(1)
        {
        if(! *position) return(0);

        /* compare from position start */
        ptr1 = position;
        ptr2 = str2;
        while(1)
                {
                /* test for end string 2 first, that would be found */
                if(! *ptr2) return(position);

                /* test for end string 1 first, that would be failed */
                if(! *ptr1) break;

                if(tolower(*ptr1) == tolower(*ptr2) )
                        {
                        ptr1++;
                        ptr2++;
                        }
                else break;
                }/* end inner while */
        position++;
        }/* end outer while */
}/* end function strcasestr */







