﻿#include "stdafx.h"

#include"Screen.hpp"
#include"Food.hpp"
#include"Timer.hpp"

using namespace std::chrono;
using std::list;
using std::vector;
using std::array;
using std::wstring;
using std::to_wstring;



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


namespace vid
{
	constexpr auto screen_width{ 110 };
	constexpr auto screen_height{ 70 };
	constexpr auto font_size{ 14 };


	Screen gscreen(vid::screen_width, screen_height, font_size);

	constexpr auto status_bar_begin = vid::screen_width / 5;

	namespace look
	{
		constexpr COLOR status_bar_color_win{ FG_GREEN };
		constexpr COLOR status_bar_color_ingame{ FG_YELLOW };
		constexpr COLOR status_bar_color_fail{ FG_RED };

		constexpr auto vertival_border{ L'\u2551' };
		constexpr auto horizontal_border_thin{ L'\u2500' };
		constexpr auto horizontal_border_wide{ L'\u2550' };
		constexpr COLOR border_color{ FG_WHITE };
	}//namespace look
}//namespace vid

namespace gTypes
{

	class GEnd
	{
		bool status;
		wstring massage;
	public:
		explicit GEnd(bool st) :status{ st }
		{
			if (st)massage = L"            Congratulations,you win!!!";
			else massage = L"            You loose,press space to continue";
		}
		bool get_status()const
		{
			return status;
		}
		wstring get_massage()const
		{
			return massage;
		}
	};//class GEnd

	class Snake
	{
		list<Screen::XY> snake;
		Screen::XY head_position;
		bool state;

		const int initial_len{ 5 };

		const wchar_t h_ch{ L'@' };
		const wchar_t b_ch{ L'\u004f' };
		const COLOR b_clr{ COLOR::FG_DARK_YELLOW };
		const COLOR h_clr{ COLOR::FG_GREEN };

		const wchar_t d_h_ch{ L'%' };
		const wchar_t d_b_ch{ L'+' };
		const COLOR d_b_clr{ COLOR::FG_DARK_GREY };
		const COLOR d_h_clr{ COLOR::FG_GREY };
	public:
		Snake();
		void collision();
		void init();
		void init(short len);
		void move_head(Screen::XY pos);
		void grow_tail();
		void grow_tail(short len);
		void reduce_tail();
		Screen::XY get_head_pos()const;
		Screen::XY get_last_pos()const;
		void putToScreen();
		void set_state(bool st);
		bool get_state()const;
		void clean_trail();
	};//class Snake declaration

	namespace dFood
	{
		constexpr auto spawn_distance_x{ 20 };
		constexpr auto spawn_distance_y{ 20 };

		namespace look
		{
			constexpr auto safe_ch_1{ L'\u2126' };
			constexpr auto unsafe_ch_1{ L'X' };
			constexpr auto safe_ch_2{ L'\u03bf' };
			constexpr auto unsafe_ch_2{ L'\u00d7' };
			constexpr auto safe_clr{ FG_GREEN };
			constexpr auto unsafe_clr{ FG_RED };
		}
		namespace time
		{
			constexpr double move_delay_1{ 220.0 };
			constexpr double move_delay_2{ 50.0 };
			constexpr double state_period_1{ 2500.0 };
			constexpr double state_period_2{ 200.0 };
		}
		class DynamicFood
		{
			DynamicSmallFood* d_food_ptr;
			Snake& snake_ref;
			bool food_alive{ false };

			wchar_t safe_ch;
			wchar_t unsafe_ch;

			Timer<std::ratio<1, 1000>> move_timer;
			Timer<std::ratio<1, 1000>> state_timer;

			double move_delay;
			double state_period;

		public:
			DynamicFood(Snake& snake_rf,
				wchar_t safe_char,
				wchar_t unsafe_char,
				double m_delay,
				double s_delay);

			void move();
			void update_state();
			bool get_food_alive();
			void set_food_alive(bool st);
			void set_state(bool st);
			void clean();
			void spawn(void (*generate_func)(const gTypes::Snake&, Food*));
		};

		DynamicFood::DynamicFood(Snake& snake_rf,
			wchar_t safe_char,
			wchar_t unsafe_char,
			double m_delay,
			double s_delay)

