#include "album_art_finder.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string urlEncode(const std::string& str) {
    CURL* curl = curl_easy_init();
    char* encoded = curl_easy_escape(curl, str.c_str(), str.length());
    std::string result(encoded);
    curl_free(encoded);
    curl_easy_cleanup(curl);
    return result;
}

std::string getAlbumCoverUrl(std::string artist, std::string title) {
    std::string query = artist + " " + title;
    std::string encodedQuery = urlEncode(query);
    std::string apiUrl = "https://api.music.yandex.net/search?text=" + encodedQuery + "&type=track&page=0&pageSize=1";
    
    CURL* curl = curl_easy_init();
    std::string response;
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Для тестов
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res == CURLE_OK) {
            // Ищем coverUri в ответе: "coverUri":"avatar.yandex.net/..."
            std::regex coverRegex("\"coverUri\":\"([^\"]+)\"");
            std::smatch match;
            
            if (std::regex_search(response, match, coverRegex)) {
                std::string coverUri = match[1].str();
                // Формируем полный URL обложки
                // Яндекс хранит обложки как avatar.yandex.net/get-music-content/...
                if (coverUri.find("http") == std::string::npos) {
                    coverUri = "https://" + coverUri;
                }
                // Заменяем размер на большой (например 400x400)
                return coverUri.replace(coverUri.find("%%"), 2, "400x400");
            }
        }
    }
    
    return "channels4_profile";
}