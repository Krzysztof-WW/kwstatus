#include "../kwstatus.h"
#include <pthread.h>
#include <pulse/def.h>
#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>
#include <pulse/error.h>
#include <pulse/volume.h>
#include <pulse/subscribe.h>

#define OUTBUF 100

#define SINK_MUTE_ICON "🔇"
#define SOURCE_MUTE_ICON ""

#define AUDIO_LEVEL_MIN "🔈"
#define AUDIO_LEVEL_MID "🔉"
#define AUDIO_LEVEL_MAX "🔊"

struct modules* mod;
static enum pa_context_state state;
static pa_server_info sinfo;
static pa_mainloop* pml;

static uint32_t default_sink_index;
static uint32_t default_source_index;
static int volume;
static int sink_mute;
static int source_mute;

static char out[OUTBUF+1];

static void
update_text(void) {
  char* sink_icon;

  if(sink_mute)
    sink_icon = SINK_MUTE_ICON;
  else if(volume > 70)
    sink_icon = AUDIO_LEVEL_MAX;
  else if(volume > 30)
    sink_icon = AUDIO_LEVEL_MID;
  else
    sink_icon = AUDIO_LEVEL_MIN;

  if(source_mute)
    snprintf(out, OUTBUF, SOURCE_MUTE_ICON " %s %d%%", sink_icon, volume);
  else
    snprintf(out, OUTBUF, "%s %d%%", sink_icon, volume);

  mod_update(mod, out);
}

static void
update_sink_info(const pa_sink_info* info) {
  pa_volume_t pvol;

  pvol = pa_cvolume_avg(&info->volume);
  volume = (double)pvol / PA_VOLUME_NORM * 100.0;
  sink_mute = info->mute;
}

static void
update_source_info(const pa_source_info* info) {
  source_mute = info->mute;
}

static void
wait_til_op_end(pa_operation* op) {
  while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
    pa_mainloop_iterate(pml, 1, NULL);
  }

  pa_operation_unref(op);
}

static void 
sink_cb(pa_context* context, const pa_sink_info* info, int eol, void* _) {
  if(eol) {
    return;
  }
  update_sink_info(info);
  update_text();
  default_sink_index = info->index;
}

static void 
source_cb(pa_context* context, const pa_source_info* info, int eol, void* _) {
  if(eol) {
    return;
  }
  update_source_info(info);
  update_text();
  default_source_index = info->index;
}

static void 
servinfo_cb(pa_context* context, const pa_server_info* info, void* _) {
  sinfo = *info;
  pa_context_get_sink_info_by_name(context, sinfo.default_sink_name, sink_cb, NULL);
  pa_context_get_source_info_by_name(context, sinfo.default_source_name, source_cb, NULL);
}

static void
state_cb(pa_context* c, void* _) {
  state = pa_context_get_state(c);
}

static void
subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {
  if((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK) { // update status of sink
    pa_context_get_sink_info_by_index(c, default_sink_index, sink_cb, NULL);
  } else if((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SOURCE) { // update mute of source
    pa_context_get_source_info_by_index(c, default_source_index, source_cb, NULL);
  } else { // update server info and update default sink and source
    pa_context_get_server_info(c, servinfo_cb, NULL);
  }
}

static void
success_cb(pa_context *c, int success, void *userdata) {
  if(!success)
    warn("Pulseaudio subscription not succeed");
}

void
pulse(void* self) {
  mod = (struct modules*)self;
  pa_context* pcontext;

  pml = pa_mainloop_new();
  pcontext = pa_context_new(pa_mainloop_get_api(pml), "kwstatus");
  pa_context_set_state_callback(pcontext, state_cb, NULL);
  pa_context_connect(pcontext, NULL, PA_CONTEXT_NOFLAGS, NULL);

  while (state != PA_CONTEXT_READY && state != PA_CONTEXT_FAILED) {
    pa_mainloop_iterate(pml, 1, NULL);
  }
  if (state != PA_CONTEXT_READY) {
    fprintf(stderr, "failed to connect to pulse daemon: %s\n", pa_strerror(pa_context_errno(pcontext)));
    return;
  }

  wait_til_op_end(pa_context_get_server_info(pcontext, servinfo_cb, NULL));
  pa_context_set_subscribe_callback(pcontext, subscribe_cb, NULL);
  wait_til_op_end(pa_context_subscribe(pcontext, PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE | PA_SUBSCRIPTION_MASK_SERVER, success_cb, NULL));
  pa_mainloop_run(pml, NULL);
}
