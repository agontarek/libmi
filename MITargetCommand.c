/******************************************************************************\
 * MITargetCommand.c - Extended libmi support for -target-* commands.
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
MITargetAttach(int pgid)
{
	MICommand *	cmd = MICommandNew("-target-attach",MIResultRecordDONE);
	
	MICommandAddOption(cmd, MIIntToCString(pgid), NULL);
	
	return cmd;
}

MICommand *
MITargetDetach(int pgid)
{
	MICommand *	cmd = MICommandNew("-target-detach",MIResultRecordDONE);
	
	MICommandAddOption(cmd, MIIntToCString(pgid), NULL);
	
	return cmd;
}

MICommand *
MITargetDetachAll(void)
{
	MICommand *	cmd = MICommandNew("-target-detach",MIResultRecordDONE);
	
	return cmd;
}

