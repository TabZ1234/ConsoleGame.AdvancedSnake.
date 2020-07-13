#pragma once
#include"stdafx.h"

using namespace std;
using namespace std::chrono;

enum direction
{
	NO_INPUT = 0,
	UP,
	LEFT,
	DOWN,
	RIGHT,
};
enum COLOR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_mAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};


constexpr int screen_width{ 110 };
constexpr int screen_height{ 70 };
constexpr int font_size{ 14 };

constexpr int initial_snake_len{ 5 };


constexpr auto dynamic_food_1_safe_move_delay{ 220ms };
constexpr auto dynamic_food_2_safe_move_delay{ 50ms };
constexpr auto dynamic_food_1_state_period{ 2500ms };
constexpr auto dynamic_food_2_state_period{ 200ms };

constexpr auto large_food_period{ 12s };

constexpr short food_spawn_distance_x{ 20 };
constexpr short food_spawn_distance_y{ 20 };


constexpr bool GAME_END_LOOSE{ false };
constexpr bool GAME_END_WIN{ true };


constexpr wchar_t snake_head_char{ L'@' };
constexpr wchar_t snake_body_char{ L'\u004f' };
constexpr auto snake_body_color{ FG_DARK_YELLOW };
constexpr auto snake_head_color{ FG_GREEN };

constexpr wchar_t dead_snake_head_char{ L'%' };
constexpr wchar_t dead_snake_body_char{ L'+' };
constexpr auto dead_snake_body_color{ FG_DARK_GREY };
constexpr auto dead_snake_head_color{ FG_GREY };

constexpr wchar_t static_food_char{ L'0' };
constexpr auto static_food_color{ FG_WHITE };

constexpr wchar_t dynamic_food_1_safe_char{ L'\u2126' };
constexpr wchar_t dynamic_food_1_unsafe_char{ L'X' };
constexpr wchar_t dynamic_food_2_safe_char{ L'\u03bf' };
constexpr wchar_t dynamic_food_2_unsafe_char{ L'\u00d7' };
constexpr auto dynamic_food_safe_color{ FG_GREEN };
constexpr auto dynamic_food_unsafe_color{ FG_RED };

constexpr wchar_t large_food_left_up_char{ L'/' };
constexpr wchar_t large_food_left_down_char{ L'\\' };
constexpr wchar_t large_food_right_up_char{ L'\\' };
constexpr wchar_t large_food_right_down_char{ L'/' };


constexpr wchar_t vertival_border{ L'\u2551' };
constexpr wchar_t horizontal_border_thin{ L'\u2500' };
constexpr wchar_t horizontal_border_wide{ L'\u2550' };
constexpr auto border_color{ FG_WHITE };

constexpr auto status_bar_color_win{ FG_GREEN };
constexpr auto status_bar_color_ingame{ FG_YELLOW };
constexpr auto status_bar_color_fail{ FG_RED };
constexpr auto status_bar_begin = screen_width / 5;
