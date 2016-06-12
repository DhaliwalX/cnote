#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <string>
#include "cnote.h"
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

std::string get_user_name()
{
    char *user = std::getenv("USER");   // we're working on linux
    if (!user) {
        return "unnamed";
    } else {
        return user;
    }
}

// sets command line options
void set_options(opt::options_description &desc,
	opt::positional_options_description &po)
{
    desc.add_options()
        ("notes-file,n",
            opt::value<std::string>()->default_value(default_notes_filename()),
            "path of the notes file")
	("help,h", "show help message and exit")
        ("author,a", opt::value<std::string>()->default_value(get_user_name()),
                "author of the note")
        ("flags,f", opt::value<cnote_flag__>()->default_value(cnote_flag::Normal),
                "flags for the note");
    po.add("notes-file", -1);
}

bool parse_command_line_options(opt::options_description &desc,
    opt::positional_options_description &po, 
    int argc, char *argv[], opt::variables_map &vm)
{
    try {
        opt::store(opt::command_line_parser(argc, argv).options(desc)
            .positional(po).run(), vm);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    opt::notify(vm);
    return true;
}

void parse_cnote_options(opt::variables_map &vm)
{
    if (vm.count("notes-file")) {
        opts.notes_file_ = vm["notes-file"].as<std::string>();
    } else {
        opts.notes_file_ = "cnotes.txt";
    }
    opts.author_ = vm["author"].as<std::string>();
    opts.flags_ = vm["flags"].as<cnote_flag__>();
}

int main(int argc, char *argv[])
{
    opt::options_description desc("Usage: ");
    opt::positional_options_description po;
    opt::variables_map vm;

    set_options(desc, po);
    if (!parse_command_line_options(desc, po, argc, argv, vm)) {
        std::cout << "See -h for usage." << std::endl;
        return -1;
    }
    
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    parse_cnote_options(vm);
    cnote_creator cc;
    cc.create_note(std::cin);
    
    return 0;
}

