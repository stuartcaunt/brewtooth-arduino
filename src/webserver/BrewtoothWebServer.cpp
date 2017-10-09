#include "BrewtoothWebServer.h"
#include <utils/Log.h>
#include <FS.h>

bool BrewtoothWebServer::serveFile(const String & path, const String & cacheHeader) {
    bool isFile = SPIFFS.exists(path);
    if (isFile) {

        String finalPath = String(path);

        String contentType = getContentType(path);
        
        // look for gz file, only if the original specified path is not a gz.  So part only works to send gzip via content encoding when a non compressed is asked for
        // if you point the the path to gzip you will serve the gzip as content type "application/x-gzip", not text or javascript etc...
        if (!finalPath.endsWith(".gz") && !SPIFFS.exists(finalPath))  {
            String pathWithGz = finalPath + ".gz";
            if (SPIFFS.exists(pathWithGz)) {
                finalPath += ".gz";
            }
        }

        File f = SPIFFS.open(finalPath, "r");
        if (!f) {
            return false;
        }

        //
        if (cacheHeader.length() != 0) {
            this->sendHeader("Cache-Control", cacheHeader);
        }

        this->streamFile(f, contentType);

        return true;

    } else {
        WARN("File with path %s does not exist", path.c_str());
        this->send(500, "text/plain", "Cannot serve file");
        
        return false;
    }
    
}

String BrewtoothWebServer::getContentType(const String & path) {
    if (path.endsWith(".html")) return "text/html";
    else if (path.endsWith(".htm")) return "text/html";
    else if (path.endsWith(".json")) return "application/json";
    else if (path.endsWith(".css")) return "text/css";
    else if (path.endsWith(".txt")) return "text/plain";
    else if (path.endsWith(".js")) return "application/javascript";
    else if (path.endsWith(".png")) return "image/png";
    else if (path.endsWith(".gif")) return "image/gif";
    else if (path.endsWith(".jpg")) return "image/jpeg";
    else if (path.endsWith(".ico")) return "image/x-icon";
    else if (path.endsWith(".svg")) return "image/svg+xml";
    else if (path.endsWith(".ttf")) return "application/x-font-ttf";
    else if (path.endsWith(".otf")) return "application/x-font-opentype";
    else if (path.endsWith(".woff")) return "application/font-woff";
    else if (path.endsWith(".woff2")) return "application/font-woff2";
    else if (path.endsWith(".eot")) return "application/vnd.ms-fontobject";
    else if (path.endsWith(".sfnt")) return "application/font-sfnt";
    else if (path.endsWith(".xml")) return "text/xml";
    else if (path.endsWith(".pdf")) return "application/pdf";
    else if (path.endsWith(".zip")) return "application/zip";
    else if(path.endsWith(".gz")) return "application/x-gzip";
    else if (path.endsWith(".appcache")) return "text/cache-manifest";
    return "application/octet-stream";
}