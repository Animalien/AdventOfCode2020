// Hi, this is my AdventOfCode 2020 stuff

#include <assert.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>


////////////////////////////
////////////////////////////
// General tools

typedef long long BigInt;


////////////////////////////
////////////////////////////
// Problems

////////////////////////////
// Problem 1 - Report Repair

void InitReportRepairTestSet(std::set<BigInt>& set)
{
	printf("Initializing test set:  1721 979 366 299 675 1456\n");
	set.insert(1721);
	set.insert(979);
	set.insert(366);
	set.insert(299);
	set.insert(675);
	set.insert(1456);
}

void ReadReportRepairInput(const char* fileName, std::set<BigInt>& set)
{
	set.clear();

	FILE* pFile = fopen(fileName, "rt");
	assert(pFile);

	printf("\nReading input from %s:  ", fileName);
	char string[16];
	while ((fgets(string, sizeof(string), pFile) != nullptr) && !feof(pFile))
	{
		const BigInt integer = atoi(string);
		printf("%lld ", integer);
		set.insert(integer);
	}
	printf("\n");

	fclose(pFile);
}

void FindReportRepairTwoNums(const std::set<BigInt>& set)
{
	for (auto iter = set.cbegin(), iterEnd = set.cend(); iter != iterEnd; ++iter)
	{
		const BigInt thisNum = *iter;
		const BigInt otherNum = 2020 - thisNum;
		if (set.count(otherNum) > 0)
		{
			printf("Found two numbers that sum to 2020!  Product = %lld * %lld = %lld\n", thisNum, otherNum, thisNum * otherNum);
			break;
		}
	}
}

void FindReportRepairThreeNums(const std::set<BigInt>& set)
{
	for (auto iter = set.cbegin(), iterEnd = set.cend(); iter != iterEnd; ++iter)
	{
		const BigInt firstNum = *iter;

		auto iter2 = iter;
		++iter2;
		for (; iter2 != iterEnd; ++iter2)
		{
			const BigInt secondNum = *iter2;
			const BigInt partialSum = firstNum + secondNum;
			if (partialSum > 2020)
				break;

			const BigInt thirdNum = 2020 - partialSum;
			if (set.count(thirdNum) > 0)
			{
				printf("Found three numbers that sum to 2020!  Product = %lld * %lld * %lld = %lld\n", firstNum, secondNum, thirdNum, firstNum * secondNum * thirdNum);
				return;
			}
		}
	}
}

void RunReportRepair()
{
	printf("Report Repair\n\n");

	std::set<BigInt> set1;
	InitReportRepairTestSet(set1);
	FindReportRepairTwoNums(set1);
	FindReportRepairThreeNums(set1);

	std::set<BigInt> set2;
	ReadReportRepairInput("Day1Input.txt", set2);
	FindReportRepairTwoNums(set2);
	FindReportRepairThreeNums(set2);
}


////////////////////////////
// Problem 2 - Password Philosophy

void InitTestPasswordLines(std::vector<std::string>& lines)
{
	lines.clear();
	lines.push_back("1-3 a: abcde");
	lines.push_back("1-3 b: cdefg");
	lines.push_back("2-9 c: ccccccccc");
}

void ReadPasswordLines(const char* fileName, std::vector<std::string>& lines)
{
	lines.clear();

	FILE* pFile = fopen(fileName, "rt");
	assert(pFile);

	char string[1024];
	while ((fgets(string, sizeof(string), pFile) != nullptr) && !feof(pFile))
	{
		lines.push_back(string);
	}

	fclose(pFile);
}

void ParseNumber(const char*& pString, char untilChar, BigInt& number)
{
	char temp[64];
	char* pOutTemp = temp;
	while (*pString != untilChar)
	{
		assert(*pString >= '0');
		assert(*pString <= '9');

		*pOutTemp++ = *pString++;
	}
	*pOutTemp = 0;

	assert(*pString == untilChar);
	++pString;

	number = atoi(temp);
}

void ParsePasswordLine(const char* line, BigInt& min, BigInt& max, char& ch, std::string& password)
{
	ParseNumber(line, '-', min);
	ParseNumber(line, ' ', max);
	ch = *line++;
	assert(*line == ':');
	++line;
	assert(*line == ' ');
	++line;

	password.clear();
	while ((*line != '\n') && (*line != 0))
	{
		password.push_back(*line);
		++line;
	}
}

BigInt CountValidPasswords(const std::vector<std::string>& lines)
{
	BigInt numValidPasswords = 0;

	std::string password;
	for (auto iter = lines.begin(); iter != lines.end(); ++iter)
	{
		printf("Parsing password line:  %s\n", iter->c_str());

		BigInt min;
		BigInt max;
		char ch;
		ParsePasswordLine(iter->c_str(), min, max, ch, password);

		printf("  min = %lld, max = %lld, ch = %c, password = '%s'\n", min, max, ch, password.c_str());

		BigInt numCh = 0;
		for (BigInt i = 0; i < (BigInt)password.length(); ++i)
		{
			if (password[i] == ch)
			{
				++numCh;
				if (numCh > max)
					break;
			}
		}

		if ((numCh >= min) && (numCh <= max))
		{
			++numValidPasswords;
			printf("  Password VALID!\n");
		}
		else
		{
			printf("  Password invalid\n");
		}
	}

	return numValidPasswords;
}

void RunPasswordPhilosophy()
{
	std::vector<std::string> passwordLines1;
	InitTestPasswordLines(passwordLines1);
	printf("Num valid test passwords = %lld\n", CountValidPasswords(passwordLines1));

	std::vector<std::string> passwordLines2;
	ReadPasswordLines("Day2Input.txt", passwordLines2);
	printf("Num valid test passwords = %lld\n", CountValidPasswords(passwordLines2));
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
	case 2:
		RunPasswordPhilosophy();
		break;
	default:
		printf("'%s' is not a valid problem number!\n\n", problemArg);
		break;
	}

	return 0;
}

