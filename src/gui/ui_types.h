#pragma once

enum class ui_object_type {
	PAGE = 0,
	CONTAINER,
	LABEL,
	TABVIEW,
	TAB,
	BUTTON,
	CHECKBOX,
	SLIDER,
	SWITCH,
	LIST,
	CHART,
	GAUGE,
	LED,
	BAR,
	CALENDAR,
	SPINBOX,
	DROPDOWN_LIST,
	ROLLER,
	TEXTAREA,
	KEYBOARD,
};

enum class ui_event_t {
	CLICKED,
	LONG_CLICKED,
	VALUE_CHANGED,
	DRAG_BEGIN,
	DRAG_END,
	PRESSED,
	RELEASED,
	PRESSED_REPEAT,
	APPLY,
};

enum calendar_value_type {
	TODAY_DATE,
	HIGHLIGHT_DATES,
};

enum ui_keyboard_type {
	UI_KEYB_NONE,
	UI_KEYB_NUM,
	UI_KEYB_TEXT,
};

class ui_keyboard_data {
public:
	ui_keyboard_type type;
};

//typedef enum ui_object_id;

typedef struct {
	ui_object_id id;
	const char* symbol;
	const char* label;
} ui_list_item_t;

typedef struct {
	ui_object_id id;
	const char* label;
} ui_tab_item_t;

typedef struct {
	uint8_t style_type;
	lv_style_t* style;
} ui_styles_t;

typedef int (*update_cb_t) (uint32_t);

typedef struct UI_Object_C ui_object_t;
typedef void (*ui_object_handler_cb_ptr)(UI_Object_C*, ui_event_t);

class UI_Object_C {
public:
	ui_object_id id;
	const char* name = nullptr;
	ui_object_id parent_id;
	ui_object_type type;
	lv_obj_t* lv_obj;
	ui_object_handler_cb_ptr ui_callback;
	void* object_data = nullptr;

	bool has_name() {
		return name != nullptr;
	}

	virtual std::string get_type_name() {
		return "none";
	};
};


class lv_obj_data_c {
public:
	lv_obj_data_c(ui_object_id id, ui_object_type type) {
		this->id = id;
		this->type = type;
	}
	ui_object_id id;
	ui_object_type type;
	char* name;
};

static const char* VALUE_ON = "ON";
static const char* VALUE_OFF = "OFF";

class ui_dropdown_specs {
public:
	ui_dropdown_specs(const char* items, bool arrow) {
		this->items = items;
		this->arrow = arrow;
	}
	const char* items;
	bool arrow = true;

	void apply_specs(lv_obj_t* lv_obj_dropdown) {
		lv_ddlist_set_options(lv_obj_dropdown, items);
		lv_ddlist_set_draw_arrow(lv_obj_dropdown, arrow);
	}
};

class ui_roller_specs {
public:
	ui_roller_specs(const char* items, uint16_t vissible_rows, bool infinite = true) {
		this->items = items;
		this->infinite = infinite;
		this->visible_rows = vissible_rows;
	}
	const char* items;
	bool infinite = true;
	uint16_t visible_rows;

	void apply_specs(lv_obj_t* lv_obj_roller) {
		lv_ddlist_set_options(lv_obj_roller, items);
		if (infinite)
			lv_roller_set_options(lv_obj_roller, items, LV_ROLLER_MODE_INIFINITE);
		else
			lv_roller_set_options(lv_obj_roller, items, LV_ROLLER_MODE_NORMAL);

		lv_roller_set_visible_row_count(lv_obj_roller, visible_rows);
	}
};

class ui_textarea_data {
public:
	UI_Object_C* ui_textarea;
	UI_Object_C* ui_keyboard;
	bool show_kb_on_click = true;
	bool hide_kb_on_done = true;
	void set_text_area(ui_object_t* ta) {
		ta->object_data = this;
		this->ui_textarea = ta;
	}
	void set_keyboard(ui_object_t* kb) {
		kb->object_data = this;
		this->ui_keyboard = kb;
	}
};

