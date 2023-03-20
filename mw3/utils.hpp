#pragma once
//#include <wmm>

namespace utils
{
	template<typename T>
	static T sig_scan(void* module, const char* signature) {
		auto pattern_to_byte = [](const char* pattern) -> std::vector<int> {
			auto bytes = std::vector<int>{};
			for (auto cur = const_cast<char*>(pattern); cur < (const_cast<char*>(pattern) + strlen(pattern)); ++cur) {
				if (*cur == '?') {
					++cur;
					if (*cur == '?')
						++cur;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(cur, &cur, 16));
				}
			}
			return bytes;
		};

		auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + reinterpret_cast<PIMAGE_DOS_HEADER>(module)->e_lfanew);
		auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
		auto module_bytes = reinterpret_cast<std::uint8_t*>(module);

		auto bytes = pattern_to_byte(signature);
		auto size = bytes.size();
		auto data = bytes.data();

		for (auto i = 0ul; i < size_of_image - size; ++i) {
			bool found = true;
			for (auto j = 0ul; j < size; ++j) {
				if (module_bytes[i + j] != data[j] && data[j] != -1) {
					found = false;
					break;
				}
			}
			if (found)
				return reinterpret_cast<T>(&module_bytes[i]);
		}
		return NULL;
	}

	template <typename t>
	static t hook_vfunc(void* class_base, void* new_function, int index)
	{
		auto p_interface = reinterpret_cast<uintptr_t**>(class_base);

		size_t method_count = 0;

		while (reinterpret_cast<uintptr_t*>(*p_interface)[method_count])
			method_count++;

		auto original_vmt = *p_interface;
		auto vmt = new uintptr_t[sizeof(uintptr_t) * method_count];
		memcpy(vmt, original_vmt, sizeof(uintptr_t) * method_count);

		vmt[index] = reinterpret_cast<uintptr_t>(new_function);
		*p_interface = vmt;


		return reinterpret_cast<t>(original_vmt[index]);;
	}

}

struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;    // from the D3DFVF_XYZRHW flag
	DWORD color;    // from the D3DFVF_DIFFUSE flag
};

class c_renderer
{
public:
	static c_renderer* get() {
		static c_renderer* instance = new c_renderer;
		return instance;
	}
public:
	struct vertex_t
	{
		FLOAT x, y, z, rhw;
		DWORD color;
	};

	void draw_rect(float x, float y, float width, float height, D3DCOLOR col) {
		D3DXVECTOR2 points[5];
		points[0] = D3DXVECTOR2(x, y);
		points[1] = D3DXVECTOR2(x + width, y);
		points[2] = D3DXVECTOR2(x + width, y + height);
		points[3] = D3DXVECTOR2(x, y + height);
		points[4] = D3DXVECTOR2(x, y);
		d3line->SetWidth(1);
		d3line->Draw(points, 5, col);
	}

