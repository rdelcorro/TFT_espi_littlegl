/*
  Sketch to generate the setup() calibration values, these are reported
  to the Serial Monitor.

  The sketch has been tested on the ESP8266 and screen with XPT2046 driver.
*/

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
#include "lvgl.h"
#include "esp_freertos_hooks.h"
#include <gui/ui.h>
#include <gui/ui_theme.h>

static lv_disp_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
lv_disp_drv_t disp_drv; 


TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
void touch_calibrate();

static UI ui;

//------------------------------------------------------------------------------------------
static int c = 0;
void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
   
   
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x, y;
    //color_p->ch.blue = (2^16) - 1;
    //color_p->ch.green = 0;
    //color_p->ch.red = 0;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            tft.drawPixel(x, y, tft.color565(color_p->ch.red, color_p->ch.green, color_p->ch.blue));
            color_p ++;
            //Serial.printf("Pixel pos: %d, %d\n", x, y); 
        }
    }
    //Serial.printf("Pixel color: r:%d, g:%d, b:%d\n", color_p->ch.red, color_p->ch.green, color_p->ch.blue);
    //Serial.printf("count: %d", c);

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*
void my_rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area)
{
  // Update the areas as needed. Can be only larger.
  // For example to always have lines 8 px height:
   area->y1 = area->y1 & 0x07;
   area->y2 = (area->y2 & 0x07) + 8;
}

void my_set_px_cb(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
    // Write to the buffer as required for the display.
    //  Write only 1-bit for monochrome displays mapped vertically:
    buf += buf_w * (y >> 3) + x;
    if(lv_color_brightness(color) > 128) (*buf) |= (1 << (y % 8));
    else (*buf) &= ~(1 << (y % 8));
}
*/

void my_monitor_cb(lv_disp_drv_t * disp_drv, uint32_t time, uint32_t px)
{

}

static void IRAM_ATTR lv_tick_task(void)
{
  lv_tick_inc(portTICK_RATE_MS);
}


void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{
  /*Send the logs via serial port*/
  if(level == LV_LOG_LEVEL_ERROR) Serial.printf("ERROR: ");
  if(level == LV_LOG_LEVEL_WARN)  Serial.printf("WARNING: ");
  if(level == LV_LOG_LEVEL_INFO)  Serial.printf("INFO: ");
  if(level == LV_LOG_LEVEL_TRACE) Serial.printf("TRACE: ");

  Serial.printf("File: ");
  Serial.printf(file);

  char line_str[8];
  sprintf(line_str,"%d", line);
  Serial.printf("#");
  Serial.printf(line_str);

  Serial.printf(": ");
  Serial.printf(dsc);
  Serial.printf("\n");
}

void lv_tutorial_objects(void)
{

  static lv_style_t my_red_style;
  lv_style_copy(&my_red_style, &lv_style_plain);
  my_red_style.body.main_color = LV_COLOR_RED;
  my_red_style.text.color = LV_COLOR_RED;

  //create Label
lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
lv_obj_set_style(label, &my_red_style); 
lv_label_set_text(label, "Hello world!");  

//lv_obj_set_pos(label, 450, 305);
   
}

static lv_obj_t * btn_enable;           /*An enable button*/
static lv_style_t style_mbox_bg;        /*Black bg. style with opacity*/
static lv_group_t * g;                  /*An Object Group*/
static lv_indev_t * emulated_kp_indev;  /*The input device of the emulated keypad*/
static lv_indev_state_t last_state = LV_INDEV_STATE_REL;
static uint32_t last_key = 0;

