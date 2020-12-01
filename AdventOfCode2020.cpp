// Hi, this is my AdventOfCode 2020 stuff


#include <stdio.h>
#include <stdlib.h>


////////////////////////////
////////////////////////////
// General tools

typedef long long BigInt;


////////////////////////////
////////////////////////////
// Problems

////////////////////////////
// Problem 1 - 



////////////////////////////
////////////////////////////
// Main

int main(int argc, char** argv) {
	if (argc <= 1) {
		printf(
			"Usages:\n"
			"  AdventOfCode2020 <problem#>\n");
		return 0;
	}

	const char* problemArg = argv[1];
	int problemNum = atoi(problemArg);
	printf("Solving problem #%d\n\n", problemNum);
	switch (problemNum) {
	case 1:
		break;
	default:
		printf("'%s' is not a valid problem number!\n\n", problemArg);
		break;
	}

	return 0;
}

