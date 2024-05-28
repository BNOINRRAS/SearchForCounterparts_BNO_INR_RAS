#include "checkfile.hpp"

#include <iostream>
//#include <stdlib.h>
#include <cstdlib>

CheckFile::CheckFile(std::string filePath, std::string fileName, std::string filePositions, double timeWindowHW) :
			myPath(filePath), myFileName(fileName), myFilePositions(filePositions), timeWindowHalfWidth(timeWindowHW)
{
	currentFilePosition = 0;
	lastFilePosition = 0;
	numberOfLines = 0;
	numberOfCounterparts = 0;
	currentFileSize = 0;
	previousFileSize = 0;

	SOD = 0;
	TJD = 0;
	ra = 0;
	dec = 0;
	error = 0;

	filePathPlusFileName = myPath + myFileName;

	myFileFStream = new std::ifstream;

	lastPositions = new std::vector<int>;
	lastPositionsMaxSize = 30;
	if (fileName == "Neu_BNO_2024_manual.txt")//В файл ручной (детальной) обработки - события добавляются позже
	{//поэтому алерты ищутся в большем отрезке времени
		lastPositionsMaxSize = 300;
	}
	readMyFile = new Readfile(myFileName, myFileFStream, lastPositions);
	fileWithPositionRead.open(myFilePositions.c_str());
	if(!fileWithPositionRead)
	{
		fileWithPositionRead.close();
		lastPositions->push_back(0);
	}
	else
	{
		while (fileWithPositionRead >> numberOfLines >> lastFilePosition)
		{
			lastPositions->push_back(lastFilePosition);
		}
	}
	std::cout << "Waiting for new events in file " << myFileName
				<< " (starting from event number " << numberOfLines << ")" << std::endl;
	makeComparison = new Comparison(timeWindowHalfWidth);

	fitsProcessingProgram = "python3 Fits_processing.py https://gracedb.ligo.org/api/superevents/";
}

