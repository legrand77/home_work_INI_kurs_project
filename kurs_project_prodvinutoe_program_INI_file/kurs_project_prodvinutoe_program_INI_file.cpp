#include <iostream>
#include <fstream>
#include "Ini.h"

int main() {
	try {
	ini_parser parser("ini.ini");
	auto value = parser.get_value <long double> ("DEFAULT.Compression");
	std::cout  << value << std::endl;
	
	for (auto i : parser.errors) {
	std::cout << "ERRORS" << std::endl;
	
		std::cout << i << std::endl; // вывод списка ошибок
		
		std::cout << std::endl;
	}
	} catch (const std::exception& ex) { std::cout << ex.what() << std::endl; }
	
	return 0;
}

