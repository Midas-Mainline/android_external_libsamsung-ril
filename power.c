/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdlib.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>

#include <samsung-ril.h>
#include <utils.h>

int ipc_pwr_phone_pwr_up(struct ipc_message *message)
{
	ril_radio_state_update(RADIO_STATE_OFF);

	return 0;
}

int ipc_pwr_phone_reset(struct ipc_message *message)
{
	ril_radio_state_update(RADIO_STATE_OFF);

	return 0;
}

int ipc_pwr_phone_state(struct ipc_message *message)
{
	struct ipc_pwr_phone_state_response_data *data;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_pwr_phone_state_response_data))
		return -1;

	if (!ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_pwr_phone_state_response_data *) message->data;

	switch (data->state) {
		case IPC_PWR_PHONE_STATE_RESPONSE_LPM:
			RIL_LOGD("Power state is low power mode");

			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);
			ril_radio_state_update(RADIO_STATE_OFF);
			break;
		case IPC_PWR_PHONE_STATE_RESPONSE_NORMAL:
			RIL_LOGD("Power state is normal");

			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);
			ril_radio_state_update(RADIO_STATE_SIM_NOT_READY);
			break;
	}

	return 0;
}

int ril_request_radio_power(void *data, size_t size, RIL_Token token)
{
	struct ipc_pwr_phone_state_request_data request_data;
	struct ril_request *request;
	int power_state;
	int rc;

	if (data == NULL || size < sizeof(power_state))
		goto error;

	request = ril_request_find_request_status(RIL_REQUEST_RADIO_POWER, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	power_state = *((int *)data);

	memset(&request_data, 0, sizeof(request_data));

	if (power_state > 0) {
		RIL_LOGD("Requesting normal power state");
		request_data.state = IPC_PWR_PHONE_STATE_REQUEST_NORMAL;
	 } else {
		RIL_LOGD("Requesting low power mode power state");
		request_data.state = IPC_PWR_PHONE_STATE_REQUEST_LPM;
	}

	rc = ipc_gen_phone_res_expect_abort(ipc_fmt_request_seq(token), IPC_PWR_PHONE_STATE);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_PWR_PHONE_STATE, IPC_TYPE_EXEC, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}