void CheckFile::makeCheck(bool email, SendEmail* sendMessage)
{
	numberOfCounterparts = 0;
	isThereAttachment = false;

	myFileFStream->open(filePathPlusFileName.c_str());
	if ( !( *myFileFStream ) )
	{
		std::cout << "File " << myFileName << " not found!" << std::endl;
		exit(0);
	}
	myFileFStream->clear();
	stat(filePathPlusFileName.c_str(), &buff);
	currentFileSize = buff.st_size;

	if (currentFileSize > previousFileSize)
	{
		while (readMyFile->checkForNewEvents())
		{
			numberOfLines++;
			getValues(*(lastPositions->end() - 2));//Начало предыдущей строки (после считывания)
			if (setupName == "LVC_INITIAL" || setupName == "LVC_UPDATE" || setupName == "LVC_PRELIMINARY")
			{
				isThereAttachment = true;
				command = fitsProcessingProgram + SKYMAP_FITS_URL + " " + SKYMAP_FITS_URL;
				std::system(command.c_str());//Создается файл с координатами контуров локализации
			}
			//Отправляем письмо о новом событии
			if (email == true)
			{
				if(isThereAttachment == true)
				{
					sendMessage->define_SKYMAP_FITS_URL(SKYMAP_FITS_URL);//Обязательно нужно передать URL-адрес!
				}
				sendMessage->formAndSendMessage(setupName, TJD, SOD, ra, dec, error, isThereAttachment);
				//std::cout << "Message new event - sent" << std::endl;
			}
			//std::cout << "New event in " << myFileName << "! (Event number " << numberOfLines << ")" << std::endl;
			//std::cout << lastPositions->size() << std::endl;
			std::vector <CheckFile*> :: iterator filesIter;
			std::vector<int> :: iterator lastPositionsIter;
			for (filesIter = filesForComparison.begin();
				filesIter < filesForComparison.end();
				filesIter++)
			{
				(*filesIter)->openFile();
				for (lastPositionsIter = (*filesIter)->getLastPositions()->begin();
					lastPositionsIter < ( (*filesIter)->getLastPositions()->end() - 1 );
					lastPositionsIter++)
				{
					(*filesIter)->getValues(*lastPositionsIter);
					//std::cout << (*filesIter)->getRA() << "\t" << *lastPositionsIter << std::endl;
					if (setupName == "LVC_INITIAL" || setupName == "LVC_UPDATE" || setupName == "LVC_PRELIMINARY")
					{
						comparisonResult = makeComparison->checkForCounterpart( (*filesIter)->getTJD(),
																		(*filesIter)->getSOD(),
																		(*filesIter)->getRA(),
																		(*filesIter)->getDec(),
																		(*filesIter)->getError(),
																		(*filesIter)->getSetupName(),
																		TJD, SOD, SKYMAP_FITS_URL);
					}
					else if((*filesIter)->getSetupName() == "LVC_INITIAL"
						|| (*filesIter)->getSetupName() == "LVC_UPDATE"
						|| (*filesIter)->getSetupName() == "LVC_PRELIMINARY")
					{
						comparisonResult = makeComparison->checkForCounterpart(TJD, SOD, ra, dec,
																		error, setupName,
																		(*filesIter)->getTJD(),
																		(*filesIter)->getSOD(),
																		(*filesIter)->getSkymapURL());
						isThereAttachment = true;//Второе событие - ГВ
					}
					else
					{
						comparisonResult = makeComparison->checkForCounterpart(TJD, SOD, ra, dec,
																		error, setupName,
																		(*filesIter)->getTJD(),
																		(*filesIter)->getSOD(),
																		(*filesIter)->getRA(),
																		(*filesIter)->getDec(),
																		(*filesIter)->getError(),
																		(*filesIter)->getSetupName());
					}
					
					if (comparisonResult == 1)
					{
						//Отправляем письмо о совпадении по времени
						//if (email == true)
						//{
						//	sendMessage->formAndSendMessage(setupName, TJD, SOD, ra, dec,
						//								(*filesIter)->getSetupName(),
						//								(*filesIter)->getTJD(),
						//								(*filesIter)->getSOD(),
						//								(*filesIter)->getRA(),
						//								(*filesIter)->getDec(),
						//								(*filesIter)->getError(),
						//								makeComparison->getTimeBetweenEvents(),
						//								makeComparison->getAngleBetweenEvents(),
						//								false, isThereAttachment);
							//std::cout << "Message time coinc - sent" << std::endl;
						//}
					}
					else if (comparisonResult == 2)
					{
						numberOfCounterparts++;
						//Отправляем письмо о совпадении по времени и координатам
						if (email == true)
						{
							
							if(isThereAttachment == true)
							{
								sendMessage->define_SKYMAP_FITS_URL(SKYMAP_FITS_URL);
							}
							sendMessage->formAndSendMessage(setupName, TJD, SOD, ra, dec, error,
														(*filesIter)->getSetupName(),
														(*filesIter)->getTJD(),
														(*filesIter)->getSOD(),
														(*filesIter)->getRA(),
														(*filesIter)->getDec(),
														(*filesIter)->getError(),
														makeComparison->getTimeBetweenEvents(),
														makeComparison->getAngleBetweenEvents(),
														true, isThereAttachment);
							//std::cout << "Message time-coord coinc - sent" << std::endl;
						}
					}
				}
				(*filesIter)->closeFile();
			}
			fileWithPositionWrite.open(myFilePositions.c_str());// , std::ios_base::app);
			fileWithPositionWrite << numberOfLines << "\t"
								<< *(lastPositions->end() - 1) << "\n";
			fileWithPositionWrite.close();

			//Ограничим размер вектора "std::vector<int>* lastPositions"
			if ( lastPositions->size() > lastPositionsMaxSize )
			{
				lastPositions->erase(lastPositions->begin());
			}
		}
	}
	else if (currentFileSize < previousFileSize)
	{
		lastPositions->clear();
		lastPositions->push_back(0);
		while (readMyFile->checkForNewEvents()) { ;} //Просто читаем и заново заполняем lastPositions
	}
	previousFileSize = currentFileSize;
	myFileFStream->close();
}

void CheckFile::addFileForComparison(CheckFile* fileToCompare)
{
	filesForComparison.push_back(fileToCompare);
}

std::vector<int>* CheckFile::getLastPositions()
{
	return lastPositions;
}

void CheckFile::getValues(int position)
{
	TJD = readMyFile->getTJDInPosition(position);
	SOD = readMyFile->getSODInPosition(position);
	ra = readMyFile->getRAInPosition(position);
	dec = readMyFile->getDecInPosition(position);
	error = readMyFile->getErrorInPosition(position);
	setupName = readMyFile->getSetupNameInPosition(position);
	SKYMAP_FITS_URL = readMyFile->getSkymapURLInPosition(position);
}

int CheckFile::getTJD()
{
	return TJD;
}

double CheckFile::getSOD()
{
	return SOD;
}

double CheckFile::getRA()
{
	return ra;
}

double CheckFile::getDec()
{
	return dec;
}
double CheckFile::getError()
{
	return error;
}

std::string CheckFile::getSetupName()
{
	return setupName;
}

std::string CheckFile::getSkymapURL()
{
	return SKYMAP_FITS_URL;
}

std::string CheckFile::getFilePathAndName()
{
	return filePathPlusFileName;
}

void CheckFile::openFile()
{
	myFileFStream->open(filePathPlusFileName.c_str());
	if ( !( *myFileFStream ) )
	{
		std::cout << "File " << myFileName << " not found!" << std::endl;
		exit(0);
	}
}

void CheckFile::closeFile()
{
	myFileFStream->close();
}

CheckFile::~CheckFile()
{

}