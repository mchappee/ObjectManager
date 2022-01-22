/* SQLWork - the Oracle Query tool
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
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
       
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <oci.h>

#include "oracle_oci.h"
/*#include "common.h"*/
#include "include/objectman.h"
#include "sqlcpr.h"

#define MAX_COLS 256
#define MAX_VNAME_LEN 32

#define list_set_opt_col_width(clist, col)     \
    gtk_clist_set_column_width (GTK_CLIST(clist), (col), \
				gtk_clist_optimal_column_width (GTK_CLIST(clist), (col)))


static int oci_initialised = 0;
static OCIEnv     *envhp;
static OCIError   *errhp;
static GHashTable *login_hash_table  = 0;
GtkCList          *clist;
gboolean          global_errors;

static void oracle_sql_error(OCIError *errhp);

int
oracle_initialise(void)
{
    assert(!oci_initialised);

    if (OCIInitialize(OCI_OBJECT, 0, 
		      (dvoid * (*)(dvoid *, size_t)) 0,
		      (dvoid * (*)(dvoid *, dvoid *, size_t))0, 
		      (void (*)(dvoid *, dvoid *)) 0 )) {     
	return OCI_ERROR;
    }
    
    if (OCIEnvInit(&envhp, OCI_DEFAULT, 0, (dvoid **) 0 )) {
	return OCI_ERROR;
    }

    if (OCIHandleAlloc(envhp, (dvoid **)&errhp, (ub4) OCI_HTYPE_ERROR, 0, 0)) {
	return OCI_ERROR;
    }
 
    login_hash_table =  g_hash_table_new(g_str_hash, g_str_equal);
    oci_initialised = 1;
    return OCI_SUCCESS;
}


int
oracle_login(
    const char	 *cstring,
    const char	 *dbid)
{
    OCISvcCtx	 *svchp;
    char	 *user	  = getusername(cstring);
    char	 *passwd  = getpassword(cstring);
    char	 *db	  = getdb(cstring);
    int          dblen    = 0;

    assert(oci_initialised);
    
    if (g_hash_table_lookup (login_hash_table, dbid)) {
	return 1;
    }

    if(db)
        dblen = strlen(db);

    if (OCILogon(envhp, errhp, &svchp, user, strlen (user),
		 passwd, strlen (passwd), db, dblen) == OCI_ERROR) {
	return 0;
    }

    g_hash_table_insert (login_hash_table, g_strdup(dbid), svchp);
    return 1;
}

static void
oci_logoff(
    gpointer	key,
    gpointer	value,
    gpointer	user_data)
{
    OCILogoff ((OCISvcCtx *)value, errhp);
}

int
oracle_finalise(void)
{
    assert(oci_initialised);
    g_hash_table_foreach(login_hash_table, oci_logoff, 0);

    if (errhp)
	OCIHandleFree((dvoid *) errhp, OCI_HTYPE_ERROR);
    
    if (envhp)
	OCIHandleFree((dvoid *) envhp, OCI_HTYPE_ENV);

    return 0;
}

/* Do a query - for each row returned pass the data to a callback */
/* FIXME - write the other routines in this file in terms of this one */
    