			:snake_ref{ snake_rf },
			safe_ch{ safe_char },
			unsafe_ch{ unsafe_char },
			move_delay{ m_delay },
			state_period{ s_delay }
		{
			d_food_ptr = new DynamicSmallFood(
				vid::gscreen,
				dFood::look::safe_clr,
				safe_char
			);
			d_food_ptr->set_ch(safe_ch);
			d_food_ptr->set_state(true);
			d_food_ptr->set_clr(gTypes::dFood::look::safe_clr);

		}//class DynamicFood declaration

		//class DynamicFood definition
		void DynamicFood::move()
		{
			Screen::XY temp_pos = d_food_ptr->get_pos();
			short d_food_dir{};

			Screen::XY snake_head_pos = snake_ref.get_head_pos();

			if (d_food_ptr->get_state())
			{
				vector<Screen::XY> free_direction;

				if (snake_head_pos != snake_ref.get_head_pos())
					free_direction.push_back({ temp_pos.x + 1, temp_pos.y });

				if (vid::gscreen.at({ temp_pos.x, temp_pos.y + 1 }) == L' ')
					free_direction.push_back({ temp_pos.x, temp_pos.y + 1 });

				if (vid::gscreen.at({ temp_pos.x - 1, temp_pos.y }) == L' ')
					free_direction.push_back({ temp_pos.x - 1, temp_pos.y });

				if (vid::gscreen.at({ temp_pos.x, temp_pos.y - 1 }) == L' ')
					free_direction.push_back({ temp_pos.x, temp_pos.y - 1 });

				if (free_direction.empty())//if dynamic food is pinched
				{
					d_food_ptr->clean();
					food_alive = false;
				}
				else
				{
					if (move_timer.elapsed() > move_delay)
					{
						d_food_ptr->clean();
						d_food_dir = rand() % free_direction.size();
						d_food_ptr->putToScreen({ free_direction.at(d_food_dir) });

						move_timer.reset();
					}
				}
			}
			else
			{

				temp_pos = d_food_ptr->get_pos();

				d_food_ptr->clean();

				Screen::XY snake_head_pos = snake_ref.get_head_pos();

				if (snake_head_pos < temp_pos)
				{
					if (vid::gscreen.at({ temp_pos.x - 1,temp_pos.y - 1 }) == L' ')
						d_food_ptr->putToScreen({ temp_pos.x - 1,temp_pos.y - 1 });
					else food_alive = false;
				}
				else
					if (snake_head_pos > temp_pos)
					{
						if (vid::gscreen.at({ temp_pos.x + 1,temp_pos.y + 1 }) == L' ')
							d_food_ptr->putToScreen({ temp_pos.x + 1,temp_pos.y + 1 });
						else food_alive = false;
					}
					else
						if (snake_head_pos.x > temp_pos.x &&
							snake_head_pos.y < temp_pos.y)
						{
							if (vid::gscreen.at({ temp_pos.x + 1,temp_pos.y - 1 }) == L' ')
								d_food_ptr->putToScreen({ temp_pos.x + 1,temp_pos.y - 1 });
							else food_alive = false;
						}
						else
							if (snake_head_pos.x < temp_pos.x &&
								snake_head_pos.y > temp_pos.y)
							{
								if (vid::gscreen.at({ temp_pos.x - 1,temp_pos.y + 1 }) == L' ')
									d_food_ptr->putToScreen({ temp_pos.x - 1,temp_pos.y + 1 });
								else food_alive = false;
							}
							else
								if (snake_head_pos.x == temp_pos.x &&
									snake_head_pos.y > temp_pos.y)
								{
									if (vid::gscreen.at({ temp_pos.x,temp_pos.y + 1 }) == L' ')
										d_food_ptr->putToScreen({ temp_pos.x,temp_pos.y + 1 });
									else food_alive = false;
								}
								else
									if (snake_head_pos.x > temp_pos.x &&
										snake_head_pos.y == temp_pos.y)
									{

										if (vid::gscreen.at({ temp_pos.x + 1,temp_pos.y }) == L' ')
											d_food_ptr->putToScreen({ temp_pos.x + 1,temp_pos.y });
										else food_alive = false;
									}
									else
										if (snake_head_pos.x == temp_pos.x &&
											snake_head_pos.y < temp_pos.y)
										{
											if (vid::gscreen.at({ temp_pos.x,temp_pos.y - 1 }) == L' ')
												d_food_ptr->putToScreen({ temp_pos.x,temp_pos.y - 1 });
											else food_alive = false;
										}
										else
											if (snake_head_pos.x < temp_pos.x &&
												snake_head_pos.y == temp_pos.y)
											{
												if (vid::gscreen.at({ temp_pos.x - 1 ,temp_pos.y }) == L' ')
													d_food_ptr->putToScreen({ temp_pos.x - 1,temp_pos.y });
												else food_alive = false;
											}
											else food_alive = false;
			}
		}
		void DynamicFood::update_state()
		{
			if (state_timer.elapsed() > state_period)
			{
				if (d_food_ptr->get_state())
				{
					d_food_ptr->set_ch(unsafe_ch);
					d_food_ptr->set_state(false);
					d_food_ptr->set_clr(gTypes::dFood::look::unsafe_clr);
				}
				else
				{
					d_food_ptr->set_ch(safe_ch);
					d_food_ptr->set_state(true);
					d_food_ptr->set_clr(gTypes::dFood::look::safe_clr);
				}


				d_food_ptr->putToScreen(d_food_ptr->get_pos());
				state_timer.reset();
			}


		}
		bool DynamicFood::get_food_alive()
		{
			return food_alive;
		}
		void DynamicFood::set_food_alive(bool st)
		{
			food_alive = st;
		}
		void DynamicFood::set_state(bool st)
		{
			d_food_ptr->set_state(st);
		}
		void DynamicFood::clean()
		{
			d_food_ptr->clean();
		}
		void DynamicFood::spawn(void (*generate_func)(const gTypes::Snake&, Food*))
		{
			d_food_ptr->clean();

			generate_func(snake_ref, d_food_ptr);
			food_alive = true;

		}
		//class DynamicFood definition

	}//namespace dFood
	namespace lFood
	{
		constexpr auto spawn_distance_x{ 20 };
		constexpr auto spawn_distance_y{ 20 };
		namespace look
		{
			constexpr auto left_up_ch{ L'/' };
			constexpr auto left_down_ch{ L'\\' };
			constexpr auto right_up_ch{ L'\\' };
			constexpr auto right_down_ch{ L'/' };
		}
		namespace time
		{
			constexpr double period{ 12.0 };
		}
		class LargeFood
		{

