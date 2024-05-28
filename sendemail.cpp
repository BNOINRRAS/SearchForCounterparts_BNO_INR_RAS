#include "sendemail.hpp"

#include <stdio.h>
#include <cstring>
#include <sstream>
#include <algorithm>//Для функции "std::replace(...)"
#include <curl/curl.h>

// Здесь много кода, позаимствованного из https://curl.se/libcurl/c/smtp-ssl.html
// и из https://curl.se/libcurl/c/smtp-mime.html

#define FROM_MAIL     "<unatlokovislam@yandex.ru>"
#define TO_MAIL       "<unatlokovislam@yandex.ru>"
#define CC_MAIL       ""

std::string messageContent;

std::string headers_text;
const char* headerAddress;

static const char inline_text[] =
	"This is the inline text message of the email.\r\n"
	"\r\n"
	"  It could be a lot of lines that would be displayed in an email\r\n"
	"viewer that is not able to handle HTML.\r\n";

SendEmail::SendEmail()
{
	SKYMAP_FITS_URL_path = "GW_Localization_Contours_and_skymaps/";
	SKYMAP_FITS_URL_png = ".png";
}

struct SendEmail::upload_status
{
  size_t bytes_read;
};

char* SendEmail::getMyMessageAddress(std::string* myMessage)
{
	return &( ( *myMessage )[0] );
}

size_t SendEmail::payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1))
  {
    return 0;
  }
 
  char* ourMsgAddress = getMyMessageAddress(&messageContent);
  //std::cout << messageContent << std::endl;

  data = &ourMsgAddress[upload_ctx->bytes_read];// Необъяснимая игра с адресами
 
  if(data)
  {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}

void SendEmail::formAndSendMessage(std::string sender, int TJD, double SOD,
			 				double ra, double dec, double error, bool isThereAttachment)
{
	std::stringstream messageContentStream;
	std::string subject = "Event from ";

	if (isThereAttachment == false)
	{
	  messageContentStream.str(std::string());
	  messageContentStream << "From: " << FROM_MAIL << "\r\n"
	                       << "To: " << TO_MAIL << "\r\n"
	                       << "Cc: " << CC_MAIL << "\r\n"
	                       << "Subject: " << subject << sender << "\r\n\r\n"
	                       << "TJD:\t" << TJD << "\nSOD:\t" << SOD << "\n"
	                       << "RA:\t" << ra << "\nDec:\t" << dec << "\n"
	                       << "Error:\t" << error << "\r\n\r\n";
	  messageContent.clear();
	  messageContent = messageContentStream.str();
	  send(isThereAttachment);
	}
	else
	{
		messageContentStream.str(std::string());
		messageContentStream << "From: " << FROM_MAIL << "\r\n"
	                       << "To: " << TO_MAIL << "\r\n"
	                       << "Cc: " << CC_MAIL << "\r\n"
	                       << "Subject: " << subject << sender;
		headers_text.clear();
		headers_text = messageContentStream.str();
		messageContentStream.str(std::string());
		messageContentStream << "TJD:\t" << TJD << "\nSOD:\t" << SOD << "\n"
	                       << "RA:\t" << ra << "\nDec:\t" << dec << "\n"
	                       << "Error:\t" << error << "\r\n\r\n";
		messageContent.clear();
		messageContent = messageContentStream.str();
		send(isThereAttachment);
	}
}