static void gui_create(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/

    /*Create a drop down list*/
    lv_obj_t * ddlist = lv_ddlist_create(scr, NULL);
    lv_ddlist_set_options(ddlist, "Low\nMedium\nHigh");
    lv_obj_set_pos(ddlist, LV_DPI / 4, LV_DPI / 4);
    lv_group_add_obj(g, ddlist);                    /*Add the object to the group*/

    /*Create a holder and check boxes on it*/
    lv_obj_t * holder = lv_cont_create(scr, NULL);   /*Create a transparent holder*/
    lv_cont_set_fit(holder, LV_FIT_TIGHT);

    lv_cont_set_layout(holder, LV_LAYOUT_COL_L);
    lv_obj_set_style(holder, &lv_style_transp);
    lv_obj_align(holder, ddlist, LV_ALIGN_OUT_RIGHT_TOP, LV_DPI / 4, 0);

    lv_obj_t * cb = lv_cb_create(holder, NULL);     /*First check box*/
    lv_cb_set_text(cb, "Red");
    lv_group_add_obj(g, cb);                        /*Add to the group*/

    cb = lv_cb_create(holder, cb);                  /*Copy the first check box. Automatically added to the same group*/
    lv_cb_set_text(cb, "Green");

    cb = lv_cb_create(holder, cb);                  /*Copy the second check box. Automatically added to the same group*/
    lv_cb_set_text(cb, "Blue");

    /*Create a sliders*/
    lv_obj_t * slider = lv_slider_create(scr, NULL);
    lv_obj_set_size(slider, LV_DPI, LV_DPI / 3);
    lv_obj_align(slider, holder, LV_ALIGN_OUT_RIGHT_TOP, LV_DPI / 4, 0);
    lv_bar_set_range(slider, 0, 20);
    lv_group_add_obj(g, slider);                    /*Add to the group*/

    /*Create a button*/
    btn_enable = lv_btn_create(scr, NULL);
    //lv_obj_set_event_cb(btn_enable, message_btn_event_cb);
    lv_btn_set_fit(btn_enable, LV_FIT_TIGHT);
    lv_group_add_obj(g, btn_enable);                /*Add to the group*/
    lv_obj_t * l = lv_label_create(btn_enable, NULL);
    lv_label_set_text(l, "Message");
    lv_obj_align(btn_enable, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 2);

    /* Create a dark plain style for a message box's background (modal)*/
    lv_style_copy(&style_mbox_bg, &lv_style_plain);
    style_mbox_bg.body.main_color = LV_COLOR_BLACK;
    style_mbox_bg.body.grad_color = LV_COLOR_BLACK;
    style_mbox_bg.body.opa = LV_OPA_50;
}



void lvTaskHandler(void*);
void testingCPU(void*);

bool input_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    uint16_t x, y;
    bool valid = tft.getTouch(&x, &y);
    data->point.x = x;
    data->point.y = y;
    data->state = valid == false ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
    if (valid == true) {
      //Serial.printf("Touch detected at x:%d y:%d\n", x, y);
    }

    return false;
}

//////// DEMO GUI

/**********************
 *      STYLES
 **********************/
static lv_style_t style_terminal;
static lv_style_t red_led;
static lv_style_t green_led;
static lv_style_t yellow_led;
static lv_style_t blue_led;
static lv_style_t panel_bg;

static ui_tab_item_t tabview_items[] = {
    {ID_TAB1, "Woonkamer"},
    {ID_TAB2, "MeterKast"},
    {ID_TAB3, "TrapKast"},
    {ID_TAB4, "Input"},
    {ID_TAB_TERMINAL, LV_SYMBOL_DOWNLOAD},
};
static const int NUM_TAB_ITEMS = 5;

void ui_handler(UI_Object_C* ui_obj, ui_event_t event) {
	auto value = ui.get_value(ui_obj);
  	if (event == ui_event_t::VALUE_CHANGED || event == ui_event_t::APPLY)
		//publish(ui_obj);
	switch (ui_obj->id) {
	case ID_BUTTON_CLEAR: {
		ui.set_value(ID_TERMINAL, "");
		break;
	}
	case ID_TEXTFIELD_SEARCH: {
		if (event == ui_event_t::APPLY) {
			//ui.label_add_text(ID_TERMINAL, value.char_value, TERMINAL_LOG_LENGTH);
		}
		break;
	}
	}
}

