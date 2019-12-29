#include <lvgl.h>
#include <vector>
#include <functional>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
//#include "arduino/time_c.h"

std::vector<UI_Object_C*> UI::ui_object_list_;
std::vector<ui_update_object_c*> UI::ui_update_object_list_;

UI::UI() {
	//lv_theme_t* th = lv_theme_night_init(210, &lv_font_roboto_16);     //Set a HUE value and a Font for the Night Theme
	//lv_theme_set_current(th);                                           //Apply the theme
}

UI_Object_C* UI::add_object(lv_obj_t* object, ui_object_id id, ui_object_id parent_id, ui_object_type object_type, ui_object_handler_cb_ptr callback, const char* name) {
	if (id > 0 && object != nullptr) {
		UI_Object_C* ui_obj = new UI_Object_C;
		ui_obj->id = id;
		ui_obj->parent_id = parent_id;
		ui_obj->type = object_type;
		ui_obj->name = name;
		ui_obj->lv_obj = object;
		ui_obj->ui_callback = callback;
		UI::ui_object_list_.push_back(ui_obj);
		return ui_obj;
	}
	return nullptr;
}

// bool UI::delete_ui_object(ui_object_t* ui_obj) {
// 	int index = 0;
// 	ui_object_id id = ui_obj->id;
// 	for (auto o : UI::ui_object_list_) {
// 		if (o->id == id) {
// 			lv_obj_del(o->lv_obj);
// 			UI::ui_object_list_.erase(UI::ui_object_list_.begin() + index);
// 			return true;
// 		}
// 		index++;
// 	}
// 	return false;
// }

ui_list_item_t* UI::object_id_in_list(ui_list_item_t* list, uint16_t num_items, UI_Object_C* ui_object) {
	for (int i = 0; i < num_items; i++) {
		if (list[i].id == ui_object->id) {
			return &list[i];
		}
	}
	return nullptr;
}

UI_Object_C* UI::add_page(ui_object_id parent_id, ui_object_id id, uint32_t width, uint32_t height) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);

	lv_obj_t* lv_obj = lv_page_create(lv_parent, NULL);

	lv_obj_set_size(lv_obj, width, height);
	lv_page_set_sb_mode(lv_obj, LV_SB_MODE_AUTO);
	lv_obj_align(lv_obj, lv_parent, LV_ALIGN_CENTER, 0, 0);
	lv_page_set_scrl_layout(lv_obj, LV_LAYOUT_COL_L);     /*Arrange elements automatically*/

	return add_object(lv_obj, id, parent_id, ui_object_type::PAGE);
}

UI_Object_C* UI::add_tabview(ui_object_id parent_id, ui_object_id id, ui_tab_item_t* tabs, uint32_t num_tabs, uint32_t width, uint32_t height) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);

	lv_obj_t* lv_obj = lv_tabview_create(lv_parent, NULL);

	if (tabs != nullptr && lv_obj != nullptr) {
		lv_obj_set_size(lv_obj, width, height);

		for (uint32_t i = 0; i < num_tabs; i++) {
			add_tab(lv_obj, tabs[i].id, id, tabs[i].label);
		}
	}
	return add_object(lv_obj, id, parent_id, ui_object_type::TABVIEW);
}

UI_Object_C* UI::add_tab(ui_object_id parent_id, ui_object_id id, const char* text) {
	lv_obj_t* lv_parent = find_lv_object_by_id(parent_id);
	UI_Object_C* ui_obj = add_tab(lv_parent, id, parent_id, text);
	return ui_obj;
}

UI_Object_C* UI::add_tab(lv_obj_t* parent, ui_object_id id, ui_object_id parent_id, const char* text) {
	lv_obj_t* lv_obj = nullptr;
	if (parent != nullptr) {
		lv_obj = lv_tabview_add_tab(parent, text);
		if (lv_obj != nullptr) {
			lv_page_set_sb_mode(lv_obj, LV_SB_MODE_OFF);
		}
	}
	return 	add_object(lv_obj, id, parent_id, ui_object_type::TAB);
}

