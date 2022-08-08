#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<string>
#include<dirent.h>
#include<vector>
#include<fcntl.h>
using namespace std;

void convertIntoStrings(int i, string command, char*& tempName, char*& tempValue) {		//helper function to divide command string into multiple strings(instructions) which are basically
																						//char*s
	string name, value;
	bool check = true;
	for (i; command[i] != '\0'; ++i) {
		if (command[i] != ' ') {
			if (check == true) {
				name += command[i];
			}

			else if (check == false) {
				value += command[i];
			}

			if (command[i + 1] == ' ') {
				check = false;
			}
		}
	}

	tempName = new char[name.size() + 1];
	name.copy(tempName, name.size() + 1);
	tempName[name.size()] = '\0';
	tempValue = new char[value.size() + 1];
	value.copy(tempValue, value.size() + 1);
	tempValue[value.size()] = '\0';
	return;
}

void convertIntoCharPointer(string s, char*& c) {	//converts the passed string s into char* c
	c = new char[s.size() + 1];
	s.copy(c, s.size() + 1);
	c[s.size()] = '\0';
}

char* redirectionConversion(string command, char **&argv, char *&fileNameC, char c, bool both, char *fileNameC1 = NULL) {	//helper function (convsersion) for redirection command
	string temp = "", fileName = "", fileName1 = "";
	vector<string> v;
	bool check = false, check1 = false;
	for (int i = 0; command[i] != '\0'; ++i) {
		if (command[i] != c && command[i] != '&') {		//handling background in redirection too
			if (command[i] != ' ') {
				if (check == false) {
					temp += command[i];
				}

				else if (check == true && check1 == false) {
					fileName += command[i];
				}

				else if (check1 == true && command[i] != '>') {
					fileName1 += command[i];
				}
			}

			if (command[i] == ' ' || command[i + 1] == '\0') {
				if (check == false) {
					if (temp != "") {
						v.insert(v.end(), temp);
					}
					temp.clear();
					temp = "";
				}

				if (fileName != "") {
					check1 = true;
				}
			}
		}

		else {
			check = true;
		}
	}

	argv = new char*[v.size() + 1];

	for (int i = 0; i < v.size(); ++i){
		convertIntoCharPointer(v[i], argv[i]);
	}
	argv[v.size()] = NULL;
	convertIntoCharPointer(fileName, fileNameC);

	if (fileName1 != "") {
		convertIntoCharPointer(fileName1, fileNameC1);
	}

	return fileNameC1;
}

bool checkCommand(string command, char check1, char check2) {	//checks given command that whether check1 or check2 is present in it or not
	for (int i = 0; command[i] != '\0'; ++i) {
		if (command[i] == check1) {
			return true;
		}

		else if (command[i] == check2) {
			return true;
		}
	}
	return false;
}

void printEnvVariables() {	//function to print all enviornment variables
	extern char **environ;
	for (int i = 0; environ[i]; ++i) {
		printf("%s\n", environ[i]);
	}
}

void sigintHandler(int sig_num) {		//signal handler for SIGINT 
    signal(SIGINT, sigintHandler);	//Reset handler to catch SIGINT next time
} 

void changeDirectoryToHome() {	//func to change directory to home
	char cwd[100];
	getcwd(cwd, sizeof(cwd));
	if (cwd[1] != '\0') {
		while (cwd[0] != '/' || cwd[1] != 'h' || cwd[2] != 'o' || cwd[3] != 'm' || cwd[4] != 'e' || cwd[5] != '\0') {
			chdir("..");
			getcwd(cwd, sizeof(cwd));
		}
	}
	return;
}

