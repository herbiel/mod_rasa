#include <switch.h>
#include <curl/curl.h>
#define MAX_PATH_LEN (256)

SWITCH_MODULE_LOAD_FUNCTION(mod_asr_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_asr_shutdown);
SWITCH_MODULE_DEFINITION(mod_asr, mod_asr_load, mod_asr_shutdown, NULL);

static struct {
    switch_memory_pool_t *pool;
    int deleteFileScore = 90;
    int maxSampleSec = 10;
    char* pcmDir = "/data/freeswitch";
    char* sampleDir = "/data/freeswitch/sample";
} globals;
typedef struct {
    switch_core_session_t   *session;
    switch_media_bug_t      *bug;
    FILE                    *stream;
    char                    *callUUID;
    char                    *filePath;
    int                     stop;
    int                     totalSample;
    int                     sampleRate;
    int                     currentMaxSampleSec;
} switch_da_t;
typedef struct {
    char                    audioName[MAX_PATH_LEN];
    long                    size;
    double                  *standardData;
} standard_audio_t;
//全局变量

SWITCH_MODULE_DEFINITION(mod_asr, mod_asr_load, mod_asr_shutdown, NULL);


static switch_bool_t asr_callback(switch_media_bug_t *bug, void *user_data, switch_abc_type_t type)
{
    switch_da_t *pvt = (switch_da_t *)user_data;
    if(pvt == NULL || pvt->stop == 1)
        return SWITCH_FALSE;
    switch_channel_t *channel = switch_core_session_get_channel(pvt->session);
    switch (type) {
        case SWITCH_ABC_TYPE_INIT:
            {
                pvt->filePath = (char*)malloc(strlen(globals.pcmDir) + strlen(pvt->callUUID) + 5 + 1);
                sprintf(pvt->filePath, "%s/%s.pcm", globals.pcmDir, pvt->callUUID);
                pvt->stream = fopen(pvt->filePath, "wb");
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%s HD Start Succeed channel:%s, file:%s\n",pvt->callUUID, switch_channel_get_name(channel),pvt->filePath);
            }
            break;
        case SWITCH_ABC_TYPE_CLOSE:
            {
                if ( pvt->stream != NULL ) {
                    fclose(pvt->stream);
                }
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%s HD Stop Succeed channel:%s\n",pvt->callUUID, switch_channel_get_name(channel));
            }
            break;
        case SWITCH_ABC_TYPE_READ_REPLACE:
            {
                switch_frame_t *frame;
                if ((frame = switch_core_media_bug_get_read_replace_frame(bug))) {
                    char*frame_data = (char*)frame->data;
                    int frame_len = frame->datalen;
                    //默认的采样率  需要转换成8K采样率  每次samples个采样(每秒50次),   8000 / 50 / samples 就是倍率
                    switch_core_media_bug_set_read_replace_frame(bug, frame);
                    long sampleRate = frame->samples * 50;
                    if(pvt->sampleRate == 0) {
                        pvt->sampleRate = 8000;
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "%s SWITCH_ABC_TYPE_READ_REPLACE sampleRate before Trans: %f\n",pvt->callUUID, sampleRate);
                    }
                    if(sampleRate > 8000) {
                        int beilv = sampleRate / 8000;
                        char newData[frame_len / beilv];
                        for(int i =0 ;i < frame_len / (beilv * 2);i++) {
                            newData[i * 2] = frame_data[i * beilv * 2];
                            newData[i * 2 + 1] = frame_data[i * beilv * 2 + 1];
                        }
                        if ( pvt->stream != NULL ) {
                            fwrite(newData, sizeof(char), frame_len / beilv , pvt->stream);
                        }
                    } else {
                        if ( pvt->stream != NULL ) {
                            fwrite(frame_data, sizeof(char), frame_len , pvt->stream);
                        }
                    }
                    pvt->totalSample ++;
                    if(pvt->totalSample > pvt->currentMaxSampleSec * 50) {
                        fclose(pvt->stream);
                        pvt->stream = NULL;
                        pvt->stop = 1;
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%s HD doCompare after %d sec, filePath:%s\n", pvt->callUUID, pvt->currentMaxSampleSec, pvt->filePath);
                        pthread_t tid;
                        int ret = pthread_create(&tid, NULL, doCompare, pvt);
                        if (ret != 0) {
                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "%s HD doCompare thread create faild, ret: %d\n", pvt->callUUID, ret);
                            return SWITCH_FALSE;
                        }
//                        doCompare(pvt);
						  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "test success");
                    }
                }
            }
            break;
        default: break;
    }
    return SWITCH_TRUE;
}


SWITCH_STANDARD_APP(stop_asr_session_function)
{
    switch_da_t *pvt;
    switch_channel_t *channel = switch_core_session_get_channel(session);
    if ((pvt = (switch_da_t*)switch_channel_get_private(channel, "asr"))) {
        switch_channel_set_private(channel, "asr", NULL);
        switch_core_media_bug_remove(session, &pvt->bug);
        switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_DEBUG, "%s Stop HD\n", switch_channel_get_name(channel));
    }
}

SWITCH_STANDARD_APP(start_asr_session_function)
{
    switch_channel_t *channel = switch_core_session_get_channel(session);
    switch_status_t status;
    switch_da_t *pvt;
    switch_codec_implementation_t read_impl;
    memset(&read_impl, 0, sizeof(switch_codec_implementation_t));
    char *argv[2] = { 0 };
    int argc;
    char *lbuf = NULL;
    switch_core_session_get_read_impl(session, &read_impl);
    if (!(pvt = (switch_da_t*)switch_core_session_alloc(session, sizeof(switch_da_t)))) {
        return;
    }
    pvt->stop = 0;
    pvt->totalSample = 0;
    pvt->currentMaxSampleSec = globals.maxSampleSec;
    pvt->sampleRate = 0;
    pvt->session = session;
    pvt->callUUID = switch_core_session_get_uuid(session);
    if (!zstr(data) && (lbuf = switch_core_session_strdup(session, data))) {
        pvt->currentMaxSampleSec = atoi(lbuf);
    }
    if ((status = switch_core_media_bug_add(session, "asr", NULL,
        asr_callback, pvt, 0, SMBF_READ_REPLACE | SMBF_NO_PAUSE | SMBF_ONE_ONLY, &(pvt->bug))) != SWITCH_STATUS_SUCCESS) {
        return;
    }
    switch_channel_set_private(channel, "asr", pvt);
    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_DEBUG, "%s Start HD %s\n", switch_channel_get_name(channel), switch_core_session_get_uuid(session));
}


SWITCH_MODULE_LOAD_FUNCTION(mod_asr_load)
{
    switch_application_interface_t *app_interface;
    globals.pool = pool;
    *module_interface = switch_loadable_module_create_module_interface(globals.pool, modname);
    SWITCH_ADD_APP(app_interface, "start_hd", "start hd", "start hangup detect", start_asr_session_function, "", SAF_MEDIA_TAP);
    SWITCH_ADD_APP(app_interface, "stop_hd", "stop hd", "stop hangup detect", stop_asr_session_function, "", SAF_NONE);
    return SWITCH_STATUS_SUCCESS;
 
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_asr_shutdown)
{
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " hangup detect shutdown\n");
    return SWITCH_STATUS_SUCCESS;
}

