#include <fstream>
#include <iostream>
#include <string>

#define SIZE 95
#define KB 1024

using namespace std;

int main(){
	fstream fs;
	fs.open("temp.txt",fstream::in|fstream::binary|fstream::app);
	for(int i = 0; i < SIZE * KB; ++i)
		fs<<"0";
	fs.close();
	return 0;
}