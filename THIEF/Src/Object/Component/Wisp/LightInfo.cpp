#include "LightInfo.h"

namespace LightTable
{
	const std::unordered_map<LIGHT_TYPE, LightData> Table =
	{
		{LIGHT_TYPE::COLOR_1, {"Data/Model/Player/Light/Textures/Wisp/Wisp1.png", {0xa5,0x00,0x82}}},
		{LIGHT_TYPE::COLOR_2, {"Data/Model/Player/Light/Textures/Wisp/Wisp2.png", {0xff,0x7f,0x7f}}},
		{LIGHT_TYPE::COLOR_3, {"Data/Model/Player/Light/Textures/Wisp/Wisp3.png", {0xff,0x7f,0xff}}},
		{LIGHT_TYPE::COLOR_4, {"Data/Model/Player/Light/Textures/Wisp/Wisp4.png", {0xef,0x85,0x7d}}},
		{LIGHT_TYPE::COLOR_5, {"Data/Model/Player/Light/Textures/Wisp/Wisp5.png", {0xec,0x6d,0x51}}},
		{LIGHT_TYPE::COLOR_6, {"Data/Model/Player/Light/Textures/Wisp/Wisp6.png", {0xff,0xea,0x00}}},
		{LIGHT_TYPE::COLOR_7, {"Data/Model/Player/Light/Textures/Wisp/Wisp7.png", {0xd1,0xde,0x4c}}},
		{LIGHT_TYPE::COLOR_8, {"Data/Model/Player/Light/Textures/Wisp/Wisp8.png", {0x8e,0xff,0x1e}}},
		{LIGHT_TYPE::COLOR_9, {"Data/Model/Player/Light/Textures/Wisp/Wisp9.png", {0x7f,0xff,0xbf}}},
		{LIGHT_TYPE::COLOR_10,{"Data/Model/Player/Light/Textures/Wisp/Wisp10.png",{0x7f,0xff,0xff}}},
		{LIGHT_TYPE::COLOR_11,{"Data/Model/Player/Light/Textures/Wisp/Wisp11.png",{0x41,0x69,0xe1}}},
		{LIGHT_TYPE::COLOR_12,{"Data/Model/Player/Light/Textures/Wisp/Wisp12.png",{0xbf,0x7f,0xff}}},
	};
}