int
oracle_do_query(
    const char	*sql_query,
    const char	*databaseid,
    int		max_rows,
    int		(*rowcallback)(int, int, char **, short *, char **, ub2 *, short *, int *, gpointer),
    int         (*rowcallback_callback)(int),
    gpointer	user_data)
{
    OCIParam	*parmdp	     = 0; 
    OCIStmt	*stmthp	     = 0;
    OCISvcCtx	*svchp	     = 0;
    OCIDefine   **defn       = 0;
    char	*colname;
    int		col;
    int		retval	     = OCI_ERROR;
    long int	err; 
    sb1		datascale;
    ub1		dataprec;
    ub2		datasize;
    ub2		stmttype;
    ub4		colnamelen;
    ub4		parmcnt      = 0; 
    
    char	**colnames   = 0;
    short	*indicators  = 0; 
    char	**values     = 0; 
    ub2	        *values_l    = 0; 
    ub2		*datatypes   = 0;
    short	*types	     = 0; 
    int		row	     = 0; 
    OCIRef	**refs       = 0;
    ub4	        *ref_len     = 0;
    
    assert(oci_initialised);
    if ((svchp = (OCISvcCtx *)g_hash_table_lookup (login_hash_table, databaseid)) == 0) {
	/*return OCI_ERROR;*/
    }

    logsql((char *)sql_query);

    err = OCIHandleAlloc( envhp, (dvoid **) &stmthp, OCI_HTYPE_STMT, 0, 0);

    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIHandleAlloc(OCI_HTYPE_STMT) failed %ld\n", err);
	goto cleanup;
    }

    err = OCIStmtPrepare (stmthp, errhp, (char *) /* Grrr */ sql_query,
			  (ub4)strlen(sql_query), 
			  (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT); 
    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIStmtPrepare failed %ld\n", err);
	goto cleanup;
    }

    err = OCIAttrGet(stmthp, OCI_HTYPE_STMT, &stmttype,
		     0, OCI_ATTR_STMT_TYPE, errhp);

    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIAttrGet(OCI_ATTR_STMT_TYPE) failed %ld\n", err);
	if(global_errors)
	    oracle_sql_error(errhp);
	goto cleanup;
    }

    err = OCIStmtExecute (svchp, stmthp, errhp, stmttype == OCI_STMT_SELECT ? 0 : 1, 0, 
			  (OCISnapshot*) 0, (OCISnapshot*)0, OCI_DEFAULT); 
 
    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIStmtExecute failed %ld\n", err);
	if(global_errors)
	    oracle_sql_error(errhp);
	goto cleanup;
    }

    if (stmttype == OCI_STMT_SELECT) {
	/* */
    
	/* get the number of columns in the select list */ 
	err = OCIAttrGet (stmthp, OCI_HTYPE_STMT, (dvoid *) &parmcnt, 
			  (ub4 *) 0, (ub4)OCI_ATTR_PARAM_COUNT, errhp); 

	if (err != OCI_SUCCESS) {
	    fprintf(stderr, "OCIAttrGet(OCI_ATTR_PARAM_COUNT) failed %ld\n", err);
	    if(global_errors)
	        oracle_sql_error(errhp);
	    goto cleanup;
	}
	
	/* Allocate storage for callback and define handles */
	colnames	= g_malloc(parmcnt * sizeof(char *));
	values	        = g_malloc(parmcnt * sizeof(char *));
	values_l	= g_malloc(parmcnt * sizeof(ub2));
	indicators	= g_malloc(parmcnt * sizeof(sb2));
	defn	        = g_malloc(parmcnt * sizeof(OCIDefine *));
	datatypes	= g_malloc(parmcnt * sizeof(ub2));
	refs	        = g_malloc(parmcnt * sizeof(OCIRef *));
	ref_len    	= g_malloc(parmcnt * sizeof(ub4));
	
	for (col = 0; col < parmcnt; col++) {
	    colnames[col] = 0;
	    values[col] = 0;
	    defn[col] = 0;
	    refs[col] = 0;
	    ref_len[col] = 0;
	}

#define OCICGetAttr(attributep, sizep, attrtype, errhp) \
    if ((err = OCIAttrGet(parmdp, OCI_DTYPE_PARAM, attributep, sizep, attrtype, errhp)) != OCI_SUCCESS) {\
        fprintf(stderr, "OCIAttrGet(" #attrtype ") failed\n");\
	if(global_errors) \
            oracle_sql_error(errhp);\
	goto cleanup; }

#define OCICDefineByPos(defnpp, errhp, valuep, value_sz, dty, indp, rlenp) \
    if ((err = OCIDefineByPos(stmthp, defnpp, errhp, col+1, valuep, value_sz,dty, \
                              indp, rlenp, 0, OCI_DEFAULT))!= OCI_SUCCESS) {\
        fprintf(stderr, "OCIDefineByPos for col %d failed\n", col+1);\
	if(global_errors) \
            oracle_sql_error(errhp);\
	goto cleanup; }

	/* get describe information for each column */
	for (col = 0; col < parmcnt; col++) { 
	    err = OCIParamGet (stmthp, OCI_HTYPE_STMT, errhp, (dvoid **)&parmdp, col+1); 
	    if (err != OCI_SUCCESS) {
		fprintf(stderr, "OCIParamGet(%d/%d) failed %ld\n", col, parmcnt, err);
		if(global_errors)
		    oracle_sql_error(errhp);
		goto cleanup;
	    }
	    
	    OCICGetAttr(&colname, &colnamelen, OCI_ATTR_NAME, errhp);
	    /* There's no guarentee that there's room to terminate the
	       supplied string so we must copy it out */
	    colnames[col] = g_malloc(colnamelen+1);
	    memcpy(colnames[col], colname, colnamelen);
	    colnames[col][colnamelen] = '\0'; 

	    OCICGetAttr(&datatypes[col], (ub4 *) 0, OCI_ATTR_DATA_TYPE, errhp);
	    
	    switch (datatypes[col]) {
	    case SQLT_RID:
	    case SQLT_DAT:
	    case SQLT_CHR:
	    case SQLT_AFC:
	    case SQLT_BIN:
	    case SQLT_LNG:
	    case SQLT_NUM:
		switch (datatypes[col]) {
		case SQLT_NUM:
		    /* Numeric types */
		    OCICGetAttr(&datascale, (ub4 *) 0, OCI_ATTR_SCALE, errhp);
		    OCICGetAttr(&dataprec, (ub4 *) 0, OCI_ATTR_PRECISION, errhp);
		    if (dataprec == 0)
			dataprec = 40; 
		    datasize = dataprec + 2;
		    if (datascale < 0) 
			datasize += -datascale; 
		    
		    break;
		case SQLT_CHR:
		case SQLT_AFC:
		case SQLT_BIN:
		    /* Character/raw types - use datasize directly */
		    /*OCICGetAttr(&datasize, (ub4 *) 0, OCI_ATTR_DATA_SIZE, errhp);*/
		    datasize = 40960;
		    break;
		case SQLT_RID:
		    datasize = 18;
		    break;
		case SQLT_DAT:
		    datasize = 100;
		    break;
		case SQLT_LNG:
		    datasize = 40960;
		    break;
		}
		values_l[col] = MAX(datasize+1, 7);
		values[col] = g_malloc(values_l[col]);
		OCICDefineByPos(&defn[col], errhp, values[col], datasize+1, 
				SQLT_STR, &indicators[col], &values_l[col]);
		break;
	    case SQLT_LBI:
		break;
	    case SQLT_NTY: {
		char	     *type_name	 = 0;
		ub2	     type_size	 = 0;
		ub1	     ptype;
		OCIRef	     *ref_tdo    = 0;
		OCIType	     *tdo	 = 0;


		OCICGetAttr(&ptype,     (ub4 *) 0, OCI_ATTR_PTYPE,     errhp);
		OCICGetAttr(&type_size, (ub4 *) 0, OCI_ATTR_DATA_SIZE, errhp);
		OCICGetAttr(&type_name, (ub4 *) 0, OCI_ATTR_TYPE_NAME, errhp);
		
		OCICGetAttr(&ref_tdo,   (ub4 *) 0, OCI_ATTR_REF_TDO,   errhp);
		err = OCIObjectPin(envhp, errhp, ref_tdo, (OCIComplexObject *) 0,
				   OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE,
				   (dvoid **)&tdo);

		if (err != OCI_SUCCESS) {
		    fprintf(stderr, "OCIObjectPin failed %ld\n", err);
		    if(global_errors)
		        oracle_sql_error(errhp);
		    goto cleanup;
		}

		fprintf(stderr, "Col %d ptype %d type %s size %d\n", col, ptype, type_name, type_size);

		values_l[col] = 0;
		values[col] = 0;
		OCICDefineByPos(&defn[col], errhp, 0, 0, 
				SQLT_NTY, &indicators[col], &values_l[col]);

		err = OCIDefineObject(defn[col], errhp, tdo, (dvoid **)&refs[col], &ref_len[col], 
				      (dvoid **)0, (ub4 *)0);
		if (err != OCI_SUCCESS) {
		    fprintf(stderr, "OCIDefineObject failed %ld\n", err);
		    if(global_errors)
		        oracle_sql_error(errhp);
		    goto cleanup;
		}


		break;
	    }

	    case SQLT_REF:
	    case SQLT_CLOB:
	    case SQLT_BLOB:

	    default:
	    }
	}

	/* By this point we have the colum definitions so call the
	   user supplied function (if there was one) */
	if (rowcallback)
	    rowcallback(-1, parmcnt, colnames, 0, 0, values_l, datatypes, (void *)rowcallback_callback, user_data);
	    
	while ((err = OCIStmtFetch(stmthp, errhp, 1, 
				   OCI_FETCH_NEXT, OCI_DEFAULT)) == OCI_SUCCESS ||
	       err == OCI_SUCCESS_WITH_INFO) {
	    if (rowcallback)
		rowcallback(row++, parmcnt, colnames, indicators, 
			    values, values_l, datatypes, (void *)rowcallback_callback, user_data);
			    			    
	}
	if ( err != OCI_NO_DATA ) {
	    fprintf(stderr, "OCIStmtFetch failed with %ld on row %d\n", err, row);
	    if(global_errors)
	        oracle_sql_error(errhp);
	    goto cleanup;
	}
	
    }

    retval = OCI_SUCCESS;
  cleanup:
    if (stmthp)
	OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    if (colnames) {
	for (col = 0; col < parmcnt; col++)
	    if (colnames[col])
		g_free(colnames[col]);
	g_free(colnames);
    }
    if (values) {
	for (col = 0; col < parmcnt; col++)
	    if (values[col])
		g_free(values[col]);
	g_free(values);
    }
    if (indicators)
	g_free(indicators);
    if (datatypes)
	g_free(datatypes);
    if (values_l)
	g_free(values_l);
    if (types)
	g_free(types);

    return retval;
}

