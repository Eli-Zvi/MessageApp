#include "Encryption.h"

/*
* erases memory where key is stored after key use
*/
void Encryption::erase_key(std::string &key) {
	std::fill(key.begin(), key.end(), 0); //more type safe than memset
}

/*
* erases memory where key is stored after key use
*/
void Encryption::erase_key(unsigned char* key, size_t key_size)
{
	std::fill(key, key + key_size, 0);
}