int main(int argc, char *argv[]) {
	char hostname[50];
	string command = "", shellPath;
	bool redirection, bg, pipes;

	gethostname(hostname, sizeof(hostname));	//getting hostname of the OS
	
	shellPath = get_current_dir_name();
	shellPath += "/gbsh";
	char* shellPathC;
	convertIntoCharPointer(shellPath, shellPathC);
	setenv("SHELL", shellPathC, 1);
	delete[] shellPathC;

    signal(SIGINT, sigintHandler);	//Set the SIGINT (Ctrl-C) signal handler to sigintHandler

	while(true) {
		printf("sameet_asadullah@%s %s ", hostname, get_current_dir_name());	//printing the terminal

        while (command == "" ) {
            getline(cin, command);	//taking command line input
        }

		redirection = checkCommand(command, '>', '<');	//checking if redirection is present in the command or not
		bg = checkCommand(command, '&', '&');			//checking if background sign is present in the command or not
		pipes = checkCommand(command, '|', '|');		//checking if pipes sign is present in the command or not

		/*multiple if conditions to execute given command line input*/
		if (command == "exit") {
			exit(0);
		}

		else if (command == "pwd") {
			printf("%s\n", get_current_dir_name());
		}

		else if (command == "clear") {
			printf("\033[2J\033[1;1H");
		}

		else if (command[0] == 'l' && command[1] == 's' && redirection == false && bg == false && pipes == false) {
			string dir = "";
			
			if (command == "ls") {
				dir = get_current_dir_name();
			}

			else {
				for (int i = 2; command[i] != '\0'; ++i) {
					if (command[i] != ' ') {
						dir += command[i];
					}
				}
			}

			if (dir == "") {
				dir = get_current_dir_name();
			}

			char* dirC;
			convertIntoCharPointer(dir, dirC);

			struct dirent *files;
			DIR *directory = opendir(dirC);
			if (directory != NULL) {
				files = readdir(directory);
				while (files != NULL) {		//printing all the files and directories within directory
					if (files->d_name[0] != '.') {
						printf ("%s\n", files->d_name);
					}
					files = readdir(directory);
				}
				closedir (directory);
			}
			delete[] dirC;
		}

		else if (command[0] == 'c' && command[1] == 'd') {
			if (command == "cd") {
				changeDirectoryToHome();
			}

			else {
				string temps = "";
				for (int i = 2; command[i] != '\0'; ++i) {
					if (command[i] != ' ') {
						temps += command[i];
					}
				}

				if (temps == "") {
					changeDirectoryToHome();
				}

				else {
					char *tempc = new char[temps.size() + 1];
					convertIntoCharPointer(temps, tempc);
					chdir(tempc);
					delete[] tempc;
				}
			}
			setenv("PWD", get_current_dir_name(), 1);	//setting enviornment variable PWB to new(changed) working directory 
		}

		else if (command == "environ") {
			printEnvVariables();
		}

		else if (command[0] == 's' && command[1] == 'e' && command[2] == 't' && command[3] == 'e' && command[4] == 'n' && command[5] == 'v') {
			char *tempName, *tempValue;
			convertIntoStrings(6, command, tempName, tempValue);
			
			if (getenv(tempName) != NULL) {	//if enviornment variable with the same name is already present
				char check;
				printf("Enviornment varibale is already defined with value '%s'. Do you want to reset it? (y/n)\n", getenv(tempName));
				scanf("%c", &check);
				
				while (check != 'y' && check != 'n') {
					printf("Enter correct input (y/n)\n");	
					scanf("%c", &check);
				}

				if (check == 'y') {
					setenv(tempName, tempValue, 1);	
				}
			}

			else {
				setenv(tempName, tempValue, 1);
			}
			delete[] tempName;
			delete[] tempValue;
		}

		else if (command[0] == 'u' && command[1] == 'n' && command[2] == 's' && command[3] == 'e' && command[4] == 't' && command[5] == 'e' && command[6] == 'n' && command[7] == 'v') {
			char *tempName, *tempValue;
			convertIntoStrings(8, command, tempName, tempValue);

			if (getenv(tempName) == NULL) {
				printf("Enviornment varibale doesn't exist\n");
			}

			else {
				unsetenv(tempName);
			}
			delete[] tempName;
			delete[] tempValue;
		}

		else if (pipes == true) {
			vector<string> v;
			string temp = "";
			vector<char**> vC;
			int count = 0;
			string filename = "";
			bool check = false;

			for (int i = 0; command[i] != '\0'; ++i) {
				if (command[i] == '|') {
					count++;
				}
			}

			for (int i = 0; command[i] != '\0'; ++i) {
				if (command[i] != ' ' && command[i] != '|' && command[i] != '<' && command[i] != '&' && command[i] != '>') {	//handling redirection and background in pipes too
					if (check == true) {
						filename += command[i];
					}
					
					else {
						temp += command[i];
					}
				}

				if (command[i] == ' ' || command[i + 1] == '\0' || command[i] == '|'){
					if (temp != "") {
						v.insert(v.end(), temp);
					}

					if (command[i] == '|' || command[i + 1] == '\0') {
						char **argv;

						argv = new char*[v.size() + 1];
						for (int i = 0; i < v.size(); ++i){
							convertIntoCharPointer(v[i], argv[i]);
						}

						argv[v.size()] = NULL;
						vC.insert(vC.end(), argv);
						v.clear();
					}

					if (check == true && filename != "") {
						check = false;
					}
					
					temp.clear();
					temp = "";
				}

				if (command[i] == '>') {
					check = true;
				}
			}

			if (filename != "") {
				check = true;
			}

			int fd[2], input;

			for (int i = 0; i < vC.size(); ++i) {	//creating pipes and executing the commands
				pipe(fd);

				pid_t pid = fork();

				if (pid > 0) {	//parent process
					close(fd[1]);
					if (bg == false) {
						wait(NULL);
					}
					input = fd[0];
				}

				else if (pid == 0) {	//child process
					if (i == 0) {
						close(fd[0]);
						dup2(fd[1] ,1);
					}

					else if (i == vC.size() - 1) {
						close(fd[1]);
						close(fd[0]);
						dup2(input, 0);

						if (check == true) {
							char *filenameC;
							convertIntoCharPointer(filename, filenameC);
							creat(filenameC, 0666);
							int file_fd = open(filenameC, O_RDWR);
							dup2(file_fd, 1);
						}
					}

					else {
						close(fd[0]);
						dup2(input, 0);
						dup2(fd[1], 1);
					}

					if (vC[i][0] != "environ") {
						execvp(vC[i][0], vC[i]);
					}
					printEnvVariables();
					exit(0);
				}

				else {
					cout << "Fork Failed" << endl;
				}
			}
		}
 
		else {
			bool outputRedirection = false, inputRedirection = false, redirectionPrecedence = false, bgPrecedence = false;
			char **argv, *fileNameC, *fileNameC1;
			int fd1, fd2;

			for (int i = 0; command[i] != '\0'; ++i) {	//checking if redirection is present in the command or not
				if (command[i] == '>') {
					outputRedirection = true;
				}

				else if (command[i] == '<') {
					inputRedirection = true;
				}
			}

			for (int i = 0; command[i] != '\0'; ++i) {	//checking which sign came first, for example if background came first in the command then the command will be run in background
														//and redirection wont work and vice versa
				if (command[i] == '>' || command[i] == '<') {
					redirectionPrecedence = true;
					break;
				}

				else if (command[i] == '&') {
					bgPrecedence = true;
					break;
				}
			}

			if (redirectionPrecedence == true && (outputRedirection == true || inputRedirection == true)) {	//redirection
				if (outputRedirection == true && inputRedirection == true) {	
					fileNameC1 = redirectionConversion(command, argv, fileNameC, '<', true, fileNameC1);
					fd1 = open(fileNameC, O_RDWR);
					if (fd1 == -1) {
						printf("%s: No such file or directory\n", fileNameC);
					}
					
					else {
						creat(fileNameC1, 0666);
						fd2 = open(fileNameC1, O_RDWR);
					}
				}

				else if (outputRedirection == true) {
					redirectionConversion(command, argv, fileNameC, '>', false);
					creat(fileNameC, 0666);
					fd1 = open(fileNameC, O_RDWR);
				}

				else {
					redirectionConversion(command, argv, fileNameC, '<', false);
					fd1 = open(fileNameC, O_RDWR);
					if (fd1 == -1) {
						printf("%s: No such file or directory\n", fileNameC);
					}
				}
			}

			pid_t pid = fork();

			if (pid > 0) {
				if (bg == false) {
					wait (NULL);
				}
			}

			else if (pid == 0) {
				if (redirectionPrecedence == true && pipes == false && (outputRedirection == true || inputRedirection == true)) {	//redirection
					if (outputRedirection == true && inputRedirection == true) {	
						if (fd1 != -1) {
							dup2(fd1, 0);
							dup2(fd2, 1);
						}
					}

					else if (outputRedirection == true) {
						dup2(fd1, 1);
					}

					else {
						if (fd1 != -1) {
							dup2(fd1, 0);
						}
					}

					if (argv[0][0] == 'e' && argv[0][1] == 'n' && argv[0][2] == 'v' && argv[0][3] == 'i' && argv[0][4] == 'r' && argv[0][5] == 'o' && argv[0][6] == 'n') {
						printEnvVariables();
						exit(0);
					}

					else if (fd1 != -1) {
						execvp(argv[0], argv); 
					}
				}

				else {	//other commands like top will execute here, if exec fails tokenization will happen
					vector<string> v;
					string temp = "";

					for (int i = 0; command[i] != '\0'; ++i) {	//converting into commands
						if (command[i] != ' ' && command[i] != '&') {
							temp += command[i];
						}

						if (command[i] == ' ' || command[i + 1] == '\0' || command[i] == '&'){
							if (temp != "") {
								v.insert(v.end(), temp);
							}
							temp.clear();
							temp = "";
						}

						if (command[i] == '&') {
							break;
						}
					}

					argv = new char*[v.size() + 1];
			
					for (int i = 0; i < v.size(); ++i){
						convertIntoCharPointer(v[i], argv[i]);
					}
					argv[v.size()] = NULL;

					if (argv[0][0] == 'e' && argv[0][1] == 'n' && argv[0][2] == 'v' && argv[0][3] == 'i' && argv[0][4] == 'r' && argv[0][5] == 'o' && argv[0][6] == 'n' && bg == true) {						
						printEnvVariables();
						exit(0);
					}

					else {
						execvp(argv[0], argv); //exec to run the command
						/*tokenization process starts from here*/
						bool check = false;
						for (int i = 0; command[i] != '\0'; ++i) {
							if (command[i] != ' ') {
								cout << command[i];
								check = true;
								if (command[i + 1] == '\0') {
									cout << endl;
								}
							}

							else {
								if (check) {
									cout << endl;
									check = false;
								}
							}
						}
					}
				}
			}

			else {
				cout << "Fork Failed" << endl;
			}	
		}

		command.clear();
        command = "";
	}
 
	exit(0); // exit normally	
}