void SendEmail::formAndSendMessage(std::string sender_1, int TJD_1, double SOD_1,
			 		double ra_1, double dec_1, double error_1,
			 		std::string sender_2, int TJD_2, double SOD_2,
			 		double ra_2, double dec_2, double error_2,
			 		double timeBetweenEvents, double angleBetweenEvents, bool isCounterpartFound, bool isThereAttachment)
{
  std::string subject;
  if (isCounterpartFound == false)
  {
  	subject = "Time coincidence";
  }
  else
  {
  	subject = "Counterpart found!";
  }

  std::stringstream messageContentStream;

  if (isThereAttachment == false)
  {
	  messageContentStream.str(std::string());
	  messageContentStream << "From: " << FROM_MAIL << "\r\n"
	                       << "To: " << TO_MAIL << "\r\n"
	                       << "Cc: " << CC_MAIL << "\r\n"
	                       << "Subject: " << subject << "\r\n\r\n"
	                       << sender_1 << "\r\n"
	                       << "TJD:\t" << TJD_1 << "\nSOD:\t" << SOD_1 << "\n"
	                       << "RA:\t" << ra_1 << "\nDec:\t" << dec_1 << "\n"
	                       << "Error:\t" << error_1 << "\r\n\r\n"
	                       << sender_2 << "\r\n"
	                       << "TJD:\t" << TJD_2 << "\nSOD:\t" << SOD_2 << "\n"
	                       << "RA:\t" << ra_2 << "\nDec:\t" << dec_2 << "\n"
	                       << "Error:\t" << error_2 << "\r\n\r\n"
	                       << "Time Between Events:\t" << timeBetweenEvents << "\n"
	                       << "Angle Between Events:\t" << angleBetweenEvents << "\r\n\r\n";

	  messageContent.clear();
	  messageContent = messageContentStream.str();
	  send(isThereAttachment);
  }
  else
  {
  	  messageContentStream.str(std::string());
	  messageContentStream << "From: " << FROM_MAIL << "\r\n"
	                       << "To: " << TO_MAIL << "\r\n"
	                       << "Cc: " << CC_MAIL << "\r\n"
	                       << "Subject: " << subject;
	  headers_text.clear();
	  headers_text = messageContentStream.str();
	  messageContentStream.str(std::string());
	  messageContentStream << sender_1 << "\r\n"
	                       << "TJD:\t" << TJD_1 << "\nSOD:\t" << SOD_1 << "\n"
	                       << "RA:\t" << ra_1 << "\nDec:\t" << dec_1 << "\n"
	                       << "Error:\t" << error_1 << "\r\n\r\n"
	                       << sender_2 << "\r\n"
	                       << "TJD:\t" << TJD_2 << "\nSOD:\t" << SOD_2 << "\n"
	                       << "RA:\t" << ra_2 << "\nDec:\t" << dec_2 << "\n"
	                       << "Error:\t" << error_2 << "\r\n\r\n"
	                       << "Time Between Events:\t" << timeBetweenEvents << "\n"
	                       << "Angle Between Events:\t" << angleBetweenEvents << "\r\n\r\n";
	  messageContent.clear();
	  messageContent = messageContentStream.str();
	  send(isThereAttachment);
  }
}

