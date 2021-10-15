#include <switch.h>



SWITCH_MODULE_LOAD_FUNCTION(mod_rasa_load);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_rasa_runtime);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown);



// Actually it explains as followings:
// static const char modname[] = "mod_rasa";
// SWITCH_MOD_DECLARE_DATA switch_loadable_module_function_table_t mod_rasa_module_interface ={
//  SWITCH_API_VERSION,
//  mod_rasa_load,
//  mod_rasa_shutdown,
//  mod_rasa_runtime(NULL),
//  SMODF_NONE
// }
//
SWITCH_MODULE_DEFINITION(mod_rasa, mod_rasa_load, mod_rasa_shutdown, NULL);

struct record_helper {
	switch_media_bug_t *bug;
	switch_memory_pool_t *helper_pool;
	switch_core_session_t *recording_session;
	switch_core_session_t *transfer_from_session;
	uint8_t transfer_complete;
	char *file;
	switch_file_handle_t *fh;
	switch_file_handle_t in_fh;
	switch_file_handle_t out_fh;
	int native;
	uint32_t packet_len;
	int min_sec;
	int final_timeout_ms;
	int initial_timeout_ms;
	int silence_threshold;
	int silence_timeout_ms;
	switch_time_t silence_time;
	int rready;
	int wready;
	switch_time_t last_read_time;
	switch_time_t last_write_time;
	switch_bool_t hangup_on_error;
	switch_codec_implementation_t read_impl;
	switch_bool_t speech_detected;
	switch_buffer_t *thread_buffer;
	switch_thread_t *thread;
	switch_mutex_t *buffer_mutex;
	int thread_ready;
	uint8_t thread_needs_transfer;
	uint32_t writes;
	uint32_t vwrites;
	const char *completion_cause;
	int start_event_sent;
	switch_event_t *variables;
};
static switch_status_t record_helper_create(struct record_helper **rh, switch_core_session_t *session)
{
	switch_status_t status;
	switch_memory_pool_t *pool;
	struct record_helper *newrh;

	assert(rh);
	assert(session);

	if ((status = switch_core_new_memory_pool(&pool)) != SWITCH_STATUS_SUCCESS) {
		return status;
	}

	if (!(newrh = switch_core_alloc(pool, sizeof(*newrh)))) {
		switch_core_destroy_memory_pool(&pool);
		return SWITCH_STATUS_MEMERR;
	}

	newrh->helper_pool = pool;
	newrh->recording_session = session;

	*rh = newrh;

	return SWITCH_STATUS_SUCCESS;
}

static switch_status_t record_helper_destroy(struct record_helper **rh, switch_core_session_t *session)
{
	switch_memory_pool_t *pool;

	assert(rh);
	assert(*rh);
	assert(session);

	if ((*rh)->recording_session != session) {
		switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Destroying a record helper of another session!\n");
	}

	if ((*rh)->native) {
		switch_core_file_close(&(*rh)->in_fh);
		switch_core_file_close(&(*rh)->out_fh);
	} else if((*rh)->fh) {
		switch_core_file_close((*rh)->fh);
	}

	pool = (*rh)->helper_pool;
	switch_core_destroy_memory_pool(&pool);
	*rh = NULL;

	return SWITCH_STATUS_SUCCESS;
}


SWITCH_STANDARD_API(rasa_function){
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "rasa load success\n");
	return SWITCH_STATUS_SUCCESS;
}

static switch_bool_t rasa_record_callback(switch_media_bug_t *bug, void *user_data, switch_abc_type_t type)
{
	switch_core_session_t *session = switch_core_media_bug_get_session(bug);
	struct record_helper *rh = (struct record_helper *) user_data;
	if (rh->recording_session != session) {
		return SWITCH_FALSE;
	}
	switch (type) {
	case SWITCH_ABC_TYPE_INIT: {//媒体bug设置成功
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "test -----1 \n");
		break;
	}
	case SWITCH_ABC_TYPE_CLOSE: { //媒体流关闭 资源回收
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "test -----2 \n");
		break;
	}
	case SWITCH_ABC_TYPE_READ_REPLACE: {//读取到音频流
		switch_size_t len;
		uint8_t data[SWITCH_RECOMMENDED_BUFFER_SIZE];
		switch_frame_t frame = { 0 };
		int i = 0;
		frame.data = data;
		frame.buflen = SWITCH_RECOMMENDED_BUFFER_SIZE;
		for (;;) {
			switch_core_media_bug_read(bug, &frame, i++ == 0 ? SWITCH_FALSE : SWITCH_TRUE);
			len = (switch_size_t) frame.datalen / 2 / frame.channels;
			switch_core_file_write(rh->fh, data, &len);
			rh->writes++;

		}
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "test -----READ_PING \n");
		break;
	}

	default: {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "test all");
		break;
	}
}
	return SWITCH_TRUE;
}


SWITCH_STANDARD_APP(rasa_session_function)
{


	switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "test show YYYYYYYYYYY");

}


SWITCH_STANDARD_APP(start_rasa_function)
{
	switch_channel_t *channel = switch_core_session_get_channel(session);
	const char *call_uuid = switch_channel_get_variable(channel, "uuid");
	//switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "test show uuid is %s \n",call_uuid);
	//callprogress_detector_start(session,'1');
	char *path = NULL;
	path = switch_core_session_strdup(session, data);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "test show uuid is %s %s\n",call_uuid,path);

}
// Actually it explains as followings:
// switch_status_t mod_rasa_load(switch_loadable_module_interface_t **module_interface, switch_memory_pool_t *pool)
SWITCH_MODULE_LOAD_FUNCTION(mod_rasa_load) {
    // init module interface
    switch_api_interface_t *api_interface;
	switch_application_interface_t *app_interface;
    *module_interface = switch_loadable_module_create_module_interface(pool, modname);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Hello World!\n");
    SWITCH_ADD_API(api_interface, "rasa", "RASA API", rasa_function, "syntax");
	SWITCH_ADD_APP(app_interface, "start_my_rasa", "start_my_rasa", "start_my_rasa", start_rasa_function, "<path>", SAF_NONE);
	SWITCH_ADD_APP(app_interface, "my_rasa", "my_rasa", "my_rasa", rasa_session_function, "<path>", SAF_MEDIA_TAP);
    return SWITCH_STATUS_SUCCESS;
}


SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown)
{
    return SWITCH_STATUS_SUCCESS;
}