/* Execute the specified SQL statement */
int
execute_sql(
    const char	 *databaseid,
    const char	 *sql_stmt,
    ...)
{
    OCIStmt	 *stmthp  = 0;
    OCISvcCtx	 *svchp	  = 0;
    int		 retval	  = OCI_ERROR;
    long int	 err; 
    ub2		 stmttype;
    va_list	 ap;
    
    OraText	 **bvnp	  = 0;
    ub1		 *bvnl	  = 0;
    OraText	 **invp	  = 0;
    ub1		 *inpl	  = 0;
    ub1		 *dupl	  = 0;
    OCIBind	 **hndl	  = 0;
    sb4		 nbindvars;
    sb4          bv;

    logsql((char *)sql_stmt);

    assert(oci_initialised);
    if ((svchp = (OCISvcCtx *)g_hash_table_lookup (login_hash_table, databaseid)) == 0) {
	/*return OCI_ERROR;*/
    }

    err = OCIHandleAlloc( envhp, (dvoid **) &stmthp, OCI_HTYPE_STMT, 0, 0);

    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIHandleAlloc(OCI_HTYPE_STMT) failed %ld\n", err);
	goto cleanup;
    }

    err = OCIStmtPrepare (stmthp, errhp, (char *) /* Grrr */ sql_stmt,
			  (ub4)strlen(sql_stmt), 
			  (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT); 
    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIStmtPrepare failed %ld\n", err);
	goto cleanup;
    }

    nbindvars = 32;
    bvnp = g_malloc(nbindvars * sizeof(*bvnp));
    bvnl = g_malloc(nbindvars * sizeof(*bvnl));
    invp = g_malloc(nbindvars * sizeof(*invp));
    inpl = g_malloc(nbindvars * sizeof(*inpl));
    dupl = g_malloc(nbindvars * sizeof(*dupl));
    hndl = g_malloc(nbindvars * sizeof(*hndl));

    for (bv = 0; bv < nbindvars; bv++) {
	hndl[bv] = 0;
    }

    /* Find No of bind variables */

    err = OCIStmtGetBindInfo ( stmthp, errhp, nbindvars, 1,
			       &nbindvars,
			       bvnp, bvnl, invp, inpl, dupl, hndl);
    
    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIStmtGetBindInfo failed %ld\n", err);
	oracle_sql_error(errhp);
	goto cleanup;
    }

    if (nbindvars < 0) {
	nbindvars *= -1;
	bvnp = g_malloc(nbindvars * sizeof(*bvnp));
	bvnl = g_malloc(nbindvars * sizeof(*bvnl));
	invp = g_malloc(nbindvars * sizeof(*invp));
	inpl = g_malloc(nbindvars * sizeof(*inpl));
	dupl = g_malloc(nbindvars * sizeof(*dupl));
	hndl = g_malloc(nbindvars * sizeof(*hndl));
	for (bv = 0; bv < nbindvars; bv++) {
	    hndl[bv] = 0;
	}

	err = OCIStmtGetBindInfo ( stmthp, errhp, nbindvars, 1,
				   &nbindvars,
				   bvnp, bvnl, invp, inpl, dupl, hndl);

	if (err != OCI_SUCCESS) {
	    fprintf(stderr, "OCIAttrGet(OCI_ATTR_STMT_TYPE) failed %ld\n", err);
	    oracle_sql_error(errhp);
	    goto cleanup;
	}

    }

    
    va_start(ap, sql_stmt);
    for (bv = 1; bv <= nbindvars; bv++) {
	char *var = va_arg(ap, char *);
	err = OCIBindByPos(stmthp, &hndl[bv], errhp, bv,
			   var, strlen(var)+1, SQLT_STR, 
			   (dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0,
			   OCI_DEFAULT);
	if (err != OCI_SUCCESS) {
	    fprintf(stderr, "OCIBindByPos for %s failed\n", bvnp[bv-1]);
	    oracle_sql_error(errhp);
	    goto cleanup;
	}
    }
    va_end(ap);

    err = OCIAttrGet(stmthp, OCI_HTYPE_STMT, &stmttype,
		     0, OCI_ATTR_STMT_TYPE, errhp);

    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIAttrGet(OCI_ATTR_STMT_TYPE) failed %ld\n", err);
	oracle_sql_error(errhp);
	goto cleanup;
    }

    err = OCIStmtExecute (svchp, stmthp, errhp, stmttype == OCI_STMT_SELECT ? 0 : 1, 0, 
			  (OCISnapshot*) 0, (OCISnapshot*)0, OCI_DEFAULT); 
 
    if (err != OCI_SUCCESS) {
	fprintf(stderr, "OCIStmtExecute failed %ld\n\nAssume Oracle 7.3.x", err);
	retval = -1;
	goto cleanup;
    }

    if (stmttype == OCI_STMT_SELECT) {
	/* this is really an error - if the query is a select than it should
	 * be passed to one of the routines which know how to handle a result set
	 */
    }

    retval = OCI_SUCCESS;
  cleanup:
    if (bvnp) g_free(bvnp);
    if (bvnl) g_free(bvnl);
    if (invp) g_free(invp);
    if (inpl) g_free(inpl);
    if (dupl) g_free(dupl);
    if (hndl) g_free(hndl);
    if (stmthp) OCIHandleFree(stmthp, OCI_HTYPE_STMT);

    return retval;
}

