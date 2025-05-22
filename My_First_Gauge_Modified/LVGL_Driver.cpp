/*****************************************************************************
  | File        :   LVGL_Driver.cpp
  | Description : Ported to LVGL 9
******************************************************************************/

#include "LVGL_Driver.h"

static lv_display_t *disp = NULL;

static lv_color_t  *buf1 = NULL;
static lv_color_t  *buf2 = NULL;

/* Serial debugging */
void Lvgl_print(const char *buf) {
    // Serial.printf(buf);
    // Serial.flush();
}

/* Display flushing */
void Lvgl_Display_LCD(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map) {
    LCD_addWindow(area->x1, area->y1, area->x2, area->y2, px_map);
    lv_display_flush_ready(disp_drv);
}

/* Read the touchpad */
void Lvgl_Touchpad_Read(lv_indev_t *indev_drv, lv_indev_data_t *data) {
    Touch_Read_Data();

    if (touch_data.points != 0x00) {
        data->point.x = touch_data.x;
        data->point.y = touch_data.y;
        data->state = LV_INDEV_STATE_PRESSED;
        printf("LVGL : X=%u Y=%u points=%d\r\n", touch_data.x, touch_data.y, touch_data.points);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    if (touch_data.gesture != NONE) {
        // Optional: handle gestures here
    }

    touch_data.x = 0;
    touch_data.y = 0;
    touch_data.points = 0;
    touch_data.gesture = NONE;
}

/* LVGL tick increment callback */
void example_increase_lvgl_tick(void *arg) {
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void Lvgl_Init(void) {
    lv_init();

    // Allocate buffers (you already use heap_caps_malloc for SPIRAM)
    buf1 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_LEN, MALLOC_CAP_SPIRAM);
    buf2 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_LEN, MALLOC_CAP_SPIRAM);

 
    // Create and configure display
    disp = lv_display_create(ESP_PANEL_LCD_WIDTH, ESP_PANEL_LCD_HEIGHT);
    lv_display_set_flush_cb(disp, Lvgl_Display_LCD);
    /* Initialize the draw buffer */
    lv_display_set_buffers(disp, buf1, buf2,LVGL_BUF_LEN , LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Create and configure input
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, Lvgl_Touchpad_Read);

    // Create label
    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello Arduino and LVGL!");
    lv_obj_set_align(label, LV_ALIGN_CENTER);

    // Setup periodic tick timer
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);
}

void Lvgl_Loop(void) {
    lv_timer_handler(); // Let the GUI do its work
}
