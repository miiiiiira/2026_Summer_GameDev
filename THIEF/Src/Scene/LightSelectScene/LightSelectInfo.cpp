#include "LightSelectInfo.h"
#include "../../Application.h"

namespace LightSelectSceneWispTable
{
	const std::unordered_map<LIGHT_TYPE, std::string> Table =
	{ 
		{LIGHT_TYPE::COLOR_0, "Data/Image/LightSelectScene/Wisp00.png" },
		{LIGHT_TYPE::COLOR_1, "Data/Image/LightSelectScene/Wisp01.png" },
		{LIGHT_TYPE::COLOR_2, "Data/Image/LightSelectScene/Wisp02.png" },
		{LIGHT_TYPE::COLOR_3, "Data/Image/LightSelectScene/Wisp03.png" },
		{LIGHT_TYPE::COLOR_4, "Data/Image/LightSelectScene/Wisp04.png" },
		{LIGHT_TYPE::COLOR_5, "Data/Image/LightSelectScene/Wisp05.png" },
		{LIGHT_TYPE::COLOR_6, "Data/Image/LightSelectScene/Wisp06.png" },
		{LIGHT_TYPE::COLOR_7, "Data/Image/LightSelectScene/Wisp07.png" },
		{LIGHT_TYPE::COLOR_8, "Data/Image/LightSelectScene/Wisp08.png" },
		{LIGHT_TYPE::COLOR_9, "Data/Image/LightSelectScene/Wisp09.png" },
		{LIGHT_TYPE::COLOR_10,"Data/Image/LightSelectScene/Wisp010.png"},
		{LIGHT_TYPE::COLOR_11,"Data/Image/LightSelectScene/Wisp011.png"},
		{LIGHT_TYPE::COLOR_12,"Data/Image/LightSelectScene/Wisp012.png"},
	};
}

namespace MouseSelectTypeTable
{
	const std::unordered_map<MOUSE_SELECT_TYPE, selectTypeData> Table =
	{ 
		{MOUSE_SELECT_TYPE::RIGHT_ARROW,{"Data/Image/LightSelectScene/RightArrow.png","Data/Image/LightSelectScene/RightArrowFrame.png",{Application::SCREEN_SIZE_X / 2-124,Application::SCREEN_SIZE_Y / 2},{44,73} } },
		{MOUSE_SELECT_TYPE::LEFT_ARROW, {"Data/Image/LightSelectScene/LeftArrow.png", "Data/Image/LightSelectScene/LeftArrowFrame.png",{54, Application::SCREEN_SIZE_Y / 2},{44,73} } },
		{MOUSE_SELECT_TYPE::GAME_START, {"Data/Image/LightSelectScene/GameStart.png", "Data/Image/LightSelectScene/GameStartFrame.png",{Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y - 88},{332,58} } },
	};
}