UI_Object_C* UI::add_container(ui_object_id parent_id, ui_object_id id, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);

	lv_obj_t* lv_obj = lv_cont_create(lv_parent, NULL);
	if (align < 999) {
		lv_obj_t* base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, base, align, x_offset, y_offset);
	}

	lv_obj_set_width(lv_obj, width);
	lv_obj_set_height(lv_obj, height);
	//lv_cont_set_fit2(lv_obj, false, true);
	lv_page_glue_obj(lv_obj, true);
	//lv_obj_set_width(lv_obj, lv_page_get_fit_width(lv_parent));

	lv_cont_set_fit2(lv_parent, false, false);
	return 	add_object(lv_obj, id, parent_id, ui_object_type::CONTAINER);
}

UI_Object_C* UI::add_textarea(ui_object_id parent_id, ui_object_id id, const char* name, ui_keyboard_type keyboard_type, bool oneline, bool pw_mode, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_ta_create(lv_parent, NULL);
	if (align < 999) {
		lv_obj_t* base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, base, align, x_offset, y_offset);
	}

	lv_ta_set_pwd_mode(lv_obj, pw_mode);
	lv_ta_set_one_line(lv_obj, oneline);
	lv_ta_set_text_sel(lv_obj, true);
	if (keyboard_type == UI_KEYB_NONE)
		lv_obj_set_event_cb(lv_obj, event_handler);
	else
		lv_obj_set_event_cb(lv_obj, text_area_event_handler);

	UI_Object_C* ui_obj = add_object(lv_obj, id, parent_id, ui_object_type::TEXTAREA, callback, name);
	ui_keyboard_data* kbd = new ui_keyboard_data();
	kbd->type = keyboard_type;
	ui_obj->object_data = kbd;
	return ui_obj;
}

UI_Object_C* UI::add_label(ui_object_id parent_id, ui_object_id id, const char* text, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_label_create(lv_parent, NULL);

	lv_label_set_text(lv_obj, text);
	lv_label_set_recolor(lv_obj, true);
	if (align < 999) {
		lv_obj_t* base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, base, align, x_offset, y_offset);
	}
	return add_object(lv_obj, id, parent_id, ui_object_type::LABEL);
}

void UI::label_add_text(ui_object_id label_id, const char* txt_in, uint32_t max_length) {
	if (txt_in == nullptr)
		return;
	lv_obj_t* lv_obj = find_lv_object_by_id(label_id);
	if (lv_obj == NULL) {
		printf("ERROR NO LABEL");
		return;                 /*Check if the window is exists*/
	}
	char* txt_log = lv_label_get_text(lv_obj);

	uint16_t txt_len = strlen(txt_in);
	uint16_t old_len = strlen(txt_log);

	//uint32_t old_offset = 0;
	if (txt_len > max_length) {
		txt_len = max_length;
		old_len = 0;
	}
	// else if ((txt_len + old_len) > max_length) {
	// 	uint32_t old_end = max_length - txt_len;
	// }

	//printf("label length = %d\n", old_len);
	char* new_txt = (char*)calloc(txt_len + old_len + 1, sizeof(char));
	if (old_len > 0)
		memcpy(new_txt, txt_log, old_len);
	memcpy(&new_txt[old_len], txt_in, txt_len);
	new_txt[old_len + txt_len] = '\n';
	new_txt[old_len + txt_len + 1] = '\0';

	lv_label_set_text(lv_obj, new_txt);
}

UI_Object_C* UI::add_switch(ui_object_id parent_id, ui_object_id id, const char* name, bool state, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_sw_create(lv_parent, NULL);
	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}
	if (state)
		lv_sw_on(lv_obj, false);
	else
		lv_sw_off(lv_obj, false);
	lv_obj_set_event_cb(lv_obj, UI::event_handler);
	return 	add_object(lv_obj, id, parent_id, ui_object_type::SWITCH, callback, name);
}

