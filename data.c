/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2011 Denis 'GNUtoo' Carikli <GNUtoo@no-log.org>
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
#include <arpa/inet.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>
#include <netutils/ifc.h>

#include <samsung-ril.h>
#include <utils.h>

int ipc2ril_gprs_fail_cause(unsigned char fail_cause)
{
	switch (fail_cause) {
		case IPC_GPRS_FAIL_CAUSE_NONE:
		case IPC_GPRS_FAIL_CAUSE_REL_BY_USER:
		case IPC_GPRS_FAIL_CAUSE_REGULAR_DEACTIVATION:
			return PDP_FAIL_NONE;
		case IPC_GPRS_FAIL_CAUSE_INSUFFICIENT_RESOURCE:
			return PDP_FAIL_INSUFFICIENT_RESOURCES;
		case IPC_GPRS_FAIL_CAUSE_UNKNOWN_APN:
			return PDP_FAIL_MISSING_UKNOWN_APN;
		case IPC_GPRS_FAIL_CAUSE_UNKNOWN_PDP_ADDRESS:
			return PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE;
		case IPC_GPRS_FAIL_CAUSE_USER_AUTH_FAILED:
			return PDP_FAIL_USER_AUTHENTICATION;
		case IPC_GPRS_FAIL_CAUSE_ACT_REJ_GGSN:
			return PDP_FAIL_ACTIVATION_REJECT_GGSN;
		case IPC_GPRS_FAIL_CAUSE_ACT_REJ_UNSPECIFIED:
			return PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED;
		case IPC_GPRS_FAIL_CAUSE_SVC_OPTION_NOT_SUPPORTED:
			return PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED;
		case IPC_GPRS_FAIL_CAUSE_SVC_NOT_SUBSCRIBED:
			return PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED;
		case IPC_GPRS_FAIL_CAUSE_SVC_OPT_OUT_ORDER:
			return PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER;
		case IPC_GPRS_FAIL_CAUSE_NSAPI_USED:
			return PDP_FAIL_NSAPI_IN_USE;
		case IPC_GPRS_FAIL_CAUSE_NETWORK_FAILURE:
			return PDP_FAIL_DATA_REGISTRATION_FAIL;
		case IPC_GPRS_FAIL_CAUSE_UNKOWN_PDP_CONTEXT:
		case IPC_GPRS_FAIL_CAUSE_INVALID_MSG:
		case IPC_GPRS_FAIL_CAUSE_PROTOCOL_ERROR:
			return PDP_FAIL_PROTOCOL_ERRORS;
		case IPC_GPRS_FAIL_CAUSE_MOBILE_FAILURE_ERROR:
			return PDP_FAIL_SIGNAL_LOST;
		case IPC_GPRS_FAIL_CAUSE_UNKNOWN_ERROR:
		default:
			return PDP_FAIL_ERROR_UNSPECIFIED;
	}
}

int ril_data_connection_register(unsigned int cid, char *apn, char *username,
	char *password, char *iface)
{
	struct ril_data_connection *data_connection;
	struct list_head *list_end;
	struct list_head *list;

	if (apn == NULL || ril_data == NULL)
		return -1;

	data_connection = (struct ril_data_connection *) calloc(1, sizeof(struct ril_data_connection));
	data_connection->cid = cid;
	data_connection->enabled = 0;
	data_connection->apn = apn;
	data_connection->username = username;
	data_connection->password = password;
	data_connection->iface = iface;

	list_end = ril_data->data_connections;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc(list_end, NULL, (void *) data_connection);

	if (ril_data->data_connections == NULL)
		ril_data->data_connections = list;

	return 0;
}

int ril_data_connection_unregister(struct ril_data_connection *data_connection)
{
	struct list_head *list;

	if (data_connection == NULL || ril_data == NULL)
		return -1;

	list = ril_data->data_connections;
	while (list != NULL) {
		if (list->data == (void *) data_connection) {
			memset(data_connection, 0, sizeof(struct ril_data_connection));
			free(data_connection);

			if (list == ril_data->data_connections)
				ril_data->data_connections = list->next;

			list_head_free(list);

			break;
		}

		list = list->next;
	}

	return 0;
}

