#include "JSON.hpp"


JSON::JSON(std::string json_string) {
    std::regex braces_regex("(^\\s*\\{)|(\\}\\s*$)"); // Matches opening and closing brace
    json_string = std::regex_replace(json_string, braces_regex, "");

    std::smatch match;
    std::regex match_regex("\"(\\w+)\":\\s*(\\d+)");

    while(true) {
        if (std::regex_search(json_string, match, match_regex)) {
            deserialized[match[1]] = deserialize_value(match[2]);
            json_string.erase(0, json_string.find(match[0]) + match[0].length());
        }
        else
            break;
    }
}

std::experimental::any JSON::deserialize_value(std::string value) {
    if (value.find("\"") != value.npos) {
        std::regex quotation_mark_regex("\"");
        return std::experimental::any(std::regex_replace(value, quotation_mark_regex, ""));
    }
    else if (value.find("\\.") != value.npos) {
        return std::experimental::any(lexical_cast<float>(value));
    }
    else {
        return std::experimental::any(lexical_cast<int>(value));
    }
}
