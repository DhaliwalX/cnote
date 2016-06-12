#include "cnote.h"
#include <string>
#include <regex>

cnote::cnote(const std::string &note, const std::string &title,
        const std::string &author, const cnote_flag__ &flags,
        const std::vector<std::string> &tags) :
    m_note(note), m_title(title), m_author(author), m_flags(flags), m_tags(tags)
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

bool cnote_parser::set_parse_options(
        const boost::program_options::variables_map &v)
{
    vm = v;
}

bool cnote_parser::parse(std::istringstream &is)
{
    is >> json_parser;

    // since our notes are stored in json array
    if (json_parser.is_array())
        return true;
    return false;
}

void cnote_parser::dump(std::ostream &os) const
{
    os << json_parser.dump(2) << std::endl;
}

#define TEMP_FILE_NAME "cnote.temp"
#define TITLE_REGEX "[ ]*#+.*"

std::string get_temp_file_path()
{
    return std::string("/home/") + getenv("USER") + "/" +  TEMP_FILE_NAME;
}

bool cnote_creator::parse_note_file(std::istream &is, std::shared_ptr<cnote> &p)
{
    std::string line;
    std::regex r(TITLE_REGEX);
    int line_count = 0;

    while (getline(is, line)) {
        if (!line_count && line.empty())
            continue;
        if (!line_count && std::regex_match(line, r)) {
            std::cout << "We got the title: " << line << std::endl;
            p->title() = line;
        } else {
            std::cout << line << std::endl;
            p->note() += line;
        }
    }
    return true;
}

std::shared_ptr<cnote> cnote_creator::create_note(std::istream &i)
{
    // so let's see how would I take the input
    // I'll just open their favorite text editor like vim or emacs to write their note
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

    // yup, it's a system() function call
    system((std::string("vim ") + get_temp_file_path()).c_str());
    std::ifstream is(get_temp_file_path());
    std::shared_ptr<cnote> note_ptr = std::make_shared<cnote>();

    if (is) {
        parse_note_file(is, note_ptr);
    } else {
        std::cerr << "error: can't open " << get_temp_file_path() << std::endl;
    }

    return note_ptr;
}