	void draw_filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, D3DCOLOR color, IDirect3DDevice9* device) {
		CUSTOMVERTEX vertices[3] = {
			{ float(x1), float(y1), 0.0f, 1.0f, color },{ float(x2), float(y2), 0.0f, 1.0f, color },{ float(x3), float(y3), 0.0f, 1.0f, color }
		};

		device->SetRenderState(D3DRS_ZENABLE, false);

		DWORD state1, state2, state3;
		device->GetRenderState(D3DRS_ALPHABLENDENABLE, &state1);
		device->GetRenderState(D3DRS_SRCBLEND, &state2);
		device->GetRenderState(D3DRS_DESTBLEND, &state3);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		/*DWORD stage1, stage2, stage3;
		device->GetTextureStageState(0, D3DTSS_COLOROP, &stage1);
		device->GetTextureStageState(0, D3DTSS_COLORARG1, &stage2);
		device->GetTextureStageState(0, D3DTSS_COLORARG2, &stage3);

		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);   //Ignored*/

		DWORD _fvf;
		device->GetFVF(&_fvf);
		device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

		device->SetTexture(0, 0);

		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, vertices, sizeof(CUSTOMVERTEX));

		device->SetFVF(_fvf);

		/*	device->SetTextureStageState(0, D3DTSS_COLOROP, stage1);
			device->SetTextureStageState(0, D3DTSS_COLORARG1, stage2);
			device->SetTextureStageState(0, D3DTSS_COLORARG2, stage3);   //Ignored*/

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, state1);
		device->SetRenderState(D3DRS_SRCBLEND, state2);
		device->SetRenderState(D3DRS_DESTBLEND, state3);

	}

	void draw_line(float x, float y, float xx, float yy, D3DCOLOR col) {
		D3DXVECTOR2 line[2];
		d3line->SetWidth(1);
		line[0] = D3DXVECTOR2(x, y);
		line[1] = D3DXVECTOR2(xx, yy);
		d3line->Draw(line, 2, col);
	}

	void draw_line(D3DXVECTOR2 x1, D3DXVECTOR2 y1, D3DCOLOR col) {
		D3DXVECTOR2 line[2];
		d3line->SetWidth(1);
		line[0] = x1;
		line[1] = y1;
		d3line->Draw(line, 2, col);
	}

	void draw_filled_rect(float x, float y, float w, float h, D3DCOLOR col) {
		D3DXVECTOR2 line[2];
		d3line->SetWidth(w);
		line[0] = D3DXVECTOR2(x + w / 2, y);
		line[1] = D3DXVECTOR2(x + w / 2, y + h);
		d3line->Begin();
		d3line->Draw(line, 2, col);
		d3line->End();
	}

	void draw_dx_string(float x, float y, LPD3DXFONT pFont, D3DCOLOR color, int flags, bool outline, std::string text, ...)
	{
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, text);
		_vsnprintf_s(buf, sizeof(buf), text.c_str(), va_alist);
		va_end(va_alist);

		RECT rect;
		if (outline) {
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(NULL, buf, -1, &rect, flags | DT_NOCLIP, color);
	}

	void draw_dx_string(float x, float y, LPD3DXFONT pFont, D3DCOLOR color, int flags, bool outline, std::wstring text, ...)
	{
		va_list va_alist;
		wchar_t buf[1024];
		va_start(va_alist, text);
		_vsnwprintf_s(buf, sizeof(buf), text.c_str(), va_alist);
		va_end(va_alist);

		RECT rect;
		if (outline) {
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextW(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextW(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextW(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextW(NULL, buf, -1, &rect, flags | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextW(NULL, buf, -1, &rect, flags | DT_NOCLIP, color);
	}

	void draw_logger_dx_string(float x, float y, LPD3DXFONT pFont, DWORD color, int a, const wchar_t* text) {
		RECT rect;
		SetRect(&rect, x - 1, y, x - 1, y);
		pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, a));
		SetRect(&rect, x + 1, y, x + 1, y);
		pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, a));
		SetRect(&rect, x, y - 1, x, y - 1);
		pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, a));
		SetRect(&rect, x, y + 1, x, y + 1);
		pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, a));
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
	}

	void draw_cursor(int x, int y) { // can't fucking draw triangle so fuck it
		draw_filled_rect(x + 1, y, 1, 17, D3DCOLOR_RGBA(3, 6, 26, 255));
		for (int i = 0; i < 11; i++)
			draw_filled_rect(x + 2 + i, y + 1 + i, 1, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 8, y + 12, 5, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 8, y + 13, 1, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 9, y + 14, 1, 2, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 10, y + 16, 1, 2, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 8, y + 18, 2, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 7, y + 16, 1, 2, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 6, y + 14, 1, 2, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 5, y + 13, 1, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 4, y + 14, 1, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 3, y + 15, 1, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		draw_filled_rect(x + 2, y + 16, 1, 1, D3DCOLOR_RGBA(3, 6, 26, 255));
		for (int i = 0; i < 4; i++)
			draw_filled_rect(x + 2 + i, y + 2 + i, 1, 14 - (i * 2), D3DCOLOR_RGBA(255 - (i * 4), 255 - (i * 4), 255 - (i * 4), 255));
		draw_filled_rect(x + 6, y + 6, 1, 8, D3DCOLOR_RGBA(235, 235, 235, 255));
		draw_filled_rect(x + 7, y + 7, 1, 9, D3DCOLOR_RGBA(231, 231, 231, 255));
		for (int i = 0; i < 4; i++)
			draw_filled_rect(x + 8 + i, y + 8 + i, 1, 4 - i, D3DCOLOR_RGBA(227 - (i * 4), 227 - (i * 4), 227 - (i * 4), 255));
		draw_filled_rect(x + 8, y + 14, 1, 4, D3DCOLOR_RGBA(207, 207, 207, 255));
		draw_filled_rect(x + 9, y + 16, 1, 2, D3DCOLOR_RGBA(203, 203, 203, 255));
	}

};

