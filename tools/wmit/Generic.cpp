#include "Generic.hpp"

bool isValidWzName(const std::string name)
{
	static const std::string valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
									 "abcdefghijklmnopqrstuvwxyz"
									 "0123456789._\\-";
	std::string::size_type found = name.find_first_not_of(valid);
	if (found == std::string::npos)
	{
		return true;
	}
	return false;
}

std::vector<std::string> split(std::istringstream& iss)
{
	using namespace std;
	vector<string> tokens;
	copy(istream_iterator<string>(iss),
		 istream_iterator<string>(),
		 back_inserter<vector<string> >(tokens));
	return tokens;
}

std::vector<std::string> split(std::istringstream& iss, char delim)
{
	using namespace std;
	vector<string> tokens;
	string str;
	while (!(iss.eof()|| iss.fail()))
	{
		getline(iss, str, delim);
		tokens.push_back(str);
	}
	return tokens;
}

std::vector<std::string> split(std::string& str)
{
	std::istringstream ss(str);
	return split(ss);
}

std::vector<std::string> split(std::string& str, char delim)
{
	std::istringstream ss(str);
	return split(ss, delim);
}
