#include "album_art_finder.h"

// Функция записи для CURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функция перевода полученной строки в адекватное состояние
std::string urlEncode(const std::string& str) {
    CURL* curl = curl_easy_init();
    char* encoded = curl_easy_escape(curl, str.c_str(), str.length());
    std::string result(encoded);
    curl_free(encoded);
    curl_easy_cleanup(curl);
    return result;
}

std::string getAlbumCoverUrl(std::string artist, std::string title) {
    // Формируем запрос
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
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
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
                if (coverUri.find("http") == std::string::npos) {
                    coverUri = "https://" + coverUri;
                }
                // Заменяем размер
                return coverUri.replace(coverUri.find("%%"), 2, "400x400");
            }
        }
    }
    
    // В случае неудачи
    return "channels4_profile";
}