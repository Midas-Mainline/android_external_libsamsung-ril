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

#ifndef _SAMSUNG_RIL_H_
#define _SAMSUNG_RIL_H_

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <telephony/ril.h>

#include <samsung-ipc.h>
#include <samsung-ril-socket.h>

#include <ipc.h>
#include <srs.h>
#include <utils.h>

/*
 * Values
 */

#define RIL_VERSION_STRING			"Samsung-RIL"

#define RIL_CLIENT_RETRY_COUNT					5
#define RIL_CLIENT_RETRY_DELAY					50000

/*
 * Macros
 */

#ifdef ALOGI
#define RIL_LOGI				ALOGI
#else
#define RIL_LOGI				LOGI
#endif

#ifdef ALOGD
#define RIL_LOGD				ALOGD
#else
#define RIL_LOGD				LOGD
#endif

#ifdef ALOGE
#define RIL_LOGE				ALOGE
#else
#define RIL_LOGE				LOGE
#endif

#define RIL_LOCK()				pthread_mutex_lock(&ril_data->mutex)
#define RIL_UNLOCK()				pthread_mutex_unlock(&ril_data->mutex)
#define RIL_REQUEST_LOCK()			pthread_mutex_lock(&ril_data->request_mutex)
#define RIL_REQUEST_UNLOCK()			pthread_mutex_unlock(&ril_data->request_mutex)
#define RIL_REQUEST_LOOP_LOCK()			pthread_mutex_lock(&ril_data->request_loop_mutex)
#define RIL_REQUEST_LOOP_UNLOCK()		pthread_mutex_unlock(&ril_data->request_loop_mutex)
#define RIL_CLIENT_LOCK(client)			pthread_mutex_lock(&client->mutex)
#define RIL_CLIENT_UNLOCK(client)		pthread_mutex_unlock(&client->mutex)

/*
 * RIL client
 */

struct ril_client;

enum {
	RIL_CLIENT_IPC_FMT,
	RIL_CLIENT_IPC_RFS,
	RIL_CLIENT_SRS,
};

struct ril_client_handlers {
	int (*create)(struct ril_client *client);
	int (*destroy)(struct ril_client *client);
	int (*open)(struct ril_client *client);
	int (*close)(struct ril_client *client);
	int (*loop)(struct ril_client *client);
};

struct ril_client_callbacks {
	int (*request_register)(struct ril_client *client, int request, RIL_Token token);
	int (*request_unregister)(struct ril_client *client, int request, RIL_Token token);
	int (*flush)(struct ril_client *client);
};

struct ril_client {
	int id;
	char *name;
	struct ril_client_handlers *handlers;
	struct ril_client_callbacks *callbacks;

	int failures;
	int available;
	void *data;

	pthread_t thread;
	pthread_mutex_t mutex;
};

extern struct ril_client *ril_clients[];
extern unsigned int ril_clients_count;

extern struct ipc_dispatch_handler ipc_fmt_dispatch_handlers[];
extern unsigned int ipc_fmt_dispatch_handlers_count;
extern struct ipc_dispatch_handler ipc_rfs_dispatch_handlers[];
extern unsigned int ipc_rfs_dispatch_handlers_count;
extern struct srs_dispatch_handler srs_dispatch_handlers[];
extern unsigned int srs_dispatch_handlers_count;

struct ril_client *ril_client_find_id(int id);
int ril_client_create(struct ril_client *client);
int ril_client_destroy(struct ril_client *client);
int ril_client_open(struct ril_client *client);
int ril_client_close(struct ril_client *client);
int ril_client_loop(struct ril_client *client);
int ril_client_request_register(struct ril_client *client, int request,
	RIL_Token token);
int ril_client_request_unregister(struct ril_client *client, int request,
	RIL_Token token);
int ril_client_flush(struct ril_client *client);

/*
 * RIL request
 */

enum {
	RIL_REQUEST_PENDING,
	RIL_REQUEST_HANDLED,
	RIL_REQUEST_UNHANDLED,
	RIL_REQUEST_COMPLETED,
};

