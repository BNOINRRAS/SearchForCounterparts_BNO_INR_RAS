#include "readfile.hpp"

#include <iostream>
#include <math.h>

Readfile::Readfile(std::string fName, std::ifstream* myFileFStream, std::vector<int>* ptrLastPositions)
	: fileName(fName), readMyFile(myFileFStream), lastPositions(ptrLastPositions)
{
	transformCoordinates = new StarCoordinates();
}

void Readfile::readGCNFile()
{
	getline(*readMyFile, stringCurrentLine, '-');
	getline(*readMyFile, stringCurrentLine, '-');
	getline(*readMyFile, stringCurrentLine, '.');
	getline(*readMyFile, stringCurrentLine, ':');
	getline(*readMyFile, stringCurrentLine, ':');
	*readMyFile >> unusedValue >> unusedValue
		>> TJD >> secondsOfDay >> ra >> dec >> error;
	getline(*readMyFile, setupName);
	setupName.erase(0, 1);//Удаляем считанный пробел в начале
	//Проверка
	//std::cout << TJD << "\t" << secondsOfDay << "\t"
	//	<< ra << "\t" << dec << "\t" << error << "\t" << setupName << std::endl;

	if (setupName == "INTEGRAL-Wakeup-packet"
		|| setupName == "INTEGRAL-Refined-packet"
		|| setupName == "INTEGRAL-Offline-packet")
	{
		ra -= transformCoordinates->getAscensionNutationAndPrecession(TJD, ra, dec);//"-=" чтобы вернуться из current в J2000
		dec -= transformCoordinates->getDeclinationNutationAndPrecession(TJD, ra, dec);
	}
	else if (setupName[0] == 'L')
	{
		if (setupName[1] == 'V')
		{
			if (setupName[2] == 'C')
			{
				stringstreamCurrentLine.clear();
				stringstreamCurrentLine.str(setupName);
				getline(stringstreamCurrentLine, setupName, ' ');
				getline(stringstreamCurrentLine, SKYMAP_FITS_URL);
				//std::cout << SKYMAP_FITS_URL << "\n" << setupName << std::endl;//Проверка
			}
		}
	}
}

void Readfile::readGammaRayFile()
{
	getline(*readMyFile, stringCurrentLine, '/');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> day;
	getline(*readMyFile, stringCurrentLine, '/');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> month;
	getline(*readMyFile, stringCurrentLine, ':');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> year >> hour;
	getline(*readMyFile, stringCurrentLine, ':');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> minute;
	getline(*readMyFile, stringCurrentLine, 'T');//"T" - избавляемся от букв "UT"
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> second;
	*readMyFile >> ra >> dec >> unusedValue;
	error = 4.7;// Ошибка определения координаты гамма-кванта на Ковре 4.7 градуса.
	secondsOfDay = hour * 3600. + minute * 60. + second;
	year = 2000 + year;
	TJD = truncatedJulianDateCalculation(day, month, year);
	setupName = "Gamma-ray on Carpet";
	//Проверка
	//std::cout << day << "\t" << month << "\t" << year << "\t" << hour
	//	<< "\t" << minute << "\t" << second << "\t" << ra << "\t" << dec << std::endl;
}

void Readfile::readNeutrinoFile()
{
	getline(*readMyFile, stringCurrentLine, '.');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> day;
	getline(*readMyFile, stringCurrentLine, '.');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> month;
	getline(*readMyFile, stringCurrentLine, ':');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> year >> hour;
	getline(*readMyFile, stringCurrentLine, ':');
	stringstreamCurrentLine.clear();
	stringstreamCurrentLine.str(stringCurrentLine);
	stringstreamCurrentLine >> minute;
	*readMyFile >> second >> dec >> ra >> unusedValue >> unusedValue;

	error = 5.0;//В качестве ошибки определения координаты нейтрино на БПСТ выбрали 5 градусов
	secondsOfDay = hour * 3600. + minute * 60. + second;
	TJD = truncatedJulianDateCalculation(day, month, year);
	setupName = "Neutrino on BUST";
	//Проверка
	//std::cout << day << "\t" << month << "\t" << year << "\t" << hour << "\t"
	//	<< minute << "\t" << second << "\t" << dec << "\t" << ra << "\t" << std::endl;
}

void Readfile::readOurFile()
{
	if (fileName == "GCN.log")
	{
		readGCNFile();
	}
	else if (fileName == "CARPET24.app")
	{
		readGammaRayFile();
	}
	else if (fileName == "Neu_BNO_2024.txt" || fileName == "Neu_BNO_2024_manual.txt")
	{
		readNeutrinoFile();
	}
}

bool Readfile::checkForNewEvents()
{
	readMyFile->clear();

	readMyFile->seekg(*(lastPositions->end()-1), std::ios::beg);

	//std::cout << readMyFile->tellg() << std::endl;
	readOurFile();
	//std::cout << TJD << "\t" << secondsOfDay << "\t" << ra << "\t" << dec << std::endl;

	//std::cout << readMyFile->tellg() << std::endl;

	if (readMyFile->tellg() != -1)
	{
		lastPositions->push_back(readMyFile->tellg());
		return true;
	}
	else
	{
		return false;
	}
}

int Readfile::getTJDInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return TJD;
}

double Readfile::getSODInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return secondsOfDay;
}

double Readfile::getRAInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return ra;
}

double Readfile::getDecInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return dec;
}

double Readfile::getErrorInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return error;
}

std::string Readfile::getSetupNameInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return setupName;
}

std::string Readfile::getSkymapURLInPosition(int position)
{
	readMyFile->clear();
	readMyFile->seekg(position, std::ios::beg);
	readOurFile();
	return SKYMAP_FITS_URL;
}

int Readfile::truncatedJulianDateCalculation(int ourDay, int ourMonth, int ourYear)
{
	//ВЫЧИСЛЕНИЕ ОКРУГЛЕННОЙ ЮЛИАНСКОЙ ДАТЫ
	int Var1 = floor((14. - (double)ourMonth) / 12.);
	int Var2 = (double)ourYear + 4800. - Var1;
	int Var3 = (double)ourMonth + (12. * Var1) - 3.;
	return (double)ourDay
			+ floor(((153. * Var3) + 2.) / 5.)
			+ (365. * Var2)
			+ floor(Var2 / 4.)
			- floor(Var2 / 100.)
			+ floor(Var2 / 400.)
			- 32045.
			- 2440000.5;//Найдено на Википедии, стр. "Юлианская дата"
}

Readfile::~Readfile()
{

}