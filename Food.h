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
	wchar_t food_char;
public:
	StaticSmallFood(Screen& gscrn, const wchar_t f_ch, short clr) :Food(gscrn, clr), food_char{ f_ch }
	{

	}
	void put_to_screen(Screen::Coord pos)override
	{
		gscreen.set({ pos.x,pos.y }, food_char);
		gscreen.set_color({ pos.x,pos.y }, food_color);
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
	Screen::Coord pos{ 6,6 };
public:
	DynamicSmallFood(Screen& gscrn,
		const wchar_t f_ch,
		short safe_clr,
		short unsafe_clr,
		wchar_t s_char,
		wchar_t uns_char)
		:StaticSmallFood(gscrn, f_ch, safe_clr),
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
	bool get_state()
	{
		return state;
	}
	Screen::Coord get_pos()
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
};

