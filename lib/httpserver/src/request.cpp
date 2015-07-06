#include "request.hpp"

const char *Request_type_str[] = {
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "OPTIONS",
    "TRACE",
    "CONNECT",
    "PATCH",
    "WAT"
};

/*
 * example request:
 * GET /index.html HTTP/1.1
 * ...
 * ...
 *
 */
Request_data Request::handle_request(const std::string &req_str) 
{
    Request_data req_data;
    // GET /index.html HTTP/1.1
    //    ^ pos and in the same time number of characters to extract
    std::size_t pos = req_str.find_first_of(" ");

    // GET /index.html HTTP/1.1
    // ^^^ substr
    std::string temp_str = req_str.substr(0, pos);

    if (temp_str == "GET") {
        req_data.req_type = Request::type::GET;
    } else if (temp_str == "POST") {
        req_data.req_type = Request::type::POST;
    } else if (temp_str == "PUT") {
        req_data.req_type = Request::type::PUT;
    } else if (temp_str == "DELETE") {
        req_data.req_type = Request::type::DELETE;
    } else if (temp_str == "OPTIONS") {
        req_data.req_type = Request::type::OPTIONS;
    } else if (temp_str == "HEAD") {
        req_data.req_type = Request::type::HEAD;
    } else if (temp_str == "TRACE") {
        req_data.req_type = Request::type::TRACE;
    } else if (temp_str == "CONNECT") {
        req_data.req_type = Request::type::CONNECT;
    } else if (temp_str == "PATCH") {
        req_data.req_type = Request::type::PATCH;
    } else {
        req_data.req_type = Request::type::WAT;
    }

    // GET /index.html HTTP/1.1
    //     ^ begin_pos
    std::size_t begin_pos = pos + 1;

    // GET /index.html HTTP/1.1
    //     ^^^^^^^^^^^ substr
    pos = req_str.find_first_of(" ", begin_pos);
    temp_str = req_str.substr(begin_pos, pos - begin_pos);
    req_data.url = temp_str;

    return req_data;

}
