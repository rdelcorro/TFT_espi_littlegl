#pragma once

#include <lvgl.h>
#include "resource-id.h"
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include "ui_types.h"

static lv_obj_t* full_keyboard;


class UI {

public:

	UI();

	UI_Object_C* find_object_by_id(ui_object_id id);

	ui_object_value get_value(UI_Object_C* ui_obj);

	void set_value(ui_object_id id, ui_object_value* value);
	void set_value(ui_object_id id, bool value);
	void set_value(ui_object_id id, int value);
	void set_value(ui_object_id id, char* value);
	void set_value(ui_object_id id, calendar_value_type type, int day = 0, int month = 0, int year = 0);
	void set_name(ui_object_id id, const char* name);
	bool toggle_value(ui_object_id id);

	void set_size(ui_object_id id, lv_coord_t width, lv_coord_t height = 0);
	void set_size(ui_object_id id, ui_object_id ref_id);
	void set_style(ui_object_id id, lv_style_t* lv_style, int style_type = 0);
	void set_align(ui_object_id id, lv_align_t align, ui_object_id base_id, int x_offset = 0, int y_offset = 0);
	void set_floating(ui_object_id id, ui_object_id parent_id);

	void spinbox_increment(ui_object_id id, bool increment);

	std::string get_type_name(ui_object_id id);

	UI_Object_C* add_page(ui_object_id parent_id, ui_object_id id, uint32_t width = LV_HOR_RES, uint32_t height = LV_VER_RES);
	UI_Object_C* add_container(ui_object_id parent_id, ui_object_id id, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset, uint32_t width = LV_HOR_RES, uint32_t height = LV_VER_RES);
	UI_Object_C* add_textarea(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, ui_keyboard_type keyboard_type = UI_KEYB_NONE, bool oneline = false, bool pw_mode = false, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_label(ui_object_id parent_id, ui_object_id id, const char* text, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	void label_add_text(ui_object_id parent_id, const char* text_in, uint32_t max_length);

	UI_Object_C* add_switch(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, bool state = false, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_button(ui_object_id parent_id, ui_object_id id, ui_object_id label_id, const char* label, const char* name = nullptr, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_checkbox(ui_object_id parent_id, ui_object_id id, const char* label, const char* name = nullptr, bool state = false, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_led(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, bool state = false, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_bar(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, int value = 0, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);


	UI_Object_C* add_calendar(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);

	UI_Object_C* add_tabview(ui_object_id parent_id, ui_object_id id, ui_tab_item_t* tabs, uint32_t num_tabs, uint32_t width = LV_HOR_RES, uint32_t height = LV_VER_RES);
	UI_Object_C* add_tab(ui_object_id parent_id, ui_object_id id, const char* text);

	UI_Object_C* add_list(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, ui_list_item_t* items = nullptr, uint32_t num_items = 0, ui_object_handler_cb_ptr callback = nullptr,
		lv_align_t align = 255, ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_dropdown(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, ui_dropdown_specs* specs = nullptr, ui_object_handler_cb_ptr callback = nullptr,
		lv_align_t align = 255, ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_roller(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, ui_roller_specs* items = nullptr, ui_object_handler_cb_ptr callback = nullptr,
		lv_align_t align = 255, ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);

	UI_Object_C* add_slider(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, int value = 0, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);
	UI_Object_C* add_spinbox(ui_object_id parent_id, ui_object_id id, const char* name = nullptr, ui_spinbox_specs* value = nullptr, ui_object_handler_cb_ptr callback = nullptr, lv_align_t align = 255,
		ui_object_id base_id = ID_NONE, uint32_t x_offset = 0, uint32_t y_offset = 0);

	ui_list_item_t* object_id_in_list(ui_list_item_t* list, uint16_t num_items, UI_Object_C* ui_object);

	bool add_update_object(ui_object_id id, uint32_t ms_time);
	void do_update_objects(lv_task_t* task);

protected:
	friend class UI_Button;
	static std::vector<UI_Object_C*> ui_object_list_;
	static std::vector<ui_update_object_c*> ui_update_object_list_;

	lv_obj_t* find_lv_object_by_id(ui_object_id id);
	lv_obj_t* find_parent_object(ui_object_id id);
	static UI_Object_C* find_object_by_ref(lv_obj_t* ref);

	UI_Object_C* add_tab(lv_obj_t* parent, ui_object_id id, ui_object_id parent_id, const char* text);

	UI_Object_C* add_object(lv_obj_t* lv_obj, ui_object_id id, ui_object_id parent_id, ui_object_type object_type, ui_object_handler_cb_ptr callback = nullptr, const char* name = nullptr);
	//	bool delete_ui_object(static ui_object_t* ui_kb);

	static void event_handler(lv_obj_t* lv_obj, lv_event_t lv_event);
	static void text_area_event_handler(lv_obj_t* text_area, lv_event_t event);
	static void keyboard_event_cb(lv_obj_t* keyboard, lv_event_t event);
	static void kb_hide_anim_end(lv_anim_t* a);

	void set_size(UI_Object_C* ui_obj, lv_coord_t lv_width, lv_coord_t lv_height);
};