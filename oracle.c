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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "include/objectman.h" /* for sqlerr macro */
#include "oracle_oci.h" 
#include "common-files/common.h" 
#include "objects/tables/tablefuncs.h"
#include "include/global.h"

#include "sqlcpr.h"

EXEC SQL INCLUDE sqlda;
EXEC SQL INCLUDE sqlca;

#define MAX_COLS 256
#define MAX_VNAME_LEN 32

/* Do a query - for each row returned pass the data to a callback */
/* FIXME - write the other routines in this file in terms of this one */

/*void
do_query(
    const char *sql_query,
    const char *databaseid,
    int  max_rows,
    int (*rowcallback)(int, int, char **, short *, short **, char **, int *, short *, gpointer),
    gpointer user_data)
{
    EXEC SQL BEGIN DECLARE SECTION;
    const char	 *cdbid;
    const char	 *query;
    EXEC SQL END DECLARE SECTION;
    SQLDA	 *select_des = sqlald(MAX_COLS, MAX_VNAME_LEN, 30); 
    short        indicators[MAX_COLS];
    int          row = 0;
    int          i;
    int          null_ok;

    assert(rowcallback);

    if (!select_des) {
	g_error("Out of memory allocating select descriptor\n");
	return;
    }

    cdbid = databaseid;
    query = sql_query;

    EXEC SQL WHENEVER SQLERROR do sqlerr();

    EXEC SQL AT :cdbid PREPARE prepQuery FROM :query;
    EXEC SQL AT :cdbid DECLARE do_queryCursor CURSOR FOR prepQuery;
    EXEC SQL AT :cdbid OPEN do_queryCursor;

    select_des->N = MAX_COLS;
    for (i = 0; i < MAX_COLS; i++)
	select_des->I[i] = &indicators[i];

    EXEC SQL AT :cdbid DESCRIBE SELECT LIST FOR prepQuery INTO select_des;

    if (select_des->F < 0) {
	show_warning_msg("Query returns too many columns (%d), maximum is %d\n",
		-(select_des->F), MAX_COLS);
	return;
    }

    select_des->N = select_des->F;

    for (i=0; i < select_des->F; i++) { 
	sqlnul (&(select_des->T[i]), &(select_des->T[i]), &null_ok);

	switch(select_des->T[i]) { 
	case ORA_TINT_NUMBER: {
	    int prec;
	    int scal;
	    sqlprc(&(select_des->L[i]), &prec, &scal); 
	    if (prec == 0)
		prec = 40; 
	    select_des->L[i] = prec + 2; 
	    if (scal < 0) 
		select_des->L[i] += -scal; 
	    break; 
	}
	case ORA_TINT_ROWID: 
	    select_des->L[i] = 18; 
	    break; 
	case ORA_TINT_DATE: 
	    select_des->L[i] = 9; 
	    break; 
	case ORA_TINT_LONG: 
	case ORA_TINT_LONGRAW:
	    select_des->L[i] = 131072; 
	    break; 
	case ORA_TINT_VARCHAR2: 
	case ORA_TINT_RAW: 
	    break; 

	} 
	select_des->V[i] = g_malloc(select_des->L[i]+7);

	select_des->L[i] += 1; 
	select_des->T[i] = ORA_TEXT_STRING; 

	select_des->S[i][select_des->C[i]] = '\0';
    }

    EXEC SQL WHENEVER NOT FOUND do break;
    
    for (row = 0; max_rows == 0 || row < max_rows; row++) {
        EXEC SQL WHENEVER SQLERROR GOTO stopover;		
        EXEC SQL AT :cdbid FETCH do_queryCursor USING DESCRIPTOR select_des;

	if (rowcallback(row, select_des->F, select_des->S, 
			select_des->T, select_des->I,
			select_des->V, (int *)select_des->L,
			select_des->T, user_data) == 0)
	    break;
    }
    EXEC SQL WHENEVER SQLERROR CONTINUE;
    EXEC SQL AT :cdbid CLOSE do_queryCursor;

  stopover:
    for (i=0; i < select_des->F; i++) { 
	g_free(select_des->V[i]);
    }
    sqlclu(select_des);
}
*/

/* See query below, values[0] is object type, values[1] is owner and
   values[2] is the name of the object */