static std::vector<int> ranks =
{
         //rank 0:
          0,
      
        //rank 1:
          800,
      
        //rank 2:
          1900,
      
        //rank 3:
          3100,
      
        //rank 4:
          4900,
      
        //rank 5:
          7100,
      
        //rank 6:
          9600,
      
        //rank 7:
          12400,
      
        //rank 8:
          15600,
      
        //rank 9:
          19200,
      
        //rank 10:
          23100,
      
        //rank 11:
          27500,
      
        //rank 12:
          32400,
      
        //rank 13:
          37800,
      
        //rank 14:
          43700,
      
        //rank 15:
          50100,
      
        //rank 16:
          57000,
      
        //rank 17:
          64400,
      
        //rank 18:
          72300,
      
        //rank 19:
          80700,
      
        //rank 20:
          89600,
      
        //rank 21:
          99000,
      
        //rank 22:
          108900,
      
        //rank 23:
          119300,
      
        //rank 24:
          130200,
      
        //rank 25:
          141600,
      
        //rank 26:
          153000,
      
        //rank 27:
          165900,
      
        //rank 28:
          178800,
      
        //rank 29:
          192200,
      
        //rank 30:
          206200,
      
        //rank 31:
          220800,
      
        //rank 32:
          236000,
      
        //rank 33:
          251800,
      
        //rank 34:
          268200,
      
        //rank 35:
          285200,
      
        //rank 36:
          302800,
      
        //rank 37:
          321000,
      
        //rank 38:
          339800,
      
        //rank 39:
          369200,
      
        //rank 40:
          379000,
      
        //rank 41:
          399800,
      
        //rank 42:
          421000,
      
        //rank 43:
          442800,
      
        //rank 44:
          456200,
      
        //rank 45:
          488200,
      
        //rank 46:
          511800,
      
        //rank 47:
          536000,
      
        //rank 48:
          560800,
      
        //rank 49:
          586200,
      
        //rank 50:
          612350,
      
        //rank 51:
          639250,
      
        //rank 52:
          666900,
      
        //rank 53:
          695300,
      
        //rank 54:
          724450,
      
        //rank 55:
          754350,
      
        //rank 56:
          785000,
      
        //rank 57:
          816400,
      
        //rank 58:
          848550,
      
        //rank 59:
          881450,
      
        //rank 60:
          915100,
      
        //rank 61:
          949500,
      
        //rank 62:
          984650,
      
        //rank 63:
          1020550,
      
        //rank 64:
          1057200,
      
        //rank 65:
          1094600,
      
        //rank 66:
          1132750,
      
        //rank 67:
          1171650,
      
        //rank 68:
          1211300,
      
        //rank 69:
          1251700,
      
        //rank 70:
          1292850,
      
        //rank 71:
          1334500,
      
        //rank 72:
          1377150,
      
        //rank 73:
          1420300,
      
        //rank 74:
          1464450,
      
        //rank 75:
          1509100,
      
        //rank 76:
          1554750,
      
        //rank 77:
          1500900,
      
        //rank 78:
          1648050,
      
        //rank 79:
          1695700,
      
        //rank 80:
          1746200
      
};