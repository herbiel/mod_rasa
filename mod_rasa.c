#include <switch.h>


SWITCH_MODULE_LOAD_FUNCTION(mod_rasa_load);
//SWITCH_MODULE_RUNTIME_FUNCTION(mod_rasa_runtime);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown);

extern "C"
{
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
}

// Actually it explains as followings:
// switch_status_t mod_rasa_load(switch_loadable_module_interface_t **module_interface, switch_memory_pool_t *pool)
SWITCH_MODULE_LOAD_FUNCTION(mod_rasa_load) {
    // init module interface
    *module_interface = switch_loadable_module_create_module_interface(pool, modname);

    return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_rasa_shutdown)
{
    return SWITCH_STATUS_SUCCESS;
}