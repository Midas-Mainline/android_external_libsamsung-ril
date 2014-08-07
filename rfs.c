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

#define LOG_TAG "RIL-RFS"
#include <utils/Log.h>

#include <samsung-ril.h>
#include <utils.h>

int ipc_rfs_nv_read_item(struct ipc_message *message)
{
	struct ipc_rfs_nv_read_item_response_header response_header;
	struct ipc_rfs_nv_read_item_request_data *data;
	struct ril_client *client;
	struct ipc_rfs_data *ipc_rfs_data;
	void *response_data = NULL;
	size_t response_size = 0;
	void *nv_data = NULL;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_rfs_nv_read_item_request_data))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_RFS);
	if (client == NULL || client->data == NULL)
		return 0;

	ipc_rfs_data = (struct ipc_rfs_data *) client->data;
	if (ipc_rfs_data->ipc_client == NULL)
		return 0;

	data = (struct ipc_rfs_nv_read_item_request_data *) message->data;

	memset(&response_header, 0, sizeof(response_header));

	nv_data = ipc_nv_data_read(ipc_rfs_data->ipc_client, data->length, data->offset);
	if (nv_data == NULL) {
		RIL_LOGE("Reading %d nv_data bytes at offset 0x%x failed", data->length, data->offset);

		response_header.confirm = 0;

		rc = ipc_rfs_send(message->aseq, IPC_RFS_NV_READ_ITEM, (void *) &response_header, sizeof(response_header));
		if (rc < 0)
			goto complete;

		goto complete;
	}

	RIL_LOGD("Read %d nv_data bytes at offset 0x%x", data->length, data->offset);

	response_header.confirm = 1;
	response_header.offset = data->offset;
	response_header.length = data->length;

	response_size = ipc_rfs_nv_data_item_size_setup(&response_header, nv_data, data->length);
	if (response_size == 0)
		goto complete;

	response_data = ipc_rfs_nv_read_item_setup(&response_header, nv_data, data->length);
	if (response_data == NULL)
		goto complete;

	rc = ipc_rfs_send(message->aseq, IPC_RFS_NV_READ_ITEM, response_data, response_size);
	if (rc < 0)
		goto complete;

	goto complete;

complete:
	if (response_data != NULL && response_size > 0)
		free(response_data);

	if (nv_data != NULL)
		free(nv_data);

	return 0;
}

int ipc_rfs_nv_write_item(struct ipc_message *message)
{
	struct ipc_rfs_nv_write_item_request_header *header;
	struct ipc_rfs_nv_write_item_response_data data;
	struct ril_client *client;
	struct ipc_rfs_data *ipc_rfs_data;
	void *nv_data;
	size_t nv_size;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_rfs_nv_write_item_request_header))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_RFS);
	if (client == NULL || client->data == NULL)
		return 0;

	ipc_rfs_data = (struct ipc_rfs_data *) client->data;
	if (ipc_rfs_data->ipc_client == NULL)
		return 0;

	header = (struct ipc_rfs_nv_write_item_request_header *) message->data;

	nv_size = ipc_rfs_nv_write_item_size_extract(message->data, message->size);
	if (nv_size == 0)
		return 0;

	nv_data = ipc_rfs_nv_write_item_extract(message->data, message->size);
	if (nv_data == NULL)
		return 0;

	memset(&data, 0, sizeof(data));

	rc = ipc_nv_data_write(ipc_rfs_data->ipc_client, nv_data, header->length, header->offset);
	if (rc < 0) {
		RIL_LOGD("Writing %d nv_data byte(s) at offset 0x%x failed", header->length, header->offset);

		data.confirm = 0;
	} else {
		RIL_LOGD("Wrote %d nv_data byte(s) at offset 0x%x", header->length, header->offset);

		data.confirm = 1;
		data.offset = header->offset;
		data.length = header->length;
	}

	rc = ipc_rfs_send(message->aseq, IPC_RFS_NV_WRITE_ITEM, (void *) &data, sizeof(data));
	if (rc < 0)
		return 0;

	return 0;
}
