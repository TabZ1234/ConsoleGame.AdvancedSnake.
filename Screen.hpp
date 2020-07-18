#pragma once
#include"stdafx.h"

#include"Timer.hpp"

using namespace std::chrono;
using std::wstring;

class Screen
{
	const int screen_width;
	const int screen_height;
	const int buffer_size;
	wchar_t* buffer;

	CONSOLE_FONT_INFOEX font;
	HANDLE console_handle;
	HWND  console_HWND;

	bool SHOW_FPS_FLAG{ false };

	Timer<std::ratio<1,1000>> timer;

	int FPS{};
public:
	Screen(int width, int height, DWORD fsize);
	~Screen();

	struct XY
	{
		int x;
		int y;

		friend bool operator==(const XY& c1, const XY& c2)
		{
			return (c1.x == c2.x && c1.y == c2.y);
		}
		friend bool operator!=(const XY& c1, const XY& c2)
		{
			return !(c1 == c2);
		}
		friend bool operator<(const XY& c1, const XY& c2)
		{
			return (c1.x < c2.x&& c1.y < c2.y);
		}
		friend bool operator>(const XY& c1, const XY& c2)
		{
			return (c1.x > c2.x && c1.y > c2.y);
		}
	};


	HANDLE get_console_handle();

	wchar_t at(const XY& pos);
	void set(const XY& pos, wchar_t symbol);

	void set_color(const XY& pos, const WORD color);

	void set_title(const wstring&);

	void fastDraw();
	void draw();

	void set_SHOW_FPS_FLAG(bool state);
};