void create_gui() {
	ui.add_tabview(ID_NONE, ID_TAB_VIEW, tabview_items, NUM_TAB_ITEMS, LV_HOR_RES, LV_VER_RES);

	ui.add_label(ID_TAB1, ID_LABEL_LEDS, "leds achter de bank", LV_ALIGN_IN_TOP_LEFT, ID_TAB1, 5, 15);
	ui.add_switch(ID_TAB1, ID_SWITCH_LEDS, "leds", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LABEL_LEDS, 10, 10);

	ui.add_label(ID_TAB1, ID_LABEL_GANG, "kleur in gang", LV_ALIGN_OUT_BOTTOM_LEFT, ID_SWITCH_LEDS, -10, 15);
	ui.add_switch(ID_TAB1, ID_SWITCH_GANG, "gang", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LABEL_GANG, 10, 10);

  ui.add_label(ID_TAB1, ID_LABEL_BLAMP, "groene lampje", LV_ALIGN_OUT_BOTTOM_LEFT, ID_SWITCH_GANG, -10, 15);
	ui.add_switch(ID_TAB1, ID_SWITCH_BLAMP, "blamp", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LABEL_BLAMP, 10, 10);

	ui_object_t* slider = ui.add_slider(ID_TAB1, ID_SLIDER_LEDS, "ledvalue", 0, ui_handler, LV_ALIGN_OUT_RIGHT_MID, ID_LABEL_LEDS, 40, 0);
	ui.add_label(ID_TAB1, ID_LABEL_TIME, "10:00", LV_ALIGN_OUT_BOTTOM_LEFT, ID_SLIDER_LEDS, 5, 10);

  lv_slider_set_range(slider->lv_obj, 0, 255);


	ui.add_label(ID_TAB2, ID_LABEL_HOEDENPLANK, "hoedenplank", LV_ALIGN_IN_TOP_LEFT, ID_TAB2, 5, 15);
	slider = ui.add_slider(ID_TAB2, ID_SLIDER_HOEDENPLANK, "hoedenplank", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LABEL_HOEDENPLANK, 10, 10);
  lv_slider_set_range(slider->lv_obj, 0, 255);
  
	ui.add_label(ID_TAB2, ID_LABEL_GANGLAMP, "gang lampje", LV_ALIGN_OUT_BOTTOM_LEFT, ID_SLIDER_HOEDENPLANK, -10, 15);
	ui.add_switch(ID_TAB2, ID_SWITCH_GANGLAMP, "ganglamp", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LABEL_GANGLAMP, 10, 10);

  ui.add_label(ID_TAB2, ID_LABEL_SCHUUR, "schuur", LV_ALIGN_OUT_BOTTOM_LEFT, ID_SWITCH_GANGLAMP, -10, 15);
	ui.add_switch(ID_TAB2, ID_SWITCH_SCHUUR, "schuur", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LABEL_SCHUUR, 10, 10);

	// ui.add_roller(ID_TAB1, ID_ROLLER, "roller", roller_specs, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_SWITCH_GANG, 0, 60);
	// ui.add_dropdown(ID_TAB1, ID_DROPDOWN, "dropdown", dropdown_spec, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_SWITCH_GANG, 0, 10);
	// ui.set_size(ID_DROPDOWN, 130, 0);
	// ui.set_size(ID_ROLLER, ID_DROPDOWN);

	// ui.add_list(ID_TAB2, ID_LIST_SYMBOLS, "list1_", list_items, NUM_LIST_ITEMS, list_handler, LV_ALIGN_IN_TOP_LEFT, ID_TAB2, 5, 5);
	// ui.set_size(ID_LIST_SYMBOLS, 150, 200);
	// ui.add_list(ID_TAB2, ID_LIST2_SYMBOLS2, "list2_", list_items2, NUM_LIST_ITEMS2, list_handler, LV_ALIGN_OUT_RIGHT_TOP, ID_LIST_SYMBOLS, 10, 0);
	// ui.set_size(ID_LIST2_SYMBOLS2, 150, 270);
	// ui.add_led(ID_TAB2, ID_LED_EXTRA2, "led_extra2", false, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LIST_SYMBOLS, 10, 10);
	// ui.set_size(ID_LED_EXTRA2, 20, 20);
	// ui.add_led(ID_TAB2, ID_LED_EXTRA3, "led_extra3", false, LV_ALIGN_OUT_RIGHT_TOP, ID_LED_EXTRA2, 20, 0);
	// ui.set_size(ID_LED_EXTRA3, ID_LED_EXTRA2);
	// ui.set_style(ID_LED_EXTRA3, &yellow_led);
	// ui.add_led(ID_TAB2, ID_LED_EXTRA4, "led_extra4", false, LV_ALIGN_OUT_RIGHT_TOP, ID_LED_EXTRA3, 20, 0);
	// ui.set_size(ID_LED_EXTRA4, ID_LED_EXTRA2);
	// ui.set_style(ID_LED_EXTRA4, &blue_led);

	// ui.add_led(ID_TAB2, ID_LED_EXTRA5, "led_extra5", false, LV_ALIGN_OUT_RIGHT_TOP, ID_LED_EXTRA4, 20, 0);
	// ui.set_size(ID_LED_EXTRA5, ID_LED_EXTRA2);
	// ui.set_style(ID_LED_EXTRA5, &green_led);

	ui.add_label(ID_TAB_TERMINAL, ID_TERMINAL, "BYE");
	ui.set_style(ID_TERMINAL, &style_terminal, 0);
	ui.add_button(ID_TAB_TERMINAL, ID_BUTTON_CLEAR, ID_BUTTON_CLEAR_LABEL, "Clear", "clearbutton", ui_handler, LV_ALIGN_IN_BOTTOM_RIGHT, ID_TAB_TERMINAL, -10, 10);
	ui.set_floating(ID_BUTTON_CLEAR, ID_TAB_TERMINAL);
	ui.set_size(ID_BUTTON_CLEAR, 100, 40);
	ui.add_textarea(ID_TAB_TERMINAL, ID_TEXTFIELD_SEARCH, "search", UI_KEYB_TEXT, true, false, ui_handler, LV_ALIGN_IN_TOP_RIGHT, ID_TAB_TERMINAL, 0, 0);
	ui.set_floating(ID_TEXTFIELD_SEARCH, ID_TAB_TERMINAL);

	// ui.add_checkbox(ID_TAB3, ID_CB_SCREENSAVER, "Screensaver", "screensaver", false, ui_handler, LV_ALIGN_IN_TOP_LEFT, ID_TAB3, 5, 10);
	// ui.add_checkbox(ID_TAB3, ID_CB_BACKLIGHT, "Backlight", "backlight", false, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_CB_SCREENSAVER, 0, 0);

	//add_led_panel(ID_TAB3, ID_LED_PANEL, ID_CB_BACKLIGHT);

	// ui.add_bar(ID_TAB3, ID_BAR_VALUE, "bar", 0, LV_ALIGN_OUT_BOTTOM_LEFT, ID_LED_PANEL, 0, 20);
	// ui.set_size(ID_BAR_VALUE, 200, 20);
	//ui.set_style(ID_BAR_VALUE, &panel_bg);

	//CALENDAR
	//ui.add_calendar(ID_TAB3, ID_CALENDAR, "calendar", ui_handler, LV_ALIGN_OUT_RIGHT_TOP, ID_CB_SCREENSAVER, 100, 5);
	//ui.set_value(ID_CALENDAR, TODAY_DATE);
	//ui.set_value(ID_CALENDAR, HIGHLIGHT_DATES, 29, 11, 2019);
	//ui.set_value(ID_CALENDAR, HIGHLIGHT_DATES, 2, 12, 2019);

	//	uint16_t party[] = { 18,11,2019 };	uint16_t church[] = { 21,11,2019 };	uint16_t kingsday[] = { 22,11,2019 };
//	ui.set_value(ID_CALENDAR, 4, HIGHLIGHT_DATES, party, church, kingsday);
//	ui.add_label(ID_TAB3, ID_LABEL_DATE, "date label", LV_ALIGN_OUT_BOTTOM_LEFT, ID_CALENDAR, 10, 10);

	//SPINBOX
	// ui_spinbox_specs* spinbox_def = new ui_spinbox_specs(3, 1, 2, 0, 100, 0, 1); // digit_count, fract, left_padding,min,max,value,step
	// ui.add_spinbox(ID_TAB3, ID_SPINBOX, "spinbox", spinbox_def, ui_handler, LV_ALIGN_OUT_BOTTOM_LEFT, ID_BAR_VALUE, 0, 20);

	// ui.add_textarea(ID_TAB4, ID_TEXTAREA_INPUT, "input", UI_KEYB_TEXT, false, false, ui_handler, LV_ALIGN_IN_TOP_LEFT, ID_TAB4, 0, 0);
	// ui.set_size(ID_TEXTAREA_INPUT, ID_TAB4);

}