/*
static int
clist_from_query_by_row_rowcallback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *colname_l, 
    short	     **indicators, 
    char	     **values, 
    int		     *values_l, 
    short	     *types, 
    gpointer	     user_data)
{
    GtkCList *clist = (GtkCList *)user_data;
    int i;
	
    if (row == 0) {
	char **row = g_malloc(clist->columns * sizeof(char *));

	for (i = 0; i < clist->columns; row[i++] = 0)
	    ;
	for (i = 0; i < ncols; i++) {
	    row[0] = colnames[i];
	    gtk_clist_append(clist, row);
	}
	g_free(row);
    }

    for (i = 0; i < ncols; i++) {
	if (*indicators[i] == -1)
	    gtk_clist_set_text(clist, i, row+1, "");
	else
	    gtk_clist_set_text(clist, i, row+1, values[i]);
    }
    return TRUE;
}
*/
/* Return a GTK Clist containing the result of a sql query
 * The query is returned rotated so that returned rows are inserted 
 * along columns. */
/*
GtkCList *
clist_from_query_by_row(
    const char *sql_query,
    const char *databaseid,
    int  ncols,
    char **titles,
    int (*rowcallback)(int))
{
    GtkCList *clist;

    if (titles) {
	clist = (GtkCList *)gtk_clist_new_with_titles(ncols, titles);
    } else {
	clist = (GtkCList *)gtk_clist_new(ncols);
    }
    do_query(sql_query, databaseid, ncols - 1, clist_from_query_by_row_rowcallback, 
	     clist);

    return clist;
}
*/

/* Return a GTK Clist containing the result of a sql query */
/*
GtkCList *
clist_from_query_old(
    const char *sql_query,
    const char *databaseid,
    int  max_rows,
    char *nullsubst,
    int (*rowcallback)(int))
{
    EXEC SQL BEGIN DECLARE SECTION;
    const char	 *cdbid;
    const char	 *query;
    EXEC SQL END DECLARE SECTION;
    SQLDA	 *select_des = sqlald(MAX_COLS, MAX_VNAME_LEN, 30); 
    short        indicators[MAX_COLS];
    int          row = 0;
    GtkCList     *clist = 0;
    int          i;
    int          null_ok;

    if (!select_des) {
	g_error("Out of memory allocating select descriptor\n");
	return 0;
    }

    cdbid = databaseid;
    query = sql_query;

    EXEC SQL WHENEVER SQLERROR do sqlerr();

    EXEC SQL AT :cdbid PREPARE prepQuery FROM :query;
    EXEC SQL AT :cdbid DECLARE clistCursor CURSOR FOR prepQuery;
    EXEC SQL AT :cdbid OPEN clistCursor;

    select_des->N = MAX_COLS;
    for (i = 0; i < MAX_COLS; i++)
	select_des->I[i] = &indicators[i];

    EXEC SQL AT :cdbid DESCRIBE SELECT LIST FOR prepQuery INTO select_des;

    if (select_des->F < 0) {
	show_warning_msg("Query returns too many columns (%d), maximum is %d\n",
		-(select_des->F), MAX_COLS);
	return 0;
    }

    select_des->N = select_des->F;

    for (i=0; i < select_des->F; i++) { 
	select_des->S[i][select_des->C[i]] = '\0';
    }

    clist = (GtkCList *)gtk_clist_new_with_titles (select_des->F, select_des->S);

    for (i=0; i < select_des->F; i++) { 
	sqlnul (&(select_des->T[i]), &(select_des->T[i]), &null_ok);

	switch(select_des->T[i]) { 
	case ORA_TINT_NUMBER: {
	    int prec;
	    int scal;
	    sqlprc(&(select_des->L[i]), &prec, &scal); 
	    if (prec == 0)
		prec = 40; 
	    select_des->L[i] = prec + 2; 
	    if (scal < 0) 
		select_des->L[i] += -scal; 
	    break; 
	}
	case ORA_TINT_ROWID: 
	    select_des->L[i] = 18; 
	    break; 
	case ORA_TINT_DATE: 
	    select_des->L[i] = 9; 
	    break; 
	case ORA_TINT_LONG: 
	case ORA_TINT_LONGRAW:
	    select_des->L[i] = 131072; 
	    break; 
	case ORA_TINT_VARCHAR2: 
	case ORA_TINT_RAW: 
	    break; 

	} 
	select_des->V[i] = g_malloc(select_des->L[i] + 1 +
				    +(nullsubst ? strlen(nullsubst) : 0));
	gtk_object_set_data(GTK_OBJECT(gtk_clist_get_column_widget (clist, i)), 
			    "coltype", (gpointer)((int)select_des->T[i]));
	select_des->L[i] += 1; 
	select_des->T[i] = ORA_TEXT_STRING; 
    }

    EXEC SQL WHENEVER NOT FOUND do break;
    
    for (row = 0; max_rows == 0 || row < max_rows; row++) {
	for (i = 0; i < select_des->F; i++)
	    *(select_des->V[i]) = '\0';

        EXEC SQL WHENEVER SQLERROR GOTO stopover;		
        EXEC SQL AT :cdbid FETCH clistCursor USING DESCRIPTOR select_des;
	
	if (nullsubst)
	    for (i = 0; i < select_des->F; i++)
		if ( *(select_des->I[i]) == -1)
		    strcpy(select_des->V[i], nullsubst);

	gtk_clist_append( clist, select_des->V);
	if (rowcallback && (rowcallback(row) == 0))
	    break;
    }
    EXEC SQL WHENEVER SQLERROR CONTINUE;
    EXEC SQL AT :cdbid CLOSE clistCursor;

  stopover:
    for (i=0; i < select_des->F; i++) { 
	g_free(select_des->V[i]);
    }
    sqlclu(select_des);
    return clist;
}
*/

