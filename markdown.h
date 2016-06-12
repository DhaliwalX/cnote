#include <iostream>
#include <regex>
#include <exception>
#include <string>
#include <sstream>
#include <map>

typedef std::string cnote_list;

class markdown_parser {
public:
    markdown_parser();
    ~markdown_parser() = default;
    
    void set_cache(const std::string &str)
    {
        cache = str;
    }

    // this function parses the tags which are of the form
    //          *tag* or _tag_ or **tag**, __tag__
    // these have same syntax as of markdown's
    std::vector<std::string> parse_tags();

    // parses the list elements and return those elements
    std::vector<cnote_list> parse_list();

    // parse the title of the note
    std::string parse_title();
private:
    std::string cache;
};

