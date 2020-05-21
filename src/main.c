#include "mgos.h"
#include "mgos_zswitch_gpio.h"
#include "mgos_zswitch_mqtt.h"

void zswitch_state_updated_cb(int ev, void *ev_data, void *ud) {
  struct mgos_zswitch_state *state = (struct mgos_zswitch_state *)ev_data;
  LOG(LL_INFO, ("Switch '%s' switched %s", state->handle->id,
    (state->value ? "ON" : "OFF")));
  (void) ud;
  (void) ev;
}

enum mgos_app_init_result mgos_app_init(void) {
  /* Create switches having the same group ID,
   * so when one will be turned on, all others
   * will be automatically torned off.
   */
  struct mgos_zswitch_gpio_cfg gpio_cfg = MGOS_ZSWITCH_GPIO_CFG;
  gpio_cfg.active_high = mgos_sys_config_get_app_relays_active_high();

  struct mgos_zswitch_cfg cfg = MGOS_ZSWITCH_CFG;
  cfg.group_id = 1;
  
  struct mgos_zswitch *sw1 = NULL;
  sw1 = mgos_zswitch_create(mgos_sys_config_get_app_relay1_id(), &cfg);
  if (sw1 == NULL) return MGOS_APP_INIT_ERROR;
  /* Attach switch to GPIOs */
  if (!mgos_zswitch_gpio_attach(sw1, mgos_sys_config_get_app_relay1_pin(), &gpio_cfg)) {
    return MGOS_APP_INIT_ERROR;
  }
  /* Attach switch to MQTT */
  if (!mgos_zswitch_mqtt_attach(sw1, mgos_sys_config_get_app_switches_state_topic(),
      mgos_sys_config_get_app_switches_cmd_topic(), NULL)) {
    return MGOS_APP_INIT_ERROR;
  }

  struct mgos_zswitch *sw2 = NULL;
  sw2 = mgos_zswitch_create(mgos_sys_config_get_app_relay2_id(), &cfg);
  if (sw2 == NULL) return MGOS_APP_INIT_ERROR;
  /* Attach switch to GPIOs */
  if (!mgos_zswitch_gpio_attach(sw2, mgos_sys_config_get_app_relay2_pin(), &gpio_cfg)) {
    return MGOS_APP_INIT_ERROR;
  }
  /* Attach switch to MQTT */
  if (!mgos_zswitch_mqtt_attach(sw2, mgos_sys_config_get_app_switches_state_topic(),
      mgos_sys_config_get_app_switches_cmd_topic(), NULL)) {
    return MGOS_APP_INIT_ERROR;
  }

  mgos_event_add_handler(MGOS_EV_ZTHING_STATE_UPDATED,
    zswitch_state_updated_cb, NULL);
 
  return MGOS_APP_INIT_SUCCESS;
}