/* Return a list containing the first column from a sql query.
 * Caller is responsible for freeing the list and the data contained 
 * in each entry */
/*
GList *
glist_from_query_old(
    const char    *sql_query,
    const char    *databaseid,
    int           max_rows,
    gboolean      showerrors,
    int          (*rowcallback)(int))
{
    EXEC SQL BEGIN DECLARE SECTION;
    const char	 *cdbid;
    const char	 *query;
    EXEC SQL END DECLARE SECTION;
    SQLDA	 *select_des = sqlald(MAX_COLS, MAX_VNAME_LEN, 30); 
    short        indicators[MAX_COLS];
    int          row = 0;
    GList        *list = 0;
    int          i;
    int          null_ok;

    if (!select_des) {
	g_error("Out of memory allocating select descriptor\n");
	return 0;
    }

    cdbid = databaseid;
    query = sql_query;

    EXEC SQL WHENEVER SQLERROR goto call_error;

    EXEC SQL AT :cdbid PREPARE prepQuery FROM :query;
    EXEC SQL AT :cdbid DECLARE glistCursor CURSOR FOR prepQuery;
    EXEC SQL AT :cdbid OPEN glistCursor;

    select_des->N = MAX_COLS;
    for (i = 0; i < MAX_COLS; i++)
	select_des->I[i] = &indicators[i];

    EXEC SQL AT :cdbid DESCRIBE SELECT LIST FOR prepQuery INTO select_des;

    if (select_des->F < 0) {
	show_warning_msg("Query returns too many columns (%d), maximum is %d\n",
		-(select_des->F), MAX_COLS);
	return 0;
    }

    select_des->N = select_des->F;

    for (i=0; i < select_des->F; i++) { 
	sqlnul (&(select_des->T[i]), &(select_des->T[i]), &null_ok);

	switch(select_des->T[i]) { 
	case ORA_TINT_NUMBER: {
	    int prec;
	    int scal;
	    sqlprc(&(select_des->L[i]), &prec, &scal); 
	    if (prec == 0)
		prec = 40; 
	    select_des->L[i] = prec + 2; 
	    if (scal < 0) 
		select_des->L[i] += -scal; 
	    break; 
	}
	case ORA_TINT_ROWID: 
	    select_des->L[i] = 18; 
	    break; 
	case ORA_TINT_DATE: 
	    select_des->L[i] = 9; 
	    break; 
	case ORA_TINT_LONG: 
	case ORA_TINT_LONGRAW:
	    select_des->L[i] = 131072; 
	    break; 
	case ORA_TINT_VARCHAR2: 
	case ORA_TINT_RAW: 
	    break; 

	} 
	select_des->V[i] = g_malloc(select_des->L[i]+7);
	select_des->L[i] += 1; 
	select_des->T[i] = ORA_TEXT_STRING; 
    }

    EXEC SQL WHENEVER NOT FOUND do break;
    
    for (row = 0; max_rows == 0 || row < max_rows; row++) {
	*(select_des->V[0]) = '\0';

        EXEC SQL WHENEVER SQLERROR GOTO stopover;		
        EXEC SQL AT :cdbid FETCH glistCursor USING DESCRIPTOR select_des;
	
	if ( *(select_des->I[0]) == -1)
	    strcpy(select_des->V[0], "<NULL>");

	list = g_list_prepend(list, g_strdup(select_des->V[0]));
	if (rowcallback && (rowcallback(row) == 0))
	    break;
    }
    EXEC SQL WHENEVER SQLERROR CONTINUE;
    EXEC SQL AT :cdbid CLOSE glistCursor;

  stopover:
    for (i=0; i < select_des->F; i++) { 
	g_free(select_des->V[i]);
    }
    sqlclu(select_des);

    return g_list_reverse(list);
    
  call_error:
    if(showerrors)
        sqlerr();
    return NULL;
}
*/

