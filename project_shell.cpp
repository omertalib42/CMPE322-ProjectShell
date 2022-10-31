//
// Created by Ömer Talip Akalın on 22.10.2022.
// The code is self-explanatory.
//

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>

// This function updates the history of our shell
// and removes old commands if the size is bigger than 15.
void update_hist(const std::string& line, std::deque<std::string>& history) {
    history.push_back(line);
    if(history.size() > 15) {
        history.pop_front();
    }
}

// This hash function enables the use of switch-cases.
// Since std::string cannot be used in switch-cases, this function hashes the predefined commands
// during compile-time (thanks to constexpr) and compares the given input with them.
// A simple explanation of not using if-else structures can be found here:
// https://en.wikipedia.org/wiki/Switch_statement#Advantages_and_disadvantages
// It's just (sort of) a philosophy at this point.
constexpr unsigned int hash(const char* s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

// This function is the core of our shell - it executes a command and fetches its output using popen().
std::string exec(const std::string &command) {
    // Creates a pipe, forks and invokes the shell by given command.
    auto pipe = popen(command.c_str(), "r");
    std::string result;

    int curr_c;

    // Unsuccessful piping.
    if(pipe == nullptr) {
        auto error_str = "ABORTED: " + command;
        std::cout << error_str << std::endl;
        exit(42);
    }

    // Fetching char by char to enable dynamic buffer on std::string.
    while((curr_c = fgetc(pipe)) != EOF) {
        result += (char)curr_c;
    }

    if(!result.empty() && result.back() == '\n')  {
        result.pop_back();
    }

    return result;
}


int main() {
    std::deque<std::string> history; // A deque is used to store previous 15 commands.
    std::string host = exec("whoami"); // Storing this to use in each command.

    while(true){
	// Print the host before each command.
        std::cout << host << " >>> " << std::flush;

        std::string line; // Line here will behave as a token.
        std::string whole_line;

        std::cin >> line;
        whole_line += line;

        switch(hash(line.c_str())) { // Comparing the hash of the input to predefined commands.
            case hash("listdir"):
                std::cout << exec("ls") << std::endl;
                update_hist(line, history);
                continue;
            case hash("mycomputername"):
                std::cout << exec("hostname") << std::endl;
                update_hist(line, history);
                continue;
            case hash("whatsmyip"):
                // The echo below simply prints the public IP.
                std::cout << exec("echo $(dig +short myip.opendns.com @resolver1.opendns.com)") << std::endl;
                update_hist(line, history);
                continue;
            case hash("dididothat"): // Implemented as if there will be quotation marks.
                std::getline(std::cin, line);
                if(!line.empty() && line.back() == '\n') { // Deleting the last escape as std::getline also records it.
                    line.pop_back();
                }
                whole_line += line;
                line.erase(0, 2); // Delete first space and first quotation mark.
                if(!line.empty() && line.back() == '"') {
                    line.pop_back(); // Delete last quotation mark (if exists)
                }
                std::cout << (std::count(history.begin(), history.end(), line) > 0 ? "YES" : "NO") << std::endl;
                update_hist(whole_line, history);
                continue;
            case hash("printfile"):
                std::getline(std::cin, line);
                if(!line.empty() && line.back() == '\n') { // Deleting the last escape as std::getline also records it.
                    line.pop_back();
                }
                whole_line += line;
                update_hist(whole_line, history);
                if(std::count(line.begin(), line.end(), '>') > 0) {
                    exec("cat " + line); // Assuming this command will work in "append" mode.
                    continue;
                } else {
                    line.erase(0, 1); // Delete space from file name.
                    std::ifstream infile(line.c_str());
                    std::string file_line;
                    while(std::getline(infile, file_line)) { // Read file line by line.
                        std::cout << file_line; // Print line.
                        int c = getchar(); // Wait for enter.
                        while(c != '\n') {
                            c = getchar(); // Wait for enter.
                        }
                    }
                }
                continue;
            case hash("hellotext"):
                // There's no default way to open the default editor in Ubuntu 20.04
                // $EDITOR and $VISUAL are empty, and xdg-open is not a safe way to achieve this.
		// Simply using gedit as assumed.
                exec("gedit");
                continue;
            case hash("exit"):
                return 0; // Exit.
            default:
                // Just for security.
                std::cout << "ERROR: " << line << " cannot be processed." << '\n' << std::flush;
                return 0;
        }
    }
}
