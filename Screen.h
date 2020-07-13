#pragma once
#include"stdafx.h"

using namespace std::chrono;
using std::wstring;

class Screen
{
	const int screen_width;
	const int screen_height;
	const int buffer_size;
	wchar_t* buffer;

	CONSOLE_FONT_INFOEX font;
	//CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	HANDLE console_handle;
	HWND  console_HWND;

	bool SHOW_FPS_FLAG = false;

	system_clock::time_point prev_time = system_clock::now();
	system_clock::time_point cur_time;

	int FPS = 0;
public:
	Screen(int width, int height, DWORD fsize);
	~Screen();

	struct Coord
	{
		int x;
		int y;

		friend bool operator==(const Coord& c1, const Coord& c2)
		{
			return (c1.x == c2.x && c1.y == c2.y);
		}
		friend bool operator!=(const Coord& c1, const Coord& c2)
		{
			return !(c1 == c2);
		}
		friend bool operator<(const Coord& c1, const Coord& c2)
		{
			return (c1.x < c2.x&& c1.y < c2.y);
		}
		friend bool operator>(const Coord& c1, const Coord& c2)
		{
			return (c1.x > c2.x && c1.y > c2.y);
		}
	};

	HANDLE get_console_handle();

	wchar_t at(const Coord& pos);
	void set(const Coord& pos, wchar_t symbol);

	void set_color(const Coord& pos, short color);

	void set_title(wstring);

	void fastDraw();
	void draw();

	void set_SHOW_FPS_FLAG(bool state);
};

