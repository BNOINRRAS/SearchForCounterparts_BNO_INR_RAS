#ifndef READFILE_H
#define READFILE_H

#include "starcoordinates.hpp"

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>

class Readfile
{
private:
    std::string fileName;
    std::ifstream* readMyFile;
    std::vector<int>* lastPositions;

    int currentFileSize, currentFilePosition, currentFileLine;
    //int previousFileSize;
    int TJD, day, month, year, hour, minute;
    double second, secondsOfDay;
    double ra, dec, error;
    std::string setupName;
    std::string SKYMAP_FITS_URL;
    double unusedValue;

    std::stringstream stringstreamCurrentLine;
    std::string header, stringDate, stringTimeUT;
    std::string stringCurrentLine;
    char charCurrentLine;

    StarCoordinates* transformCoordinates;

    void readOurFile();

    void readGCNFile();
    void readGammaRayFile();
    void readNeutrinoFile();

    int truncatedJulianDateCalculation(int ourDay, int ourMonth, int ourYear);

public:
    Readfile(std::string fName, std::ifstream* myFileFStream, std::vector<int>* ptrLastPositions);

    bool checkForNewEvents();

    int getTJDInPosition(int position);
    double getSODInPosition(int position);
    double getRAInPosition(int position);
    double getDecInPosition(int position);
    double getErrorInPosition(int position);
    std::string getSetupNameInPosition(int position);
    std::string getSkymapURLInPosition(int position);

    ~Readfile();
};

#endif
