#include "starcoordinates.hpp"

#include <math.h>
#include <iostream>

//const double M_PI = 3.1415926535897932;//Для Windows

StarCoordinates::StarCoordinates()
{
    TJD = 0;
    SOD = 0;
    Ascension = 0;
    Declination = 0;
}

void StarCoordinates::convertToBUSTCoordinates(int argTJD, double argSOD, double argAscension, double argDeclination)
{
    //RA и Dec в этом методе задаются в градусах
    TJD = argTJD;
    SOD = argSOD;
    Ascension = argAscension;
    Declination = argDeclination;
    
    if (Ascension == 360.)
    {
        Ascension = 0.;
    }

    //ВЫЧИСЛЕНИЕ ЮЛИАНСКОЙ ДАТЫ
    JD = TJD + 2440000.5;

    //ВЫЧИСЛЕНИЕ GMST В ВЫБРАННЫЙ МОМЕНТ ВРЕМЕНИ
    d = JD - 0.5 - 2451545.0;//J2000//Вычли 0.5, потому что в 00:00 UT1 наступает половина Юлианского дня
    t = d / 36525.;
    V1 = 6. * 3600. + 41. * 60. + 50.54841;
    V2 = 8640184.812866 * t;
    V3 = 0.093104 * t * t;
    V4 = -0.0000062 * t * t * t;

    V5 = SOD * 1.00273790935;//Время с поправкой для перехода к Гринвичскому Времени
    GMST = V1 + V2 + V3 + V4;//Получили GMST в 00:00 UT1 на Гринвичском меридиане
    GMST = GMST + V5;//Получили GMST на выбранный момент времени
    while (GMST > 86400.)
    {
        GMST = GMST - 86400.;
    }
    while (GMST <= 0)
    {
        GMST = GMST + 86400.;
    }
    GMST = GMST / 3600.;//Перевели в часы
    GMSTdeg = GMST * 15.;//Перевели часы в градусы
    //ЗАДАЕТСЯ ПРЯМОЕ ВОСХОЖДЕНИЕ
    Ascension = Ascension / (180. / M_PI);
    //ЗАДАЕТСЯ СКЛОНЕНИЕ
    Declination = Declination / (180. / M_PI);
    Latitude = 43.271438;//Примерное положение ПСТ
    Latitude = Latitude / (180. / M_PI);
    Longitude = 42.695409;//Примерное положение ПСТ 42 41 43.5
    Longitude = Longitude / (180. / M_PI);

    getNutation(TJD);//AscensionNutation и DeclinationNutation получают правильные значения относительно J2000
    getPrecession(TJD);//AscensionPrecession и DeclinationPrecession получают правильные значения относительно J2000
    GMSTdeg = GMSTdeg + NutationInLongitude * (180. / M_PI) * cos(TrueObliquityOfTheEcliptic);//Добавили поправку за нутацию для GMST. Расчет окончен - получили GTST (Greenwich True Sidereal Time)
    if (GMSTdeg > 360.)
    {
        GMSTdeg = GMSTdeg - 360.;
    }
    if (GMSTdeg < 0)
    {
        GMSTdeg = GMSTdeg + 360.;
    }

    AscensionCalculation = Ascension + AscensionPrecession + AscensionNutation;//Добавили поправки за прецессию и нутацию
    DeclinationCalculation = Declination + DeclinationPrecession + DeclinationNutation;//Добавили поправки за прецессию и нутацию
    if (AscensionCalculation < 0)
    {
        AscensionCalculation = AscensionCalculation + (2. * M_PI);
    }
    if (AscensionCalculation > 2. * M_PI)
    {
        AscensionCalculation = AscensionCalculation - (2. * M_PI);
    }

    //Последняя стадия расчета координат - получаем зенитный и азимутальный угол
    HourAngle = (GMSTdeg / (180. / M_PI)) + Longitude - AscensionCalculation;

    ZenithAngle = acos(sin(DeclinationCalculation) * sin(Latitude) + cos(DeclinationCalculation) * cos(Latitude) * cos(HourAngle));

    AzimuthalAngle = atan((sin(HourAngle)) * cos(DeclinationCalculation)
        / ((sin(Latitude) * cos(HourAngle) * cos(DeclinationCalculation)) - (sin(DeclinationCalculation) * cos(Latitude))));

    if (((sin(Latitude) * cos(HourAngle) * cos(DeclinationCalculation)) - (sin(DeclinationCalculation) * cos(Latitude))) > 0)
    {
        if (AzimuthalAngle < 0)
        {
            AzimuthalAngle = AzimuthalAngle + (2. * M_PI);
        }
    }
    else
    {
        AzimuthalAngle = AzimuthalAngle + M_PI;
    }

    if (AzimuthalAngle > M_PI)
    {
        AzimuthalAngle = AzimuthalAngle - (2. * M_PI);
    }
    //ПЕРЕВОД В СИСТЕМУ ПСТ
    AzimuthalAngle360 = AzimuthalAngle;
    if (AzimuthalAngle < 0)
    {
        AzimuthalAngle360 = (2. * M_PI) + AzimuthalAngle;
    }
    AzimuthalAngleBUST = (421.5 * (M_PI / 180.)) - AzimuthalAngle360;//Угол между направлением на север и направлением fi=270 равен 28.5 градуса
    if (AzimuthalAngleBUST > (2. * M_PI))
    {
        AzimuthalAngleBUST = AzimuthalAngleBUST - (2. * M_PI);
    }
    altitude = (M_PI / 2.) - ZenithAngle;
}

