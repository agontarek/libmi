/*******************************************************************************
 * Copyright (c) 2005 The Regents of the University of California.
 * This material was produced under U.S. Government contract W-7405-ENG-36
 * for Los Alamos National Laboratory, which is operated by the University
 * of California for the U.S. Department of Energy. The U.S. Government has
 * rights to use, reproduce, and distribute this software. NEITHER THE
 * GOVERNMENT NOR THE UNIVERSITY MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified
 * to produce derivative works, such modified software should be clearly marked,
 * so as not to confuse it with the version available from LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * LA-CC 04-115
 *
 * Copyright 2012-2014 Cray Inc. All Rights Reserved.
 *
 *******************************************************************************/

 /**
 * @author Clement chu
 *
 */

#ifdef HAVE_CONFIG_H
#include	<config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "MIList.h"
#include "MIResult.h"
#include "MIValue.h"
#include "MIOOBRecord.h"
#include "MISignalInfo.h"
#include "CLIOutput.h"

static int
getBoolean(char* value)
{
	if (value != NULL && strncmp(value, "Yes", 3) == 0) {
		return 1;
	}
	return 0;
}

void
CLIGetSigHandleList(MICommand *cmd, MIList** signals)
{
	MIList *oobs;
	MIOOBRecord *oob;
	MISignalInfo *sig;
	char *text = NULL;
	char *token;
	char *pch;
	int i;
	const char* delims[] = { " ", "\\" };

	*signals = MIListNew();

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL)
		return;

	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0' || *text == '\\') {
			continue;
		}

		if (strncmp(text, "Signal", 6) != 0 && strncmp(text, "Use", 3) != 0 && strncmp(text, "info", 4) != 0) {
			token = strdup(text);
			pch = strstr(token, delims[0]);
			if (pch == NULL) {
				continue;
			}
			sig = MISignalInfoNew();
			for (i=0; pch != NULL; i++,pch=strstr(token, delims[1])) {
				if (*pch == '\0') {
					break;
				}

				*pch = '\0';
				pch++;
				while (*pch == ' ' || *pch =='t') { //remove whitespace or t character
					pch++;
				}
				if (*pch == '\\') { //ignore '\\t' again
					pch += 2;
				}

				switch(i) {
					case 0:
						sig->name = strdup(token);
						break;
					case 1:
						sig->stop = getBoolean(token);
						break;
					case 2:
						sig->print = getBoolean(token);
						break;
					case 3:
						sig->pass = getBoolean(token);
						break;
					case 4:
						sig->desc = strdup(token);
						break;
				}
				token = strdup(pch);
			}
			free(token);
			free(pch);
			MIListAdd(*signals, (void *)sig);
		}
	}
}

double
CLIGetGDBVersion(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;
	char *			text;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) {
		return -1.0;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) {
		return -1.0;
	}

	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0') {
			continue;
		}
		while (*text == ' ') {
			text++;
		}

		/*
		 * linux self: GUN gdb 6.5.0
		 * fedore: GNU gdb Red Hat Linux (6.5-8.fc6rh)
		 * Mac OS X: GNU gdb 6.1-20040303 (Apple version gdb-384) (Mon Mar 21 00:05:26 GMT 2005)
		 */
		if (strncmp(text, "GNU gdb", 7) == 0) {
			/*
			 * bypass "GNU gdb"
			 */
			text += 8;

			/*
			 * find first digit
			 */
			while (*text != '\0' && !isdigit(*text))
				text++;

			/*
			 * Convert whatever is here to a double
			 */
			if (*text != '\0')
				return strtod(text, NULL);
		}
	}
	return -1.0;
}

char *
CLIGetGDBArch(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;
	char *			text;
	char *			arch_start;
	char *			arch_end;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) {
		return NULL;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) {
		return NULL;
	}

	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0') {
			continue;
		}
		while (*text == ' ') {
			text++;
		}
		
		// get GDB architecture, e.g. "x86" (without quotes)
		if (strncmp(text, "This GDB was configured as", 26) == 0) {
			arch_start = strchr(text, '"');
			// point past the '"'
			text = ++arch_start;
			arch_end = strchr(text, '"');

			// extract the string
			if ((text = malloc(sizeof(char)*(arch_end - arch_start + 1))) == (void *)0)
				return NULL;
				
			strncpy(text, arch_start, arch_end-arch_start);
			text[arch_end-arch_start] = '\0';
			
			return text;
		}
	}
	return NULL;
}

