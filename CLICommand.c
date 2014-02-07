/******************************************************************************
 * Copyright (c) 2005 The Regents of the University of California.
 * This material was produced under U.S. Government contract W-7405-ENG-36
 * for Los Alamos National Laboratory, which is operated by the University
 * of California for the U.S. Department of Energy. The U.S. Government has
 * rights to use, reproduce, and distribute this software. NEITHER THE
 * GOVERNMENT NOR THE UNIVERSITY MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified
 * to produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from LANL.
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
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#include	<config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MICommand.h"

MICommand *
CLIHandle(char *arg) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "handle";
	cmd_argv[1] = arg;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIInfoThreads(void) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "info";
	cmd_argv[1] = "threads";
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIPtype(char *name) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "ptype";
	cmd_argv[1] = name;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIWhatis(char *var) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "whatis";
	cmd_argv[1] = var;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIListSignals(char *name) {
	MICommand * cmd;
	char *		cmd_argv[3];
	
	cmd_argv[0] = "info";
	cmd_argv[1] = "signals";
	cmd_argv[2] = name;
	
	if (name != NULL)
		cmd = CLIBypass(3, cmd_argv);
	else
		cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLISignalInfo(char *arg) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "signal";
	cmd_argv[1] = arg;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIFrame(void) {
	MICommand * cmd;
	char *		cmd_argv[1];
	
	cmd_argv[0] = "frame";
	
	cmd = CLIBypass(1, cmd_argv);

	return cmd;
}

MICommand *
CLIList(char *arg) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "list";
	cmd_argv[1] = arg;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIListSingleLine(int lineno) {
	MICommand * cmd;
	char *		cmd_argv[2];
	char 		arg[BUFSIZ];
	
	snprintf(arg, BUFSIZ, "%d,%d", lineno, lineno);
	
	cmd_argv[0] = "list";
	cmd_argv[1] = arg;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIPrint(char *arg) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "print";
	cmd_argv[1] = arg;
	
	cmd = CLIBypass(2, cmd_argv);

	return cmd;
}

MICommand *
CLIPrintHex(char *arg) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "print/a";
	cmd_argv[1] = arg;
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIInfoProc(void) {
	MICommand * cmd;
	char *		cmd_argv[2];
	
	cmd_argv[0] = "info";
	cmd_argv[1] = "proc";
	
	cmd = CLIBypass(2, cmd_argv);
	
	return cmd;
}

MICommand *
CLIInfoScope(char *location) {
	MICommand * cmd;
	char *		cmd_argv[3];
	
	cmd_argv[0] = "info";
	cmd_argv[1] = "scope";
	cmd_argv[2] = location;
	
	cmd = CLIBypass(3, cmd_argv);
	
	return cmd;
}

// This implements the "approved" method of passing CLI commands directly to
// the gdb interpreter. Support for passing commands directly can go away at
// any time.
MICommand *
CLIBypass(int cmd_argc, char *cmd_argv[]) {
	int			i;
	int			cmd_len = 3;
	char *		cmd_str;
	char *		tmp_str;
	MICommand * mi_cmd;
	
	if (cmd_argc == 0 || (cmd_argv == NULL ? 1:(*cmd_argv == NULL))) {
		// there were no arguments...
		return NULL;
	}
	
	mi_cmd = MICommandNew("-interpreter-exec", MIResultRecordDONE);
	MICommandAddOption(mi_cmd, "console", NULL);
	
	// first figure out the string length
	for ( i=0; i < cmd_argc; i++) {
		cmd_len += strlen(cmd_argv[i])+1;
	}
	
	// allocate the string
	if ((cmd_str = malloc(sizeof(char)*cmd_len)) == (void *)0) {
		MICommandFree(mi_cmd);
		return NULL;
	}
	
	// write the first arg
	snprintf(cmd_str, cmd_len, "\"%s", cmd_argv[0]);
	
	// now write the rest of the string
	for ( i=1; i < cmd_argc; i++) {
		tmp_str = strdup(cmd_str);
		snprintf(cmd_str, cmd_len, "%s %s", tmp_str, cmd_argv[i]);
		free(tmp_str);
	}
	
	// write the final "
	tmp_str = strdup(cmd_str);
	snprintf(cmd_str, cmd_len, "%s\"", tmp_str);
	free(tmp_str);
	
	MICommandAddOption(mi_cmd, cmd_str, NULL);
	
	free(cmd_str);	
	
	return mi_cmd;
}