UI_Object_C* UI::add_spinbox(ui_object_id parent_id, ui_object_id id, const char* name, ui_spinbox_specs* object_specs, ui_object_handler_cb_ptr callback, lv_align_t align,
	ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_spinbox_create(lv_parent, NULL);

	UI_Object_C* ui_obj = add_object(lv_obj, id, parent_id, ui_object_type::SPINBOX, callback, name);
	object_specs->apply_specs(ui_obj);

	//lv_spinbox_step_prev(spinbox);
	//lv_obj_set_width(lv_obj, 100);
	//lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);
	if (align < 999) {
		lv_obj_t* base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, base, align, x_offset, y_offset);
	}
	lv_obj_set_event_cb(lv_obj, UI::event_handler);
	//ui_obj->object_data = object_specs;
	return 	ui_obj;
}

UI_Object_C* UI::add_button(ui_object_id parent_id, ui_object_id id, ui_object_id label_id, const char* label, const char* name, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_btn_create(parent, NULL);
	lv_obj_t* lv_btn_label = lv_label_create(lv_obj, NULL);
	lv_label_set_text(lv_btn_label, label);
	add_object(lv_btn_label, label_id, parent_id, ui_object_type::LABEL);

	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}
	lv_obj_set_event_cb(lv_obj, UI::event_handler);

	return 	add_object(lv_obj, id, parent_id, ui_object_type::BUTTON, callback, name);
}

UI_Object_C* UI::add_checkbox(ui_object_id parent_id, ui_object_id id, const char* label, const char* name, bool state, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_cb_create(lv_parent, NULL);
	lv_cb_set_text(lv_obj, label);

	if (align < 999) {
		lv_obj_t* base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, base, align, x_offset, y_offset);
	}
	lv_cb_set_checked(lv_obj, state);
	lv_obj_set_event_cb(lv_obj, UI::event_handler);
	return add_object(lv_obj, id, parent_id, ui_object_type::CHECKBOX, callback, name);
}

UI_Object_C* UI::add_led(ui_object_id parent_id, ui_object_id id, const char* name, bool state, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_led_create(lv_parent, NULL);
	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}
	if (state)
		lv_led_on(lv_obj);
	else
		lv_led_off(lv_obj);
	return add_object(lv_obj, id, parent_id, ui_object_type::LED, nullptr, name);
}

UI_Object_C* UI::add_bar(ui_object_id parent_id, ui_object_id id, const char* name, int value, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_bar_create(lv_parent, NULL);
	if (align < 999) {
		lv_obj_t* _lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, _lv_base, align, x_offset, y_offset);
	}

	lv_bar_set_value(lv_obj, value, false);
	return add_object(lv_obj, id, parent_id, ui_object_type::BAR, nullptr, name);
}

UI_Object_C* UI::add_calendar(ui_object_id parent_id, ui_object_id id, const char* name, ui_object_handler_cb_ptr callback, lv_align_t align,
	ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_calendar_create(lv_parent, NULL);

	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}

	lv_obj_set_event_cb(lv_obj, UI::event_handler);

	return add_object(lv_obj, id, parent_id, ui_object_type::CALENDAR, callback, name);
}


void UI::event_handler(lv_obj_t* lv_obj, lv_event_t event) {
	UI_Object_C* ui_obj = find_object_by_ref(lv_obj);
	if (ui_obj->ui_callback != nullptr)
		switch (ui_obj->type) {
		case ui_object_type::BUTTON: {
			if (event == LV_EVENT_PRESSED)
				ui_obj->ui_callback(ui_obj, ui_event_t::CLICKED);
			else if (event == LV_EVENT_RELEASED)
				ui_obj->ui_callback(ui_obj, ui_event_t::RELEASED);
			break; }
		case ui_object_type::SWITCH: 
		case ui_object_type::CHECKBOX:
		case ui_object_type::CALENDAR:
		case ui_object_type::SLIDER:
		case ui_object_type::DROPDOWN_LIST:
		case ui_object_type::ROLLER:
		case ui_object_type::LIST:
		case ui_object_type::SPINBOX: {
			if (event == LV_EVENT_VALUE_CHANGED)
				ui_obj->ui_callback(ui_obj, ui_event_t::VALUE_CHANGED);
			//if (event == LV_EVENT_CLICKED)
			//	ui_obj->ui_callback(ui_obj, ui_event_t::CLICKED);
			break; }
		case ui_object_type::TEXTAREA: {
			if (event == LV_EVENT_CLICKED)
				//show the keyboard
				ui_obj->ui_callback(ui_obj, ui_event_t::CLICKED);
			break; }
		}
}

