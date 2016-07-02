#include "cnote.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <regex>

cnote_options opts;
cnote::cnote(const std::string &note, const std::string &title,
        const std::string &author, const cnote_flag__ &flags,
        const std::vector<std::string> &tags) :
    m_flags(flags),  m_tags(tags), m_note(note), m_title(title), m_author(author)
{ }

cnote::cnote() :
    m_flags(cnote_flag::Normal)
{ }

std::string &cnote::note()
{
    return this->m_note;
}

std::string &cnote::title()
{
    return this->m_title;
}

std::string &cnote::author()
{
    return this->m_author;
}

bool cnote::has_tag(const std::string &tag) const
{
    for (const auto &t : this->m_tags) {
        if (t == tag)
            return true;
    }
    return false;
}

bool cnote::has_flag(const cnote_flag & flag) const
{
    return flag & this->m_flags;
}

void cnote::mark_flag(const cnote_flag &flag)
{
    this->m_flags |= (flag);
}

void cnote::mark_tag(const std::string &tag)
{
    this->m_tags.push_back(tag);
}

std::string cnote::to_json() const 
{
    json j = {};

    std::string tags = "[ ";

    for (const auto &tag : this->m_tags) {
        tags += tag + ",";
    }
    if (m_tags.size())
        tags.pop_back();
    tags += "]";
    j["title"] = json::string_t(this->m_title);
    j["author"] = json::string_t(this->m_author);
    j["note"] = json::string_t(this->m_note);
    j["tags"] = json::string_t(tags);
    j["flags"] = json::number_integer_t(this->m_flags);

    return j.dump(4);
}

bool cnote_parser::parse(std::istream &is)
{
    try {
        is >> json_parser;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    // since our notes are stored in json array
    if (json_parser.is_array()) {
        if (opts.debug_)
            std::cout << opts.notes_db_ << " [OK]" << std::endl;
        return true;
    }
    return false;
}

std::shared_ptr<cnote> cnote_parser::find_note(const std::string &title) const
{
    std::shared_ptr<cnote> note;
    std::string t;

    for (const auto &element : json_parser) {
        t = element["title"];
        if (t == title) {
            note = std::make_shared<cnote>();
            note->title() = t;
            note->author() = element["author"];
            for (const auto &tag : element["tags"]) {
                note->mark_tag(tag.get<std::string>());
            }
            note->note() = element["note"];
        }
    }

    return note;
}

void cnote_parser::list_note(std::ostream &os) const
{
    size_t i = 0;
    for (const auto &element : json_parser) {
        os << "[" << i << "] " << element["title"] << std::endl;
    }
}

std::string get_flag_string(cnote_flag flag, int i)
{
    switch (flag & static_cast<cnote_flag>(i)) {
    case cnote_flag::Normal:
        return "Normal";
    case cnote_flag::Important:
        return "Important";
    case cnote_flag::Todo:
        return "Todo";
    case cnote_flag::Log:
        return "Log";
    case cnote_flag::Event:
        return "Event";

    default:
        throw cnote_bad_flag_exception();
    }
}

std::ostream &operator<<(std::ostream &os, cnote &cn)
{
    os << "{\n\t";
    os << "'title': '" << cn.title() << "',\n\t";
    os << "'note': '" << cn.note() << "',\n\t";
    os << "'author': '" << cn.author() << "',\n\t";
    os << "'tags': " << "[ ";
    for (const auto &tag : cn.m_tags) {
        os << "'" << tag << "', ";
    }
    os << " ],\n\t";
    os << "'flags': [ ";
    for (auto i = 1; i != 32; i <<= 1) {
        if (cn.has_flag(static_cast<cnote_flag>(i))) {
            os << "'" <<
                get_flag_string(static_cast<cnote_flag>(cn.m_flags), i) << "', ";
        }
    }
    os << "]\n}";
    return os;
}

void cnote_parser::dump(std::ostream &os) const
{
    os << json_parser.dump(4) << std::endl;
}

void cnote_parser::save_note(std::shared_ptr<cnote> note)
{
    try {
        std::istringstream is {  note->to_json() };
        json jnote;
        is >> jnote;
        if (opts.debug_) {
            std::cout << "jnote: " << jnote.dump(4);
        }
        json_parser.push_back(jnote);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(2);
    }
}

bool cnote_creator::parse_note_file(std::istream &is, std::shared_ptr<cnote> &p)
{
    std::string note;
    char buffer[1024];
    markdown_parser parser;

    // read the whole file
    while (is) {
        is.read(buffer, 1023);
        auto count = is.gcount();
        note.append(buffer, count);
    }
    parser.set_cache(note);

    std::string title = parser.parse_title();
    while (!title.length()) {
        std::cout << "Please specify a title: ";
        std::getline(std::cin, title);
    }

    p->title() = title;
    p->note() = note;
    p->author() = getenv("USER");
    std::vector<std::string> tags = parser.parse_tags();
    for (const auto &i : tags) {
        p->mark_tag(i);
    }
    if (opts.debug_) {
        std::cout << (*p).to_json() << std::endl;
    }
    return true;
}


class arguments {
public:
    arguments() :
        argv{ } 
    { }

    void push_back(const char *arg)
    {
        if (arg == nullptr) {
            argv.push_back(nullptr);
            return;
        }
        char *temp = new char[strlen(arg)];
        std::strcpy(temp, arg);
        argv.push_back(temp);
    }

    char **data()
    {
        return argv.data();
    }

private:
    std::vector<char*> argv;
};

bool launch_editor(const char *editor, const char *filename)
{
    int editor_status = 0;
    arguments args;
    args.push_back(editor);
    args.push_back(filename);
    args.push_back(nullptr);

    char **arg_list = args.data(); 
    if (execvp(editor, arg_list) == -1)
        throw std::runtime_error((std::string() + "unable to execv " 
                    + editor + ": " + strerror(errno)).c_str());
    if (wait(&editor_status) == -1)
        throw std::runtime_error((std::string() + "unable to wait: "
                    + strerror(errno)).c_str());
    if (!WIFEXITED(editor_status)) {
        std::cout << editor << " exited abnormally, aborting." << std::endl;
        return false;
    }
    return true;
}

std::shared_ptr<cnote> cnote_creator::create_note()
{
    // so let's see how would I take the input
    // I'll just open their favorite text editor like vim or emacs to write
    // their note
    // in any way
    // But I should give them some format for making notes
    // so here is the format
    // It should be much like Markdown, because they know about that
    //   # Title  "will be in this format"
    //    then they can create list of todo's like this
    //          [] To be done
    //          [*] Done
    //    They can create tags like *ATag*
    //    They can create list
    //          + Item 1
    //          + Item 2
    //
    // get the user's favourite editor, if not specified then default will
    // be mine's favourite that's `vim`.
    
    char *editor = getenv("EDITOR");
    std::string e = "";
    if (!editor) {
        e = "vim";
    } else {
        e = editor;
    }
    std::ofstream os { opts.notes_file_.c_str(), std::ios::trunc };
    os.close();
    system((e + " " + opts.notes_file_).c_str());
    std::ifstream is(opts.notes_file_);
    std::shared_ptr<cnote> note_ptr = std::make_shared<cnote>();

    if (is) {
        parse_note_file(is, note_ptr);
    } else {
        std::cerr << "error: can't open " << opts.notes_file_ << std::endl;
    }
    is.close();
    return note_ptr;
}

