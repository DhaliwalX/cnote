#include "markdown.h"

markdown_parser::markdown_parser()
{
}

std::vector<std::string> markdown_parser::parse_tags()
{
    // a bit scary! 
    std::regex tag_r { R"(((\*\*(.*?)\*\*)|(\*(.*?)\*)|(__(.*?)__)))" };
    std::vector<std::string> ret;
    std::string tag;
    std::regex tag_e { R"([a-zA-Z0-9]*)" };     // yup, you are right tags
                                                // tags should be a valid name

    auto start = std::sregex_iterator(cache.begin(), cache.end(), tag_r);
    auto end = std::sregex_iterator();

    if (std::distance(start, end) == 0)
        return {};
    for (auto it = start; it != end; it++) {
        auto str = it->str();
        str = std::regex_replace(str, std::regex{ R"([\*]*)" }, "");
        str = std::regex_replace(str, std::regex{ R"([_]*)" }, "");
        tag = str;
        ret.push_back(tag);
    }

    return ret;
}

std::vector<cnote_list> markdown_parser::parse_list()
{
    std::regex list_r { R"((([ ]*[-][ ]*)([\[])([x]|\*|[X]|)([\]]))(.*))" };
    std::vector<std::string> ret;
    
    auto start = std::sregex_iterator(cache.begin(), cache.end(), list_r);
    auto end = std::sregex_iterator();

    if (std::distance(start, end) == 0)
        return {};
    for (auto it = start; it != end; it++)
    {
        ret.push_back((*it).str());
    }

    return ret;
}

std::string markdown_parser::parse_title()
{
    std::regex title_r { R"([ ]*#+.*)" };

    auto start = std::sregex_iterator(cache.begin(), cache.end(), title_r);
    auto end = std::sregex_iterator();

    if (start == end) {
        std::cout << "warning: no title provided\n";
        return "";
    }

    std::string str = (*start).str();
    str = std::regex_replace(str, std::regex{ R"([ ]*#[ ]*)" }, "");
    return str;
}