/*
 * Attempt to fix up the type information returned by the ptype command.
 *
 * - Replace newlines and tabs with spaces and replace multiple spaces with a single space.
 * - Remove "\\n" sequences.
 * - Discard '{' and any following characters
 *
 * Space allocated for the result must be freed by the caller.
 */
static char *
fix_type(char *str)
{
	int		finished = 0;
	int		seen_space = 0;
	int		seen_backslash = 0;
	char *	s = str;
	char *	r;
	char * 	result = (char *)malloc(strlen(str));

	/*
	 * Remove leading whitespace
	 */
	while (isspace(*s)) {
		s++;
	}

	for (r = result; *s != '\0' && !finished; s++) {
		switch (*s) {
		case ' ':
		case '\n':
		case '\t':
			if (!seen_space) {
				*r++ = ' ';
				seen_space = 1;
			}
			break;

		case '\\':
			seen_backslash = 1;
			break;

		case '{':
			finished = 1;
			break;

		default:
			if (!seen_backslash) {
				*r++ = *s;
			}
			seen_backslash = 0;
			seen_space = 0;
		}
	}

	*r = '\0';

	/*
	 * Remove trailing whitespace
	 */
	r--;
	while (isspace(*r) && r >= result) {
		*r-- = '\0';
	}

	return result;
}

/*
 * Attempt to fix up the full type string returned by the ptype command.
 *
 * - Change "\\n" sequences to "\n".
 *
 * Space allocated for the result must be freed by the caller.
 */
static char *
fix_full_type(char *str)
{
	char *	s = str;
	char *	r;
	char * 	tmp = (char *)malloc(strlen(str));
	char *	result;
	int		seen_backslash = 0;

	for (r = tmp; *s != '\0'; s++) {
		switch (*s) {
			case '\\':
				++seen_backslash;
				break;

			case 'n':
				if (seen_backslash) {
					*r++ = '\n';
					seen_backslash = 0;
					break;
				}
				// drop through
			default:
				*r++ = *s;
				seen_backslash = 0;
		}
	}

	*r = '\0';
	
	result = strdup(tmp);
	free(tmp);
	free(str);

	return result;
}

char *
CLIGetPtypeInfo(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;
	char *			text = NULL;
	int				len = 0;
	char *			result;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL)
		return NULL;

	// First determine the length of the strings
	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0') {
			continue;
		}

		if (strncmp(text, "type = ", 7) == 0) {
			text += 7;
			len += strlen(text);
			continue;
		}
		
		len += strlen(text);
	}
	
	result = (char *)malloc(len+1);
	memset(result, '\0', len+1);
	
	// Now write the string
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0') {
			continue;
		}
		
		if (strncmp(text, "type = ", 7) == 0) {
			text += 7;
			strncat(result, text, len);
			continue;
		}
		
		strncat(result, text, len);
	}
	
	return fix_full_type(result);
}

MIList *
CLIGetPtypeFieldList(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;
	char *			text = NULL;
	char *			field;
	MIList *		result = MIListNew();

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL)
		return NULL;
	
	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) 
	{
		text = oob->cstring;
		
		if (*text == '\0') 
		{
			continue;
		}
		
		if (strncmp(text, "type = ", 7) == 0) 
		{
			continue;
		}
		
		if (strncmp(text, "}", 1) == 0) 
		{
			continue;
		}
		
		field = fix_type(text);
		
		if (field[strlen(field)-1] == ';')
		{
			field[strlen(field)-1] = '\0';
			MIListAdd(result, (void *)field);
		} else
		{
			free(field);
		}
	}
	
	return result;
}

char *
CLIGetWhatisInfo(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;
	char *			text = NULL;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL)
		return NULL;

	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0') {
			continue;
		}
		while (*text == ' ') {
			text++;
		}

		if (strncmp(text, "type =", 6) == 0) {
			text += 6;
			text = fix_type(text);

			if (strlen(text) == 0) {
				/*
				 * Look at next line for type
				 */
				oob = (MIOOBRecord *)MIListGet(oobs);
				if (oob != NULL) {
					free(text);
					text = fix_type(oob->cstring);
				}
			}
			return text;
		}
	}
	return NULL;
}

CLIInfoThreadsInfo *
CLIInfoThreadsInfoNew(void)
{
	CLIInfoThreadsInfo * info;
	info = (CLIInfoThreadsInfo *)malloc(sizeof(CLIInfoThreadsInfo));
	info->current_thread_id = 1;
	info->thread_ids = NULL;
	return info;
}

