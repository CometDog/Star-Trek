#include "pebble.h"

// Allows the use of colors such as "GColorMidnightGreen"
#ifdef PBL_PLATFORM_BASALT // Only use this for 3.0+
  #include "gcolor_definitions.h"
#endif

// Main window
static Window *s_main_window;

// Background and hand layers
static Layer *s_solid_bg_layer, *s_date_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static TextLayer *s_time_layer, *s_day_label, *s_num_label, *s_star_label;
static char s_day_buffer[] = "XXX";
  static char s_num_buffer[] ="00";
static char s_star_buffer[] = "0000.00";

static GFont s_time_font, s_date_font, s_star_font;

// Update background when called
static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_day_label, s_day_buffer);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
  
  strftime(s_star_buffer, sizeof(s_star_buffer), "%y%m.%d", t);
  text_layer_set_text(s_star_label, s_star_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// Loads the layers onto the main window
static void window_load(Window *s_main_window) {
  
  // Creates window_layer as root and sets its bounds
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the simple single color backgroud behind the face layer. Then apply it to the window layer
  s_solid_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_solid_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_solid_bg_layer);
  
  // Create the face on the background layer above the solid color. Then apply it to the window layer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  #if PBL_PLATFORM_BASALT // Only set this for 3.0 +
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  #endif
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(21, 0, 119, 95));
  text_layer_set_background_color(s_time_layer, GColorClear);
  #ifdef PBL_COLOR
    text_layer_set_text_color(s_time_layer, GColorBabyBlueEyes);
  #else
    text_layer_set_text_color(s_time_layer, GColorWhite);
  #endif
  text_layer_set_text(s_time_layer, "00:00");
  
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_BOLD_90));

  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // Make sure the time is displayed from the start
  update_time();
  
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);
  
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_16));

  s_day_label = text_layer_create(GRect(90, -5, 20, 25));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorClear);
  #ifdef PBL_COLOR
    text_layer_set_text_color(s_day_label, GColorBabyBlueEyes);
  #else
    text_layer_set_text_color(s_day_label, GColorWhite);
  #endif
  text_layer_set_font(s_day_label, s_date_font);

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

  s_num_label = text_layer_create(GRect(108, -5, 13, 25));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorClear);
  #ifdef PBL_COLOR
    text_layer_set_text_color(s_num_label, GColorBabyBlueEyes);
  #else
    text_layer_set_text_color(s_num_label, GColorWhite);
  #endif
  text_layer_set_font(s_num_label, s_date_font);
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));
  
  s_star_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_BOLD_25));
  
  s_star_label = text_layer_create(GRect(21, 90, 119, 95));
  text_layer_set_text(s_star_label, s_star_buffer);
  text_layer_set_background_color(s_star_label, GColorClear);
  #ifdef PBL_COLOR
    text_layer_set_text_color(s_star_label, GColorBabyBlueEyes);
  #else
    text_layer_set_text_color(s_star_label, GColorWhite);
  #endif
  text_layer_set_font(s_star_label, s_star_font);
  text_layer_set_text_alignment(s_star_label, GTextAlignmentRight);
  
  layer_add_child(s_date_layer, text_layer_get_layer(s_star_label));
  
}

// Unload the layers from the main window
static void window_unload(Window *s_main_window) {
  
  // Destroy the background color
  layer_destroy(s_solid_bg_layer);
  
  // Destroy the watch face
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  
    //Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_star_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  layer_destroy(s_date_layer);

  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);
  text_layer_destroy(s_star_label);
}

// Initialize the main window
static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
   // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';
  s_star_buffer[0] = '\0';
  
  window_stack_push(s_main_window, true);

}

// Deinitialize the main window
static void deinit() {

  // Unsubscribe from the tick timer
  tick_timer_service_unsubscribe();
  
  // Destroy the main window
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}