int ril_data_connection_flush(void)
{
	struct ril_data_connection *data_connection;
	struct list_head *list;
	struct list_head *list_next;

	if (ril_data == NULL)
		return -1;

	list = ril_data->data_connections;
	while (list != NULL) {
		if (list->data != NULL) {
			data_connection = (struct ril_data_connection *) list->data;

			ril_data_connection_stop(data_connection);

			memset(data_connection, 0, sizeof(struct ril_data_connection));
			free(data_connection);
		}

		if (list == ril_data->data_connections)
			ril_data->data_connections = list->next;

		list_next = list->next;

		list_head_free(list);

		list = list_next;
	}

	return 0;
}

struct ril_data_connection *ril_data_connection_find_cid(unsigned int cid)
{
	struct ril_data_connection *data_connection;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	list = ril_data->data_connections;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		data_connection = (struct ril_data_connection *) list->data;

		if (data_connection->cid == cid)
			return data_connection;

list_continue:
		list = list->next;
	}

	return NULL;
}

struct ril_data_connection *ril_data_connection_find_token(RIL_Token token)
{
	struct ril_data_connection *data_connection;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	list = ril_data->data_connections;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		data_connection = (struct ril_data_connection *) list->data;

		if (data_connection->token == token)
			return data_connection;

list_continue:
		list = list->next;
	}

	return NULL;
}

struct ril_data_connection *ril_data_connection_start(char *apn, char *username,
	char *password)
{
	struct ril_data_connection *data_connection;
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	struct ipc_client_gprs_capabilities gprs_capabilities;
	char *iface = NULL;
	unsigned int cid;
	unsigned int i;
	int rc;

	if (apn == NULL)
		goto error;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		goto error;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;
	if (ipc_fmt_data->ipc_client == NULL)
		goto error;

	rc = ipc_client_gprs_get_capabilities(ipc_fmt_data->ipc_client, &gprs_capabilities);
	if (rc < 0)
		goto error;

	for (i = 0; i < gprs_capabilities.cid_count; i++) {
		data_connection = ril_data_connection_find_cid(i + 1);
		if (data_connection == NULL)
			break;
	}

	cid = i + 1;

	if (cid > gprs_capabilities.cid_count) {
		RIL_LOGE("%s: No place left for a new data connection", __func__);
		goto error;
	}

	iface = ipc_client_gprs_get_iface(ipc_fmt_data->ipc_client, cid);
	if (iface == NULL)
		goto error;

	rc = ril_data_connection_register(cid, apn, username, password, iface);
	if (rc < 0)
		goto error;

	data_connection = ril_data_connection_find_cid(cid);
	if (data_connection == NULL)
		goto error;

	RIL_LOGD("Starting new data connection with cid: %d and iface: %s", cid, iface);

	goto complete;

error:
	if (iface != NULL)
		free(iface);

	data_connection = NULL;

complete:
	return data_connection;
}

int ril_data_connection_stop(struct ril_data_connection *data_connection)
{
	int rc;

	if (data_connection == NULL)
		return -1;

	if (data_connection->apn != NULL)
		free(data_connection->apn);

	if (data_connection->username != NULL)
		free(data_connection->username);

	if (data_connection->password != NULL)
		free(data_connection->password);

	if (data_connection->iface != NULL)
		free(data_connection->iface);

	if (data_connection->ip != NULL)
		free(data_connection->ip);

	if (data_connection->gateway != NULL)
		free(data_connection->gateway);

	if (data_connection->subnet_mask != NULL)
		free(data_connection->subnet_mask);

	if (data_connection->dns1 != NULL)
		free(data_connection->dns1);

	if (data_connection->dns2 != NULL)
		free(data_connection->dns2);

	rc = ril_data_connection_unregister(data_connection);
	if (rc < 0)
		return -1;

	return 0;
}