static void
oracle_sql_error(
    OCIError  *errhp)
{
    text errbuf[512];
    
    if (errhp) {
	sb4    *errcodep;
	sb4    errcode;
	
	errcodep = &errcode;
	
	(void) OCIErrorGet(errhp, (ub4) 1, (text *) NULL, errcodep,
			   errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
	(void) fprintf(stderr, "Error - %.*s\n", 512, errbuf);
        logsql((char *)errbuf);
	msgbox(errbuf);
    }
}

/* Return a list containing the first column from a sql query.
 * Caller is responsible for freeing the list and the data contained 
 * in each entry */
 
int
glist_from_query_callback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data)
{
    GList    *list = user_data;

    if (row >= 0){
	list = g_list_append(list, g_strdup(values[0]));
	if (rowcallback)
	    rowcallback(row);
    }

    return TRUE;

}

GList *
glist_from_query(
    const char    *sql_query,
    const char    *databaseid,
    int           max_rows,
    gboolean      showerrors,
    int          (*rowcallback)(int))
{
    GList    *list = 0;
    int    msg=0;
    
    global_errors = showerrors;
    
    if(rowcallback==NULL){
        rowcallback=active_step;
	msg = 1;
    }
    
    list = g_list_append(list, "Pointers_Make_My_Brain_Hurt_--MC");
    if(oracle_do_query(sql_query, databaseid, 0, (void *)glist_from_query_callback, rowcallback, list) == OCI_SUCCESS){
        list = g_list_remove(list, "Pointers_Make_My_Brain_Hurt_--MC");
	if(msg)
	    active_clear();
	return list;
    }else{
        g_list_free(list);
	if(msg)
	    active_clear();
        return NULL;
    }
}

