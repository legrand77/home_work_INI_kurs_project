#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <fstream>

		template <class CharT>
		inline void ltrim(std::basic_string<CharT>& s, const std::locale& loc) {
			s.erase(s.begin(),
				std::find_if(s.begin(), s.end(),
					[&loc](CharT ch) { return !std::isspace(ch, loc); }));
		}

		template <class CharT>
		inline void rtrim(std::basic_string<CharT>& s, const std::locale& loc) {
			s.erase(std::find_if(s.rbegin(), s.rend(),
				[&loc](CharT ch) { return !std::isspace(ch, loc); }).base(),
				s.end());
		}
	

	template<class CharT>
	class Format
	{
	public:
		// used for generating
		const CharT char_section_start;
		const CharT char_section_end;
		const CharT char_assign;
		const CharT char_comment;

		// used for parsing
		virtual bool is_section_start(CharT ch) const { return ch == char_section_start; }
		virtual bool is_section_end(CharT ch) const { return ch == char_section_end; }
		virtual bool is_assign(CharT ch) const { return ch == char_assign; }
		virtual bool is_comment(CharT ch) const { return ch == char_comment; }

		// used for interpolation
		const CharT char_interpol;
		const CharT char_interpol_start;
		const CharT char_interpol_sep;
		const CharT char_interpol_end;

		Format(CharT section_start, CharT section_end, CharT assign, CharT comment, CharT interpol, CharT interpol_start, CharT interpol_sep, CharT interpol_end)
			: char_section_start(section_start)
			, char_section_end(section_end)
			, char_assign(assign)
			, char_comment(comment)
			, char_interpol(interpol)
			, char_interpol_start(interpol_start)
			, char_interpol_sep(interpol_sep)
			, char_interpol_end(interpol_end) {
		}

		Format() : Format('[', ']', '=', ';', '$', '{', ':', '}') {}

	};

	class ini_parser
	{
	    public:
		std::map<std::string, std::map<std::string, std::string>> sections;
		std::string cstring{};
		std::ifstream is;
		std::list<std::string> errors;
		std::shared_ptr<Format<char>> format;

		static const int max_interpolation_depth = 10;

		ini_parser(const std::string& s) : cstring(s), is(cstring), format(std::make_shared<Format<char>>()) {

			if (!is.is_open()) throw str_error("error open file");
			std::string line;
			std::string section;
			const std::locale loc{ "C" };
			while (std::getline(is, line)) {	// считывание строк с пробелами из файла
				ltrim(line, loc);
				rtrim(line, loc);
				const auto length = line.length();
				if (length > 0) {
					const auto pos = std::find_if(line.begin(), line.end(), [this](char ch) { return format->is_assign(ch); }); //позиция =
					const auto& front = line.front();
					if (format->is_comment(front)) {
					}
					else if (format->is_section_start(front)) {
						if (format->is_section_end(line.back())){
							section = line.substr(1, length - 2);
                             auto& sec = sections[section];
                         }
						else
							errors.push_back(line); // вывод ошибок
					}
					else if (pos != line.begin() && pos != line.end()) {
						std::string variable(line.begin(), pos);
						std::string value(pos + 1, line.end());
						rtrim(variable, loc);
						ltrim(value, loc);
						sections[section].erase(variable);
					    sections[section].emplace(variable, value);
					}
					else {
						errors.push_back(line);// вывод ошибок
					}
				}
			}

		};
		auto mget_value(const std::string& s) {
			std::string s1{},s2{};
			s1 = s2 = s;
			auto it = s.find('.');
			while (true) {
				auto it1 = s.find('.', it + 1);
				if ((it1 > it) && it1 <= s.length()) it = it1;
				else break;
			};
			s1.erase(it);
			s2.erase(0, it + 1);		
			int count{};
			for (auto& i : sections) {
				if (s1 == i.first) count++;
			}
			if (!count) {
				std::cout << "ERROR SECTION" << std::endl;
				std::cout << std::endl;
				for (auto i : sections) {
					std::cout << i.first << std::endl;

				}
				std::cout << std::endl;
			  throw str_error("not found section");
			}
			int count_1{};
			for (auto& i : sections[s1]) {

				if (s2 == i.first) count_1++;
			}
			if (!count_1){
				std::cout << "ERROR VALUE" << std::endl;
				std::cout << std::endl;
				for (auto i : sections[s1]) {
					std::cout << i.first << std::endl;

				}
				std::cout << std::endl;
				throw str_error("not found value");
			}

			auto it2 = sections[s1].find(s2);
			return it2;
		}





		template <typename T>
		T get_value(const std::string& s) {
			auto it2 = mget_value(s);
			T y{};
			std::string type = typeid(y).name();

			if (type == "int")  return  std::stoi(it2->second);
			else if (type == "float")  return std::stof(it2->second);
			else if (type == "long")  return std::stol(it2->second);
			else if (type == "__int64")  return std::stoll(it2->second);
			else if (type == "unsigned long")  return std::stoul(it2->second);
			else if (type == "unsigned __int64")  return std::stoull(it2->second);
			else if (type == "double")  return std::stod(it2->second);
			else if (type == "long double")  return std::stold(it2->second);
			else throw str_error("error type");
		}

		template <> //специализация для std::string
		    std::string get_value(const std::string & s) {
			auto it2 = mget_value(s);
			return it2->second;	
		}	

		

		class str_error : public std::exception // обработка исключений
		{
		public:
			str_error(const std::string& message) : message{ message } {}
			const char* what() const noexcept override
			{
				return message.c_str();     // получаем из std::string строку const char*
			}
		private:
			std::string message;    // сообщение об ошибке
		};
	};