			StaticLargeFood* l_food_ptr;
			Snake& snake_ref;
			bool food_alive;

			Timer<std::ratio<1>> timer;
			array<COLOR, 7> blink_colors;
		public:
			explicit LargeFood(Snake& snake_rf);
			void blink();
			void clean();
			void spawn(void (*generate_func)(const gTypes::Snake&, Food*));
			bool get_food_alive();
			void set_food_alive(bool st);
		};//class LargeFood declaration

		//class LargeFood definition
		LargeFood::LargeFood(gTypes::Snake& snake_rf) :snake_ref{ snake_rf },
			food_alive{ false }
		{
			blink_colors = {
				FG_BLUE,
				FG_GREEN,
				FG_CYAN,
				FG_RED,
				FG_WHITE,
				FG_YELLOW,
				FG_DARK_MAGENTA
			};

			l_food_ptr = new StaticLargeFood(
				vid::gscreen,
				blink_colors[0],
				gTypes::lFood::look::left_up_ch,
				gTypes::lFood::look::left_down_ch,
				gTypes::lFood::look::right_up_ch,
				gTypes::lFood::look::right_down_ch
			);
		}
		void LargeFood::blink()
		{
			if (timer.elapsed() < gTypes::lFood::time::period)
			{
				l_food_ptr->set_clr(
					blink_colors[rand() % blink_colors.size()]
				);
				l_food_ptr->putToScreen(l_food_ptr->get_pos());
				timer.reset();
			}
			else
			{
				food_alive = false;
				l_food_ptr->clean();
			}
			//large food blinking
		}
		void LargeFood::clean()
		{
			l_food_ptr->clean();
		}
		void LargeFood::spawn(void (*generate_func)(const gTypes::Snake&, Food*))
		{
			generate_func(snake_ref, l_food_ptr);
			food_alive = true;
			timer.reset();
		}
		bool LargeFood::get_food_alive()
		{
			return food_alive;
		}
		void LargeFood::set_food_alive(bool st)
		{
			food_alive = st;
		}
		//class LargeFood definition
	}//namespace lFood 
	namespace sFood
	{
		constexpr auto spawn_distance_x{ 20 };
		constexpr auto spawn_distance_y{ 20 };

