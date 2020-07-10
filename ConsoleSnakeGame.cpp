#include "stdafx.h"

#include"Screen.h"
#include"Food.h"

using namespace std;
using namespace std::chrono;

constexpr int screen_width{ 110 };
constexpr int screen_height{ 70 };
constexpr int font_size{ 14 };

constexpr wchar_t snake_head_char{ L'@' };
constexpr wchar_t snake_body_char{ L'\u004f' };
constexpr wchar_t dead_snake_head_char{ L'%' };
constexpr wchar_t dead_snake_body_char{ L'+' };

constexpr wchar_t static_food_char{ L'0' };
constexpr wchar_t dynamic_food_safe_char{ L'8' };
constexpr wchar_t dynamic_food_unsafe_char{ L'X' };

constexpr wchar_t vertival_border{ L'\u2551' };
constexpr wchar_t horizontal_border_thin{ L'\u2500' };
constexpr wchar_t horizontal_border_wide{ L'\u2550' };

constexpr bool GAME_END_LOOSE{ false };
constexpr bool GAME_END_WIN{ true };

constexpr auto dynamic_food_safe_move_delay = 200ms;
constexpr auto dynamic_food_state_period = 1s;


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
	BG_DARK_MAGENTA = 0x0050,
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
constexpr auto dead_snake_body_color{ FG_DARK_GREY };
constexpr auto dead_snake_head_color{ FG_GREY };
constexpr auto snake_body_color{ FG_DARK_YELLOW };
constexpr auto snake_head_color{ FG_GREEN };

constexpr auto static_food_color{ FG_YELLOW };
constexpr auto dynamic_food_safe_color{ FG_GREEN };
constexpr auto dynamic_food_unsafe_color{ FG_RED };

constexpr auto status_bar_color_win{ FG_GREEN };
constexpr auto status_bar_color_ingame{ FG_YELLOW };
constexpr auto status_bar_color_fail{ FG_RED };

constexpr auto border_color{ FG_WHITE };


enum direction
{
	NO_INPUT = 0,
	UP,
	LEFT,
	DOWN,
	RIGHT,
};
namespace  PreviousInput
{
	bool up_key_prev{ false }, down_key_prev{ false },
		right_key_prev{ false }, left_key_prev{ false };

	direction dir_prev{};
}

void arrow_keys_input(int& max_score, milliseconds& snake_speed);
void initScreen(Screen& gscreen);
direction input(int& max_score, milliseconds& snake_speed);
void generateFood(Screen& gscreen, Food* food_obj);
void put_status_bar(Screen& gscreen, const wstring& status_bar, COLOR color);



int wmain()
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);
	_wsetlocale(LC_ALL, NULL);

#ifdef NDEBUG
	srand(time(NULL));
