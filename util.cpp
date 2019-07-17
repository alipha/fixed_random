#include "util.h"


const unsigned long long_max = static_cast<unsigned long>((1ULL << 32) - 1);
const unsigned long long_half_max = 1UL << 31;


template<>
void util::assert_unsigned_char<unsigned char>(unsigned char) {}


unsigned int util::get_bit_count(unsigned long value) {
	unsigned int bit_count;
	unsigned long power_of_2 = 1;

	for (bit_count = 0; bit_count < 32 && value >= power_of_2; bit_count++)
		power_of_2 <<= 1;

	return bit_count;
}