void setup_theme() {

  lv_theme_t *th = ui_theme_init( 138, &lv_font_roboto_16); // Set a HUE value and a Font for the Night Theme

  static lv_style_t style_tv_btn_bg;
  lv_style_copy(&style_tv_btn_bg, th->style.tabview.btn.bg);
  style_tv_btn_bg.body.padding.top = 0;
  style_tv_btn_bg.body.padding.bottom = 0;
  style_tv_btn_bg.body.padding.left = 0;

  static lv_style_t style_tv_btn_rel;
  lv_style_copy(&style_tv_btn_rel, th->style.tabview.btn.rel);
  style_tv_btn_rel.body.radius = 0;
  style_tv_btn_rel.body.opa = LV_OPA_50;
  style_tv_btn_rel.body.border.width = 0;

  static lv_style_t style_tv_btn_pr;
  lv_style_copy(&style_tv_btn_pr, th->style.tabview.btn.pr);
  style_tv_btn_pr.body.radius = 0;
  style_tv_btn_pr.body.opa = LV_OPA_50;
  style_tv_btn_pr.body.border.width = 0;

  static lv_style_t style_tv_indic;
  lv_style_copy(&style_tv_indic, &lv_style_plain_color);
  style_tv_indic.body.main_color = lv_color_hex(0xffffff);
  style_tv_indic.body.grad_color = lv_color_hex(0xffffff);

  th->style.tabview.indic = &style_tv_indic;
  th->style.tabview.btn.tgl_rel = &style_tv_btn_pr;
  th->style.tabview.btn.tgl_pr = &style_tv_btn_pr;
  th->style.tabview.btn.bg = &style_tv_btn_bg;
  th->style.tabview.btn.pr = &style_tv_btn_pr;
  th->style.tabview.btn.rel = &style_tv_btn_rel;

  // TERMINAL
  lv_style_copy(&style_terminal, &lv_style_plain_color);
  style_terminal.text.color = lv_color_hex(0xffffff);
  style_terminal.text.font = &lv_font_roboto_16;

  // LEDS
  lv_style_copy(&red_led, th->style.led);
  red_led.body.main_color = lv_color_hsv_to_rgb(0, 100, 100);
  red_led.body.grad_color = lv_color_hsv_to_rgb(0, 100, 40);
  red_led.body.border.color = lv_color_hsv_to_rgb(0, 60, 60);
  red_led.body.shadow.color = lv_color_hsv_to_rgb(0, 100, 100);

  lv_style_copy(&green_led, th->style.led);
  green_led.body.main_color = lv_color_hsv_to_rgb(96, 100, 100);
  green_led.body.grad_color = lv_color_hsv_to_rgb(96, 100, 40);
  green_led.body.border.color = lv_color_hsv_to_rgb(96, 60, 60);
  green_led.body.shadow.color = lv_color_hsv_to_rgb(96, 100, 100);

  lv_style_copy(&yellow_led, th->style.led);
  yellow_led.body.main_color = lv_color_hsv_to_rgb(60, 100, 100);
  yellow_led.body.grad_color = lv_color_hsv_to_rgb(60, 100, 40);
  yellow_led.body.border.color = lv_color_hsv_to_rgb(60, 60, 60);
  yellow_led.body.shadow.color = lv_color_hsv_to_rgb(60, 100, 100);

  lv_style_copy(&blue_led, th->style.led);
  blue_led.body.main_color = lv_color_hsv_to_rgb(240, 100, 100);
  blue_led.body.grad_color = lv_color_hsv_to_rgb(240, 100, 40);
  blue_led.body.border.color = lv_color_hsv_to_rgb(240, 60, 60);
  blue_led.body.shadow.color = lv_color_hsv_to_rgb(240, 100, 100);

  lv_style_copy(&panel_bg, th->style.panel);
  panel_bg.body.main_color = lv_color_hex(0x222222);
  panel_bg.body.grad_color = lv_color_hex(0x555555);
  panel_bg.body.border.width = 3;
  panel_bg.body.border.color = lv_color_hex(0xeeeeee);

  lv_theme_set_current(th);
}


