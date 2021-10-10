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
SWITCH_MODULE_DEFINITION(mod_rasa, mod_rasa_load, mod_rasa_shutdown, NULL);

SWITCH_STANDARD_API(rasa_function){
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "rasa load success\n");
	return SWITCH_STATUS_SUCCESS;
}

static switch_bool_t record_callback(switch_media_bug_t *bug, void *user_data, switch_abc_type_t type)
{
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
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "test -----READ_REPLACE: \n");
		break;
	}
	case SWITCH_ABC_TYPE_READ_PING:
	{//读取到音频流
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
	switch_media_bug_t *bug;
	switch_status_t status;

	if ((status = switch_core_media_bug_add(session, "my_rasa", "",
										record_callback, "", 0, SMBF_READ_STREAM | SMBF_WRITE_STREAM | SMBF_READ_PING, &bug)) != SWITCH_STATUS_SUCCESS) {
	switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Error adding media bug for file \n");
	}

}


SWITCH_STANDARD_APP(start_rasa_function)
{
	switch_channel_t *channel = switch_core_session_get_channel(session);
	const char *call_uuid = switch_channel_get_variable(channel, "uuid");
	//switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "test show uuid is %s \n",call_uuid);
	//callprogress_detector_start(session,'1');
	char *file_path = NULL;
	file_path = "/var/wwww/test";
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "test show uuid is %s %s\n",call_uuid,file_path);

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
	SWITCH_ADD_APP(app_interface, "start_my_rasa", "start_my_rasa", "start_my_rasa", start_rasa_function, "", SAF_NONE);
	SWITCH_ADD_APP(app_interface, "my_rasa", "my_rasa", "my_rasa", rasa_session_function, "", SAF_NONE);
    return SWITCH_STATUS_SUCCESS;
}


SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown)
{
    return SWITCH_STATUS_SUCCESS;
}