#ifndef UTILS_H
#define UTILS_H
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <Windows.h>
#include <string>
#include <locale>
#include <map>
#include <vector>
#include <ostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include <glad/glad.h>

#include <regex>
#include <vector>
#include <string>
#include <utility>

#include "math_utils.h"

static std::string multibyte2utf8(std::wstring str) {
    int utf8_num = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    char* utf8_str = new char[utf8_num];
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, utf8_str, utf8_num, NULL, NULL);
    std::string result = utf8_str;
    delete[] utf8_str;
    return result;
}
static std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    if (from.empty())
        return "";
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}


static bool isAlphabet(wchar_t c) {
    if (L'_' == c || (L'a' <= c && c <= L'z') || (L'A' <= c && c <= L'Z')) {
        return true;
    }
    return false;
}

static bool isNumber(wchar_t c) {
    if (L'0' <= c && c <= L'9') {
        return true;
    }
    return false;
}

static vec4 hex2rgba(int hexValue) {
    vec4 rgbColor;
    rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;
    rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;
    rgbColor.b = ((hexValue) & 0xFF) / 255.0;
    rgbColor.a = 1.0f;
    return rgbColor;
}


enum class GLRenderMode {
    GL2D, GL3D
};

static void glSetRenderMode(GLRenderMode mode) {
    if (mode == GLRenderMode::GL2D) {
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (mode == GLRenderMode::GL3D) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        glDisable(GL_BLEND);
    }
}

static std::wstring s2ws(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

static void writeFile(const std::wstring& filePath, const std::wstring& content) {
    std::wofstream writeFile(filePath);
    if (writeFile.is_open()) {
        writeFile << content;
        writeFile.close();
    }
}

static std::string readFile(const std::string& filePath) {
    std::ifstream openFile(filePath.data());
    std::string content = "";
    if (openFile.is_open()) {
        std::string line;
        while (getline(openFile, line)) {
            content += line + "\n";
        }
        openFile.close();
    }
    return content;
}

static size_t getSizeOfFile(const std::wstring& path) {
    struct _stat fileinfo;
    _wstat(path.c_str(), &fileinfo);
    return fileinfo.st_size;
}


static std::wstring readFileW(const std::wstring& filename) {
    std::wstring buffer = L"";
    FILE* f;
    _wfopen_s(&f, filename.c_str(), L"rtS, ccs=UTF-8");
    if (f == NULL) {
        return buffer;
    }
    size_t filesize = getSizeOfFile(filename);
    if (filesize > 0) {
        buffer.resize(filesize);
        size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
        buffer.resize(wchars_read);
        buffer.shrink_to_fit();
    }
    fclose(f);
    return buffer;
}

static std::string getCurrentPath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

static std::wstring getCurrentPathW() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    std::string temp = std::string(std::string(buffer).substr(0, pos));
    return std::wstring(temp.begin(), temp.end());
}

static inline bool fileExists(const std::string name) {
    std::ifstream f(name.c_str());
    return f.good();
}






#endif