/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2019 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
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

#ifndef _RIL_VERSIONS_H_
#define _RIL_VERSIONS_H_

#include <telephony/ril.h>

#if RIL_VERSION >= 6
#define RIL_Data_Call_Response_compat RIL_Data_Call_Response_v6
#else
#define RIL_Data_Call_Response_compat RIL_Data_Call_Response
#endif

#if RIL_VERSION >= 6
#define RIL_CardStatus_compat RIL_CardStatus_v6
#else
#define RIL_CardStatus_compat RIL_CardStatus_v6
#endif

#if RIL_VERSION >= 6
#define RIL_SignalStrength_compat RIL_SignalStrength_v6
#else
#define RIL_SignalStrength_compat RIL_SignalStrength
#endif

#if RIL_VERSION >= 6
#define RIL_SIM_IO_compat RIL_SIM_IO_v6
#else
#define RIL_SIM_IO_compat RIL_SIM_IO
#endif

#endif /* _RIL_VERSIONS_H_ */
