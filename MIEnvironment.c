/******************************************************************************\
 * MIEnvironment.c - Extended libmi support for -environment-* commands.
 *
 * Copyright 2012-2014 Cray Inc. All Rights Reserved.
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
#include "MIEnvironment.h"

MIString *
MIGetEnvironmentDirectoryInfo(MICommand *cmd)
{
	MIList *	results;
	MIResult *	r;
	MIString *	source_path = NULL;

	if (!cmd->completed || cmd->output == NULL || cmd->output->rr == NULL)
		return NULL;

	results = cmd->output->rr->results;
	
	for (MIListSet(results); (r = (MIResult *)MIListGet(results)) != NULL;) {
		if (strcmp(r->variable, "source-path") == 0) {
			if (r->value != NULL) {
				source_path = MIValueToString(r->value);
			}
		}
	}
	
	return source_path;
}