int ril_data_connection_enable(struct ril_data_connection *data_connection)
{
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	in_addr_t ip_addr;
	in_addr_t gateway_addr;
	in_addr_t subnet_mask_addr;
	in_addr_t dns1_addr;
	in_addr_t dns2_addr;
	int rc;

	if (data_connection == NULL || data_connection->iface == NULL || data_connection->ip == NULL || data_connection->gateway == NULL || data_connection->subnet_mask == NULL)
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return -1;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;
	if (ipc_fmt_data->ipc_client == NULL)
		return -1;

	rc = ipc_client_gprs_activate(ipc_fmt_data->ipc_client, data_connection->cid);
	if (rc < 0)
		return -1;

	ip_addr = inet_addr(data_connection->ip);
	gateway_addr = inet_addr(data_connection->gateway);
	subnet_mask_addr = inet_addr(data_connection->subnet_mask);

	if (data_connection->dns1 != NULL)
		dns1_addr = inet_addr(data_connection->dns1);
	else
		dns1_addr = 0;

	if (data_connection->dns2 != NULL)
		dns2_addr = inet_addr(data_connection->dns2);
	else
		dns2_addr = 0;

	rc = ifc_configure(data_connection->iface, ip_addr, ipv4NetmaskToPrefixLength(subnet_mask_addr), gateway_addr, dns1_addr, dns2_addr);
	if (rc < 0)
		return -1;

	data_connection->enabled = 1;

	RIL_LOGD("%s: Enabled data connection with cid %d", __func__, data_connection->cid);

	return 0;
}

int ril_data_connection_disable(struct ril_data_connection *data_connection)
{
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	int rc;

	if (data_connection == NULL || data_connection->iface == NULL)
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return -1;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;
	if (ipc_fmt_data->ipc_client == NULL)
		return -1;

	rc = ifc_reset_connections(data_connection->iface, RESET_IPV4_ADDRESSES);
	if (rc < 0)
		return -1;

	rc = ifc_disable(data_connection->iface);
	if (rc < 0)
		return -1;

	rc = ipc_client_gprs_deactivate(ipc_fmt_data->ipc_client, data_connection->cid);
	if (rc < 0)
		return -1;

	data_connection->enabled = 0;

	RIL_LOGD("%s: Disabled data connection with cid %d", __func__, data_connection->cid);

	return 0;;
}