struct ril_request_handler {
	int request;
	int (*handler)(void *data, size_t size, RIL_Token token);
};

struct ril_request {
	int request;
	void *data;
	size_t size;
	RIL_Token token;

	int status;
};

extern struct ril_request_handler ril_request_handlers[];
extern unsigned int ril_request_handlers_count;

int ril_request_register(int request, void *data, size_t size, RIL_Token token);
int ril_request_unregister(struct ril_request *request);
int ril_request_flush(void);
struct ril_request *ril_request_find(void);
struct ril_request *ril_request_find_request_status(int request, int status);
struct ril_request *ril_request_find_request(int request);
struct ril_request *ril_request_find_token(RIL_Token token);
struct ril_request *ril_request_find_status(int status);
int ril_request_complete(RIL_Token token, RIL_Errno error, void *data,
	size_t size);
int ril_request_unsolicited(int request, void *data, size_t size);
int ril_request_timed_callback(RIL_TimedCallback callback, void *data,
	const struct timeval *time);

/*
 * RIL request data
 */

struct ril_request_data {
	int request;
	void *data;
	size_t size;
};

int ril_request_data_register(int request, void *data, size_t size);
int ril_request_data_unregister(struct ril_request_data *request_data);
int ril_request_data_flush(void);
struct ril_request_data *ril_request_data_find_request(int request);
int ril_request_data_free(int request);
int ril_request_data_set(int request, void *data, size_t size);
int ril_request_data_set_uniq(int request, void *data, size_t size);
size_t ril_request_data_size_get(int request);
void *ril_request_data_get(int request);

/*
 * RIL radio state
 */

int ril_radio_state_update(RIL_RadioState radio_state);
int ril_radio_state_check(RIL_RadioState radio_state);

/*
 * RIL data
 */

struct ril_data {
	const struct RIL_Env *env;

	RIL_RadioState radio_state;
	char *sim_pin;

	struct list_head *requests;
	struct list_head *requests_data;

	struct list_head *data_connections;

	pthread_mutex_t mutex;

	pthread_t request_thread;
	pthread_mutex_t request_mutex;
	pthread_mutex_t request_loop_mutex;
};

extern struct ril_data *ril_data;

int ril_data_create(void);
int ril_data_destroy(void);

/*
 * Power
 */

int ipc_pwr_phone_pwr_up(struct ipc_message *message);
int ipc_pwr_phone_reset(struct ipc_message *message);
int ipc_pwr_phone_state(struct ipc_message *message);
int ril_request_radio_power(void *data, size_t size, RIL_Token token);

/*
 * Call
 */

int ril_request_dial(void *data, size_t size, RIL_Token token);
int ipc_call_incoming(struct ipc_message *message);
int ril_request_hangup(void *data, size_t size, RIL_Token token);
int ril_request_answer(void *data, size_t size, RIL_Token token);
int ipc_call_status(struct ipc_message *message);
int ril_request_last_call_fail_cause(void *data, size_t size, RIL_Token token);
int ipc_call_list(struct ipc_message *message);
int ril_request_get_current_calls(void *data, size_t size, RIL_Token token);
int ipc_call_cont_dtmf_callback(struct ipc_message *message);
int ipc_call_burst_dtmf(struct ipc_message *message);
int ril_request_dtmf_complete(unsigned char aseq, char tone);
int ril_request_dtmf(void *data, size_t size, RIL_Token token);
int ril_request_dtmf_start_complete(unsigned char aseq, char tone);
int ril_request_dtmf_start(void *data, size_t size, RIL_Token token);
int ril_request_dtmf_stop_complete(unsigned char aseq, int callback);
int ril_request_dtmf_stop(void *data, size_t size, RIL_Token token);

/*
 * SMS
 */

int ipc_sms_send_msg(struct ipc_message *message);
int ril_request_send_sms_complete(unsigned char seq, const void *smsc,
	size_t smsc_size, const void *pdu, size_t pdu_size);
