#pragma once
#include <chrono>
using namespace std::chrono;

template<class ratio>
class Timer
{
private:
	using clock_t = high_resolution_clock;
	using duration_t = duration<double, ratio >;

	time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now())
	{
	}

	void reset()
	{
		m_beg = clock_t::now();
	}

	double elapsed() const
	{
		return duration_cast<duration_t>(clock_t::now() - m_beg).count();
	}
};