int ipc_gprs_define_pdp_context_callback(struct ipc_message *message)
{
	struct ipc_gprs_pdp_context_request_set_data request_data;
	struct ril_data_connection *data_connection;
	struct ipc_gen_phone_res_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	data_connection = ril_data_connection_find_token(ipc_fmt_request_token(message->aseq));
	if (data_connection == NULL) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		return 0;
	}

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0)
		goto error;

	rc = ipc_gprs_pdp_context_request_set_setup(&request_data, 1, data_connection->cid, data_connection->username, data_connection->password);
	if (rc < 0)
		goto error;

	rc = ipc_gen_phone_res_expect_callback(message->aseq, IPC_GPRS_PDP_CONTEXT, ipc_gprs_pdp_context_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(message->aseq, IPC_GPRS_PDP_CONTEXT, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	goto complete;

error:
	ril_data_connection_stop(data_connection);

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

int ril_request_setup_data_call(void *data, size_t size, RIL_Token token)
{
	struct ipc_gprs_define_pdp_context_data request_data;
	struct ril_data_connection *data_connection = NULL;
	struct ril_request *request;
	char *apn = NULL;
	char *username = NULL;
	char *password = NULL;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 6 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	request = ril_request_find_request_status(RIL_REQUEST_SETUP_DATA_CALL, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	if (values[2] == NULL) {
		RIL_LOGE("%s: No APN was provided", __func__);
		goto error;
	}

	apn = strdup(values[2]);

	if (values[3] != NULL)
		username = strdup(values[3]);

	if (values[4] != NULL)
		password = strdup(values[4]);

	strings_array_free(values, size);
	values = NULL;

	RIL_LOGD("Setting up data connection to APN: %s with username/password: %s/%s", apn, username, password);

	data_connection = ril_data_connection_start(apn, username, password);
	if (data_connection == NULL)
		goto error;

	data_connection->token = token;

	rc = ipc_gprs_define_pdp_context_setup(&request_data, 1, data_connection->cid, data_connection->apn);
	if (rc < 0)
		goto error;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_GPRS_DEFINE_PDP_CONTEXT, ipc_gprs_define_pdp_context_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_GPRS_DEFINE_PDP_CONTEXT, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	if (data_connection != NULL) {
		ril_data_connection_stop(data_connection);
	} else {
		if (apn != NULL)
			free(apn);

		if (username != NULL)
			free(username);

		if (password != NULL)
			free(password);
	}

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_gprs_ps(struct ipc_message *message)
{
	struct ipc_gprs_ps_data *data;
	struct ril_data_connection *data_connection;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gprs_ps_data))
		return -1;

	data = (struct ipc_gprs_ps_data *) message->data;

	data_connection = ril_data_connection_find_cid(data->cid);
	if (data_connection == NULL)
		return 0;

	data_connection->attached = !!data->attached;

	return 0;
}

int ipc_gprs_pdp_context(struct ipc_message *message)
{
	struct ipc_gprs_pdp_context_request_get_data *data;
	struct ril_data_connection *data_connection;
#if RIL_VERSION >= 6
	RIL_Data_Call_Response_v6 response[3];
#else
	RIL_Data_Call_Response response[3];
#endif
	unsigned int entries_count;
	unsigned int index = 0;
	size_t size;
	unsigned int i;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gprs_pdp_context_request_get_data))
		return -1;

	data = (struct ipc_gprs_pdp_context_request_get_data *) message->data;

	entries_count = sizeof(data->entries) / sizeof(struct ipc_gprs_pdp_context_request_get_entry);

	memset(&response, 0, sizeof(response));

	for (i = 0; i < entries_count; i++) {
		if (!data->entries[i].active)
			continue;

		data_connection = ril_data_connection_find_cid(data->entries[i].cid);
		if (data_connection == NULL)
			continue;

#if RIL_VERSION >= 6
		response[index].status = PDP_FAIL_NONE;
#endif
		response[index].cid = data->entries[i].cid;

		if (data_connection->enabled)
			response[index].active = 2;
		else
			response[index].active = 0;

		response[index].type = strdup("IP");
#if RIL_VERSION >= 6
		if (data_connection->iface != NULL)
			response[index].ifname = strdup(data_connection->iface);

		if (data_connection->ip != NULL)
			response[index].addresses = strdup(data_connection->ip);

		asprintf(&response[index].dnses, "%s %s", data_connection->dns1, data_connection->dns2);

		if (data_connection->gateway != NULL)
			response[index].gateways = strdup(data_connection->gateway);
#else
		if (data_connection->apn != NULL)
			response[index].apn = strdup(data_connection->apn);

		if (data_connection->ip != NULL)
			response[index].address = strdup(data_connection->ip);
#endif

		index++;
	}

#if RIL_VERSION >= 6
	size = index * sizeof(RIL_Data_Call_Response_v6);
#else
	size = index * sizeof(RIL_Data_Call_Response);
#endif

	if (!ipc_seq_valid(message->aseq))
		ril_request_unsolicited(RIL_UNSOL_DATA_CALL_LIST_CHANGED, &response, size);
	else
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, &response, size);

	for (i = 0; i < index; i++) {
		if (response[i].type != NULL)
			free(response[i].type);

#if RIL_VERSION >= 6
		if (response[i].ifname != NULL)
			free(response[i].ifname);

		if (response[i].addresses != NULL)
			free(response[i].addresses);

		if (response[i].dnses != NULL)
			free(response[i].dnses);

		if (response[i].gateways != NULL)
			free(response[i].gateways);
#else
		if (response[i].apn != NULL)
			free(response[i].apn);

		if (response[i].address != NULL)
			free(response[i].address);
#endif
	}

	return 0;
}

int ril_request_data_call_list(void *data, size_t size, RIL_Token token)
{
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_SUCCESS, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_GPRS_PDP_CONTEXT, IPC_TYPE_GET, NULL, 0);
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

int ipc_gprs_pdp_context_callback(struct ipc_message *message)
{
	struct ril_data_connection *data_connection;
	struct ipc_gen_phone_res_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	data_connection = ril_data_connection_find_token(ipc_fmt_request_token(message->aseq));
	if (data_connection == NULL) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		return 0;
	}

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0)
		goto error;

	goto complete;

error:
	ril_data_connection_stop(data_connection);

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