int ril_request_send_sms(void *data, size_t size, RIL_Token token);
int ipc_sms_incoming_msg(struct ipc_message *message);
int ipc_sms_save_msg(struct ipc_message *message);
int ril_request_write_sms_to_sim(void *data, size_t size, RIL_Token token);
int ipc_sms_del_msg(struct ipc_message *message);
int ril_request_delete_sms_on_sim(void *data, size_t size, RIL_Token token);
int ipc_sms_deliver_report(struct ipc_message *message);
int ril_request_sms_acknowledge(void *data, size_t size, RIL_Token token);
int ipc_sms_svc_center_addr(struct ipc_message *message);

/*
 * SIM
 */

int ipc_sec_pin_status_callback(struct ipc_message *message);
int ipc_sec_pin_status(struct ipc_message *message);
int ril_request_get_sim_status(void *data, size_t size, RIL_Token token);
int ipc_sec_phone_lock(struct ipc_message *message);
int ril_request_query_facility_lock(void *data, size_t size, RIL_Token token);
int ipc_sec_callback(struct ipc_message *message);
int ril_request_set_facility_lock(void *data, size_t size, RIL_Token token);
int ril_request_enter_sim_pin(void *data, size_t size, RIL_Token token);
int ril_request_enter_sim_puk(void *data, size_t size, RIL_Token token);
int ril_request_enter_sim_pin2(void *data, size_t size, RIL_Token token);
int ril_request_enter_sim_puk2(void *data, size_t size, RIL_Token token);
int ril_request_change_sim_pin(void *data, size_t size, RIL_Token token);
int ril_request_change_sim_pin2(void *data, size_t size, RIL_Token token);
int ipc_sec_rsim_access(struct ipc_message *message);
int ril_request_sim_io(void *data, size_t size, RIL_Token token);
int ipc_sec_sim_icc_type(struct ipc_message *message);
int ipc_sec_lock_infomation(struct ipc_message *message);

/*
 * Network
 */

int ipc_disp_icon_info(struct ipc_message *message);
int ril_request_signal_strength(void *data, size_t size, RIL_Token token);
int ipc_disp_rssi_info(struct ipc_message *message);
int ipc_net_plmn_sel(struct ipc_message *message);
int ril_request_query_network_selection_mode(void *data, size_t size,
	RIL_Token token);
int ipc_net_plmn_sel_callback(struct ipc_message *message);
int ril_request_set_network_selection_automatic(void *data, size_t size,
	RIL_Token token);
int ril_request_set_network_selection_manual(void *data, size_t size,
	RIL_Token token);
int ipc_net_serving_network(struct ipc_message *message);
int ril_request_operator(void *data, size_t size, RIL_Token token);
int ipc_net_plmn_list(struct ipc_message *message);
int ril_request_query_available_networks(void *data, size_t size,
	RIL_Token token);
int ipc_net_regist(struct ipc_message *message);
#if RIL_VERSION >= 6
int ril_request_voice_registration_state(void *data, size_t size,
	RIL_Token token);
#else
int ril_request_registration_state(void *data, size_t size, RIL_Token token);
#endif
#if RIL_VERSION >= 6
int ril_request_data_registration_state(void *data, size_t size,
	RIL_Token token);
#else
int ril_request_gprs_registration_state(void *data, size_t size,
	RIL_Token token);
#endif
int ipc_net_mode_sel(struct ipc_message *message);
int ril_request_get_preferred_network_type(void *data, size_t size,
	RIL_Token token);
int ril_request_set_preferred_network_type(void *data, size_t size,
	RIL_Token token);

/*
 * Sound
 */

int srs_snd_set_call_volume(struct srs_message *message);
int ril_request_set_mute(void *data, size_t size, RIL_Token token);
int srs_snd_set_mic_mute(struct srs_message *message);
int srs_snd_set_call_audio_path(struct srs_message *message);
int srs_snd_set_call_clock_sync(struct srs_message *message);

/*
 * Misc
 */