void UI::text_area_event_handler(lv_obj_t* text_area, lv_event_t event) {
	lv_obj_t* parent = nullptr;
	/*Text area is on the scrollable part of the page but we need the page itself*/
	uint8_t protect = lv_obj_get_protect(text_area);
	UI_Object_C* ui_ta_obj = find_object_by_ref(text_area);
	if (protect && LV_PROTECT_PARENT)
		parent = lv_obj_get_parent(text_area);
	else
		parent = lv_obj_get_parent(lv_obj_get_parent(text_area));

	if (event == LV_EVENT_CLICKED) {
		//chech ik the keyboard is active on another textarea.
		//If so destroy it
		if (full_keyboard != NULL && lv_kb_get_ta(full_keyboard) != text_area) {
			lv_obj_del(full_keyboard);
			full_keyboard = NULL;
		}
		//If there is no keyboard active create one for the textarea in focus
		if (full_keyboard == NULL) {
			full_keyboard = lv_kb_create(parent, NULL);
			//protect the keyboard from scrolling
			lv_obj_set_protect(full_keyboard, LV_PROTECT_PARENT);
			lv_obj_set_parent(full_keyboard, parent);

			ui_keyboard_data* kbd = (ui_keyboard_data*)ui_ta_obj->object_data;
			if (kbd->type == UI_KEYB_NUM) {
				lv_kb_set_mode(full_keyboard, LV_KB_MODE_NUM);
			}
			else if (kbd->type == UI_KEYB_TEXT) {
				lv_kb_set_mode(full_keyboard, LV_KB_MODE_TEXT);
			}
			lv_obj_set_size(full_keyboard, lv_obj_get_width_fit(parent), lv_obj_get_height_fit(parent) / 2);
			lv_obj_align(full_keyboard, parent, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
			lv_kb_set_ta(full_keyboard, text_area);

			lv_obj_set_event_cb(full_keyboard, keyboard_event_cb);

#if LV_USE_ANIMATION
			lv_anim_t a;
			a.var = full_keyboard;
			a.start = LV_VER_RES;
			a.end = lv_obj_get_y(full_keyboard);
			a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
			a.path_cb = lv_anim_path_linear;
			a.ready_cb = NULL;
			a.act_time = 0;
			a.time = 300;
			a.playback = 0;
			a.playback_pause = 0;
			a.repeat = 0;
			a.repeat_pause = 0;
			lv_anim_create(&a);
#endif
		}
	}
	event_handler(text_area, event);
}

void UI::keyboard_event_cb(lv_obj_t* kb, lv_event_t event)
{

	lv_kb_def_event_cb(kb, event);
	if (event == LV_EVENT_APPLY || event == LV_EVENT_CANCEL) {
#if LV_USE_ANIMATION
		lv_anim_t a;
		a.var = full_keyboard;
		a.start = lv_obj_get_y(full_keyboard);
		a.end = LV_VER_RES;
		a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
		a.path_cb = lv_anim_path_linear;
		a.ready_cb = kb_hide_anim_end;
		a.act_time = 0;
		a.time = 300;
		a.playback = 0;
		a.playback_pause = 0;
		a.repeat = 0;
		a.repeat_pause = 0;
		lv_anim_create(&a);
#else
		lv_obj_del(full_keyboard);
		full_keyboard = NULL;
#endif
	}
	if (event == LV_EVENT_APPLY) {
		lv_obj_t* ta = lv_kb_get_ta(kb);
		lv_ta_get_text(ta);
		ui_object_t* ui_obj = find_object_by_ref(ta);
		if (ui_obj != nullptr) {
			if(ui_obj->ui_callback != nullptr)
				ui_obj->ui_callback(ui_obj, ui_event_t::APPLY);
		}
	}
}


#if LV_USE_ANIMATION
void UI::kb_hide_anim_end(lv_anim_t* a)
{
	lv_obj_del(full_keyboard);
	full_keyboard = NULL;
}
#endif

void UI::set_value(ui_object_id id, bool value) {
	ui_object_value obj_value;
	obj_value.bool_value = value;
	set_value(id, &obj_value);
}

void UI::set_value(ui_object_id id, int value) {
	ui_object_value obj_value;
	obj_value.int_value = value;
	set_value(id, &obj_value);
}

void UI::set_value(ui_object_id id, char* value) {
	ui_object_value obj_value;
	obj_value.char_value = value;
	set_value(id, &obj_value);
}

void UI::set_value(ui_object_id id, calendar_value_type type, int day, int month, int year) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	if (ui_obj == nullptr)
		return;
	lv_obj_t* lv_obj = ui_obj->lv_obj;

	if (ui_obj->type == ui_object_type::CALENDAR) {
		lv_calendar_date_t lv_date;
		if (type == TODAY_DATE) {
			if (year == 0) { // if the year is 0 we show todays date
				time_t now;
				struct tm info;
				time(&now);
				localtime_r(&now, &info);
				day = info.tm_mday;
				month = info.tm_mon;
				year = info.tm_year + 1900;
			}
			lv_date.day = day;
			lv_date.month = month;
			lv_date.year = year;
			lv_calendar_set_today_date(lv_obj, &lv_date);
			lv_calendar_set_showed_date(lv_obj, &lv_date);
		}
		else if (type == HIGHLIGHT_DATES) {
			lv_calendar_date_t* lv_dates = lv_calendar_get_highlighted_dates(ui_obj->lv_obj);
			uint16_t num_dates = lv_calendar_get_highlighted_dates_num(ui_obj->lv_obj);
			lv_calendar_date_t* lv_new_dates = new lv_calendar_date_t[num_dates + 1];

			for (int i = 0; i < num_dates; i++) {
				lv_new_dates[i].day = lv_dates[i].day;
				lv_new_dates[i].month = lv_dates[i].month;
				lv_new_dates[i].year = lv_dates[i].year;
			}
			lv_new_dates[num_dates].day = day;
			lv_new_dates[num_dates].month = month;
			lv_new_dates[num_dates].year = year;

			lv_calendar_set_highlighted_dates(lv_obj, lv_new_dates, num_dates + 1);
			if (lv_dates != NULL)
				delete(lv_dates);
		}
	}

}