		namespace look
		{
			constexpr auto ch{ L'0' };
			constexpr auto clr{ FG_WHITE };
		}
		class StaticFood
		{
			StaticSmallFood* s_f_ptr;
			Snake& snake_ref;
			bool alive;
		public:
			explicit StaticFood(Snake& snake_rf)
				:alive{ true },
				snake_ref{ snake_rf }
			{
				s_f_ptr = new StaticSmallFood(
					vid::gscreen,
					gTypes::sFood::look::ch,
					gTypes::sFood::look::clr);
			}
			bool get_alive()const { return alive; }
			bool set_alive(bool st) { alive = st; }
			void spawn(void (*generate_func)(const gTypes::Snake&, Food*))
			{
				generate_func(snake_ref, s_f_ptr);
				alive = true;
			}
		};//class StaticFood declaration and definition

	}//namespace sFood

	//class Snake definiton
	Snake::Snake()
	{
		head_position = { vid::screen_width / 2,vid::screen_height / 2 };
		state = true;
	}
	void Snake::collision()
	{
		//borders or tail gameover
		wchar_t head_char = vid::gscreen.at(head_position);
		if (head_char == vid::look::vertival_border ||
			head_char == vid::look::horizontal_border_thin ||
			head_char == vid::look::horizontal_border_wide ||
			head_char == b_ch ||
			head_char == gTypes::dFood::look::unsafe_ch_1 ||
			head_char == gTypes::dFood::look::unsafe_ch_2
			)state = false;
		//borders or tail gameover
	}
	void Snake::init()
	{
		head_position = { vid::screen_width / 2,vid::screen_height / 2 };
		for (int i = 0; i < initial_len; i++)
			snake.push_back({ head_position.x + i,head_position.y });
	}
	void Snake::init(short len)
	{
		head_position = { vid::screen_width / 2,vid::screen_height / 2 };
		for (int i = 0; i < len; i++)
			snake.push_back({ head_position.x + i,head_position.y });
	}
	void Snake::move_head(Screen::XY pos)
	{
		head_position = pos;
		snake.push_front(pos);
	}
	void Snake::grow_tail()
	{
		snake.push_back(snake.back());
	}
	void Snake::grow_tail(short len)
	{
		for (int i = 0; i < len; i++)grow_tail();
	}
	void Snake::reduce_tail()
	{
		assert(snake.size() > 0);

		if (state)clean_trail();
		snake.pop_back();
	}
	Screen::XY Snake::get_head_pos()const
	{
		return head_position;
	}
	Screen::XY Snake::get_last_pos()const
	{
		assert(snake.size() > 0);
		return snake.back();
	}
	void Snake::putToScreen()
	{
		assert(snake.size() > 0);

		//put snake body and head to screen buffer
		for (const auto& coord : snake)
		{
			vid::gscreen.set(coord, state ? b_ch : d_b_ch);
			vid::gscreen.set_color(coord, state ? b_clr : d_b_clr);
		}
		//put snake body and head to screen buffer

		//put snake body and head to screen buffer
		vid::gscreen.set(snake.front(), state ? h_ch : d_h_ch);
		vid::gscreen.set_color(snake.front(), state ? h_clr : d_h_clr);
		//put snake body and head to screen buffer
	}
	void Snake::set_state(bool st)
	{
		state = st;
	}
	bool Snake::get_state()const
	{
		return state;
	}
	void Snake::clean_trail()
	{
		assert(snake.size() > 0);
		vid::gscreen.set(snake.back(), L' ');
	}
	//class Snake definiton
}