int
clist_from_query_callback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data)
{
    int         col;
    GtkCList    *swap_list;
    
    if ( row == -1 && user_data == NULL)
        clist = (GtkCList *)gtk_clist_new_with_titles (ncols, colnames);
	
    if(user_data == NULL)    
        swap_list = clist;
    else
        swap_list = GTK_CLIST(user_data);

    if (row >= 0){
    
        for (col = 0; col < ncols; col++)
	    if ( indicators[col] == -1)
		strcpy(values[col], "<NULL>");

	gtk_clist_append(swap_list, values);
	if (rowcallback)
	    rowcallback(row);
    }

    return TRUE;

}

GtkCList *
clist_from_query(
    const char *sql_query,
    const char *databaseid,
    int  max_rows,
    char *nullsubst,
    int (*rowcallback)(int))
{
    int    msg=0;
    
    global_errors = TRUE;
    
    if(rowcallback==NULL){
        rowcallback=active_step;
	msg = 1;
    }
	
    if(oracle_do_query(sql_query, databaseid, max_rows, (void *)clist_from_query_callback, rowcallback, NULL) == OCI_SUCCESS){ 
       if(msg)
	    active_clear();
	return clist;
    }else{
        if(msg)
	    active_clear();
        return NULL;
    }
}

void
set_clist_from_query(
    GtkCList   *clist,
    const char *sql_query,
    const char *databaseid,
    int  max_rows,
    int (*rowcallback)(int))
{
    int    msg=0;
    
    if(rowcallback==NULL){
        rowcallback=active_step;
	msg = 1;
    }
    
    if(oracle_do_query(sql_query, databaseid, max_rows, (void *)clist_from_query_callback, rowcallback, clist) == OCI_SUCCESS){
        if(msg)
	    active_clear();
	return;
    }else{
        if(msg)
	    active_clear();
        return;
    }
}

