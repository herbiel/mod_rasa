#include <switch.h>
#include <curl/curl.h>


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

// #define MAX_PEERS 128
SWITCH_STANDARD_API(rasa_function){
	// switch_dial_handle_t *dh;
	// switch_dial_leg_list_t *ll;
	// switch_dial_leg_t *leg = NULL;

	// char *peer_names[MAX_PEERS] = { 0 };
	// switch_event_t *peer_vars[MAX_PEERS] = { 0 };
	// int i;
	// switch_core_session_t *peer_session = NULL;
	// switch_call_cause_t cause;
	
	
	// switch_dial_handle_create(&dh);


	// switch_dial_handle_add_global_var(dh, "ignore_early_media", "true");
	// switch_dial_handle_add_global_var_printf(dh, "coolness_count", "%d", 12);
	// switch_dial_handle_add_leg_list(dh, &ll);

	// switch_dial_leg_list_add_leg(ll, &leg, "user/1002");
	// switch_dial_handle_get_peers(dh, 0, peer_names, MAX_PEERS);
	// switch_dial_handle_get_vars(dh, 0, peer_vars, MAX_PEERS);

	// for(i = 0; i < MAX_PEERS; i++) {
	// 	if (peer_names[i]) {
	// 		char *foo;
			
	// 		printf("peer: [%s]\n", peer_names[i]);

	// 		if (peer_vars[i]) {
	// 			if (switch_event_serialize(peer_vars[i], &foo, SWITCH_FALSE) == SWITCH_STATUS_SUCCESS) {
	// 				printf("%s\n", foo);
	// 			}
	// 		}
	// 		printf("\n\n");
	// 	}
	// }


	// switch_ivr_originate(NULL, &peer_session, &cause, NULL, 0, NULL, NULL, NULL, NULL, NULL, SOF_NONE, NULL, dh);
    long http_code;
	CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: Agent-007");
    curl = curl_easy_init();    // 初始化
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// 改协议头
    curl_easy_setopt(curl, CURLOPT_URL,"https://www.baidu.com");
    res = curl_easy_perform(curl);   // 执行
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != 0) {

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%s\n", http_code);

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