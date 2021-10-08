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
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "rasa load success\n",p);
	return SWITCH_STATUS_SUCCESS;
}

SWITCH_STANDARD_APP(rasa_session_function)
{
	switch_channel_t *channel = switch_core_session_get_channel(session);
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, switch_channel_get_variable(channel, "uuid"));
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
	SWITCH_ADD_APP(app_interface, "my_rasa", "my_rasa", "my_rasa", rasa_session_function, "", SAF_NONE);
    return SWITCH_STATUS_SUCCESS;
}


SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown)
{
    return SWITCH_STATUS_SUCCESS;
}