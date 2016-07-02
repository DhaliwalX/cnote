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
    return ret + ".cnote";
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
                "flags for the note")
        ("search,s", opt::value<std::string>(), "title of the note to search")
        ("db-file,d", opt::value<std::string>()
                ->default_value(std::string("/home/") + get_user_name() + "/.cndb"), 
                "name of file in which database will be stored")
        ("debug", opt::value<bool>()->default_value(false), "print the debug output")
        ("list,l", "list the notes");
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
    opts.notes_db_ = vm["db-file"].as<std::string>();
    opts.debug_ = vm["debug"].as<bool>();
}

void create_database_file(const char *dbfile)
{
    std::ofstream os { dbfile };

    if (!os) {
        std::cerr << "fatal: unable to create " << dbfile << std::endl;
        exit(-1);
    }
    os << "[]" << std::endl;
    os.close();
}

bool parse_note_database(cnote_parser &parser, const char *dbfile)
{
    std::ifstream is(dbfile);
    bool result = false;

    if (!is) {
        std::cerr << "fatal: unable to open " << dbfile << std::endl;
        std::cout << "creating one..." << std::endl;
        create_database_file(dbfile);
    }
    result = parser.parse(is);
    is.close();
    return result;
}

bool write_note_database(cnote_parser &parser, const char *dbfile)
{
    std::ofstream os(dbfile);

    if (!os) {
        std::cerr << "fatal: unable to open " << dbfile << std::endl;
        return false;
    }

    parser.dump(os);
    if (opts.debug_) {
        parser.dump(std::cout);
    }
    os.close();
    return true;
}

bool print_note(cnote_parser &parser, std::string title)
{
    std::shared_ptr<cnote> note = parser.find_note(title);

    if (note) {
        // we found the note
        if (opts.debug_)
            std::cout << "Found note: " << title << std::endl;
    }
    return true;
}

void list_notes(cnote_parser &parser)
{
    parser.list_note(std::cout);    
}

int main(int argc, char *argv[])
{
    opt::options_description desc("Usage: ");
    opt::positional_options_description po;
    opt::variables_map vm;
    std::shared_ptr<cnote> cn;
    set_options(desc, po);
    if (!parse_command_line_options(desc, po, argc, argv, vm)) {
        std::cout << "See -h for usage." << std::endl;
        return -1;
    }
    parse_cnote_options(vm);
    cnote_parser parser;
    if (!parse_note_database(parser, opts.notes_db_.c_str()))
        return -1;
    
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    } else if (vm.count("search")) {
        if (opts.debug_)
            std::cout << "Searching for: " << vm["search"].as<std::string>()
                    << std::endl;
        print_note(parser, vm["search"].as<std::string>());
        return 0;
    } else if (vm.count("list")) {
        list_notes(parser);
        return 0;
    }

    cnote_creator cc;
    cn = cc.create_note();

    if (cn) {
        parser.save_note(cn);
        write_note_database(parser, opts.notes_db_.c_str());
    }
    return 0;
}

