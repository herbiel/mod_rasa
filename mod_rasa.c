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
	switch_dial_handle_t *dh;
	switch_dial_leg_list_t *ll;
	switch_dial_leg_t *leg = NULL;

	
	switch_dial_handle_create(&dh);


	switch_dial_handle_add_global_var(dh, "ignore_early_media", "true");
	switch_dial_handle_add_global_var_printf(dh, "coolness_count", "%d", 12);
	switch_dial_handle_add_leg_list(dh, &ll);

	switch_dial_leg_list_add_leg(ll, &leg, "user/1002");
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Hello rasa!\n");

	return SWITCH_STATUS_SUCCESS;
}
// Actually it explains as followings:
// switch_status_t mod_rasa_load(switch_loadable_module_interface_t **module_interface, switch_memory_pool_t *pool)
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