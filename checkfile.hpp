#ifndef CHECKFILE_H
#define CHECKFILE_H

#include "readfile.hpp"
#include "comparison.hpp"
#include "sendemail.hpp"

#include <vector>
#include <fstream>

class CheckFile
{
private:
    std::ifstream* myFileFStream;
    std::string filePathPlusFileName;
    struct stat buff;

    double timeWindowHalfWidth;//Полуширина временного окна в секундах

    int currentFilePosition;
    int lastFilePosition;

    int currentFileSize;
    int previousFileSize;

    int numberOfLines;

    int numberOfCounterparts;

    int TJD;
    double SOD;
    double ra;
    double dec;
    double error;
    std::string setupName;
    std::string SKYMAP_FITS_URL;
    
    std::string fitsProcessingProgram;
    std::string command;

    int comparisonResult;//1 - совпадение по времени, 2 - совпадение по времени и координатам

    std::ifstream fileWithPositionRead;
    std::ofstream fileWithPositionWrite;

    std::vector<int>* lastPositions;
    int lastPositionsMaxSize;

    std::vector <CheckFile*> filesForComparison;

    std::string myPath;
    std::string myFileName;
    std::string myFilePositions;

    Readfile* readMyFile;
    Comparison* makeComparison;

    bool isThereAttachment;

public:
    CheckFile(std::string filePath, std::string fileName, std::string filePositions, double timeWindowHW);

    std::vector<int>* getLastPositions();
    std::string getSetupName();
    std::string getSkymapURL();

    void addFileForComparison(CheckFile* fileToCompare);
    void makeCheck(bool email, SendEmail* sendMessage);

    void openFile();
    void closeFile();

    void getValues(int position);

    int getTJD();
    double getSOD();
    double getRA();
    double getDec();
    double getError();

    std::string getFilePathAndName();

    ~CheckFile();

};

#endif