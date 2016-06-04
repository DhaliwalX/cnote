#include "cnote.h"

cnote::cnote(const std::string &note, const std::string &title,
        const std::string &author, const cnote_flag &flag,
        const std::vector<std::string> &tags) :
    note(note), title(title), author(author), flags(flag), tags(tags)
{ }

cnote::cnote() :
    flags(cnote_flag::Normal)
{ }

std::string &cnote::note()
{
    return this->note;
}

std::string &cnote::title()
{
    return this->title;
}

std::string &cnote::author()
{
    return this->author;
}

bool has_tag(const std::string &tag) const
{
    for (const auto &t : this->tags) {
        if (t == tag)
            return true;
    }
    return false;
}

bool has_flag(const cnote_flag & flag) const
{
    return flag & this->flags;
}

void mark_flag(const cnote_flag &flag)
{
    this->flags |= flag;
}

void mark_tag(const std::string &tag)
{
    this->tags.push_back(tag);
}

bool cnote_parser::set_parse_options(
        const boost::program_options::variables_map &v)
{
    vm = v;
}

bool parse(std::istringstream &is)
{
    is >> json_parser;

    // since our notes are stored in json array
    if (json_parser.is_array())
        return true;
    return false;
}

void dump(std::ostream &os) const
{
    os << json_parser.dump(2) << std::endl;
}

