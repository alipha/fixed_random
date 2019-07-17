#include "fixed_random.h"
#include "source_depleted.h"
#include "util.h"

#include <algorithm>


fixed_random::fixed_random() : source(), source_bit_size(0) {
	this->reset_pos();
}


unsigned long fixed_random::get(unsigned long max) {
	unsigned long value;
	if (!get(max, value))
		throw source_depleted("Not enough source bits left for get.");
	return value;
}


bool fixed_random::get(unsigned long max, unsigned long &out_value) {
	unsigned int bit_count = util::get_bit_count(max - 1);
	unsigned long value;

	while(this->consume_bits(bit_count, value)) {
		if (value < max) {
			out_value = value;
			this->leftover_bits = 0;
			this->leftover_bit_count = 0;
			return true;
		}

		value -= max;
		unsigned long leftover_max = (util::get_bitmask(bit_count) - max + 1);
		unsigned long subrange_max = util::get_lowest_bit(leftover_max);

		while (value >= subrange_max && leftover_max > 0) {
			value -= subrange_max;
			leftover_max -= subrange_max;
			subrange_max = util::get_lowest_bit(leftover_max);
		}
		
		if (subrange_max > 1) {
			unsigned int subrange_bit_count = util::get_bit_count(subrange_max - 1);
			this->leftover_bits = value;
			this->leftover_bit_count += subrange_bit_count;
		}
	}

	return false;
}


std::size_t fixed_random::bit_count_left() const {
	return this->source_bit_size + this->leftover_bit_count - this->source_bit_pos;
}


void fixed_random::reset_pos() {
	this->source_bit_pos = 0;
	this->leftover_bits = 0;
	this->leftover_bit_count = 0;
}


bool fixed_random::consume_bits(unsigned int bit_count, unsigned long &out_bits) {
	unsigned int leftover_count = std::min(bit_count, this->leftover_bit_count);
	unsigned long value = 0;

	if (bit_count > this->bit_count_left())
		return false;

	if (leftover_count > 0) {
		value = this->leftover_bits & util::get_bitmask(leftover_count);

		this->leftover_bits >>= leftover_count;
		this->leftover_bit_count -= leftover_count;
		bit_count -= leftover_count;
	}

	if (bit_count > 0) {
		value = util::shift_left(value, bit_count);
		value |= this->consume_source_bits(bit_count);
	}

	out_bits = value;
	return true;
}


unsigned long fixed_random::consume_source_bits(unsigned int bit_count) {
	std::size_t byte_pos = this->source_bit_pos >> 3;
	unsigned int bit_pos = this->source_bit_pos & 7;

	unsigned long value = static_cast<unsigned long>(this->get_5_bytes(byte_pos) >> bit_pos);

	this->source_bit_pos += bit_count;

	return value & util::get_bitmask(bit_count);
}


unsigned long long fixed_random::get_5_bytes(std::size_t source_byte_pos) {
	std::size_t i = source_byte_pos;

#if _DEBUG
	return this->source.at(i)
		| (this->source.at(i + 1) << 8)
		| (util::implicit_cast<unsigned long>(this->source.at(i + 2)) << 16)
		| (util::implicit_cast<unsigned long>(this->source.at(i + 3)) << 24)
		| (util::implicit_cast<unsigned long long>(this->source.at(i + 4)) << 32);
#else
	return this->source[i]
		| (this->source[i + 1] << 8)
		| (util::implicit_cast<unsigned long>(this->source[i + 2]) << 16)
		| (util::implicit_cast<unsigned long>(this->source[i + 3]) << 24)
		| (util::implicit_cast<unsigned long long>(this->source[i + 4]) << 32);
#endif
}