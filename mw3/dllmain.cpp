// dllmain.cpp : Defines the entry point for the DLL application.

#include <Windows.h>
#include <stdio.h>
#include "main.hpp"
#include "menu.hpp"

//448770
void(__cdecl* o_cg_draw2d)(int);
bool(__cdecl* o_live_skill)(int, int);
int(__fastcall* o_writepacket)(int);


void __cdecl cg_draw_2d(int local)
{
	esp_mutex.lock();
	if (vars::esp::enabled) {
		//esp_mutex.lock();
		esp_loop();
	//	esp_mutex.unlock();
	}
	esp_mutex.unlock();

	//if (vars::aimbot::enabled)
	//	aimbot();

	if (vars::aimbot::enabled)
		aimbot();

	return o_cg_draw2d(local);
}


typedef int(__cdecl* live_get_skills_fn)(int a1, int a2);
live_get_skills_fn o_live_get_skills = 0;
int __cdecl live_get_skills(int a1, int a2)
{
	printf("eyaeas\n");
	return 0;
}

typedef struct
{
	vector_3			lerpOrigin;
	vector_3			Velocity;
	char				unk01[0x340];
	int				Overlay; //65 = Nightvision
	char				unk02[0x4];
	float				spreadMultiplier;
	char				unk03[0x18];
	int				SecondaryStock;
	char				unk04[0xC];
	int				PrimaryStock;
	char				unk05[0x64];
	int				SecondaryClip;
	char				unk06[0x8];
	int				Nades;
	char				unk07[0x8];
	int				PrimaryClip;
	char				unk08[0x8];
	int				Flashes;
	char				unk09[0x9C];
	int				ServerTime;
	char				unk10[0x13];
	BYTE				Perks;
	char				unk[0x2ECD];
	float				SpeedMultiplier;
	char				unk11[0x20C];
	int				NoClip;
	char				unk12[0x338];
}entity_info_t;

int get_local_client() {
	return *(int*)(0x00900FA0 + 0x150);
}

