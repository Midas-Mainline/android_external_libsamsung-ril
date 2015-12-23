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

#ifndef _SAMSUNG_RIL_SOCKET_H_
#define _SAMSUNG_RIL_SOCKET_H_

#include <stdlib.h>

/*
 * Groups
 */

#define SRS_GROUP_CONTROL					0x01
#define SRS_GROUP_SND						0x02
#define SRS_GROUP_TEST						0x03

/*
 * Commands
 */

#define SRS_CONTROL_PING					0x0101

#define SRS_SND_SET_CALL_VOLUME					0x0201
#define SRS_SND_SET_CALL_AUDIO_PATH				0x0202
#define SRS_SND_SET_CALL_CLOCK_SYNC				0x0203
#define SRS_SND_SET_MIC_MUTE				        0x0204

#define SRS_TEST_SET_RADIO_STATE				0x0301

/*
 * Values
 */

#define SRS_SOCKET_NAME				"samsung-ril-socket"
#define SRS_BUFFER_LENGTH					0x1000

#define SRS_CONTROL_CAFFE					0xCAFFE

enum srs_snd_type {
	SRS_SND_TYPE_VOICE,
	SRS_SND_TYPE_SPEAKER,
	SRS_SND_TYPE_HEADSET,
	SRS_SND_TYPE_BTVOICE
};

enum srs_snd_path {
	SRS_SND_PATH_HANDSET,
	SRS_SND_PATH_HEADSET,
	SRS_SND_PATH_SPEAKER,
	SRS_SND_PATH_BLUETOOTH,
	SRS_SND_PATH_BLUETOOTH_NO_NR,
	SRS_SND_PATH_HEADPHONE
};

enum srs_snd_clock {
	SND_CLOCK_STOP,
	SND_CLOCK_START
};

/*
 * Macros
 */

#define SRS_COMMAND(group, index)		((group << 8) | index)
#define SRS_GROUP(command)			(command >> 8)
#define SRS_INDEX(command)			(command & 0xff)

/*
 * Structures
 */

struct srs_message {
	unsigned short command;
	void *data;
	size_t size;
};

struct srs_header {
	unsigned int length;
	unsigned char group;
	unsigned char index;
} __attribute__((__packed__));

struct srs_control_ping_data {
	unsigned int caffe;
} __attribute__((__packed__));

struct srs_snd_call_volume_data {
	enum srs_snd_type type;
	int volume;
} __attribute__((__packed__));

struct srs_snd_call_audio_path_data {
	enum srs_snd_path path;
} __attribute__((__packed__));

struct srs_snd_call_clock_sync_data {
	unsigned char sync;
} __attribute__((__packed__));

struct srs_snd_mic_mute_data {
	int mute;
} __attribute__((__packed__));

struct srs_test_set_radio_state_data {
	int state;
} __attribute__((__packed__));

#endif
