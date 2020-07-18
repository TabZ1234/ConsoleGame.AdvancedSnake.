#pragma once
#include"Screen.hpp"

class Food
{
protected:
	Screen& gscreen;
	short clr;
	Screen::XY pos{ 7,7 };
public:
	Food(Screen& gscrn, short color) :clr{ color }, gscreen{ gscrn }
	{

	}
	virtual void putToScreen(Screen::XY pos) = 0;
	virtual void putToScreen() = 0;
	virtual void clean() = 0;

	virtual void set_clr(short color) = 0;
	virtual short get_clr()const = 0;
};
class StaticSmallFood : public Food
{
private:
	wchar_t ch;
public:
	StaticSmallFood(Screen& gscrn, const wchar_t chr, short clr)
		:Food(gscrn, clr), ch{ chr }
	{

	}
	void putToScreen(Screen::XY new_pos)override
	{
		pos = new_pos;
		gscreen.set({ pos.x,pos.y }, ch);
		gscreen.set_color({ pos.x,pos.y }, clr);
	}
	void putToScreen()override
	{
		gscreen.set({ pos.x,pos.y }, ch);
		gscreen.set_color({ pos.x,pos.y }, clr);
	}
	void clean()override { gscreen.set(pos, L' '); }

	void set_clr(short color)override { clr = color; }
	short get_clr()const override { return clr; }
};
class DynamicSmallFood : public Food
{
	bool state;
	wchar_t ch;
public:
	DynamicSmallFood(Screen& gscrn,
		wchar_t chr,
		short clr)
		:Food(gscrn, clr),
		ch{ chr },
		state{ true }

	{

	}
	void putToScreen(Screen::XY new_pos)override
	{
		pos = new_pos;
		gscreen.set({ pos.x,pos.y }, ch);
		gscreen.set_color({ pos.x,pos.y }, clr);
	}
	void putToScreen()override
	{
		gscreen.set({ pos.x,pos.y }, ch);
		gscreen.set_color({ pos.x,pos.y }, clr);
	}
	bool get_state()const { return state; }
	void set_state(bool st) { state = st; }

	void set_ch(wchar_t chr) { ch = chr; }

	Screen::XY get_pos()const { return pos; }

	void clean()override { gscreen.set(pos, L' '); }

	void set_clr(short color)override { clr = color; }
	short get_clr()const override { return clr; }
};
class StaticLargeFood :public Food
{
	wchar_t left_up_char;
	wchar_t left_down_char;
	wchar_t right_up_char;
	wchar_t right_down_char;
public:
	StaticLargeFood(Screen& gscreen,
		short food_clr,
		wchar_t left_up_ch,
		wchar_t left_down_ch,
		wchar_t right_up_ch,
		wchar_t right_down_ch
	) :Food(gscreen, food_clr),
		left_up_char{ left_up_ch },
		left_down_char{ left_down_ch },
		right_up_char{ right_up_ch },
		right_down_char{ right_down_ch }
	{

	}
	void putToScreen(Screen::XY left_up_pos)override
	{
		pos = left_up_pos;

		gscreen.set(pos, left_up_char);
		gscreen.set({ pos.x,pos.y + 1 }, left_down_char);
		gscreen.set({ pos.x + 1,pos.y }, right_up_char);
		gscreen.set({ pos.x + 1,pos.y + 1 }, right_down_char);

		set_clr(clr);
	}
	void putToScreen()override
	{

		gscreen.set(pos, left_up_char);
		gscreen.set({ pos.x,pos.y + 1 }, left_down_char);
		gscreen.set({ pos.x + 1,pos.y }, right_up_char);
		gscreen.set({ pos.x + 1,pos.y + 1 }, right_down_char);

		set_clr(clr);
	}
	void clean()override
	{
		gscreen.set(pos, L' ');
		gscreen.set({ pos.x + 1,pos.y }, L' ');
		gscreen.set({ pos.x,pos.y + 1 }, L' ');
		gscreen.set({ pos.x + 1,pos.y + 1 }, L' ');
	}
	virtual void set_clr(short color)override
	{
		clr = color;

		gscreen.set_color(pos, clr);
		gscreen.set_color({ pos.x,pos.y + 1 }, clr);
		gscreen.set_color({ pos.x + 1,pos.y }, clr);
		gscreen.set_color({ pos.x + 1,pos.y + 1 }, clr);
	}
	virtual short get_clr()const override { return clr; }

	Screen::XY get_pos() { return pos; }
};

