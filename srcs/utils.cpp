#include "../includes/utils.hpp"

std::string to_string(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}