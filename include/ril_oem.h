/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 *
 * Samsung-RIL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Samsung-RIL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Samsung-RIL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _RIL_OEM_H_
#define _RIL_OEM_H_

#include <stdlib.h>

#define RIL_OEM_HOOK_TAG_SVC					1

#define RIL_OEM_COMMAND_SVC_ENTER_MODE				1
#define RIL_OEM_COMMAND_SVC_END_MODE				2
#define RIL_OEM_COMMAND_SVC_KEY					3

typedef struct {
	unsigned char tag;
	unsigned char command;
	unsigned short length;
} RIL_OEMHookHeader;

typedef struct {
	unsigned char mode;
	unsigned char type;
	unsigned char query;
} RIL_OEMHookSvcEnterMode;

typedef struct {
	unsigned char mode;
} RIL_OEMHookSvcEndMode;

typedef struct {
	unsigned char key;
	unsigned char query;
} RIL_OEMHookSvcKey;

#endif
