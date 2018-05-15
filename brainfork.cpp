/*
Compilation on linux:
g++ brainfork.cpp -std=c++17 -pthread -o brainfork
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <thread>
#include <vector>

using namespace std;

void read_script(string filename, string *instruction_set_ptr);
void command_loop(string *instruction_set_ptr, int instruction_pointer);
int find_closing_bracket(string* instruction_set_ptr, int instruction_pointer, int instruction_set_size);
int find_opening_bracket(string* instruction_set_ptr, int instruction_pointer, int instruction_set_size);

vector<thread> thread_set;

int main(int argc, char *argv[]) {

	if(argc != 2) {
		cout << "Usage: " << argv[0] << " <script_name>" << endl;
		return 1;
	}

	string instruction_set, *instruction_set_ptr = &instruction_set;

	read_script(argv[1], instruction_set_ptr);

	command_loop(instruction_set_ptr, 0);

	for(int i=0; i<thread_set.size(); i++)
		thread_set[i].join();

	return 0;
}


void read_script(string filename, string *instruction_set_ptr) {

	ifstream script_file (filename);

	int instruction_set_max_size = numeric_limits<int>::max();

	if(script_file.is_open()) {
		string line;

		while(getline(script_file, line)) {
			*instruction_set_ptr += line;
			if((*instruction_set_ptr).length() > instruction_set_max_size) {
				cout << "Too big file, max size:" << instruction_set_max_size << endl;
				script_file.close();
				exit(2);
			}
		}
	} else {
		cout << "Unable to open file" << endl;
		exit(1);
	}
	script_file.close();

	return;
}


void command_loop(string *instruction_set_ptr, int instruction_pointer) {
	
	int *memory =  new int[30000];	
	int memory_pointer = 0;
	int instruction_set_size = (*instruction_set_ptr).length() - 1;

	while(true) {
		if(instruction_pointer > instruction_set_size) {
			break;
		}

		switch((*instruction_set_ptr)[instruction_pointer]) {
			case '>':
				instruction_pointer++;
				memory_pointer++;
				if(memory_pointer == 30000) memory_pointer = 0;
				break;

			case '<':
				instruction_pointer++;
				memory_pointer--;
				if(memory_pointer == -1) memory_pointer = 29999;
				break;

			case '+':
				memory[memory_pointer]++;
				instruction_pointer++;
				break;

			case '-':
				if(memory[memory_pointer] == 0) {
					instruction_pointer++;
				} else {
					memory[memory_pointer]--;
					instruction_pointer++;
				}
				break;

			case '.':
				cout << (char)memory[memory_pointer];
				instruction_pointer++;
				break;

			case ',':
				cin >> memory[memory_pointer];
				instruction_pointer++;
				break;

			case '[':
				if(memory[memory_pointer] == 0) {
					instruction_pointer = find_closing_bracket(instruction_set_ptr, instruction_pointer, instruction_set_size);
				} else {
					instruction_pointer++;
				}
				break;

			case ']':
				instruction_pointer = find_opening_bracket(instruction_set_ptr, instruction_pointer, instruction_set_size);
				break;

			case 'Y':
				{
				memory[memory_pointer] = 0;
				instruction_pointer++;
				thread_set.push_back(thread(command_loop, instruction_set_ptr, instruction_pointer));
				}
				break;

			default:
				instruction_pointer++;
				break;

		}

	}

	delete[] memory;

	return;
}


int find_closing_bracket(string* instruction_set_ptr, int instruction_pointer, int instruction_set_size) {

	int i=instruction_pointer + 1;
	int depth = 0;
	
	for(; i<=instruction_set_size; i++) {
		if((*instruction_set_ptr)[i] == '[') depth++;
		else if((*instruction_set_ptr)[i] == ']') depth--;

		if(depth == -1)
			break;
	}

	if(depth != -1) {
		cout << "Error: missing closing bracket for bracket on position:" << instruction_pointer << endl;
		exit(3);
	}

	return i+1;
}


int find_opening_bracket(string* instruction_set_ptr, int instruction_pointer, int instruction_set_size) {

	int i=instruction_pointer - 1;
	int depth = 0;

	for(; i>=0; i--) {
		if((*instruction_set_ptr)[i] == '[') depth++;
		else if((*instruction_set_ptr)[i] == ']') depth--;

		if(depth == 1)
			break;
	}

	if(depth != 1) {
		cout << "Error: missing opening bracket for bracket on position:" << instruction_pointer << endl;
		exit(4);
	}

	return i;

}