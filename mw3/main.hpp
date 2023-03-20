#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h> 
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <memory>
#include <assert.h>
#include <intrin.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <winerror.h>
#pragma warning( disable : 4091)
#include <ShlObj.h>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "Winmm.lib")
#pragma warning( disable : 4244)
#pragma warning( disable : 4405)


#define get_a(x) ((x>>24)&255)
#define get_r(x) ((x>>16)&255)
#define get_g(x) ((x>>12)&255)
#define get_b(x) (x&255)


using endscene_fn = long(__stdcall*)(IDirect3DDevice9 * device);
using eng_fn = bool(__stdcall*)( int );
static endscene_fn o_endscene;
static eng_fn o_eng;
static LPD3DXFONT tahoma_12 = NULL;
static LPD3DXFONT tahoma_10 = NULL;
static ID3DXLine* d3line = NULL;
static bool visuals_on = false;
static std::mutex esp_mutex;

namespace vars
{
	namespace aimbot
	{
		static bool enabled = false;
		static bool vischeck = true;
		static float fov = 80.f;
	}
	namespace esp
	{
		static bool enabled = false;
		static bool draw_skeleton = false;
		static D3DCOLOR visible_col = D3DCOLOR_RGBA(0, 255, 0, 255);
		static D3DCOLOR unvisibile_col = D3DCOLOR_RGBA(255, 0, 0, 255);
	}

	namespace menu
	{
		static bool force_host = false;
		static bool noclip = false;
		static float menu_x = 20;
		static float menu_y = 50;
		static float menu_alpha = 255;
		static float weapon_fov = 68.f;
		static float weapon_x = 0.f;
		static float weapon_y = 0.f;
		static float weapon_z = 0.f;
	}
}

struct box_t
{
	float x;
	float y;
};

//std::vector<box_t> draw_que;

#include "utils.hpp"


