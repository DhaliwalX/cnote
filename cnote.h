#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <sstream>
#include "json.hpp"
#include "markdown.h"

using json = nlohmann::json;


typedef unsigned int cnote_flag__;

enum cnote_flag : cnote_flag__ {
    Normal = 1,
    Important = 2,
    Todo = 4,
    Log = 8,
    Event = 16
};

// options passed from the command line
struct cnote_options {
    cnote_flag__ flags_;
    std::string author_;
    std::string title_;
    std::string notes_file_;
    std::string notes_db_;
    bool debug_;
};

extern cnote_options opts;

class cnote_exception : public std::exception {
public:
    const char *what() const noexcept override
    {
        return "Unknown cnote_exception thrown";
    }
};

class cnote_bad_flag_exception : public cnote_exception {
public:
    const char *what() const noexcept override 
    {
        return "Bad flag for note";
    }
};

class cnote {
public:
    cnote(const std::string &note, const std::string &title,
            const std::string &author, const cnote_flag__ &flags,
            const std::vector<std::string> &tags);
    cnote();

    friend std::ostream &operator<<(std::ostream &os, cnote &cn);
    cnote(const cnote &) = default;
    cnote& operator=(const cnote &) = default;
    ~cnote() = default;

    std::string &note();
    std::string &title();
    std::string &author();
    bool has_tag(const std::string &tag) const;
    bool has_flag(const cnote_flag &flag) const;
    void mark_flag(const cnote_flag &flag);
    void mark_tag(const std::string &tag);
    std::string to_json() const;
private:
    cnote_flag__ m_flags;
    std::vector<std::string> m_tags;
    std::string m_note;
    std::string m_title;
    std::string m_author;
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
    
    bool parse(std::istream &stream);

    // print the output of the parser to the output stream
    void dump(std::ostream &os) const;
private:
    json json_parser;
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

