//
// Created by juzenhon on 2018/2/9.
//



#include <curl.h>


int postform(CURL *easy_handle){
    curl_mime *mime;
    curl_mimepart *part;

    /* Build an HTTP form with a single field named "data", */
    mime = curl_mime_init(easy_handle);
    part = curl_mime_addpart(mime);
    curl_mime_data(part, "This is the field data", CURL_ZERO_TERMINATED);

    curl_mime_name(part, "data");
    curl_mime_filedata(part,"sss.png");

//    /* send image data from memory */
//    curl_mime_data(part, imagebuf, imagebuf_len);
//    /* set a file name to make it look like a file upload */
//    curl_mime_filename(part, "image.png");

    /* Post and send it. */
    curl_easy_setopt(easy_handle, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(easy_handle, CURLOPT_URL, "http://example.com");


    curl_easy_perform(easy_handle);

    /* Clean-up. */
    curl_easy_cleanup(easy_handle);
    curl_mime_free(mime);
}