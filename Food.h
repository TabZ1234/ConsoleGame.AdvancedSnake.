#pragma once
#include"Screen.h"

class Food
{
protected:
	Screen& gscreen;
	short food_color;
public:
	Food(Screen& gscrn, short color) :food_color{ color }, gscreen{ gscrn }
	{

	}
	virtual void put_to_screen(Screen::Coord pos) = 0;
	virtual void clean() = 0;

	void set_food_color(short color)
	{
		food_color = color;
	}
	wchar_t get_food_color()const
	{
		return food_color;
	}
};
class StaticSmallFood : public Food
{
protected:
	Screen::Coord pos{ 7,7 };
private:
	wchar_t food_char;
public:
	StaticSmallFood(Screen& gscrn, const wchar_t f_ch, short clr) :Food(gscrn, clr), food_char{ f_ch }
	{

	}
	void put_to_screen(Screen::Coord new_pos)override
	{
		pos = new_pos;
		gscreen.set({ new_pos.x,new_pos.y }, food_char);
		gscreen.set_color({ new_pos.x,new_pos.y }, food_color);
	}
	void clean()override
	{
		gscreen.set(pos, L' ');
	}
};
class DynamicSmallFood : public StaticSmallFood
{
	bool state;
	short safe_color;
	short unsafe_color;

	wchar_t safe_char;
	wchar_t unsafe_char;

	wchar_t active_char;
public:
	DynamicSmallFood(Screen& gscrn,
		short safe_clr,
		short unsafe_clr,
		wchar_t s_char,
		wchar_t uns_char)
		:StaticSmallFood(gscrn, s_char, safe_clr),
		safe_color{ safe_clr },
		unsafe_color{ unsafe_clr },
		safe_char{ s_char },
		unsafe_char{ uns_char },
		state{ false }
	{
		set_food_color(unsafe_color);
	}
	void put_to_screen(Screen::Coord new_pos)override
	{
		pos = new_pos;
		gscreen.set({ pos.x,pos.y }, [&]()->wchar_t
			{
				if (state == true)return safe_char;
				else return unsafe_char;
			}());
		gscreen.set_color({ pos.x,pos.y }, food_color);
	}
	void move(Screen::Coord new_pos)
	{
		put_to_screen(new_pos);
		pos = new_pos;
	}
	bool get_state()const
	{
		return state;
	}
	Screen::Coord get_pos()const
	{
		return pos;
	}
	void set_state(bool st)
	{
		state = st;
		if (state == true)
		{
			set_food_color(safe_color);
			active_char = safe_char;
		}
		else
		{
			set_food_color(unsafe_color);
			active_char = unsafe_char;
		}
	}
	void clean()override
	{
		gscreen.set(pos, L' ');
	}
};
class StaticLargeFood :public Food
{
	wchar_t left_up_char;
	wchar_t left_down_char;
	wchar_t right_up_char;
	wchar_t right_down_char;
	Screen::Coord left_up_position;
public:
	StaticLargeFood(Screen &gscreen,
		short food_clr,
		wchar_t left_up_ch,
		wchar_t left_down_ch,
		wchar_t right_up_ch,
		wchar_t right_down_ch
	):Food(gscreen,food_clr),
		left_up_char{ left_up_ch },
		left_down_char{ left_down_ch },
		right_up_char{ right_up_ch },
		right_down_char{ right_down_ch }
	{

	}
	void put_to_screen(Screen::Coord left_up_pos)override
	{
		left_up_position = left_up_pos;

		gscreen.set(left_up_pos, left_up_char);
		gscreen.set({ left_up_pos.x,left_up_pos.y + 1 }, left_down_char);
		gscreen.set({ left_up_pos.x + 1,left_up_pos.y }, right_up_char);
		gscreen.set({ left_up_pos.x + 1,left_up_pos.y + 1 }, right_down_char);

		gscreen.set_color(left_up_pos, food_color);
		gscreen.set_color({ left_up_pos.x,left_up_pos.y + 1 }, food_color);
		gscreen.set_color({ left_up_pos.x + 1,left_up_pos.y }, food_color); 
		gscreen.set_color({ left_up_pos.x + 1,left_up_pos.y + 1 }, food_color);
	}
	Screen::Coord get_pos()
	{
		return left_up_position;
	}
	void clean()override
	{
		gscreen.set(left_up_position, L' ');
		gscreen.set({ left_up_position.x + 1,left_up_position.y }, L' ');
		gscreen.set({ left_up_position.x,left_up_position.y + 1 }, L' ');
		gscreen.set({ left_up_position.x + 1,left_up_position.y + 1 }, L' ');
	}
};