void UI::set_name(ui_object_id id, const char* name) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	if (ui_obj == nullptr)
		return;
	ui_obj->name = name;
}

void UI::set_value(ui_object_id id, ui_object_value* value) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	if (ui_obj == nullptr)
		return;
	lv_obj_t* lv_obj = ui_obj->lv_obj;

	//num_args = 0;
	switch (ui_obj->type) {
	case ui_object_type::LABEL: {
		lv_label_set_text(lv_obj, (const char*)value->char_value);
		break;
	}
	case ui_object_type::BUTTON: {
		//const char* value = va_arg(arguments, const char*);
		//ui_set_button_text();
		break;
	}
	case ui_object_type::SLIDER: {
		lv_slider_set_value(lv_obj, value->int_value, true);
		break;
	}
	case ui_object_type::BAR: {
		lv_bar_set_value(lv_obj, value->int_value, true);
		break;
	}
	case ui_object_type::SWITCH: {
		if (value->bool_value)
			lv_sw_on(lv_obj, true);
		else
			lv_sw_off(lv_obj, true);
		break;
	}
	case ui_object_type::LED: {
		if (value->bool_value)
			lv_led_on(lv_obj);
		else
			lv_led_off(lv_obj);
		break;
	}
							//case ui_object_type::CALENDAR: {
							//	lv_calendar_date_t lv_date;
							//	calendar_value_type type = va_arg(arguments, calendar_value_type);
							//	if (type == TODAY_DATE) {
							//		lv_date.day = va_arg(arguments, uint8_t);
							//		lv_date.month = va_arg(arguments, uint8_t);
							//		lv_date.year = va_arg(arguments, uint16_t);
							//		lv_calendar_set_today_date(lv_obj, &lv_date);
							//		lv_calendar_set_showed_date(lv_obj, &lv_date);
							//	}
							//	else if (type == HIGHLIGHT_DATES) {
							//		const int num_dates = num_args - 1;
							//		lv_calendar_date_t* lv_dates = new lv_calendar_date_t[num_dates];
							//		for (int i = 0; i < num_dates; i++) {
							//			uint16_t* date = va_arg(arguments, uint16_t*);
							//			lv_dates[i].day = date[0];
							//			lv_dates[i].month = date[1];
							//			lv_dates[i].year = date[2];
							//		}
							//		lv_calendar_set_highlighted_dates(lv_obj, lv_dates, num_dates);
							//	}
							//	break;
							//}
	}
	//	va_end(arguments);
}