void
CLIInfoThreadsInfoFree(CLIInfoThreadsInfo *info)
{
	if (info->thread_ids != NULL)
		MIListFree(info->thread_ids, free);
	free(info);
}

CLIInfoThreadsInfo *
CLIGetInfoThreadsInfo(MICommand *cmd)
{
	MIList *				oobs;
	MIOOBRecord *			oob;
	CLIInfoThreadsInfo *	info = CLIInfoThreadsInfoNew();
	char * 					text = NULL;
	char *					id = NULL;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) {
		return info;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) {
		return info;
	}

	oobs = cmd->output->oobs;
	info->thread_ids = MIListNew();
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;

		if (*text == '\0') {
			continue;
		}
		while (*text == ' ') {
			text++;
		}
		if (strncmp(text, "*", 1) == 0) {
			text += 2;//escape "* ";
			if (isdigit(*text)) {
				info->current_thread_id = strtol(text, &text, 10);
			}
			continue;
		}
		if (isdigit(*text)) {
			if (info->thread_ids == NULL)
				info->thread_ids = MIListNew();

			id = strchr(text, ' ');
			if (id != NULL) {
				*id = '\0';
				MIListAdd(info->thread_ids, (void *)strdup(text));
			}
		}
	}
	return info;
}

CLIInfoProcInfo *
CLIInfoProcInfoNew()
{
	CLIInfoProcInfo *	info;
	info = (CLIInfoProcInfo *)malloc(sizeof(CLIInfoProcInfo));
	info->pid = -1;
	info->cmdline = NULL;
	info->cwd = NULL;
	info->exe = NULL;
	return info;
}

void
CLIInfoProcInfoFree(CLIInfoProcInfo *info)
{
	if (info->cmdline != NULL) {
		free(info->cmdline);
	}
	if (info->cwd != NULL) {
		free(info->cwd);
	}
	if (info->exe != NULL) {
		free(info->exe);
	}
	free(info);
}

char *
CLIGetCurrentSourceLine(MICommand *cmd)
{
	MIList *				oobs;
	MIOOBRecord *			oob;
	char * 					text = NULL;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) {
		return NULL;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) {
		return NULL;
	}

	oobs = cmd->output->oobs;

	//Skip the first record, which is "list\n"
	MIListSet(oobs);
	MIListGet(oobs);
	oob = (MIOOBRecord *)MIListGet(oobs);
	if (oob != NULL)
	{
		text = oob->cstring;

		if (text != NULL)
		{
			return strdup(text);
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

char *
CLIGetPrintInfo(MICommand *cmd)
{
	MIList *				oobs;
	MIOOBRecord *			oob;
	char * 					text = NULL;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) {
		return NULL;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) {
		return NULL;
	}

	oobs = cmd->output->oobs;

	//Skip the first record, which is "list\n"
	MIListSet(oobs);
	MIListGet(oobs);
	oob = (MIOOBRecord *)MIListGet(oobs);
	if (oob != NULL)
	{
		text = oob->cstring;

		if (text != NULL)
		{
			return strdup(text);
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}


#ifdef __APPLE__
CLIInfoProcInfo *
CLIGetInfoProcInfo(MICommand *cmd)
{
	char *				str = "";
	MIResult *			result;
	MIValue *			value;
	MIResultRecord *	rr;
	CLIInfoProcInfo *	info = CLIInfoProcInfoNew();

	if (!cmd->completed || cmd->output == NULL || cmd->output->rr == NULL)
		return info;

	rr = cmd->output->rr;
	for (MIListSet(rr->results); (result = (MIResult *)MIListGet(rr->results)) != NULL; ) {
		value = result->value;
		if (value->type == MIValueTypeConst) {
			str = value->cstring;
		}
		if (strcmp(result->variable, "process-id") == 0) {
			info->pid = (int)strtol(str, NULL, 10);
		}
	}

	return info;
}
#else /* __APPLE__ */
CLIInfoProcInfo *
CLIGetInfoProcInfo(MICommand *cmd)
{
	int					len;
	MIList *			oobs;
	MIOOBRecord *		oob;
	char *				text = NULL;
	CLIInfoProcInfo *	info = CLIInfoProcInfoNew();

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL)
		return info;

	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) {
		text = oob->cstring;
		if (*text == '\0') {
			continue;
		}
		while (*text == ' ') {
			text++;
		}

		if (strncmp(text, "process", 7) == 0) {
			text += 8; /* bypass " " */
			if (text != NULL) {
				info->pid = (int)strtol(text, NULL, 10);
			}
		} else if (strncmp(text, "cmdline", 7) == 0) {
			text += 11; /* bypass " = '" */
			len = strlen(text) - 1; /* exclude "'" */
			info->cmdline = (char *)malloc(len + 1);
			memcpy(info->cmdline, text, len);
			info->cmdline[len] = '\0';
		} else if (strncmp(text, "cwd", 3) == 0) {
			text += 7; /* bypass " = '" */
			len = strlen(text) - 1; /* exclude "'" */
			info->cwd = (char *)malloc(len + 1);
			memcpy(info->cwd, text, len);
			info->cwd[len] = '\0';
		} else if (strncmp(text, "exe", 3) == 0) {
			text += 7; /* bypass " = '" */
			len = strlen(text) - 1; /* exclude "'" */
			info->exe = (char *)malloc(len + 1);
			memcpy(info->exe, text, len);
			info->exe[len] = '\0';
		}
	}
	return info;
}
#endif /* __APPLE__ */

char *
CLIGetHexValueLineFromPrintInfo(MICommand *cmd)
{
	MIList *				oobs;
	MIOOBRecord *			oob;
	char * 					text = NULL;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) {
		return NULL;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) {
		return NULL;
	}

	oobs = cmd->output->oobs;

	//Skip the first record, which is "list\n"
	MIListSet(oobs);
	MIListGet(oobs);
	oob = (MIOOBRecord *)MIListGet(oobs);
	while (oob != NULL)
	{
		text = oob->cstring;

		if (text != NULL && strstr(text, "0x"))
		{
			return strdup(text);
		}
		oob = (MIOOBRecord *)MIListGet(oobs);
	}
	return NULL;
}

