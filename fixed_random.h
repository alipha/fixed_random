#ifndef FIXED_RANDOM_H
#define FIXED_RANDOM_H

#include "util.h"

#include <vector>
#include <cstddef>


class fixed_random {
public:
	fixed_random();

	template<typename Iter>
	fixed_random(Iter begin, Iter end);

	template<typename Iter>
	void set_source(Iter begin, Iter end);

	unsigned long get(unsigned long max);
	bool get(unsigned long max, unsigned long &out_value);

	std::size_t bit_count_left() const;

private:
	void reset_pos();
	bool consume_bits(unsigned int bit_count, unsigned long &out_bits);
	unsigned long consume_source_bits(unsigned int bit_count);

	unsigned long long get_5_bytes(std::size_t source_byte_pos);


	std::vector<unsigned char> source;
	std::size_t source_bit_size;
	std::size_t source_bit_pos;

	unsigned long leftover_bits;
	unsigned int leftover_bit_count;
};


template<typename Iter>
fixed_random::fixed_random(Iter begin, Iter end) {
	if(begin != end)
		util::assert_unsigned_char(*begin);

	this->set_source(begin, end);
}


template<typename Iter>
void fixed_random::set_source(Iter begin, Iter end) {
	if(begin != end)
		util::assert_unsigned_char(*begin);

	this->reset_pos();

	this->source.assign(begin, end);
	this->source_bit_size = this->source.size() << 3;

	this->source.push_back(0);	// extra room so fetches don't go off the end of the vector
	this->source.push_back(0);
	this->source.push_back(0);
	this->source.push_back(0);
}



#endif
