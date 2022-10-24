#include <iostream>
#include <random>
#include <sys/wait.h>
#include <unistd.h>

void do_work(const std::string& s) {
    const int NUM_TIMES = 5;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> randomizer(1, 2);
    for(int i = 0;NUM_TIMES > i;i++) {
        int k = randomizer(mt);
        sleep(k);
        std::cout << "Done pass " << s << ' ' << i << '\n';
    }

}

int main() {
    std::cout << getpid() << '\n';

    pid_t pid = fork();

    if(pid > 0) {
        std::cout << "Parent: " << pid << ' ' << getpid() << '\n';
        do_work("parent");
        std::cout << wait(nullptr) << ' ';
        std::cout << "Parent ending" << '\n';
    } else if (pid == 0){
        std::cout << "Child: " << pid << ' ' << getpid() << '\n' ;
        do_work("child");
    } else {
        std::cout << "FAILED FORK" << "\n";
        exit(1);
    }

    std::cout << "AAAAAAAAAAAAAAA\n";

}
