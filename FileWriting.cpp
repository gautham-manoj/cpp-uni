#include <fstream>
#include <math.h>
#include <limits>

using namespace std;

#ifndef MARMOSET_TESTING
#include <iostream>
#endif


#ifndef MARMOSET_TESTING
struct Student
{
    int studentID;
    int grade;
};

struct Dataset
{
    int numStudents;
    Student* students;
};

struct Rejects
{
    int numRejects;
    int* rejects;
};

struct Mode
{
    int numModes;
    int* modes;
};

struct Statistics
{
    int minimum;
    float average;
    int maximum;
    float popStdDev;
    float smplStdDev;
    Mode mode;
    int histogram[10];
};
#endif


int readCSV(const char filename[], const int minAcceptableID, const int maxAcceptableID,
            Dataset& data, Rejects& rejects);
int computeStatistics(Dataset& data, Statistics& stats);
int writeCSV(const char filename[], const Statistics& stats);

const int CAPACITY = 1000;
bool TOOMANYREJECTS = false;

bool stringToInt(const char input[], int& value, int& i, int maxLineLength)
{
    value = 0;
    bool positive = true;
    if (input[i] == 43)
        i++;
    else if (input[i] == 45)
    {
        positive = false;
        i++;
    }
    while (i < maxLineLength && input[i] > 47 && input[i] < 58)
    {
        if ((2147483647 - (input[i] - 48)) / 10 < value)
            return false;
        value *= 10;
        value += input[i] - 48;
        i++;
    }
    if (!positive)
        value *= -1;
    return true;
}

bool skipSpaces(const char line[], int& i, int maxLineLength)
{
    while (i < maxLineLength && (line[i] < 48 || line[i] > 57) && line[i] != 43 && line[i] != 45)
    {
        if (line[i] < 33)
            i++;
        else
            return false;
    }
    return true;
}

bool parseLine(char line[], int& studentID, int& grade, int maxLineLength)
{
    int i = 0;
    if (skipSpaces(line, i, maxLineLength))
    {
        if (i == maxLineLength)
            return false;
        if (stringToInt(line, studentID, i, maxLineLength))
        {
            while (i < maxLineLength && line[i] != 44)
            {
                if (line[i] < 33)
                    i++;
                else
                {
                    studentID = -1;
                    grade = -1;
                    return true;
                }
            }
            if (i == maxLineLength)
            {
                studentID = -1;
                grade = -1;
                return true;
            }
            i++;
            if (skipSpaces(line, i, maxLineLength))
            {
                if (i == maxLineLength)
                {
                    studentID = -1;
                    grade = -1;
                    return true;
                }
                if (stringToInt(line, grade, i, maxLineLength))
                    return true;
            }
        }
    }
    studentID = -1;
    grade = -1;
    return true;
}


