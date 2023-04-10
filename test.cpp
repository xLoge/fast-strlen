#include <iostream>
#include <chrono>
#include <random>
#include <cstring>

template<class TIME = std::milli>
class Timer
{
private:
	using clock = std::chrono::steady_clock;
	using duration = std::chrono::duration<double>;

public:
	static constexpr size_t m_Multiplier = TIME::den;
	const char* m_Name = "Scope";

	clock::time_point m_Start = clock::now();
	bool m_Output = true;

	Timer() = default;

	Timer(const char* name)
		: m_Name(name)
	{

	}

	~Timer()
	{
		if (m_Output) {
			this->print();
		}
	}

	void print()
	{
		const auto current = this->current();
		std::cout << m_Name << " took " << current * m_Multiplier;

#if _HAS_CXX20
		std::chrono::_Write_unit_suffix<TIME>(std::cout);
#endif
		std::cout << '\n';
	}

	void disable_output()
	{
		m_Output = false;
	}

	void reset()
	{
		m_Start = clock::now();
	}

	double current()
	{
		const duration current = clock::now() - m_Start;

		return current.count();
	}
};

template <class char_type>
constexpr inline size_t fast_strlen(const char_type* _begin)
{
	typedef unsigned long long size_t;

	constexpr size_t mask_high = static_cast<size_t>(0x8080808080808080U); // Works for X64 and X86
	constexpr size_t mask_low = static_cast<size_t>(0x0101010101010101U);
	const size_t* aligned_end = reinterpret_cast<const size_t*>(_begin);
	const char_type* end = _begin;

	// Check 8 bytes at once without simd instructions
	for (size_t data;;) {
		data = *aligned_end++;
		if ((data - mask_low) & (~data) & mask_high) {
			break;
		}
		end = reinterpret_cast<const char_type*>(aligned_end);
	}

	// Count rest one by one
	for (; *end != char_type(); ++end);

	return static_cast<size_t>(end - _begin);
}

int main()
{
	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_int_distribution<size_t> dist(14000000000, 15000000000);
	
	const auto RAND = dist(mt);

	char* cstring = new char[RAND];
	std::memset(cstring, (1 + RAND % 64), RAND);

	size_t std_res, fast_res;
	double std_time, fast_time;

	{
		Timer T("std::strlen");
		std::cout << "std::strlen" << "\n";
		std_res = std::strlen(cstring);
		std_time = T.current();
		std::cout << std_res << "\n";
		T.print();
		T.disable_output();
		std::cout << "\n";
	}

	{
		Timer T("fast_strlen");
		std::cout << "fast_strlen" << "\n";
		fast_res = fast_strlen(cstring);
		fast_time = T.current();
		std::cout << fast_res << "\n";
		T.print();
		T.disable_output();
		std::cout << "\n";
	}

	const double test_time = std_time / fast_time;

	std::cout << "fast_strlen was " << test_time << (test_time > 1.0 ? "x faster" : "x slower") << " and the results are " << (fast_res == std_res ? "equal" : "not equal") << "\n";
}
