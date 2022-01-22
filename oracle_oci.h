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
/* Useful oracle stuff not found in their headers */

/* Oracle internal variable types */
typedef enum {
    ORA_TINT_VARCHAR2= 1,
    ORA_TINT_NUMBER  = 2,
    ORA_TINT_LONG    = 8,  
    ORA_TINT_ROWID   = 11,  
    ORA_TINT_DATE    = 12,
    ORA_TINT_RAW     = 23,
    ORA_TINT_LONGRAW = 24,
    ORA_TINT_CHAR    = 96} OracleInternalType;

/* Oracle external variable types */
typedef enum {
    ORA_TEXT_VARCHAR2     = 1,
    ORA_TEXT_NUMBER       = 2,
    ORA_TEXT_INTEGER      = 3,
    ORA_TEXT_FLOAT        = 4,
    ORA_TEXT_STRING       = 5,
    ORA_TEXT_VARNUM       = 6,
    ORA_TEXT_DECIMAL      = 7,
    ORA_TEXT_LONG         = 8,
    ORA_TEXT_VARCHAR      = 9,
    ORA_TEXT_ROWID        = 11,
    ORA_TEXT_DATE         = 12,
    ORA_TEXT_VARRAW       = 15,
    ORA_TEXT_RAW          = 23,
    ORA_TEXT_LONGRAW      = 24,
    ORA_TEXT_UNSIGNED     = 68,
    ORA_TEXT_DISPLAY      = 91,
    ORA_TEXT_LONGVARCHAR  = 94,
    ORA_TEXT_LONGVARRAW   = 95,
    ORA_TEXT_CHAR         = 96,
    ORA_TEXT_CHARZ        = 97} OracleExternalType;


/* Oracle database functions - in dboracle.c */
typedef int word;
extern word osnsui(word *handlp, void (*astp), char * ctx);


