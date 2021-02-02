// Hi, this is my AdventOfCode 2020 stuff

#include <assert.h>
#include <deque>
#include <limits.h>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


////////////////////////////
////////////////////////////
// Shared tools

////////////////////////////
// Basic Types

typedef long long BigInt;
typedef std::initializer_list<BigInt> BigIntInitList;
typedef std::vector<BigInt> BigIntList;
typedef std::vector<BigIntList> BigIntListList;
typedef std::map<BigInt, BigInt> BigIntMap;
typedef std::set<BigInt> BigIntSet;
typedef std::deque<BigInt> BigIntDeque;

typedef unsigned long long BigUInt;

const BigInt MAX_BIG_INT = LLONG_MAX;
const BigInt MIN_BIG_INT = LLONG_MIN;
const BigInt MAX_BIG_UINT = ULLONG_MAX;

typedef std::vector<bool> BoolList;


////////////////////////////
// Factorization

class Factorization : public BigIntMap
{
public:
    Factorization() : BigIntMap() {}

    bool IsPrime() const { return ((size() == 1) && (begin()->second == 1)); }

    void Absorb(const Factorization& other)
    {
        for (auto iter = other.begin(); iter != other.end(); ++iter)
        {
            Absorb(iter->first, iter->second);
        }
    }

    BigInt CalcProduct() const
    {
        BigInt product = 1;
        for (auto iter = begin(); iter != end(); ++iter)
        {
            for (BigInt i = 0; i < iter->second; ++i)
            {
                product *= iter->first;
            }
        }
        return product;
    }

    void PrintFactors() const
    {
        for (auto iter = begin(); iter != end(); ++iter)
        {
            printf("(%lldn of %lld) ", iter->second, iter->first);
        }
    }

    BigInt CalcNumDivisors() const
    {
        if (IsPrime())
        {
            // if prime, then number of divisors is simply:  1, and itself
            return 2;
        }

        BigInt numDivisors = 1;
        // the number of divisors will be the numbers of combinations of prime factors.
        // in a given divisor, each prime factor can be included from 0 to N times, where
        // N is the number of times that prime factor exists in the original number.
        // (the divisor with ZERO of any prime factors included, is the divisor 1, which every number has.)
        for (auto iter = begin(); iter != end(); ++iter)
        {
            numDivisors *= (iter->second + 1);
        }
        // add 1 more for the original number, being one of its own divisors
        numDivisors += 1;

        return numDivisors;
    }

private:
    void Absorb(BigInt number, BigInt numFactors)
    {
        auto iter = find(number);
        if (iter != end())
        {
            iter->second = std::max(iter->second, numFactors);
        }
        else
        {
            insert(value_type(number, numFactors));
        }
    }
};

class FactorizationCache : public std::map<BigInt, Factorization>
{
public:
    FactorizationCache() : std::map<BigInt, Factorization>() {}

    void PopulateCache(BigInt num) { Factorize(num * 2); }

    const Factorization& Factorize(BigInt num)
    {
        iterator fiter = find(num);
        if (fiter == end())
        {
            fiter = NewFactorize(num);
        }

        return fiter->second;
    }

private:
    iterator NewFactorize(BigInt num)
    {
        auto newValue = insert(value_type(num, Factorization()));
        iterator newIter = newValue.first;
        Factorization& newFactorization = newIter->second;

        const BigInt halfNum = num / 2;
        BigInt prodRemaining = num;
        for (BigInt i = 2; i <= halfNum; ++i)
        {
            const Factorization& f = Factorize(i);
            if (f.IsPrime())
            {
                // i is prime, so see how many i factors fit into this number
                BigInt numFactors = 0;
                for (;;)
                {
                    const lldiv_t divRem = lldiv(prodRemaining, i);
                    if (divRem.rem != 0)
                    {
                        break;
                    }
                    ++numFactors;
                    prodRemaining = divRem.quot;
                }
                if (numFactors > 0)
                {
                    newFactorization.emplace(i, numFactors);
                }
            }

            if (prodRemaining == 1)
            {
                break;
            }
        }
        if (newFactorization.empty())
        {
            newFactorization.emplace(num, 1);
        }

        return newIter;
    }
};

static FactorizationCache s_factorizationCache;


////////////////////////////
// Strings

typedef std::vector<std::string> StringList;
typedef std::vector<StringList> StringListList;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringMap;
typedef std::unordered_set<std::string> UnorderedStringSet;

static const char* fileNameBase = "..\\Input\\";

void ReadFileLines(const char* fileName, StringList& lines)
{
    lines.clear();

    std::string fullFileName = fileNameBase;
    fullFileName += fileName;
    FILE* pFile = fopen(fullFileName.c_str(), "rt");
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

void Tokenize(const std::string& st, StringList& tokens, char delim)
{
    std::stringstream stream(st);

    tokens.clear();
    std::string token;
    while (std::getline(stream, token, delim))
    {
        tokens.push_back(token);
    }
}

void ParseBigIntList(const std::string& st, BigIntList& intList, char delim)
{
    StringList tokens;
    Tokenize(st, tokens, delim);

    intList.clear();
    for (const auto& intString: tokens)
    {
        intList.push_back(atoll(intString.c_str()));
    }
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

void MultiplyStringLists(StringList& origList, const StringList& rhsList)
{
    if (origList.empty())
    {
        origList = rhsList;
        return;
    }

    StringList newList;
    for (const auto& lhs: origList)
    {
        for (const auto& rhs: rhsList)
        {
            newList.push_back(lhs + rhs);
        }
    }

    origList.swap(newList);
}


////////////////////////////
// Sets

template<typename T>
void IntersectSet(std::set<T>& lhs, const std::set<T>& rhs)
{
    std::set<T> newSet;

    auto iterLeft = lhs.cbegin();
    const auto iterLeftEnd = lhs.cend();

    auto iterRight = rhs.cbegin();
    const auto iterRightEnd = rhs.cend();

    for (;;)
    {
        if (iterLeft == iterLeftEnd)
            break;
        if (iterRight == iterRightEnd)
            break;

        const T& leftValue = *iterLeft;
        const T& rightValue = *iterRight;

        if (leftValue == rightValue)
        {
            newSet.insert(*iterLeft);
            ++iterLeft;
            ++iterRight;
        }
        else if (leftValue < rightValue)
        {
            ++iterLeft;
        }
        else
        {
            ++iterRight;
        }
    }

    lhs.swap(newSet);
}


////////////////////////////
// Circular Buffer

template<typename T, BigInt NUM_CAPACITY_BITS>
class CircularBuffer
{
public:
    CircularBuffer() : m_buffer(), m_numInBuffer(0), m_readIndex(0), m_writeIndex(0) {}
    CircularBuffer(const CircularBuffer& other) : m_buffer(), m_numInBuffer(0), m_readIndex(0), m_writeIndex(0) { *this = other; }

    BigInt GetCapacity() const { return CAPACITY; }
    BigInt GetSize() const { return m_numInBuffer; }
    bool IsEmpty() const { return m_numInBuffer == 0; }

    void Clear() { m_numInBuffer = m_readIndex = m_writeIndex = 0; }

    CircularBuffer& operator=(const CircularBuffer& rhs)
    {
        Clear();
        for (const T& item: rhs)
            Write(item);
        return *this;
    }

    const T& PeekRead() const
    {
        assert(!IsEmpty());
        return m_buffer[m_readIndex];
    }
    T Read()
    {
        const T retVal = PeekRead();
        PopRead();
        return retVal;
    }
    void PopRead()
    {
        assert(!IsEmpty());
        m_readIndex = IncrementIndex(m_readIndex);
        --m_numInBuffer;
    }
    void PopWrite()
    {
        assert(!IsEmpty());
        m_writeIndex = DecrementIndex(m_writeIndex);
        --m_numInBuffer;
    }

    void Write(const T& newValue)
    {
        m_buffer[m_writeIndex] = newValue;
        m_writeIndex = IncrementIndex(m_writeIndex);
        ++m_numInBuffer;
    }

    struct ConstIterator
    {
        const CircularBuffer& buffer;
        BigInt index;

        ConstIterator(const CircularBuffer& b, BigInt i) : buffer(b), index(i) {}
        ConstIterator(const ConstIterator& other) : buffer(other.buffer), index(other.index) {}

        bool operator==(const ConstIterator& rhs) const { return ((&buffer == &rhs.buffer) && (index == rhs.index)); }
        bool operator!=(const ConstIterator& rhs) const { return ((&buffer != &rhs.buffer) || (index != rhs.index)); }

        const T& operator*() const { return buffer.m_buffer[index]; }

        void operator++() { index = StepIndex(index, +1); }
    };
    struct Iterator
    {
        CircularBuffer& buffer;
        BigInt index;

        Iterator(CircularBuffer& b, BigInt i) : buffer(b), index(i) {}
        Iterator(const Iterator& other) : buffer(other.buffer), index(other.index) {}

        bool operator==(const Iterator& rhs) const { return ((&buffer == &rhs.buffer) && (index == rhs.index)); }
        bool operator!=(const Iterator& rhs) const { return ((&buffer != &rhs.buffer) || (index != rhs.index)); }

        T& operator*() const { return buffer.m_buffer[index]; }

        void operator++() { index = StepIndex(index, +1); }
    };

    bool operator==(const CircularBuffer& rhs) const
    {
        if (GetSize() != rhs.GetSize())
            return false;
        ConstIterator iter = cbegin();
        const ConstIterator iterEnd = cend();
        ConstIterator rhsIter = rhs.cbegin();
        for (; iter != iterEnd; ++iter, ++rhsIter)
        {
            if (*iter != *rhsIter)
                return false;
        }

        return true;
    }
    bool operator!=(const CircularBuffer& rhs) const { return !(*this == rhs); }

    // STL

    bool empty() const { return IsEmpty(); }
    size_t size() const { return GetSize(); }
    void resize(size_t newSize)
    {
        const BigInt sizeDiff = (BigInt)newSize - GetSize();
        if (sizeDiff > 0)
        {
            while (sizeDiff > 0)
            {
                Write(T());
                --sizeDiff;
            }
        }
        else
        {
            while (sizeDiff < 0)
            {
                PopWrite();
                ++sizeDiff;
            }
        }
    }
    const T& front() const { return PeekRead(); }
    void pop_front() { PopRead(); }
    void push_back(const T& newValue) { Write(newValue); }
    ConstIterator cbegin() const { return ConstIterator(*this, m_readIndex); }
    ConstIterator cend() const { return ConstIterator(*this, m_writeIndex); }
    ConstIterator begin() const { return ConstIterator(*this, m_readIndex); }
    ConstIterator end() const { return ConstIterator(*this, m_writeIndex); }
    Iterator begin() { return Iterator(*this, m_readIndex); }
    Iterator end() { return Iterator(*this, m_writeIndex); }

private:
    static const BigInt CAPACITY = 1LL << NUM_CAPACITY_BITS;
    static const BigInt CAPACITY_MASK = CAPACITY - 1;

    static BigInt IncrementIndex(BigInt index) { return StepIndex(index, +1); }
    static BigInt DecrementIndex(BigInt index) { return StepIndex(index, -1); }
    static BigInt StepIndex(BigInt index, BigInt step) { return (index + step + CAPACITY) & CAPACITY_MASK; }

    T m_buffer[CAPACITY];
    BigInt m_numInBuffer;
    BigInt m_readIndex;
    BigInt m_writeIndex;
};



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
                printf(
                    "Found three numbers that sum to 2020!  Product = %lld * %lld * %lld = %lld\n",
                    firstNum,
                    secondNum,
                    thirdNum,
                    firstNum * secondNum * thirdNum);
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

void InitTestPasswordLines(StringList& lines)
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

BigInt CountValidPasswords(const StringList& lines, bool newScheme, bool verbose)
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
    StringList passwordLines1;
    InitTestPasswordLines(passwordLines1);
    printf("Num valid test passwords (first scheme) = %lld\n", CountValidPasswords(passwordLines1, false, true));
    printf("Num valid test passwords (second scheme) = %lld\n", CountValidPasswords(passwordLines1, true, true));

    StringList passwordLines2;
    ReadFileLines("Day2Input.txt", passwordLines2);
    printf("Num valid test passwords (first scheme) = %lld\n", CountValidPasswords(passwordLines2, false, false));
    printf("Num valid test passwords (second scheme) = %lld\n", CountValidPasswords(passwordLines2, true, false));
}


////////////////////////////
// Problem 3 - Toboggan Trajectory

void InitTobogTrajTestData(StringList& data)
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

BigInt CountTobogTrajTrees(const StringList& data, BigInt rightStep, BigInt downStep, bool verbose)
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

void IterateProdNumTreesDifferentSlopes(const StringList& data, BigInt rightStep, BigInt downStep, BigInt& prod)
{
    const BigInt count = CountTobogTrajTrees(data, rightStep, downStep, false);
    printf("Num trees with slope (%lld,%lld) = %lld\n", rightStep, downStep, count);
    prod *= count;
}

void CalcProdNumTreesDifferentSlopes(const StringList& data)
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

    StringList testData;
    InitTobogTrajTestData(testData);
    printf(
        "Num trees encountered in test data with slope (%lld,%lld) = %lld\n",
        rightStep,
        downStep,
        CountTobogTrajTrees(testData, rightStep, downStep, true));
    CalcProdNumTreesDifferentSlopes(testData);

    StringList fileData;
    ReadFileLines("Day3Input.txt", fileData);
    printf(
        "Num trees encountered in file data with slope (%lld,%lld) = %lld\n",
        rightStep,
        downStep,
        CountTobogTrajTrees(fileData, rightStep, downStep, false));
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
        bool operator()(const char* a, const char* b) const { return (strcmp(a, b) < 0); }
    };

    typedef std::string PassportEntry::*FieldPtr;
    static const std::map<const char*, FieldPtr, Comp> s_fieldMap;
};

