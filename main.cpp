#include "checkfile.hpp"
#include "starcoordinates.hpp"
#include "sendemail.hpp"

#include <math.h>
#include <iostream>
#include <fstream>
#include <unistd.h>//Для Linux
//#include <windows.h>//Для Windows
#include <stdlib.h>
#include <vector>

int main(int argc, char* argv[])
{
    double timeWindowHalfWidth;
    char charEmail;
    char charClose = 'n';
    //По умолчанию ширина временного окна плюс/минус 500 секунд
    //По умолчанию письма по почте не отправляются
    if (argc == 1)
    {
        timeWindowHalfWidth = 500.;
        charEmail = 'n';
        std::cout << "Time window set to (plus/minus) " << timeWindowHalfWidth << " seconds" << std::endl;
    }
    else if (argc == 2)
    {
        timeWindowHalfWidth = strtod(argv[1], NULL);
        if (timeWindowHalfWidth == 0)
        {
            std::cout << "Enter the correct value for the time window!" << std::endl;
            exit(0);
        }
        charEmail = 'n';
        std::cout << "Time window set to (plus/minus) " << timeWindowHalfWidth << " seconds" << std::endl;
    }
    else if (argc == 3)
    {
        timeWindowHalfWidth = strtod(argv[1], NULL);
        if (timeWindowHalfWidth == 0)
        {
            std::cout << "Enter the correct value for the time window!" << std::endl;
            exit(0);
        }
        if (*argv[2] == 'n' || *argv[2] == 'y')
        {
            charEmail = *argv[2];
        }
        else
        {
            std::cout << "Wrong second argument! Type \'n' or \'y'!" << std::endl;
            exit(0);
        }
        std::cout << "Time window set to (plus/minus) " << timeWindowHalfWidth << " seconds" << std::endl;
    }
    else
    {
        std::cout << "Wrong number of arguments!" << std::endl;
        exit(0);
    }

    std::ofstream writeFileControl;
    writeFileControl.open("emailandclose");
    writeFileControl << "Send_email\t" << charEmail << std::endl
        << "Close_the_program\t" << 'n';
    writeFileControl.close();

    std::string fileWithAlertsPath;
    std::string fileWithGammaRaysPath;
    std::string fileWithNeutrinosPath;
    std::string fileWithNeutrinosManualPath;

    std::string fileWithAlerts;
    std::string fileWithGammaRays;
    std::string fileWithNeutrinos;
    std::string fileWithNeutrinosManual;

    std::string fileWithAlertPositions;
    std::string fileWithGammaRayPositions;
    std::string fileWithNeutrinoPositions;
    std::string fileWithNeutrinoManualPositions;

    fileWithAlerts = "GCN.log";
    fileWithGammaRays = "CARPET24.app";
    fileWithNeutrinos = "Neu_BNO_2024.txt";
    fileWithNeutrinosManual = "Neu_BNO_2024_manual.txt";//Manual - файл в который добавляются нейтрино после ручной детальной обработки

    fileWithAlertPositions = "GCNFilePositions.dat";
    fileWithGammaRayPositions = "GammaRayFilePositions.dat";
    fileWithNeutrinoPositions = "NeutrinoFilePositions.dat";
    fileWithNeutrinoManualPositions = "NeutrinoManualFilePositions.dat";

    fileWithAlertsPath = "/home/islam/calculation/GCN_Kafka/";//"/home/dirhttp/html/BNO/Doc/gcn_grb/"
    fileWithGammaRaysPath = "/home/islam/Desktop/NTerskol_data/Carpet/";//"/home/dirhttp/html/BNO/Doc/Carpet/"
    fileWithNeutrinosPath = "/home/islam/Desktop/NTerskol_data/Neutrino/";//"/home/dirhttp/html/BNO/Doc/Neutrino/"
    fileWithNeutrinosManualPath = "/home/islam/Desktop/NTerskol_data/Neutrino/";//"/home/dirhttp/html/BNO/Doc/Neutrino/"

    int secondCounter = 0;
    int dayCounter = 0;

	std::ifstream readfileControl;
	bool email;

    int timeToWaitLinux = 5;
    int timeToWaitWindows = 5000;

    CheckFile* checkAlert;
    CheckFile* checkGammaRay;
    CheckFile* checkNeutrino;
    CheckFile* checkNeutrinoManual;

    checkAlert = new CheckFile(fileWithAlertsPath, fileWithAlerts, fileWithAlertPositions, timeWindowHalfWidth);
    checkGammaRay = new CheckFile(fileWithGammaRaysPath, fileWithGammaRays, fileWithGammaRayPositions, timeWindowHalfWidth);
    checkNeutrino = new CheckFile(fileWithNeutrinosPath, fileWithNeutrinos, fileWithNeutrinoPositions, timeWindowHalfWidth);
    checkNeutrinoManual = new CheckFile(fileWithNeutrinosManualPath, fileWithNeutrinosManual,
                                        fileWithNeutrinoManualPositions, timeWindowHalfWidth);

    checkAlert->addFileForComparison(checkNeutrino);
    checkAlert->addFileForComparison(checkGammaRay);

    checkNeutrino->addFileForComparison(checkAlert);

    checkGammaRay->addFileForComparison(checkAlert);

    checkNeutrinoManual->addFileForComparison(checkAlert);

    SendEmail* sendMessage;
    sendMessage = new SendEmail();//Создадим 1 объект SendEmail для всей программы

    std::string readString;//String для пропуска ненужной информации
    while(1)
    {
        readfileControl.open("emailandclose");
        readfileControl >> readString >> charEmail >> readString >> charClose;
        if(charEmail == 'n')
        {
            email = false;
        }
        else if (charEmail == 'y')
        {
            email = true;
        }
        if (charClose == 'y')
        {
            readfileControl.close();
            std::cout << "Program closed" << std::endl;
            exit(0);
        }
        readfileControl.close();

        checkAlert->makeCheck(email, sendMessage);//GCN-файл должен быть первым!
    	sleep(timeToWaitLinux);//Для Linux
        //Sleep(timeToWaitWindows);//Для Windows

        checkGammaRay->makeCheck(email, sendMessage);
    	sleep(timeToWaitLinux);//Для Linux
        //Sleep(timeToWaitWindows);//Для Windows

        checkNeutrino->makeCheck(email, sendMessage);
        checkNeutrinoManual->makeCheck(email, sendMessage);
        sleep(timeToWaitLinux);//Для Linux
        //Sleep(timeToWaitWindows);//Для Windows

        secondCounter += timeToWaitLinux * 3;
    	if(secondCounter % 86400 == 0)
    	{
    		dayCounter++;
            std::cout << "This program has been working for " << dayCounter << " days" << std::endl;
    	}
    }

    return 0;
}
