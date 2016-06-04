#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <string>

namespace opt = boost::program_options;

// this will return default filename for the notes file where all the notes
// will be stored, As usual default notes file is /home/$(USER)/.cnote
std::string default_notes_filename()
{
    std::string ret;
    char *user;
    user = getenv("USER");
    if (user == NULL) {
        std::cerr << "error: unable to get username, " << strerror(errno)
                << std::endl;
        exit(1);
    }
    return ret +"/home/" + user + "/.cnote";
}

// sets command line options
void set_options(opt::options_description &desc,
	opt::positional_options_description &po)
{
    desc.add_options()
        ("notes-file,n",
            opt::value<std::string>()->default_value(default_notes_filename()),
            "path of the notes file")
	("help,h", "show help message and exit");
    po.add("title,t", -1);
}

void parse_command_line_options(opt::options_description &desc,
    opt::positional_options_description &po, int argc, char *argv[], opt::variables_map &vm)
{
    try {
        opt::store(opt::command_line_parser(argc, argv).options(desc)
            .positional(po).run(), vm);
        opt::notify(vm);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    opt::options_description desc("Usage: ");
    opt::positional_options_description po;
    opt::variables_map vm;

    set_options(desc, po);
    parse_command_line_options(desc, po, argc, argv, vm);
    for (const auto it : vm) {
        std::cout << it.first << ": ";
        auto &value = it.second.value();

        if (auto v = boost::any_cast<uint32_t>(&value))
            std::cout << *v << std::endl;
        else if (auto v = boost::any_cast<std::string>(&value))
            std::cout << *v << std::endl;
    }
    return 0;
}

