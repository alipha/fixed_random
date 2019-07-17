#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
//extern "C" {
#endif

	void sha512_hash(const uint8_t message[], size_t len, uint64_t hash[]);
	void sha512_compress(uint64_t state[], const uint8_t block[]);

#ifdef __cplusplus
//}
#endif

#endif
