#ifndef COMPARISON_H
#define COMPARISON_H

#include <iostream>
#include <fstream>

class Comparison
{
private:
	double timeDifference;
	double angleBetweenEvents;

	double timeWindowHalfWidth;//Полуширина временного окна в секундах

	std::ofstream fileCounterparts;
	std::ofstream fileTimeCoincidences;

	std::ifstream fileGWLocalizationContours;
	int numberOfContours;
	int numberOfContourPoints;
	int firstPointX;//Первая точка нужна чтобы обработать последнюю грань контура, так как он замкнутый
	int firstPointY;
	double edgeX1;
	double edgeY1;
	double edgeX2;
	double edgeY2;
	int numberOfIntersections;

	std::string modified_SKYMAP_FITS_URL;//Строка SKYMAP_FITS_URL, в которой символ '/' заменен на '.'
	std::string firstWordTrash;// Ненужные слова в файле записываются сюда

	double getTimeDifference(int TJD1, double secondsOfDay1, int TJD2, double secondsOfDay2);
	double getAngleDifference(double raPoint1, double decPoint1, double raPoint2, double decPoint2);

public:
    Comparison(double timeWindowHW);

    //checkForCounterpart: 1 - совпадение по времени, 2 - совпадение по времени и координатам
	int checkForCounterpart(int firstEventTJD, double firstEventSOD, double firstEventRA, double firstEventDec,
		double firstEventError, std::string firstEventSetupName,
		int secondEventTJD, double secondEventSOD, double secondEventRA, double secondEventDec,
		double secondEventError, std::string secondEventSetupName);
	//Перегрузка для сравнения с пятном ГВ события
	int checkForCounterpart(int firstEventTJD, double firstEventSOD, double firstEventRA, double firstEventDec,
		double firstEventError, std::string firstEventSetupName,
		int GWEventTJD, double GWEventSOD, std::string SKYMAP_FITS_URL);

	double getTimeBetweenEvents();
	double getAngleBetweenEvents();

	~Comparison();
};

#endif
