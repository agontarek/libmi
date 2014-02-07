/******************************************************************************\
 * MIFile.c - Extended libmi support for -file-* commands.
 *
 * Copyright 2013-2014 Cray Inc. All Rights Reserved.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#include	<config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MIResult.h"
#include "MIString.h"
#include "MIList.h"
#include "MIValue.h"
#include "MIFile.h"

MISource *
MISourceNew(void)
{
	MISource *	source;

	source = (MISource *)malloc(sizeof(MISource));
	source->line = 0;
	source->file = NULL;
	source->fullname = NULL;
	source->macro_info = 0;
	
	return source;
}

void
MISourceFree(MISource *source)
{
	if (source->file != NULL)
		free(source->file);
	if (source->fullname != NULL)
		free(source->fullname);
	free(source);
}

MISources *
MISourcesNew(void)
{
	MISources *	sources;

	sources = (MISources *)malloc(sizeof(MISources));
	sources->file = NULL;
	sources->fullname = NULL;
	
	return sources;
}

void
MISourcesFree(MISources *sources)
{
	if (sources->file != NULL)
		free(sources->file);
	if (sources->fullname != NULL)
		free(sources->fullname);
	free(sources);
}

MISource *
MIGetFileListExecSourceFileInfo(MICommand *cmd)
{
	MIResultRecord *	rr;
	MIResult *			result;
	MISource *			source = MISourceNew();

	if (!cmd->completed || cmd->output == NULL || cmd->output->rr == NULL)
		return NULL;

	rr = cmd->output->rr;
	
	for (MIListSet(rr->results); (result = (MIResult *)MIListGet(rr->results)) != NULL; ) 
	{
		if (result->value == NULL || result->value->type != MIValueTypeConst)
			continue;
		
		if (strcmp(result->variable, "line") == 0)
		{
			source->line = (int)strtol(result->value->cstring, NULL, 10);
		} else if (strcmp(result->variable, "file") == 0)
		{
			source->file = strdup(result->value->cstring);
		} else if (strcmp(result->variable, "fullname") == 0)
		{
			source->fullname = strdup(result->value->cstring);
		} else if (strcmp(result->variable, "macro-info") == 0)
		{
			source->macro_info = (int)strtol(result->value->cstring, NULL, 10);
		}
	}
	
	return source;
}

static MISources *
MISourcesParse(MIValue *tuple)
{
	MIList *	results = tuple->results;
	MIResult *	result;
	MISources *	sources = MISourcesNew();
	
	for (MIListSet(results); (result = (MIResult *)MIListGet(results)) != NULL; )
	{
		if (result->value == NULL || result->value->type != MIValueTypeConst)
			continue;
			
		if (strcmp(result->variable, "file") == 0)
		{
			sources->file = strdup(result->value->cstring);
		} else if (strcmp(result->variable, "fullname") == 0)
		{
			sources->fullname = strdup(result->value->cstring);
		}
	}
	
	return sources;
}

MIList *
MIGetFileListExecSourceFilesInfo(MICommand *cmd)
{
	MIResultRecord *	rr;
	MIResult *			result;
	MIValue *			res_val;
	MIValue *			val;
	MIList *			sources = MIListNew();

	if (!cmd->completed || cmd->output == NULL || cmd->output->rr == NULL)
		return NULL;
		
	rr = cmd->output->rr;
	
	for (MIListSet(rr->results); (result = (MIResult *)MIListGet(rr->results)) != NULL; ) 
	{
		if (strcmp(result->variable, "files") == 0)
		{
			val = result->value;
			
			// only expect to have a list of tuple values here
			if (val->type == MIValueTypeList)
			{
				for (MIListSet(val->values); (res_val = (MIValue *)MIListGet(val->values)) != NULL; )
				{	
					MIListAdd(sources, MISourcesParse(res_val));
				}
			}
		}
	}
	
	return sources;
}