const std::map<const char*, PassportEntry::FieldPtr, PassportEntry::Comp> PassportEntry::s_fieldMap = {
    { "byr", &PassportEntry::byr }, { "iyr", &PassportEntry::iyr }, { "eyr", &PassportEntry::eyr },
    { "hgt", &PassportEntry::hgt }, { "hcl", &PassportEntry::hcl }, { "ecl", &PassportEntry::ecl },
    { "pid", &PassportEntry::pid }, { "cid", &PassportEntry::cid },
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

bool PassportEntry::IsValid() const
{
    if (!StringIsIntWithinRange(byr, 1920, 2002))
        return false;

    if (!StringIsIntWithinRange(iyr, 2010, 2020))
        return false;

    if (!StringIsIntWithinRange(eyr, 2020, 2030))
        return false;

    if (!StringIsIntWithinRangeAndSuffix(hgt, 150, 193, "cm", 2) && !StringIsIntWithinRangeAndSuffix(hgt, 59, 76, "in", 2))
        return false;

    if ((hcl.length() != 7) || (hcl[0] != '#') || !StringHasLowerCaseAlphaNumeric(hcl, 1))
        return false;

    if (ecl.empty()
        || ((ecl != "amb") && (ecl != "blu") && (ecl != "brn") && (ecl != "gry") && (ecl != "grn") && (ecl != "hzl")
            && (ecl != "oth")))
        return false;

    if ((pid.length() != 9) || !StringHasDigits(pid))
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
    ReadPassportFile("Day4TestInput.txt", testData, true);
    printf("Num valid passports in test data = %lld\n", CountValidPassports(testData));

    std::vector<PassportEntry> invalidData;
    ReadPassportFile("Day4InvalidInput.txt", invalidData, true);
    std::vector<PassportEntry> validData;
    ReadPassportFile("Day4ValidInput.txt", validData, true);

    std::vector<PassportEntry> data;
    ReadPassportFile("Day4Input.txt", data, false);
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

void CalcSeatIDs(const StringList& data, std::set<BigInt>& seatIDs)
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

    printf(
        "Seat IDs range from %lld (%lld,%lld) to %lld (%lld,%lld)\n",
        smallestSeatID,
        smallestRow,
        smallestColumn,
        largestSeatID,
        largestRow,
        largestColumn);

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

    StringList data;
    ReadFileLines("Day5Input.txt", data);

    std::set<BigInt> seatIDs;
    CalcSeatIDs(data, seatIDs);
    printf("Largest seat ID = %lld\n", FindLargestSeatID(seatIDs));

    printf("My seat ID = %lld\n", FindMySeatID(seatIDs));
}


////////////////////////////
// Problem 6 - Custom Customs

BigInt CalcSumQuestionCountsAnyone(const StringList& data, bool verbose)
{
    BigInt sum = 0;

    std::set<char> questionSet;

    for (BigInt i = 0; i < (BigInt)data.size(); ++i)
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

BigInt CalcSumQuestionCountsEveryone(const StringList& data, bool verbose)
{
    BigInt sum = 0;

    std::set<char> intersectionQuestionSet;
    bool newPersonGroup = true;
    std::set<char> currQuestionSet;

    for (BigInt i = 0; i < (BigInt)data.size(); ++i)
    {
        if (data[i].empty())
        {
            sum += intersectionQuestionSet.size();
            if (verbose)
                printf("Counted %lld answers\n", intersectionQuestionSet.size());
            intersectionQuestionSet.clear();
            newPersonGroup = true;
        }
        else
        {
            currQuestionSet.clear();
            const std::string& answers = data[i];
            for (BigInt j = 0; j < (BigInt)answers.length(); ++j)
            {
                currQuestionSet.insert(answers[j]);
            }

            if (newPersonGroup)
                intersectionQuestionSet = currQuestionSet;
            else
                IntersectSet(intersectionQuestionSet, currQuestionSet);
            newPersonGroup = false;
        }
    }

    sum += intersectionQuestionSet.size();
    if (verbose)
        printf("Counted %lld answers\n", intersectionQuestionSet.size());

    return sum;
}

void RunCustomCustoms()
{
    StringList testData;
    ReadFileLines("Day6TestInput.txt", testData);
    printf("Sum question counts (anyone style) of test data = %lld\n", CalcSumQuestionCountsAnyone(testData, false));
    printf("Sum question counts (everyone style) of test data = %lld\n", CalcSumQuestionCountsEveryone(testData, true));

    StringList data;
    ReadFileLines("Day6Input.txt", data);
    printf("Sum question counts (anyone style) = %lld\n", CalcSumQuestionCountsAnyone(data, false));
    printf("Sum question counts (everyone style) = %lld\n", CalcSumQuestionCountsEveryone(data, false));
}



////////////////////////////
// Problem 7 - Handy Haversacks

struct Haversack
{
    std::string type;
    std::map<std::string, BigInt> contains;
    std::set<std::string> canBeContainedBy;
};

const Haversack& GetConstHaversackFromData(const std::map<std::string, Haversack>& data, const std::string& type)
{
    auto findHaver = data.find(type);
    assert(findHaver != data.end());

    return findHaver->second;
}

Haversack& GetHaversackFromData(std::map<std::string, Haversack>& data, const std::string& type)
{
    auto insertHaver = data.insert(std::pair<std::string, Haversack>(type, Haversack()));
    auto nodeIter = insertHaver.first;
    Haversack& rHaversack = nodeIter->second;
    if (insertHaver.second)
        rHaversack.type = type;
    return rHaversack;
}

void ReadHaversackData(const char* fileName, std::map<std::string, Haversack>& data)
{
    data.clear();

    StringList lines;
    ReadFileLines(fileName, lines);

    StringList tokens;
    for (BigInt i = 0; i < (BigInt)lines.size(); ++i)
    {
        Tokenize(lines[i], tokens, ' ');

        assert(tokens.size() >= 7);
        const std::string type = tokens[0] + ' ' + tokens[1];

        Haversack& thisHaversack = GetHaversackFromData(data, type);

        assert(tokens[2] == "bags");
        assert(tokens[3] == "contain");

        if (tokens[4] == "no")
            continue;

        BigInt index = 4;
        while ((tokens.size() - index) >= 4)
        {
            const std::string& numString = tokens[index];
            assert(StringHasDigits(numString));

            const BigInt number = atoi(numString.c_str());

            const std::string otherType = tokens[index + 1] + ' ' + tokens[index + 2];

            Haversack& otherHaversack = GetHaversackFromData(data, otherType);

            thisHaversack.contains.insert(std::pair<std::string, BigInt>(otherType, number));
            otherHaversack.canBeContainedBy.insert(type);

            index += 4;
        }
    }
}

void PrintHaversackData(const std::map<std::string, Haversack>& data)
{
    printf("Haversack data:\n");
    for (auto iter = data.cbegin(); iter != data.cend(); ++iter)
    {
        const Haversack& haversack = iter->second;
        printf("  Type = '%s' ", haversack.type.c_str());

        if (!haversack.contains.empty())
        {
            printf(", Contains: ");
            for (auto iter2 = haversack.contains.cbegin(); iter2 != haversack.contains.cend(); ++iter2)
                printf("%lld x '%s' ", iter2->second, iter2->first.c_str());
        }

        if (!haversack.canBeContainedBy.empty())
        {
            printf(", Can be contained by: ");
            for (auto iter2 = haversack.canBeContainedBy.cbegin(); iter2 != haversack.canBeContainedBy.cend(); ++iter2)
                printf("'%s' ", iter2->c_str());
        }

        printf("\n");
    }
}

BigInt CalcHowManyBagsCanContain(
    const std::map<std::string, Haversack>& data,
    const std::string& type,
    bool verbose,
    std::set<std::string>* pAlreadyCheckedSet = nullptr)
{
    const Haversack& haversack = GetConstHaversackFromData(data, type);

    std::set<std::string> alreadyCheckedSetInit;
    if (!pAlreadyCheckedSet)
        pAlreadyCheckedSet = &alreadyCheckedSetInit;

    BigInt count = 0;

    if (verbose)
        printf("Type '%s'\n", type.c_str());
    for (auto iter = haversack.canBeContainedBy.cbegin(); iter != haversack.canBeContainedBy.cend(); ++iter)
    {
        if (pAlreadyCheckedSet->count(*iter) > 0)
            continue;

        if (verbose)
            printf("Can be contained by '%s'\n", iter->c_str());

        count += 1;
        count += CalcHowManyBagsCanContain(data, *iter, verbose, pAlreadyCheckedSet);

        pAlreadyCheckedSet->insert(*iter);
    }

    return count;
}

BigInt CalcHowManyBagsAreContained(const std::map<std::string, Haversack>& data, const std::string& type, bool verbose)
{
    const Haversack& haversack = GetConstHaversackFromData(data, type);

    BigInt count = 0;

    if (verbose)
        printf("Type '%s'\n", type.c_str());
    for (auto iter = haversack.contains.cbegin(); iter != haversack.contains.cend(); ++iter)
    {
        if (verbose)
            printf("Contains %lld of '%s'\n", iter->second, iter->first.c_str());

        count += (iter->second * (1 + CalcHowManyBagsAreContained(data, iter->first, verbose)));
    }

    return count;
}

void RunHandyHaversacks()
{
    std::map<std::string, Haversack> testData;
    ReadHaversackData("Day7TestInput.txt", testData);
    PrintHaversackData(testData);

    std::map<std::string, Haversack> data;
    ReadHaversackData("Day7Input.txt", data);

    const std::string interestingType = "shiny gold";
    printf(
        "Number of bags in test data that can contain '%s' = %lld\n",
        interestingType.c_str(),
        CalcHowManyBagsCanContain(testData, interestingType, true));
    printf(
        "Number of bags in main data that can contain '%s' = %lld\n",
        interestingType.c_str(),
        CalcHowManyBagsCanContain(data, interestingType, false));

    std::map<std::string, Haversack> testDataA;
    ReadHaversackData("Day7TestInputA.txt", testDataA);
    PrintHaversackData(testDataA);

    printf(
        "Number of bags in test data that '%s' contains = %lld\n",
        interestingType.c_str(),
        CalcHowManyBagsAreContained(testData, interestingType, true));
    printf(
        "Number of bags in test data that '%s' contains = %lld\n",
        interestingType.c_str(),
        CalcHowManyBagsAreContained(testDataA, interestingType, true));
    printf(
        "Number of bags in main data that '%s' contains = %lld\n",
        interestingType.c_str(),
        CalcHowManyBagsAreContained(data, interestingType, false));
}


////////////////////////////
// Problem 8 - Handheld Halting

enum class InstructionType
{
    NOP,
    ACC,
    JMP,
};

struct Instruction
{
    InstructionType type;
    BigInt arg;
};

class Program
{
public:
    Program(const char* fileName) : m_nextInstructionIndex(0), m_accumulator(0), m_programTerminated(false)
    {
        StringList lines;
        StringList tokens;

        ReadFileLines(fileName, lines);

        for (BigInt i = 0; i < (BigInt)lines.size(); ++i)
        {
            Tokenize(lines[i], tokens, ' ');
            assert(tokens.size() == 2);

            Instruction newInstruction;
            if (tokens[0] == "nop")
                newInstruction.type = InstructionType::NOP;
            else if (tokens[0] == "acc")
                newInstruction.type = InstructionType::ACC;
            else
            {
                assert(tokens[0] == "jmp");
                newInstruction.type = InstructionType::JMP;
            }

            newInstruction.arg = atoi(tokens[1].c_str());

            m_instructions.push_back(newInstruction);
        }

        m_instructionRunCounts.resize(m_instructions.size(), false);
    }

    void ResetExecution()
    {
        m_nextInstructionIndex = 0;
        m_accumulator = 0;
        std::fill(m_instructionRunCounts.begin(), m_instructionRunCounts.end(), false);
        m_programTerminated = false;
    }

    BigInt GetAccumulator() const { return m_accumulator; }
    bool DidProgramTerminate() const { return m_programTerminated; }

    void ExecuteNextInstruction(bool verbose)
    {
        assert(!m_programTerminated);

        const BigInt instructionToExecute = m_nextInstructionIndex;
        if (instructionToExecute >= (BigInt)m_instructions.size())
        {
            assert(instructionToExecute == (BigInt)m_instructions.size());
            m_programTerminated = true;
            if (verbose)
                printf("Program terminated\n");
            return;
        }

        const Instruction& nextInstruction = m_instructions[instructionToExecute];
        switch (nextInstruction.type)
        {
            case InstructionType::NOP:
            default:
                assert(nextInstruction.type == InstructionType::NOP);
                // nothing-doing
                ++m_nextInstructionIndex;
                if (verbose)
                    printf("%lld: NOP %+lld\n", instructionToExecute, nextInstruction.arg);
                break;
            case InstructionType::ACC:
                m_accumulator += nextInstruction.arg;
                ++m_nextInstructionIndex;
                if (verbose)
                    printf(
                        "%lld: ACC %+lld, %lld = %lld%+lld\n",
                        instructionToExecute,
                        nextInstruction.arg,
                        m_accumulator,
                        m_accumulator - nextInstruction.arg,
                        nextInstruction.arg);
                break;
            case InstructionType::JMP:
                m_nextInstructionIndex += nextInstruction.arg;
                if (verbose)
                    printf(
                        "%lld: JMP %+lld, %lld = %lld%+lld\n",
                        instructionToExecute,
                        nextInstruction.arg,
                        m_nextInstructionIndex,
                        m_nextInstructionIndex - nextInstruction.arg,
                        nextInstruction.arg);
                break;
        }
        ++m_instructionRunCounts[instructionToExecute];
    }

    BigInt HasNextInstructionBeenRunBefore() const
    {
        return (m_nextInstructionIndex < (BigInt)m_instructionRunCounts.size())
               && (m_instructionRunCounts[m_nextInstructionIndex] > 0);
    }

    void ExecuteUntilLoopsOrTerminates(bool verbose)
    {
        while (!HasNextInstructionBeenRunBefore() && !DidProgramTerminate())
            ExecuteNextInstruction(verbose);

        if (verbose && HasNextInstructionBeenRunBefore())
            printf("Program began to loop\n");
    }

    void FindFix(BigInt& instructionToFix, InstructionType& origInstructionType, BigInt& accumlatorAfterTermination, bool verbose)
    {
        for (BigInt i = 0; i < (BigInt)m_instructions.size(); ++i)
        {
            Instruction& instruction = m_instructions[i];
            if (instruction.type == InstructionType::ACC)
                continue;

            const InstructionType typeBackup = instruction.type;
            if (instruction.type == InstructionType::NOP)
            {
                instruction.type = InstructionType::JMP;
                if (verbose)
                    printf("Changing instruction %lld from NOP to JMP\n", i);
            }
            else
            {
                instruction.type = InstructionType::NOP;
                if (verbose)
                    printf("Changing instruction %lld from JMP to NOP\n", i);
            }

            ResetExecution();
            ExecuteUntilLoopsOrTerminates(false);

            instruction.type = typeBackup;

            if (DidProgramTerminate())
            {
                instructionToFix = i;
                origInstructionType = typeBackup;
                accumlatorAfterTermination = m_accumulator;
                if (verbose)
                    printf("Program terminated successfully!!!\n");
                return;
            }

            if (verbose)
                printf("Program began to loop\n");
        }

        if (verbose)
        {
            instructionToFix = -1;
            origInstructionType = InstructionType::NOP;
            accumlatorAfterTermination = -1;
            printf("Fix not found!!!\n");
        }
    }


private:
    std::vector<Instruction> m_instructions;

    BigInt m_nextInstructionIndex;
    BigInt m_accumulator;
    BigIntList m_instructionRunCounts;
    bool m_programTerminated;
};

void RunHandheldHalting()
{
    Program testProgram("Day8TestInput.txt");
    printf("Test program, running until loop:\n");
    testProgram.ExecuteUntilLoopsOrTerminates(true);
    printf("Accumulator = %lld\n", testProgram.GetAccumulator());
    printf("Fixing test program\n");
    BigInt instructionToFix;
    InstructionType origInstructionType;
    BigInt accumulatorAfterFix;
    testProgram.FindFix(instructionToFix, origInstructionType, accumulatorAfterFix, true);
    printf(
        "Test program was fixed by changing instruction %lld from %s to %s, allowing program to terminate normally with accumulator = %lld\n",
        instructionToFix,
        (origInstructionType == InstructionType::NOP) ? "NOP" : "JMP",
        (origInstructionType == InstructionType::NOP) ? "JMP" : "NOP",
        accumulatorAfterFix);

    Program program("Day8Input.txt");
    printf("\nMain program, running until loop:\n");
    program.ExecuteUntilLoopsOrTerminates(false);
    printf("Accumulator = %lld\n", program.GetAccumulator());
    printf("Fixing main program\n");
    program.FindFix(instructionToFix, origInstructionType, accumulatorAfterFix, true);
    printf(
        "Main program was fixed by changing instruction %lld from %s to %s, allowing program to terminate normally with accumulator = %lld\n",
        instructionToFix,
        (origInstructionType == InstructionType::NOP) ? "NOP" : "JMP",
        (origInstructionType == InstructionType::NOP) ? "JMP" : "NOP",
        accumulatorAfterFix);
}


////////////////////////////
// Problem 9 - Encoding Error

class XMasNumberSeries
{
public:
    XMasNumberSeries(BigInt windowSize, const char* fileName) : m_windowSize(windowSize)
    {
        StringList lines;
        ReadFileLines(fileName, lines);
        for (BigInt i = 0; i < (BigInt)lines.size(); ++i)
        {
            m_numbers.push_back(atoi(lines[i].c_str()));
        }
    }

    BigInt FindFirstInvalidNumber(bool verbose) const
    {
        std::set<BigInt> rollingSet;

        for (BigInt i = 0; i < m_windowSize; ++i)
            rollingSet.insert(m_numbers[i]);

        for (BigInt i = m_windowSize; i < (BigInt)m_numbers.size(); ++i)
        {
            const BigInt thisNumber = m_numbers[i];

            bool thisNumberIsValid = false;
            assert(rollingSet.size() == m_windowSize);
            for (auto iter = rollingSet.cbegin(); iter != rollingSet.cend(); ++iter)
            {
                const BigInt firstNumber = *iter;
                const BigInt secondNumber = thisNumber - firstNumber;

                if ((secondNumber <= 0) || (secondNumber < firstNumber))
                    break;

                if (rollingSet.count(secondNumber) > 0)
                {
                    thisNumberIsValid = true;
                    if (verbose)
                        printf(
                            "This number %lld is the sum of %lld and %lld, therefore valid\n",
                            thisNumber,
                            firstNumber,
                            secondNumber);
                    break;
                }
            }

            if (!thisNumberIsValid)
                return thisNumber;

            rollingSet.erase(m_numbers[i - m_windowSize]);
            rollingSet.insert(thisNumber);
            assert(rollingSet.size() == m_windowSize);
        }

        return -1;
    }

    BigInt FindEncryptionWeakness(BigInt invalidNumber, bool verbose) const
    {
        assert(invalidNumber = FindFirstInvalidNumber(false));

        BigInt sum = 0;
        std::set<BigInt> numbersInSum;

        assert(m_numbers.size() >= 2);

        BigInt firstIndex = 0;
        BigInt secondIndex = 1;
        const BigInt firstNumber = m_numbers[firstIndex];
        const BigInt secondNumber = m_numbers[secondIndex];

        sum = firstNumber + secondNumber;
        if (sum == invalidNumber)
        {
            if (verbose)
                printf(
                    "Immediately found encryption weakness with first number = %lld, second number = %lld\n",
                    firstNumber,
                    secondNumber);
            return sum;
        }

        numbersInSum.insert(firstNumber);
        numbersInSum.insert(secondNumber);

        if (verbose)
            printf("Starting with first number = %lld, second number = %lld\n", firstNumber, secondNumber);

        for (;;)
        {
            if (sum < invalidNumber)
            {
                if (secondIndex >= ((BigInt)m_numbers.size() - 1))
                    return -1;

                ++secondIndex;
                const BigInt newNumber = m_numbers[secondIndex];
                sum += newNumber;
                numbersInSum.insert(newNumber);

                if (verbose)
                {
                    printf("Added new second number, sum = %lld, sequence is now:  ", sum);
                    for (BigInt i = firstIndex; i <= secondIndex; ++i)
                    {
                        printf("%lld ", m_numbers[i]);
                    }
                    printf("\n");
                }
            }
            else
            {
                assert(sum > invalidNumber);

                const BigInt prevNumber = m_numbers[firstIndex];
                sum -= prevNumber;
                numbersInSum.erase(prevNumber);
                ++firstIndex;

                if (firstIndex == secondIndex)
                {
                    if (secondIndex >= ((BigInt)m_numbers.size() - 1))
                        return -1;

                    ++secondIndex;
                    const BigInt newNumber = m_numbers[secondIndex];
                    sum += newNumber;
                    numbersInSum.insert(newNumber);

                    if (verbose)
                    {
                        printf("Incremented first and second numbers, sum = %lld, sequence is now:  ", sum);
                        for (BigInt i = firstIndex; i <= secondIndex; ++i)
                        {
                            printf("%lld ", m_numbers[i]);
                        }
                        printf("\n");
                    }
                }
                else
                {
                    if (verbose)
                    {
                        printf("Removed first number, sum = %lld, sequence is now:  ", sum);
                        for (BigInt i = firstIndex; i <= secondIndex; ++i)
                        {
                            printf("%lld ", m_numbers[i]);
                        }
                        printf("\n");
                    }
                }
            }

            if (sum == invalidNumber)
            {
                assert(numbersInSum.size() >= 2);
                const BigInt smallest = *numbersInSum.cbegin();
                const BigInt largest = *numbersInSum.crbegin();
                const BigInt weakness = smallest + largest;

                if (verbose)
                {
                    printf(
                        "Found valid sum = %lld, smallest is %lld, largest is %lld, weakness is %lld, sequence is:  ",
                        sum,
                        smallest,
                        largest,
                        weakness);
                    for (BigInt i = firstIndex; i <= secondIndex; ++i)
                    {
                        printf("%lld ", m_numbers[i]);
                    }
                    printf("\n");
                }

                return weakness;
            }
        }

        return -1;
    }


private:
    BigInt m_windowSize;
    BigIntList m_numbers;
};

void RunEncodingError()
{
    XMasNumberSeries testSeries(5, "Day9TestInput.txt");
    BigInt firstInvalidNumber = testSeries.FindFirstInvalidNumber(true);
    printf("First invalid number in test series = %lld\n", firstInvalidNumber);
    printf("Encryption weakness = %lld\n", testSeries.FindEncryptionWeakness(firstInvalidNumber, true));

    XMasNumberSeries mainSeries(25, "Day9Input.txt");
    firstInvalidNumber = mainSeries.FindFirstInvalidNumber(true);
    printf("First invalid number in main series = %lld\n", firstInvalidNumber);
    printf("Encryption weakness = %lld\n", mainSeries.FindEncryptionWeakness(firstInvalidNumber, false));
}


////////////////////////////
// Problem 10 - Adapter Array

void ReadAdapterArray(const char* fileName, std::set<BigInt>& jolts, bool verbose)
{
    StringList lines;
    ReadFileLines(fileName, lines);

    jolts.clear();
    for (BigInt i = 0; i < (BigInt)lines.size(); ++i)
    {
        jolts.insert(atoi(lines[i].c_str()));
    }

    if (verbose)
    {
        printf("Read adapter array from '%s', sorted list = ", fileName);
        for (auto iter = jolts.cbegin(); iter != jolts.cend(); ++iter)
        {
            printf("%lld ", *iter);
        }
        printf("\n");
    }
}

void CalcAdaptorJoltGaps(
    const std::set<BigInt>& jolts, BigInt& num1JoltGaps, BigInt& num2JoltGaps, BigInt& num3JoltGaps, bool verbose)
{
    assert(jolts.size() >= 2);
    assert(*jolts.cbegin() <= 3);

    num1JoltGaps = 0;
    num2JoltGaps = 0;
    num3JoltGaps = 0;

    BigInt prevJolt = 0;
    for (auto iter = jolts.cbegin(); iter != jolts.cend(); ++iter)
    {
        const BigInt thisJolt = *iter;
        const BigInt joltGap = thisJolt - prevJolt;
        assert(joltGap >= 1);
        assert(joltGap <= 3);

        if (joltGap == 1)
            ++num1JoltGaps;
        else if (joltGap == 3)
            ++num3JoltGaps;

        if (verbose)
        {
            printf("This jolt = %lld, which is a gap %lld from prev jolt %lld\n", thisJolt, joltGap, prevJolt);
            if (joltGap == 1)
                printf("Num 1-jolt gaps incremented to %lld\n", num1JoltGaps);
            else if (joltGap == 2)
                printf("Num 2-jolt gaps incremented to %lld\n", num2JoltGaps);
            else if (joltGap == 3)
                printf("Num 3-jolt gaps incremented to %lld\n", num3JoltGaps);
        }

        prevJolt = thisJolt;
    }

    ++num3JoltGaps;
    if (verbose)
        printf(
            "Final 3-jolt gap from %lld to %lld\nFound %lld 1-jolt gaps, %lld 2-jolt gaps, and %lld 3-jolt gaps\n",
            prevJolt,
            prevJolt + 3,
            num1JoltGaps,
            num2JoltGaps,
            num3JoltGaps);
}

BigInt CalcProdAdaptor1JoltAnd3JoltGaps(const std::set<BigInt>& jolts, bool verbose)
{
    BigInt num1JoltGaps = 0;
    BigInt num2JoltGaps = 0;
    BigInt num3JoltGaps = 0;
    CalcAdaptorJoltGaps(jolts, num1JoltGaps, num2JoltGaps, num3JoltGaps, verbose);

    return num1JoltGaps * num3JoltGaps;
}

void IncrementContigAdaptorStretches(BigIntMap& contigStretches, BigInt stretchLen, bool verbose)
{
    if (verbose)
        printf("Finished a contig stretch of length %lld\n", stretchLen);

    auto contigFinder = contigStretches.find(stretchLen);
    if (contigFinder != contigStretches.end())
    {
        ++(contigFinder->second);
        if (verbose)
            printf("Now there are %lld contig stretches of length %lld\n", contigFinder->second, stretchLen);
    }
    else
    {
        contigStretches.insert(std::pair<BigInt, BigInt>(stretchLen, 1));
        if (verbose)
            printf("Now there is 1 contig stretch of length %lld\n", stretchLen);
    }
}

void CalcContigAdaptorStretches(const std::set<BigInt>& jolts, BigIntMap& contigStretches, bool verbose)
{
    BigInt num1JoltGaps = 0;
    BigInt num2JoltGaps = 0;
    BigInt num3JoltGaps = 0;
    CalcAdaptorJoltGaps(jolts, num1JoltGaps, num2JoltGaps, num3JoltGaps, false);

    assert(num2JoltGaps == 0);

    contigStretches.clear();
    BigInt currStretchLen = 1;

    BigInt prevJolt = 0;
    for (auto iter = jolts.cbegin(); iter != jolts.cend(); ++iter)
    {
        const BigInt thisJolt = *iter;
        const BigInt gap = thisJolt - prevJolt;

        if (gap == 1)
        {
            ++currStretchLen;
        }
        else
        {
            assert(gap == 3);

            IncrementContigAdaptorStretches(contigStretches, currStretchLen, verbose);

            currStretchLen = 1;
        }

        prevJolt = thisJolt;
    }
    IncrementContigAdaptorStretches(contigStretches, currStretchLen, verbose);

    if (verbose)
    {
        printf("Entire list of stretches:  ");
        for (auto iter = contigStretches.cbegin(); iter != contigStretches.cend(); ++iter)
            printf("%lldn of %lld  ", iter->second, iter->first);
        printf("\n");
    }
}

BigInt CalcNumWaysToConnectAdaptors(const std::set<BigInt>& jolts, bool verbose)
{
    BigIntMap contigStretches;
    CalcContigAdaptorStretches(jolts, contigStretches, verbose);

    BigInt numWays = 1;

    for (auto iter = contigStretches.cbegin(); iter != contigStretches.cend(); ++iter)
    {
        if (iter->first < 3)
            continue;

        BigInt numStretchWays = 1;
        switch (iter->first)
        {
            case 3:
                numStretchWays = 2;
                break;
            case 4:
                numStretchWays = 4;
                break;
            case 5:
                numStretchWays = 7;
                break;
            default:
                assert(false);
                break;
        }

        const BigInt term = (BigInt)pow((double)numStretchWays, (double)iter->second);
        if (verbose)
            printf(
                "There are %lld stretches of length %lld;  each one contributes %lld ways;  term is %lld ^ %lld;  numWays = numWays * %lld = %lld * %lld = %lld\n",
                iter->second,
                iter->first,
                numStretchWays,
                numStretchWays,
                iter->second,
                term,
                numWays,
                term,
                numWays * term);

        numWays *= term;
    }

    return numWays;
}

void RunAdapterArray()
{
    std::set<BigInt> testJoltsA;
    ReadAdapterArray("Day10TestInputA.txt", testJoltsA, true);
    printf("Test array A, product of 1 jolt and 3 jolt gaps = %lld\n", CalcProdAdaptor1JoltAnd3JoltGaps(testJoltsA, true));
    printf("Test array A, num ways to connect adaptors = %lld\n", CalcNumWaysToConnectAdaptors(testJoltsA, true));

    std::set<BigInt> testJoltsB;
    ReadAdapterArray("Day10TestInputB.txt", testJoltsB, true);
    printf("Test array B, product of 1 jolt and 3 jolt gaps = %lld\n", CalcProdAdaptor1JoltAnd3JoltGaps(testJoltsB, true));
    printf("Test array B, num ways to connect adaptors = %lld\n", CalcNumWaysToConnectAdaptors(testJoltsB, true));

    std::set<BigInt> mainJolts;
    ReadAdapterArray("Day10Input.txt", mainJolts, false);
    printf("Main array, product of 1 jolt and 3 jolt gaps = %lld\n", CalcProdAdaptor1JoltAnd3JoltGaps(mainJolts, false));
    printf("Main array, num ways to connect adaptors = %lld\n", CalcNumWaysToConnectAdaptors(mainJolts, true));
}


////////////////////////////
// Problem 11 - Seating System

class SeatingLayout
{
public:
    SeatingLayout(const char* fileName, bool verbose) : m_numSeatsX(0), m_numSeatsY(0)
    {
        ReadFileLines(fileName, m_seats);

        if (verbose)
            printf("Read seating layout from %s:\n", fileName);

        m_numSeatsX = m_seats[0].length();
        m_numSeatsY = (BigInt)m_seats.size();
        for (BigInt i = 1; i < m_numSeatsY; ++i)
            assert(m_seats[i].length() == m_numSeatsX);

        m_origSeats = m_seats;

        if (verbose)
            PrintLayout();
    }

    void Reset() { m_seats = m_origSeats; }

    char GetLocationState(BigInt x, BigInt y) const { return m_seats[y][x]; }

    void StepForwardUntilNothingChanges(BigInt seeingDistance, BigInt maxSeenOccupied, bool verbose)
    {
        if (verbose)
        {
            printf("Initial state:\n");
            PrintLayout();
        }
        for (;;)
        {
            bool somethingChanged = false;
            StepForward(seeingDistance, maxSeenOccupied, &somethingChanged);

            if (!somethingChanged)
            {
                if (verbose)
                    printf("State settled!\n");
                break;
            }

            if (verbose)
            {
                printf("Next state:\n");
                PrintLayout();
            }
        }
    }

    void StepForward(BigInt seeingDistance, BigInt maxSeenOccupied, bool* pSomethingChanged = nullptr)
    {
        StringList newSeats;
        newSeats.resize(m_numSeatsY);

        bool somethingChanged = false;
        for (BigInt y = 0; y < m_numSeatsY; ++y)
        {
            std::string& row = newSeats[y];

            row.clear();
            for (BigInt x = 0; x < m_numSeatsX; ++x)
            {
                const char locState = GetLocationState(x, y);
                switch (locState)
                {
                    case 'L':
                        if (CountOccupiedSeatsSeen(x, y, seeingDistance) <= 0)
                        {
                            row += '#';
                            somethingChanged = true;
                        }
                        else
                        {
                            row += 'L';
                        }
                        break;
                    case '#':
                        if (CountOccupiedSeatsSeen(x, y, seeingDistance) >= maxSeenOccupied)
                        {
                            row += 'L';
                            somethingChanged = true;
                        }
                        else
                        {
                            row += '#';
                        }
                        break;
                    default:
                        assert(locState == '.');
                        row += '.';
                        break;
                }
            }
        }

        m_seats.swap(newSeats);
        if (pSomethingChanged)
            *pSomethingChanged = somethingChanged;
    }

    void PrintLayout()
    {
        for (BigInt i = 0; i < (BigInt)m_seats.size(); ++i)
            printf("  %s\n", m_seats[i].c_str());
    }

    BigInt CountLocationStatesOfType(char type) const
    {
        BigInt count = 0;
        for (BigInt y = 0; y < m_numSeatsY; ++y)
        {
            const std::string& row = m_seats[y];
            count += std::count(row.cbegin(), row.cend(), type);
        }
        return count;
    }


private:
    BigInt CountOccupiedSeatsSeen(BigInt x, BigInt y, BigInt seeingDistance) const
    {
        BigInt count = 0;

        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, +1, 0);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, +1, +1);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, 0, +1);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, -1, +1);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, -1, 0);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, -1, -1);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, 0, -1);
        count += CanSeeOccupiedSeatInDirection(x, y, seeingDistance, +1, -1);

        return count;
    }

    bool CanSeeOccupiedSeatInDirection(BigInt x, BigInt y, BigInt seeingDistance, BigInt xDir, BigInt yDir) const
    {
        assert((xDir == -1) || (xDir == 0) || (xDir == +1));
        assert((yDir == -1) || (yDir == 0) || (yDir == +1));
        assert((xDir != 0) || (yDir != 0));

        for (BigInt i = 0; (seeingDistance < 0) || (i < seeingDistance); ++i)
        {
            x += xDir;
            y += yDir;

            if ((x < 0) || (x >= m_numSeatsX) || (y < 0) || (y >= m_numSeatsY))
                return false;

            const char seenSpotType = m_seats[y][x];
            if (seenSpotType == '#')
                return true;
            if (seenSpotType == 'L')
                return false;

            assert(seenSpotType == '.');
        }

        return false;
    }

    StringList m_origSeats;
    StringList m_seats;
    BigInt m_numSeatsX;
    BigInt m_numSeatsY;
};

