#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <regex>
#include <unordered_map>
#include <experimental/any>
#include "debug.h"

class JSON {
    private:
        std::unordered_map <std::string, std::experimental::any> deserialized;
        std::experimental::any deserialize_value(std::string value);

        /* This is probably slow as hell */
        template <typename T>
        T lexical_cast(const std::string& str) {
            T var;
            std::istringstream iss;
            iss.str(str);
            iss >> var;
            // Ignore any error bits that may have been set on the stream - YOLO
            return var;
        }
    public:
        JSON(std::string json_string);
        JSON() {};
        template <typename T>
        T get(std::string index) {return std::experimental::any_cast<T>(deserialized[index]);};
        bool is_empty() const {return deserialized.size() == 0;}
};

#endif // JSON_HPP