bool is_alive(int Client)
{
	if (*(int*)(0x1A66E28 + (Client * 0x274) + 0x19C) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

typedef struct
{
	char unknown0[28]; //0x0000
	__int32 serverTime; //0x001C  
	char unknown32[4]; //0x0020
	char gameType[4];                                //0x24 (0x9CE144)
	char unknown40[28]; //0x0028
	char hostName[64];                               //0x44 (0x9CE164)
	char _0x0084[0xC4];                              //0x84
	int maxClients;                                  //0x148 (0x9CE268)
	char _0x014C[0x4];                               //0x14C
	char mapName[64];                                //0x150 (0x9CE270)	
}cgs_t;

HRESULT __stdcall end_scene(IDirect3DDevice9* pDevice)
{
	if (!tahoma_12 || !tahoma_10 || !d3line)
	{
		D3DXCreateFontA(pDevice, 12, 0, 400, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Tahoma", &tahoma_12);
		D3DXCreateFontA(pDevice, 10, 0, 400, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Tahoma", &tahoma_10);
		D3DXCreateLine(pDevice, &d3line);
		return o_endscene(pDevice);
	}

	//write_ptr(0x5CC6ACC, 0xC, 300);

	static auto is_active = false;
	static auto was_active = false;

	if (GetAsyncKeyState(VK_INSERT) & 1)
		is_active = !is_active;

	if (is_active) {
		menu_framework->draw();
		menu_framework->do_menu_controls();
	}

	//auto cgs = (cgs_t*)(0x8FCA20);

	//0x8DFD84 // ishost
	auto is_host = *(int*)(0x8DFD84);
	c_renderer::get()->draw_dx_string(20, 20, tahoma_12, D3DCOLOR_RGBA(255, 0, 0, 255), DT_LEFT, true, (is_host) ? "HOST" : (std::string("NOT HOST")).c_str());

	static float o1 = 0.f, o2 = 0.f, o3 = 0.f, o4 = 0.f;
	
	if (o1 != vars::menu::weapon_fov || o2 != vars::menu::weapon_x || o3 != vars::menu::weapon_y || o4 != vars::menu::weapon_z) {
		write_ptr(0xB0C738, 0xC, vars::menu::weapon_fov);
		write_ptr(0xB1EDA4, 0xC, vars::menu::weapon_x);
		write_ptr(0xB1ED80, 0xC, vars::menu::weapon_y);
		write_ptr(0xB1ED90, 0xC, vars::menu::weapon_z);
		//write_ptr(0x1C2D398, 0xC, 0); //player_stunWhiteFlash
		write_ptr(0x8FC9E0, 0xC, 1);

		o1 = vars::menu::weapon_fov;
		o2 = vars::menu::weapon_x;
		o3 = vars::menu::weapon_y;
		o4 = vars::menu::weapon_z;
	}


	if (vars::menu::noclip) {
		auto server_entinfo = (entity_info_t*)0x1C2D45C;
		int local = get_local_client();
		if (is_alive(local) && server_entinfo) {
			server_entinfo[local].SpeedMultiplier = 4.5f;
		}
	}
	if (vars::menu::force_host) {
		auto host = *(int*)20135204;


		*(int*)(host + 0xC) = 1;
		//Cbuf_AddText(0, "party_minplayers 1\n");
	}

	esp_mutex.lock();
	if (!draw_queue.empty()) {
		for (auto p : draw_queue) {
			if (p.type == 0) {

				c_renderer::get()->draw_line(D3DXVECTOR2(p.x, p.y), D3DXVECTOR2(p.w, p.h), p.col);

			}
		}
		draw_queue.clear();

	}
	esp_mutex.unlock();

	return o_endscene(pDevice);
}



int __fastcall write_packet(int a1)
{
	auto input = (input_t*)0x1065C90;
	//PINT cmd_num = (PINT)input->currentCmdNum;
	//usercmd_t* cl_cmds = (usercmd_t*)input->usercmds;
	int newCmdNum = input->currentCmdNum & 0x7F;
	usercmd_t* newCmd = &input->usercmds[newCmdNum];

	(input->currentCmdNum)++;

	int cmdNum = input->currentCmdNum & 0x7F;
	usercmd_t* oldCmd = &input->usercmds[cmdNum];
	*oldCmd = *newCmd;

	newCmd->servertime -= 1;

	//if (vars::aimbot::enabled)
		//aimbot(newCmd);

	return o_writepacket(a1);
}



typedef int(__cdecl* sub_448300_fn)(int a1, signed int a2, int a3);
sub_448300_fn o_sub_448300 = 0;
int __cdecl sub_448300(int a1, signed int a2, int a3)
{
	auto server_entinfo = (entity_info_t*)0x1C2D45C;
	int local = get_local_client();
	if (is_alive(local)) {
		server_entinfo[local].NoClip = vars::menu::noclip ? 2 : 0;
		if (server_entinfo[local].NoClip == 2)
			printf("zoomzoom nigga\n");
	}

	return o_sub_448300(a1, a2, a3);
}

unsigned long __stdcall on_attach(LPVOID dll)
{
	auto dx9device = **(uint32_t**)(utils::sig_scan<std::uint8_t*>(GetModuleHandleA("shaderapidx9.dll"), "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	printf("dx9device %i\n", dx9device);
	o_endscene = utils::hook_vfunc<endscene_fn>((void*)dx9device, end_scene, 42);
	printf("Hooked\n");

	return 1;
}

static void* direct_jmp(void* from, void* to, size_t bytestosave)
{
	size_t len = bytestosave + 0x5 + 0x5; // bytes apart of new jump + new jump

	void* memory = malloc(len);

	memset(memory, 0x0, len);

	DWORD memusedsize = bytestosave + 0x5;

	DWORD newaddr = ((DWORD)from + memusedsize);

	memcpy(memory, from, memusedsize);

	((BYTE*)memory)[memusedsize] = 0xE9;

	DWORD Difference = (newaddr - (DWORD)(&((BYTE*)memory)[memusedsize]) - 0x5);

	*(DWORD*)&(((BYTE*)memory)[memusedsize + 1]) = Difference;

	Difference = ((DWORD)to - (DWORD)from - 0x5);

	DWORD DefaultProtection = 0x0;

	VirtualProtect(from, 0x5, PAGE_EXECUTE_READWRITE, &DefaultProtection);

	// near jump
	*(BYTE*)from = 0xE9;

	*(DWORD*)&(((BYTE*)from)[1]) = Difference;

	VirtualProtect(from, 0x5, DefaultProtection, NULL);

	// set same protection
	VirtualProtect(memory, len, DefaultProtection, NULL);

	return memory;
}

int __stdcall DllMain(
	_In_      HINSTANCE hinstDll,
	_In_      DWORD     fdwReason,
	_In_opt_  LPVOID    lpvReserved
)
{
	DisableThreadLibraryCalls(hinstDll);
	if (fdwReason == DLL_PROCESS_ATTACH) {
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		auto base = (DWORD)GetModuleHandleA("iw5mp.exe");
		printf("base %i\n", base);
		DWORD* D3D_VT = *(DWORD**)(base + 0x61F7AA0);
		printf("D3D_VT %i\n", D3D_VT);

		o_endscene = utils::hook_vfunc<endscene_fn>((BYTE*)D3D_VT, end_scene, 42);

		o_cg_draw2d = (void(__cdecl*)(int))direct_jmp((void*)0x448770, (void*)&cg_draw_2d, 0x0);

		o_live_get_skills = (live_get_skills_fn)direct_jmp((void*)0x64EED0, (void*)&live_get_skills, 0x0);

		//o_sub_448300 = (sub_448300_fn)direct_jmp((void*)0x488390, (void*)&sub_448300, 0x0);
		//	o_writepacket = (decltype(&write_packet))direct_jmp((void*)0x48AA70, (void*)&write_packet, 0x0);
		//o_Live_Skill = (bool(__cdecl*)(int, int))direct_jmp((void*)0x0064EED0, (void*)&Live_GetSkill, 0x0);


		printf("Hooked\n");
	}
	return 1;
}

