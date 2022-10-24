//
// Created by Ömer Talip Akalın on 22.10.2022.
//

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>

std::deque<std::string> history;

void update_hist(const std::string& line) {
    history.push_back(line);
    if(history.size() > 15) {
        history.pop_front();
    }
}

constexpr unsigned int hash(const char* s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

std::string exec(const std::string &command) {
    auto pipe = popen(command.c_str(), "r");
    std::string result;

    int curr_c;

    if(pipe == nullptr) {
        auto error_str = "ABORTED: " + command;
        std::cout << error_str << std::endl << std::flush;
        exit(42);
    }

    while((curr_c = fgetc(pipe)) != EOF) {
        result += (char)curr_c;
    }

    if(result.back() == '\n')  {
        result.pop_back();
    }

    return result;
}


int main() {
    std::string host = exec("whoami");

    while(true){
        std::cout << host << " >>> " << std::flush;

        std::string line;
        std::string whole_line;

        std::cin >> line;
        whole_line += line;
        switch(hash(line.c_str())) {
            case hash("listdir"):
                std::cout << exec("ls") << std::endl << std::flush;
                update_hist(line);
                continue;
            case hash("mycomputername"):
                std::cout << exec("hostname") << std::endl << std::flush;
                update_hist(line);
                continue;
            case hash("whatsmyip"):
                std::cout << exec("echo $(dig +short myip.opendns.com @resolver1.opendns.com)") << std::endl << std::flush;
                update_hist(line);
                continue;
            case hash("dididothat"):
                std::getline(std::cin, line);
                if(line.back() == '\n') {
                    line.pop_back();
                }
                whole_line += line;
                line.erase(0, 2); // Delete first space and first quotation mark.
                line.pop_back(); // Delete last quotation mark.
                std::cout << (std::count(history.begin(), history.end(), line) > 0 ? "YES" : "NO") << std::endl << std::flush;
                update_hist(whole_line);
                continue;
            case hash("printfile"):
                std::getline(std::cin, line);
                if(line.back() == '\n') {
                    line.pop_back();
                }
                whole_line += line;
                update_hist(whole_line);
                if(std::count(line.begin(), line.end(), '>') > 0) {
                    exec("cat " + line);
                    continue;
                } else {
                    line.erase(0, 1);
                    std::ifstream infile(line.c_str());
                    std::string file_line;
                    while(std::getline(infile, file_line)) {
                        std::cout << file_line << std::flush;
                        int c = getchar();
                        while(c != '\n') {
                            c = getchar();
                        }
                    }
                }
                continue;
            case hash("exit"):
                return 0;
            default:
                std::cout << "ERROR: " << line << " cannot be processed." << '\n' << std::flush;
                return 0;
        }

    }

}