void RunSeatingSystem()
{
    SeatingLayout testLayout("Day11TestInput.txt", true);
    testLayout.StepForwardUntilNothingChanges(1, 4, true);
    printf(
        "For test input, seeing distance of 1 and seeing max occupants 4, after settling, number of occupied seats = %lld\n",
        testLayout.CountLocationStatesOfType('#'));
    testLayout.Reset();
    testLayout.StepForwardUntilNothingChanges(-1, 5, true);
    printf(
        "For test input, seeing distance of infinity and seeing max occupants 5, after settling, number of occupied seats = %lld\n",
        testLayout.CountLocationStatesOfType('#'));

    SeatingLayout mainLayout("Day11Input.txt", false);
    mainLayout.StepForwardUntilNothingChanges(1, 4, false);
    printf(
        "For main input, seeing distance of 1 and seeing max occupants 4, after settling, number of occupied seats = %lld\n",
        mainLayout.CountLocationStatesOfType('#'));
    mainLayout.Reset();
    mainLayout.StepForwardUntilNothingChanges(-1, 5, false);
    printf(
        "For main input, seeing distance of infinity and seeing max occupants 5, after settling, number of occupied seats = %lld\n",
        mainLayout.CountLocationStatesOfType('#'));
}


////////////////////////////
// Problem 12 - Rain Risk

void StepRainRiskShip(BigInt& xPos, BigInt& yPos, BigInt& facing, const std::string& command, bool verbose)
{
    assert(command.length() >= 2);

    const char commandPrefix = command[0];
    const BigInt commandArg = atoi(command.c_str() + 1);
    switch (commandPrefix)
    {
        case 'N':
            yPos -= commandArg;
            if (verbose)
                printf("Moved %lld units North\n", commandArg);
            break;
        case 'S':
            yPos += commandArg;
            if (verbose)
                printf("Moved %lld units South\n", commandArg);
            break;
        case 'E':
            xPos += commandArg;
            if (verbose)
                printf("Moved %lld units East\n", commandArg);
            break;
        case 'W':
            xPos -= commandArg;
            if (verbose)
                printf("Moved %lld units West\n", commandArg);
            break;
        case 'L':
            assert((commandArg % 90) == 0);
            facing = (facing - commandArg + 360) % 360;
            if (verbose)
                printf("Rotated %lld degrees to the Left\n", commandArg);
            break;
        case 'R':
            assert((commandArg % 90) == 0);
            facing = (facing + commandArg) % 360;
            if (verbose)
                printf("Rotated %lld degrees to the Right\n", commandArg);
            break;
        case 'F':
            switch (facing)
            {
                case 0:
                    yPos -= commandArg;
                    break;
                case 90:
                    xPos += commandArg;
                    break;
                case 180:
                    yPos += commandArg;
                    break;
                case 270:
                    xPos -= commandArg;
                    break;
                default:
                    assert(false && "Invalid facing");
                    break;
            }
            if (verbose)
                printf("Moved %lld units Forward\n", commandArg);
            break;
        default:
            assert(false && "Invalid command prefix");
            break;
    }

    if (verbose)
        printf("New position is x = %lld, y = %lld, facing = %lld\n", xPos, yPos, facing);
}

BigInt CalcManhattanDistance(const StringList& lines, bool verbose)
{
    BigInt xPos = 0;
    BigInt yPos = 0;
    BigInt facing = 90;
    for (BigInt i = 0; i < (BigInt)lines.size(); ++i)
    {
        const std::string& command = lines[i];
        if (verbose)
            printf("Received command:  %s\n", command.c_str());
        StepRainRiskShip(xPos, yPos, facing, command, verbose);
    }

    return (abs(xPos) + abs(yPos));
}

void StepRainRiskShipWithWaypoint(
    BigInt& xPos, BigInt& yPos, BigInt& waypointXPos, BigInt& waypointYPos, const std::string& command, bool verbose)
{
    assert(command.length() >= 2);

    const char commandPrefix = command[0];
    const BigInt commandArg = atoi(command.c_str() + 1);
    switch (commandPrefix)
    {
        case 'N':
            waypointYPos -= commandArg;
            if (verbose)
                printf("Moved waypoint %lld units North\n", commandArg);
            break;
        case 'S':
            waypointYPos += commandArg;
            if (verbose)
                printf("Moved waypoint %lld units South\n", commandArg);
            break;
        case 'E':
            waypointXPos += commandArg;
            if (verbose)
                printf("Moved waypoint %lld units East\n", commandArg);
            break;
        case 'W':
            waypointXPos -= commandArg;
            if (verbose)
                printf("Moved waypoint %lld units West\n", commandArg);
            break;
        case 'L':
            assert((commandArg % 90) == 0);
            switch (commandArg)
            {
                case 0:
                    assert(false && "0 rotation not expected!");
                    break;
                case 90:
                    std::swap(waypointXPos, waypointYPos);
                    waypointYPos = -waypointYPos;
                    break;
                case 180:
                    waypointXPos = -waypointXPos;
                    waypointYPos = -waypointYPos;
                    break;
                case 270:
                    std::swap(waypointXPos, waypointYPos);
                    waypointXPos = -waypointXPos;
                    break;
                default:
                    assert(false && "Invalid facing");
                    break;
            }
            if (verbose)
                printf("Rotated waypoint %lld degrees to the Left\n", commandArg);
            break;
        case 'R':
            assert((commandArg % 90) == 0);
            switch (commandArg)
            {
                case 0:
                    assert(false && "0 rotation not expected!");
                    break;
                case 90:
                    std::swap(waypointXPos, waypointYPos);
                    waypointXPos = -waypointXPos;
                    break;
                case 180:
                    waypointXPos = -waypointXPos;
                    waypointYPos = -waypointYPos;
                    break;
                case 270:
                    std::swap(waypointXPos, waypointYPos);
                    waypointYPos = -waypointYPos;
                    break;
                default:
                    assert(false && "Invalid facing");
                    break;
            }
            if (verbose)
                printf("Rotated waypoint %lld degrees to the Right\n", commandArg);
            break;
        case 'F':
            xPos += (waypointXPos * commandArg);
            yPos += (waypointYPos * commandArg);
            if (verbose)
                printf(
                    "Moved %lld times Forward toward the waypoint:  xStep = %lld * %lld = %lld, yStep = %lld * %lld = %lld,\n",
                    commandArg,
                    waypointXPos,
                    commandArg,
                    waypointXPos * commandArg,
                    waypointYPos,
                    commandArg,
                    waypointYPos * commandArg);
            break;
        default:
            assert(false && "Invalid command prefix");
            break;
    }

    if (verbose)
        printf(
            "New position is x = %lld, y = %lld; waypoint position is x = %lld, y = %lld\n",
            xPos,
            yPos,
            waypointXPos,
            waypointYPos);
}

BigInt CalcManhattanDistanceWithWaypoint(const StringList& lines, bool verbose)
{
    BigInt xPos = 0;
    BigInt yPos = 0;
    BigInt waypointXPos = 10;
    BigInt waypointYPos = -1;
    for (BigInt i = 0; i < (BigInt)lines.size(); ++i)
    {
        const std::string& command = lines[i];
        if (verbose)
            printf("Received command:  %s\n", command.c_str());
        StepRainRiskShipWithWaypoint(xPos, yPos, waypointXPos, waypointYPos, command, verbose);
    }

    return (abs(xPos) + abs(yPos));
}

void RunRainRisk()
{
    StringList testData;
    ReadFileLines("Day12TestInput.txt", testData);
    printf("Manhattan distance after running commands in test data = %lld\n", CalcManhattanDistance(testData, true));
    printf(
        "Manhattan distance after running commands with waypoint in test data = %lld\n",
        CalcManhattanDistanceWithWaypoint(testData, true));

    StringList mainData;
    ReadFileLines("Day12Input.txt", mainData);
    printf("Manhattan distance after running commands in main data = %lld\n", CalcManhattanDistance(mainData, false));
    printf(
        "Manhattan distance after running commands with waypoint in main data = %lld\n",
        CalcManhattanDistanceWithWaypoint(mainData, false));
}


////////////////////////////
// Problem 13 - Shuttle Search