////////


void setup() {
  // Use serial port
  Serial.begin(115200);

  // Let the screen boot
  //delay(2000);

  // Initialise the TFT screen
  tft.init();
  
  // Set the rotation before we calibrate
  tft.setRotation(1);

  // Calibrate the touch screen and retrieve the scaling factors
  //touch_calibrate();

  uint16_t calData[5] = { 260, 3665, 249, 3521, 7 };
  tft.setTouch(calData);

  // Clear the screen
  tft.fillScreen(TFT_GREEN);

  lv_init();

  /*Initialize `disp_buf` with the buffer(s) */
  lv_disp_buf_init(&disp_buf, buf_1, nullptr, LV_HOR_RES_MAX*10);

  lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
  disp_drv.buffer = &disp_buf;            /*Set an initialized buffer*/
  disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
  disp_drv.hor_res = 480;
  disp_drv.ver_res = 320;
  lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

  // Init the touchpad
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);      /*Basic initialization*/
  indev_drv.type =LV_INDEV_TYPE_POINTER;                 /*See below.*/
  indev_drv.read_cb =input_read;              /*See below.*/
  /*Register the driver in LittlevGL and save the created input device object*/
  lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);

  lv_log_register_print_cb(my_log_cb);

  esp_register_freertos_tick_hook(lv_tick_task);

  xTaskCreate(lvTaskHandler,          /* Task function. */
                    "lvtaskhandler",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */


    
    //gui_create();

    setup_theme();
    create_gui();
    
    
}


void lvTaskHandler( void * parameter ) {
  while(true) {
    vTaskDelay(1);
    lv_task_handler();
  }
}

void testingCPU( void * parameter ) {
  while(true) {
    digitalWrite(34, LOW);
    vTaskDelay(1000);
    digitalWrite(34, HIGH);
    vTaskDelay(1000);
  }
}


//------------------------------------------------------------------------------------------

void loop(void) {
  //tft.fillScreen(TFT_BLACK);
  //delay(1000);
  //for (int i=0; i< 300; ++i) {
  //  for (int j=0; j< 300; ++j) {
  //    tft.drawPixel(j, i, tft.color565(255, 0, 0));
  //  }
 //}
  //delay(3000);

  digitalWrite(34, LOW);
    delay(1);
    digitalWrite(34, HIGH);
    delay(1);
}

//------------------------------------------------------------------------------------------

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup():");
  Serial.print("  uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println(); Serial.println();

  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}