void arrowKeysInput(int& max_score, double& game_speed);
direction input();

void initScreen();
void putStatusBar(const wstring& status_bar, COLOR color);

void generateSmallFood(const gTypes::Snake& snake, Food* food_obj);
void generateLargeFood(const gTypes::Snake& snake, Food* food_obj);

int wmain()
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);
	_wsetlocale(LC_ALL, NULL);


#ifdef NDEBUG
	srand(time(NULL));
#endif



	vid::gscreen.set_SHOW_FPS_FLAG(true);
	vid::gscreen.set_title(L"");

	while (true)
	{
		initScreen();

		gTypes::Snake snake;
		snake.init();


		gTypes::sFood::StaticFood s_food(
			snake
		);
		s_food.spawn(generateSmallFood);

		gTypes::dFood::DynamicFood d_food_1(
			snake,
			gTypes::dFood::look::safe_ch_1,
			gTypes::dFood::look::unsafe_ch_1,
			gTypes::dFood::time::move_delay_1,
			gTypes::dFood::time::state_period_1
		);
		gTypes::dFood::DynamicFood d_food_2(
			snake,
			gTypes::dFood::look::safe_ch_2,
			gTypes::dFood::look::unsafe_ch_2,
			gTypes::dFood::time::move_delay_2,
			gTypes::dFood::time::state_period_2
		);

		gTypes::lFood::LargeFood l_food(snake);

#ifdef NDEBUG
		d_food_1.set_food_alive(false);
		d_food_2.set_food_alive(false);
#else
		d_food_1.set_food_alive(true);
		d_food_2.set_food_alive(true);
#endif



		int game_score{};
		int max_score{ 2000 };
		double game_speed{ 110 };

		wstring status_bar = L"Score : " +
			to_wstring(game_score) + L'/' +
			to_wstring(max_score) +
			L"   Speed : move/" +
			to_wstring(static_cast<int>(game_speed)) +
			L" ms";
		putStatusBar(status_bar, vid::look::status_bar_color_ingame);

		wchar_t head_char{};


		direction dir_prev{ LEFT };
		direction temp_dir{};
		direction dir{ LEFT };

		Timer<std::ratio<1, 1000>> input_timer;
		try
		{
			while (true)
			{
				input_timer.reset();
				while (input_timer.elapsed() < game_speed)
				{
					temp_dir = input();

					if (temp_dir != NO_INPUT &&
						dir_prev != temp_dir + 2 &&
						dir_prev != temp_dir - 2)dir = temp_dir;
				}
				dir_prev = dir;

				switch (dir)
				{
				case UP:
				{
					snake.move_head({ snake.get_head_pos().x,snake.get_head_pos().y - 1 });
					break;
				}
				case DOWN:
				{
					snake.move_head({ snake.get_head_pos().x,snake.get_head_pos().y + 1 });
					break;
				}
				case RIGHT:
				{
					snake.move_head({ snake.get_head_pos().x + 1,snake.get_head_pos().y });
					break;
				}
				case LEFT:
				{
					snake.move_head({ snake.get_head_pos().x - 1,snake.get_head_pos().y });
					break;
				}
				}

				snake.collision();

				head_char = vid::gscreen.at(snake.get_head_pos());

				//if the snake ate static small food
				if (head_char == gTypes::sFood::look::ch)
				{
					game_score += 10;
					snake.grow_tail();

					s_food.spawn(generateSmallFood);

					if (!d_food_1.get_food_alive())d_food_1.spawn(generateSmallFood);
					if (!d_food_2.get_food_alive())d_food_2.spawn(generateSmallFood);
				}
				//if the snake ate static small food

				//if the snake ate dynamic small food №1
				if (head_char == gTypes::dFood::look::safe_ch_1)
				{
					game_score += 100;
					snake.grow_tail(3);

					d_food_1.clean();
					d_food_1.set_state(false);

					if (!l_food.get_food_alive())l_food.spawn(generateLargeFood);
				}
				//if the snake ate dynamic small food №1

				//if the snake ate dynamic small food №2
				if (head_char == gTypes::dFood::look::safe_ch_2)
				{
					game_score += 300;
					snake.grow_tail(3);

					d_food_2.clean();

					d_food_2.set_state(false);

					if (!l_food.get_food_alive())l_food.spawn(generateLargeFood);
				}
				//if the snake ate dynamic small food №2

				//if the snake ate large food
				if (l_food.get_food_alive() &&
					head_char == gTypes::lFood::look::left_up_ch ||
					head_char == gTypes::lFood::look::left_up_ch
					)
				{
					game_score += 500;
					snake.grow_tail(3);

					l_food.set_food_alive(false);
					l_food.clean();
				}
				//if the snake ate large food

				//status bar
				arrowKeysInput(max_score, game_speed);
				status_bar = L"Score : " +
					to_wstring(game_score) + L'/' +
					to_wstring(max_score) +
					L"   Speed : move/" +
					to_wstring(static_cast<int>(game_speed)) +
					L" ms";
				putStatusBar(status_bar, vid::look::status_bar_color_ingame);
				//status bar

				if (l_food.get_food_alive())l_food.blink();

				if (d_food_1.get_food_alive())
				{
					d_food_1.update_state();
					d_food_1.move();
				}

				if (d_food_2.get_food_alive())
				{
					d_food_2.update_state();
					d_food_2.move();
				}

				snake.collision();

				snake.putToScreen();
				snake.reduce_tail();


				vid::gscreen.fastDraw();

				if (game_score >= max_score)throw gTypes::GEnd(true);
				if (!snake.get_state())throw gTypes::GEnd(false);
			}
		}
		catch (const  gTypes::GEnd& end)
		{
			if (end.get_status())putStatusBar(end.get_massage(),
				vid::look::status_bar_color_win);
			else putStatusBar(end.get_massage(),
				vid::look::status_bar_color_fail);

			vid::gscreen.fastDraw();
		}

		while ((0x8000 & GetAsyncKeyState(unsigned char('\x20'))) == 0);
		//waiting for space
	}

	return 0;
}
void putStatusBar(const wstring& status_bar, COLOR color)
{
	//cleaning status bar
	for (int x = vid::status_bar_begin; x < vid::screen_width - 1; x++)
		vid::gscreen.set({ x,1 }, L' ');
	//cleaning status bar

	auto it = status_bar.begin();
	for (int x = vid::status_bar_begin;
		x < vid::screen_width - 1 && it != status_bar.end();
		x++, it++)
	{
		vid::gscreen.set({ x,1 }, *it);
		vid::gscreen.set_color({ x,1 }, color);
	}
}
void generateLargeFood(const gTypes::Snake& snake, Food* food_obj)
{
	vector<Screen::XY> free_cells;
	for (int y = 3; y < vid::screen_height - 4; y++)
		for (int x = 3; x < vid::screen_width - 4; x++)
			if (vid::gscreen.at({ x,y }) == L' ')free_cells.push_back({ x,y });

	if (free_cells.size() == 0)return;

	int food_cell{};
	int counter{};
	do
	{
		food_cell = rand() % free_cells.size();
		counter++;
		if (counter >= free_cells.size())break;
	} while (abs(snake.get_head_pos().x - free_cells.at(food_cell).x) < gTypes::sFood::spawn_distance_x &&
		abs(snake.get_head_pos().y - free_cells.at(food_cell).y) < gTypes::sFood::spawn_distance_y);


	food_obj->putToScreen(free_cells.at(food_cell));
}
void generateSmallFood(const gTypes::Snake& snake, Food* food_obj)
{

	vector<Screen::XY> free_cells;
	for (int y = 3; y < vid::screen_height - 1; y++)
		for (int x = 1; x < vid::screen_width - 1; x++)
			if (vid::gscreen.at({ x,y }) == L' ')free_cells.push_back({ x,y });

	if (free_cells.size() == 0)throw gTypes::GEnd(true);

	int food_cell{};
	int counter{};
	do
	{
		food_cell = rand() % free_cells.size();
		counter++;
		if (counter >= free_cells.size())break;
	} while (abs(snake.get_head_pos().x - free_cells.at(food_cell).x) < gTypes::sFood::spawn_distance_x &&
		abs(snake.get_head_pos().y - free_cells.at(food_cell).y) < gTypes::sFood::spawn_distance_y);


	food_obj->putToScreen(free_cells.at(food_cell));
}
void arrowKeysInput(int& max_score, double& game_speed)
{
	if (GetAsyncKeyState(VK_BACK))exit(EXIT_SUCCESS);

	if (GetAsyncKeyState(VK_UP))game_speed += 10.0;
	if (GetAsyncKeyState(VK_DOWN))
	{
		game_speed -= 10.0;
		if (game_speed <= 0.0)game_speed = 10.0;
	}
	if (GetAsyncKeyState(VK_LEFT))max_score -= 100;
	if (GetAsyncKeyState(VK_RIGHT))max_score += 100;
}
direction input()
{
	if (GetAsyncKeyState(0x57))return UP;
	if (GetAsyncKeyState(0x41))return LEFT;
	if (GetAsyncKeyState(0x53))return DOWN;
	if (GetAsyncKeyState(0x44))return RIGHT;

	return NO_INPUT;
}
void initScreen()
{

	//filling the whole field with spaces
	for (int y = 0; y < vid::screen_height; y++)
		for (int x = 0; x < vid::screen_width; x++)
			vid::gscreen.set({ x,y }, L' ');
	//filling the whole field with spaces

	//upper screen border
	for (int x = 0; x < vid::screen_width; x++)
	{
		vid::gscreen.set({ x,0 }, vid::look::horizontal_border_wide);
		vid::gscreen.set_color({ x,0 }, vid::look::border_color);
	}
	//upper screen border

	//left screen border
	for (int y = 0; y < vid::screen_height; y++)
	{
		vid::gscreen.set({ 0,y }, vid::look::vertival_border);
		vid::gscreen.set_color({ 0,y }, vid::look::border_color);
	}
	//left screen border

	//right screen border
	for (int y = 0; y < vid::screen_height; y++)
	{
		vid::gscreen.set({ vid::screen_width - 1,y }, vid::look::vertival_border);
		vid::gscreen.set_color({ vid::screen_width - 1,y }, vid::look::border_color);
	}
	//right screen border

	//bottom screen border
	for (int x = 0; x < vid::screen_width; x++)
	{
		vid::gscreen.set({ x,vid::screen_height - 1 }, vid::look::horizontal_border_wide);
		vid::gscreen.set_color({ x,vid::screen_height - 1 }, vid::look::border_color);
	}
	//bottom screen border

	//left upper corner
	vid::gscreen.set({ 0,0 }, L'\u2554');
	vid::gscreen.set_color({ 0,0 }, vid::look::border_color);
	//left upper corner

	//right upper corner
	vid::gscreen.set({ vid::screen_width - 1,0 }, L'\u2557');
	vid::gscreen.set_color({ vid::screen_width - 1,0 }, vid::look::border_color);
	//right upper corner

	//left bottom corner
	vid::gscreen.set({ 0,vid::screen_height - 1 }, L'\u255a');
	vid::gscreen.set_color({ 0,vid::screen_height - 1 }, vid::look::border_color);
	//left bottom corner

	//right bottom corner
	vid::gscreen.set({ vid::screen_width - 1,vid::screen_height - 1 }, L'\u255d');
	vid::gscreen.set_color({ vid::screen_width - 1,vid::screen_height - 1 }, vid::look::border_color);
	//right bottom corner

	//partition between the status bar and the playing field
	vid::gscreen.set({ 0,2 }, L'\u255f');
	vid::gscreen.set_color({ 0,2 }, vid::look::border_color);

	for (int x = 1; x < vid::screen_width - 1; x++)
	{
		vid::gscreen.set({ x,2 }, vid::look::horizontal_border_thin);
		vid::gscreen.set_color({ x,2 }, vid::look::border_color);
	}
	vid::gscreen.set({ vid::screen_width - 1,2 }, L'\u2562');
	vid::gscreen.set_color({ vid::screen_width - 1,2 }, vid::look::border_color);
	//partition between the status bar and the playing field
}
