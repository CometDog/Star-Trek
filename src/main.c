#include "pebble.h"
#ifdef PBL_COLOR // Only use this for color
  #include "gcolor_definitions.h" // Allows the use of colors
#endif

static Window *s_main_window; // Main window
static Layer *s_solid_bg_layer, *s_date_layer; // Solid color background and date layer
static BitmapLayer *s_background_layer; // Create face bitmap layer
static GBitmap *s_background_bitmap; // Create bitmap of face
static TextLayer *s_time_layer, *s_day_label, *s_num_label, *s_star_label; // Create time layer and all the labels for it
static char s_day_buffer[] = "XXX"; // Create the buffer for the day
static char s_num_buffer[] ="00"; // Create the buffer for the date number
static char s_star_buffer[] = "0000.00"; // Create the buffer for the star date
static char s_time_buffer[] = "00:00"; // Create a buffer for the time
static GFont s_time_font, s_date_font, s_star_font; // Create fonts

// Makes text uppercase when called
char *upcase(char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 0x20;
        }
    }

    return str;
}

// Update background when called
static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack); // Create fill color
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone); // Apply that color
}

// Update the time when called
static void update_time() {
  time_t temp = time(NULL); // Get the time
  struct tm *tick_time = localtime(&temp); // Create the time structure

  if(clock_is_24h_style() == true) {
    strftime(s_time_buffer, sizeof("00:00"), "%I:%M", tick_time); // Write time in 24 hour format into buffer
  } else {
    strftime(s_time_buffer, sizeof("00:00"), "%I:%M", tick_time); // Write time in 12 hour format into buffer
  }
  text_layer_set_text(s_time_layer, s_time_buffer); // Apply time to time layer
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL); // Get the time
  struct tm *t = localtime(&now); // Create the time structure

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t); // Write day to buffer
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t); // Write date number to buffer
  strftime(s_star_buffer, sizeof(s_star_buffer), "%y%m.%d", t); // Write star date to buffer
  
  upcase(s_day_buffer); // Make the day uppercase
  
  text_layer_set_text(s_day_label, s_day_buffer); // Apply to day label
  text_layer_set_text(s_num_label, s_num_buffer); // Apply to date number label
  text_layer_set_text(s_star_label, s_star_buffer); // Apply to star date label
}

// Used to call the update time function
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(); // Updates time
}

// Loads the layers onto the main window
static void window_load(Window *s_main_window) {
  Layer *window_layer = window_get_root_layer(s_main_window); // Creates the main layer for the whole screen
  GRect bounds = layer_get_bounds(window_layer); // Set the bounds to the full size of the screen
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_BOLD_85)); // Time font
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_16)); // Date font
  s_star_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OKUDA_BOLD_25)); // Star font
  
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND); // Set the bitmap resource

  s_solid_bg_layer = layer_create(bounds); // Create the solid color background layer
  s_background_layer = bitmap_layer_create(bounds); // Create the face layer
  s_date_layer = layer_create(bounds); // Create the date layer
  s_time_layer = text_layer_create(GRect(10, 10, 130, 95)); // Create the time layer
  s_day_label = text_layer_create(GRect(90, -5, 20, 25)); // Create the day label
  s_num_label = text_layer_create(GRect(108, -5, 13, 25)); // Create the date number label
  s_star_label = text_layer_create(GRect(21, 90, 119, 95)); // Create the star date label
  
  text_layer_set_text(s_time_layer, s_time_buffer); // Set the time layer to time buffer
  text_layer_set_text(s_day_label, s_day_buffer); // Set day label to day buffer
  text_layer_set_text(s_num_label, s_num_buffer); // Set date number label to date number buffer
  text_layer_set_text(s_star_label, s_star_buffer); // Set star date label to star date buffer
  
  layer_set_update_proc(s_solid_bg_layer, bg_update_proc); // Update the solid color background
  layer_set_update_proc(s_date_layer, date_update_proc); // Update the date layer
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap); // Set the face bitmap to the background layer
  text_layer_set_background_color(s_time_layer, GColorClear); // Make the time layer's background transparent
  text_layer_set_background_color(s_day_label, GColorClear); // Set the day label background to transparent
  text_layer_set_background_color(s_num_label, GColorClear); // Set the date number label background to transparent
  text_layer_set_background_color(s_star_label, GColorClear); // Set the star date background to transparent
  
  #if PBL_PLATFORM_BASALT // Only set this for 3.0 +
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet); // Set the face layer to be a transparent png image
  #endif
    
  #ifdef PBL_COLOR
    text_layer_set_text_color(s_time_layer, GColorBabyBlueEyes); // Set color for time
    text_layer_set_text_color(s_day_label, GColorBabyBlueEyes); // Set color for day
    text_layer_set_text_color(s_num_label, GColorBabyBlueEyes); // Set color for date number
    text_layer_set_text_color(s_star_label, GColorBabyBlueEyes); // Set color for star date
  #else
    text_layer_set_text_color(s_time_layer, GColorWhite); // Set black & white for time
    text_layer_set_text_color(s_day_label, GColorWhite); // Set black & white for day
    text_layer_set_text_color(s_num_label, GColorWhite); // Set black & white for date number
    text_layer_set_text_color(s_star_label, GColorWhite); // Set black & white for star date
  #endif
  
  text_layer_set_font(s_time_layer, s_time_font); // Set time font
  text_layer_set_font(s_day_label, s_date_font); // Set day font
  text_layer_set_font(s_num_label, s_date_font); // Set date number font
  text_layer_set_font(s_star_label, s_star_font); // Set star date font
  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight); // Align time text
  text_layer_set_text_alignment(s_star_label, GTextAlignmentRight); // Align star date text
  
  layer_add_child(window_layer, s_solid_bg_layer); // Add solid color background to main layer
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer)); // Add face background to main layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer)); // Add time layer to main layer
  layer_add_child(window_layer, s_date_layer); // Add date layer to main layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label)); // Add day label to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label)); // Add date number label to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_star_label)); // Add star date label to date layer
  
  update_time(); // Update time immediately to avoid blank time
}

// Unload the layers from the main window
static void window_unload(Window *s_main_window) {
  layer_destroy(s_solid_bg_layer); // Destroy the background color
  layer_destroy(s_date_layer); // Destroy date layer
  gbitmap_destroy(s_background_bitmap); // Destroy face bitmap
  bitmap_layer_destroy(s_background_layer); // Destroy face layer
  
  fonts_unload_custom_font(s_time_font); // Unload time font
  fonts_unload_custom_font(s_date_font); // Unload date font
  fonts_unload_custom_font(s_star_font); // Unload star date font
  
  text_layer_destroy(s_time_layer); // Destroy time layer 
  text_layer_destroy(s_day_label); // Destroy day label
  text_layer_destroy(s_num_label); // Destroy date number label
  text_layer_destroy(s_star_label); // Destroy star date label
}

// Initialize the main window
static void init() {
  s_main_window = window_create(); // Create main window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load, // Allow window_load to manage window
    .unload = window_unload, // Allow window_unload to manage window
  });
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); // Update time every minute
  
  s_day_buffer[0] = '\0'; // Reset day buffer
  s_num_buffer[0] = '\0'; // Reset date number buffer
  s_star_buffer[0] = '\0'; // Reset star date buffer
  
  window_stack_push(s_main_window, true); // Show window. Animated = true

}

// Deinitialize the main window
static void deinit() {
  tick_timer_service_unsubscribe(); // Unsubscribe from the tick timer
  
  window_destroy(s_main_window); // Destroy the main window
}

int main() {
  init();
  app_event_loop();
  deinit();
}