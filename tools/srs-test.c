/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2014 Paul Kocialkowski <contact@paulk.fr>
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
#include <stdio.h>
#include <string.h>

#include <telephony/ril.h>

#include <samsung-ril-socket.h>
#include <srs-client.h>

struct radio_state_string {
	char *string;
	RIL_RadioState state;
};

void display_help(void)
{
	printf("Usage: srs-test [COMMAND]\n");
	printf("\n");
	printf("Commands:\n");
	printf("  radio-state [STATE] - set radio state\n");
	printf("\n");
	printf("States: off, unavailable, sim-not-ready, sim-locked-absent, sim-ready, on\n");
}

int radio_state(struct srs_client *client, char *string)
{
	struct radio_state_string radio_state_strings[] = {
		{ "off",		RADIO_STATE_OFF },
		{ "unavailable",	RADIO_STATE_UNAVAILABLE },
		{ "sim-not-ready",	RADIO_STATE_SIM_NOT_READY },
		{ "sim-locked-absent",	RADIO_STATE_SIM_LOCKED_OR_ABSENT },
		{ "sim-ready",		RADIO_STATE_SIM_READY },
		{ "on",			RADIO_STATE_ON },
	};
	struct srs_test_set_radio_state_data data;
	RIL_RadioState state = 0;
	unsigned int count;
	unsigned int i;
	int rc;

	if (client == NULL || string == NULL)
		return -1;

	count = sizeof(radio_state_strings) / sizeof(struct radio_state_string);

	for (i = 0; i < count; i++) {
		if (radio_state_strings[i].string == NULL)
			break;

		if (strcmp(radio_state_strings[i].string, string) == 0) {
			state = radio_state_strings[i].state;
			break;
		}
	}

	if (i >= count)
		return -1;

	printf("%s: Setting radio state to %s\n", __func__, string);

	memset(&data, 0, sizeof(data));
	data.state = (int) state;

	rc = srs_client_send(client, SRS_TEST_SET_RADIO_STATE, &data, sizeof(data));
	if (rc < 0) {
		printf("%s: Settings radio state failed\n", __func__);
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int (*callback)(struct srs_client *client, char *string) = NULL;
	struct srs_client *client = NULL;
	char *string = NULL;
	int rc;

	if (argc < 2) {
		display_help();
		goto error;
	}

	if (strcmp(argv[1], "radio-state") == 0) {
		if (argc < 3) {
			display_help();
			goto error;
		}

		callback = radio_state;
		string = argv[2];
	} else {
		display_help();
		goto error;
	}

	if (callback == NULL)
		goto error;

	client = srs_client_create();
	if (client == NULL) {
		printf("Creating SRS client failed\n");
		goto error;
	}

	rc = srs_client_open(client);
	if (rc < 0) {
		printf("Opening SRS client failed\n");
		goto error;
	}

	rc = callback(client, string);
	if (rc < 0)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = 1;

complete:
	if (client != NULL) {
		srs_client_close(client);
		srs_client_destroy(client);
	}

	return rc;
}