void BuildShuttleAvailBusList(const std::string& busesLine, std::vector<std::pair<BigInt, BigInt>>& availBuses, bool verbose)
{
    StringList tokens;
    Tokenize(busesLine, tokens, ',');

    availBuses.clear();
    if (verbose)
        printf("Reading available buses:\n");
    for (BigInt tokenIndex = 0; tokenIndex < (BigInt)tokens.size(); ++tokenIndex)
    {
        const std::string& busToken = tokens[tokenIndex];

        if (verbose)
            printf("  Read %s, ", busToken.c_str());

        if (busToken == "x")
        {
            if (verbose)
                printf("skipping\n");
            continue;
        }

        const BigInt oneBus = atoi(busToken.c_str());
        availBuses.push_back(std::pair<BigInt, BigInt>(oneBus, tokenIndex));
        if (verbose)
            printf("bus %lld added, with index = %lld\n", oneBus, tokenIndex);
    }

    if (verbose)
    {
        printf("Full bus list:  ");
        for (auto iter = availBuses.cbegin(); iter != availBuses.cend(); ++iter)
            printf("%lld (at %lld) ", iter->first, iter->second);
        printf("\n");
    }
}

void ReadShuttleSearchFile(
    const char* fileName, BigInt& startTime, std::vector<std::pair<BigInt, BigInt>>& availBuses, bool verbose)
{
    StringList lines;
    ReadFileLines(fileName, lines);
    assert(lines.size() == 2);

    if (verbose)
        printf("Reading shuttle search file %s\n", fileName);

    startTime = atoi(lines[0].c_str());
    if (verbose)
        printf("Read earliest timestamp = %lld\n", startTime);

    BuildShuttleAvailBusList(lines[1], availBuses, verbose);
}

BigInt CalcShuttleProdIDAndWaitTime(BigInt startTime, const std::vector<std::pair<BigInt, BigInt>>& availBuses, bool verbose)
{
    BigInt currTime = startTime;

    for (;;)
    {
        for (auto iter = availBuses.cbegin(); iter != availBuses.cend(); ++iter)
        {
            const BigInt currBus = iter->first;
            const BigInt timeMod = currTime % currBus;
            if (timeMod == 0)
            {
                const BigInt waitTime = currTime - startTime;
                if (verbose)
                    printf("Found bus id %lld departing at time %lld (wait time = %lld)\n", currBus, currTime, waitTime);
                return currBus * waitTime;
            }

            if (verbose)
                printf("Bus id %lld does not depart at time %lld (time mod == %lld), skipping...\n", currBus, currTime, timeMod);
        }

        ++currTime;
    }
}

BigInt CalcEarliestShuttleTime(const std::vector<std::pair<BigInt, BigInt>>& availBuses, bool verbose, BigInt minTime = 0)
{
    BigInt currTime = 0;
    BigInt timeStep = 0;

    for (auto iter = availBuses.cbegin(); iter != availBuses.cend(); ++iter)
    {
        if (iter->first > timeStep)
        {
            timeStep = iter->first;
            if (minTime > 0)
            {
                currTime = minTime - iter->second;

                const BigInt mod = minTime % iter->first;
                if (mod > 0)
                {
                    currTime += (iter->first - mod);
                }
            }
            else
            {
                currTime = timeStep - iter->second;
            }
        }
    }

    while (currTime < 0)
    {
        currTime += timeStep;
    }

    for (;;)
    {
        if (verbose)
            printf("Testing time stamp %lld\n", currTime);

        bool currTimeIsGood = true;
        for (auto iter = availBuses.cbegin(); iter != availBuses.cend(); ++iter)
        {
            const BigInt currBus = iter->first;
            const BigInt currIndex = iter->second;

            const BigInt timeMod = (currTime + currIndex) % currBus;
            if (timeMod != 0)
            {
                currTimeIsGood = false;
                if (verbose)
                    printf(
                        "  Curr bus %lld, at index %lld, has bad timeMod %lld, rejecting this time...\n",
                        currBus,
                        currIndex,
                        timeMod);
                break;
            }
            else
            {
                if (verbose)
                    printf("  Curr bus %lld, at index %lld, has GOOD timeMod!\n", currBus, currIndex);
            }
        }

        if (currTimeIsGood)
        {
            if (verbose)
                printf("  Found good currTime = %lld\n", currTime);
            return currTime;
        }

        currTime += timeStep;
    }
}

void IncrementSingleShuttleOffset(BigInt& offset, BigInt thisNum, BigInt increment)
{
    offset += increment;

    const lldiv_t div = lldiv(offset, thisNum);
    offset = div.rem;
}

void IncrementShuttleOffsets(
    const std::vector<std::pair<BigInt, BigInt>>& availBuses,
    BigIntList& offsets,
    BigInt& grandTotalAdded,
    BigInt focusIndex,
    BigInt increment,
    bool verbose)
{
    const BigInt focusNum = availBuses[focusIndex].first;
    BigInt& focusOffset = offsets[focusIndex];

    // get the focused index synced to 0

    BigInt added = 0;
    while (focusOffset > 0)
    {
        IncrementSingleShuttleOffset(focusOffset, focusNum, increment);
        added += increment;
    }

    // step the previous offsets, asserting they are still 0 (synced)

    for (BigInt i = 0; i < focusIndex; ++i)
    {
        IncrementSingleShuttleOffset(offsets[i], availBuses[i].first, added);
        assert(offsets[i] == 0);
    }

    // step the remaining offsets (which may be and probably are still unsynced)

    for (BigInt i = focusIndex + 1; i < (BigInt)availBuses.size(); ++i)
    {
        IncrementSingleShuttleOffset(offsets[i], availBuses[i].first, added);
    }

    grandTotalAdded += added;
}

BigInt CalcEarliestShuttleTime2(const std::vector<std::pair<BigInt, BigInt>>& availBuses, bool verbose)
{
    BigIntList offsets;
    offsets.reserve(availBuses.size());

    if (verbose)
        printf("Avail buses:\n");

    for (auto iter = availBuses.cbegin(); iter != availBuses.cend(); ++iter)
    {
        const BigInt thisNum = iter->first;
        const BigInt index = iter->second;

        if (verbose)
            printf(
                "  Bus id %lld (%s), at index %lld, ",
                thisNum,
                s_factorizationCache.Factorize(thisNum).IsPrime() ? "PRIME" : "not prime",
                index);

        BigInt offset = -index;
        while (offset < 0)
            offset += thisNum;

        const BigInt timeBeforeNextSync = (offset == 0) ? 0 : (thisNum - offset);

        if (verbose)
            printf("position at time T = %lld, time before next sync = %lld\n", offset, timeBeforeNextSync);

        offsets.push_back(offset);
    }

    BigInt grandTotalAdded = 0;

    BigInt runningProduct = availBuses[0].first;
    for (BigInt i = 1; i < (BigInt)availBuses.size(); ++i)
    {
        IncrementShuttleOffsets(availBuses, offsets, grandTotalAdded, i, runningProduct, verbose);

        runningProduct *= availBuses[i].first;
    }

    const BigInt earliestShuttleTime = runningProduct - grandTotalAdded;

    if (verbose)
        printf(
            "Grand total added = %lld, grand product = %lld, difference = %lld\n",
            grandTotalAdded,
            runningProduct,
            earliestShuttleTime);

    return earliestShuttleTime;
}

BigInt CalcEarliestShuttleTime(const char* busLine, bool verbose)
{
    std::vector<std::pair<BigInt, BigInt>> availBuses;
    BuildShuttleAvailBusList(busLine, availBuses, verbose);
    return CalcEarliestShuttleTime2(availBuses, verbose);
}

void TestInlineEarliestShuttleCase(const char* busLine, bool verbose)
{
    printf("With inline data '%s', earliest possible timestamp = %lld\n", busLine, CalcEarliestShuttleTime(busLine, verbose));
}

void RunShuttleSearch()
{
    BigInt testStartTime = 0;
    std::vector<std::pair<BigInt, BigInt>> testAvailBuses;
    ReadShuttleSearchFile("Day13TestInput.txt", testStartTime, testAvailBuses, false);
    printf(
        "With test data, prod of shuttle ID of earliest departing bus and wait time = %lld\n",
        CalcShuttleProdIDAndWaitTime(testStartTime, testAvailBuses, false));

    BigInt mainStartTime = 0;
    std::vector<std::pair<BigInt, BigInt>> mainAvailBuses;
    ReadShuttleSearchFile("Day13Input.txt", mainStartTime, mainAvailBuses, false);
    printf(
        "With main data, prod of shuttle ID of earliest departing bus and wait time = %lld\n",
        CalcShuttleProdIDAndWaitTime(mainStartTime, mainAvailBuses, false));

    printf("With test data, earliest possible timestamp = %lld\n", CalcEarliestShuttleTime2(testAvailBuses, false));
    TestInlineEarliestShuttleCase("17,x,13,19", false);
    TestInlineEarliestShuttleCase("67,7,59,61", false);
    TestInlineEarliestShuttleCase("67,x,7,59,61", false);
    TestInlineEarliestShuttleCase("67,7,x,59,61", false);
    TestInlineEarliestShuttleCase("1789,37,47,1889", false);

    printf("\n\n\n");
    printf("With main data, earliest possible timestamp = %lld\n", CalcEarliestShuttleTime2(mainAvailBuses, true));
}


////////////////////////////
// Problem 14 - Docking Data

typedef BigIntMap DockingDataMemory;

void GenerateDockingFloatingBitIndexList(BigInt stompMask, BigInt origStompValueMask, BigIntList& list, bool verbose)
{
    static const BigInt MAX_BIT_INDEX = 36;

    if (verbose)
        printf(
            "   Generating floating bit index list from stompMask 0x%016llX, stompValueMask 0x%016llX\n",
            stompMask,
            origStompValueMask);

    list.clear();
    for (BigInt bitIndex = 0; bitIndex < MAX_BIT_INDEX; ++bitIndex)
    {
        const BigInt bitMask = 1LL << bitIndex;
        if (!(stompMask & bitMask))
            continue;
        if (origStompValueMask & bitMask)
            continue;

        list.push_back(bitIndex);

        if (verbose)
            printf("      %lld\n", bitIndex);
    }
}

void IterateGenerateDockingStompValueMaskList(
    BigInt stompMask,
    BigInt origStompValueMask,
    BigInt stompValueMaskSoFar,
    BigInt bitIndexIndex,
    const BigIntList& floatingBitIndexList,
    BigIntList& stompValueMaskList)
{
    if (bitIndexIndex >= (BigInt)floatingBitIndexList.size())
    {
        stompValueMaskList.push_back(stompValueMaskSoFar);
        return;
    }

    const BigInt bitIndex = floatingBitIndexList[bitIndexIndex];
    const BigInt bitMask = 1LL << bitIndex;

    const BigInt nextBitIndexIndex = bitIndexIndex + 1;

    IterateGenerateDockingStompValueMaskList(
        stompMask, origStompValueMask, stompValueMaskSoFar, nextBitIndexIndex, floatingBitIndexList, stompValueMaskList);
    IterateGenerateDockingStompValueMaskList(
        stompMask,
        origStompValueMask,
        stompValueMaskSoFar | bitMask,
        nextBitIndexIndex,
        floatingBitIndexList,
        stompValueMaskList);
}

void GenerateDockingStompValueMaskList(BigInt stompMask, BigInt origStompValueMask, BigIntList& stompValueMaskList, bool verbose)
{
    BigIntList floatingBitIndexList;
    GenerateDockingFloatingBitIndexList(stompMask, origStompValueMask, floatingBitIndexList, verbose);

    stompValueMaskList.clear();
    IterateGenerateDockingStompValueMaskList(
        stompMask, origStompValueMask, origStompValueMask, 0, floatingBitIndexList, stompValueMaskList);
}

BigInt CalcStompMaskedValue(BigInt stompMask, BigInt stompValueMask, BigInt origValue)
{
    return (stompMask & stompValueMask) | (~stompMask & origValue);
}

void WriteDockingDataValue(
    DockingDataMemory& memory,
    BigInt stompMask,
    const BigIntList& stompValueMaskList,
    BigInt location,
    BigInt value,
    bool version2,
    bool verbose)
{
    for (BigInt thisStompValueMask: stompValueMaskList)
    {
        const BigInt thisLocation = CalcStompMaskedValue(stompMask, thisStompValueMask, location);
        memory[thisLocation] = value;

        if (verbose)
            printf("    mem[%lld->%lld] = %lld\n", location, thisLocation, value);
    }
}

void ProcessDockingDataFile(const char* fileName, DockingDataMemory& memory, bool version2, bool verbose)
{
    if (verbose)
        printf("Processing docking data from file %s\n", fileName);

    memory.clear();

    StringList fileLines;
    ReadFileLines(fileName, fileLines);

    BigInt stompMask = 0;
    BigInt stompValueMask = 0;
    BigIntList stompValueMaskList;
    for (const auto& line: fileLines)
    {
        StringList tokens;
        Tokenize(line, tokens, ' ');

        assert(tokens.size() == 3);
        assert(tokens[1] == "=");

        const auto& first = tokens[0];
        if (first == "mask")
        {
            if (verbose)
                printf("  Found mask: ");

            stompMask = 0;
            stompValueMask = 0;
            stompValueMaskList.clear();

            const auto& mask = tokens[2];
            for (const auto& ch: mask)
            {
                if (verbose)
                    printf("%c", ch);

                stompMask <<= 1;
                stompValueMask <<= 1;

                if (version2)
                    switch (ch)
                    {
                        case 'X':
                            // stomp bit is a "floater"
                            stompMask |= 1;
                            break;
                        case '0':
                            // leave original value bit alone
                            break;
                        case '1':
                            // stomp bit with 1
                            stompMask |= 1;
                            stompValueMask |= 1;
                            break;
                        default:
                            assert(false && "Invalid mask character!");
                            break;
                    }
                else
                    switch (ch)
                    {
                        case 'X':
                            // leave original value bit alone
                            break;
                        case '0':
                            // stomp bit with 0
                            stompMask |= 1;
                            break;
                        case '1':
                            // stomp bit with 1
                            stompMask |= 1;
                            stompValueMask |= 1;
                            break;
                        default:
                            assert(false && "Invalid mask character!");
                            break;
                    }
            }

            if (verbose)
                printf("\n");

            if (version2)
                GenerateDockingStompValueMaskList(stompMask, stompValueMask, stompValueMaskList, verbose);
            else
                stompValueMaskList.push_back(stompValueMask);

            if (verbose)
            {
                printf(
                    "    Stomp mask = 0x%016llX\n"
                    "    Stomp value masks:\n",
                    stompMask);

                for (const BigInt thisStompValueMask: stompValueMaskList)
                {
                    printf("      0x%016llX\n", thisStompValueMask);
                }
            }
        }
        else
        {
            assert(first.compare(0, 4, "mem[") == 0);
            assert(first.back() == ']');

            const BigInt location = atoll(first.c_str() + 4);
            const BigInt value = atoll(tokens[2].c_str());

            if (verbose)
                printf("  mem[%lld] = %lld\n", location, value);

            WriteDockingDataValue(memory, stompMask, stompValueMaskList, location, value, version2, verbose);
        }
    }
}

void PrintDockingDataMemory(const char* name, const DockingDataMemory& memory)
{
    printf("Docking memory in %s:\n", name);
    for (const auto& item: memory)
    {
        printf("  Location %lld, value %lld\n", item.first, item.second);
    }
}

BigInt CalcSumDockingDataMemoryValues(const DockingDataMemory& memory)
{
    BigInt sum = 0;

    for (const auto& node: memory)
    {
        sum += node.second;
    }

    return sum;
}

void RunDockingData()
{
    DockingDataMemory testData;
    ProcessDockingDataFile("Day14TestInput.txt", testData, false, true);
    PrintDockingDataMemory("test (version 1)", testData);
    printf("Test (version 1) data sum of all memory values = %lld\n", CalcSumDockingDataMemoryValues(testData));

    DockingDataMemory testDataB;
    ProcessDockingDataFile("Day14TestInputB.txt", testDataB, true, true);
    PrintDockingDataMemory("testB (version 2)", testDataB);
    printf("Test B (version 2) data sum of all memory values = %lld\n", CalcSumDockingDataMemoryValues(testDataB));

    DockingDataMemory mainData;
    ProcessDockingDataFile("Day14Input.txt", mainData, false, false);
    //PrintDockingDataMemory("main version 1", mainData);
    printf("Main data sum of all memory values (version 1) = %lld\n", CalcSumDockingDataMemoryValues(mainData));
    ProcessDockingDataFile("Day14Input.txt", mainData, true, false);
    //PrintDockingDataMemory("main version 2", mainData);
    printf("Main data sum of all memory values (version 2) = %lld\n", CalcSumDockingDataMemoryValues(mainData));
}


////////////////////////////
// Problem 15 - Rambunctious Recitation

void ProgressRambunctiousList(BigIntList& numberMap, BigInt& lastNumber, BigInt& lastNumberIndex, bool verbose)
{
    if (verbose)
        printf("  Last number was %lld at index %lld,", lastNumber, lastNumberIndex);

    BigInt newNumber = 0;

    const BigInt prevNumberIndex = numberMap[lastNumber];

    if (prevNumberIndex >= 0)
    {
        newNumber = lastNumberIndex - prevNumberIndex;

        if (verbose)
            printf(
                " which was found previously at index %lld, so new number = last index - prev index = %lld - %lld = %lld\n",
                prevNumberIndex,
                lastNumberIndex,
                prevNumberIndex,
                newNumber);
    }
    else
    {
        if (verbose)
            printf(" which was not found, so new number = 0\n");
    }

    numberMap[lastNumber] = lastNumberIndex;

    lastNumber = newNumber;
    ++lastNumberIndex;
}

BigInt CalcNthRambunctiousNumber(BigIntList& numberMap, BigInt lastNumber, BigInt lastNumberIndex, BigInt n, bool verbose)
{
    const BigInt nMinusOne = n - 1;

    assert(lastNumberIndex < nMinusOne);

    while (lastNumberIndex < nMinusOne)
        ProgressRambunctiousList(numberMap, lastNumber, lastNumberIndex, verbose);

    assert(lastNumberIndex == nMinusOne);

    return lastNumber;
}

void CalcAndPrintNthRambunctiousNumber(BigIntInitList startList, BigInt n, bool verbose)
{
    BigIntList numberMap;
    numberMap.resize(n, -1);

    printf("Given list ");
    for (const BigInt number: startList)
        printf("%lld,", number);

    const BigInt lastNumberIndex = (BigInt)startList.size() - 1;
    BigInt index = lastNumberIndex;

    auto iter = std::rbegin(startList);
    const auto iterEnd = std::rend(startList);

    BigInt lastNumber = *iter;

    --index;
    ++iter;
    while (iter != iterEnd)
    {
        const BigInt thisNumber = *iter;
        numberMap[thisNumber] = index;

        --index;
        ++iter;
    }

    printf(" the %lldth number is %lld\n", n, CalcNthRambunctiousNumber(numberMap, lastNumber, lastNumberIndex, n, verbose));
}

