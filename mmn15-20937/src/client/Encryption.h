#pragma once
/**
* @author Ilay Zvi
* @file Encryption.h
* contents:
* namespace : Encryption - defines a namespace with methods for key erasure
*
*/
#include <iostream>
#include "RSAWrapper.h"
#include "Encryption.h"
#include "Base64Wrapper.h"

namespace Encryption {
	void erase_key(std::string& key);

	void erase_key(unsigned char* key, size_t key_size);
}