class vector_3
{
public:
	vector_3(void) { x = y = z = 0.0f; }
	vector_3(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	vector_3(float* v) { x = v[0]; y = v[1]; z = v[2]; }
	vector_3(const float* v) { x = v[0]; y = v[1]; z = v[2]; }
	vector_3(const vector_3& v) { x = v.x; y = v.y; z = v.z; }

	//vector_3(float rgfl[3]) { x = rgfl[0]; y = rgfl[1]; z = rgfl[2]; }

	vector_3 operator-(const vector_3& v) const
	{
		return vector_3(x - v.x, y - v.y, z - v.z);
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}
	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	vector_3 operator+(const vector_3& v) const
	{
		return vector_3(x + v.x, y + v.y, z + v.z);
	}

	float Length(void) const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float get_distance(float x2, float y2) {
		return sqrtf(pow(x2 - x, 2.0f) + pow(y2 - y, 2.0f));
	}

	//-----------------------------------------------------------------------------
	// Get the distance from this vector to the other one 
	//-----------------------------------------------------------------------------
	float DistTo(const vector_3& vOther) const
	{
		vector_3 delta;
		delta = *this - vOther;
		return delta.Length();
	}


	bool IsZero(void) const {
		return (x > -0.01f && x < 0.01f &&
			y > -0.01f && y < 0.01f &&
			z > -0.01f && z < 0.01f);
	}

public:
	float x;
	float y;
	float z;
};

class vector_2
{
public:
	float x;
	float y;
};

class centity_t	//UPDATED
{
public:
	char _0x0000[0x2];                            //0x0
	short is_valid;                                  //0x2 (0xAE59C2)
	char _0x0004[0x10];                           //0x4
	vector_3 origin;                                //0x14 (0xAE59D4)
	vector_3 angles;                                //0x20 (0xAE59E0)
	char _0x002C[0x3C];                           //0x2C
	int flags;                                    //0x68 (0xAE5A28)
	char _0x006C[0xC];                            //0x6C
	vector_3 old_origin;                             //0x78 (0xAE5A38)
	char _0x0084[0x18];                           //0x84
	vector_3 old_angles;                             //0x9C (0xAE5A5C)
	char _0x00A8[0x28];                           //0xA8
	int client_num;                                //0xD0 To Find: https://i.imgur.com/isGfgAQ.png
	short type;                                   //0xD4 (0xAE5A94)
	char _0x00D6[0x12];                           //0xD6
	vector_3 new_origin;                             //0xE8 (0xAE5AA8)
	char _0x00F4[0x1C];                           //0xF4
	vector_3 new_angles;                             //0x110 (0xAE5AD0)
	char _0x011C[0x7C];                           //0x11C
	byte weapon_id;                                //0x198 (0xAE5B58)
	char _0x0199[0x37];                           //0x199
	int is_alive;                                  //0x1D0 (0xAE5B90)
	char _0x01D4[0x24];                           //0x1D4
};//Size=0x01F8

class CClientInfo //UPDATED
{
public:
	int valid;                                    //0x0 (0xAD9A78)
	char _0x0004[0x8];                            //0x4
	char name[16];                                //0xC (0xAD9A84)
	int team;                                     //0x1C (0xAD9A94)
	char _0x0020[0x480];                          //0x20
	int attacking;                                //0x4A0 (0xAD9F18)
	char _0x04A4[0x4];                            //0x4A4
	int zooming;                                  //0x4A8 (0xAD9F20)
	char _0x04AC[0xB8];                           //0x4AC
};//Size=0x0560	

class CG_t
{
public:
	int frame_time;                                //0x0 (0x9DC3A0)
	char _0x0004[0xC];                            //0x4
	int is_in_game;                                 //0x10 (0x9DC3B0)
	char _0x0014[0x13C];                          //0x14
	int client_num;                                //0x150 (0x9DC4F0)
	char _0x0154[0x48];                           //0x154
	int health;                                   //0x19C (0x9DC53C)
	char _0x01A0[0x64];                           //0x1A0
	int advanced_uav;                              //0x204 (0x9DC5A4)
	char _0x0208[0x38];                           //0x208
	int next_attack;                               //0x240 (0x9DC5E0)
};//Size=0x0160

class matrix_3x3_t
{
public:
	vector_3 a, b, c;
};

static bool world_to_screen(const vector_3& world, vector_3& screen)
{
	vector_3 DaWelr = world;
	vector_2 Da2d;
	bool wat = ((bool(*)(int, void*, vector_3*, vector_2*))0x44ADC0)(0,
		// ((void*(*)())0x4B7610)()
		(void*)0x133BAC8
		, &DaWelr, &Da2d);
	if (wat)
	{
		screen = vector_3(Da2d.x, Da2d.y, 0.f);
		return true;
	}
	return false;
}


typedef unsigned short(__cdecl* register_tag_fn)(const char*);
static register_tag_fn our_register_tag = (register_tag_fn)0x517400;


//char *__cdecl sub_556A30(int a1)
typedef void*(__cdecl* tag_ptr_rn)(int);
static tag_ptr_rn tag_ptr = (tag_ptr_rn)0x556A30;

typedef bool(__cdecl* get_tag_pos_internal_fn)(centity_t*, void*, WORD, vector_3*);
static get_tag_pos_internal_fn get_tag_pos_internal = (get_tag_pos_internal_fn)0x44D9D0;

typedef void(__cdecl* cbuf_addtext_fn)(int, const char*);
static cbuf_addtext_fn cbuf_addtext = (cbuf_addtext_fn)0x00545680;


static bool get_bone_pos(centity_t* pEnt, const char* bone, vector_3* out) {

	WORD bone_id = our_register_tag(bone);

	auto tag = tag_ptr(pEnt->client_num);
	if (tag == nullptr)
		return false;

	return get_tag_pos_internal(pEnt, tag, bone_id, out);
}

static void get_tag_pos(centity_t* ent, WORD bone, vector_3* out)
{
	DWORD ngiga = 0x410840;
	__asm {
		push out
		movzx edi, bone
		mov esi, ent
		call ngiga
		add esp, 0x4
	}
}


static void aim_at_world_position(vector_3 end_pos)
{
	vector_3 screen;
	if (!world_to_screen(end_pos, screen))
		return;

	float center_x = 1920 / 2.f;
	float center_y = 1080 / 2.f;

	mouse_event(MOUSEEVENTF_MOVE, (screen.x - center_x), (screen.y - center_y), 0, 0);
}


class CViewMatrix
{
public:
	vector_3 Recoil; //0x0000
	vector_3 Origin; //0x000C
	char pad_0018[128]; //0x0018
	vector_3 ViewAngle; //0x0098
}; //Size: 0x00A4

struct draw_queue_t
{
	int type = -1;
	int x = 0.f, y = 0.f, w = 0.f, h = 0.f;
	std::string text;
	D3DCOLOR col;
};
static std::vector<draw_queue_t> draw_queue = {};
#define M_PI		3.14159265358979323846f	// matches value in gcc v2 math.h
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
//Math copypasta, credit: rawrimatiger


class Refdef_t //UPDATED
{
public:
	char _0x0000[0x8];                            //0x0
	int Width;                                    //0x8 (0xA475B8)
	int Height;                                   //0xC (0xA475BC)
	float FovX;                                     //0x10 (0xA475C0)
	float FovY;                                     //0x14 (0xA475C4)
	vector_3 ViewOrg;                               //0x18 (0xA475C8)
	vector_3 ViewAxis[3];                           //0x24 (0xA475D4)
	char _0x0048[0x24];                           //0x48
	float ZoomProgress;                           //0x6C (0xA4761C)
	char _0x0070[0x4ABC];                         //0x70
	float RefViewAngleY;                          //0x4B2C (0xA4C0DC)
	float RefViewAngleX;                          //0x4B30 (0xA4C0E0)
	char _0x4B34[0x34];                           //0x4B34
	float WeaponViewAngleX;                       //0x4B68 (0xA4C118)
	float WeaponViewAngleY;                       //0x4B6C (0xA4C11C)
};
#define	PITCH					0		// up / down
#define	YAW						1		// left / right
#define	ROLL					2
static void vectoangles(const vector_3 value1, vector_3& angles)
{
	float	forward;
	float	yaw, pitch;

	if (value1[1] == 0 && value1[0] == 0)
	{
		yaw = 0;
		if (value1[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		if (value1[0])
			yaw = (atan2(value1[1], value1[0]) * (180.0f / M_PI));
		else if (value1[1] > 0)
			yaw = 90.0f;
		else
			yaw = 270.0f;

		if (yaw < 0.0f)
			yaw += 360.0f;

		forward = sqrt((value1[0] * value1[0]) + (value1[1] * value1[1]));
		pitch = (atan2(value1[2], forward) * (180.0f / M_PI));

		if (pitch < 0.0f)
			pitch += 360;
	}

	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

static void GetAngleToOrigin(vector_3 vOrigin, float& flOutX, float& flOutY)
{
	auto refdef = ((Refdef_t*)0x96C1B0);
	vector_3 vEntity, vAngle;
	VectorSubtract(vOrigin, refdef->ViewOrg, vEntity);
	vectoangles(vEntity, vAngle);

	if (vAngle[1] > 180.0f)
		vAngle[1] -= 360.0f;
	else if (vAngle[1] < -180.0f)
		vAngle[1] += 360.0f;

	if (vAngle[0] > 180.0f)
		vAngle[0] -= 360.0f;
	else if (vAngle[0] < -180.0f)
		vAngle[0] += 360.0f;

	//float* RefAngles = (float*)0xA4C0DC;oldold
	//float* RefAngles = (float*)0xA534EC; //Weaponviewangles anstatt Refdef vAngles verwenden (is einfach akkurater xD)
	vector_3 RefAngles = vector_3(refdef->RefViewAngleY, refdef->RefViewAngleX, 0.f); //Weaponviewangles anstatt Refdef vAngles verwenden (is einfach akkurater xD)


	vAngle[1] -= RefAngles[1];
	vAngle[0] -= RefAngles[0];
	//auto xx = *(float*)0x1065C84;// += AimAt[0];
	//auto yy = *(float*)0x1065C88;

	//vAngle[1] -= yy;
	//vAngle[0] -= xx;

	if (vAngle[0] > 180.0f)
		vAngle[0] -= 360.0f;
	else if (vAngle[0] < -180.0f)
		vAngle[0] += 360.0f;

	if (vAngle[1] > 180.0f)
		vAngle[1] -= 360.0f;
	else if (vAngle[1] < -180.0f)
		vAngle[1] += 360.0f;

	flOutX = vAngle[1];
	flOutY = vAngle[0];
}


typedef struct
{
    float fraction; //0x0000 
    char _0x0004[60];
    DWORD materialType; //0x0040 
}trace_t;//Size=0x0044

typedef void(__cdecl* process_cg_trace_capsule)(trace_t* traceResults, const float* start, const float* end, int passEntityNum, int contentMask);
static process_cg_trace_capsule cg_trace_capsule = (process_cg_trace_capsule)0x00483980;

static bool is_visible(const vector_3& Startt, const vector_3& Endd)
{

	trace_t t;

	//CFloat3 null_vec = { 0,0,0 }; 
	//OurCG_Trace(&t, Startt, Endd, null_vec, MW3::Game->ClientNum, 0x803003);
	vector_3 starttt = Startt;
	vector_3 endddd = Endd;
	cg_trace_capsule(&t, (float*)&starttt, (float*)&endddd, ((CG_t*)0x900FA0)->client_num, 0x803003);

	return (t.fraction > 0.87f);
}

template <class Value>
static void write_ptr(DWORD pointer, DWORD pointerofs, Value value)
{

	DWORD dwPointer = *(DWORD*)pointer;
	*(Value*)(dwPointer + pointerofs) = value;

}


typedef struct {
	int servertime; //0-4
	int buttons; //4-8
	int viewangles[3]; //8-14
	char _0x14[0x8];
	char forwardmove; //1c
	char rightmove; //1d
	char upmove; //1e
	char _0x1F[0xD];
}usercmd_t;
#define	CMD_BACKUP 0x7F
typedef struct
{
	usercmd_t usercmds[128];
	int currentCmdNum; // 0x16E8

	usercmd_t* get_user_cmd(int cmdNum)
	{
		if ((cmdNum & CMD_BACKUP) > 127)
			return nullptr;

		return &usercmds[cmdNum & CMD_BACKUP];
	}
}input_t;

static void aimbot()
{

	auto EntityList = (centity_t*)0xA0A5C0;
	auto ClientInfo = (CClientInfo*)0x9FE678;
	auto Game = (CG_t*)0x900FA0;
	if (EntityList && ClientInfo && Game) {

		((CViewMatrix*)0x01065BEC)->Recoil = vector_3(0.f, 0.f, 0.f);

		vector_3 targetAngles;
		vector_3 targetHead = vector_3(0, 0, 0);;
		float max_fov = vars::aimbot::fov;
		int m_best_index = -1;
		auto viworg = ((Refdef_t*)0x96C1B0)->ViewOrg;
		for (int Index = 0; Index < 64; Index++)
		{
			auto entity = EntityList[Index];
			if (!ClientInfo[Index].valid)
				continue;
			if (entity.type != 1)
				continue;
			if (entity.origin.IsZero())
				continue;
			if (!(entity.is_alive & 1))
				continue;
			if (ClientInfo[Game->client_num].team == ClientInfo[Index].team)
				continue;

			vector_3 head = vector_3(0, 0, 0);
			get_bone_pos(&entity, "j_head", &head);
			if (!head.IsZero()) {
				auto ref = ((Refdef_t*)0x96C1B0);
				vector_3 w2s;
				if (world_to_screen(head, w2s)) {
					vector_3 center = { float(ref->Width / 2), float(ref->Height / 2), 0.f };
					vector_3 w2sDelta = { w2s.x - center.x, w2s.y - center.y, 0.f };
					float m_fov = center.get_distance(w2s.x, w2s.y);
					if (m_fov < max_fov) {
						max_fov = m_fov;
						m_best_index = Index;
						targetHead = head;
					}
				}
			}
		}
		if (m_best_index != -1 && !targetHead.IsZero()) {
			float cx, cy;
			GetAngleToOrigin(targetHead, cx, cy);
			if (vars::aimbot::vischeck ? is_visible(viworg, targetHead) : true)
			{
				if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)) {

					*(float*)0x1065C88 += cx;
					*(float*)0x1065C84 += cy;
				}
			}
		}
	}
}



static void esp_loop() {
	auto EntityList = (centity_t*)0xA0A5C0;
	auto ClientInfo = (CClientInfo*)0x9FE678;
	auto Game = (CG_t*)0x900FA0;
	if (EntityList && ClientInfo && Game) {


		vector_3 targetAngles;
		vector_3 targetHead = vector_3(0, 0, 0);;
		float dist = 11111.0f;
		auto viworg = ((Refdef_t*)0x96C1B0)->ViewOrg;
		for (int Index = 0; Index < 64; Index++)
		{
			auto entity = EntityList[Index];
			if (!ClientInfo[Index].valid)
				continue;
			if (entity.type != 1)
				continue;
			if (entity.origin.IsZero())
				continue;
			if (!(entity.is_alive & 1))
				continue;
			if (ClientInfo[Game->client_num].team == ClientInfo[Index].team)
				continue;





			static const char* bone_positions[][2] = {
			  { "j_head", "j_neck" },              
			  { "j_neck", "j_spineupper" },       
			  { "j_spineupper", "j_shoulder_le" },
			  { "j_spineupper", "j_shoulder_ri" },
			  { "j_shoulder_ri", "j_elbow_ri" },
			  { "j_shoulder_le", "j_elbow_le" },
			  { "j_elbow_le", "j_wrist_le" },
			  { "j_elbow_ri", "j_wrist_ri" },
			 // { "j_spineupper", "j_hip_le" },
			 // { "j_spineupper", "j_hip_ri" },
			  { "j_spineupper", "j_spinelower" },
			   { "j_spinelower", "j_hip_le" },
			   { "j_spinelower", "j_hip_ri" },
			   { "j_hip_le", "j_knee_le" },
			   { "j_hip_ri", "j_knee_ri" },
			   { "j_knee_ri", "j_ankle_ri" },
			   { "j_knee_le", "j_ankle_le" },
			   { "j_spinelower", "j_shoulder_ri" },
			   { "j_neck", "j_shoulder_ri" }
			};

			if (vars::esp::draw_skeleton) {
				for (int i = 0; i < 17; ++i)
				{
					vector_3 bone1(0.f, 0.f, 0.f);
					vector_3 bone2(0.f, 0.f, 0.f);
					if (!get_bone_pos(&entity, bone_positions[i][0], &bone1))
						continue;
					if (!get_bone_pos(&entity, bone_positions[i][1], &bone2))
						continue;
					if (bone1.IsZero() || bone2.IsZero())
						continue;


					vector_3 b2d;
					vector_3 b2d2;
					if (!world_to_screen(bone1, b2d))
						continue;

					if (!world_to_screen(bone2, b2d2))
						continue;


					auto q = draw_queue_t{ 0, (int)b2d.x, (int)b2d.y, (int)b2d2.x, (int)b2d2.y };
					q.col = is_visible(viworg, bone1) ? vars::esp::visible_col : vars::esp::unvisibile_col;
					draw_queue.emplace_back(q);

				}
			}
		}
	}
}