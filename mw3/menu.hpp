#pragma once
//#include "main.hpp"

class c_menu_framework
{

public:
	float menu_x = 250, menu_y = 290;
	enum item_type : int {
		type_bool = 0,
		type_int = 1,
		type_float = 2
	};
	enum tab_type : int {
		type_on_off = 0,
		type_tab = 1,
		type_dont_draw = 2
	};
	static struct menu_item_t {
		std::wstring m_title;

		float* m_float;
		int* m_int;
		bool* m_bool;

		float m_float_step;
		int m_int_step;
		int m_type;
		int tab_type;

		float m_float_min;
		int m_int_min;

		float m_float_max;
		int m_int_max;

		std::wstring tool_tip;
	};

	//int m_total_items = 0;
	int m_current_pos = 0;
	int menu_page = 0;
	std::vector<menu_item_t> items;
public:

	void add_entry(std::wstring title, bool* value, int is_tab = 0, std::wstring tooltip = L"") {
		menu_item_t m_item;
		m_item.m_title = title;
		m_item.m_bool = value;
		m_item.m_type = item_type::type_bool;
		m_item.tab_type = is_tab;
		m_item.tool_tip = tooltip;
		items.emplace_back(m_item);
	}

	template <class t>
	void add_entry(std::wstring title, t* value, t min, t max, t step) {
		menu_item_t m_item;
		m_item.m_title = title;
		if (std::is_same<t, float>()) {
			m_item.m_float = (float*)value;
			m_item.m_float_min = min;
			m_item.m_float_max = max;
			m_item.m_float_step = step;
			m_item.m_type = item_type::type_float;
		}
		else if (std::is_same<t, int>()) {
			m_item.m_int = (int*)value;
			m_item.m_int_min = min;
			m_item.m_int_max = max;
			m_item.m_int_step = step;
			m_item.m_type = item_type::type_int;
		}
		items.emplace_back(m_item);
	}

	void draw();

	void do_menu_controls();

};



void c_menu_framework::draw() {
	this->menu_x = vars::menu::menu_x;
	this->menu_y = vars::menu::menu_y;


	static bool esp_tab = false;
	static bool aimbot_tab = false;

	this->add_entry(L"AIMBOT", &aimbot_tab, true);
	if (aimbot_tab) {
		this->add_entry(L"  Enabled", &vars::aimbot::enabled);
		this->add_entry(L"  FOV", &vars::aimbot::fov, 0.f, 180.f, 0.5f);
		this->add_entry(L"  VisCheck", &vars::aimbot::vischeck);

	}
	this->add_entry(L"ESP", &esp_tab, true);
	if (esp_tab) {
		this->add_entry(L"  Enabled", &vars::esp::enabled);
		this->add_entry(L"  Skeleton", &vars::esp::draw_skeleton);
		static bool color_tab = false;
		//this->add_entry(L"  COLOR", &color_tab, true);
		if (color_tab) {
			static int vis_r = get_r(vars::esp::visible_col), vis_g = get_g(vars::esp::visible_col), vis_b = get_b(vars::esp::visible_col), unvis_r = get_r(vars::esp::unvisibile_col), unvis_g = get_g(vars::esp::unvisibile_col), unvis_b = get_b(vars::esp::unvisibile_col);
			this->add_entry(L"    Visible Red", &vis_r, 0, 255, 1);
			this->add_entry(L"    Visible Green", &vis_g, 0, 255, 1);
			this->add_entry(L"    Visible Blue", &vis_b, 0, 255, 1);

			this->add_entry(L"    UnVisible Red", &unvis_r, 0, 255, 1);
			this->add_entry(L"    UnVisible Green", &unvis_g, 0, 255, 1);
			this->add_entry(L"    UnVisible Blue", &unvis_b, 0, 255, 1);
			vars::esp::unvisibile_col = D3DCOLOR_RGBA(unvis_r, unvis_g, unvis_b, 0);
			vars::esp::visible_col = D3DCOLOR_RGBA(vis_r, vis_g, vis_b, 0);

		}
	}
	static bool menu_tab = false;
	this->add_entry(L"MENU", &menu_tab, true);
	if (menu_tab) {
		this->add_entry(L"Force Host", &vars::menu::force_host);
		this->add_entry(L"Noclip", &vars::menu::noclip);

		this->add_entry(L"  Menu x", &vars::menu::menu_x, 0.f, 1920.f, 1.f);
		this->add_entry(L"  Menu y", &vars::menu::menu_y, 0.f, 1080.f, 1.f);
		this->add_entry(L"  Menu Alpha", &vars::menu::menu_alpha, 0.f, 255.f, 1.f);

		this->add_entry(L"  viewmodel fov", &vars::menu::weapon_fov, 0.f, 300.f, 1.f);
		this->add_entry(L"  viewmodel x", &vars::menu::weapon_x, 0.f, 300.f, 0.1f);
		this->add_entry(L"  viewmodel y", &vars::menu::weapon_y, 0.f, 300.f, 0.1f);
		this->add_entry(L"  viewmodel z", &vars::menu::weapon_z, 0.f, 300.f, 0.1f);
	}
	static bool stat_tab = false;
	this->add_entry(L"STAT EDITOR", &stat_tab, true);
	if (stat_tab) {
		static int prestige = *(int*)(0x1CE5048), rank = 0;
		this->add_entry(L"    Prestige", &prestige, 0, 20, 1);
		this->add_entry(L"    Rank", &rank, 0, 80, 1);
		static bool set = false;
		this->add_entry(L"    Set Edits", &set);
		if (set) {
			*(int*)(0x1CE5048) = prestige;
			*(int*)(0x1CE4E38) = ranks[rank];


			set = false;
		}
		//if (prestige != o_prestige) {
		//	*(int*)(30298184) = prestige;
		//	o_prestige_presi
		//}
	}
}