int readCSV(const char filename[], const int minAcceptableID, const int maxAcceptableID,
            Dataset& data, Rejects& rejects)
{
    if (minAcceptableID > maxAcceptableID || filename == NULL)
        return -1;
    int filenameSize = 0;
    bool tempModified = false;
    while (filename[filenameSize] != '\0')
    {
        filenameSize++;
    }
    if (filenameSize > 4)
    {
        if (filename[filenameSize - 4] != '.' || filename[filenameSize - 3] != 'c'
            || filename[filenameSize - 2] != 's' || filename[filenameSize - 1] != 'v')
            tempModified = true;
    }
    else
        tempModified = true;
    char tempFileName[filenameSize + 5];
    if (tempModified)
    {
        for (int i = 0; i < filenameSize; i++)
        {
            tempFileName[i] = filename[i];
        }
        tempFileName[filenameSize] = '.';
        tempFileName[filenameSize + 1] = 'c';
        tempFileName[filenameSize + 2] = 's';
        tempFileName[filenameSize + 3] = 'v';
        tempFileName[filenameSize + 4] = '\0';
    }
    const int maxLineLength = 100;
    char line[maxLineLength];
    ifstream infile;
    if (tempModified)
        infile.open(tempFileName);
    else
        infile.open(filename);
    if (!infile.is_open())
        return -1;
    bool done = false;
    int fileLineNumber = 0;
    int numberOfStudents = 0;
    int numberOfRejects = 0;
    Student* dStudents = new Student[CAPACITY];
    int* dRejects = new int[CAPACITY];
    while (!done)
    {
        ++fileLineNumber;
        if (!infile.getline(line, maxLineLength))
        {
            if (infile.eof())
                done = true;
            else
                return -1;
        }
        else
        {
            int studentID;
            int grade;
            if (parseLine(line, studentID, grade, maxLineLength))
            {
                if (numberOfRejects > CAPACITY || numberOfStudents > CAPACITY)
                    return -1;
                else
                {
                    if (studentID >= minAcceptableID && studentID <= maxAcceptableID && grade >= 0
                        && grade <= 100)
                    {
                        dStudents[numberOfStudents].studentID = studentID;
                        dStudents[numberOfStudents].grade = grade;
                        numberOfStudents++;
                    }
                    else
                    {
                        dRejects[numberOfRejects] = fileLineNumber;
                        numberOfRejects++;
                    }
                }
            }
        }
    }
    infile.close();
    data.numStudents = numberOfStudents;
    if (numberOfStudents > 0)
    {
        data.students = new Student[numberOfStudents];
        for (int i = 0; i < numberOfStudents; i++)
        {
            data.students[i] = dStudents[i];
        }
    }
    delete[] dStudents;
    dStudents = NULL;
    rejects.numRejects = numberOfRejects;
    
    if (numberOfRejects > 0)
    {
        if (numberOfRejects > 10)
            TOOMANYREJECTS = true;
        rejects.rejects = new int[numberOfRejects];
        for (int i = 0; i < numberOfRejects; i++)
        {
            rejects.rejects[i] = dRejects[i];
        }
    }
    delete[] dRejects;
    dRejects = NULL;
    if (TOOMANYREJECTS)
        return -1;
    return numberOfRejects;
}

int minimum(const int dataset[], const int size)
{
    if (size > 0)
    {
        int tmpmin = dataset[0];
        for (int i = 1; i < size; i++)
        {
            if (dataset[i] < tmpmin)
                tmpmin = dataset[i];
        }
        return tmpmin;
    }
    return -1;
}


float average(const int dataset[], const int size)
{
    if (size > 0)
    {
        float tmpsum = 0;
        for (int i = 0; i < size; i++)
        {
            tmpsum += dataset[i];
        }
        float tmpavg = tmpsum / size;
        return tmpavg;
    }
    return std::numeric_limits<float>::quiet_NaN();
}


int maximum(const int dataset[], const int size)
{
    if (size > 0)
    {
        int tmpmax = dataset[0];
        for (int i = 1; i < size; i++)
        {
            if (dataset[i] > tmpmax)
                tmpmax = dataset[i];
        }
        return tmpmax;
    }
    return -1;
}


float popStdDev(const int dataset[], const int size)
{
    if (size > 0)
    {
        float tmpsumdif = 0;
        float tmpavg = average(dataset, size);
        for (int i = 0; i < size; i++)
        {
            tmpsumdif += pow(((float)dataset[i] - tmpavg), 2);
        }
        float tmppsd = sqrt(tmpsumdif / size);
        return tmppsd;
    }
    return std::numeric_limits<float>::quiet_NaN();
}


float smplStdDev(const int dataset[], const int size)
{
    if (size > 1)
    {
        float tmpsumdif = 0;
        float tmpavg = average(dataset, size);
        for (int i = 0; i < size; i++)
        {
            tmpsumdif += pow(((float)dataset[i] - tmpavg), 2);
        }
        float tmpssd = sqrt(tmpsumdif / (size - 1));
        return tmpssd;
    }
    return std::numeric_limits<float>::quiet_NaN();
}


int qsort(int start, int end, int arr[])
{
    int pivot = arr[end];
    int split = start;
    for (int i = start; i < end; i++)
    {
        if (arr[i] <= pivot)
        {
            int tmp = arr[split];
            arr[split] = arr[i];
            arr[i] = tmp;
            split++;
        }
    }
    int tmp = arr[split];
    arr[split] = arr[end];
    arr[end] = tmp;
    return split;
}