void StarCoordinates::getNutation(int argTJD)
{
    TJD = argTJD;

    //ВЫЧИСЛЕНИЕ t - количество юлианских столетий по 36525 средних солнечных суток
    JD = TJD + 2440000.5;
    d = JD - 0.5 - 2451545.0;//J2000//Вычли 0.5, потому что в 00:00 UT1 наступает половина Юлианского дня
    t = d / 36525.;

    //Расчеты ведутся с помощью перехода к прямоугольной системе координат
    //Источник "МЕТОДИЧЕСКИЕ УКАЗАНИЯ. СПУТНИКИ ЗЕМЛИ ИСКУССТВЕННЫЕ. ОСНОВНЫЕ СИСТЕМЫ КООРДИНАТ ДЛЯ БАЛЛИСТИЧЕСКОГО ОБЕСПЕЧЕНИЯ ПОЛЕТОВ И МЕТОДИКА РАСЧЕТА ЗВЕЗДНОГО ВРЕМЕНИ" 1989г. (1990г.)
    //РАСЧЕТ НУТАЦИИ
    //Средняя аномалия Луны
    l = (485866.733 / 3600.) / (180. / M_PI)
        + ((1717915922.633 / 3600.) / (180. / M_PI)) * t
        + ((31.310 / 3600.) / (180. / M_PI)) * t * t
        + ((0.064 / 3600.) / (180. / M_PI)) * t * t * t;
    //Средняя аномалия Солнца
    l1 = (1287099.804 / 3600.) / (180. / M_PI)
        + ((129596581.224 / 3600.) / (180. / M_PI)) * t
        - ((0.577 / 3600.) / (180. / M_PI)) * t * t
        - ((0.012 / 3600.) / (180. / M_PI)) * t * t * t;
    //Средний аргумент широты Луны
    F = (335778.877 / 3600.) / (180. / M_PI)
        + ((1739527263.137 / 3600.) / (180. / M_PI)) * t
        - ((13.257 / 3600.) / (180. / M_PI)) * t * t
        + ((0.011 / 3600.) / (180. / M_PI)) * t * t * t;
    //Разность средних долгот Луны и Солнца
    D = (1072261.307 / 3600.) / (180. / M_PI)
        + ((1602961601.328 / 3600.) / (180. / M_PI)) * t
        - ((6.891 / 3600.) / (180. / M_PI)) * t * t
        + ((0.019 / 3600.) / (180. / M_PI)) * t * t * t;
    //Средняя долгота восходящего узла орбиты Луны на эклиптике
    OM = (450160.280 / 3600.) / (180. / M_PI)
        - ((6962890.539 / 3600.) / (180. / M_PI)) * t
        + ((7.455 / 3600.) / (180. / M_PI)) * t * t
        + ((0.008 / 3600.) / (180. / M_PI)) * t * t * t;
    //Расчет с точностью 0'',1
    //Нутация в долготе
    NutationInLongitude = ((-17.1996 / 3600.) / (180. / M_PI)) * sin(OM)
        + ((0.2062 / 3600.) / (180. / M_PI)) * sin(2. * OM)
        - ((1.3187 / 3600.) / (180. / M_PI)) * sin(2. * (F - D + OM))
        + ((0.1426 / 3600.) / (180. / M_PI)) * sin(l1)
        - ((0.2274 / 3600.) / (180. / M_PI)) * sin(2. * (F + OM));

    //Нутация в наклоне
    NutationInObliquity = ((9.2025 / 3600.) / (180. / M_PI)) * cos(OM)
        + ((0.5736 / 3600.) / (180. / M_PI)) * cos(2. * (F - D + OM))
        + ((0.0977 / 3600.) / (180. / M_PI)) * cos(2. * (F + D));

    MeanObliquityOfTheEcliptic = (84381.448 / 3600.) / (180. / M_PI)
        - ((46.815 / 3600.) / (180. / M_PI)) * t
        - ((0.00059 / 3600.) / (180. / M_PI)) * t * t
        + ((0.001813 / 3600.) / (180. / M_PI)) * t * t * t;

    TrueObliquityOfTheEcliptic = MeanObliquityOfTheEcliptic + NutationInObliquity;

    n11 = cos(NutationInLongitude);
    n12 = -sin(NutationInLongitude) * cos(MeanObliquityOfTheEcliptic);
    n13 = -sin(NutationInLongitude) * sin(MeanObliquityOfTheEcliptic);
    n21 = sin(NutationInLongitude) * cos(TrueObliquityOfTheEcliptic);
    n22 = cos(NutationInLongitude) * cos(TrueObliquityOfTheEcliptic) * cos(MeanObliquityOfTheEcliptic) + sin(TrueObliquityOfTheEcliptic) * sin(MeanObliquityOfTheEcliptic);
    n23 = cos(NutationInLongitude) * cos(TrueObliquityOfTheEcliptic) * sin(MeanObliquityOfTheEcliptic) - sin(TrueObliquityOfTheEcliptic) * cos(MeanObliquityOfTheEcliptic);
    n31 = sin(NutationInLongitude) * sin(TrueObliquityOfTheEcliptic);
    n32 = cos(NutationInLongitude) * sin(TrueObliquityOfTheEcliptic) * cos(MeanObliquityOfTheEcliptic) - cos(TrueObliquityOfTheEcliptic) * sin(MeanObliquityOfTheEcliptic);
    n33 = cos(NutationInLongitude) * sin(TrueObliquityOfTheEcliptic) * sin(MeanObliquityOfTheEcliptic) + cos(TrueObliquityOfTheEcliptic) * cos(MeanObliquityOfTheEcliptic);

    X2000 = cos(Ascension) * cos(Declination);
    Y2000 = sin(Ascension) * cos(Declination);
    Z2000 = sin(Declination);

    Xcurrent = X2000 * n11 + Y2000 * n12 + Z2000 * n13;
    Ycurrent = X2000 * n21 + Y2000 * n22 + Z2000 * n23;
    Zcurrent = X2000 * n31 + Y2000 * n32 + Z2000 * n33;

    /*RAWithNutation = atan2(Ycurrent, Xcurrent);
    if (RAWithNutation < 0)
    {
        RAWithNutation = RAWithNutation + (2. * M_PI);
    }
    if (RAWithNutation > 2. * M_PI)
    {
        RAWithNutation = RAWithNutation - (2. * M_PI);
    }*/

    DecWithNutation = asin(Zcurrent);

    //AscensionNutation = RAWithNutation - Ascension;
    AscensionNutation = NutationInLongitude * cos(TrueObliquityOfTheEcliptic);
    DeclinationNutation = DecWithNutation - Declination;
}

