/*
* ΔLaunch
* Copyright (c) 2018-2019, Reisyukaku
* 
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
* 
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "Network.hpp"

Network* Network::instance = NULL;
static std::string readBuffer;
struct curl_slist *Network::hosts = NULL;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    readBuffer.append((char*)contents, realsize);
    return realsize;
}

Network::Network() {
	msgBox = UI::MessageBox::getInstance();
    //nifmSetWirelessCommunicationEnabled(true);
    curl_global_init(CURL_GLOBAL_ALL);
    //hosts = curl_slist_append(NULL, "reinx.guide:167.99.228.103");
};

std::string Network::Request(std::string url, RequestMethod method, std::string postdata) {
	std::string outStr = "";
    CURL *curl = curl_easy_init();
    CURLcode res = CURLE_OK;
    long httpCode = 0;

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //curl_easy_setopt(curl, CURLOPT_RESOLVE, hosts);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        if(method == HTTP_POST)
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);
        
        if(res != CURLE_OK) {
            msgBox->Show("Network error", "Curl error: " + std::to_string(res));
        }
        else if(httpCode != 200) {
            msgBox->Show("Network error", "Http error: " + std::to_string(httpCode));
        }
        else {
            outStr = readBuffer;
        }
    }

    return outStr;
}

void Network::Download(std::string url, std::string file) {
	FILE *fp;
    CURLcode res = CURLE_OK;
    CURL *curl = curl_easy_init();
    if (curl) {   
        fp = fopen(file.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_RESOLVE, hosts);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    } else {
        res = CURLE_HTTP_RETURNED_ERROR;
    }
    if (res != CURLE_OK) {
        remove(file.c_str());
        msgBox->Show("Network error", "Curl error: " + std::to_string(res));
    }
}

u32 Network::GetWifiStrength() {
	u32 wifiStrength = 0;
#ifdef __SWITCH__
    NifmInternetConnectionType contype;
    NifmInternetConnectionStatus conStat;
    nifmGetInternetConnectionStatus(&contype, &wifiStrength, &conStat);
#endif
    return wifiStrength;
}