#endif

	Screen temp(1, 1, 1);
	temp.~Screen();

	Screen gscreen(screen_width, screen_height, font_size);
	gscreen.set_SHOW_FPS_FLAG(true);
	gscreen.set_title(L"");

	while (true)
	{
		initScreen(gscreen);

		//initial snake length
		list<Screen::Coord> snake;
		for (int i = 0; i < 5; i++)
			snake.push_back({ screen_width / 2 + i,screen_height / 2 });
		//initial snake length


		StaticSmallFood* s_sm_food_ptr = new StaticSmallFood(
			gscreen,
			static_food_char,
			static_food_color);
		generateFood(gscreen, s_sm_food_ptr);//initial food


		DynamicSmallFood* d_sm_food_ptr = new DynamicSmallFood(
			gscreen,
			dynamic_food_safe_char,
			dynamic_food_safe_color,
			dynamic_food_unsafe_color,
			dynamic_food_safe_char,
			dynamic_food_unsafe_char
		);


		//bool w_key{}, a_key{}, s_key{}, d_key{};
		direction dir{ LEFT };

		int game_score{};
		int max_score{ 1000 };

		auto snake_speed{ 100ms };

		wstring status_bar = L"Score : " +
			to_wstring(game_score) + L'/' +
			to_wstring(max_score) +
			L"   Speed : move/" +
			to_wstring(snake_speed.count()) +
			L"ms   Use arrows to change max_score/speed";
		put_status_bar(gscreen, status_bar, status_bar_color_ingame);

		bool snake_dead{};

		wchar_t head_char{};

		bool a{ false };

		direction temp_dir{};

		auto dynamic_food_move_start = system_clock::now();
		auto dynamic_food_state_start = system_clock::now();

		Screen::Coord temp_pos{};
		bool dynamic_alive{ true };
		short d_sm_food_dir{};
		vector<Screen::Coord> free_direction;

		try
		{
			while (true)
			{
				auto start_time = system_clock::now();
				while ((system_clock::now() - start_time) < snake_speed)
				{
					temp_dir = input(max_score, snake_speed);

					if (temp_dir != NO_INPUT &&
						PreviousInput::dir_prev != temp_dir + 2 &&
						PreviousInput::dir_prev != temp_dir - 2)dir = temp_dir;
				}
				PreviousInput::dir_prev = dir;
				switch (dir)
				{
				case UP:
				{
					snake.push_front({ snake.front().x,snake.front().y - 1 });
					break;
				}
				case DOWN:
				{
					snake.push_front({ snake.front().x,snake.front().y + 1 });
					break;
				}
				case RIGHT:
				{
					snake.push_front({ snake.front().x + 1,snake.front().y });
					break;
				}
				case LEFT:
				{
					snake.push_front({ snake.front().x - 1,snake.front().y });
					break;
				}
				}

				//borders or tail gameover
				head_char = gscreen.at(snake.front());
				if (head_char == vertival_border ||
					head_char == horizontal_border_thin ||
					head_char == horizontal_border_wide ||
					head_char == snake_body_char
					)snake_dead = true;
				//borders or tail gameover



				if (system_clock::now() - dynamic_food_state_start >
					dynamic_food_state_period
					&& dynamic_alive)
				{
					d_sm_food_ptr->set_state(!d_sm_food_ptr->get_state());
					d_sm_food_ptr->move(d_sm_food_ptr->get_pos());

					dynamic_food_state_start = system_clock::now();
				}

				//if the snake ate static_small_food
				if (head_char == static_food_char)
				{
					game_score += 10;
					snake.push_back(snake.back());

					generateFood(gscreen, s_sm_food_ptr);

					gscreen.set(d_sm_food_ptr->get_pos(), L' ');

					generateFood(gscreen, d_sm_food_ptr);

					dynamic_alive = true;
					d_sm_food_ptr->set_state(true);
				}
				//if the snake ate static_small_food

				//if the snake ate dynamic_small_food
				if (head_char == dynamic_food_safe_char)
				{
					game_score += 100;
					snake.push_back(snake.back());
					snake.push_back(snake.back());

					gscreen.set(d_sm_food_ptr->get_pos(), L' ');

					dynamic_alive = false;
					d_sm_food_ptr->set_state(false);
				}
				else if (head_char == dynamic_food_unsafe_char)snake_dead = true;
				//if the snake ate dynamic_small_food

				//dynamic_small_safe_food move logic
				if (dynamic_alive && d_sm_food_ptr->get_state())
				{
					temp_pos = d_sm_food_ptr->get_pos();

					if (gscreen.at({ temp_pos.x + 1, temp_pos.y }) == L' ')
						free_direction.push_back({ temp_pos.x + 1, temp_pos.y });

					if (gscreen.at({ temp_pos.x, temp_pos.y + 1 }) == L' ')
						free_direction.push_back({ temp_pos.x, temp_pos.y + 1 });

					if (gscreen.at({ temp_pos.x - 1, temp_pos.y }) == L' ')
						free_direction.push_back({ temp_pos.x - 1, temp_pos.y });

					if (gscreen.at({ temp_pos.x, temp_pos.y - 1 }) == L' ')
						free_direction.push_back({ temp_pos.x, temp_pos.y - 1 });

					if (free_direction.empty())//if dynamic food is pinched
					{
						gscreen.set(temp_pos, L' ');
						dynamic_alive = false;
					}
					else
					{
						if (system_clock::now() - dynamic_food_move_start > dynamic_food_safe_move_delay)
						{
							gscreen.set(d_sm_food_ptr->get_pos(), L' ');

							d_sm_food_dir = rand() % free_direction.size();
							d_sm_food_ptr->move({ free_direction.at(dynamic_alive) });

							dynamic_food_move_start = system_clock::now();
						}
					}
					free_direction.clear();
				}
				//dynamic_small_safe_food move logic


				//status bar
				arrow_keys_input(max_score, snake_speed);
				status_bar = L"Score : " +
					to_wstring(game_score) + L'/' +
					to_wstring(max_score) +
					L"   Speed : move/" +
					to_wstring(snake_speed.count()) +
					L"ms   Use arrows to change max_score/speed";
				put_status_bar(gscreen, status_bar, status_bar_color_ingame);
				//status bar

				//put snake body and head to screen buffer
				for (const auto& coord : snake)
				{
					gscreen.set(coord, snake_dead ? dead_snake_body_char : snake_body_char);
					gscreen.set_color(coord, snake_dead ? dead_snake_body_color : snake_body_color);
				}
				//put snake body and head to screen buffer


				//put snake body and head to screen buffer
				gscreen.set(snake.front(), snake_dead ? dead_snake_head_char : snake_head_char);
				gscreen.set_color(snake.front(), snake_dead ? dead_snake_head_color : snake_head_color);
				//put snake body and head to screen buffer

				gscreen.set(snake.back(), L' ');//cleaning snake "trail"
				snake.pop_back();

				gscreen.fastDraw();

				if (game_score >= max_score)throw GAME_END_WIN;
				if (snake_dead)throw GAME_END_LOOSE;
			}
		}
		catch (const bool game_state)
		{
			if (game_state)
			{
				status_bar = L"                  Congratulation,you WIN !!!";
				put_status_bar(gscreen, status_bar, status_bar_color_win);
				gscreen.fastDraw();
			}
			else
			{
				status_bar = L"        GameOver !!!    Press space to continue ";
				put_status_bar(gscreen, status_bar, status_bar_color_fail);
				gscreen.fastDraw();
			}
		}
		while ((0x8000 & GetAsyncKeyState(unsigned char('\x20'))) == 0);
		//waiting for space
	}

	return 0;
}

