#include "stdafx.h"

#include"Screen.h"
#include"Food.h"

#include"constants.h"

namespace vid
{
	Screen gscreen(screen_width, screen_height, font_size);
}

namespace GameTypes
{
	class Snake
	{
		list<Screen::Coord> snake;
		Screen::Coord head_position;
		bool state;
	public:
		Snake()
		{
			head_position = { screen_width / 2,screen_height / 2 };
			state = true;
		}
		void collision()
		{
			//borders or tail gameover
			wchar_t head_char = vid::gscreen.at(head_position);
			if (head_char == vertival_border ||
				head_char == horizontal_border_thin ||
				head_char == horizontal_border_wide ||
				head_char == snake_body_char ||
				head_char == dynamic_food_1_unsafe_char ||
				head_char == dynamic_food_2_unsafe_char
				)state = false;
			//borders or tail gameover
		}
		void init(short len)
		{
			head_position = { screen_width / 2,screen_height / 2 };
			for (int i = 0; i < len; i++)
				snake.push_back({ head_position.x + i,head_position.y });
		}
		void move_head(Screen::Coord pos)
		{
			head_position = pos;
			snake.push_front(pos);
		}
		void grow_tail()
		{
			snake.push_back(snake.back());
		}
		void grow_tail(short len)
		{
			for (int i = 0; i < len; i++)grow_tail();
		}
		void reduce_tail()
		{
			assert(snake.size() > 0);

			if (state)clean_trail();
			snake.pop_back();
		}
		Screen::Coord get_head_pos()const
		{
			return head_position;
		}
		Screen::Coord get_last_pos()const
		{
			assert(snake.size() > 0);
			return snake.back();
		}
		void put_to_screen()
		{
			assert(snake.size() > 0);

			//put snake body and head to screen buffer
			for (const auto& coord : snake)
			{
				vid::gscreen.set(coord, state ? snake_body_char : dead_snake_body_char);
				vid::gscreen.set_color(coord, state ? snake_body_color : dead_snake_body_color);
			}
			//put snake body and head to screen buffer

			//put snake body and head to screen buffer
			vid::gscreen.set(snake.front(), state ? snake_head_char : dead_snake_head_char);
			vid::gscreen.set_color(snake.front(), state ? snake_head_color : dead_snake_head_color);
			//put snake body and head to screen buffer
		}
		void set_state(bool st)
		{
			state = st;
		}
		bool get_state()const
		{
			return state;
		}
		void clean_trail()
		{
			assert(snake.size() > 0);
			vid::gscreen.set(snake.back(), L' ');
		}
	};

	class DynamicFood
	{
		DynamicSmallFood* d_food_ptr;
		Snake& snake_ref;
		bool food_alive{ false };

		time_point<system_clock>  dynamic_food_state_start = system_clock::now();
		time_point<system_clock>  dynamic_food_move_start = system_clock::now();

		milliseconds dynamic_food_move_delay;
		milliseconds dynamic_food_state_period;

