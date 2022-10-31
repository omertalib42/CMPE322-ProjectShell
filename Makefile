build: project_shell.cpp
	g++ -std=c++14 project_shell.cpp -oproject_shell -Wall

run: ./project_shell
	./project_shell

clean: ./project_shell
	rm -rf ./project_shell
