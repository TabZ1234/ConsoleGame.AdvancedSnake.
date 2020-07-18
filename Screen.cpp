#include "stdafx.h"
#include "Screen.hpp"

Screen::Screen(int width, int height, DWORD fsize)
	:screen_width{ width },
	screen_height{ height },
	buffer_size{ width * height }
{
	assert(fsize > 14 || fsize < 30);

	SHORT screen_width_s = static_cast<SHORT>(screen_width);
	SHORT screen_height_s = static_cast<SHORT>(screen_height);

	buffer = new wchar_t[buffer_size];

	console_handle = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		NULL,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL);

	SetConsoleScreenBufferSize(console_handle, { screen_width_s,screen_height_s });

	console_HWND = GetConsoleWindow();


	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	font.nFont = 0;
	font.dwFontSize.X = 12;
	font.dwFontSize.Y = static_cast<SHORT>(fsize);
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;
	wcscpy_s(font.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx(console_handle, NULL, &font);


	SetConsoleMode(console_handle,
		ENABLE_PROCESSED_INPUT |
		ENABLE_WRAP_AT_EOL_OUTPUT |
		DISABLE_NEWLINE_AUTO_RETURN |
		ENABLE_LVB_GRID_WORLDWIDE
	);
	SetConsoleActiveScreenBuffer(console_handle);

	SetWindowPos(console_HWND, HWND_BOTTOM, 0, 0,
		100,
		100,
		NULL);

	SetWindowPos(console_HWND, HWND_TOP, 0, 0,
		font.dwFontSize.X* (screen_width + 2),
		font.dwFontSize.Y* (screen_height + 3),
		NULL);

}
Screen::~Screen()
{
	delete[] buffer;
}
HANDLE Screen::get_console_handle()
{
	return console_handle;
}
wchar_t Screen::at(const XY& pos)
{
	assert((screen_width * pos.y + pos.x) < buffer_size
		&& (screen_width * pos.y + pos.x) >= 0);

	return buffer[screen_width * pos.y + pos.x];
}
void Screen::set(const XY& pos, wchar_t symbol)
{
	assert((screen_width * pos.y + pos.x) < buffer_size
		&& (screen_width * pos.y + pos.x) >= 0);

	buffer[screen_width * pos.y + pos.x] = symbol;
}
void Screen::set_color(const XY& pos,const WORD color)
{
	DWORD bwritten{};
	WriteConsoleOutputAttribute(
		console_handle,
		&color,
		1,
		{ static_cast<SHORT>(pos.x),static_cast<SHORT>(pos.y) },
		&bwritten
	);
}
void Screen::set_title(const wstring& title)
{
	SetConsoleTitle(title.c_str());
}
void Screen::set_SHOW_FPS_FLAG(bool state)
{
	SHOW_FPS_FLAG = state;
}
void Screen::fastDraw()
{
	//#ifndef NDEBUG
	if (SHOW_FPS_FLAG)
	{
		if (timer.elapsed() >= 1000.0)
		{
			wstring title = L"TabZ1234 Console Snake  | FPS :" + std::to_wstring(FPS);
			SetConsoleTitle(title.c_str());
			FPS = 0;
			timer.reset();
		}
		else FPS++;
	};
	///#endif
	DWORD bytes_written{};
	WriteConsoleOutputCharacter(
		console_handle,
		buffer,
		buffer_size,
		{ 0,0 },
		&bytes_written);
}
void Screen::draw()
{
	DWORD bytes_written{};
	WriteConsole(
		console_handle,
		buffer,
		buffer_size,
		&bytes_written,
		NULL);
}
