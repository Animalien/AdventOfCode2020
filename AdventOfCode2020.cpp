// Hi, this is my AdventOfCode 2020 stuff

#include <assert.h>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>


////////////////////////////
////////////////////////////
// General tools

typedef long long BigInt;

void ReadFileLines(const char* fileName, std::vector<std::string>& lines)
{
	lines.clear();

	FILE* pFile = fopen(fileName, "rt");
	assert(pFile);

	char string[1024];
	for (;;)
	{
		const char* fgetsRet = fgets(string, sizeof(string), pFile);
		if (!fgetsRet)
			break;

		const BigInt len = strlen(string);
		if ((len > 0) && (string[len - 1] == '\n'))
			string[len - 1] = 0;
		lines.push_back(string);

		if (feof(pFile))
			break;
	}

	fclose(pFile);
}


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
	ReadReportRepairInput("Input\\Day1Input.txt", set2);
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

void ParsePasswordLine(const char* line, BigInt& num1, BigInt& num2, char& ch, std::string& password)
{
	ParseNumber(line, '-', num1);
	ParseNumber(line, ' ', num2);
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

BigInt CountValidPasswords(const std::vector<std::string>& lines, bool newScheme, bool verbose)
{
	BigInt numValidPasswords = 0;

	std::string password;
	for (auto iter = lines.begin(); iter != lines.end(); ++iter)
	{
		if (verbose)
			printf("Parsing password line:  %s\n", iter->c_str());

		BigInt num1;
		BigInt num2;
		char ch;
		ParsePasswordLine(iter->c_str(), num1, num2, ch, password);

		if (verbose)
			printf("  num1 = %lld, num2 = %lld, ch = %c, password = '%s'\n", num1, num2, ch, password.c_str());

		if (newScheme)
		{
			BigInt numMatches = 0;

			if (password[num1 - 1] == ch)
				++numMatches;
			if (password[num2 - 1] == ch)
				++numMatches;

			if (numMatches == 1)
			{
				++numValidPasswords;
				if (verbose)
					printf("  Password VALID!\n");
			}
			else
			{
				if (verbose)
					printf("  Password invalid\n");
			}
		}
		else
		{
			BigInt numCh = 0;
			for (BigInt i = 0; i < (BigInt)password.length(); ++i)
			{
				if (password[i] == ch)
				{
					++numCh;
					if (numCh > num2)
						break;
				}
			}

			if ((numCh >= num1) && (numCh <= num2))
			{
				++numValidPasswords;
				if (verbose)
					printf("  Password VALID!\n");
			}
			else
			{
				if (verbose)
					printf("  Password invalid\n");
			}
		}
	}

	return numValidPasswords;
}

void RunPasswordPhilosophy()
{
	std::vector<std::string> passwordLines1;
	InitTestPasswordLines(passwordLines1);
	printf("Num valid test passwords (first scheme) = %lld\n", CountValidPasswords(passwordLines1, false, true));
	printf("Num valid test passwords (second scheme) = %lld\n", CountValidPasswords(passwordLines1, true, true));

	std::vector<std::string> passwordLines2;
	ReadFileLines("Input\\Day2Input.txt", passwordLines2);
	printf("Num valid test passwords (first scheme) = %lld\n", CountValidPasswords(passwordLines2, false, false));
	printf("Num valid test passwords (second scheme) = %lld\n", CountValidPasswords(passwordLines2, true, false));
}


////////////////////////////
// Problem 3 - Toboggan Trajectory

void InitTobogTrajTestData(std::vector<std::string>& data)
{
	data.clear();
	data.push_back("..##.......");
	data.push_back("#...#...#..");
	data.push_back(".#....#..#.");
	data.push_back("..#.#...#.#");
	data.push_back(".#...##..#.");
	data.push_back("..#.##.....");
	data.push_back(".#.#.#....#");
	data.push_back(".#........#");
	data.push_back("#.##...#...");
	data.push_back("#...##....#");
	data.push_back(".#..#...#.#");
}

BigInt CountTobogTrajTrees(const std::vector<std::string>& data, BigInt rightStep, BigInt downStep, bool verbose)
{
	BigInt numTrees = 0;

	const BigInt lineSize = data[0].length();

	BigInt xPos = 0;
	BigInt yPos = 0;

	while (yPos < (BigInt)data.size())
	{
		assert((BigInt)data[yPos].length() == lineSize);

		const bool hitTree = (data[yPos][xPos] == '#');
		if (hitTree)
			++numTrees;

		if (verbose)
		{
			static std::string lineCopy;
			lineCopy = data[yPos];
			lineCopy[xPos] = hitTree ? 'X' : 'O';
			printf("%s\n", lineCopy.c_str());
		}

		xPos = (xPos + rightStep) % lineSize;
		yPos += downStep;
	}

	return numTrees;
}

void IterateProdNumTreesDifferentSlopes(const std::vector<std::string>& data, BigInt rightStep, BigInt downStep, BigInt& prod)
{
	const BigInt count = CountTobogTrajTrees(data, rightStep, downStep, false);
	printf("Num trees with slope (%lld,%lld) = %lld\n", rightStep, downStep, count);
	prod *= count;
}

void CalcProdNumTreesDifferentSlopes(const std::vector<std::string>& data)
{
	BigInt prod = 1;

	IterateProdNumTreesDifferentSlopes(data, 1, 1, prod);
	IterateProdNumTreesDifferentSlopes(data, 3, 1, prod);
	IterateProdNumTreesDifferentSlopes(data, 5, 1, prod);
	IterateProdNumTreesDifferentSlopes(data, 7, 1, prod);
	IterateProdNumTreesDifferentSlopes(data, 1, 2, prod);
	printf("Grand product = %lld\n", prod);
}

void RunTobogganTrajectory()
{
	const BigInt rightStep = 3;
	const BigInt downStep = 1;

	std::vector<std::string> testData;
	InitTobogTrajTestData(testData);
	printf("Num trees encountered in test data with slope (%lld,%lld) = %lld\n", rightStep, downStep, CountTobogTrajTrees(testData, rightStep, downStep, true));
	CalcProdNumTreesDifferentSlopes(testData);

	std::vector<std::string> fileData;
	ReadFileLines("Input\\Day3Input.txt", fileData);
	printf("Num trees encountered in file data with slope (%lld,%lld) = %lld\n", rightStep, downStep, CountTobogTrajTrees(fileData, rightStep, downStep, false));
	CalcProdNumTreesDifferentSlopes(fileData);
}


////////////////////////////
// Problem 4 - Passport Processing

struct PassportEntry
{
	std::string byr;
	std::string iyr;
	std::string eyr;
	std::string hgt;
	std::string hcl;
	std::string ecl;
	std::string pid;
	std::string cid;

	void ReadField(const char* fieldName, const char* fieldValue, bool verbose);

	bool IsValid() const;

private:
	struct Comp
	{
		bool operator()(const char* a, const char* b) const
		{
			return (strcmp(a, b) < 0);
		}
	};

	typedef std::string PassportEntry::* FieldPtr;
	static const std::map<const char*, FieldPtr, Comp> s_fieldMap;
};

const std::map<const char*, PassportEntry::FieldPtr, PassportEntry::Comp> PassportEntry::s_fieldMap =
{
	{ "byr", &PassportEntry::byr },
	{ "iyr", &PassportEntry::iyr },
	{ "eyr", &PassportEntry::eyr },
	{ "hgt", &PassportEntry::hgt },
	{ "hcl", &PassportEntry::hcl },
	{ "ecl", &PassportEntry::ecl },
	{ "pid", &PassportEntry::pid },
	{ "cid", &PassportEntry::cid },
};

void PassportEntry::ReadField(const char* fieldName, const char* fieldValue, bool verbose)
{
	auto iter = s_fieldMap.find(fieldName);
	assert(iter != s_fieldMap.end());

	auto pMember = iter->second;
	this->*pMember = fieldValue;

	if (verbose)
		printf("Read field '%s' = '%s'\n", fieldName, fieldValue);
}

bool StringHasDigits(const std::string& st, BigInt start = 0, BigInt end = -1)
{
	if (end <= 0)
		end = st.length() - 1;

	for (BigInt i = start; i <= end; ++i)
	{
		const char ch = st[i];
		if (ch < '0')
			return false;
		if (ch > '9')
			return false;
	}

	return true;
}

bool StringHasLowerCaseAlphaNumeric(const std::string& st, BigInt start = 0, BigInt end = -1)
{
	if (end <= 0)
		end = st.length() - 1;

	for (BigInt i = start; i <= end; ++i)
	{
		const char ch = st[i];
		if ((ch >= '0') && (ch <= '9'))
			continue;
		if ((ch >= 'a') && (ch <= 'z'))
			continue;
		return false;
	}

	return true;
}

bool StringIsIntWithinRange(const std::string& st, BigInt min, BigInt max, BigInt skipLastChars = 0)
{
	if (st.empty())
		return false;
	if (!StringHasDigits(st, 0, st.length() - skipLastChars - 1))
		return false;

	const BigInt value = atoi(st.c_str());
	return ((value >= min) && (value <= max));
}

bool StringIsIntWithinRangeAndSuffix(const std::string& st, BigInt min, BigInt max, const char* suffix, BigInt suffixLen)
{
	if ((BigInt)st.length() <= suffixLen)
		return false;
	if (st.compare(st.length() - suffixLen, suffixLen, suffix) != 0)
		return false;

	return StringIsIntWithinRange(st, min, max, suffixLen);
}

bool PassportEntry::IsValid() const
{
	if (!StringIsIntWithinRange(byr, 1920, 2002))
		return false;

	if (!StringIsIntWithinRange(iyr, 2010, 2020))
		return false;

	if (!StringIsIntWithinRange(eyr, 2020, 2030))
		return false;

	if (!StringIsIntWithinRangeAndSuffix(hgt, 150, 193, "cm", 2) &&
		!StringIsIntWithinRangeAndSuffix(hgt, 59, 76, "in", 2))
		return false;

	if ((hcl.length() != 7) ||
		(hcl[0] != '#') ||
		!StringHasLowerCaseAlphaNumeric(hcl, 1))
		return false;

	if (ecl.empty() ||
		((ecl != "amb") &&
			(ecl != "blu") &&
			(ecl != "brn") &&
			(ecl != "gry") &&
			(ecl != "grn") &&
			(ecl != "hzl") &&
			(ecl != "oth")))
		return false;

	if ((pid.length() != 9) ||
		!StringHasDigits(pid))
		return false;

	// ignore cid presence and contents

	return true;
}

void ReadPassportFile(const char* fileName, std::vector<PassportEntry>& data, bool verbose)
{
	FILE* pFile = fopen(fileName, "rt");
	assert(pFile);

	PassportEntry* pEntry = nullptr;
	std::string fieldName;
	std::string fieldValue;
	std::string* pStringReading = &fieldName;

	int ch = 0;
	int chPrev = 0;
	for (;;)
	{
		ch = fgetc(pFile);
		if ((ch == '\n') || (ch == ' ') || (ch == EOF))
		{
			if (!fieldValue.empty())
			{
				assert(!fieldName.empty());
				assert(pEntry != nullptr);

				pEntry->ReadField(fieldName.c_str(), fieldValue.c_str(), verbose);
				fieldName.clear();
				fieldValue.clear();
				pStringReading = &fieldName;
			}

			if ((ch == '\n') && (chPrev == '\n'))
			{
				if (verbose && pEntry)
					printf("Entry is %s\n", pEntry->IsValid() ? "VALID" : "INVALID");

				pEntry = nullptr;
			}
			else if (ch == EOF)
				break;
		}
		else if (ch == ':')
		{
			assert(!fieldName.empty());
			assert(fieldValue.empty());

			pStringReading = &fieldValue;
		}
		else
		{
			if (pEntry == nullptr)
			{
				assert(fieldName.empty());
				assert(fieldValue.empty());
				assert(pStringReading == &fieldName);

				data.push_back(PassportEntry());
				pEntry = &(data.back());

				if (verbose)
					printf("\nNew passport entry started\n");
			}

			pStringReading->push_back((char)ch);
		}

		chPrev = ch;
	}

	if (verbose && pEntry)
		printf("Entry is %s\n", pEntry->IsValid() ? "VALID" : "INVALID");
}

BigInt CountValidPassports(const std::vector<PassportEntry>& data)
{
	BigInt count = 0;

	for (BigInt i = 0; i < (BigInt)data.size(); ++i)
	{
		if (data[i].IsValid())
			++count;
	}

	return count;
}

void RunPassportProcessing()
{
	std::vector<PassportEntry> testData;
	ReadPassportFile("Input\\Day4TestInput.txt", testData, true);
	printf("Num valid passports in test data = %lld\n", CountValidPassports(testData));

	std::vector<PassportEntry> invalidData;
	ReadPassportFile("Input\\Day4InvalidInput.txt", invalidData, true);
	std::vector<PassportEntry> validData;
	ReadPassportFile("Input\\Day4ValidInput.txt", validData, true);

	std::vector<PassportEntry> data;
	ReadPassportFile("Input\\Day4Input.txt", data, false);
	printf("Num valid passports in data = %lld\n", CountValidPassports(data));
}


////////////////////////////
// Problem 5 - Binary Boarding

void IterateBoardingPass(BigInt& min, BigInt& max, bool tiltLarge)
{
	assert(min < max);
	
	const BigInt diff = max - min;
	assert((diff & 1) != 0);

	const BigInt half = (diff + 1) / 2;

	if (tiltLarge)
	{
		min += half;
	}
	else
	{
		max -= half;
	}
}

BigInt CalcBoardingPassSeatID(const char* pass, bool verbose)
{
	assert(pass != nullptr);
	assert(strlen(pass) == 10);

	if (verbose)
		printf("Pass = %s\n", pass);

	BigInt min = 0;
	BigInt max = 127;

	for (BigInt i = 0; i < 7; ++i)
	{
		assert(*pass != 0);
		assert((*pass == 'F') || (*pass == 'B'));

		if (verbose)
			printf("  (%lld,%lld) -> %c -> ", min, max, *pass);

		IterateBoardingPass(min, max, *pass == 'B');

		if (verbose)
			printf("(%lld,%lld)\n", min, max);

		++pass;
	}
	assert(min == max);

	const BigInt row = min;
	if (verbose)
		printf("  row = %lld\n", row);

	min = 0;
	max = 7;

	for (BigInt i = 0; i < 3; ++i)
	{
		assert(*pass != 0);
		assert((*pass == 'L') || (*pass == 'R'));

		if (verbose)
			printf("  (%lld,%lld) -> %c -> ", min, max, *pass);

		IterateBoardingPass(min, max, *pass == 'R');

		if (verbose)
			printf("(%lld,%lld)\n", min, max);

		++pass;
	}
	assert(min == max);

	const BigInt column = min;
	if (verbose)
		printf("  column = %lld\n", column);

	const BigInt seatID = (row * 8) + column;
	if (verbose)
		printf("  seat ID = %lld\n", seatID);

	return seatID;
}

void CalcSeatIDs(const std::vector<std::string>& data, std::set<BigInt>& seatIDs)
{
	seatIDs.clear();
	for (BigInt i = 0; i < (BigInt)data.size(); ++i)
	{
		const BigInt seatID = CalcBoardingPassSeatID(data[i].c_str(), false);
		seatIDs.insert(seatID);
		printf("Pass %s -> Seat ID %lld\n", data[i].c_str(), seatID);
	}
}

BigInt FindSmallestSeatID(const std::set<BigInt>& seatIDs)
{
	auto iter = seatIDs.cbegin();
	if (iter == seatIDs.cend())
		return -1;

	return *iter;
}

BigInt FindLargestSeatID(const std::set<BigInt>& seatIDs)
{
	auto iter = seatIDs.crbegin();
	if (iter == seatIDs.crend())
		return -1;

	return *iter;
}

void DecodeSeatID(BigInt seatID, BigInt& row, BigInt& column)
{
	row = seatID >> 3;
	column = seatID & 7;
}

BigInt FindMySeatID(const std::set<BigInt>& seatIDs)
{
	BigInt prevSeatID = -1;

	const BigInt smallestSeatID = FindSmallestSeatID(seatIDs);
	BigInt smallestRow, smallestColumn;
	DecodeSeatID(smallestSeatID, smallestRow, smallestColumn);

	const BigInt largestSeatID = FindLargestSeatID(seatIDs);
	BigInt largestRow, largestColumn;
	DecodeSeatID(largestSeatID, largestRow, largestColumn);

	printf("Seat IDs range from %lld (%lld,%lld) to %lld (%lld,%lld)\n", smallestSeatID, smallestRow, smallestColumn, largestSeatID, largestRow, largestColumn);

	BigInt mySeatID = -1;
	for (auto iter = seatIDs.cbegin(); iter != seatIDs.cend(); ++iter)
	{
		const BigInt seatID = *iter;
		
		if (prevSeatID > 0)
		{
			const BigInt diff = seatID - prevSeatID;
			if (diff > 1)
			{
				printf("Found gap between %lld and %lld\n", prevSeatID, seatID);

				BigInt row, column;
				DecodeSeatID(seatID, row, column);
				printf("Seat ID %lld = row %lld, column %lld\n", seatID, row, column);

				assert(mySeatID < 0);
				assert(diff == 2);
				mySeatID = seatID - 1;
			}
		}

		prevSeatID = seatID;
	}

	return mySeatID;
}

void RunBinaryBoarding()
{
	CalcBoardingPassSeatID("FBFBBFFRLR", true);
	CalcBoardingPassSeatID("BFFFBBFRRR", true);
	CalcBoardingPassSeatID("FFFBBBFRRR", true);
	CalcBoardingPassSeatID("BBFFBBFRLL", true);

	std::vector<std::string> data;
	ReadFileLines("Input\\Day5Input.txt", data);

	std::set<BigInt> seatIDs;
	CalcSeatIDs(data, seatIDs);
	printf("Largest seat ID = %lld\n", FindLargestSeatID(seatIDs));

	printf("My seat ID = %lld\n", FindMySeatID(seatIDs));
}


////////////////////////////
// Problem 6 - Custom Customs

BigInt CalcSumQuestionCounts(const std::vector<std::string>& data, bool verbose)
{
	BigInt sum = 0;

	std::set<char> questionSet;

	for (BigInt i=0; i<(BigInt)data.size(); ++i)
	{
		if (data[i].empty())
		{
			sum += questionSet.size();
			if (verbose)
				printf("Counted %lld answers\n", questionSet.size());
			questionSet.clear();
		}
		else
		{
			const std::string& answers = data[i];
			for (BigInt j = 0; j < (BigInt)answers.length(); ++j)
			{
				questionSet.insert(answers[j]);
			}
		}
	}

	sum += questionSet.size();
	if (verbose)
		printf("Counted %lld answers\n", questionSet.size());

	return sum;
}

void RunCustomCustoms()
{
	std::vector<std::string> testData;
	ReadFileLines("Input\\Day6TestInput.txt", testData);
	printf("Sum question counts of test data = %lld\n", CalcSumQuestionCounts(testData, true));

	std::vector<std::string> data;
	ReadFileLines("Input\\Day6Input.txt", data);
	printf("Sum question counts = %lld\n", CalcSumQuestionCounts(data, true));
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
	case 3:
		RunTobogganTrajectory();
		break;
	case 4:
		RunPassportProcessing();
		break;
	case 5:
		RunBinaryBoarding();
		break;
	case 6:
		RunCustomCustoms();
		break;
	default:
		printf("'%s' is not a valid problem number!\n\n", problemArg);
		break;
	}

	return 0;
}