void c_menu_framework::do_menu_controls() {
	static int item_gap = 14;
	int alpha = vars::menu::menu_alpha;
	//alpha = vars::menu::menu_alpha;

	auto get_frame_rate = []() -> int
	{
		static int i_fps, i_last_fps;
		static float fl_last_tick_count, fl_tick_count;
		fl_tick_count = clock() * 0.001f;
		i_fps++;
		if ((fl_tick_count - fl_last_tick_count) >= 1.0f)
		{
			fl_last_tick_count = fl_tick_count;
			i_last_fps = i_fps;
			i_fps = 0;
		}
		return i_last_fps;
	};

	//c_renderer::get()->draw_dx_string(255, 255, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, 255), DT_LEFT, true, L"Faggots");
	//return;
	c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y - 17, 154, 17, D3DCOLOR_RGBA(26, 26, 26, alpha));
	c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y - 17, 154, 17, D3DCOLOR_RGBA(102, 0, 204, alpha));

	c_renderer::get()->draw_dx_string(this->menu_x + (154 / 2), this->menu_y - 15, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_CENTER, false, std::string("Device's Premium Hookenz - " + std::to_string(get_frame_rate())).c_str());

	c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y, 154, (items.size() * item_gap), D3DCOLOR_RGBA(31, 31, 31, alpha));
	c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y, 154, (items.size() * item_gap), D3DCOLOR_RGBA(102, 0, 204, alpha));

	c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y + (item_gap * m_current_pos), 154, item_gap, D3DCOLOR_RGBA(26, 26, 26, alpha));
	c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y + (item_gap * m_current_pos), 154, item_gap, D3DCOLOR_RGBA(102, 0, 204, alpha));

	if (items.empty())
		return;
	if (m_current_pos > items.size())
		m_current_pos = items.size();

	if (!items[m_current_pos].tool_tip.empty()) {
		c_renderer::get()->draw_filled_rect(this->menu_x - 3, this->menu_y + (items.size() * item_gap) + 3, 154, 28, D3DCOLOR_RGBA(31, 31, 31, alpha));
		c_renderer::get()->draw_rect(this->menu_x - 3, this->menu_y + (items.size() * item_gap) + 3, 154, 28, D3DCOLOR_RGBA(102, 0, 204, alpha));

		c_renderer::get()->draw_dx_string(this->menu_x, this->menu_y + (items.size() * item_gap) + 5, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_LEFT, true, L"NOTE:");
		c_renderer::get()->draw_dx_string(this->menu_x, this->menu_y + (items.size() * item_gap) + 16, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_LEFT, true, items[m_current_pos].tool_tip.c_str());

	}
	//0x00545680
	// 6A ? E8 ? ? ? ? 8B 44 24 ? 8A 08 83 C4 ? 80 F9 ? 74 05 80 F9 ? 75 23 8A 48 ? 80 F9 ? 7C 1B 80 F9 ? 7D 16 0F BE C9 83 C0 ? 83 E9 ? 80 38 ? 75 0C 40 80 38 ? 74 FA EB 04 8B 4C 24 ? 55 33 D2 8D 2C 49 56 8D 2C AD ? ? ? ? 38 10 74 07 42 80 3C 02 ? 75 F9 8B 75 ? 8D 0C 16 3B 4D ? 7C 0D 6A ? E8 ? ? ? ? 83 C4 ? 5E 5D C3 8B 4D ? 57 8D 7A ? 03 CE 8B F0 85 FF 74 0B 8A 06 88 01 41 46 83 EF ? 75 F5 01 55 ? 6A ? E8 ? ? ? ? 83 C4 ? 5F 5E 5D C3

	for (auto item = 0; item < items.size(); ++item) {
		auto height = this->menu_y + (item_gap * item);

		switch (items[item].m_type)
		{
		case item_type::type_bool: {
			c_renderer::get()->draw_dx_string(this->menu_x, height, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_LEFT, true, items[item].m_title.c_str());
			if (items[item].tab_type == tab_type::type_on_off)
				c_renderer::get()->draw_dx_string(this->menu_x + 150, height, tahoma_12, *items[item].m_bool ? D3DCOLOR_RGBA(0, 255, 0, alpha) : D3DCOLOR_RGBA(255, 0, 0, alpha), DT_RIGHT, true, *items[item].m_bool ? L"ON" : L"OFF");
			else if (items[item].tab_type == tab_type::type_tab)
				c_renderer::get()->draw_dx_string(this->menu_x + 150, height, tahoma_12, *items[item].m_bool ? D3DCOLOR_RGBA(0, 255, 0, alpha) : D3DCOLOR_RGBA(255, 0, 0, alpha), DT_RIGHT, true, *items[item].m_bool ? L"[-]" : L"[+]");
			break;
		}
		case item_type::type_int: {
			c_renderer::get()->draw_dx_string(this->menu_x, height, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_LEFT, true, items[item].m_title.c_str());
			c_renderer::get()->draw_dx_string(this->menu_x + 150, height, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_RIGHT, true, L"%i", *items[item].m_int);
			break;
		}
		case item_type::type_float: {
			c_renderer::get()->draw_dx_string(this->menu_x, height, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_LEFT, true, items[item].m_title.c_str());
			c_renderer::get()->draw_dx_string(this->menu_x + 150, height, tahoma_12, D3DCOLOR_RGBA(255, 255, 255, alpha), DT_RIGHT, true, L"%.2f", *items[item].m_float);
			break;
		}
		default:
			break;
		}

		if (GetAsyncKeyState(VK_UP) & 1) {
			m_current_pos = (m_current_pos > 0) ? (m_current_pos - 1) : items.size() - 1;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 1) {
			m_current_pos = (m_current_pos < items.size() - 1) ? (m_current_pos + 1) : 0;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 1) {
			switch (items[m_current_pos].m_type) {
			case item_type::type_bool: {
				*items[m_current_pos].m_bool = !*items[m_current_pos].m_bool;
				break;
			}
			case item_type::type_int: {
				*items[m_current_pos].m_int -= items[m_current_pos].m_int_step;
				if (*items[m_current_pos].m_int < items[m_current_pos].m_int_min)
					*items[m_current_pos].m_int = items[m_current_pos].m_int_max;
				break;
			}
			case item_type::type_float: {
				*items[m_current_pos].m_float -= items[m_current_pos].m_float_step;
				if (*items[m_current_pos].m_float < items[m_current_pos].m_float_min)
					*items[m_current_pos].m_float = items[m_current_pos].m_float_max;
				break;
			}
			default:
				break;
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 1) {
			switch (items[m_current_pos].m_type) {
			case item_type::type_bool: {
				*items[m_current_pos].m_bool = !*items[m_current_pos].m_bool;
				break;
			}
			case item_type::type_int: {
				*items[m_current_pos].m_int += items[m_current_pos].m_int_step;
				if (*items[m_current_pos].m_int > items[m_current_pos].m_int_max)
					*items[m_current_pos].m_int = items[m_current_pos].m_int_min;
				break;
			}
			case item_type::type_float: {
				*items[m_current_pos].m_float += items[m_current_pos].m_float_step;
				if (*items[m_current_pos].m_float > items[m_current_pos].m_float_max)
					*items[m_current_pos].m_float = items[m_current_pos].m_float_min;
				break;
			}
			default:
				break;
			}
		}
	}
	if (!items.empty())
		items.clear();
}
static c_menu_framework* menu_framework = new c_menu_framework;