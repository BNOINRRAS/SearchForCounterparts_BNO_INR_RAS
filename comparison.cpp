#include "comparison.hpp"
#include "starcoordinates.hpp"

#include <math.h>
#include <cstdlib>
#include <algorithm>//Для функции "std::replace(...)"

//const double M_PI = 3.1415926535897932;//Для Windows

Comparison::Comparison(double timeWindowHW) : timeWindowHalfWidth(timeWindowHW)
{
	
}

double Comparison::getTimeDifference(int TJD1, double secondsOfDay1, int TJD2, double secondsOfDay2)
{
	if (TJD1 < TJD2)
	{
		if (secondsOfDay1 <= secondsOfDay2)
		{
			timeDifference = 86400 + abs(secondsOfDay1 - secondsOfDay2);
		}
		else
		{
			timeDifference = 86400 - abs(secondsOfDay1 - secondsOfDay2);
		}
		timeDifference += double(abs(TJD1 - TJD2) - 1) * 86400.;
	}
	else if (TJD1 > TJD2)
	{
		if (secondsOfDay2 <= secondsOfDay1)
		{
			timeDifference = 86400 + abs(secondsOfDay2 - secondsOfDay1);
		}
		else
		{
			timeDifference = 86400 - abs(secondsOfDay1 - secondsOfDay2);
		}
		timeDifference += double(abs(TJD1 - TJD2) - 1) * 86400.;
	}
	else if (TJD1 == TJD2)
	{
		timeDifference = abs(secondsOfDay1 - secondsOfDay2);
	}
	return timeDifference;
}

double Comparison::getAngleDifference(double raPoint1, double decPoint1, double raPoint2, double decPoint2)
{
	angleBetweenEvents = acos(sin(decPoint1 / (180. / M_PI)) * sin(decPoint2 / (180. / M_PI)) +
						cos(decPoint1 / (180. / M_PI)) * cos(decPoint2 / (180. / M_PI)) *
						cos((raPoint1 - raPoint2) / (180. / M_PI))) * (180. / M_PI);
	return angleBetweenEvents;
}

int Comparison::checkForCounterpart(int firstEventTJD, double firstEventSOD, double firstEventRA, double firstEventDec,
	double firstEventError, std::string firstEventSetupName,
	int secondEventTJD, double secondEventSOD, double secondEventRA, double secondEventDec,
	double secondEventError, std::string secondEventSetupName)
{
	int returnValue = 0;

	if (getTimeDifference(firstEventTJD, firstEventSOD, secondEventTJD, secondEventSOD) < timeWindowHalfWidth)
	{
		//Сообщение о том что есть совпадение по времени
		fileTimeCoincidences.open("reportTimeCoincidences.txt", std::ios_base::app);
		fileTimeCoincidences << "Coincidence in time!" << std::endl
			<< "Time difference:\t"
			<< getTimeDifference(firstEventTJD, firstEventSOD, secondEventTJD, secondEventSOD) << std::endl
			<< "\tFirst\t" << "Second" << std::endl
			<< "Name: " << firstEventSetupName << "\t" << secondEventSetupName << std::endl
			<< "TJD: " << firstEventTJD << "\t" << secondEventTJD << std::endl
			<< "Seconds: " << firstEventSOD << "\t" << secondEventSOD << std::endl << std::endl;
		fileTimeCoincidences.close();
		returnValue = 1;

		if (getAngleDifference(firstEventRA, firstEventDec, secondEventRA, secondEventDec)
			< firstEventError + secondEventError)
		{
			//Сообщение о том что есть совпадение по локализации
			fileCounterparts.open("reportCounterparts.txt", std::ios_base::app);
			fileCounterparts << "Counterpart found!" << std::endl
				<< "Time difference:\t"
				<< getTimeDifference(firstEventTJD, firstEventSOD, secondEventTJD, secondEventSOD) << std::endl
				<< "Angle difference:\t"
				<< getAngleDifference(firstEventRA, firstEventDec, secondEventRA, secondEventDec) << std::endl
				<< "\tFirst\t" << "Second" << std::endl
				<< "Name: " << firstEventSetupName << "\t" << secondEventSetupName << std::endl
				<< "TJD: " << firstEventTJD << "\t" << secondEventTJD << std::endl
				<< "Seconds: " << firstEventSOD << "\t" << secondEventSOD << std::endl
				<< "RA: " << firstEventRA << "\t" << secondEventRA << std::endl
				<< "Dec: " << firstEventDec << "\t" << secondEventDec << std::endl
				<< "Error: " << firstEventError << "\t" << secondEventError << std::endl << std::endl;
			fileCounterparts.close();
			returnValue = 2;
		}
	}
	return returnValue;
}