static int
clist_from_query_by_row_rowcallback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data)
{
    GtkCList *clist = (GtkCList *)user_data;
    int i;
	
    /* For the first row insert the column titles into col 0 in the list */
    if (row > -1){
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
	    if (indicators[i] == -1)
		gtk_clist_set_text(clist, i, row+1, "");
	    else
		gtk_clist_set_text(clist, i, row+1, values[i]);
	}
    }
    return TRUE;
}

GtkCList *
clist_from_query_by_row(
    const char *sql_query,
    const char *databaseid,
    int  ncols,
    char **titles,
    int (*rowcallback)(int))
{

    if (titles) {
	clist = (GtkCList *)gtk_clist_new_with_titles(ncols, titles);
    } else {
	clist = (GtkCList *)gtk_clist_new(ncols);
    }
    
    if(oracle_do_query(sql_query, databaseid, ncols - 1, (void *)clist_from_query_by_row_rowcallback, NULL, clist) == OCI_SUCCESS)
	return clist;
    else
        return NULL;
	
    return clist;
}

int
source_from_query_callback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data)
{

    if (row >= 0)
        gtk_text_insert(GTK_TEXT(user_data), NULL, NULL, NULL, values[0], strlen(values[0]));

    return TRUE;

}

char *
insert_code_owner(
    char    *code,
    char    *owner,
    char    *procname)
{
    char    *token1;
    char    *string_voodoo;
    int     a=0;
    
    token1 = strnocasestr(code, procname);
    if(token1)
        a = strlen(code) - strlen(token1);
    else
        return NULL;

    string_voodoo = g_malloc(strlen(code) + strlen(owner) + 2);
    strncpy(string_voodoo, code, a);
    sse(ssb(owner));
    strcpy(string_voodoo+a, owner);
    strcat(string_voodoo, ".");
    strcat(string_voodoo, token1);
    
    return string_voodoo;
}


