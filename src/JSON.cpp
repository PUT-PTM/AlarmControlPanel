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
        return std::experimental::any(std::stof(value));
    }
    else {
        return std::experimental::any(std::stoi(value));
    }
}

std::string JSON::get_representation(std::experimental::any value) {
    std::string value_type_name = value.type().name();

    if (value_type_name == "i") {
        return std::to_string(std::experimental::any_cast<int>(value));
    }
    else if (value_type_name == "Ss") {
        std::string result = "\"";
        result += std::experimental::any_cast<std::string>(value);
        result += "\"";
        return result;
    }
    else if (value_type_name == "PKc") {
        std::string result = "\"";
        result += std::experimental::any_cast<const char *>(value);
        result += "\"";
        return result;
    }
    else if (value_type_name == "d") {
        return std::to_string(std::experimental::any_cast<double>(value));
    }
    else if (value_type_name == "f") {
        return std::to_string(std::experimental::any_cast<float>(value));
    }
    else {
        debug("TYPE: %s\n", value_type_name.c_str());
        return "";
    }
}

std::string JSON::serialize() {
    size_t number_of_objects = inserted_objects.size();

    std::string serialized;
    if (number_of_objects > 1) {
        serialized += "[";
    }

    for (size_t i = 0; i < number_of_objects; i++) {
        serialized += "{\n";

        for (size_t j = inserted_objects[i].begin; j <= inserted_objects[i].end; j++) {
            serialized += "\"";
            serialized += inserted_indexes[j];
            serialized += "\": ";
            serialized += get_representation(inserted_values[j]);

            if (inserted_objects[i].end - j != 0) {
                serialized += ",";
            }

            serialized += "\n";
        }

        if (number_of_objects - i == 1) {
            serialized += "}";
        }
        else {
            serialized += "},\n";
        }
    }

    if (number_of_objects > 1) {
        serialized += "]";
    }

    return serialized;
}

void JSON::add_object() {
    size_t start_index = inserted_values.size();
    inserted_objects.emplace_back(Range{start_index, start_index - 1});
}