CLILineInfo *
CLILineInfoNew(void)
{
	CLILineInfo * line;
	
	line = (CLILineInfo *)malloc(sizeof(CLILineInfo));
	line->line_no = 0;
	line->line = NULL;
	
	return line;
}

void
CLILineInfoFree(CLILineInfo *line)
{
	if (line->line != NULL) {
		free(line->line);
	}
	
	free(line);
}

MIList *
CLIGetListInfo(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;

	char *			text;
	int				lineno;
	char *			endptr;

	MIList *		lines = MIListNew();
	CLILineInfo *	line;

	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) 
	{
		return NULL;
	}

	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) 
	{
		return NULL;
	}

	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) 
	{
		text = oob->cstring;
		
		if (*text == '\0') 
		{
			continue;
		}
		
		while (*text == ' ')
		{
			text++;
		}

		// ignore lines that are not prefixed with a line number
		if (!isdigit(*text)) 
		{
			continue;
		}

		endptr = NULL;
		lineno = (int)strtol(text, &endptr, 10);

		// bypass the first "\t" (added by GDB)
		endptr += 2;

		line = CLILineInfoNew();
		line->line_no = lineno;
		line->line = strdup(endptr);
		
		MIListAdd(lines, (void *)line);
	}

	return lines;
}

MIList *
CLIGetVariablesFromInfoScopeInfo(MICommand *cmd)
{
	MIList *		oobs;
	MIOOBRecord *	oob;
	char *			text;
	
	char *			sym = NULL;
	MIList *		syms = MIListNew();
	
	if (!cmd->completed || cmd->output == NULL || cmd->output->oobs == NULL) 
	{
		return NULL;
	}
	
	if (cmd->output->rr != NULL && cmd->output->rr->resultClass == MIResultRecordERROR) 
	{
		return NULL;
	}
	
	oobs = cmd->output->oobs;
	for (MIListSet(oobs); (oob = (MIOOBRecord *)MIListGet(oobs)) != NULL; ) 
	{
		text = oob->cstring;
		
		if (*text == '\0') 
		{
			continue;
		}
		
		while (*text == ' ')
		{
			text++;
		}
		
		// ensure that this is a variable
		if (strstr(text, "is a variable") == NULL)
		{
			continue;
		}
		
		if (sscanf(text, "Symbol %ms is a variable", &sym) != 1)
		{
			continue;
		}
		
		MIListAdd(syms, (void *)sym);
	}
	
	return syms;
}

