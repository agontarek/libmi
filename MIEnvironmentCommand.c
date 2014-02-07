/******************************************************************************\
 * MIEnvironmentCommand.c - Extended libmi support for -environment-* commands.
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

#include "MICommand.h" 

MICommand *
MIEnvironmentCd(char *pathdir)
{
	MICommand *	cmd = MICommandNew("-environment-cd", MIResultRecordDONE);
	
	MICommandAddOption(cmd, pathdir, NULL);
	
	return cmd;
}

MICommand *
MIEnvironmentDirectory(int reset, char **pathdirs)
{
	int i;
	MICommand *	cmd = MICommandNew("-environment-directory", MIResultRecordDONE);
	
	if (reset) {
		MICommandAddOption(cmd, "-r", NULL);
	}
	
	for ( i=0; pathdirs[i] != NULL; i++) {
		MICommandAddOption(cmd, pathdirs[i], NULL);
	}
	
	return cmd;
}

MICommand *
MIEnvironmentPath(int reset, char **pathdirs)
{
	int i;
	MICommand *	cmd = MICommandNew("-environment-path", MIResultRecordDONE);
	
	if (reset) {
		MICommandAddOption(cmd, "-r", NULL);
	}
	
	for ( i=0; pathdirs[i] != NULL; i++) {
		MICommandAddOption(cmd, pathdirs[i], NULL);
	}
	
	return cmd;
}

MICommand *
MIEnvironmentPwd(void)
{
	MICommand *	cmd = MICommandNew("-environment-pwd", MIResultRecordDONE);
	
	return cmd;
}