void RunRambunctiousRecitation()
{
    CalcAndPrintNthRambunctiousNumber({ 0, 3, 6 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 1, 3, 2 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 2, 1, 3 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 1, 2, 3 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 2, 3, 1 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 3, 2, 1 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 3, 1, 2 }, 2020, false);
    CalcAndPrintNthRambunctiousNumber({ 15, 12, 0, 14, 3, 1 }, 2020, false);

    CalcAndPrintNthRambunctiousNumber({ 0, 3, 6 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 1, 3, 2 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 2, 1, 3 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 1, 2, 3 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 2, 3, 1 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 3, 2, 1 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 3, 1, 2 }, 30000000, false);
    CalcAndPrintNthRambunctiousNumber({ 15, 12, 0, 14, 3, 1 }, 30000000, false);
}


////////////////////////////
// Problem 16 - Ticket Translation

class TicketTranslationData
{
public:
    TicketTranslationData(const char* fileName)
    {
        StringList fileLines;
        ReadFileLines(fileName, fileLines);

        // rules

        m_ruleList.clear();
        BigInt lineIndex = 0;
        while (!fileLines[lineIndex].empty())
        {
            m_ruleList.push_back(Rule());
            ParseRule(fileLines[lineIndex], m_ruleList.back());

            ++lineIndex;
        }

        // my ticket

        ++lineIndex;
        assert(fileLines[lineIndex] == "your ticket:");
        ++lineIndex;
        ParseTicket(fileLines[lineIndex], m_myTicket);
        ++lineIndex;
        assert(fileLines[lineIndex].empty());

        // nearby tickets

        ++lineIndex;
        assert(fileLines[lineIndex] == "nearby tickets:");
        ++lineIndex;
        m_nearbyTickets.clear();
        for (; lineIndex < (BigInt)fileLines.size(); ++lineIndex)
        {
            m_nearbyTickets.push_back(Ticket());
            ParseTicket(fileLines[lineIndex], m_nearbyTickets.back());
        }
    }

    BigInt CalcTicketScanningErrorRate() const
    {
        BigInt errorRate = 0;
        for (const auto& nearbyTicket: m_nearbyTickets)
        {
            for (const BigInt ticketValue: nearbyTicket)
            {
                if (!CanTicketNumberBeValid(ticketValue))
                    errorRate += ticketValue;
            }
        }
        return errorRate;
    }

    void RemoveInvalidTickets()
    {
        const auto removeIter = std::remove_if(
            m_nearbyTickets.begin(), m_nearbyTickets.end(), [&](const Ticket& ticket) { return !CanTicketBeValid(ticket); });

        if (removeIter != m_nearbyTickets.end())
            m_nearbyTickets.erase(removeIter, m_nearbyTickets.end());
    }

    void CalcFieldIndices(bool verbose)
    {
        if (verbose)
            printf("Calculating field indices\n\nRemoving invalid tickets\n");

        RemoveInvalidTickets();

        if (verbose)
            printf("Absorbing tickets into rule field index sets\n");
        for (const auto& ticket: m_nearbyTickets)
        {
            if (verbose)
                printf("  One ticket\n");

            for (auto& rule: m_ruleList)
            {
                if (verbose)
                    printf("    One rule\n");

                AbsorbTicketIntoRuleFieldIndexSet(ticket, rule);
            }
        }

        if (verbose)
            PrintRuleFields();

        for (;;)
        {
            const bool wasAbleToReduce = ReduceRuleFieldSets();

            if (verbose)
                PrintRuleFields();

            if (!wasAbleToReduce)
                break;
        }
    }

    BigInt CalcProductOfMyTicketFieldsWithPrefix(const std::string& prefix, bool verbose) const
    {
        BigInt product = 1;
        for (const auto& rule: m_ruleList)
        {
            if (rule.name.compare(0, prefix.length(), prefix) == 0)
            {
                const BigInt myTicketFieldValue = m_myTicket[rule.fieldIndex];
                product *= myTicketFieldValue;

                if (verbose)
                    printf(
                        "Found valid field '%s', product = product * field value = %lld * %lld = %lld\n",
                        rule.name.c_str(),
                        product / myTicketFieldValue,
                        myTicketFieldValue,
                        product);
            }
            else
            {
                if (verbose)
                    printf("Field '%s' does not contain prefix '%s'\n", rule.name.c_str(), prefix.c_str());
            }
        }
        return product;
    }

private:
    struct Rule
    {
        std::string name;
        BigInt range1Min, range1Max;
        BigInt range2Min, range2Max;
        BigIntSet fieldIndexSet;
        BigInt fieldIndex;

        Rule() : name(), range1Min(-1), range1Max(-1), range2Min(-1), range2Max(-1), fieldIndexSet(), fieldIndex(-1) {}
    };

    void ParseRule(const std::string& st, Rule& rule)
    {
        StringList tokens;
        Tokenize(st, tokens, ' ');

        BigInt rangeStartIndex;
        if (tokens.size() == 4)
        {
            rule.name = tokens[0];
            rangeStartIndex = 1;
        }
        else
        {
            assert(tokens.size() == 5);
            rule.name = tokens[0] + " " + tokens[1];   // ignore the ":" for now since we don't care about names yet
            rangeStartIndex = 2;
        }

        assert(tokens[rangeStartIndex + 1] == "or");

        ParseRange(tokens[rangeStartIndex], rule.range1Min, rule.range1Max);
        ParseRange(tokens[rangeStartIndex + 2], rule.range2Min, rule.range2Max);
    }

    void ParseRange(const std::string& st, BigInt& min, BigInt& max)
    {
        StringList tokens;
        Tokenize(st, tokens, '-');
        assert(tokens.size() == 2);
        min = atoll(tokens[0].c_str());
        max = atoll(tokens[1].c_str());
    }

    typedef std::vector<Rule> RuleList;

    RuleList m_ruleList;

    void PrintRuleFields()
    {
        printf("Rule fields:\n");
        for (const auto& rule: m_ruleList)
        {
            printf("  Rule: ");
            if (!rule.fieldIndexSet.empty())
            {
                printf("set = ");
                for (const BigInt index: rule.fieldIndexSet)
                {
                    printf("%lld ", index);
                }
                printf("\n");
            }
            else
            {
                printf("index = %lld\n", rule.fieldIndex);
            }
        }
    }

    bool ReduceRuleFieldSets()
    {
        // look for a set of one, then assign that index there and remove it from all other sets
        BigInt singularIndex = -1;
        for (const auto& rule: m_ruleList)
        {
            if (rule.fieldIndexSet.size() == 1)
            {
                assert(singularIndex < 0);
                singularIndex = *rule.fieldIndexSet.cbegin();
            }
        }

        if (singularIndex < 0)
            return false;

        for (auto& rule: m_ruleList)
        {
            if (!rule.fieldIndexSet.empty())
            {
                if (rule.fieldIndexSet.size() == 1)
                {
                    assert(singularIndex == *rule.fieldIndexSet.cbegin());
                    rule.fieldIndex = singularIndex;
                    rule.fieldIndexSet.clear();
                }
                else
                {
                    rule.fieldIndexSet.erase(singularIndex);
                }
            }
        }

        return true;
    }

    typedef BigIntList Ticket;
    typedef std::vector<Ticket> TicketList;

    void ParseTicket(const std::string& st, Ticket& ticket) { ParseBigIntList(st, ticket, ','); }

    bool CanTicketBeValid(const Ticket& ticket) const
    {
        for (const BigInt ticketValue: ticket)
        {
            if (!CanTicketNumberBeValid(ticketValue))
                return false;
        }
        return true;
    }

    bool CanTicketNumberBeValid(BigInt number) const
    {
        for (const auto& rule: m_ruleList)
        {
            if (IsTicketNumberAllowedByRule(number, rule))
                return true;
        }
        return false;
    }

    bool IsTicketNumberAllowedByRule(BigInt number, const Rule& rule) const
    {
        return ((number >= rule.range1Min) && (number <= rule.range1Max))
               || ((number >= rule.range2Min) && (number <= rule.range2Max));
    }

    void AbsorbTicketIntoRuleFieldIndexSet(const Ticket& ticket, Rule& rule)
    {
        BigIntSet thisSet;
        for (BigInt i = 0; i < (BigInt)ticket.size(); ++i)
        {
            const BigInt ticketNumber = ticket[i];
            if (IsTicketNumberAllowedByRule(ticketNumber, rule))
                thisSet.insert(i);
        }

        if (rule.fieldIndexSet.empty())
            rule.fieldIndexSet.swap(thisSet);
        else
            IntersectSet(rule.fieldIndexSet, thisSet);
    }

    Ticket m_myTicket;
    TicketList m_nearbyTickets;
};

void RunTicketTranslation()
{
    TicketTranslationData testData("Day16TestInput.txt");
    printf("Test data ticket scanning error rate = %lld\n", testData.CalcTicketScanningErrorRate());
    //testData.CalcFieldIndices(false); -- this set of test data does not have a valid solution here

    TicketTranslationData testDataB("Day16TestInputB.txt");
    printf("Test data B ticket scanning error rate = %lld\n", testDataB.CalcTicketScanningErrorRate());
    testDataB.CalcFieldIndices(true);

    TicketTranslationData mainData("Day16Input.txt");
    printf("Main data ticket scanning error rate = %lld\n", mainData.CalcTicketScanningErrorRate());
    mainData.CalcFieldIndices(true);
    const std::string prefix = "departure";
    printf(
        "Main data tickets, product of my ticket fields with prefix %s = %lld\n",
        prefix.c_str(),
        mainData.CalcProductOfMyTicketFieldsWithPrefix(prefix, true));
}


////////////////////////////
// Problem 17 - Conway Cubes

class PocketDimension
{
public:
    PocketDimension(const char* fileName)
    {
        m_grid.push_back(FullDimension());
        m_grid[0].push_back(DimensionLayer());
        ReadFileLines(fileName, m_grid[0].back());
        m_nextGrid = m_grid;
    }

