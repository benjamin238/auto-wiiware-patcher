#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


size_t find_string_in_buf(unsigned char *buf, size_t len, size_t offset, const char *s)
{
    long i, j;
    int slen = strlen(s);
    long imax = len - slen - 1;
    long ret = -1;
    bool match;

    for(i = 0+offset; i < imax; i++) {
        match = true;

        for(j=0; j<slen; j++) {
            if(buf[i+j] != s[j]) {
                match = false;
                break;
            }
        }

        if(match) {
            ret = i;
            break;
        }
    }

    return ret;
}

size_t Filesize(ifstream &file) {
    file.seekg(0, std::ios::end);
    size_t len = file.tellg();
    file.seekg(0, std::ios_base::beg);

    return len;
}

void ReplaceURLs(char *buffer, size_t len) {
    size_t pos = 0;
    int cnt = 0;

    string url;

    while(1) {
        // Find all urls starting with https
        pos = find_string_in_buf((unsigned char*)buffer, len, pos, "https://");
        if(pos < len-1) {
            int idx = 0;
            url = "";

            // copy url from buffer into string
            while(1) {
                url.push_back(buffer[pos+idx]);
                idx++;

                if(buffer[pos+idx] == '\0') {
                    // end of url
                    break;
                }
            }

            // get original length
            size_t len_url = url.size();

            // https -> http
            url.erase(4, 1);

            // replace nintendo url with wiimm url
            size_t p = url.find("nintendowifi.net");
            if(p != string::npos) {
                url.replace(p, 16, "benfi.ml");
            }

            // Fill up string with NULL until original length is reached
            while(url.size() < len_url) {
                url.push_back('\0');
            }

            // Copy modified url back to buffer
            strncpy(&buffer[pos], url.c_str(), url.size());
            cnt++;

        } if(pos >= len-1) {
            // end of buffer
            break;
        }
        pos++;
    }
    cout << "Replaced " << cnt << " https-URLs." << endl;

    cnt = 0;
    pos = 0;
    while(1) {
        // find all urls without https
        pos = (size_t)find_string_in_buf((unsigned char*)buffer, len, pos, "nintendowifi.net");
        if(pos < len-1) {
            int idx = 0;
            url = "";

            // copy url from buffer into string
            while(1) {
                url.push_back(buffer[pos+idx]);
                idx++;

                if(buffer[pos+idx] == '\0') {
                    // end of url
                    break;
                }
            }

            // get original length
            size_t len_url = url.size();

            // replace nintendo url with wiimm url
            url.replace(0, 16, "wiimmfi.de");

            // Fill up string with NULL until original length is reached
            while(url.size() < len_url) {
                url.push_back('\0');
            }

            // Copy modified url back to buffer
            strncpy(&buffer[pos], url.c_str(), url.size());
            cnt++;

        } if(pos >= len-1) {
            // end of buffer
            break;
        }
        pos++;
    }
    cout << "Replaced " << cnt << " other URLs." << endl;
}


int main()
{
    cout << "WiiWare Patcher 0.2" << endl;


    ifstream file("00000001.app", std::ifstream::binary);
    if(!file.is_open()) {
        cerr << "Failed to open 00000001.app" << endl;
        cerr << "Are you trying to run auto-wiiware-patcher? Please run patcher.bat instead." << endl;
        system("pause");
        return -1;
    }

    // Encode file
    int ret = system("lzx.exe -d 00000001.app");
    if(ret != 0) {
        cerr << "Failed to encode 00000001.app" << endl;
        cerr << "Are you sure that a program called lzx is in this directory?" << endl;
        system("pause");
        return -2;
    }
    cout << "Exit code: " << ret << endl << endl;

    // Get file size
    size_t len = Filesize(file);
    char *buffer = new char[len];

    // Read file into buffer
    file.read(buffer, len);
    if(!file) {
        cerr << "Error: only " << file.gcount() << " could be read.";
        file.close();
        delete buffer;
        system("pause");
        return -3;
    }
    file.close();

    cout << "Replacing strings..." << endl;
    ReplaceURLs(buffer, len);


    // Save patched file
    ofstream ofile("00000001.app", std::ifstream::binary);
    if(!ofile.is_open()) {
        cerr << "Failed to open 00000001.app" << endl;
        system("pause");
        return -4;
    }
    ofile.write(buffer, len);
    ofile.close();

    delete buffer;

    // Decode File
    ret = system("lzx.exe -evb 00000001.app");
    if(ret != 0) {
        cerr << "Failed to decode 00000001.app" << endl;
        cerr << "Are you sure that a program called lzx is in this directory?" << endl;
        system("pause");
        return -4;
    }
    cout << "Exit code: " << ret << endl;


    return 0;
}
