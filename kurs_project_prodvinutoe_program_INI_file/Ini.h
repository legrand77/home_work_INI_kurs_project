#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <string>
#include <fstream>


 void ltrim(std::string& s, const std::locale& loc) {
    s.erase(s.begin(),
    std::find_if(s.begin(), s.end(),
    [&loc](char ch) { return !std::isspace(ch, loc); }));
  }


 void rtrim(std::string& s, const std::locale& loc) {
     s.erase(std::find_if(s.rbegin(), s.rend(),
     [&loc](char ch) { return !std::isspace(ch, loc); }).base(),
     s.end());
  }	    
    

class ini_parser{
   public:
     std::map<std::string, std::map<std::string, std::string>> sections;
     std::string cstring{};
     std::ifstream is;
     std::list<std::string> errors;

     ini_parser(const std::string& s) : cstring(s), is(cstring) {
        if (!is.is_open()) throw str_error("error open file");
        std::string line;
        std::string section;
        const std::locale loc{ "C" };
         while (std::getline(is, line)) {	// считывание строк с пробелами из файла
              ltrim(line, loc);
              rtrim(line, loc);
              const auto length = line.length();
              if (length > 0) {
                  const auto pos = line.find('=');
                    if (line[0]== ';') {
                    }
                    else if (line[0] == '[') {
                        if (line[length - 1] == ']'){
                            section = line.substr(1, length - 2);
                         }
                        else
                            errors.push_back(line); // вывод ошибок
                    }
                    else if ((line[0] != '=' && '=' != line[length-1])) {
                        std::string variable{}, value{};
                        variable = value = line;     
                        variable.erase(pos);
                        value.erase(0, pos + 1);
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

     auto mget_value(const std::string& s){
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
            return  std::stoi(it2->second);
     }
     template <> //специализация для std::string
     std::string get_value(const std::string & s) {
        auto it2 = mget_value(s);
        return it2->second;	
     }	
     template <> //специализация для float
     float get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stof(it2->second);
     }
     template <> //специализация для long
     long get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stol(it2->second);
     }
     template <> //специализация для long long
     __int64 get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stoll(it2->second);
     }
     template <> //специализация для unsigned long
     unsigned long get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stoul(it2->second);
     }
     template <> //специализация для unsigned __int64
     unsigned __int64 get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stoull(it2->second);
     }
     template <> //специализация для double
     double get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stod(it2->second);
     }
     template <> //специализация для long double
     long double get_value(const std::string& s) {
         auto it2 = mget_value(s);
         return std::stold(it2->second);
     }
  class str_error : public std::exception{
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