	public:
		DynamicFood(Snake& snake_rf,
			wchar_t dynamic_food_safe_char,
			wchar_t dynamic_food_unsafe_char,
			milliseconds d_fd_move_delay,
			milliseconds d_fd_state_period
		) :snake_ref{ snake_rf },
			dynamic_food_move_delay{ d_fd_move_delay },
			dynamic_food_state_period{ d_fd_state_period }
		{
			d_food_ptr = new DynamicSmallFood(
				vid::gscreen,
				dynamic_food_safe_color,
				dynamic_food_unsafe_color,
				dynamic_food_safe_char,
				dynamic_food_unsafe_char
			);

		}
		void move()
		{
			Screen::Coord temp_pos = d_food_ptr->get_pos();
			short d_food_dir{};

			Screen::Coord snake_head_pos = snake_ref.get_head_pos();
			//dynamic_small_food_ptr_2 move logic

			if (d_food_ptr->get_state())
			{
				vector<Screen::Coord> free_direction;

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
					if (system_clock::now() - dynamic_food_move_start >
						dynamic_food_move_delay)
					{
						d_food_ptr->clean();
						d_food_dir = rand() % free_direction.size();
						d_food_ptr->move({ free_direction.at(d_food_dir) });

						dynamic_food_move_start = system_clock::now();
					}
				}
			}
			else
			{

				temp_pos = d_food_ptr->get_pos();

				d_food_ptr->clean();

				Screen::Coord snake_head_pos = snake_ref.get_head_pos();

				if (snake_head_pos < temp_pos)
				{
					if (vid::gscreen.at({ temp_pos.x - 1,temp_pos.y - 1 }) == L' ')
						d_food_ptr->move({ temp_pos.x - 1,temp_pos.y - 1 });
					else food_alive = false;
					//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
				}
				else
					if (snake_head_pos > temp_pos)
					{
						if (vid::gscreen.at({ temp_pos.x + 1,temp_pos.y + 1 }) == L' ')
							d_food_ptr->move({ temp_pos.x + 1,temp_pos.y + 1 });
						else food_alive = false;
						//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
					}
					else
						if (snake_head_pos.x > temp_pos.x &&
							snake_head_pos.y < temp_pos.y)
						{
							if (vid::gscreen.at({ temp_pos.x + 1,temp_pos.y - 1 }) == L' ')
								d_food_ptr->move({ temp_pos.x + 1,temp_pos.y - 1 });
							else food_alive = false;
							//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
						}
						else
							if (snake_head_pos.x < temp_pos.x &&
								snake_head_pos.y > temp_pos.y)
							{
								if (vid::gscreen.at({ temp_pos.x - 1,temp_pos.y + 1 }) == L' ')
									d_food_ptr->move({ temp_pos.x - 1,temp_pos.y + 1 });
								else food_alive = false;
								//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
							}
							else
								if (snake_head_pos.x == temp_pos.x &&
									snake_head_pos.y > temp_pos.y)
								{
									if (vid::gscreen.at({ temp_pos.x,temp_pos.y + 1 }) == L' ')
										d_food_ptr->move({ temp_pos.x,temp_pos.y + 1 });
									else food_alive = false;
									//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
								}
								else
									if (snake_head_pos.x > temp_pos.x &&
										snake_head_pos.y == temp_pos.y)
									{

										if (vid::gscreen.at({ temp_pos.x + 1,temp_pos.y }) == L' ')
											d_food_ptr->move({ temp_pos.x + 1,temp_pos.y });
										else food_alive = false;
										//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
									}
									else
										if (snake_head_pos.x == temp_pos.x &&
											snake_head_pos.y < temp_pos.y)
										{
											if (vid::gscreen.at({ temp_pos.x,temp_pos.y - 1 }) == L' ')
												d_food_ptr->move({ temp_pos.x,temp_pos.y - 1 });
											else food_alive = false;
											//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
										}
										else
											if (snake_head_pos.x < temp_pos.x &&
												snake_head_pos.y == temp_pos.y)
											{
												if (vid::gscreen.at({ temp_pos.x - 1 ,temp_pos.y }) == L' ')
													d_food_ptr->move({ temp_pos.x - 1,temp_pos.y });
												else food_alive = false;
												//if (d_food_ptr->get_pos() == snake_head_pos)snake_ref.set_state(false);
											}
											else food_alive = false;
			}
			//dynamic_small_food_ptr_2 move logic
		}
		void update_state()
		{
			//dynamic_food_1 state logic
			if (system_clock::now() - dynamic_food_state_start >
				dynamic_food_state_period && food_alive)
			{
				d_food_ptr->set_state(!d_food_ptr->get_state());
				d_food_ptr->put_to_screen(d_food_ptr->get_pos());
				dynamic_food_state_start = system_clock::now();
			}
			//dynamic_food_1 state logic

		}
		bool get_food_alive()
		{
			return food_alive;
		}
		void set_food_alive(bool st)
		{
			food_alive = st;
		}
		void set_state(bool st)
		{
			d_food_ptr->set_state(st);
		}
		void clean()
		{
			d_food_ptr->clean();
		}
		void spawn(void (*generate_func)(const GameTypes::Snake&, Food*))
		{
			d_food_ptr->clean();

			generate_func(snake_ref, d_food_ptr);
			food_alive = true;

		}
	};

	class LargeFood
	{

		StaticLargeFood* l_food_ptr;
		Snake& snake_ref;
		bool l_food_alive;

		time_point<system_clock> large_food_start = system_clock::now();

		array<COLOR, 7> large_food_blink_colors;
	public:
		explicit LargeFood(Snake& snake_rf) :snake_ref{ snake_rf },
			l_food_alive{ false }
		{
			large_food_blink_colors = {
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
				large_food_blink_colors[rand() % large_food_blink_colors.size()],
				large_food_left_up_char,
				large_food_left_down_char,
				large_food_right_up_char,
				large_food_right_down_char
			);
		}
		void blink()
		{
			if (system_clock::now() - large_food_start <
				large_food_period)
			{
				l_food_ptr->set_food_color(
					large_food_blink_colors[rand() % large_food_blink_colors.size()]
				);
				l_food_ptr->put_to_screen(l_food_ptr->get_pos());
				large_food_start = system_clock::now();
			}
			else
			{
				l_food_alive = false;
				l_food_ptr->clean();
			}
			//large food blinking
		}
		void clean()
		{
			l_food_ptr->clean();
		}
		void spawn(void (*generate_func)(const GameTypes::Snake&, Food*))
		{
			generate_func(snake_ref, l_food_ptr);
			l_food_alive = true;
			large_food_start = system_clock::now();
		}
		bool get_l_food_alive()
		{
			return l_food_alive;
		}
		void set_l_food_alive(bool st)
		{
			l_food_alive = st;
		}
	};
}