//Перегрузка для обработки ГВ события
int Comparison::checkForCounterpart(int firstEventTJD, double firstEventSOD, double firstEventRA, double firstEventDec,
		double firstEventError, std::string firstEventSetupName,
		int GWEventTJD, double GWEventSOD, std::string SKYMAP_FITS_URL)
{
	int returnValue = 0;

	if (getTimeDifference(firstEventTJD, firstEventSOD, GWEventTJD, GWEventSOD) < timeWindowHalfWidth)
	{
		//Сообщение о том что есть совпадение по времени
		fileTimeCoincidences.open("reportTimeCoincidences.txt", std::ios_base::app);
		fileTimeCoincidences << "Coincidence in time!" << std::endl
			<< "Time difference:\t"
			<< getTimeDifference(firstEventTJD, firstEventSOD, GWEventTJD, GWEventSOD) << std::endl
			<< "\tFirst\t" << "Second" << std::endl
			<< "Name: " << firstEventSetupName << "\t" << "Gravitational Wave" << std::endl
			<< "TJD: " << firstEventTJD << "\t" << GWEventTJD << std::endl
			<< "Seconds: " << firstEventSOD << "\t" << GWEventSOD << std::endl << std::endl;
		fileTimeCoincidences.close();
		returnValue = 1;

		//Для проверки того что точка внутри контура локализации используем метод "бросания прямых"
		modified_SKYMAP_FITS_URL = SKYMAP_FITS_URL;
		std::replace(modified_SKYMAP_FITS_URL.begin(), modified_SKYMAP_FITS_URL.end(), '/', '.');
		fileGWLocalizationContours.open("GW_Localization_Contours_and_skymaps/" + modified_SKYMAP_FITS_URL + ".txt");
		fileGWLocalizationContours >> firstWordTrash >> numberOfContours;//Записываем первое слово в "command" чтобы не создавать новую переменную
		for (int i = 0; i < numberOfContours; i++)
		{
			numberOfIntersections = 0;
			fileGWLocalizationContours >> firstWordTrash >> numberOfContourPoints;
			for (int j = 0; j < numberOfContourPoints; j++)
			{
				if (j == 0)
				{
					fileGWLocalizationContours >> edgeX1 >> edgeY1 >> edgeX2 >> edgeY2;
					firstPointX = edgeX1;
					firstPointY = edgeY1;
				}
				else if (j > 0 && j < numberOfContourPoints - 1)
				{
					edgeX1 = edgeX2;
					edgeY1 = edgeY2;
					fileGWLocalizationContours >> edgeX2 >> edgeY2;
				}
				else if (j == numberOfContourPoints - 1)
				{
					edgeX1 = edgeX2;
					edgeY1 = edgeY2;
					edgeX2 = firstPointX;
					edgeY2 = firstPointY;
				}
				if (getAngleDifference(firstEventRA, firstEventDec, edgeX1, edgeY1) < firstEventError)// Проверяем близко ли координаты к контуру
				{
					returnValue = 2;
					return returnValue;
				}
				else if (firstEventDec <= edgeY1 && firstEventDec > edgeY2// Считаем пересечения
					|| firstEventDec >= edgeY1 && firstEventDec < edgeY2)
				{
					if (firstEventRA < edgeX1 + ( ( edgeX2 - edgeX1 )
					    * ( ( firstEventDec - edgeY1 ) / ( edgeY2 - edgeY1 ) ) ) )
					{
						numberOfIntersections++;
					}
				}
			}
			if (numberOfIntersections % 2 != 0)
			{
				returnValue = 2;
				break;
			}
		}
	}
	angleBetweenEvents = 0.0;
	return returnValue;
}

double Comparison::getTimeBetweenEvents()
{
	return timeDifference;
}

double Comparison::getAngleBetweenEvents()
{
	return angleBetweenEvents;
}

Comparison::~Comparison()
{

}