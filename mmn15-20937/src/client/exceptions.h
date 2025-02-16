#pragma once
/**
* @author Ilay Zvi
* @file exceptions.h
* contents:
* class - GeneralError : defines an exception sent by the server
* class - UserError : defines an exception by the user during a prompt
* 
*/
#include <stdexcept>
#include <string>

class GeneralError : public std::exception {
public:

	explicit GeneralError();

	const char* what() const noexcept override;

};

class UserError : public std::exception {
private:
	std::string _err_msg;
public:
	
	explicit UserError(const std::string& err_msg);

	const char* what() const noexcept override;
	
};