void quicksort(int start, int end, int arr[])
{
    if (start < end)
    {
        int split = qsort(start, end, arr);
        quicksort(start, split - 1, arr);
        quicksort(split, end, arr);
    }
    else
        return;
}


bool sort(int arr[], const int size)
{
    if (size > 0)
    {
        quicksort(0, size - 1, arr);
        return true;
    }
    return false;
}


int mode(const int dataset[], const int size, int*& mode)
{
    int arr[size];
    for (int i = 0; i < size; i++)
    {
        arr[i] = dataset[i];
    }
    if (sort(arr, size))
    {
        int number = arr[0];
        int count = 1;
        int* dMode = new int[CAPACITY];
        dMode[0] = arr[0];
        int modeCount = 1;
        int numberOfModes = 1;
        for (int i = 1; i < size; i++)
        {
            if (arr[i] == number)
                count++;
            else
            {
                number = arr[i];
                count = 1;
            }
            if (count > modeCount)
            {
                dMode[0] = arr[i];
                numberOfModes = 1;
                modeCount++;
            }
            else if (count == modeCount)
            {
                dMode[numberOfModes] = arr[i];
                numberOfModes++;
            }
        }
        if (numberOfModes > 0)
        {
            mode = new int[numberOfModes];
            for (int i = 0; i < numberOfModes; i++)
            {
                mode[i] = dMode[i];
            }
        }
        delete[] dMode;
        dMode = NULL;
        return numberOfModes;
    }
    return -1;
}

bool histogram(const int dataset[], const int size, int histogram[])
{
    if (histogram != NULL)
    {
        for (int i = 0; i < 10; i++)
        {
            histogram[i] = 0;
        }
        if (size > 0)
        {
            for (int i = 0; i < size; i++)
            {
                int bracket = dataset[i] / 10;
                if (bracket >= 0 && bracket < 10)
                    histogram[bracket]++;
                else
                    histogram[9]++;
            }
            return true;
        }
    }
    return false;
}


int computeStatistics(Dataset& data, Statistics& stats)
{
    int numGrades = data.numStudents;
    int grades[numGrades];
    for (int i = 0; i < numGrades; i++)
    {
        grades[i] = data.students[i].grade;
    }
    stats.minimum = minimum(grades, numGrades);
    stats.average = average(grades, numGrades);
    stats.maximum = maximum(grades, numGrades);
    stats.popStdDev = popStdDev(grades, numGrades);
    stats.smplStdDev = smplStdDev(grades, numGrades);
    stats.mode.numModes = mode(grades, numGrades, stats.mode.modes);
    int warningCounter = 0;
    if (stats.minimum < 0)
        warningCounter++;
    if (stats.average != stats.average)
        warningCounter++;
    if (stats.maximum < 0)
        warningCounter++;
    if (stats.popStdDev != stats.popStdDev)
        warningCounter++;
    if (stats.smplStdDev != stats.smplStdDev)
        warningCounter++;
    if (stats.mode.numModes < 1)
        warningCounter++;
    if (!histogram(grades, numGrades, stats.histogram))
        warningCounter++;
    if (warningCounter > 6 || TOOMANYREJECTS)
        return -1;
    return warningCounter;
}

