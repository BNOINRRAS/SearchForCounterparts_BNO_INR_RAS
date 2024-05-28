#ifndef SENDEMAIL_H
#define SENDEMAIL_H

#include <iostream>
#include <string>

class SendEmail
{
private:
	//Содержимое письма (Разница по времени, координатам)
	//double timeDifference, angleBetweenEvents;

    struct upload_status;

    static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp);

    static char* getMyMessageAddress(std::string* myMessage);

    std::string modified_SKYMAP_FITS_URL;//Строка SKYMAP_FITS_URL, в которой символ '/' заменен на '.'
    std::string SKYMAP_FITS_URL_path;
    std::string SKYMAP_FITS_URL_png;

    std::string picturePathAndName;

	int send(bool isThereAttachment);

public:
    SendEmail();

    void formAndSendMessage(std::string sender, int TJD, double SOD,
			 		double ra, double dec, double error, bool isThereAttachment);

    void formAndSendMessage(std::string sender_1, int TJD_1, double SOD_1,
			 		double ra_1, double dec_1, double error_1,
			 		std::string sender_2, int TJD_2, double SOD_2,
			 		double ra_2, double dec_2, double error_2,
			 		double timeBetweenEvents, double angleBetweenEvents, bool isCounterpartFound, bool isThereAttachment);

    void define_SKYMAP_FITS_URL(std::string SKYMAP_FITS_URL);

    ~SendEmail();

};

#endif // SENDEMAIL_H