/* Fill an existing clist from a query. The supplied list can
 * have more columns that the query returns (up to MAX_COLS)
 * - extra columns will be left blank. If the list has fewer
 * columns then the rightmost from the result set will be ignored
 * Column titles will not be set
 */
/*
void
set_clist_from_query_old(
    GtkCList *clist,
    char *sql_query,
    char *databaseid,
    int  max_rows,
    int (*rowcallback)(int))
{
    EXEC SQL BEGIN DECLARE SECTION;
    char	 *cdbid;
    char	 *query;
    EXEC SQL END DECLARE SECTION;
    SQLDA	 *select_des = sqlald(MAX_COLS, MAX_VNAME_LEN, 30); 
    short        indicators[MAX_COLS];
    int          row = 0;
    int          i;
    int          null_ok;

    if (!select_des) {
	g_error("Out of memory allocating select descriptor\n");
	return;
    }

    cdbid = databaseid;
    query = sql_query;

    EXEC SQL WHENEVER SQLERROR do sqlerr();

    EXEC SQL AT :cdbid PREPARE prepQuery FROM :query;
    EXEC SQL AT :cdbid DECLARE setclistCursor CURSOR FOR prepQuery;
    EXEC SQL AT :cdbid OPEN setclistCursor;

    select_des->N = MAX_COLS;
    for (i = 0; i < MAX_COLS; i++)
	select_des->I[i] = &indicators[i];

    EXEC SQL AT :cdbid DESCRIBE SELECT LIST FOR prepQuery INTO select_des;

    if (select_des->F < 0) {
	show_warning_msg("Query returns too many columns (%d), maximum is %d\n",
		-(select_des->F), MAX_COLS);
	return;
    }

    select_des->N = select_des->F;

    gtk_clist_clear(GTK_CLIST(clist));

    for (i=0; i < select_des->F; i++) { 
	sqlnul (&(select_des->T[i]), &(select_des->T[i]), &null_ok);

	switch(select_des->T[i]) { 
	case ORA_TINT_NUMBER: {
	    int prec;
	    int scal;
	    sqlprc(&(select_des->L[i]), &prec, &scal); 
	    if (prec == 0)
		prec = 40; 
	    select_des->L[i] = prec + 2; 
	    if (scal < 0) 
		select_des->L[i] += -scal; 
	    break; 
	}
	case ORA_TINT_ROWID: 
	    select_des->L[i] = 18; 
	    break; 
	case ORA_TINT_DATE: 
	    select_des->L[i] = 9; 
	    break; 
	case ORA_TINT_LONG: 
	case ORA_TINT_LONGRAW:
	    select_des->L[i] = 131072; 
	    break; 
	case ORA_TINT_VARCHAR2: 
	case ORA_TINT_RAW: 
	    break; 

	} 
	select_des->V[i] = g_malloc(select_des->L[i]+7);
	if (i < clist->columns)
	    gtk_object_set_data(GTK_OBJECT(gtk_clist_get_column_widget (clist, i)), 
				"coltype", (gpointer)((int)select_des->T[i]));
	select_des->L[i] += 1; 
	select_des->T[i] = ORA_TEXT_STRING; 
    }

    EXEC SQL WHENEVER NOT FOUND do break;
    
    for (row = 0; max_rows == 0 || row < max_rows; row++) {
	for (i = 0; i < select_des->F; i++)
	    *(select_des->V[i]) = '\0';

        EXEC SQL WHENEVER SQLERROR GOTO stopover;		
        EXEC SQL AT :cdbid FETCH setclistCursor USING DESCRIPTOR select_des;
	
	for (i = 0; i < select_des->F; i++)
	    if ( *(select_des->I[i]) == -1)
		strcpy(select_des->V[i], "<NULL>");

	gtk_clist_append( clist, select_des->V);
	if (rowcallback && (rowcallback(row) == 0))
	    break;
    }
    EXEC SQL WHENEVER SQLERROR CONTINUE;
    EXEC SQL AT :cdbid CLOSE setclistCursor;

  stopover:
    for (i=0; i < select_des->F; i++) { 
	g_free(select_des->V[i]);
    }
    sqlclu(select_des);
    return;
}
*/