int ril_request_deactivate_data_call(void *data, size_t size, RIL_Token token)
{
	struct ipc_gprs_pdp_context_request_set_data request_data;
	struct ril_data_connection *data_connection = NULL;
	struct ril_request *request;
	char **values = NULL;
	unsigned int cid;
	int rc;

	if (data == NULL || size < 2 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DEACTIVATE_DATA_CALL, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	if (values[0] == NULL) {
		RIL_LOGE("%s: No cid was provided", __func__);
		goto error;
	}

	cid = (unsigned int) atoi(values[0]);

	strings_array_free(values, size);
	values = NULL;

	data_connection = ril_data_connection_find_cid(cid);
	if (data_connection == NULL)
		goto error;

	data_connection->token = token;

	rc = ipc_gprs_pdp_context_request_set_setup(&request_data, 0, data_connection->cid, NULL, NULL);
	if (rc < 0)
		goto error;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_GPRS_PDP_CONTEXT, ipc_gprs_pdp_context_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_GPRS_PDP_CONTEXT, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	if (data_connection != NULL)
		ril_data_connection_stop(data_connection);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_gprs_ip_configuration(struct ipc_message *message)
{
	struct ipc_gprs_ip_configuration_data *data;
	struct ril_data_connection *data_connection;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gprs_ip_configuration_data))
		return -1;

	data = (struct ipc_gprs_ip_configuration_data *) message->data;

	data_connection = ril_data_connection_find_cid(data->cid);
	if (data_connection == NULL) {
		if (ipc_seq_valid(message->aseq))
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

		return 0;
	}

	if (data_connection->ip != NULL)
		free(data_connection->ip);

	asprintf(&data_connection->ip, "%i.%i.%i.%i", data->ip[0], data->ip[1], data->ip[2], data->ip[3]);

	if (data_connection->gateway != NULL)
		free(data_connection->gateway);

	asprintf(&data_connection->gateway, "%i.%i.%i.%i", data->ip[0], data->ip[1], data->ip[2], data->ip[3]);

	if (data_connection->subnet_mask != NULL)
		free(data_connection->subnet_mask);

	asprintf(&data_connection->subnet_mask, "255.255.255.255");

	if (data_connection->dns1 != NULL)
		free(data_connection->dns1);

	asprintf(&data_connection->dns1, "%i.%i.%i.%i", data->dns1[0], data->dns1[1], data->dns1[2], data->dns1[3]);

	if (data_connection->dns2 != NULL)
		free(data_connection->dns2);

	asprintf(&data_connection->dns2, "%i.%i.%i.%i", data->dns2[0], data->dns2[1], data->dns2[2], data->dns2[3]);

	return 0;
}

int ipc_gprs_hsdpa_status(struct ipc_message *message)
{
	struct ipc_gprs_hsdpa_status_data *data;
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gprs_hsdpa_status_data))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_gprs_hsdpa_status_data *) message->data;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;

	if (ipc_fmt_data->hsdpa_status_data.status != data->status) {
		ipc_fmt_data->hsdpa_status_data.status = data->status;

#if RIL_VERSION >= 6
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif
	}

	return 0;
}

