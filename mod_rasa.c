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




SWITCH_STANDARD_API(rasa_function){
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "rasa load success\n");
	return SWITCH_STATUS_SUCCESS;
}



SWITCH_STANDARD_APP(rasa_session_function)
{

	switch_file_handle_t *fh;
	char *path = NULL;
	uint32_t limit = 0;
	path = "/var/lib/freeswitch/recordings/test.wav";
    switch_ivr_record_session_event(session, file, limit, fh, NULL);
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