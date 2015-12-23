/*
 * This file is part of Samsung-RIL.
 *
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

unsigned char srs2ipc_call_type(int type)
{
	switch (type) {
		case SRS_SND_TYPE_VOICE:
			return IPC_SND_VOLUME_TYPE_VOICE;
		case SRS_SND_TYPE_SPEAKER:
			return IPC_SND_VOLUME_TYPE_SPEAKER;
		case SRS_SND_TYPE_HEADSET:
			return IPC_SND_VOLUME_TYPE_HEADSET;
		case SRS_SND_TYPE_BTVOICE:
			return IPC_SND_VOLUME_TYPE_BTVOICE;
		default:
			return IPC_SND_VOLUME_TYPE_VOICE;
	}
}

unsigned char srs2ipc_audio_path(int path)
{
	switch (path) {
		case SRS_SND_PATH_HANDSET:
			return IPC_SND_AUDIO_PATH_HANDSET;
		case SRS_SND_PATH_HEADSET:
			return IPC_SND_AUDIO_PATH_HEADSET;
		case SRS_SND_PATH_SPEAKER:
			return IPC_SND_AUDIO_PATH_SPEAKER;
		case SRS_SND_PATH_BLUETOOTH:
			return IPC_SND_AUDIO_PATH_BLUETOOTH;
		case SRS_SND_PATH_BLUETOOTH_NO_NR:
			return IPC_SND_AUDIO_PATH_BLUETOOTH_NO_NR;
		case SRS_SND_PATH_HEADPHONE:
			return IPC_SND_AUDIO_PATH_HEADPHONE;
		default:
			return IPC_SND_AUDIO_PATH_HANDSET;
	}
}

int srs_snd_set_call_volume(struct srs_message *message)
{
	struct ipc_snd_spkr_volume_ctrl_data request_data;
	struct srs_snd_call_volume_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct srs_snd_call_volume_data))
		return -1;

	data = (struct srs_snd_call_volume_data *) message->data;

	memset(&request_data, 0, sizeof(request_data));
	request_data.type = srs2ipc_call_type(data->type);
	request_data.volume = data->volume;

	rc = ipc_fmt_send(ipc_fmt_seq(), IPC_SND_SPKR_VOLUME_CTRL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		return 0;

	return 0;
}

int ril_request_set_mute(void *data, size_t size, RIL_Token token)
{
	struct ipc_snd_mic_mute_ctrl_data request_data;
	int mute;
	int rc;

	if (data == NULL || size < sizeof(int))
		goto error;

	mute = *((int *) data);

	memset(&request_data, 0, sizeof(request_data));
	request_data.mute = !!mute;

	rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_SND_MIC_MUTE_CTRL);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SND_MIC_MUTE_CTRL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
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

int srs_snd_set_mic_mute(struct srs_message *message)
{
	struct ipc_snd_mic_mute_ctrl_data request_data;
	struct srs_snd_mic_mute_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct srs_snd_mic_mute_data))
		return -1;

	data = (struct srs_snd_mic_mute_data *) message->data;

	memset(&request_data, 0, sizeof(request_data));
	request_data.mute = !!(data->mute);

	rc = ipc_fmt_send(ipc_fmt_seq(), IPC_SND_MIC_MUTE_CTRL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		return 0;

	return 0;
}

int srs_snd_set_call_audio_path(struct srs_message *message)
{
	struct ipc_snd_audio_path_ctrl_data request_data;
	struct srs_snd_call_audio_path_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct srs_snd_call_audio_path_data))
		return -1;

	data = (struct srs_snd_call_audio_path_data *) message->data;

	memset(&request_data, 0, sizeof(request_data));
	request_data.path = srs2ipc_audio_path(data->path);

	rc = ipc_fmt_send(ipc_fmt_seq(), IPC_SND_AUDIO_PATH_CTRL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		return 0;

	return 0;
}

int srs_snd_set_call_clock_sync(struct srs_message *message)
{
	struct ipc_snd_clock_ctrl_data request_data;
	struct srs_snd_call_clock_sync_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct srs_snd_call_clock_sync_data))
		return -1;

	data = (struct srs_snd_call_clock_sync_data *) message->data;

	memset(&request_data, 0, sizeof(request_data));
	request_data.sync = data->sync;

	rc = ipc_fmt_send(ipc_fmt_seq(), IPC_SND_CLOCK_CTRL, IPC_TYPE_EXEC, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		return 0;

	return 0;
}
