/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
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

#define LOG_TAG "RIL-SND"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

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
			RIL_LOGE("Unknown call type: 0x%x", type);
			return 0;
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
			RIL_LOGE("Unknown audio path: 0x%x", path);
			return 0;
	}
}

void ril_request_set_mute(RIL_Token t, void *data, int length)
{
	int *value;
	unsigned char mute;

	if (data == NULL || length < (int) sizeof(int))
		return;

	if (ril_radio_state_complete(RADIO_STATE_OFF, t))
		return;

	value = (int *) data;
	mute = *value ? 1 : 0;

	RIL_LOGD("Mute is %d\n", mute);

	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_SND_MIC_MUTE_CTRL);

	ipc_fmt_send(IPC_SND_MIC_MUTE_CTRL, IPC_TYPE_SET, (void *) &mute, sizeof(mute), ril_request_get_id(t));
}

void srs_snd_set_call_clock_sync(struct srs_message *message)
{
	unsigned char *sync;

	if (message == NULL || message->data == NULL || message->length < (int) sizeof(unsigned char))
		return;

	sync = (unsigned char *) message->data;

	RIL_LOGD("Clock sync is 0x%x\n", *sync);

	ipc_fmt_send(IPC_SND_CLOCK_CTRL, IPC_TYPE_EXEC, sync, sizeof(unsigned char), ril_request_id_get());
}

void srs_snd_set_call_volume(struct srs_message *message)
{
	struct srs_snd_call_volume *call_volume;
	struct ipc_snd_spkr_volume_ctrl volume_ctrl;

	if (message == NULL || message->data == NULL || message->length < (int) sizeof(struct srs_snd_call_volume))
		return;

	call_volume = (struct srs_snd_call_volume *) message->data;

	RIL_LOGD("Call volume for: 0x%x vol = 0x%x\n", call_volume->type, call_volume->volume);

	memset(&volume_ctrl, 0, sizeof(volume_ctrl));
	volume_ctrl.type = srs2ipc_call_type(call_volume->type);
	volume_ctrl.volume = call_volume->volume;

	ipc_fmt_send(IPC_SND_SPKR_VOLUME_CTRL, IPC_TYPE_SET, (void *) &volume_ctrl, sizeof(volume_ctrl), ril_request_id_get());
}

void srs_snd_set_call_audio_path(struct srs_message *message)
{
	int *audio_path;
	unsigned char path;

	if (message == NULL || message->data == NULL || message->length < (int) sizeof(int))
		return;

	audio_path = (int *) message->data;
	path = srs2ipc_audio_path(*audio_path);

	RIL_LOGD("Audio path to: 0x%x\n", path);

	ipc_fmt_send(IPC_SND_AUDIO_PATH_CTRL, IPC_TYPE_SET, (void *) &path, sizeof(path), ril_request_id_get());
}