bool UI::toggle_value(ui_object_id id) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	lv_obj_t* lv_obj = ui_obj->lv_obj;
	bool handled = false;
	switch (ui_obj->type) {
	case ui_object_type::LED:
		lv_led_toggle(lv_obj);
		handled = true;
		break;
	case ui_object_type::SWITCH:
		lv_sw_toggle(lv_obj, true);
		handled = true;
		break;
	}
	return handled;
}

ui_object_value UI::get_value(UI_Object_C* ui_object) {
	ui_object_value retval;
	switch (ui_object->type) {
	case ui_object_type::SWITCH: {
		retval.set_value(lv_sw_get_state(ui_object->lv_obj));
		break; }
	case ui_object_type::SLIDER: {
		retval.set_value(lv_slider_get_value(ui_object->lv_obj));
		break; }
	case ui_object_type::LABEL: {
		retval.set_value(lv_label_get_text(ui_object->lv_obj));
		break;
	}
	case ui_object_type::BUTTON: {
		lv_btn_state_t state =  lv_btn_get_state(ui_object->lv_obj);
		retval.set_value(state == LV_BTN_STATE_PR);
		break;
	}
	case ui_object_type::CHECKBOX: {
		retval.set_value(lv_cb_is_checked(ui_object->lv_obj));
		break;
	}
	case ui_object_type::CALENDAR: {
		lv_calendar_date_t* lv_date = lv_calendar_get_pressed_date(ui_object->lv_obj);
		retval.set_date(lv_date->day, lv_date->month, lv_date->year);
		break;
	}
	case ui_object_type::SPINBOX: {
		ui_spinbox_specs* specs = (ui_spinbox_specs*)ui_object->object_data;
		if (specs == nullptr)
			break;
		retval.int_value = lv_spinbox_get_value(ui_object->lv_obj);
		retval.float_value = (float)((0.0 + retval.int_value) / specs->fract);
		sprintf((char*)retval.char_value, "%f", retval.float_value);
		printf("------------- frac = %d  val = %d ->  float = %f\n", specs->fract, retval.int_value, retval.float_value);
		break;
	}
	case ui_object_type::ROLLER: {
		lv_roller_get_selected_str(ui_object->lv_obj, (char*)retval.char_value, retval.buff_size);
		break;
	}
	case ui_object_type::DROPDOWN_LIST: {
		lv_ddlist_get_selected_str(ui_object->lv_obj, (char*)retval.char_value, retval.buff_size);
		break;
	}
	case ui_object_type::LIST: {
		lv_obj_t* but = lv_list_get_btn_selected(ui_object->lv_obj);

		if(ui_object->lv_obj != nullptr)
			retval.set_value(lv_list_get_btn_text(but));
		break;
	}
	case ui_object_type::TEXTAREA: {
		if (ui_object->lv_obj != nullptr)
			retval.set_value(lv_ta_get_text(ui_object->lv_obj));
		break;
	}

	}
	return retval;
}

std::string UI::get_type_name(ui_object_id id) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	if (ui_obj != nullptr) {
		switch (ui_obj->type) {
		case ui_object_type::SWITCH:
			return "switch";
		case ui_object_type::SLIDER:
			return "slider";
		case ui_object_type::BUTTON:
			return "button";
		case ui_object_type::CHECKBOX:
			return "checkbox";
		case ui_object_type::KEYBOARD:
			return "keyboard";
		case ui_object_type::DROPDOWN_LIST:
			return "dropdown";
		case ui_object_type::LIST:
			return "list";
		case ui_object_type::ROLLER:
			return "roller";
		}
	}
	return "none";
}