void StarCoordinates::getPrecession(int argTJD)
{
    TJD = argTJD;

    //ВЫЧИСЛЕНИЕ t - количество юлианских столетий по 36525 средних солнечных суток
    JD = TJD + 2440000.5;
    d = JD - 0.5 - 2451545.0;//J2000//Вычли 0.5, потому что в 00:00 UT1 наступает половина Юлианского дня
    t = d / 36525.;

    //РАСЧЕТ ПРЕЦЕССИИ
    coeff1 = ((2306.2181 / 3600.) / (180. / M_PI)) * t
        + ((0.30188 / 3600.) / (180. / M_PI)) * t * t
        + ((0.017998 / 3600.) / (180. / M_PI)) * t * t * t;//ζ
    coeff2 = ((2306.2181 / 3600.) / (180. / M_PI)) * t
        + ((1.09468 / 3600.) / (180. / M_PI)) * t * t
        + ((0.018203 / 3600.) / (180. / M_PI)) * t * t * t;//Z
    coeff3 = ((2004.3109 / 3600.) / (180. / M_PI)) * t
        - ((0.42665 / 3600.) / (180. / M_PI)) * t * t
        - ((0.041833 / 3600.) / (180. / M_PI)) * t * t * t;//θ
    p11 = cos(coeff1) * cos(coeff2) * cos(coeff3) - sin(coeff1) * sin(coeff2);
    p12 = -sin(coeff1) * cos(coeff2) * cos(coeff3) - cos(coeff1) * sin(coeff2);
    p13 = -cos(coeff2) * sin(coeff3);
    p21 = cos(coeff1) * sin(coeff2) * cos(coeff3) + sin(coeff1) * cos(coeff2);
    p22 = -sin(coeff1) * sin(coeff2) * cos(coeff3) + cos(coeff1) * cos(coeff2);
    p23 = -sin(coeff2) * sin(coeff3);
    p31 = cos(coeff1) * sin(coeff3);
    p32 = -sin(coeff1) * sin(coeff3);
    p33 = cos(coeff3);

    X2000 = cos(Ascension) * cos(Declination);
    Y2000 = sin(Ascension) * cos(Declination);
    Z2000 = sin(Declination);

    Xcurrent = X2000 * p11 + Y2000 * p12 + Z2000 * p13;
    Ycurrent = X2000 * p21 + Y2000 * p22 + Z2000 * p23;
    Zcurrent = X2000 * p31 + Y2000 * p32 + Z2000 * p33;

    RAWithPrecession = atan2(Ycurrent, Xcurrent);
    if (RAWithPrecession < 0)
    {
        RAWithPrecession = RAWithPrecession + (2. * M_PI);
    }
    if (RAWithPrecession > 2. * M_PI)
    {
        RAWithPrecession = RAWithPrecession - (2. * M_PI);
    }

    DecWithPrecession = asin(Zcurrent);

    AscensionPrecession = RAWithPrecession - Ascension;
    DeclinationPrecession = DecWithPrecession - Declination;
}

double StarCoordinates::getAscensionNutationAndPrecession(int argTJD, double argAscension, double argDeclination)
{
    Ascension = argAscension;
    Declination = argDeclination;
    if (Ascension == 360. / (180. / M_PI))
    {
        Ascension = 0.;
    }
    getNutation(argTJD);
    getPrecession(argTJD);
    return AscensionNutation + AscensionPrecession;
}

double StarCoordinates::getDeclinationNutationAndPrecession(int argTJD, double argAscension, double argDeclination)
{
    Ascension = argAscension;
    Declination = argDeclination;
    if (Ascension == 360. / (180. / M_PI))
    {
        Ascension = 0.;
    }
    getNutation(argTJD);
    getPrecession(argTJD);
    return DeclinationNutation + DeclinationPrecession;
}


StarCoordinates::~StarCoordinates()
{

}