void arrowKeysInput(int& max_score, milliseconds& snake_speed);
direction input();

void initScreen();
void putStatusBar(const wstring& status_bar, COLOR color);

void generateFood(const GameTypes::Snake& snake, Food* food_obj);




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

		GameTypes::Snake snake;
		snake.init(initial_snake_len);

		StaticSmallFood* s_food_ptr = new StaticSmallFood(
			vid::gscreen,
			static_food_char,
			static_food_color);
		generateFood(snake, s_food_ptr);

		GameTypes::DynamicFood d_food_1(
			snake,
			dynamic_food_1_safe_char,
			dynamic_food_1_unsafe_char,
			dynamic_food_1_safe_move_delay,
			dynamic_food_1_state_period
		);
		GameTypes::DynamicFood d_food_2(
			snake,
			dynamic_food_2_safe_char,
			dynamic_food_2_unsafe_char,
			dynamic_food_2_safe_move_delay,
			dynamic_food_2_state_period
		);

		GameTypes::LargeFood l_food(snake);

#ifdef NDEBUG
		d_food_1.set_food_alive(false);
		d_food_2.set_food_alive(false);
#else
		d_food_1.set_food_alive(true);
		d_food_2.set_food_alive(true);
#endif



		int game_score{};
		int max_score{ 2000 };
		auto snake_speed{ 110ms };

		wstring status_bar = L"Score : " +
			to_wstring(game_score) + L'/' +
			to_wstring(max_score) +
			L"   Speed : move/" +
			to_wstring(snake_speed.count()) +
			L" ms";
		putStatusBar(status_bar, status_bar_color_ingame);

		wchar_t head_char{};


		direction dir_prev{ LEFT };
		direction temp_dir{};
		direction dir{ LEFT };


		try
		{
			while (true)
			{
				auto start_time = system_clock::now();
				while ((system_clock::now() - start_time) < snake_speed)
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
				//if the snake ate static_small_food_ptr
				if (head_char == static_food_char)
				{
					game_score += 10;
					snake.grow_tail();

					generateFood(snake, s_food_ptr);

					if (!d_food_1.get_food_alive())d_food_1.spawn(generateFood);
					if (!d_food_2.get_food_alive())d_food_2.spawn(generateFood);
				}
				//if the snake ate static_small_food_ptr

				//if the snake ate dynamic_small_food_ptr_1
				if (head_char == dynamic_food_1_safe_char)
				{
					game_score += 100;
					snake.grow_tail(3);

					d_food_1.clean();
					d_food_1.set_state(false);

					if (!l_food.get_l_food_alive())l_food.spawn(generateFood);
				}//if the snake ate dynamic_small_food_ptr_1
				else if (head_char == dynamic_food_2_safe_char)//if the snake ate dynamic_small_food_ptr_2
				{
					game_score += 300;
					snake.grow_tail(3);

					d_food_2.clean();

					d_food_2.set_state(false);

					if (!l_food.get_l_food_alive())l_food.spawn(generateFood);
				}
				//if the snake ate dynamic_small_food_ptr_2

				//if the snake ate large food
				if (l_food.get_l_food_alive() &&
					head_char == large_food_left_up_char ||
					head_char == large_food_left_down_char
					)
				{
					game_score += 500;
					snake.grow_tail(3);

					l_food.set_l_food_alive(false);
					l_food.clean();
				}
				//if the snake ate large food

				//status bar
				arrowKeysInput(max_score, snake_speed);
				status_bar = L"Score : " +
					to_wstring(game_score) + L'/' +
					to_wstring(max_score) +
					L"   Speed : move/" +
					to_wstring(snake_speed.count()) +
					L" ms";
				putStatusBar(status_bar, status_bar_color_ingame);
				//status bar

				if (l_food.get_l_food_alive())l_food.blink();

				d_food_1.update_state();
				d_food_2.update_state();

				if (d_food_1.get_food_alive())d_food_1.move();
				if (d_food_2.get_food_alive())d_food_2.move();

				snake.collision();

				snake.put_to_screen();
				snake.reduce_tail();


				vid::gscreen.fastDraw();

				if (game_score >= max_score)throw GAME_END_WIN;
				if (!snake.get_state())throw GAME_END_LOOSE;
			}
		}
		catch (const bool game_status)
		{
			if (game_status)
			{
				status_bar = L"                  Congratulations,you WIN !!!";
				putStatusBar(status_bar, status_bar_color_win);
				vid::gscreen.fastDraw();
			}
			else
			{
				status_bar = L"        GameOver !!!    Press space to continue ";
				putStatusBar(status_bar, status_bar_color_fail);
				vid::gscreen.fastDraw();
			}
		}

		while ((0x8000 & GetAsyncKeyState(unsigned char('\x20'))) == 0);//waiting for space
	}

	return 0;
}
void putStatusBar(const wstring& status_bar, COLOR color)
{
	//cleaning status bar
	for (int x = status_bar_begin; x < screen_width - 1; x++)
		vid::gscreen.set({ x,1 }, L' ');
	//cleaning status bar

	auto it = status_bar.begin();
	for (int x = status_bar_begin;
		x < screen_width - 1 && it != status_bar.end();
		x++, it++)
	{
		vid::gscreen.set({ x,1 }, *it);
		vid::gscreen.set_color({ x,1 }, color);
	}
}
void generateFood(const GameTypes::Snake& snake, Food* food_obj)
{

	vector<Screen::Coord> free_cells;
	for (int y = 3; y < screen_height - 1; y++)
		for (int x = 1; x < screen_width - 1; x++)
			if (vid::gscreen.at({ x,y }) == L' ')free_cells.push_back({ x,y });

	if (free_cells.size() == 0)throw GAME_END_WIN;

	if (typeid(food_obj) == typeid(StaticLargeFood*))
	{
		vector<Screen::Coord>::iterator it;
		for (it = free_cells.begin(); it != free_cells.end(); it++)
			if ((*it).x >= screen_width - 2 ||
				(*it).y >= screen_height - 2 ||
				vid::gscreen.at({ (*it).x + 1,(*it).y }) ||
				vid::gscreen.at({ (*it).x,(*it).y + 1 }) ||
				vid::gscreen.at({ (*it).x + 1,(*it).y + 1 })
				)
				free_cells.erase(it);
	}

	int food_cell{};
	int counter{};
	do
	{
		food_cell = rand() % free_cells.size();
		counter++;
		if (counter >= free_cells.size())break;
	} while (abs(snake.get_head_pos().x - free_cells.at(food_cell).x) < food_spawn_distance_x &&
		abs(snake.get_head_pos().y - free_cells.at(food_cell).y) < food_spawn_distance_y);


	food_obj->put_to_screen(free_cells.at(food_cell));
}
void arrowKeysInput(int& max_score, milliseconds& snake_speed)
{
	if (GetAsyncKeyState(VK_BACK))exit(EXIT_SUCCESS);

	if (GetAsyncKeyState(VK_UP))snake_speed += 10ms;
	if (GetAsyncKeyState(VK_DOWN))
	{
		snake_speed -= 10ms;
		if (snake_speed <= 0ms)snake_speed = 10ms;
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
	for (int y = 0; y < screen_height; y++)
		for (int x = 0; x < screen_width; x++)
			vid::gscreen.set({ x,y }, L' ');
	//filling the whole field with spaces

	//upper screen border
	for (int x = 0; x < screen_width; x++)
	{
		vid::gscreen.set({ x,0 }, horizontal_border_wide);
		vid::gscreen.set_color({ x,0 }, border_color);
	}
	//upper screen border

	//left screen border
	for (int y = 0; y < screen_height; y++)
	{
		vid::gscreen.set({ 0,y }, vertival_border);
		vid::gscreen.set_color({ 0,y }, border_color);
	}
	//left screen border

	//right screen border
	for (int y = 0; y < screen_height; y++)
	{
		vid::gscreen.set({ screen_width - 1,y }, vertival_border);
		vid::gscreen.set_color({ screen_width - 1,y }, border_color);
	}
	//right screen border

	//bottom screen border
	for (int x = 0; x < screen_width; x++)
	{
		vid::gscreen.set({ x,screen_height - 1 }, horizontal_border_wide);
		vid::gscreen.set_color({ x,screen_height - 1 }, border_color);
	}
	//bottom screen border

	//left upper corner
	vid::gscreen.set({ 0,0 }, L'\u2554');
	vid::gscreen.set_color({ 0,0 }, border_color);
	//left upper corner

	//right upper corner
	vid::gscreen.set({ screen_width - 1,0 }, L'\u2557');
	vid::gscreen.set_color({ screen_width - 1,0 }, border_color);
	//right upper corner

	//left bottom corner
	vid::gscreen.set({ 0,screen_height - 1 }, L'\u255a');
	vid::gscreen.set_color({ 0,screen_height - 1 }, border_color);
	//left bottom corner

	//right bottom corner
	vid::gscreen.set({ screen_width - 1,screen_height - 1 }, L'\u255d');
	vid::gscreen.set_color({ screen_width - 1,screen_height - 1 }, border_color);
	//right bottom corner

	//partition between the status bar and the playing field
	vid::gscreen.set({ 0,2 }, L'\u255f');
	vid::gscreen.set_color({ 0,2 }, border_color);

	for (int x = 1; x < screen_width - 1; x++)
	{
		vid::gscreen.set({ x,2 }, horizontal_border_thin);
		vid::gscreen.set_color({ x,2 }, border_color);
	}
	vid::gscreen.set({ screen_width - 1,2 }, L'\u2562');
	vid::gscreen.set_color({ screen_width - 1,2 }, border_color);
	//partition between the status bar and the playing field
}