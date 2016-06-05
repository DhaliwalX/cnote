#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <sstream>

typedef unsigned int cnote_flag__;

enum class cnote_flag : cnote_flag__ {
    Normal = 0,
    Important = 1,
    Todo = 2,
    Log = 4,
    Event = 8
};

class cnote {
public:
    cnote(const std::string &note, const std::string &title,
            const std::string &author, const cnote_flag &flag,
            const std::vector<std::string> &tags);
    cnote();

    cnote(const cnote &) = default;
    cnote& operator=(const cnote &) = default;
    ~cnote() = default;

    std::string &note();
    std::string &title();
    std::string &author();
    bool has_tag(const std::string &tag) const;
    bool has_flag(const cnote_flag &flag) const;
    void mark_flag(const cnote_flag &flag);
    void mark_tag(const std::string &tag;
private:
    cnote_flag flags;
    std::vector<std::string> tags;
    std::string note;
    std::string title;
    std::string author;
};

// every cnote will be stored in the json format
// for e.g.
// [
//    {
//       "title": "A simple note",
//       "time": "time",
//       "author": "prince",
//       "tags": [ "simple", "first" ],
//       "flags" : [ "important", "alarm", "todo" ],
//       "note": "blah blah blah blah blah...."
//    }
// ]

class cnote_parser {
public:
    cnote_parser() = default;
    ~cnote_parser() = default;
    cnote_parser(const cnote_parser &) = default;
    cnote_parser& operator=(const cnote_parser &) = default;
    
    bool set_parse_options(const boost::program_options::variables_map &v);
    bool parse(std::istringstream &stream);

    // print the output of the parser to the output stream
    void dump(std::ofstream &os) const;
private:
    json json_parser;
    std::vector<cnote> note_store;
    boost::program_options::variables_map vm;
};

class cnote_creator {
public:
    cnote_creator() = default;
    cnote_creator(const cnote_creator &) = default;
    cnote_creator &operator=(const cnote_creator &) = default;
    ~cnote_creator() = default;
    
    bool create_options(const boost::program_options::variables_map &vm);

    bool parse_note_file(std::istream &is, std::shared_ptr<cnote> &p); 
    std::shared_ptr<cnote> create_note(std::istream &is);
    std::shared_ptr<cnote> create_node(std::string &str); 
private:
    boost::program_options::variables_map vm;
};

