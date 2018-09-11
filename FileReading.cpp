
#include <fstream>
using namespace std;
//////////////////////////////////////////////////////////////
//
// #includes and function declarations; do not remove
//

#ifndef MARMOSET_TESTING
#include <iostream>
#endif

//////////////////////////////////////////////////////////////
//
// Function declarations; do not remove
// Replace "..." in bisectionHelper() with any parameter(s)
// you need to ensure termination when precision and
// minIntervalSize are too tightly specified.
//

int histogram(const char filename[], int histogram[10], 
              const int minAcceptableID, const int maxAcceptableID,
              int*& rejects);

//////////////////////////////////////////////////////////////
//
// Your code here ...
//  
// Return -1 if you cannot compute the histogram for any
// reason;
// Return 0 if the histogram is correct and there are no
// rejections;
// Return the number of rejects if there are rejected
// records from the file
//
bool charCheck(const char line[], const int i)
{
    if ((line[i] > 32 && line[i] < 43) || (line[i] > 45 && line[i] < 48) || line[i] > 57)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool parseLine(const char line[], int& studentID, int& grade)
{
    int i = 0;
    // Initial blank space
    while ((line[i] < 48 || line[i] > 57) && line[i] != 43 && line[i] != 45)
    {
        i++;
        if (!charCheck(line, i))
            return false;
    }
    // Student ID
    studentID = 0;
    int sign = 1;
    while (line[i] > 32 && line[i] != 44)
    {
        if (!charCheck(line, i))
            return false;
        
        if (line[i] == 43)
            sign = 1;
        else if (line[i] == 45)
            sign = -1;
        else
            studentID = studentID * 10 + (line[i] - '0');
        i++;
    }
    studentID *= sign;
    // Character (comma, space) between student ID and grade
    while ((line[i] < 48 || line[i] > 57) && line[i] != 43 && line[i] != 45)
    {
        i++;
        if (!charCheck(line, i))
            return false;
    }
    // Grade
    grade = 0;
    sign = 1;
    while (line[i] > 32)
    {
        if (!charCheck(line, i))
            return false;
        if (line[i] == 43)
            sign = 1;
        else if (line[i] == 45)
            sign = -1;
        else
            grade = grade * 10 + (line[i] - '0');
        i++;
    }
    grade *= sign;
    return true;
}

int histogram(const char filename[], int histogram[10], 
              const int minAcceptableID, const int maxAcceptableID,
              int*& rejects)
{
    if (minAcceptableID > maxAcceptableID || histogram == NULL)
        return -1;
    int numRejects = 1;
    rejects = new int[numRejects];
    for (int i = 0; i < 10; i++)
    {
        histogram[i] = 0;
    }
    const int maxLineLength = 100;
    char line[maxLineLength];
    ifstream infile;            // declaring object
    infile.open(filename);      // open file
    if (!infile.is_open())
        return -1;              // file opening error
    bool done = false;
    int fileLineNumber = 0;
    while (!done)               // Read from file
    {
        ++fileLineNumber;
        if (!infile.getline(line, maxLineLength))     // Next line
        {
            if (infile.eof())                         // End of file
                done = true;
            else
                return -1;                            // File reading error
        }
        
        if (!done)
        {
            int studentID;
            int grade;
            bool valid = parseLine(line, studentID, grade);
            if (!valid)
                return -1;
            // validity of student IDs
            if (studentID < minAcceptableID || studentID > maxAcceptableID || grade < 0 || grade > 100)
            {
                int* rejectedEntries = new int[numRejects];
                for (int i = 0; i < numRejects; i++)
                {
                    rejectedEntries[i] = rejects[i];
                }
                rejectedEntries[numRejects - 1] = fileLineNumber;
                rejects = rejectedEntries;
                numRejects++;
            }
            else
            {
                if (grade >= 0 && grade <= 9)
                    histogram[0]++;
                else if (grade >= 10 && grade <= 19)
                    histogram[1]++;
                else if (grade >= 20 && grade <= 29)
                    histogram[2]++;
                else if (grade >= 30 && grade <= 39)
                    histogram[3]++;
                else if (grade >= 40 && grade <= 49)
                    histogram[4]++;
                else if (grade >= 50 && grade <= 59)
                    histogram[5]++;
                else if (grade >= 60 && grade <= 69)
                    histogram[6]++;
                else if (grade >= 70 && grade <= 79)
                    histogram[7]++;
                else if (grade >= 80 && grade <= 89)
                    histogram[8]++;
                else
                    histogram[9]++;
            }
        }
        
    }
    if (numRejects != 0)
        return numRejects - 1;
    return 0;
}

//////////////////////////////////////////////////////////////
//
// Test Driver
//
// Do not remove the #ifndef and its associated #endif
// This is required so that when you submit your code,
// this test driver will be ignored by Marmoset
//

#ifndef MARMOSET_TESTING

#define isNaN(X) (X != X)  // NaN is the only float that is not equal to itself

int main(const int argc, const char* const argv[]) {
  
  // Some test driver code here ....
  if (argc < 2) {

  }
  const int numBuckets = 10;
  const int bucketRange = 100/numBuckets;
  int  buckets[numBuckets];
  int* rejectedRecords;
  int retCode = histogram(argv[1], buckets, 22200000, 22299999, rejectedRecords);
  if (retCode < 0) {
      cout << "Unable to compute histogram." << endl;
  }
  else {
    for (int i = 0; i < numBuckets; ++i)
      if (i != numBuckets-1)
	cout << "Number [" << i*bucketRange << "," << (((i+1)*bucketRange)-1) << "]: " << buckets[i] << endl;
      else
	cout << "Number [" << i*bucketRange << "," << (((i+1)*bucketRange)) << "]: " << buckets[i] << endl;
    if (retCode > 0) {
      cout << "Number of rejected records: " << retCode << endl;
      cout << "Rejected records are:";
      for (int i = 0; i < retCode; ++i)
	cout << " " << rejectedRecords[i];
      cout << endl;
      delete[] rejectedRecords;
    }
  }

  return 0;
}

#endif
