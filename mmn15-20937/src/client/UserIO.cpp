#include "UserIO.h"

UserCommand UserIO::validate_string(int input) {
    switch (input) {
    case 110: case 120: case 130: case 140:case 150: case 151: case 152:case 0:
        return UserCommand(input); 
    default:
        return UserCommand::INVALID_COMMAND;
    }
}

types::username UserIO::get_username_input(UserCommand code){
    std::string prompt = "Please enter a username ";
    types::username username = {0};
    std::string input;

    if (code == UserCommand::REGISTRATION) {
        prompt += "to register with";
    }else if(code == UserCommand::PUBLIC_KEY_REQUEST) {
        prompt += "to get the public key of";
    }
    else if (code == UserCommand::SEND_TXT_MSG || code == UserCommand::SEND_SYMMETRIC_KEY || code == UserCommand::REQUEST_SYMMETRIC_KEY) {
        prompt += "to send a message to";
    }
    else {
        throw std::invalid_argument("internal error has occured, request code does not require username input");
    }

    std::cout << prompt << ": ";
    std::getline(std::cin, input);

    if (input.size() <= constants::USERNAME_LENGTH) //check for maximum username size being exceeded
        std::copy(input.begin(), input.end(), username.begin());
    else throw UserError("Username exceeds maximum size of 254");

    return username;
}

Message UserIO::get_message_input(types::uuid id, MessageType type, Contact& c){
    if (type == MessageType::SYMMETRIC_KEY_REQUEST_TYPE) {
        return Message(type, constants::EMPTY_PAYLOAD);
    }
    else if (type == MessageType::SYMMETRIC_KEY_SEND_TYPE) { 
        //does not check if the user already has a sym key because he could have lost it when exiting

        std::string rsa_pub = c.get_rsa_pub(); //get user's pub key

        if (rsa_pub.size() != RSAPublicWrapper::KEYSIZE) { //ensures proper key size or that it even exists
            throw UserError("Contact does not have a registered public key");
        }

        unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
        std::string symmetric_key(reinterpret_cast<const char*>(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH)), 
            AESWrapper::DEFAULT_KEYLENGTH); //create sym key and reinterpret to const char from unsigned char* to match string constructor

        c.set_symmetric_key(symmetric_key); //store symmetric key in contact

        RSAPublicWrapper rsapub(rsa_pub); //create rsa public key

        std::string cipher = rsapub.encrypt((const char*)key, AESWrapper::DEFAULT_KEYLENGTH); //encrypt symmetrical key with public rsa key

        Encryption::erase_key(symmetric_key); //erase aes key from memory
        Encryption::erase_key(key, AESWrapper::DEFAULT_KEYLENGTH); //erase aes key from memory

        return Message(type, cipher.size(), cipher); //return encrypted rsa public key
    }
    else if (type == MessageType::MESSAGE_SEND_TYPE) {
        std::string symmetrical_key = c.get_symmetric_key();

        if (symmetrical_key.size() != AESWrapper::DEFAULT_KEYLENGTH) { //ensures proper key size or that it even exists
            throw UserError("Contact does not have a registered symmetrical key");
        }

        std::string plaintext = "";
        std::cout << "Please type the message that will be sent to the user: ";
        std::getline(std::cin, plaintext); //get message from user

        unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
        std::copy(symmetrical_key.begin(), symmetrical_key.end(), key); //copy key into unsigned char

        AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH); //create aes key

        std::string ciphertext = aes.encrypt(plaintext.c_str(), plaintext.length()); //encrypt user message

        Encryption::erase_key(key, AESWrapper::DEFAULT_KEYLENGTH); //erase aes key

        return Message(type, ciphertext.size(), ciphertext); //return encrypted user message
    }
    else {
        throw std::invalid_argument("internal error has occured, message type is invalid");
    }
}

UserCommand UserIO::get_user_input() {
    std::cout << "MessageU client at your service.\n"
        << "110) Register\n"
        << "120) Request for clients list\n"
        << "130) Request for public key\n"
        << "140) Request for waiting messages\n"
        << "150) Send a text message\n"
        << "151) Send a request for symmetric key\n"
        << "152) Send your symmetric key\n"
        << "0) Exit client\n";

    std::string input;
    std::getline(std::cin, input);
    input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end()); // removes all whitespace from input

    if (input.size() == 3 || input.size() == 1) {
        try {
            int code = stoi(input);

            return validate_string(code);
        }
        catch (const std::invalid_argument&) {
            std::cout << "Error: given non integer.\n";
        }
        catch (const std::out_of_range&) {
            std::cout << "Error: Integer out of range.\n";
        }
    }

    return UserCommand::INVALID_COMMAND;
}