void UI::set_size(ui_object_id id, lv_coord_t width, lv_coord_t height) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	set_size(ui_obj, width, height);
}

void UI::set_size(ui_object_id id, ui_object_id ref_id) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	lv_obj_t* lv_obj = find_lv_object_by_id(ref_id);
	lv_coord_t width = lv_obj_get_width(lv_obj);
	lv_coord_t height = lv_obj_get_height(lv_obj);
	set_size(ui_obj, width, height);
}

void UI::set_size(UI_Object_C* ui_obj, lv_coord_t width, lv_coord_t height) {
	if (ui_obj == nullptr) return;

	switch (ui_obj->type) {
	case  ui_object_type::DROPDOWN_LIST: {
		lv_ddlist_set_fix_width(ui_obj->lv_obj, width);
		break;
	}
	case  ui_object_type::ROLLER: {
		lv_roller_set_fix_width(ui_obj->lv_obj, width);
		break;
	}
	default: {
		if (height > 0 && width > 0)
			lv_obj_set_size(ui_obj->lv_obj, width, height);
		else if (width > 0)
			lv_obj_set_width(ui_obj->lv_obj, width);
		else if (height > 0)
			lv_obj_set_height(ui_obj->lv_obj, height);
		break;
	}
	}

}


void UI::set_style(ui_object_id id, lv_style_t* lv_style, int style_type) {
	UI_Object_C* ui_obj = find_object_by_id(id);

	switch (ui_obj->type) {
	case ui_object_type::CALENDAR:
		lv_calendar_set_style(ui_obj->lv_obj, style_type, lv_style);
		break;
	case ui_object_type::LED:
		lv_led_set_style(ui_obj->lv_obj, LV_LED_STYLE_MAIN, lv_style);
		break;
	default:
		lv_obj_set_style(ui_obj->lv_obj, lv_style);
		break;

	}
}

void UI::set_align(ui_object_id id, lv_align_t align, ui_object_id base_id, int x_offset, int y_offset) {
	lv_obj_t* lv_obj = find_lv_object_by_id(id);
	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}
}

void UI::set_floating(ui_object_id id, ui_object_id parent_id) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	UI_Object_C* ui_obj_par = find_object_by_id(parent_id);
	lv_obj_set_protect(ui_obj->lv_obj, LV_PROTECT_PARENT);
	lv_obj_set_parent(ui_obj->lv_obj, ui_obj_par->lv_obj);
}

void UI::spinbox_increment(ui_object_id id, bool increment) {
	lv_obj_t* lv_obj = find_lv_object_by_id(id);
	if (increment)
		lv_spinbox_increment(lv_obj);
	else
		lv_spinbox_decrement(lv_obj);
}


UI_Object_C* UI::add_list(ui_object_id parent_id, ui_object_id id, const char* name, ui_list_item_t* items, uint32_t num_items, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_list_create(lv_parent, NULL);

	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}

	lv_obj_set_height(lv_obj, lv_obj_get_height(lv_parent));

	lv_obj_t* lv_list_btn;
	for (uint32_t i = 0; i < num_items; i++) {
		lv_list_btn = lv_list_add_btn(lv_obj, items[i].symbol, items[i].label);
		lv_obj_set_event_cb(lv_list_btn, UI::event_handler);

		char* TotalLine{ new char[strlen(name) + strlen(items[i].label) + 1] };

		TotalLine = strcpy(TotalLine, name);
		TotalLine = strcat(TotalLine, items[i].label);

		add_object(lv_list_btn, items[i].id, id, ui_object_type::BUTTON, callback, TotalLine);
	}
	return 	add_object(lv_obj, id, parent_id, ui_object_type::LIST, nullptr, name);
}
UI_Object_C* UI::add_dropdown(ui_object_id parent_id, ui_object_id id, const char* name, ui_dropdown_specs* specs, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_ddlist_create(lv_parent, NULL);
	specs->apply_specs(lv_obj);
	//lv_ddlist_set_fix_width(ddlist, 150);
	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}
	lv_obj_set_event_cb(lv_obj, UI::event_handler);
	UI_Object_C* ui_obj = add_object(lv_obj, id, parent_id, ui_object_type::DROPDOWN_LIST, callback,name);
	ui_obj->object_data = specs;

	return 	ui_obj;
}