int writeCSV(const char filename[], const Statistics& stats)
{
    int warningCounter = 0;
    if (stats.minimum < 0)
        warningCounter++;
    if (stats.average != stats.average)
        warningCounter++;
    if (stats.maximum < 0)
        warningCounter++;
    if (stats.popStdDev != stats.popStdDev)
        warningCounter++;
    if (stats.smplStdDev != stats.smplStdDev)
        warningCounter++;
    if (stats.mode.numModes < 0)
        warningCounter++;
    if (warningCounter > 5 || TOOMANYREJECTS || filename == NULL)
        return -1;
    int filenameSize = 0;
    bool CSV = false;
    while (filename[filenameSize] != '\0')
    {
        filenameSize++;
    }
    if (filenameSize > 4)
    {
        if (filename[filenameSize - 4] == '.' && filename[filenameSize - 3] == 'c'
            && filename[filenameSize - 2] == 's' && filename[filenameSize - 1] == 'v')
            CSV = true;
    }
    char tempFileNameCSV[filenameSize + 2];
    char tempFileNameNoCSV[filenameSize + 6];
    if (CSV)
    {
        for (int i = 0; i < filenameSize - 4; i++)
        {
            tempFileNameCSV[i] = filename[i];
        }
        tempFileNameCSV[filenameSize - 4] = '.';
        tempFileNameCSV[filenameSize - 3] = 's';
        tempFileNameCSV[filenameSize - 2] = 't';
        tempFileNameCSV[filenameSize - 1] = 'a';
        tempFileNameCSV[filenameSize] = 't';
        tempFileNameCSV[filenameSize + 1] = '\0';
    }
    else
    {
        for (int i = 0; i < filenameSize; i++)
        {
            tempFileNameNoCSV[i] = filename[i];
        }
        tempFileNameNoCSV[filenameSize] = '.';
        tempFileNameNoCSV[filenameSize + 1] = 's';
        tempFileNameNoCSV[filenameSize + 2] = 't';
        tempFileNameNoCSV[filenameSize + 3] = 'a';
        tempFileNameNoCSV[filenameSize + 4] = 't';
        tempFileNameNoCSV[filenameSize + 5] = '\0';
    }
    ofstream outfile;
    if (CSV)
        outfile.open(tempFileNameCSV);
    else
        outfile.open(tempFileNameNoCSV);
    if (!outfile.is_open())
        return -1;
    outfile << "Minimum: " << stats.minimum << '\n';
    outfile << "Average: " << stats.average << '\n';
    outfile << "Maximum: " << stats.maximum << '\n';
    outfile << "Population Standard Deviation: " << stats.popStdDev << '\n';
    outfile << "Sample Standard Deviation: " << stats.smplStdDev << '\n';
    if (stats.mode.numModes > 0)
    {
        outfile << "Modes: " << stats.mode.modes[0];
        for (int i = 1; i < stats.mode.numModes; i++)
        {
            outfile << ", " << stats.mode.modes[i];
        }
        outfile << '\n';
    }
    outfile << "Histogram: " << '\n';
    for (int i = 0; i < 10; i++)
    {
        outfile << "[" << (i * 10) << "-" << ((((i + 1) * 10) - 1) + i / 9)
        << "]: " << stats.histogram[i] << '\n';
    }
    outfile.close();
    return warningCounter;
}


#ifndef MARMOSET_TESTING

#define isNaN(X) (X != X) // NaN is the only float that is not equal to itself

int main(const int argc, const char* const argv[])
{
    
    // // Some test driver code here ....
    int minAcceptableID = 22200000;
    int maxAcceptableID = 22299999;
    
    Dataset data = { 0 };
    Rejects rejects = { 0 };
    Statistics stats = { 0 };
    
    
    if (readCSV(argv[1], minAcceptableID, maxAcceptableID, data, rejects) < 0)
    {
        std::cout << ".csv file could not be read" << std::endl;
    }
    if (computeStatistics(data, stats) < 0)
    {
        std::cout << "Stats could not be computed" << std::endl;
    }
    
    if (writeCSV(argv[1], stats) < 0)
    {
        std::cout << ".stat file could not be written" << std::endl;
    }
    //
    // std::cout << "Average: " << stats.average << std::endl;
    // std::cout << "Minimum: " << stats.minimum << std::endl;
    // std::cout << "Maximum: " << stats.maximum << std::endl;
    // std::cout << "Population standard deviation: " << stats.popStdDev << std::endl;
    // std::cout << "Sample standard deviation: " << stats.smplStdDev << std::endl;
    //
    //
    // for (unsigned char i = 0; i < stats.mode.numModes; i++)
    // {
    //     std::cout << "Mode: " << stats.mode.modes[i] << std::endl;
    // }
    //
    // for (unsigned char i = 0; i < 10; i++)
    // {
    //     std::cout << "Histogram bin [" << (i*10) << ", " << ((((i+1)*10) - 1) + i/9) << "]: " <<
    // stats.histogram[i] << std::endl;
    // }
    
    return 0;
}

#endif