int ipc_misc_me_version(struct ipc_message *message);
int ril_request_baseband_version(void *data, size_t size, RIL_Token token);
int ipc_misc_me_imsi(struct ipc_message *message);
int ril_request_get_imsi(void *data, size_t size, RIL_Token token);
int ipc_misc_me_sn(struct ipc_message *message);
int ril_request_get_imei(void *data, size_t size, RIL_Token token);
int ril_request_get_imeisv(void *data, size_t size, RIL_Token token);
int ipc_misc_time_info(struct ipc_message *message);
int ril_request_screen_state(void *data, size_t size, RIL_Token token);

/*
 * SS
 */

enum {
	USSD_ENCODING_UNKNOWN,
	USSD_ENCODING_GSM7,
	USSD_ENCODING_UCS2
};

int ipc_ss_ussd(struct ipc_message *message);
int ril_request_send_ussd(void *data, size_t size, RIL_Token token);
int ril_request_cancel_ussd(void *data, size_t size, RIL_Token token);

/*
 * OEM
 */

int ipc_svc_display_screen(struct ipc_message *message);
int ril_request_oem_hook_raw(void *data, size_t size, RIL_Token token);

/*
 * Data
 */

struct ril_data_connection {
	unsigned int cid;
	RIL_Token token;

	int enabled;
	int attached;

	char *apn;
	char *username;
	char *password;

	char *iface;
	char *ip;
	char *gateway;
	char *subnet_mask;
	char *dns1;
	char *dns2;
};

int ril_data_connection_register(unsigned int cid, char *apn, char *username,
	char *password, char *iface);
int ril_data_connection_unregister(struct ril_data_connection *data_connection);
int ril_data_connection_flush(void);
struct ril_data_connection *ril_data_connection_find_cid(unsigned int cid);
struct ril_data_connection *ril_data_connection_find_token(RIL_Token token);
struct ril_data_connection *ril_data_connection_start(char *apn, char *username,
	char *password);
int ril_data_connection_stop(struct ril_data_connection *data_connection);
int ril_data_connection_enable(struct ril_data_connection *data_connection);
int ril_data_connection_disable(struct ril_data_connection *data_connection);
int ipc_gprs_define_pdp_context_callback(struct ipc_message *message);
int ril_request_setup_data_call(void *data, size_t size, RIL_Token token);
int ipc_gprs_ps(struct ipc_message *message);
int ipc_gprs_pdp_context(struct ipc_message *message);
int ril_request_data_call_list(void *data, size_t size, RIL_Token token);
int ipc_gprs_pdp_context_callback(struct ipc_message *message);
int ril_request_deactivate_data_call(void *data, size_t size, RIL_Token token);
int ipc_gprs_ip_configuration(struct ipc_message *message);
int ipc_gprs_hsdpa_status(struct ipc_message *message);
int ipc_gprs_call_status(struct ipc_message *message);
int ril_request_last_data_call_fail_cause(void *data, size_t size,
	RIL_Token token);

/*
 * RFS
 */

int ipc_rfs_nv_read_item(struct ipc_message *message);
int ipc_rfs_nv_write_item(struct ipc_message *message);

/*
 * GEN
 */

struct ipc_gen_phone_res_expect {
	unsigned char aseq;
	unsigned short command;
	int (*callback)(struct ipc_message *message);
	int complete;
	int abort;
};

int ipc_gen_phone_res_expect_register(struct ril_client *client,
	unsigned char aseq, unsigned short command,
	int (*callback)(struct ipc_message *message),
	int complete, int abort);
int ipc_gen_phone_res_expect_unregister(struct ril_client *client,
	struct ipc_gen_phone_res_expect *expect);
int ipc_gen_phone_res_expect_flush(struct ril_client *client);
struct ipc_gen_phone_res_expect *ipc_gen_phone_res_expect_find_aseq(struct ril_client *client,
	unsigned char aseq);
int ipc_gen_phone_res_expect_callback(unsigned char aseq, unsigned short command,
	int (*callback)(struct ipc_message *message));
int ipc_gen_phone_res_expect_complete(unsigned char aseq, unsigned short command);
int ipc_gen_phone_res_expect_abort(unsigned char aseq, unsigned short command);
int ipc_gen_phone_res(struct ipc_message *message);

#endif