UI_Object_C* UI::add_roller(ui_object_id parent_id, ui_object_id id, const char* name, ui_roller_specs* specs, ui_object_handler_cb_ptr callback, lv_align_t align, ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_roller_create(lv_parent, NULL);
	specs->apply_specs(lv_obj);
	//lv_ddlist_set_fix_width(ddlist, 150);
	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}

	lv_obj_set_event_cb(lv_obj, UI::event_handler);
	UI_Object_C* ui_obj = add_object(lv_obj, id, parent_id, ui_object_type::ROLLER, callback,name);
	ui_obj->object_data = specs;
	return 	ui_obj;
}

UI_Object_C* UI::add_slider(ui_object_id parent_id, ui_object_id id, const char* name, int value, ui_object_handler_cb_ptr callback, lv_align_t align,
	ui_object_id base_id, uint32_t x_offset, uint32_t y_offset) {
	lv_obj_t* lv_parent = find_parent_object(parent_id);
	lv_obj_t* lv_obj = lv_slider_create(lv_parent, NULL);
	if (align < 999) {
		lv_obj_t* lv_base = find_lv_object_by_id(base_id);
		lv_obj_align(lv_obj, lv_base, align, x_offset, y_offset);
	}
	lv_slider_set_value(lv_obj, value, false);
	lv_obj_set_event_cb(lv_obj, UI::event_handler);
	return 	add_object(lv_obj, id, parent_id, ui_object_type::SLIDER, callback, name);
}

UI_Object_C* UI::find_object_by_id(ui_object_id id) {
	for (auto ui_obj : UI::ui_object_list_) {
		if ((uint32_t)ui_obj->id == id)
			return ui_obj;
	}
	return nullptr;
}

lv_obj_t* UI::find_lv_object_by_id(ui_object_id id) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	if (ui_obj == nullptr)
		return nullptr;
	return ui_obj->lv_obj;
}

UI_Object_C* UI::find_object_by_ref(lv_obj_t* ref) {
	for (auto ui_obj : UI::ui_object_list_) {
		if (ui_obj->lv_obj == ref)
			return ui_obj;
	}
	return nullptr;
}

lv_obj_t* UI::find_parent_object(ui_object_id id) {
	lv_obj_t* lv_parent = nullptr;
	if (id > 0)
		lv_parent = find_lv_object_by_id(id);
	if (lv_parent == nullptr)
		lv_parent = lv_scr_act();
	return lv_parent;
}

bool UI::add_update_object(ui_object_id id, uint32_t ms_time) {
	UI_Object_C* ui_obj = find_object_by_id(id);
	if (ui_obj != nullptr) {
		ui_update_object_c* ui_update_obj = new ui_update_object_c(ui_obj, ms_time);
		ui_update_object_list_.push_back(ui_update_obj);
		return true;
	}
	return false;
}

void UI::do_update_objects(lv_task_t* task) {

	for (auto ui_update_obj : ui_update_object_list_) {
		//check if object can fire
		if (ui_update_obj->go(task->last_run)) {
			UI_Object_C* ui_obj = ui_update_obj->ui_obj;
			switch (ui_obj->type) {
			case ui_object_type::LABEL:
				time_t now;
				struct tm info;
				char buf[64];
				time(&now);
				localtime_r(&now, &info);
				strftime(buf, sizeof(buf), "%H:%M:%S", &info);
				lv_label_set_text(ui_obj->lv_obj, buf);
				lv_obj_realign(ui_obj->lv_obj);
				break;
			case ui_object_type::LED:
				lv_led_toggle(ui_obj->lv_obj);
				break;
			default:
				printf("OBJECT %d WRONG TYPE %d FOR UPDATE_TIME\n", ui_obj->id, ui_obj->type);
			}
		}
	}
}