char *
source_from_query(
    const char *sql_query,
    const char *databaseid,
    const char *owner,
    const char *procname)
{
    GtkWidget  *text;
    char       *cp;
    char       *buffer_cat;
    char       *ownered_code;
	
    text = gtk_text_new(NULL, NULL);
    
    if(oracle_do_query(sql_query, databaseid, 0, (void *)source_from_query_callback, NULL, text) == OCI_SUCCESS){
	cp = gtk_editable_get_chars(GTK_EDITABLE (text),(gint) 0,(gint) gtk_text_get_length (GTK_TEXT (text)));
	if(cp!=NULL){
	    ownered_code = insert_code_owner(cp, (char *)owner, (char *)procname);	
	    g_free(cp);
	    if(!ownered_code)
	        return NULL;
            
	    buffer_cat = malloc(strlen(ownered_code) + 200);
	    
	    strcpy(buffer_cat, "CREATE OR REPLACE ");
	    strcat(buffer_cat, ownered_code);
	    g_free(ownered_code);
	    gtk_widget_destroy(text);
	}else
	    buffer_cat = NULL;
	    
	return buffer_cat;
    }else
        return NULL;

}


void 
obj_sql_error()
{ 
    char *error_message = g_malloc(128);
    char *sql_stmt = g_malloc(4096);
    char *emsg;
    int function_code;
    size_t buf_len, msg_len; 
    char msgtemplate[] = 
	"Internal SQL Error:\n\n"
	"An error occured while executing the sql statement\n"
	"   %s\n\n"
	"The error was\n"
	"   %s\n";

    buf_len = 127;
    sqlglm(error_message, &buf_len, &msg_len);
    if (msg_len > 127) {
	error_message = g_realloc(error_message, msg_len+1);
	buf_len = msg_len;
	sqlglm(error_message, &buf_len, &msg_len);
    }
    error_message[msg_len] = '\0';

    buf_len = 4095;
    sqlgls(sql_stmt, &buf_len, &function_code);
    sql_stmt[buf_len] = '\0';


    fprintf(stderr, "\n\n%.70s\n\n%s\n",sqlca.sqlerrm.sqlerrmc, sql_stmt); 
    emsg=g_malloc(strlen(msgtemplate)+strlen(sql_stmt)+strlen(error_message)+1);
    sprintf(emsg,"%s\n%s",sql_stmt, error_message);
    msgbox(emsg);
    
    g_free(emsg);
    g_free(error_message);
    g_free(sql_stmt);
}

void
oci_error(errhp, status)
{

    text errbuf[512];
    ub4 errcode;

	switch (status)
	{
	    case OCI_SUCCESS:
	        break;
	    case OCI_SUCCESS_WITH_INFO:
	        break;
	    case OCI_NEED_DATA:
	        fprintf(stderr,"ErrorOCI_NEED_DATA\n");
	        break;
	    case OCI_NO_DATA:
	        fprintf(stderr,"ErrorOCI_NO_DATA\n");
	        break;
	    case OCI_ERROR:
	        OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,
                errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
                logsql((char *)errbuf);
                msgbox(errbuf);
	        break;
	    case OCI_INVALID_HANDLE:
	        fprintf(stderr,"ErrorOCI_INVALID_HANDLE\n");
	        break;
	    case OCI_STILL_EXECUTING:
	        fprintf(stderr,"ErrorOCI_STILL_EXECUTE\n");
	        break;
	    case OCI_CONTINUE:
	        fprintf(stderr,"ErrorOCI_CONTINUE\n");
	        break;
	    default:
	        break;
	}

}

