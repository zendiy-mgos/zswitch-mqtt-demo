load("api_config.js")
load("api_events.js")
load("api_zswitch_gpio.js")
load("api_zswitch_mqtt.js")

/* Create switches having the same group ID,
 * so when one will be turned on, all others
 * will be automatically torned off.
 */   
let cfg = {groupId: 1};
let sw1 = ZenSwitch.create(Cfg.get('app.relay1.id'), cfg);
let sw2 = ZenSwitch.create(Cfg.get('app.relay2.id'), cfg);

let success = false;
let gpioCfg = {activeHigh: Cfg.get('app.relays_active_high')};
if (sw1 && sw2) {
  if (sw1.GPIO.attach(Cfg.get('app.relay1.pin'), gpioCfg) &&
      sw2.GPIO.attach(Cfg.get('app.relay2.pin'), gpioCfg)) {
    
    let stateTopic = Cfg.get('app.switches.state_topic');
    let cmdTopic = Cfg.get('app.switches.cmd_topic');
    if (sw1.MQTT.attach(stateTopic, cmdTopic) &&
        sw2.MQTT.attach(stateTopic, cmdTopic)) {
      Event.addHandler(ZenThing.EV_STATE_UPDATED, function(ev, evdata, ud) {
        let state = ZenSwitch.parseState(evdata);
        print('Switch', state.thing.id, 'switched', state.value ? 'ON' : 'OFF');
      }, null);
      success = true;
    }
  }
}

if (!success) {
  if (sw1) {
    sw1.GPIO.detach();
    sw1.MQTT.detach();
    sw1.close();
  }
  if (sw2) {
    sw2.GPIO.detach();
    sw2.MQTT.detach();
    sw2.close();
  }
  print('Error initializing the firmware');
}
