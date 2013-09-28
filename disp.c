/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
 *
 * Based on CyanogenMod Smdk4210RIL implementation
 * Copyright (C) 2011 The CyanogenMod Project
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

#define LOG_TAG "RIL-DISP"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

#if RIL_VERSION >= 6
void ipc2ril_rssi(unsigned char rssi, RIL_SignalStrength_v6 *ss)
#else
void ipc2ril_rssi(unsigned char rssi, RIL_SignalStrength *ss)
#endif
{
	int asu = 0;

	if (ss == NULL)
		return;

	if (rssi > 0x6f) {
		asu = 0;
	} else {
		asu = (((rssi - 0x71) * -1) - ((rssi - 0x71) * -1) % 2) / 2;
		if (asu > 31)
			asu = 31;
	}

	LOGD("Signal Strength is %d\n", asu);

#if RIL_VERSION >= 6
	memset(ss, 0, sizeof(RIL_SignalStrength_v6));
	memset(&ss->LTE_SignalStrength, -1, sizeof(ss->LTE_SignalStrength));
#else
	memset(ss, 0, sizeof(RIL_SignalStrength));
#endif

	ss->GW_SignalStrength.signalStrength = asu;
	ss->GW_SignalStrength.bitErrorRate = 99;
}

#if RIL_VERSION >= 6
void ipc2ril_bars(unsigned char bars, RIL_SignalStrength_v6 *ss)
#else
void ipc2ril_bars(unsigned char bars, RIL_SignalStrength *ss)
#endif
{
	int asu = 0;

	if (ss == NULL)
		return;

	switch (bars) {
		case 0	: asu = 1;	break;
		case 1	: asu = 3;	break;
		case 2	: asu = 5;	break;
		case 3	: asu = 8;	break;
		case 4	: asu = 12;	break;
		case 5	: asu = 15;	break;
		default	: asu = bars;	break;
	}

	LOGD("Signal Strength is %d\n", asu);

#if RIL_VERSION >= 6
	memset(ss, 0, sizeof(RIL_SignalStrength_v6));
	memset(&ss->LTE_SignalStrength, -1, sizeof(ss->LTE_SignalStrength));
#else
	memset(ss, 0, sizeof(RIL_SignalStrength));
#endif

	ss->GW_SignalStrength.signalStrength = asu;
	ss->GW_SignalStrength.bitErrorRate = 99;
}

void ril_request_signal_strength(RIL_Token t)
{
	unsigned char request = 1;

	if (ril_radio_state_complete(RADIO_STATE_OFF, t))
		return;

	ipc_fmt_send(IPC_DISP_ICON_INFO, IPC_TYPE_GET, &request, sizeof(request), ril_request_get_id(t));
}

void ipc_disp_icon_info(struct ipc_message_info *info)
{
	struct ipc_disp_icon_info *icon_info;
#if RIL_VERSION >= 6
	RIL_SignalStrength_v6 ss;
#else
	RIL_SignalStrength ss;
#endif

	if (info->data == NULL || info->length < sizeof(struct ipc_disp_icon_info))
		goto error;

	if (ril_radio_state_complete(RADIO_STATE_OFF, RIL_TOKEN_NULL))
		return;

	icon_info = (struct ipc_disp_icon_info *) info->data;

	if (info->type == IPC_TYPE_RESP) {
		ipc2ril_rssi(icon_info->rssi, &ss);
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, &ss, sizeof(ss));

	} else {
		ipc2ril_bars(icon_info->bars, &ss);
		ril_request_unsolicited(RIL_UNSOL_SIGNAL_STRENGTH, &ss, sizeof(ss));
	}

	return;

error:
	if (info->type == IPC_TYPE_RESP)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_disp_rssi_info(struct ipc_message_info *info)
{
	struct ipc_disp_rssi_info *rssi_info;
#if RIL_VERSION >= 6
	RIL_SignalStrength_v6 ss;
#else
	RIL_SignalStrength ss;
#endif
	int rssi;

	if (info->data == NULL || info->length < sizeof(struct ipc_disp_rssi_info))
		return;

	if (ril_radio_state_complete(RADIO_STATE_OFF, RIL_TOKEN_NULL))
		return;

	rssi_info = (struct ipc_disp_rssi_info *) info->data;

	ipc2ril_rssi(rssi_info->rssi, &ss);

	ril_request_unsolicited(RIL_UNSOL_SIGNAL_STRENGTH, &ss, sizeof(ss));
}
