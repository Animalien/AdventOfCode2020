// Hi, this is my AdventOfCode 2020 stuff

#include <assert.h>
#include <set>
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
// Problem 1 - Report Repair

void RunReportRepair()
{
	std::set<BigInt> set;

	printf("Reading Report Repair input\n");

	FILE* pFile = fopen("Day1Input.txt", "rt");
	assert(pFile);

	char string[16];
	while ((fgets(string, sizeof(string), pFile) != nullptr) && !feof(pFile))
	{
		const BigInt newInt = atoi(string);
		printf("  Read %lld, ", newInt);

		const BigInt otherInt = 2020 - newInt;
		printf("other = %lld, ", otherInt);

		if (set.count(otherInt) > 0)
		{
			const BigInt prod = newInt * otherInt;
			printf("other IS in set!  Product = %lld\n", prod);
			fclose(pFile);
			return;
		}

		printf("other is NOT in set\n");
		set.insert(newInt);
	}

	printf("Did NOT find valid candidates!!!!\n");
	fclose(pFile);
}


////////////////////////////
////////////////////////////
// Main

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		printf(
			"Usages:\n"
			"  AdventOfCode2020 <problem#>\n");
		return 0;
	}

	const char* problemArg = argv[1];
	int problemNum = atoi(problemArg);
	printf("Solving problem #%d\n\n", problemNum);
	switch (problemNum)
	{
	case 1:
		RunReportRepair();
		break;
	default:
		printf("'%s' is not a valid problem number!\n\n", problemArg);
		break;
	}

	return 0;
}