int ipc_gprs_call_status(struct ipc_message *message)
{
#if RIL_VERSION >= 6
	RIL_Data_Call_Response_v6 response;
#else
	char *setup_data_call_response[3];
	int fail_cause;
	unsigned int i;
#endif
	struct ipc_gprs_call_status_data *data;
	struct ril_data_connection *data_connection;
	struct ril_request *request;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gprs_call_status_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return 0;

	data = (struct ipc_gprs_call_status_data *) message->data;

	data_connection = ril_data_connection_find_cid(data->cid);
	if (data_connection == NULL) {
		RIL_LOGE("%s: Finding data connection with cid: %d failed", __func__, data->cid);

		if (ipc_seq_valid(message->aseq))
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

		return 0;
	}

	request = ril_request_find_token(data_connection->token);

	if (data->status == IPC_GPRS_STATUS_ENABLED) {
		if (data_connection->enabled) {
			if (request != NULL && request->request == RIL_REQUEST_DEACTIVATE_DATA_CALL)
				goto error;
			else
				RIL_LOGD("%s: Data connection with cid %d is already enabled", __func__, data_connection->cid);
		} else {
			rc = ril_data_connection_enable(data_connection);
			if (rc < 0)
				goto error;

			if (request != NULL && request->request == RIL_REQUEST_SETUP_DATA_CALL) {
				memset(&response, 0, sizeof(response));
#if RIL_VERSION >= 6
				response.status = ipc2ril_gprs_fail_cause(data->fail_cause);
				response.cid = data_connection->cid;
				response.active = 2;
				response.type = strdup("IP");

				if (data_connection->iface != NULL)
					response.ifname = strdup(data_connection->iface);

				if (data_connection->ip != NULL)
					response.addresses = strdup(data_connection->ip);

				asprintf(&response.dnses, "%s %s", data_connection->dns1, data_connection->dns2);

				if (data_connection->gateway != NULL)
					response.gateways = strdup(data_connection->gateway);
#else
				asprintf(&response[0], "%d", gprs_connection->cid);

				if (data_connection->iface != NULL)
					response[1] = strdup(data_connection->iface);

				if (data_connection->ip != NULL)
					response[2] = strdup(data_connection->ip);
#endif

				ril_request_complete(data_connection->token, RIL_E_SUCCESS, &response, sizeof(response));
				data_connection->token = NULL;

#if RIL_VERSION >= 6
				if (response.type != NULL)
					free(response.type);

				if (response.ifname != NULL)
					free(response.ifname);

				if (response.addresses != NULL)
					free(response.addresses);

				if (response.gateways != NULL)
					free(response.gateways);

				if (response.dnses != NULL)
					free(response.dnses);
#else
				for (i = 0; i < 3; i++) {
					if (reponse[i] != NULL)
						free(response[i]);
				}
#endif
			} else {
				RIL_LOGD("%s: Data connection with cid: %d got unexpectedly enabled", __func__, data_connection->cid);
			}
		}
	} else if (data->status == IPC_GPRS_STATUS_DISABLED || data->status == IPC_GPRS_STATUS_NOT_ENABLED) {
		if (data_connection->enabled) {
			rc = ril_data_connection_disable(data_connection);
			if (rc < 0)
				goto error;

			ril_data_connection_stop(data_connection);

			if (request != NULL && request->request == RIL_REQUEST_DEACTIVATE_DATA_CALL)
				ril_request_complete(request->token, RIL_E_SUCCESS, NULL, 0);
			else
				RIL_LOGD("%s: Data connection with cid: %d got unexpectedly disabled", __func__, data->cid);
		} else {
			if (request != NULL && request->request == RIL_REQUEST_SETUP_DATA_CALL) {
#if RIL_VERSION >= 6
				memset(&response, 0, sizeof(response));
				response.status = ipc2ril_gprs_fail_cause(data->fail_cause);

				ril_request_complete(request->token, RIL_E_SUCCESS, (void *) &response, sizeof(response));
				// Avoid completing the request twice
				request = NULL;
#else
				fail_cause = ipc2ril_gprs_fail_cause(data->fail_cause);

				ril_request_data_set_uniq(RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE, (void *) &fail_cause, sizeof(fail_cause));
#endif
				goto error;
			} else {
				RIL_LOGD("%s: Data connection with cid: %d is already disabled", __func__, data_connection->cid);
			}
		}
	}

	if (request == NULL) {
		rc = ipc_fmt_send(0, IPC_GPRS_PDP_CONTEXT, IPC_TYPE_GET, NULL, 0);
		if (rc < 0)
			goto error;
	}

	goto complete;

error:
	ril_data_connection_stop(data_connection);

	if (request != NULL)
		ril_request_complete(request->token, RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

int ril_request_last_data_call_fail_cause(void *data, size_t size,
	RIL_Token token)
{
	void *fail_cause_data;
	size_t fail_cause_size;
	int fail_cause;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	fail_cause_size = ril_request_data_size_get(RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE);
	fail_cause_data = ril_request_data_get(RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE);

	if (fail_cause_data != NULL && fail_cause_size >= sizeof(int)) {
		fail_cause = *((int *) fail_cause_data);
		free(fail_cause_data);
	} else {
		fail_cause = PDP_FAIL_ERROR_UNSPECIFIED;
	}

	ril_request_complete(token, RIL_E_SUCCESS, (void *) &fail_cause, sizeof(fail_cause));

	return RIL_REQUEST_COMPLETED;
}
