#ifndef STARCOORDINATES_H
#define STARCOORDINATES_H

class StarCoordinates
{
private:
    int TJD;
    double SOD;
    double Ascension;
    double Declination;

	double AzimuthalAngle360;//Азимутальный угол от 0 до 360
    double ZenithAngle;//Зенитный угол
    double altitude;//Высота
    double AzimuthalAngle;//Азимутальный угол от -180 до 180
    double AzimuthalAngleBUST;//Азимутальный угол в системе ПСТ
    double AscensionNutation;//Нутация по Пр. восх.
    double DeclinationNutation;//Нутация по склонению
    double AscensionPrecession;//Прецессия по Пр. восх.
    double DeclinationPrecession;//Прецессия по склонению
    double HourAngle;//Часовой угол
    double Latitude;//Широта места наблюдения
    double Longitude;//Долгота места наблюдения
    double GMSTdeg;//GMST в градусах

    double AscensionCalculation;//Прямое восхождение с поправкой на прецессию и нутацию
    double DeclinationCalculation;//Склонение с поправкой на прецессию и нутацию

    double GMST;//Гринвичское среднее звёздное время
    double d;//Интервал времени от эпохи T0 до эпохи T в средних солнечных сутках
    double M;//Всемирное время UT1 момента события, выраженное в долях суток
    double t;//Интервал времени от эпохи T0 до эпохи T в юлианских столетиях по 36525 средних солнечных суток
    int JD;//Юлианская дата 
    double V1, V2, V3, V4, V5;//Суммируемые члены в формуле вычисления Гринвичского Среднего Звездного Времени

    double deg, min, sec;//Параметры для вычисления наклона эклиптики к экватору
    double NutationInLongitude;//Нутация в долготе
    double NutationInObliquity;//Нутация в наклоне
    double MeanObliquityOfTheEcliptic;//Средний наклон эклиптики к экватору
    double TrueObliquityOfTheEcliptic;//Истинный наклон эклиптики к экватору

    double X2000, Y2000, Z2000;//X, Y, Z в прямоугольной системе координат в момент J2000
    double Xcurrent, Ycurrent, Zcurrent;//X, Y, Z в прямоугольной системе координат в момент события
    
    double RAWithNutation;//Прямое восхождение с поправкой на нутацию
    double DecWithNutation;//Склонение с поправкой на нутацию
    double l;//Средняя аномалия Луны
    double l1;//Средняя аномалия Солнца
    double F;//Средний аргумент широты Луны
    double D;//Разность средних долгот Луны и Солнца
    double OM;//Средняя долгота восходящего узла орбиты Луны на эклиптике
    double n11, n12, n13, n21, n22, n23, n31, n32, n33;//Члены матрицы нутации

    double RAWithPrecession;//Прямое восхождение с поправкой на прецессию
    double DecWithPrecession;//Склонение с поправкой на прецессию
    double coeff1, coeff2, coeff3;//Коэффициенты в матрице прецессии - ζ, Z, θ
    double p11, p12, p13, p21, p22, p23, p31, p32, p33;//Члены матрицы прецессии

    void getNutation(int argTJD);
    void getPrecession(int argTJD);

public:
    StarCoordinates();
    
    void convertToBUSTCoordinates(int argTJD, double argSOD, double argAscension, double argDeclination);

    double getAscensionNutationAndPrecession(int argTJD, double argAscension, double argDeclination);
    double getDeclinationNutationAndPrecession(int argTJD, double argAscension, double argDeclination);
    
    ~StarCoordinates();
};

#endif
