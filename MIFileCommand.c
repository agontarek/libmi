/******************************************************************************\
 * MIFileCommand.c - Extended libmi support for -file-* commands.
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

#include "MICommand.h" 

MICommand *
MIFileExecAndSymbols(char *file)
{
	MICommand *	cmd = MICommandNew("-file-exec-and-symbols", MIResultRecordDONE);
	
	MICommandAddOption(cmd, file, NULL);
	
	return cmd;
}

MICommand *
MIFileExecFile(char *file)
{
	MICommand *	cmd = MICommandNew("-file-exec-file", MIResultRecordDONE);
	
	MICommandAddOption(cmd, file, NULL);
	
	return cmd;
}

MICommand *
MIFileListExecSourceFile(void)
{
	MICommand *	cmd = MICommandNew("-file-list-exec-source-file", MIResultRecordDONE);
	
	return cmd;
}

MICommand *
MIFileListExecSourceFiles(void)
{
	MICommand *	cmd = MICommandNew("-file-list-exec-source-files", MIResultRecordDONE);
	
	return cmd;
}

MICommand *
MIFileSymbolFile(char *file)
{
	MICommand *	cmd = MICommandNew("-file-symbol-file", MIResultRecordDONE);
	
	MICommandAddOption(cmd, file, NULL);
	
	return cmd;
}