    void Print() const
    {
        BigInt minX, maxX, minY, maxY, minZ, maxZ, minW, maxW;
        CalcActiveExtents(minX, maxX, minY, maxY, minZ, maxZ, minW, maxW);

        for (BigInt w = minW; w <= maxW; ++w)
        {
            for (BigInt z = minZ; z <= maxZ; ++z)
            {
                for (BigInt y = minY; y <= maxY; ++y)
                {
                    for (BigInt x = minX; x <= maxX; ++x)
                    {
                        const char thisState = GetCellState(x, y, z, w);
                        printf("%c", thisState);
                    }
                    printf("\n");
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("\n");
    }

    void Iterate(bool includeHyperDimension)
    {
        GrowIfNecessary();

        BigInt minX, maxX, minY, maxY, minZ, maxZ, minW, maxW;
        CalcActiveExtents(minX, maxX, minY, maxY, minZ, maxZ, minW, maxW, true, includeHyperDimension);

        for (BigInt w = minW; w <= maxW; ++w)
        {
            for (BigInt z = minZ; z <= maxZ; ++z)
            {
                for (BigInt y = minY; y <= maxY; ++y)
                {
                    for (BigInt x = minX; x <= maxX; ++x)
                    {
                        const char thisState = GetCellState(x, y, z, w);
                        const BigInt numActiveNeighbors = CountActiveNeighbors(x, y, z, w, includeHyperDimension);
                        if (thisState == ACTIVE_STATE)
                        {
                            if ((numActiveNeighbors == 2) || (numActiveNeighbors == 3))
                                SetNextCellState(x, y, z, w, ACTIVE_STATE);
                            else
                                SetNextCellState(x, y, z, w, INACTIVE_STATE);
                        }
                        else
                        {
                            assert(thisState == INACTIVE_STATE);

                            if (numActiveNeighbors == 3)
                                SetNextCellState(x, y, z, w, ACTIVE_STATE);
                            else
                                SetNextCellState(x, y, z, w, INACTIVE_STATE);
                        }
                    }
                }
            }
        }

        m_grid.swap(m_nextGrid);
    }

    BigInt CountActiveCellsAfterIterations(BigInt numIterations, bool includeHyperDimension, bool verbose)
    {
        for (BigInt i = 0; i < numIterations; ++i)
        {
            Iterate(includeHyperDimension);

            if (verbose)
            {
                printf("After iteration #%lld:\n\n", i + 1);
                Print();
            }
        }
        return CountActiveInGrid();
    }

private:
    typedef StringList DimensionLayer;
    typedef std::vector<DimensionLayer> FullDimension;
    typedef std::vector<FullDimension> HyperDimension;

    static const char ACTIVE_STATE = '#';
    static const char INACTIVE_STATE = '.';

    void CalcActiveExtents(
        BigInt& minX,
        BigInt& maxX,
        BigInt& minY,
        BigInt& maxY,
        BigInt& minZ,
        BigInt& maxZ,
        BigInt& minW,
        BigInt& maxW,
        bool puffXYZ = false,
        bool puffW = false) const
    {
        const BigInt sizeX = m_grid[0][0][0].length();
        const BigInt sizeY = m_grid[0][0].size();
        const BigInt sizeZ = m_grid[0].size();
        const BigInt sizeW = m_grid.size();

        minX = MAX_BIG_INT;
        maxX = MIN_BIG_INT;
        minY = MAX_BIG_INT;
        maxY = MIN_BIG_INT;
        minZ = MAX_BIG_INT;
        maxZ = MIN_BIG_INT;
        minW = MAX_BIG_INT;
        maxW = MIN_BIG_INT;

        for (BigInt w = 0; w < sizeW; ++w)
        {
            for (BigInt z = 0; z < sizeZ; ++z)
            {
                for (BigInt y = 0; y < sizeY; ++y)
                {
                    for (BigInt x = 0; x < sizeX; ++x)
                    {
                        if (GetCellState(x, y, z, w) == ACTIVE_STATE)
                        {
                            minX = std::min(x, minX);
                            maxX = std::max(x, maxX);

                            minY = std::min(y, minY);
                            maxY = std::max(y, maxY);

                            minZ = std::min(z, minZ);
                            maxZ = std::max(z, maxZ);

                            minW = std::min(w, minW);
                            maxW = std::max(w, maxW);
                        }
                    }
                }
            }
        }

        if (puffXYZ)
        {
            minX = std::max(0LL, minX - 1);
            maxX = std::min(sizeX - 1, maxX + 1);
            minY = std::max(0LL, minY - 1);
            maxY = std::min(sizeY - 1, maxY + 1);
            minZ = std::max(0LL, minZ - 1);
            maxZ = std::min(sizeZ - 1, maxZ + 1);
        }

        if (puffW)
        {
            minW = std::max(0LL, minW - 1);
            maxW = std::min(sizeW - 1, maxW + 1);
        }
    }

    void GrowIfNecessary()
    {
        const BigInt sizeX = m_grid[0][0][0].length();
        const BigInt sizeY = m_grid[0][0].size();
        const BigInt sizeZ = m_grid[0].size();
        const BigInt sizeW = m_grid.size();

        const BigInt maxX = sizeX - 1;
        const BigInt maxY = sizeY - 1;
        const BigInt maxZ = sizeZ - 1;
        const BigInt maxW = sizeW - 1;

        if (!IsAnyActiveInRange(0, 0, 0, maxY, 0, maxZ, 0, maxW) && !IsAnyActiveInRange(maxX, maxX, 0, maxY, 0, maxZ, 0, maxW)
            && !IsAnyActiveInRange(0, maxX, 0, 0, 0, maxZ, 0, maxW) && !IsAnyActiveInRange(0, maxX, maxY, maxY, 0, maxZ, 0, maxW)
            && !IsAnyActiveInRange(0, maxX, 0, maxY, 0, 0, 0, maxW) && !IsAnyActiveInRange(0, maxX, 0, maxY, maxZ, maxZ, 0, maxW)
            && !IsAnyActiveInRange(0, maxX, 0, maxY, 0, maxZ, 0, 0) && !IsAnyActiveInRange(0, maxX, 0, maxY, 0, maxZ, maxW, maxW))
            return;

        // we have at least one active cell on a border, so grow x 3 in each dimension

        const BigInt newSizeX = sizeX * 3;
        const BigInt newSizeY = sizeY * 3;
        const BigInt newSizeZ = sizeZ * 3;
        const BigInt newSizeW = sizeW * 3;

        const std::string prefix(sizeX, INACTIVE_STATE);
        const std::string suffix(sizeY, INACTIVE_STATE);
        for (auto& cube: m_grid)
        {
            for (auto& layer: cube)
            {
                for (auto& line: layer)
                {
                    line.insert(0, prefix);
                    line.append(suffix);
                }
            }
        }
        for (auto& cube: m_nextGrid)
        {
            for (auto& layer: cube)
            {
                for (auto& line: layer)
                {
                    line.insert(0, prefix);
                    line.append(suffix);
                }
            }
        }

        const std::string blankLine(newSizeX, INACTIVE_STATE);
        for (auto& cube: m_grid)
        {
            for (auto& layer: cube)
            {
                layer.insert(layer.begin(), sizeY, blankLine);
                layer.insert(layer.end(), sizeY, blankLine);
            }
        }
        for (auto& cube: m_nextGrid)
        {
            for (auto& layer: cube)
            {
                layer.insert(layer.begin(), sizeY, blankLine);
                layer.insert(layer.end(), sizeY, blankLine);
            }
        }

        const DimensionLayer blankLayer(newSizeY, blankLine);
        for (auto& cube: m_grid)
        {
            cube.insert(cube.begin(), sizeZ, blankLayer);
            cube.insert(cube.end(), sizeZ, blankLayer);
        }
        for (auto& cube: m_nextGrid)
        {
            cube.insert(cube.begin(), sizeZ, blankLayer);
            cube.insert(cube.end(), sizeZ, blankLayer);
        }

        const FullDimension blankCube(newSizeZ, blankLayer);
        m_grid.insert(m_grid.begin(), sizeW, blankCube);
        m_grid.insert(m_grid.end(), sizeW, blankCube);
        m_nextGrid.insert(m_nextGrid.begin(), sizeW, blankCube);
        m_nextGrid.insert(m_nextGrid.end(), sizeW, blankCube);
    }

    bool IsAnyActiveInRange(
        BigInt minX, BigInt maxX, BigInt minY, BigInt maxY, BigInt minZ, BigInt maxZ, BigInt minW, BigInt maxW) const
    {
        return CountActiveInRange(minX, maxX, minY, maxY, minZ, maxZ, minW, maxW) > 0;
    }

    BigInt CountActiveInGrid() const
    {
        const BigInt maxX = m_grid[0][0][0].length() - 1;
        const BigInt maxY = m_grid[0][0].size() - 1;
        const BigInt maxZ = m_grid[0].size() - 1;
        const BigInt maxW = m_grid.size() - 1;

        return CountActiveInRange(0, maxX, 0, maxY, 0, maxZ, 0, maxW);
    }

    BigInt CountActiveInRange(
        BigInt minX, BigInt maxX, BigInt minY, BigInt maxY, BigInt minZ, BigInt maxZ, BigInt minW, BigInt maxW) const
    {
        BigInt count = 0;
        for (BigInt w = minW; w <= maxW; ++w)
        {
            for (BigInt z = minZ; z <= maxZ; ++z)
            {
                for (BigInt y = minY; y <= maxY; ++y)
                {
                    for (BigInt x = minX; x <= maxX; ++x)
                    {
                        if (GetCellState(x, y, z, w) == ACTIVE_STATE)
                        {
                            ++count;
                        }
                    }
                }
            }
        }

        return count;
    }

    BigInt CountActiveNeighbors(BigInt x, BigInt y, BigInt z, BigInt w, bool includeHyperDimension) const
    {
        const BigInt x0 = std::max(0LL, x - 1);
        const BigInt x1 = std::min((BigInt)m_grid[0][0][0].length() - 1, x + 1);
        const BigInt y0 = std::max(0LL, y - 1);
        const BigInt y1 = std::min((BigInt)m_grid[0][0].size() - 1, y + 1);
        const BigInt z0 = std::max(0LL, z - 1);
        const BigInt z1 = std::min((BigInt)m_grid[0].size() - 1, z + 1);
        const BigInt w0 = includeHyperDimension ? std::max(0LL, w - 1) : w;
        const BigInt w1 = includeHyperDimension ? std::min((BigInt)m_grid.size() - 1, w + 1) : w;

        BigInt sum = 0;
        for (BigInt wi = w0; wi <= w1; ++wi)
        {
            for (BigInt zi = z0; zi <= z1; ++zi)
            {
                for (BigInt yi = y0; yi <= y1; ++yi)
                {
                    for (BigInt xi = x0; xi <= x1; ++xi)
                    {
                        if ((xi == x) && (yi == y) && (zi == z) && (wi == w))
                            continue;

                        if (GetCellState(xi, yi, zi, wi) == ACTIVE_STATE)
                            ++sum;
                    }
                }
            }
        }

        return sum;
    }

    char GetCellState(BigInt x, BigInt y, BigInt z, BigInt w) const { return m_grid[w][z][y][x]; }
    void SetNextCellState(BigInt x, BigInt y, BigInt z, BigInt w, char newState) { m_nextGrid[w][z][y][x] = newState; }

    HyperDimension m_grid;
    HyperDimension m_nextGrid;
};

void RunConwayCubes()
{
    const BigInt NUM_ITERATIONS = 6;

    PocketDimension testData("Day17TestInput.txt");
    printf("Test data init state\n\n");
    testData.Print();
    printf(
        "Test data num active cells after %lld iterations = %lld\n",
        NUM_ITERATIONS,
        testData.CountActiveCellsAfterIterations(NUM_ITERATIONS, false, true));

    printf("\nMain data init state\n\n");
    PocketDimension mainData("Day17Input.txt");
    mainData.Print();
    printf(
        "Main data num active cells after %lld iterations = %lld\n",
        NUM_ITERATIONS,
        mainData.CountActiveCellsAfterIterations(NUM_ITERATIONS, false, false));

    PocketDimension testData2("Day17TestInput.txt");
    printf("Test data 2 init state\n\n");
    //testData2.Print();
    printf(
        "Test data 2 num active cells after %lld iterations = %lld\n",
        NUM_ITERATIONS,
        testData2.CountActiveCellsAfterIterations(NUM_ITERATIONS, true, true));

    printf("\nMain data 2 init state\n\n");
    PocketDimension mainData2("Day17Input.txt");
    //mainData2.Print();
    printf(
        "Main data 2 num active cells after %lld iterations = %lld\n",
        NUM_ITERATIONS,
        mainData2.CountActiveCellsAfterIterations(NUM_ITERATIONS, true, false));
}


////////////////////////////
// Problem 18 - Operation Order

struct ExpressionNode;

typedef std::vector<ExpressionNode> Expression;

struct ExpressionNode
{
    bool add;
    bool multiply;
    BigInt number;
    Expression subExpression;

    ExpressionNode() : add(false), multiply(false), number(0), subExpression() {}
};

void ParseExpressionString(const char*& st, Expression& expression, bool verbose)
{
    static std::string numberParse;
    for (;;)
    {
        if (*st == '\0')
            break;
        if (*st == ')')
        {
            ++st;
            if (*st == ' ')
                ++st;
            break;
        }

        expression.push_back(ExpressionNode());
        ExpressionNode& node = expression.back();

        if (*st == '*')
        {
            node.multiply = true;
            ++st;
            assert(*st == ' ');
            ++st;
        }
        else if (*st == '+')
        {
            node.add = true;
            ++st;
            assert(*st == ' ');
            ++st;
        }

        if (*st == '(')
        {
            ++st;
            ParseExpressionString(st, node.subExpression, verbose);
        }
        else
        {
            numberParse.clear();
            while ((*st >= '0') && (*st <= '9'))
            {
                numberParse += *st;
                ++st;
            }
            node.number = atoll(numberParse.c_str());

            if (*st == ' ')
                ++st;
        }
    }
}

BigInt CalcExpression(Expression& expression, bool withAddPrecedence, bool verbose)
{
    assert(!expression.empty());

    // first evaluate subexpressions

    for (auto& node: expression)
    {
        if (!node.subExpression.empty())
        {
            node.number = CalcExpression(node.subExpression, withAddPrecedence, verbose);
            node.subExpression.clear();
        }
    }

    // then collapse adds if we are using add precedence

    if (withAddPrecedence)
    {
        ExpressionNode* pFirstAddendNode = &(expression[0]);
        for (BigInt i = 1; i < (BigInt)expression.size(); ++i)
        {
            ExpressionNode& thisNode = expression[i];
            assert(thisNode.subExpression.empty());

            if (thisNode.add)
            {
                // add in this number to the first addend of this sequence
                pFirstAddendNode->number += thisNode.number;

                // turn this node into a multiply by 1
                thisNode.add = false;
                thisNode.multiply = true;
                thisNode.number = 1;
            }
            else
            {
                pFirstAddendNode = &thisNode;
            }
        }
    }

    // then add/multiply all the remaining values

    BigInt runningNumber = 0;
    bool sawFirstNode = false;
    for (const auto& node: expression)
    {
        assert(node.subExpression.empty());

        BigInt thisNumber = node.number;
        if (!sawFirstNode)
        {
            assert(!node.add && !node.multiply);
            sawFirstNode = true;
            runningNumber = thisNumber;
        }
        else
        {
            if (node.add)
            {
                assert(!node.multiply);
                assert(!withAddPrecedence);   // all adds should have been eliminated up above

                runningNumber += thisNumber;
            }
            else
            {
                assert(node.multiply);

                runningNumber *= thisNumber;
            }
        }
    }

    return runningNumber;
}

BigInt ParseAndCalcExpression(const std::string& st, bool withAddPrecedence, bool verbose)
{
    assert(!st.empty());

    const char* pSt = &(st[0]);
    Expression expression;
    ParseExpressionString(pSt, expression, verbose);

    return CalcExpression(expression, withAddPrecedence, verbose);
}

BigInt CalcExpressionListSum(const StringList& expressionList, bool withAddPrecedence, bool verbose)
{
    BigInt sum = 0;
    for (const auto& expression: expressionList)
    {
        const BigInt answer = ParseAndCalcExpression(expression, withAddPrecedence, verbose);
        sum += answer;

        if (verbose)
            printf("  %s = %lld\n", expression.c_str(), answer);
    }

    return sum;
}

void RunOperationOrder()
{
    StringList testData;
    ReadFileLines("Day18TestInput.txt", testData);
    printf("Test data, expression list sum = %lld\n", CalcExpressionListSum(testData, false, true));
    printf("Test data, expression list sum with add precedence = %lld\n", CalcExpressionListSum(testData, true, true));

    StringList mainData;
    ReadFileLines("Day18Input.txt", mainData);
    printf("Main data, expression list sum = %lld\n", CalcExpressionListSum(mainData, false, false));
    printf("Main data, expression list sum with add precedence = %lld\n", CalcExpressionListSum(mainData, true, false));
}


////////////////////////////
// Problem 19 - Monster Messages

class MonsterMessages
{
public:
    MonsterMessages(const char* fileName)
    {
        StringList fileLines;
        ReadFileLines(fileName, fileLines);

        m_ruleList.resize(fileLines.size());

        BigInt lineIndex = 0;
        while (!fileLines[lineIndex].empty())
        {
            BigInt ruleIndex = 0;
            Rule rule;
            ParseRule(fileLines[lineIndex], ruleIndex, rule);
            SwapRules(m_ruleList[ruleIndex], rule);
            ++lineIndex;
            assert(lineIndex < (BigInt)fileLines.size());
        }

        ++lineIndex;
        while (lineIndex < (BigInt)fileLines.size())
        {
            m_messageList.push_back(std::string());
            m_messageList.back().swap(fileLines[lineIndex]);
            ++lineIndex;
        }
    }

    BigInt CalcNumMessagesMatchPrimeRule(bool verbose) const
    {
        // derive messages that match

        UnorderedStringSet matchingSet;
        DeriveMatchingMessages(0, matchingSet);

        if (verbose)
        {
            printf("Deriving matching messages:\n");
            for (const auto& msg: matchingSet)
            {
                printf("  %s\n", msg.c_str());
            }
        }

        // compare to messages in list

        BigInt numMatches = 0;
        for (const auto& message: m_messageList)
        {
            numMatches += matchingSet.count(message);
        }

        return numMatches;
    }

    void DeriveMatchingMessages(BigInt ruleIndex, UnorderedStringSet& matchingSet) const
    {
        matchingSet.clear();

        StringList stringList;
        IterateDeriveMatchingMessages(ruleIndex, stringList);
        for (const auto& st: stringList)
        {
            matchingSet.insert(st);
        }
    }

    void DeriveMatchingMessages(BigInt ruleIndex, StringList& stringList) const
    {
        IterateDeriveMatchingMessages(ruleIndex, stringList);
    }

    BigInt CalcNumMessagesMatchingSpecialRuleComposite() const
    {
        // Here we are doing the matchine messages check, but with some rules changes.
        //
        // Rule 0 did consist and still consists of rules 8 and 11 in succession.
        //
        // But now:
        // Rule 8, which was simply 42, now becomes 42 OR 42 and 8.
        // Rule 11, which was 42 and 31, now becomes 42 31 OR 42 11 31.
        //
        // With these rule changes, it turns out rule 0 becomes:
        //    <match rule 42 N1 times> + <match rule 42 N2 times> + <match rule 31 N2 times>
        //
        // It also turns out that all of the possible values from rules 31 and 42 are the same length.
        // So in order to count the number of message matching this new rule 0, we
        // simply need to segment out the message and make sure the segments match the above
        // pattern (with N1 + N2 + N2 = full length of message, and all length numbers being
        // an even multiple of the segment length).

        const BigInt ruleLeft = 42;
        UnorderedStringSet leftStringSet;
        DeriveMatchingMessages(ruleLeft, leftStringSet);
        const BigInt segmentLength = (leftStringSet.begin())->length();
        for (const auto& st: leftStringSet)
            assert(st.length() == segmentLength);
        printf("Derived %lld left rule %lld matching strings\n", leftStringSet.size(), ruleLeft);

        const BigInt ruleRight = 31;
        UnorderedStringSet rightStringSet;
        DeriveMatchingMessages(ruleRight, rightStringSet);
        for (const auto& st: rightStringSet)
            assert(st.length() == segmentLength);
        printf("Derived %lld right rule %lld matching strings\n", rightStringSet.size(), ruleRight);

        BigInt numMatches = 0;

        printf("Searching messages for composite matches\n");
        std::string segment(segmentLength, ' ');
        for (const auto& message: m_messageList)
        {
            const BigInt messageLength = message.length();

            const lldiv_t div = lldiv(messageLength, segmentLength);

            if (div.rem != 0)
                continue;

            const BigInt numSegments = div.quot;
            const BigInt maxNumRightSegments = (numSegments - 1) / 2;

            BigInt segmentIndex = numSegments - 1;
            const BigInt earliestRightSegmentIndex = numSegments - maxNumRightSegments;
            for (; segmentIndex >= earliestRightSegmentIndex; --segmentIndex)
            {
                const auto segmentStartIter = std::next(message.begin(), segmentIndex * segmentLength);
                const auto segmentEndIter = std::next(segmentStartIter, segmentLength);
                std::copy(segmentStartIter, segmentEndIter, segment.begin());

                if (rightStringSet.count(segment) <= 0)
                    break;
            }

            const BigInt numRightSegments = numSegments - segmentIndex - 1;
            if (numRightSegments <= 0)
                continue;

            for (; segmentIndex >= 0; --segmentIndex)
            {
                const auto segmentStartIter = std::next(message.begin(), segmentIndex * segmentLength);
                const auto segmentEndIter = std::next(segmentStartIter, segmentLength);
                std::copy(segmentStartIter, segmentEndIter, segment.begin());

                if (leftStringSet.count(segment) <= 0)
                    break;
            }

            if (segmentIndex < 0)
            {
                ++numMatches;
                printf(
                    "  %s is valid, consists of %lld left segments + %lld right segments, full length = %lld, segmentIndex = %lld\n",
                    message.c_str(),
                    numSegments - numRightSegments,
                    numRightSegments,
                    message.length(),
                    segmentIndex);
            }
            else
            {
                printf(
                    "  %s is not valid, length = %lld, segmentIndex = %lld\n", message.c_str(), message.length(), segmentIndex);
            }
        }

        return numMatches;
    }

private:
    struct Rule
    {
        char specificRule;
        BigIntListList subRules;

        Rule() : specificRule(0), subRules() {}
    };

    typedef std::vector<Rule> RuleList;

    void ParseRule(const std::string& st, BigInt& index, Rule& rule)
    {
        StringList tokens;
        Tokenize(st, tokens, ' ');
        assert(tokens.size() >= 2);

        const std::string& indexString = tokens[0];
        assert(indexString[indexString.length() - 1] == ':');
        index = atoll(indexString.c_str());

        BigInt tokenIndex = 1;
        if (tokens[tokenIndex][0] == '\"')
        {
            assert(tokens[tokenIndex].length() == 3);
            assert(tokens[tokenIndex][2] == '\"');
            rule.specificRule = tokens[tokenIndex][1];
            return;
        }

        rule.subRules.push_back(BigIntList());
        BigIntList* pCurrSubRule = &(rule.subRules.back());
        for (; tokenIndex < (BigInt)tokens.size(); ++tokenIndex)
        {
            const std::string& token = tokens[tokenIndex];
            if (token == "|")
            {
                rule.subRules.push_back(BigIntList());
                pCurrSubRule = &(rule.subRules.back());
            }
            else
            {
                pCurrSubRule->push_back(atoll(token.c_str()));
            }
        }
    }

    static void SwapRules(Rule& lhs, Rule& rhs)
    {
        std::swap(lhs.specificRule, rhs.specificRule);
        lhs.subRules.swap(rhs.subRules);
    }

    void IterateDeriveMatchingMessages(BigInt ruleIndex, StringList& stringList) const
    {
        stringList.clear();
        const Rule& rule = m_ruleList[ruleIndex];
        if (rule.specificRule != '\0')
        {
            assert(rule.subRules.empty());
            stringList.push_back(std::string());
            stringList.back() += rule.specificRule;
        }
        else
        {
            for (const auto& subRule: rule.subRules)
            {
                StringList subStringList;
                for (const BigInt subSubRuleIndex: subRule)
                {
                    StringList subSubStringList;
                    IterateDeriveMatchingMessages(subSubRuleIndex, subSubStringList);
                    MultiplyStringLists(subStringList, subSubStringList);
                }
                stringList.insert(stringList.end(), subStringList.cbegin(), subStringList.cend());
            }
        }
    }

    RuleList m_ruleList;
    StringList m_messageList;
};

void RunMonsterMessages()
{
    MonsterMessages testData("Day19TestInput.txt");
    printf("Test data, num messages that completely match rule 0 = %lld\n", testData.CalcNumMessagesMatchPrimeRule(true));

    MonsterMessages mainData("Day19Input.txt");
    printf("Main data, num messages that completely match rule 0 = %lld\n\n", mainData.CalcNumMessagesMatchPrimeRule(false));

    MonsterMessages testDataB("Day19TestInputB.txt");
    printf(
        "Test data B num messages that match rule 0 with replacement rules = %lld\n",
        testDataB.CalcNumMessagesMatchingSpecialRuleComposite());

    printf(
        "Main data num messages that match rule 0 with replacement rules = %lld\n",
        mainData.CalcNumMessagesMatchingSpecialRuleComposite());
}



////////////////////////////
// Problem 20 - Jurassic Jigsaw

class JurassicJigsaw
{
public:
    JurassicJigsaw(const char* fileName) : m_edgeMap(), m_edgeTileList(), m_image(), m_imageEdgeSize(0)
    {
        StringList fileLines;
        ReadFileLines(fileName, fileLines);

        BigInt lineIndex = 0;
        while (lineIndex < (BigInt)fileLines.size())
        {
            m_tileList.push_back(Tile());
            Tile& tile = m_tileList.back();

            StringList tokens;
            Tokenize(fileLines[lineIndex], tokens, ' ');
            assert(tokens.size() == 2);
            assert(tokens[0] == "Tile");
            tile.id = atoll(tokens[1].c_str());
            ++lineIndex;

            while ((lineIndex < (BigInt)fileLines.size()) && !fileLines[lineIndex].empty())
            {
                tile.data.push_back(fileLines[lineIndex]);
                ++lineIndex;
            }

            ++lineIndex;
        }

        for (auto& tile: m_tileList)
        {
            RegisterTileEdges(tile);
        }
    }

    BigInt CalcCornerTileIdProduct() const
    {
        BigInt product = 1;
        for (const auto& tile: m_tileList)
        {
            BigInt numLoneEdges = 0;
            for (const BigInt edgeIndex: tile.edgeList)
            {
                if (m_edgeTileList[edgeIndex].size() == 1)
                    ++numLoneEdges;
            }
            if (numLoneEdges == 2)
            {
                product *= tile.id;
            }
        }
        return product;
    }

    void AssembleImage(bool verbose)
    {
        if (verbose)
        {
            printf("Tiles:\n");
            for (const auto& tile: m_tileList)
            {
                printf("  Tile id %lld, has edges:\n", tile.id);
                for (const BigInt edgeIndex: tile.edgeList)
                {
                    printf("    edge index %lld, which is shared by tiles: ", edgeIndex);
                    const auto& tilesList = m_edgeTileList[edgeIndex];
                    if (tilesList.size() == 1)
                    {
                        assert(tilesList[0] == tile.id);
                        printf("<none>\n");
                    }
                    else
                    {
                        for (const BigInt tileId: tilesList)
                        {
                            if (tileId == tile.id)
                                continue;
                            printf("%lld ", tileId);
                        }
                        printf("\n");
                    }
                }
            }
        }

        const BigInt numTiles = m_tileList.size();

        const double numTilesPerSideDouble = sqrt((double)numTiles);
        const BigInt numTilesPerSide = (BigInt)numTilesPerSideDouble;
        assert((double)numTilesPerSide == numTilesPerSideDouble);
        const BigInt lastSideIndex = numTilesPerSide - 1;

        BoolList tilesUsed(m_tileList.size(), false);
        BigIntList imageTileIndices(m_tileList.size(), -1);
        BigIntList tileRotations(m_tileList.size(), 0);
        BoolList tilesFlipped(m_tileList.size(), false);

        BigIntList edgeListToCheck(NUM_EDGES, UNKNOWN_EDGE);
        for (BigInt y = 0; y <= lastSideIndex; ++y)
        {
            for (BigInt x = 0; x <= lastSideIndex; ++x)
            {
                const BigInt imageTileIndexIndex = y * numTilesPerSide + x;

                edgeListToCheck[TOP_EDGE] = LONELY_EDGE;
                if (y > 0)
                {
                    const BigInt aboveTileIndexIndex = imageTileIndexIndex - numTilesPerSide;
                    edgeListToCheck[TOP_EDGE] = GetAlreadyPlacedTileEdge(
                        imageTileIndices[aboveTileIndexIndex],
                        tileRotations[aboveTileIndexIndex],
                        tilesFlipped[aboveTileIndexIndex],
                        BOTTOM_EDGE);
                }

                edgeListToCheck[LEFT_EDGE] = LONELY_EDGE;
                if (x > 0)
                {
                    const BigInt leftwardTileIndexIndex = imageTileIndexIndex - 1;
                    edgeListToCheck[LEFT_EDGE] = GetAlreadyPlacedTileEdge(
                        imageTileIndices[leftwardTileIndexIndex],
                        tileRotations[leftwardTileIndexIndex],
                        tilesFlipped[leftwardTileIndexIndex],
                        RIGHT_EDGE);
                }

                edgeListToCheck[RIGHT_EDGE] = UNKNOWN_EDGE;
                if (x >= lastSideIndex)
                    edgeListToCheck[RIGHT_EDGE] = LONELY_EDGE;

                edgeListToCheck[BOTTOM_EDGE] = UNKNOWN_EDGE;
                if (y >= lastSideIndex)
                    edgeListToCheck[BOTTOM_EDGE] = LONELY_EDGE;

                BigInt foundTileIndex = -1;
                BigInt rotated = 0;
                bool flipped = false;
                const bool foundTile = FindTileWithEdges(tilesUsed, edgeListToCheck, foundTileIndex, rotated, flipped);
                assert(foundTile);
                assert(foundTileIndex >= 0);

                tilesUsed[foundTileIndex] = true;
                imageTileIndices[imageTileIndexIndex] = foundTileIndex;
                tileRotations[imageTileIndexIndex] = rotated;
                tilesFlipped[imageTileIndexIndex] = flipped;
            }
        }

        const bool extraVerbose = false;

        if (verbose)
            printf("\n\nImage tiles arranged (before trimming and finalizing):\n\n");

        const BigInt tileEdgeSize = m_tileList[0].data.size();
        m_imageEdgeSize = numTilesPerSide * (tileEdgeSize - 2);
        const BigInt imageSize = m_imageEdgeSize * m_imageEdgeSize;
        m_image.reserve(imageSize);
        for (BigInt tileY = 0; tileY < numTilesPerSide; ++tileY)
        {
            for (BigInt y = 0; y < tileEdgeSize; ++y)
            {
                for (BigInt tileX = 0; tileX < numTilesPerSide; ++tileX)
                {
                    const BigInt tileIndexIndex = tileY * numTilesPerSide + tileX;
                    const BigInt tileIndex = imageTileIndices[tileIndexIndex];
                    const BigInt rotated = tileRotations[tileIndexIndex];
                    const bool flipped = tilesFlipped[tileIndexIndex];

                    for (BigInt x = 0; x < tileEdgeSize; ++x)
                    {
                        const char charInTile = GetCharInPlacedTile(tileIndex, rotated, flipped, x, y);

                        if ((x > 0) && (x < (tileEdgeSize - 1)) && (y > 0) && (y < (tileEdgeSize - 1)))
                            m_image.push_back(charInTile);

                        if (verbose)
                        {
                            if (extraVerbose)
                                printf("%lld%c%c ", rotated, charInTile, flipped ? 'f' : '-');
                            else
                                printf("%c", charInTile);
                        }
                    }
                    if (verbose)
                        printf(extraVerbose ? "  " : " ");
                }
                if (verbose)
                    printf("\n");
            }
            if (verbose)
                printf("\n\n");
        }

        if (verbose)
        {
            printf("\n\nComplete assembled image:\n\n");
            PrintImage();
        }
    }

    void PrintImage() const { PrintImage(m_image); }

    BigInt CountOccurrencesOfPattern(const StringList& pattern, bool markOccurrences, bool verbose)
    {
        std::string imageCopy;
        if (verbose && !markOccurrences)
            imageCopy = m_image;

        const BigInt patternSizeX = pattern[0].length();
        const BigInt patternSizeY = pattern.size();

        const BigInt lastX = m_imageEdgeSize - patternSizeX;
        const BigInt lastY = m_imageEdgeSize - patternSizeY;

        BigInt count = 0;
        for (BigInt y = 0; y <= lastY; ++y)
        {
            for (BigInt x = 0; x <= lastX; ++x)
            {
                bool matchFailed = false;
                for (BigInt yy = 0; !matchFailed && (yy < patternSizeY); ++yy)
                {
                    for (BigInt xx = 0; !matchFailed && (xx < patternSizeX); ++xx)
                    {
                        const char patternChar = pattern[yy][xx];

                        if (patternChar == ' ')
                            continue;

                        const BigInt imageCharIndex = (y + yy) * m_imageEdgeSize + x + xx;
                        const char imageChar = m_image[imageCharIndex];

                        if ((imageChar != patternChar) && (imageChar != MARK_OCCURRENCE_CHAR))
                            matchFailed = true;
                    }
                }
                if (!matchFailed)
                {
                    ++count;

                    if (verbose || markOccurrences)
                    {
                        for (BigInt yy = 0; !matchFailed && (yy < patternSizeY); ++yy)
                        {
                            for (BigInt xx = 0; !matchFailed && (xx < patternSizeX); ++xx)
                            {
                                const char patternChar = pattern[yy][xx];

                                if (patternChar == ' ')
                                    continue;

                                const BigInt imageCharIndex = (y + yy) * m_imageEdgeSize + x + xx;
                                if (markOccurrences)
                                    m_image[imageCharIndex] = MARK_OCCURRENCE_CHAR;
                                else
                                    imageCopy[imageCharIndex] = MARK_OCCURRENCE_CHAR;
                            }
                        }
                    }
                }
            }
        }

        if (verbose)
        {
            printf("Image with found pattern occurences:\n");
            if (markOccurrences)
                PrintImage();
            else
                PrintImage(imageCopy);
        }

        return count;
    }

    void MarkOccurencesOfPatternAllowTransform(const StringList& pattern, bool verbose)
    {
        if (verbose)
            printf("Checking for occurrences of pattern...\n");

        BigInt count = 0;
        for (BigInt i = 0; i < 4; ++i)
        {
            count = CountOccurrencesOfPattern(pattern, true, false);
            if (count > 0)
                break;

            if (verbose)
                printf("Occurrences of pattern not found. Rotating...\n");

            Rotate();
        }

        if (count <= 0)
        {
            if (verbose)
                printf("Occurrences of pattern not found. Flipping...\n");

            Flip();
            for (BigInt i = 0; i < 4; ++i)
            {
                count = CountOccurrencesOfPattern(pattern, true, false);
                if (count > 0)
                    break;

                if (verbose)
                    printf("Occurrences of pattern not found. Rotating...\n");

                Rotate();
            }
        }

        if (verbose)
            printf("%lld occurrences found!\n", count);
        PrintImage();
    }

    void Rotate()
    {
        std::string imageCopy;
        imageCopy.reserve(m_image.length());

        for (BigInt x = 0; x < m_imageEdgeSize; ++x)
        {
            for (BigInt y = m_imageEdgeSize - 1; y >= 0; --y)
            {
                imageCopy += m_image[y * m_imageEdgeSize + x];
            }
        }

        m_image.swap(imageCopy);
    }

    void Flip()
    {
        std::string imageCopy;
        imageCopy.reserve(m_image.length());

        for (BigInt y = 0; y < m_imageEdgeSize; ++y)
        {
            for (BigInt x = m_imageEdgeSize - 1; x >= 0; --x)
            {
                imageCopy += m_image[y * m_imageEdgeSize + x];
            }
        }

        m_image.swap(imageCopy);
    }

    BigInt CountCharsInImage(char ch) const { return std::count(m_image.cbegin(), m_image.cend(), ch); }

private:
    static const char MARK_OCCURRENCE_CHAR = 'O';

    struct Tile
    {
        BigInt id;
        StringList data;
        BigIntList edgeList;

        Tile() : id(0), data(), edgeList() {}
    };

    typedef std::vector<Tile> TileList;
    TileList m_tileList;

    void RegisterTileEdges(Tile& tile)
    {
        StringList edges;
        GetTileEdges(tile, edges);

        for (const auto& edge: edges)
        {
            BigInt edgeIndex;
            RegisterTileEdge(tile.id, edge, edgeIndex);
            tile.edgeList.push_back(edgeIndex);
        }
    }

    void RegisterTileEdge(BigInt tileId, const std::string& edge, BigInt& edgeIndex)
    {
        const auto findIter1 = m_edgeMap.find(edge);
        if (findIter1 != m_edgeMap.end())
        {
            edgeIndex = findIter1->second;
            BigIntList& edgeTileList = m_edgeTileList[edgeIndex];
            edgeTileList.push_back(tileId);
        }
        else
        {
            std::string reversedEdge = edge;
            std::reverse(reversedEdge.begin(), reversedEdge.end());

            const auto findIter2 = m_edgeMap.find(reversedEdge);
            if (findIter2 != m_edgeMap.end())
            {
                edgeIndex = findIter2->second;
                BigIntList& edgeTileList = m_edgeTileList[edgeIndex];
                edgeTileList.push_back(tileId);
            }
            else
            {
                edgeIndex = m_edgeTileList.size();
                m_edgeMap.insert(std::pair<std::string, BigInt>(edge, edgeIndex));
                m_edgeTileList.push_back(BigIntList());
                m_edgeTileList.back().push_back(tileId);
            }
        }
    }

    enum Edges
    {
        TOP_EDGE,
        RIGHT_EDGE,
        BOTTOM_EDGE,
        LEFT_EDGE,
        NUM_EDGES,
    };
    void GetTileEdges(const Tile& tile, StringList& edges)
    {
        edges.resize(NUM_EDGES);

        edges[TOP_EDGE] = tile.data[0];
        edges[BOTTOM_EDGE] = tile.data[tile.data.size() - 1];

        std::string leftVertEdge, rightVertEdge;
        for (const auto& tileLine: tile.data)
        {
            leftVertEdge += tileLine[0];
            rightVertEdge += tileLine[tileLine.length() - 1];
        }

        edges[LEFT_EDGE] = leftVertEdge;
        edges[RIGHT_EDGE] = rightVertEdge;
    }

    static const BigInt LONELY_EDGE = -1;
    static const BigInt UNKNOWN_EDGE = -2;

    bool FindTileWithEdges(
        const BoolList& tilesUsed,
        const BigIntList& edgeList,
        BigInt& tileIndex,
        BigInt& rotated,
        bool& flipped,
        BigInt startIndex = 0) const
    {
        static BigIntList thisTileEdgeList(4, UNKNOWN_EDGE);
        for (tileIndex = 0; tileIndex < (BigInt)m_tileList.size(); ++tileIndex)
        {
            if (tilesUsed[tileIndex])
                continue;

            const Tile& tile = m_tileList[tileIndex];

            for (BigInt edge = TOP_EDGE; edge < NUM_EDGES; ++edge)
            {
                if (m_edgeTileList[tile.edgeList[edge]].size() == 1)
                    thisTileEdgeList[edge] = LONELY_EDGE;
                else
                    thisTileEdgeList[edge] = tile.edgeList[edge];
            }

            if (DoEdgeListsMatch(edgeList, thisTileEdgeList, rotated, flipped))
                return true;
        }

        return false;
    }

    static bool DoEdgeListsMatch(const BigIntList& lhs, const BigIntList& rhs, BigInt& rotated, bool& flipped)
    {
        assert(lhs.size() == NUM_EDGES);
        assert(rhs.size() == NUM_EDGES);

        flipped = false;
        for (rotated = 0; rotated < NUM_EDGES; ++rotated)
        {
            if (DoEdgesMatch(lhs, rhs, rotated, flipped))
                return true;
        }

        flipped = true;
        for (rotated = 0; rotated < NUM_EDGES; ++rotated)
        {
            if (DoEdgesMatch(lhs, rhs, rotated, flipped))
                return true;
        }

        return false;
    }

    static bool DoEdgesMatch(const BigIntList& lhs, const BigIntList& rhs, BigInt rotated, bool flipped)
    {
        for (BigInt edge = TOP_EDGE; edge < NUM_EDGES; ++edge)
        {
            const BigInt leftEdge = lhs[edge];
            const BigInt transformedRightEdge = TransformPlacedEdgeToSourceEdge(edge, rotated, flipped);
            const BigInt rightEdge = rhs[transformedRightEdge];

            if ((leftEdge != UNKNOWN_EDGE) && (rightEdge != UNKNOWN_EDGE) && (leftEdge != rightEdge))
                return false;
        }

        return true;
    }

    BigInt GetAlreadyPlacedTileEdge(BigInt tileIndex, BigInt rotation, bool flipped, BigInt placedEdge) const
    {
        const Tile& tile = m_tileList[tileIndex];
        const BigInt originalEdgeDir = TransformPlacedEdgeToSourceEdge(placedEdge, rotation, flipped);
        return tile.edgeList[originalEdgeDir];
    }

    static BigInt TransformSourceEdgeToPlacedEdge(BigInt edge, BigInt rotation, bool flipped)
    {
        return flipped ? ((NUM_EDGES - edge + rotation) % NUM_EDGES) : ((edge + rotation) % NUM_EDGES);
    }

    static BigInt TransformPlacedEdgeToSourceEdge(BigInt edge, BigInt rotation, bool flipped)
    {
        return flipped ? ((NUM_EDGES - edge + rotation) % NUM_EDGES) : ((NUM_EDGES + edge - rotation) % NUM_EDGES);
    }

    char GetCharInPlacedTile(BigInt tileIndex, BigInt rotated, bool flipped, BigInt x, BigInt y) const
    {
        const Tile& tile = m_tileList[tileIndex];
        const BigInt tileEdgeSize = tile.data.size();

        TransformPlacedTileCoordToSourceCoord(tileEdgeSize, rotated, flipped, x, y);

        return tile.data[y][x];
    }

    static void TransformPlacedTileCoordToSourceCoord(BigInt tileEdgeSize, BigInt rotated, bool flipped, BigInt& x, BigInt& y)
    {
        BigInt newX = 0;
        BigInt newY = 0;
        switch (rotated)
        {
            case 0:
            default:
                newX = x;
                newY = y;
                break;
            case 1:
                newX = y;
                newY = tileEdgeSize - 1 - x;
                break;
            case 2:
                newX = tileEdgeSize - 1 - x;
                newY = tileEdgeSize - 1 - y;
                break;
            case 3:
                newX = tileEdgeSize - 1 - y;
                newY = x;
                break;
        }
        x = flipped ? (tileEdgeSize - 1 - newX) : newX;
        y = newY;
    }

    void PrintImage(const std::string& image) const
    {
        for (BigInt y = 0; y < m_imageEdgeSize; ++y)
        {
            for (BigInt x = 0; x < m_imageEdgeSize; ++x)
            {
                printf("%c", image[y * m_imageEdgeSize + x]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    typedef std::unordered_map<std::string, BigInt> EdgeMap;
    typedef BigIntListList EdgeTileList;
    EdgeMap m_edgeMap;
    EdgeTileList m_edgeTileList;

    std::string m_image;
    BigInt m_imageEdgeSize;
};

void RunJurassicJigsaw()
{
    static const char CHARS_TO_COUNT = '#';

    StringList seaMonster;
    ReadFileLines("Day20SeaMonster.txt", seaMonster);

    JurassicJigsaw testData("Day20TestInput.txt");
    printf("Test data corner tile id product = %lld\n", testData.CalcCornerTileIdProduct());
    testData.AssembleImage(true);
    testData.MarkOccurencesOfPatternAllowTransform(seaMonster, true);
    printf(
        "Test data count of %c chars after marking sea monster patterns = %lld\n",
        CHARS_TO_COUNT,
        testData.CountCharsInImage(CHARS_TO_COUNT));

    JurassicJigsaw mainData("Day20Input.txt");
    printf("Main data corner tile id product = %lld\n", mainData.CalcCornerTileIdProduct());
    mainData.AssembleImage(false);
    mainData.MarkOccurencesOfPatternAllowTransform(seaMonster, true);
    printf(
        "Main data count of %c chars after marking sea monster patterns = %lld\n",
        CHARS_TO_COUNT,
        mainData.CountCharsInImage(CHARS_TO_COUNT));
}



////////////////////////////
// Problem 21 - Allergen Assessment

class AllergenLab
{
public:
    AllergenLab(const char* fileName, bool verbose)
    {
        ReadFileLines(fileName, m_data);

        for (const auto& line: m_data)
        {
            m_foodList.push_back(Food());
            Food& food = m_foodList.back();

            StringList tokens;
            Tokenize(line, tokens, ' ');
            const BigInt numTokens = tokens.size();

            if (verbose)
                printf("Food:\n  Ingredients: ");

            BigInt tokenIndex = 0;
            while ((tokenIndex < numTokens) && (tokens[tokenIndex][0] != '('))
            {
                const std::string& ingredient = tokens[tokenIndex];
                food.ingredients.insert(ingredient);
                ++tokenIndex;

                m_ingredientSet.insert(ingredient);

                if (verbose)
                    printf("%s ", ingredient.c_str());
            }
            if (verbose)
                printf("\n  Allergens: ");

            if ((tokenIndex < numTokens) && (tokens[tokenIndex][0] == '('))
            {
                assert(tokens[tokenIndex] == "(contains");
                ++tokenIndex;

                while (tokenIndex < numTokens)
                {
                    const std::string allergen = tokens[tokenIndex].substr(0, tokens[tokenIndex].length() - 1);
                    food.allergens.insert(allergen);
                    ++tokenIndex;

                    StringSet& allergenIngredientSet =
                        m_allergenToIngredientsWorkingMap
                            .insert(AllergenToIngredientsWorkingMap::value_type(allergen, StringSet()))
                            .first->second;
                    if (allergenIngredientSet.empty())
                        allergenIngredientSet = food.ingredients;
                    else
                        IntersectSet(allergenIngredientSet, food.ingredients);

                    if (verbose)
                        printf("%s ", allergen.c_str());
                }
                if (verbose)
                    printf("\n");
            }
        }

        if (verbose)
        {
            printf("\nIngredients:\n");
            for (const auto& ingredient: m_ingredientSet)
            {
                printf("  %s\n", ingredient.c_str());
            }

            printf("Allergens:\n");
            for (const auto& allergenMapNode: m_allergenToIngredientsWorkingMap)
            {
                const auto& allergen = allergenMapNode.first;
                printf("  %s\n    ingredients with allergen: ", allergen.c_str());
                for (const auto& ingredient: allergenMapNode.second)
                {
                    printf("%s ", ingredient.c_str());
                }
                printf("\n");
            }
        }

        m_nonAllergenIngredientSet = m_ingredientSet;

        ReduceAllergenIngredients();

        if (verbose)
        {
            printf("\nNon-allergen ingredients:\n");
            for (const auto& ingredient: m_nonAllergenIngredientSet)
            {
                printf("  %s\n", ingredient.c_str());
            }

            printf("\nAllergen-ingredient pairings:\n");
            for (const auto& pairing: m_allergenToIngredientsMap)
            {
                printf("  %s - %s\n", pairing.first.c_str(), pairing.second.c_str());
            }
        }
    }

    BigInt CountHowManyTimesNonAlergenIngredientsAppearInFood() const
    {
        BigInt count = 0;
        for (const auto& ingredient: m_nonAllergenIngredientSet)
        {
            for (const auto& food: m_foodList)
            {
                count += food.ingredients.count(ingredient);
            }
        }
        return count;
    }

    std::string DetermineCanonicalDangerousIngredientList() const
    {
        std::string list;
        for (const auto& mapNode: m_allergenToIngredientsMap)
        {
            if (list.empty())
                list = mapNode.second;
            else
            {
                list += ",";
                list += mapNode.second;
            }
        }
        return list;
    }

private:
    struct Food
    {
        StringSet ingredients;
        StringSet allergens;

        Food() : ingredients(), allergens() {}
    };

    typedef std::vector<Food> FoodList;

    typedef std::map<std::string, StringSet> AllergenToIngredientsWorkingMap;
    typedef StringMap AllergenToIngredientsMap;

    void ReduceAllergenIngredients()
    {
        bool foundSomethingToReduce = false;
        do
        {
            foundSomethingToReduce = false;
            std::string ingredientToRemove;

            for (const auto& allergenMapNode: m_allergenToIngredientsWorkingMap)
            {
                if (allergenMapNode.second.size() == 1)
                {
                    const auto& ingredient = *allergenMapNode.second.begin();

                    // ingredient has an allergen, so remove it from the non-allergen ingredients

                    m_nonAllergenIngredientSet.erase(ingredient);

                    // add this allergen-ingredient assocation to the final map

                    m_allergenToIngredientsMap.insert(AllergenToIngredientsMap::value_type(allergenMapNode.first, ingredient));

                    // remove all other instances of ingredient

                    foundSomethingToReduce = true;
                    ingredientToRemove = ingredient;
                    break;
                }
            }

            if (foundSomethingToReduce)
            {
                assert(!ingredientToRemove.empty());

                for (auto& allergenMapNode: m_allergenToIngredientsWorkingMap)
                    allergenMapNode.second.erase(ingredientToRemove);
            }
        } while (foundSomethingToReduce);
    }

    StringList m_data;
    FoodList m_foodList;
    StringSet m_ingredientSet;
    StringSet m_nonAllergenIngredientSet;
    AllergenToIngredientsWorkingMap m_allergenToIngredientsWorkingMap;
    AllergenToIngredientsMap m_allergenToIngredientsMap;
};

void RunAllergenAssessment()
{
    AllergenLab testData("Day21TestInput.txt", true);
    printf(
        "In test data, number of times non-allergen ingredients appear in food = %lld\n",
        testData.CountHowManyTimesNonAlergenIngredientsAppearInFood());
    printf("Test data canonical dangerous ingredient list: %s\n", testData.DetermineCanonicalDangerousIngredientList().c_str());

    AllergenLab mainData("Day21Input.txt", false);
    printf(
        "In main data, number of times non-allergen ingredients appear in food = %lld\n",
        mainData.CountHowManyTimesNonAlergenIngredientsAppearInFood());
    printf("Main data canonical dangerous ingredient list: %s\n", mainData.DetermineCanonicalDangerousIngredientList().c_str());
}



////////////////////////////
// Problem 22 - Crab Combat

class CrabCombat
{
public:
    CrabCombat(const char* fileName) : m_nextGameNumber(1), m_thisGameNumber(0)
    {
        StringList fileLines;
        ReadFileLines(fileName, fileLines);

        BigInt lineIndex = 0;
        assert(fileLines[lineIndex] == "Player 1:");
        ++lineIndex;

        while ((lineIndex < (BigInt)fileLines.size()) && !fileLines[lineIndex].empty())
        {
            m_player1Deck.push_back(atoll(fileLines[lineIndex].c_str()));
            ++lineIndex;
        }

        assert(((lineIndex < (BigInt)fileLines.size()) && fileLines[lineIndex].empty()));
        ++lineIndex;

        assert(fileLines[lineIndex] == "Player 2:");
        ++lineIndex;

        while ((lineIndex < (BigInt)fileLines.size()) && !fileLines[lineIndex].empty())
        {
            m_player2Deck.push_back(atoll(fileLines[lineIndex].c_str()));
            ++lineIndex;
        }

        m_origPlayer1Deck = m_player1Deck;
        m_origPlayer2Deck = m_player2Deck;
    }

    void Reset()
    {
        m_nextGameNumber = 1;
        m_thisGameNumber = 0;
        m_player1Deck = m_origPlayer1Deck;
        m_player2Deck = m_origPlayer2Deck;
        m_roundSnapshotSet.Reset();
    }

    void PlayGame(BigInt& winningScore, bool verbose)
    {
        BigInt round = 1;
        if (verbose)
            printf("Playing game:\n\n");
        while (!m_player1Deck.empty() && !m_player2Deck.empty())
        {
            const BigInt player1Plays = m_player1Deck.front();
            const BigInt player2Plays = m_player2Deck.front();

            if (verbose)
            {
                printf("-- Round %lld --\n", round);
                printf("Player 1's deck: ");
                for (const BigInt card: m_player1Deck)
                    printf("%lld ", card);
                printf("\nPlayer 2's deck: ");
                for (const BigInt card: m_player2Deck)
                    printf("%lld ", card);
                printf("\nPlayer 1 plays: %lld\n", player1Plays);
                printf("Player 2 plays: %lld\n", player2Plays);
            }

            m_player1Deck.pop_front();
            m_player2Deck.pop_front();

            if (player1Plays > player2Plays)
            {
                m_player1Deck.push_back(player1Plays);
                m_player1Deck.push_back(player2Plays);

                if (verbose)
                    printf("Player 1 wins the round!\n\n");
            }
            else
            {
                assert(player2Plays > player1Plays);

                m_player2Deck.push_back(player2Plays);
                m_player2Deck.push_back(player1Plays);

                if (verbose)
                    printf("Player 2 wins the round!\n\n");
            }

            ++round;
        }

        if (verbose)
        {
            printf("== Post-game results ==\n");

            printf("Player 1's deck: ");
            for (const BigInt card: m_player1Deck)
                printf("%lld ", card);
            printf("\nPlayer 2's deck: ");
            for (const BigInt card: m_player2Deck)
                printf("%lld ", card);
            printf("\n\n");

            if (m_player1Deck.empty())
                printf("Player 2 wins the game!\n\n");
            else
            {
                assert(m_player2Deck.empty());
                printf("Player 1 wins the game!\n\n");
            }
        }

        winningScore = CalcWinningPlayerScore();
    }

    void PlayRecursiveGame(BigInt* pWinningPlayer, BigInt* pWinningScore, bool verbose)
    {
        m_thisGameNumber = m_nextGameNumber++;

        static GameSnapshot startGameSnapshot;
        CreateGameSnapshot(m_player1Deck, m_player2Deck, startGameSnapshot);

        m_roundSnapshotSet.Reset();

        BigInt round = 1;
        if (verbose)
            printf("=== Game %lld ===\n\n", m_thisGameNumber);

        while (!m_player1Deck.empty() && !m_player2Deck.empty())
        {
            static GameSnapshot gameSnapshot;
            CreateGameSnapshot(m_player1Deck, m_player2Deck, gameSnapshot);
            if (m_roundSnapshotSet.InsertQuery(gameSnapshot))
            {
                static BigInt count = 0;
                ++count;

                if (pWinningPlayer)
                    *pWinningPlayer = 1;
                if (pWinningScore)
                    *pWinningScore = 0;

                m_winnerMap.Insert(startGameSnapshot, 1);

                if (verbose)
                    printf(
                        "-- Round %lld (Game %lld) --\nRound is a repeater, so Player 1 wins the game!\n\n",
                        round,
                        m_thisGameNumber);

                return;
            }

            const BigInt player1Plays = m_player1Deck.front();
            const BigInt player2Plays = m_player2Deck.front();

            if (verbose)
            {
                printf("-- Round %lld (Game %lld) --\n", round, m_thisGameNumber);
                printf("Player 1's deck: ");
                for (const BigInt card: m_player1Deck)
                    printf("%lld ", card);
                printf("\nPlayer 2's deck: ");
                for (const BigInt card: m_player2Deck)
                    printf("%lld ", card);
                printf("\nPlayer 1 plays: %lld\n", player1Plays);
                printf("Player 2 plays: %lld\n", player2Plays);
            }

            m_player1Deck.pop_front();
            m_player2Deck.pop_front();

            if (((BigInt)m_player1Deck.size() >= player1Plays) && ((BigInt)m_player2Deck.size() >= player2Plays))
            {
                BigInt winningPlayer = 0;

                static GameSnapshot recursiveGameSnapshot;
                CreateGameSnapshot(m_player1Deck, m_player2Deck, recursiveGameSnapshot, player1Plays, player2Plays);
                if (m_winnerMap.Query(recursiveGameSnapshot, &winningPlayer))
                {
                    static BigInt count = 0;
                    ++count;

                    if (verbose)
                        printf("Recursive game was won before, with winner %lld\n\n", winningPlayer);
                }
                else
                {
                    m_pausedGameStack.push_back(PausedGame(m_thisGameNumber, m_player1Deck, m_player2Deck, m_roundSnapshotSet));

                    if (verbose)
                        printf("Playing a sub-game to determine the winner...\n\n");

                    PlayRecursiveGame(&winningPlayer, nullptr, verbose);

                    PausedGame& pausedGame = m_pausedGameStack.back();
                    m_thisGameNumber = pausedGame.gameNumber;
                    m_player1Deck = pausedGame.player1Deck;
                    m_player2Deck = pausedGame.player2Deck;
                    m_roundSnapshotSet.Swap(pausedGame.roundSnapshotSet);
                    m_pausedGameStack.pop_back();

                    if (verbose)
                        printf("...anyway, back to game %lld.\n", m_thisGameNumber);
                }

                if (winningPlayer == 1)
                {
                    m_player1Deck.push_back(player1Plays);
                    m_player1Deck.push_back(player2Plays);

                    if (verbose)
                        printf("Player 1 wins round %lld of game %lld!\n\n", round, m_thisGameNumber);
                }
                else
                {
                    m_player2Deck.push_back(player2Plays);
                    m_player2Deck.push_back(player1Plays);

                    if (verbose)
                        printf("Player 2 wins round %lld of game %lld!\n\n", round, m_thisGameNumber);
                }
            }
            else if (player1Plays > player2Plays)
            {
                m_player1Deck.push_back(player1Plays);
                m_player1Deck.push_back(player2Plays);

                if (verbose)
                    printf("Player 1 wins round %lld of game %lld!\n\n", round, m_thisGameNumber);
            }
            else
            {
                assert(player2Plays > player1Plays);

                m_player2Deck.push_back(player2Plays);
                m_player2Deck.push_back(player1Plays);

                if (verbose)
                    printf("Player 2 wins round %lld of game %lld!\n\n", round, m_thisGameNumber);
            }

            ++round;
        }

        if (verbose)
        {
            printf("== Post-game results ==\n");

            printf("Player 1's deck: ");
            for (const BigInt card: m_player1Deck)
                printf("%lld ", card);
            printf("\nPlayer 2's deck: ");
            for (const BigInt card: m_player2Deck)
                printf("%lld ", card);
            printf("\n\n");

            if (m_player1Deck.empty())
                printf("Player 2 wins the game!\n\n");
            else
            {
                assert(m_player2Deck.empty());
                printf("Player 1 wins the game!\n\n");
            }
        }

        const BigInt winningPlayer = m_player2Deck.empty() ? 1 : 2;
        if (pWinningPlayer)
            *pWinningPlayer = winningPlayer;
        if (pWinningScore)
            *pWinningScore = CalcWinningScore(m_player2Deck.empty() ? m_player1Deck : m_player2Deck);
        m_winnerMap.Insert(startGameSnapshot, winningPlayer);
    }

    BigInt CalcWinningPlayerScore() const
    {
        assert(m_player1Deck.empty() || m_player2Deck.empty());

        const CrabDeck& winningPlayerDeck = m_player2Deck.empty() ? m_player1Deck : m_player2Deck;
        return CalcWinningScore(winningPlayerDeck);
    }

private:
    typedef CircularBuffer<BigInt, 6> CrabDeck;

    typedef std::string GameSnapshot;

    class RoundSnapshotSet
    {
    public:
        RoundSnapshotSet() : m_set() {}

        void Reset() { m_set.clear(); }
        void Swap(RoundSnapshotSet& rhs) { m_set.swap(rhs.m_set); }

        // attempt to insert in set, and return whether snapshot was already in the set originally
        bool InsertQuery(const GameSnapshot& snapshot) { return !m_set.insert(snapshot).second; }


    private:
        UnorderedStringSet m_set;
    };

    class GameWinnerMap
    {
    public:
        GameWinnerMap() : m_map() {}

        bool Query(const GameSnapshot& snapshot, BigInt* pWinner = nullptr) const
        {
            const auto findWinnerIter = m_map.find(snapshot);
            if (findWinnerIter != m_map.end())
            {
                if (pWinner)
                    *pWinner = findWinnerIter->second;
                return true;
            }
            else
            {
                return false;
            }
        }

        void Insert(const GameSnapshot& snapshot, BigInt winner) { m_map.insert(WinnerMap::value_type(snapshot, winner)); }

    private:
        typedef std::unordered_map<GameSnapshot, BigInt> WinnerMap;
        WinnerMap m_map;
    };

    struct PausedGame
    {
        BigInt gameNumber;
        CrabDeck player1Deck;
        CrabDeck player2Deck;
        RoundSnapshotSet roundSnapshotSet;

        PausedGame(BigInt gn, const CrabDeck& p1D, const CrabDeck& p2D, const RoundSnapshotSet& rsss)
            : gameNumber(gn), player1Deck(p1D), player2Deck(p2D), roundSnapshotSet(rsss)
        {
        }
    };

    typedef std::vector<PausedGame> PausedGameStack;

    static void CreateGameSnapshot(
        const CrabDeck& deck1, const CrabDeck& deck2, GameSnapshot& snapshot, BigInt numFromDeck1 = -1, BigInt numFromDeck2 = -1)
    {
        const BigInt BASE_CHAR = 32;

        snapshot.clear();
        for (const BigInt card: deck1)
        {
            snapshot += (char)(uint8_t)(BigUInt)(BASE_CHAR + card);

            if (numFromDeck1 > 0)
            {
                --numFromDeck1;
                if (numFromDeck1 <= 0)
                    break;
            }
        }
        snapshot += (char)(uint8_t)(BASE_CHAR);
        for (const BigInt card: deck2)
        {
            snapshot += (char)(uint8_t)(BigUInt)(BASE_CHAR + card);

            if (numFromDeck2 > 0)
            {
                --numFromDeck2;
                if (numFromDeck2 <= 0)
                    break;
            }
        }
    }

    static BigInt CalcWinningScore(const CrabDeck& winningPlayerDeck)
    {
        BigInt score = 0;
        BigInt index = (BigInt)winningPlayerDeck.size();

        for (const BigInt card: winningPlayerDeck)
        {
            score += index * card;
            --index;
        }

        assert(index == 0);

        return score;
    }

    BigInt m_nextGameNumber;

    BigInt m_thisGameNumber;
    CrabDeck m_player1Deck;
    CrabDeck m_player2Deck;
    RoundSnapshotSet m_roundSnapshotSet;

    PausedGameStack m_pausedGameStack;

    CrabDeck m_origPlayer1Deck;
    CrabDeck m_origPlayer2Deck;

    GameWinnerMap m_winnerMap;
};

void RunCrabCombat()
{
    BigInt winningScore = 0;

    CrabCombat testData("Day22TestInput.txt");
    testData.PlayGame(winningScore, true);
    printf("Test data, winning player score = %lld\n", winningScore);
    testData.Reset();
    testData.PlayRecursiveGame(nullptr, &winningScore, true);
    printf("Test data, winning player score for recursive game = %lld\n", winningScore);

    CrabCombat mainData("Day22Input.txt");
    mainData.PlayGame(winningScore, false);
    printf("Main data, winning player score = %lld\n", winningScore);
    mainData.Reset();
    mainData.PlayRecursiveGame(nullptr, &winningScore, false);
    printf("Main data, winning player score for recursive game = %lld\n", winningScore);
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
        case 7:
            RunHandyHaversacks();
            break;
        case 8:
            RunHandheldHalting();
            break;
        case 9:
            RunEncodingError();
            break;
        case 10:
            RunAdapterArray();
            break;
        case 11:
            RunSeatingSystem();
            break;
        case 12:
            RunRainRisk();
            break;
        case 13:
            RunShuttleSearch();
            break;
        case 14:
            RunDockingData();
            break;
        case 15:
            RunRambunctiousRecitation();
            break;
        case 16:
            RunTicketTranslation();
            break;
        case 17:
            RunConwayCubes();
            break;
        case 18:
            RunOperationOrder();
            break;
        case 19:
            RunMonsterMessages();
            break;
        case 20:
            RunJurassicJigsaw();
            break;
        case 21:
            RunAllergenAssessment();
            break;
        case 22:
            RunCrabCombat();
            break;
        default:
            printf("'%s' is not a valid problem number!\n\n", problemArg);
            break;
    }

    return 0;
}