void put_status_bar(Screen& gscreen, const wstring& status_bar, COLOR color)
{

	for (int x = screen_width / 5; x < screen_width - 1; x++)
		gscreen.set({ x,1 }, L' ');

	auto it = status_bar.begin();

	for (int x = screen_width / 5;
		x < screen_width - 1 && it != status_bar.end();
		x++, it++)
	{
		gscreen.set({ x,1 }, *it);
		gscreen.set_color({ x,1 }, color);
	}
}
void generateFood(Screen& gscreen, Food* food_obj)
{

	vector<Screen::Coord> free_cells;
	for (int y = 3; y < screen_height - 1; y++)
		for (int x = 1; x < screen_width - 1; x++)
			if (gscreen.at({ x,y }) == L' ')free_cells.push_back({ x,y });

	if (free_cells.size() == 0)throw GAME_END_WIN;

	int food_cell = rand() % free_cells.size();

	food_obj->put_to_screen(free_cells.at(food_cell));
}
void arrow_keys_input(int& max_score, milliseconds& snake_speed)
{
	bool  enter_key{}, up_key{}, down_key{}, right_key{}, left_key{};

	enter_key = GetAsyncKeyState(VK_RETURN);
	if (enter_key)exit(EXIT_SUCCESS);

	up_key = GetAsyncKeyState(VK_UP);
	if (up_key && !PreviousInput::up_key_prev)snake_speed += 10ms;
	PreviousInput::up_key_prev = up_key;

	down_key = GetAsyncKeyState(VK_DOWN);
	if (down_key && !PreviousInput::down_key_prev)
	{
		snake_speed -= 10ms;
		if (snake_speed <= 0ms)snake_speed = 10ms;
	}
	PreviousInput::down_key_prev = down_key;

	right_key = GetAsyncKeyState(VK_RIGHT);
	if (right_key && !PreviousInput::right_key_prev)max_score += 100;
	PreviousInput::right_key_prev = right_key;

	left_key = GetAsyncKeyState(VK_LEFT);
	if (left_key && !PreviousInput::left_key_prev)max_score -= 100;
	PreviousInput::left_key_prev = left_key;
}
direction input(int& max_score, milliseconds& snake_speed)
{
	bool w_key{}, a_key{}, s_key{}, d_key{};

	direction dir{};

	w_key = GetAsyncKeyState(0x57);
	a_key = GetAsyncKeyState(0x41);
	s_key = GetAsyncKeyState(0x53);
	d_key = GetAsyncKeyState(0x44);

	if (w_key)dir = UP;
	else if (a_key)dir = LEFT;
	else if (s_key)dir = DOWN;
	else if (d_key)dir = RIGHT;

	return dir;
}
void initScreen(Screen& gscreen)
{

	//filling the whole field with spaces
	for (int y = 0; y < screen_height; y++)
		for (int x = 0; x < screen_width; x++)
			gscreen.set({ x,y }, L' ');
	//filling the whole field with spaces

	//upper screen border
	for (int x = 0; x < screen_width; x++)
	{
		gscreen.set({ x,0 }, horizontal_border_wide);
		gscreen.set_color({ x,0 }, border_color);
	}
	//upper screen border

	//left screen border
	for (int y = 0; y < screen_height; y++)
	{
		gscreen.set({ 0,y }, vertival_border);
		gscreen.set_color({ 0,y }, border_color);
	}
	//left screen border

	//right screen border
	for (int y = 0; y < screen_height; y++)
	{
		gscreen.set({ screen_width - 1,y }, vertival_border);
		gscreen.set_color({ screen_width - 1,y }, border_color);
	}
	//right screen border

	//bottom screen border
	for (int x = 0; x < screen_width; x++)
	{
		gscreen.set({ x,screen_height - 1 }, horizontal_border_wide);
		gscreen.set_color({ x,screen_height - 1 }, border_color);
	}
	//bottom screen border

	//left upper corner
	gscreen.set({ 0,0 }, L'\u2554');
	gscreen.set_color({ 0,0 }, border_color);
	//left upper corner

	//right upper corner
	gscreen.set({ screen_width - 1,0 }, L'\u2557');
	gscreen.set_color({ screen_width - 1,0 }, border_color);
	//right upper corner

	//left bottom corner
	gscreen.set({ 0,screen_height - 1 }, L'\u255a');
	gscreen.set_color({ 0,screen_height - 1 }, border_color);
	//left bottom corner

	//right bottom corner
	gscreen.set({ screen_width - 1,screen_height - 1 }, L'\u255d');
	gscreen.set_color({ screen_width - 1,screen_height - 1 }, border_color);
	//right bottom corner

	//partition between the status bar and the playing field
	gscreen.set({ 0,2 }, L'\u255f');
	gscreen.set_color({ 0,2 }, border_color);

	for (int x = 1; x < screen_width - 1; x++)
	{
		gscreen.set({ x,2 }, horizontal_border_thin);
		gscreen.set_color({ x,2 }, border_color);
	}
	gscreen.set({ screen_width - 1,2 }, L'\u2562');
	gscreen.set_color({ screen_width - 1,2 }, border_color);
	//partition between the status bar and the playing field
}