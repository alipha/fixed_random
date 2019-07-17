#include "fixed_random.h"
#include "source_depleted.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <deque>
#include <map>
#include <regex>
#include <cstdlib>


typedef std::map<unsigned int, std::map<unsigned long, unsigned long>> count_map; 


void read_and_verify(fixed_random &r, unsigned long max, unsigned long expected_value, unsigned int expected_bits_left);
fixed_random init(std::string input, unsigned int base, unsigned int per_byte);
fixed_random init_bin(std::string binary);
fixed_random init_hex(std::string hex);
void run_test(const char *name, void(*func)());



void basic_test() {
	fixed_random r = init_bin("0000" "1001");
	read_and_verify(r, 10, 9, 4);
	read_and_verify(r, 10, 0, 0);
}


void rejection_test() {
	fixed_random r = init_bin("1" "100""1010");
	read_and_verify(r, 10, 4, 1);
	read_and_verify(r, 2, 1, 0);

	r = init_bin("0" "001""1011");
	read_and_verify(r, 10, 9, 1);
	read_and_verify(r, 2, 0, 0);
}


void rejection_test2() {
	fixed_random r = init_bin("11" "01""1100");
	read_and_verify(r, 10, 1, 2);
	read_and_verify(r, 4, 3, 0);

	r = init_bin("00" "01""1101");
	read_and_verify(r, 10, 5, 2);
	read_and_verify(r, 4, 0, 0);
}


void double_rejection_test() {
	fixed_random r = init_bin("10""00""1111");
	read_and_verify(r, 10, 2, 0);

	r = init_bin("0001000" "001""11""1110");
	read_and_verify(r, 10, 9, 7);
	read_and_verify(r, 100, 8, 0);
}


void large_range_test() {
    fixed_random r = init_hex("9020410f" "e503a100");
    read_and_verify(r, 0xf0123456, 0xe503a100, 32);
    read_and_verify(r, 0x20000000, 0x1020410f, 3);
    read_and_verify(r, 8, 4, 0);
}


void large_range_rejection_test() {
    fixed_random r = init_hex("00" "87654321");
    read_and_verify(r, 0x6000000, 0x5950c84, 11); 
}


void source_depleted_test() {
    fixed_random r = init_bin("11111111");
    try {
        r.get(6);
        std::cout << "Assertion failed: expected exception.\n" << std::endl;
    } catch(source_depleted) {
        std::cout << "Success" << std::endl;
    }
}


void source_depleted_test2() {
    fixed_random r = init_bin("11100000");
    unsigned long out;

    if(r.get(65, out)) {
        std::cout << "Assertion failed: expected get to return false.\n" << std::endl;
    } else {
        std::cout << "Success" << std::endl;
    }
}


void evaluate_distribution() {
    std::ifstream dev_random("/dev/urandom", std::ifstream::binary);
    count_map counts;

    for(unsigned long i = 0; i < 100000UL; i++) {
        unsigned char bytes[500];
        if(!dev_random.read(reinterpret_cast<char*>(bytes), sizeof bytes)) {
            std::cout << "Reading /dev/urandom failed.\n" << std::endl;
            return;
        }

        fixed_random r(bytes, bytes + sizeof bytes);

        for(unsigned int j = 10000; j <= 20000; j += 10000) {
            unsigned int start_bits = r.bit_count_left();
            unsigned long value = r.get(10);
            unsigned int consumed = start_bits - r.bit_count_left();
            counts[j + (consumed > 4)][value]++;
        }
    }

    dev_random.close();


    for(count_map::const_iterator it = counts.begin(); it != counts.end(); it++) {
        for(std::map<unsigned long, unsigned long>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            std::cout << it->first << "\t" << it2->first << "\t" << it2->second << std::endl;
        }
    }
}


void run_tests() {
	run_test("basic_test", basic_test);
	run_test("rejection_test", rejection_test);
	run_test("rejection_test2", rejection_test2);
	run_test("double_rejection_test", double_rejection_test);
    run_test("large_range_test", large_range_test);
    run_test("source_depleted_test", source_depleted_test);
    run_test("source_depleted_test2", source_depleted_test2);
    run_test("evaluate_distribution", evaluate_distribution);
}



void read_and_verify(fixed_random &r, unsigned long max, unsigned long expected_value, unsigned int expected_bits_left) {
	std::size_t prior_bits_left = r.bit_count_left();
	unsigned long actual_value = r.get(max);
	std::size_t actual_bits_left = r.bit_count_left();

	if (actual_value != expected_value || actual_bits_left != expected_bits_left) {
		std::cout << "Assertion failed: Prior bits left: " << prior_bits_left << std::endl;
		std::cout << "\tExpected value: " << std::hex << expected_value << std::endl;
		std::cout << "\t  Actual value: " << std::hex << actual_value << std::endl;
		std::cout << "\tExpected bits left: " << std::dec << expected_bits_left << std::endl;
		std::cout << "\t  Actual bits left: " << std::dec << actual_bits_left << std::endl << std::endl;
	}
}


fixed_random init(std::string input, unsigned int base, unsigned int per_byte) {
	//input = std::regex_replace(input, std::regex(" "), "");

	if (input.size() & (per_byte - 1)) {
		std::cout << input.size() << " is not a multiple of " << per_byte << std::endl;
		std::exit(1);
	}

	std::deque<unsigned char> bytes;

	for (size_t i = 0; i < input.size(); i += per_byte) {
		std::vector<char> byte(input.begin() + i, input.begin() + i + per_byte);
		byte.push_back('\0');

		char *end_ptr;
		bytes.push_front(static_cast<unsigned char>(std::strtoul(&byte[0], &end_ptr, base)));

		if (*end_ptr != '\0') {
			std::cout << "Invalid base-" << base << " string: " << input << std::endl;
			std::exit(1);
		}
	}

	return fixed_random(bytes.begin(), bytes.end());
}


fixed_random init_bin(std::string binary) {
	return init(binary, 2, 8);
}

fixed_random init_hex(std::string hex) {
	return init(hex, 16, 2);
}


void run_test(const char *name, void(*func)()) {
	std::cout << "Starting " << name << std::endl;
	func();
	std::cout << "Finished " << name << std::endl;
}




