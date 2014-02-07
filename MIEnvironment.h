/******************************************************************************\
 * MIEnvironment.h - Extended libmi support for -environment-* commands.
 *
 * Copyright 2012-2014 Cray Inc. All Rights Reserved.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 ******************************************************************************/
 
#ifndef _MIENVIRONMENT_H_
#define _MIENVIRONMENT_H_
 
#include "MICommand.h"
#include "MIString.h"

extern MIString *MIGetEnvironmentDirectoryInfo(MICommand *cmd);

#endif /* _MIENVIRONMENT_H_ */
