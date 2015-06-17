#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_m_layer;
static TextLayer *s_date_layer;
static TextLayer *s_date_bold_layer;
static TextLayer *s_connection_layer;
static GBitmap *s_canyon_bitmap;
static BitmapLayer *s_bitmap_layer;


static void handle_bluetooth(bool connected) {
  text_layer_set_text(s_connection_layer, connected ? " " : "!");
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:";
  static char bufferm[] = "00:";
  static char bufferd[] = "XXX 00   /00";
  static char bufferd2[] = "00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:"), "%H:", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:"), "%I:", tick_time);
  }
  
  strftime(bufferm, sizeof("00"), "%M", tick_time);
  
  strftime(bufferd, sizeof("XXX 00   /00"), "%a      /%m", tick_time);
  strftime(bufferd2, sizeof("00"), "%d", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_time_m_layer, bufferm);
  text_layer_set_text(s_date_layer, bufferd);
  text_layer_set_text(s_date_bold_layer, bufferd2);
  
  handle_bluetooth(bluetooth_connection_service_peek());
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, bounds.size.h/2 - 21, bounds.size.w/2, 50));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorOrange);
  text_layer_set_text(s_time_layer, "00:");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  //text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
  
  //MINUTES
  s_time_m_layer = text_layer_create(GRect(bounds.size.w/2, bounds.size.h/2 - 21, bounds.size.w/2, 50));
  text_layer_set_background_color(s_time_m_layer, GColorBlack);
  text_layer_set_text_color(s_time_m_layer, GColorOrange);
  text_layer_set_text(s_time_m_layer, "00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_m_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_m_layer, GTextAlignmentLeft);
  
  
  
  // DATE
  
  s_date_layer = text_layer_create(GRect(20, 140, bounds.size.w, 50));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorOrange);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  s_date_bold_layer = text_layer_create(GRect(20+62, 140, 20, 50));
  text_layer_set_background_color(s_date_bold_layer, GColorBlack);
  text_layer_set_text_color(s_date_bold_layer, GColorOrange);
  text_layer_set_font(s_date_bold_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_date_bold_layer, GTextAlignmentCenter);

  
  // BT
  s_connection_layer = text_layer_create(GRect(0, 140, 20, 50));
  text_layer_set_background_color(s_connection_layer, GColorBlack);
  text_layer_set_text_color(s_connection_layer, GColorOrange);
  text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
  
  // BITMAP
  s_canyon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_LOGO_CANYON_140x20);
  s_bitmap_layer = bitmap_layer_create(GRect(2, 20, 140, 20));
  bitmap_layer_set_bitmap(s_bitmap_layer, s_canyon_bitmap);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_m_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_bold_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_m_layer);
  text_layer_destroy(s_date_layer);
  gbitmap_destroy(s_canyon_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);


  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  bluetooth_connection_service_subscribe(handle_bluetooth);
  

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