int
execute_immediate_sql(
    const char	 *databaseid,
    const char	 *sql_stmt)
{
    long int    err;
    OCIStmt     *stmthp = 0;
    OCISvcCtx   *svchp = 0;

    logsql((char *)sql_stmt);
	    
    svchp = (OCISvcCtx *)g_hash_table_lookup (login_hash_table, databaseid);
    err = OCIHandleAlloc( envhp, (dvoid **) &stmthp, OCI_HTYPE_STMT, 0, 0);
    
    err = OCIStmtPrepare(stmthp, errhp, (text *) sql_stmt,
                        (ub4) strlen((const char *) sql_stmt),
                        (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			
    if (err != OCI_SUCCESS && err != OCI_SUCCESS_WITH_INFO) {
        fprintf(stderr,"\nError in execute_immediate_sql (OCIStmtPrepare) Status %ld",err);
	oci_error(errhp,err);
    }
	
    err = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1,
                         (ub4) 0, (OCISnapshot *) NULL,
                         (OCISnapshot *) NULL, (ub4) OCI_DEFAULT);
    if (err != OCI_SUCCESS && err != OCI_SUCCESS_WITH_INFO) {
	fprintf(stderr,"\nError in execute_immediate_sql (OCIStmtExecute) Status %ld",err);
	oci_error(errhp,err);
	return 0;
    }
	
    err = OCITransCommit(svchp, errhp, (ub4) 0);
    if (err != OCI_SUCCESS && err != OCI_SUCCESS_WITH_INFO) {
	fprintf(stderr,"\nError in execute_immediate_sql (OCITransCommit) Status %ld",err);
	oci_error(errhp,err);
	return 0;
    }
	
    return 1;
}

int
execute_nonblocking_sql(
    const char	 *databaseid,
    const char	 *sql_stmt,
    GtkWidget    *timelabel)
{
    long int    err;
    OCIStmt     *stmthp = 0;
    OCISvcCtx   *svchp = 0;
    OCIServer   *srvhp = 0;
    time_t      now;
    time_t      later;
    int         elapsed;
    char        timestr[50];
    dvoid       *tmp;
    pid_t       pid;
    text errbuf[512];
    ub4 errcode;
    
    svchp = (OCISvcCtx *)g_hash_table_lookup (login_hash_table, databaseid);
    err = OCIHandleAlloc( envhp, (dvoid **) &stmthp, OCI_HTYPE_STMT, 0, 0);
    
    err = OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &srvhp,
                    (ub4) OCI_HTYPE_SERVER, 52, (dvoid **) &tmp);
		    
    err = OCIStmtPrepare(stmthp, errhp, (text *) sql_stmt,
                        (ub4) strlen((const char *) sql_stmt),
                        (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

    if (err != OCI_SUCCESS) {
        fprintf(stderr,"\nError in execute_immediate_sql (OCIStmtPrepare) Status %ld",err);
    }

/*   Leave this here as a NONBLOCKING HOWTO */
/*    err = OCIAttrSet ((dvoid *) srvhp, (ub4) OCI_HTYPE_SERVER, 
                             (dvoid *) 0, (ub4) 0, 
                             (ub4) OCI_ATTR_NONBLOCKING_MODE, errhp);
    if (err != OCI_SUCCESS)
    { 
        printf ("Unable to set non-blocking mode...\n"); 
	oci_error(errhp,err);
        return 0; 
    }*/

    now = time (NULL);
    
    pid = fork();

    if (pid == 0) {
        err = OCIStmtExecute(svchp, stmthp, errhp, (ub4) 1,
                         (ub4) 0, (OCISnapshot *) NULL,
                         (OCISnapshot *) NULL, (ub4) OCI_DEFAULT);
        if (err != OCI_SUCCESS){
	    OCIErrorGet ((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,
            errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
            msgbox(errbuf);
	}
	err = OCITransCommit(svchp, errhp, (ub4) 0);
        if (err != OCI_SUCCESS)
	    fprintf(stderr,"\nError in execute_immediate_sql (OCITransCommit) Status %ld\n",err);
	    
	_exit(0);
    }
    
    if (pid < 0) {
	perror("fork error");
	return 0;
    }
    	
    if(pid){
        while(err != -1){
	    waitpid(pid, NULL, WNOHANG);
	    err = kill(pid, 0);
            later = time (NULL);
            elapsed = later - now;
	    sprintf(timestr,"%d \nseconds elapsed",elapsed);
	    gtk_label_set_text(GTK_LABEL(timelabel),timestr);
	    while (gtk_events_pending())
	        gtk_main_iteration();
	}
    }


    return 1;
}


static void
free_glist_string_item(
    gpointer list_data,
    gpointer user_data)
{
    g_free(list_data);
}


char *
getrowcount(
    char    *databaseid,
    char    *sql_query)
{
    GList         *count;
    char          *hardcount;

    count = glist_from_query(sql_query, databaseid, 0, TRUE, NULL);       
	
    if(count != NULL)
        hardcount = g_strdup(count->data);
    else
        return NULL;
	
    g_list_foreach (count, free_glist_string_item, 0);
    g_list_free (count);
    
    return hardcount;

}


