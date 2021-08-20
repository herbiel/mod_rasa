#include <switch.h>
#include <curl/curl.h>


SWITCH_MODULE_LOAD_FUNCTION(mod_rasa_load);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_rasa_runtime);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown);



SWITCH_MODULE_DEFINITION(mod_rasa, mod_rasa_load, mod_rasa_shutdown, NULL);


SWITCH_STANDARD_API(rasa_function){
	switch_channel_t *channel;
	const char *billsec;

	if (!(channel = switch_core_session_get_channel(session))) {
		return SWITCH_STATUS_SUCCESS;
	}
	if switch_channel_get_state(channel) == CS_HANGUP) {
		billsec = switch_channel_get_variable(channel, "billsec");
	}
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%s\n",billsec);
	return SWITCH_STATUS_SUCCESS;

}
SWITCH_MODULE_LOAD_FUNCTION(mod_rasa_load) {
    // init module interface
	switch_api_interface_t *api_interface;
    *module_interface = switch_loadable_module_create_module_interface(pool, modname);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Hello World!\n");
	SWITCH_ADD_API(api_interface, "rasa", "RASA API", rasa_function, "syntax");
    return SWITCH_STATUS_SUCCESS;
}


SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown)
{
    return SWITCH_STATUS_SUCCESS;
}