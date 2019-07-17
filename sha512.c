#include "sha512.h"
#include <string.h>

#define BLOCK_LEN 128  // In bytes
#define STATE_LEN 8  // In words


void sha512_hash(const uint8_t message[], size_t len, uint64_t hash[]) {
	hash[0] = UINT64_C(0x6A09E667F3BCC908);
	hash[1] = UINT64_C(0xBB67AE8584CAA73B);
	hash[2] = UINT64_C(0x3C6EF372FE94F82B);
	hash[3] = UINT64_C(0xA54FF53A5F1D36F1);
	hash[4] = UINT64_C(0x510E527FADE682D1);
	hash[5] = UINT64_C(0x9B05688C2B3E6C1F);
	hash[6] = UINT64_C(0x1F83D9ABFB41BD6B);
	hash[7] = UINT64_C(0x5BE0CD19137E2179);

#define LENGTH_SIZE 16  // In bytes

	size_t off;
	for (off = 0; len - off >= BLOCK_LEN; off += BLOCK_LEN)
		sha512_compress(hash, &message[off]);

	uint8_t block[BLOCK_LEN] = { 0 };
	size_t rem = len - off;
	memcpy(block, &message[off], rem);

	block[rem] = 0x80;
	rem++;
	if (BLOCK_LEN - rem < LENGTH_SIZE) {
		sha512_compress(hash, block);
		memset(block, 0, sizeof(block));
	}

	block[BLOCK_LEN - 1] = (uint8_t)((len & 0x1FU) << 3);
	len >>= 5;
	for (int i = 1; i < LENGTH_SIZE; i++, len >>= 8)
		block[BLOCK_LEN - 1 - i] = (uint8_t)(len & 0xFFU);
	sha512_compress(hash, block);
}