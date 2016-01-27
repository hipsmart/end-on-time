#include <pebble.h>

#define KEY_EVENT_TITLE 0
#define KEY_EVENT_TIME 1

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_countdown_layer;
static TextLayer *s_message_layer;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M%p", tick_time);
  static char s_minute[3];
  strftime(s_minute, sizeof(s_minute), "%M", tick_time);
  static int s_cd;
  s_cd = 60 - atoi(s_minute);
  static char s_countdown[3];
  snprintf(s_countdown, 3, "%02d", s_cd);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  //Also update the countdown on its TextLayer
  text_layer_set_text(s_countdown_layer, s_countdown);

  //Also update the message on its TextLayer
//  text_layer_set_text(s_message_layer, "minutes left in event");
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 0), bounds.size.w, 40));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorPictonBlue);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // Create countdown Layer
  s_countdown_layer = text_layer_create(
  GRect(0, PBL_IF_ROUND_ELSE(58, 50), bounds.size.w, 50));

  // Style the text
  text_layer_set_background_color(s_countdown_layer, GColorWhite);
  text_layer_set_text_color(s_countdown_layer, GColorBlack);
  text_layer_set_font(s_countdown_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_countdown_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_countdown_layer));

  // Create message Layer
  s_message_layer = text_layer_create(
  GRect(0, PBL_IF_ROUND_ELSE(58, 90), bounds.size.w, 80));

  // Style the text
  text_layer_set_background_color(s_message_layer, GColorClear);
  text_layer_set_text_color(s_message_layer, GColorBlack);
  text_layer_set_font(s_message_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_message_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_message_layer));

}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_countdown_layer);
  text_layer_destroy(s_message_layer);

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_EVENT_TIME);
  Tuple *conditions_tuple = dict_find(iterator, KEY_EVENT_TITLE);
  
  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    // Assemble full string and display
//    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "minutes until %s", conditions_buffer);
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "minutes left in %s", "Manager Meeting");

    text_layer_set_text(s_message_layer, weather_layer_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
 
  // Open AppMessage
  app_message_open(64, 64);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}