class ui_spinbox_specs {
public:
	ui_spinbox_specs(int count, int sep, int left, int min, int max, int val, int step) {
		digit_count = count;
		seperator_pos = sep;
		left_number_padding = left;
		this->min = min;
		this->max = max;
		this->value = val;
		this->step = step;
		this->fract = 1;
		if (seperator_pos > 0 && seperator_pos < count)
			fract = pow(10, (digit_count - seperator_pos));

	}
	int digit_count;
	int seperator_pos;
	int left_number_padding;
	int min;
	int max;
	int value;
	int step;
	int fract;

	void apply_specs(UI_Object_C* ui_obj_spinner) {
		ui_obj_spinner->object_data = this;
		lv_spinbox_set_digit_format(ui_obj_spinner->lv_obj, digit_count, seperator_pos);
		lv_spinbox_set_padding_left(ui_obj_spinner->lv_obj, left_number_padding);
		lv_spinbox_set_range(ui_obj_spinner->lv_obj, min, max);
		lv_spinbox_set_step(ui_obj_spinner->lv_obj, step);
	}
};

class ui_update_object_c {
public:
	ui_update_object_c(UI_Object_C* ui_obj, uint32_t ms_time) {
		this->ui_obj = ui_obj;
		this->ms_time = ms_time;
	}

	bool go(unsigned long  now) {
		if ((this->last_time + this->ms_time) < now) {
			this->last_time = now;
			return true;
		}
		return false;
	}

	uint32_t ms_time;
	unsigned long  last_time;
	UI_Object_C* ui_obj;
};

class ui_object_value {
private:
	const char* char_none = "None";
public:
	ui_object_value() {	};

	ui_object_value(lv_obj_t* lv_obj) {	
		lv_obj_data_c* data = (lv_obj_data_c*)lv_obj_get_user_data(lv_obj);
		switch (data->type) {
		case ui_object_type::CALENDAR: {
			lv_calendar_date_t* lv_date = lv_calendar_get_pressed_date(lv_obj);
			set_date(lv_date->day, lv_date->month, lv_date->year - 1900);
			break;
		}
		case ui_object_type::SWITCH: {
			set_value(lv_sw_get_state(lv_obj));
			break;
		}
		default:
			set_value(char_none);
			break;
		}
	};

	static const int buff_size = 120;
	int int_value = 0;
	float float_value = 0.0;
	char* char_value = new char[buff_size];
	double double_value = 0;
	bool bool_value = false;
	//tm* date_time = new tm();


	void set_value(char* string) {
		strcpy(char_value, string);
	}

	void set_value(bool value) {
		bool_value = value;
		if (value)
			strcpy(char_value, VALUE_ON);
		else
			strcpy(char_value, VALUE_OFF);
	}

	void set_value(int value) {
		int_value = value;
		float_value = (float)0.0 + value;
		sprintf((char*)char_value, "%d", value);
	}

	void set_date(int day, int month, int year) {
		//char* buffer = new char[buff_size];
		tm date_time;
		date_time.tm_mday = day;
		date_time.tm_mon = month;
		date_time.tm_year = year;
		strftime(char_value, buff_size, "%d %B %Y", &date_time);
		//strcpy(char_value, buffer);
		//delete(buffer);
	}

	//void set_date(int day, int month, int year) {
	//	//date_time->tm_mday = day;
	//	//date_time->tm_mon = month;
	//	//date_time->tm_year = year - 1900;
	//	set_date_str(day,month,year - 1900);
	//}

	//void set_time(int h, int m, int s) {
	//	date_time->tm_hour = h;
	//	date_time->tm_min = m;
	//	date_time->tm_sec = s;
	//	set_time_str();
	//}



	void set_time_str(int h, int m, int s) {
		char* buffer = new char[buff_size];
		tm date_time;
		date_time.tm_hour = h;
		date_time.tm_min = m;
		date_time.tm_sec = s;
		strftime(buffer, buff_size, "%h:%m:%s", &date_time);
		strcpy(char_value, buffer);
		delete(buffer);
	}

	//void set_date_time_str() {
	//	char* buffer = new char[buff_size];
	//	strftime(buffer, buff_size, "%h:%m:%s %d %B %Y", date_time);
	//	strcpy(char_value, buffer);
	//	//delete(buffer);
	//}

	std::string* get_string() {
		return new std::string(char_value);
	}
};