int SendEmail::send(bool isThereAttachment)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  curl = curl_easy_init();

  /* Set username and password */
  //Yandex
  curl_easy_setopt(curl, CURLOPT_USERNAME, "unatlokovislam@yandex.ru");
  curl_easy_setopt(curl, CURLOPT_PASSWORD, "bptznbsyvydpdcig");
  curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.yandex.ru:465");
  //Mail
  //curl_easy_setopt(curl, CURLOPT_USERNAME, "unatlokov@mail.ru");
  //curl_easy_setopt(curl, CURLOPT_PASSWORD, "aCRu89Tagb1uqjyxWpwh");
  //curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.mail.ru:465");

  if (isThereAttachment == false)
  {
	  struct curl_slist *recipients = NULL;
	  struct upload_status upload_ctx = { 0 };
	 
	  if(curl) {
	    /* If you want to connect to a site who is not using a certificate that is
	     * signed by one of the certs in the CA bundle you have, you can skip the
	     * verification of the server's certificate. This makes the connection
	     * A LOT LESS SECURE.
	     *
	     * If you have a CA cert for the server stored someplace else than in the
	     * default bundle, then the CURLOPT_CAPATH option might come handy for
	     * you. */
	#ifdef SKIP_PEER_VERIFICATION
	    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	#endif
	 
	    /* If the site you are connecting to uses a different host name that what
	     * they have mentioned in their server certificate's commonName (or
	     * subjectAltName) fields, libcurl will refuse to connect. You can skip
	     * this check, but this will make the connection less secure. */
	#ifdef SKIP_HOSTNAME_VERIFICATION
	    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	#endif
	 
	    /* Note that this option is not strictly required, omitting it will result
	     * in libcurl sending the MAIL FROM command with empty sender data. All
	     * autoresponses should have an empty reverse-path, and should be directed
	     * to the address in the reverse-path which triggered them. Otherwise,
	     * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
	     * details.
	     */
	    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_MAIL);
	 
	    /* Add two recipients, in this particular case they correspond to the
	     * To: and Cc: addressees in the header, but they could be any kind of
	     * recipient. */
	    recipients = curl_slist_append(recipients, &TO_MAIL[0]);
	    //recipients = curl_slist_append(recipients, CC_MAIL);
	    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
	 
	    /* We are using a callback function to specify the payload (the headers and
	     * body of the message). You could just use the CURLOPT_READDATA option to
	     * specify a FILE pointer to read from. */
	    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
	    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
	    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	 
	    /* Since the traffic will be encrypted, it is very useful to turn on debug
	     * information within libcurl to see what is happening during the
	     * transfer */
	    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	 
	    /* Send the message */
	    res = curl_easy_perform(curl);
	 
	    /* Check for errors */
	    if(res != CURLE_OK)
	      fprintf(stderr, "curl_easy_perform() failed: %s\n",
	              curl_easy_strerror(res));
	 
	    /* Free the list of recipients */
	    curl_slist_free_all(recipients);
	 
	    /* Always cleanup */
	    curl_easy_cleanup(curl);
	  }
	 
	  return (int)res;
  }
  else
  {
	  if(curl) {
	    struct curl_slist *headers = NULL;
	    struct curl_slist *recipients = NULL;
	    struct curl_slist *slist = NULL;
	    curl_mime *mime;
	    //curl_mime *alt;
	    curl_mimepart *part;
	    const char **cpp;
	 	 
	    /* Note that this option is not strictly required, omitting it will result
	     * in libcurl sending the MAIL FROM command with empty sender data. All
	     * autoresponses should have an empty reverse-path, and should be directed
	     * to the address in the reverse-path which triggered them. Otherwise,
	     * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
	     * details.
	     */
	    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_MAIL);
	 
	    /* Add two recipients, in this particular case they correspond to the
	     * To: and Cc: addressees in the header, but they could be any kind of
	     * recipient. */
	    recipients = curl_slist_append(recipients, TO_MAIL);
	    recipients = curl_slist_append(recipients, CC_MAIL);
	    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
	 
	    /* allow one of the recipients to fail and still consider it okay */
	    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT_ALLLOWFAILS, 1L);
	 
	    /* Build and set the message header list. */
	    headerAddress = getMyMessageAddress(&headers_text);
	    for(cpp = &headerAddress; *cpp; cpp++)
	      headers = curl_slist_append(headers, *cpp);
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	    /* Build the mime message. */
	    mime = curl_mime_init(curl);
	 
	    /* The inline part is an alternative proposing the html and the text
	       versions of the email. */
	    //alt = curl_mime_init(curl);
	 
	    /* HTML message. */
	    //part = curl_mime_addpart(alt);
	    //curl_mime_data(part, inline_html, CURL_ZERO_TERMINATED);
	    //curl_mime_type(part, "text/html");
	 
	    /* Text message. */
	    //part = curl_mime_addpart(alt);

	    /* Create the inline part. */
	    part = curl_mime_addpart(mime);
	    curl_mime_data(part, messageContent.c_str(), CURL_ZERO_TERMINATED);
	    //curl_mime_subparts(part, alt);
	    slist = curl_slist_append(NULL, "Content-Disposition: inline");
	    curl_mime_headers(part, slist, 1);
	 
	    /* Add the current source program as an attachment. */
	    part = curl_mime_addpart(mime);
	    picturePathAndName = SKYMAP_FITS_URL_path + modified_SKYMAP_FITS_URL + SKYMAP_FITS_URL_png;
	    curl_mime_filedata(part, picturePathAndName.c_str());
    	curl_mime_encoder(part, "base64");
	    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
	 
	    /* Send the message */
	    res = curl_easy_perform(curl);
	 
	    /* Check for errors */
	    if(res != CURLE_OK)
	      fprintf(stderr, "curl_easy_perform() failed: %s\n",
	              curl_easy_strerror(res));
	 
	    /* Free lists. */
	    curl_slist_free_all(recipients);
	    curl_slist_free_all(headers);
	 
	    /* curl will not send the QUIT command until you call cleanup, so you
	     * should be able to re-use this connection for additional messages
	     * (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and
	     * calling curl_easy_perform() again. It may not be a good idea to keep
	     * the connection open for a very long time though (more than a few
	     * minutes may result in the server timing out the connection), and you do
	     * want to clean up in the end.
	     */
	    curl_easy_cleanup(curl);
	 
	    /* Free multipart message. */
	    curl_mime_free(mime);
	  }
	 
	  return (int)res;
  }
}

void SendEmail::define_SKYMAP_FITS_URL(std::string SKYMAP_FITS_URL)
{
	modified_SKYMAP_FITS_URL = SKYMAP_FITS_URL;
	std::replace(modified_SKYMAP_FITS_URL.begin(), modified_SKYMAP_FITS_URL.end(), '/', '.');
}

SendEmail::~SendEmail()
{

}