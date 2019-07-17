#ifndef UTIL_H
#define UTIL_H

#include <cstddef>


class util {
public:
	template<typename T>
	static void assert_unsigned_char(T);

	template<typename T, typename U>
	static T implicit_cast(U value) { return value; }

	template<typename T, std::size_t N>
	static std::size_t array_len(T(&)[N]) { return N;  }

	static unsigned int get_bit_count(unsigned long value);
	static unsigned long get_bitmask(unsigned int bit_count) { return (1UL << bit_count) - 1; }

	static unsigned long get_lowest_bit(unsigned long value) { return (value & (value - 1)) ^ value; }

	static unsigned long shift_left(unsigned long value, unsigned int amount) { return amount == 32 ? 0 : value << amount;  }
	static unsigned long shift_right(unsigned long value, unsigned int amount) { return amount == 32 ? 0 : value >> amount; }
};


template<typename T>
void util::assert_unsigned_char(T) {
	int fixed_random_constructor_expects_unsigned_char_iterator[sizeof(T) - sizeof(T)];
}

template<>
void util::assert_unsigned_char<unsigned char>(unsigned char);


#endif