/* Execute the specified SQL statement */
/*
int
execute_sql(
    const char	 *databaseid,
    const char	 *sql_stmt,
    ...)
{
    EXEC SQL BEGIN DECLARE SECTION;
    const char	 *cdbid;
    const char	 *query;
    EXEC SQL END DECLARE SECTION;
    SQLDA	 *bind_des  = sqlald(MAX_COLS, MAX_VNAME_LEN, 30); 
    short	 indicators[MAX_COLS];
    int		 i;
    int		 ret	    = 0;
    va_list	 ap;

    if (!bind_des) {
	g_error("Out of memory allocating select descriptor\n");
	return -1;
    }

    cdbid = databaseid;
    query = sql_stmt;
	    
    EXEC SQL WHENEVER SQLERROR do sqlerr();
    EXEC SQL WHENEVER NOT FOUND CONTINUE;

    EXEC SQL AT :cdbid PREPARE prepQuery FROM :query;

    bind_des->N = MAX_COLS;
    for (i = 0; i < MAX_COLS; i++)
	bind_des->I[i] = &indicators[i];

    EXEC SQL AT :cdbid DESCRIBE BIND VARIABLES FOR prepQuery INTO bind_des;

    if (bind_des->F < 0) {
	show_warning_msg("Query returns too many columns (%d), maximum is %d\n",
		-(bind_des->F), MAX_COLS);
	return -1;
    }

    bind_des->N = bind_des->F;

    va_start(ap, sql_stmt);
    for (i = 0 ; i < bind_des->F; i++) {
	bind_des->T[i] = ORA_TEXT_STRING;
	bind_des->V[i] = va_arg(ap, char *);
	bind_des->L[i] = strlen(bind_des->V[i])+1;
	*(bind_des->I[i]) = 0;
    }
    va_end(ap);
    
    EXEC SQL AT :cdbid EXECUTE prepQuery USING DESCRIPTOR bind_des;
    
    EXEC SQL WHENEVER SQLERROR CONTINUE;
    ret = 1;
    
  stopover:
    sqlclu(bind_des);
    return ret;
}
*/
/*******************************************/
/*          Start Matthew Edits            */
/*******************************************/

/*int
execute_login(
    char    *cstring, 
    char    *dbid)
{
    EXEC SQL BEGIN DECLARE SECTION;
    char    *bound_cstring;
    char    *bound_dbid;
    EXEC SQL END DECLARE SECTION;
    
    bound_cstring = cstring;
    bound_dbid    = dbid;
    
    EXEC SQL WHENEVER SQLERROR goto failed_login;
    EXEC SQL CONNECT :bound_cstring AT :bound_dbid;

    return 1;
    
    failed_login:
        return 0;

}*/

/*
int
execute_immediate_sql(
    const char	 *databaseid,
    const char	 *sql_stmt)
{
    EXEC SQL BEGIN DECLARE SECTION;
    char    *query;
    char    *dbid;
    EXEC SQL END DECLARE SECTION;
    
    dbid  = (char *)databaseid;
    query = (char *)sql_stmt;
    EXEC SQL WHENEVER SQLERROR GOTO stopover;
    EXEC SQL AT :dbid EXECUTE IMMEDIATE :query;
    
    return 1;
    
    stopover:
        obj_sql_error();
        return 0;

    long int    err;
    OCISvcCtx	*svchp	     = 0;
    
    svchp = (OCISvcCtx *)databaseid;
    
    err = OCIStmtPrepare(stmthp, errhp, (text *) sql_stmt,
                        (ub4) strlen((const char *) sql_stmt),
                        (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));
    err = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1,
                         (ub4) 0, (OCISnapshot *) NULL,
                         (OCISnapshot *) NULL, (ub4) OCI_DEFAULT));
    err = OCITransCommit(svchp, errhp, (ub4) 0));

}
*/

/*
char *
getrowcount(
    char    *databaseid,
    char    *sql_query)
{
    EXEC SQL BEGIN DECLARE SECTION;
    char      *query;
    char      *dbid;
    varchar   count[15];
    char      *ret_count;
    EXEC SQL END DECLARE SECTION;

    query = sql_query;
    dbid  = databaseid;

    EXEC SQL WHENEVER SQLERROR goto stopover;

    EXEC SQL AT :dbid PREPARE prepQuery FROM :query;
    EXEC SQL AT :dbid DECLARE getcount CURSOR FOR prepQuery;
    EXEC SQL AT :dbid OPEN getcount;

    EXEC SQL AT :dbid
        FETCH getcount
        INTO :count;
	
    EXEC SQL AT :dbid CLOSE getcount;

    count.arr[count.len]='\0';
    ret_count = count.arr;
    ret_count = ret_count ? strdup(ret_count) : 0;

    return ret_count;

    stopover:
        obj_sql_error();
	return NULL;

}
*/



