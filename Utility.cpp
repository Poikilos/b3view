#include "Utility.h"
#include <algorithm>
#include <clocale>
#include <cmath>
#include <cwctype> // #include <cwtype>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include "Debug.h"

using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace std;

void Utility::dumpVectorToConsole(const vector3df& vector)
{
    debug() << "X: " << vector.X << " Y: " << vector.Y << " Z: " << vector.Z << endl;
}

void Utility::dumpMeshInfoToConsole(IAnimatedMeshSceneNode* node)
{
    if (node == nullptr) {
        debug() << "[MESH]: # node: nullptr" << endl;
        return;
    }
    if (node->getMesh() == nullptr) {
        debug() << "[MESH]: # node->getMesh(): nullptr" << endl;
        return;
    }
    // Dump some information about the mesh to the console
    IAnimatedMesh* mesh = node->getMesh();

    debug() << "[MESH]: # of frames            : " << mesh->getFrameCount()
            << endl;
    debug() << "[MESH]: # of materials         : " << node->getMaterialCount()
            << endl;
    for (irr::u32 matIndex = 0; matIndex < node->getMaterialCount(); matIndex++) {
        debug() << "[MESH]:   Material # " << matIndex << endl;
        const SMaterial& material = node->getMaterial(matIndex);
        debug() << "[MESH]:      Diffuse Color       : A"
                << material.DiffuseColor.getAlpha()
                << " R" << material.DiffuseColor.getRed()
                << " G" << material.DiffuseColor.getGreen()
                << " B" << material.DiffuseColor.getBlue() << endl;
        debug() << "[MESH]:      Specular Color      : A"
                << material.SpecularColor.getAlpha()
                << " R" << material.SpecularColor.getRed()
                << " G" << material.SpecularColor.getGreen()
                << " B" << material.SpecularColor.getBlue() << endl;
        debug() << "[MESH]:      Specular Shininess  : "
                << material.Shininess << endl;

        // check for # textures
        int textures = 0;
        for (irr::u32 ti = 0; ti < MATERIAL_MAX_TEXTURES; ti++)
            if (material.getTexture(ti) != nullptr)
                textures++;
        debug() << "[MESH]:      # of textures       : " << textures << endl;
    }
}

std::wstring Utility::parentOfPath(const wstring& path)
{
    std::wstring ret = L".";
    if (path == L".") {
        ret = L"..";
    } else {
        std::wstring::size_type lastSlashPos = path.find_last_of(L"/");
        if (lastSlashPos == std::wstring::npos) {
            lastSlashPos = path.find_last_of(L"\\");
        }
        if (lastSlashPos != std::wstring::npos) {
            ret = path.substr(0, lastSlashPos);
        }
    }
    return ret;
}

wstring Utility::basename(const wstring& path)
{
    std::wstring ret = path;
    std::wstring::size_type lastSlashPos = path.find_last_of(L"/");
    if (lastSlashPos == std::wstring::npos) {
        lastSlashPos = path.find_last_of(L"\\");
    }
    if (lastSlashPos != std::wstring::npos) {
        ret = path.substr(lastSlashPos + 1);
    }
    return ret;
}

wstring Utility::withoutExtension(const wstring& path)
{
    std::wstring ret = path;
    std::wstring::size_type lastDotPos = path.find_last_of(L".");
    if (lastDotPos != std::wstring::npos) {
        std::wstring::size_type lastSlashPos = path.find_last_of(L"/");
        if (lastSlashPos == std::wstring::npos) {
            lastSlashPos = path.find_last_of(L"\\");
        }
        if (lastSlashPos != std::wstring::npos) {
            if (lastDotPos > lastSlashPos)
                ret = path.substr(0, lastDotPos);
        } else
            ret = path.substr(0, lastDotPos);
    }
    return ret;
}

wstring Utility::extensionOf(const wstring& path)
{
    std::wstring ret = L"";
    std::wstring::size_type lastDotPos = path.find_last_of(L".");
    if (lastDotPos != std::wstring::npos) {
        std::wstring::size_type lastSlashPos = path.find_last_of(L"/");
        if (lastSlashPos == std::wstring::npos) {
            lastSlashPos = path.find_last_of(L"\\");
        }
        if (lastSlashPos != std::wstring::npos) {
            if (lastDotPos > lastSlashPos)
                ret = path.substr(lastDotPos + 1);
        } else
            ret = path.substr(lastDotPos + 1);
    }
    return ret;
}

wstring Utility::delimiter(const wstring& path)
{
    std::wstring ret = L"/";
    std::wstring::size_type lastSlashPos = path.find_last_of(L"/");
    if (lastSlashPos == std::wstring::npos) {
        // ret = L"/";
    } else {
        std::wstring::size_type lastSlashPos = path.find_last_of(L"\\");
        if (lastSlashPos != std::wstring::npos) {
            ret = L"\\";
        }
    }
    return ret;
}

bool Utility::isFile(const std::string& name)
{
    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

std::string Utility::toString(const std::wstring& ws)
{
    std::string ret;
    if (ws.length() > 0) {
        // std::string str = "Hello";
        ret = std::string(ws.length(), L' '); // Make room for characters
        // Copy string to wstring.
        std::copy(ws.begin(), ws.end(), ret.begin());
    }
    return ret;

    // below sometimes results in "internal_utf8_loop_single: Assertion
    //   `inptr - bytebuf > (state->__count & 7)' failed."
    //   on the converter.out call:
    // if (ws.length() > 0) {
    // // convert to w_string using locale: see Phillipp on
    // // <https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string>
    // std::setlocale(LC_ALL, "");
    // const std::locale locale("");
    // typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
    // const converter_type& converter = std::use_facet<converter_type>(locale);
    // std::vector<char> to(ws.length() * converter.max_length());
    // std::mbstate_t state;
    // const wchar_t* from_next = nullptr;
    // char* to_next = nullptr;
    // const converter_type::result result = converter.out(
    //     state, ws.data(),
    //     ws.data() + ws.length(),
    //     from_next,
    //     &to[0],
    //     &to[0] + to.size(),
    //     to_next
    // );
    // if (result == converter_type::ok or result == converter_type::noconv) {
    //     const std::string s(&to[0], to_next);
    //     // std::cout <<"std::string =     "<<s<<std::endl;
    //     ret = s;
    // }
    //return ret;
}

std::string Utility::toLower(const std::string& s)
{
    std::string ret = s;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    return ret;
}

wstring Utility::toLower(const wstring& s)
{
    wstring ret = s;
    std::transform(ret.begin(), ret.end(), ret.begin(), towlower);
    return ret;
}

wstring Utility::toWstring(irr::f32 val)
{
    return std::to_wstring(val);
}

wstring Utility::toWstring(int val)
{
    return std::to_wstring(val);
}

wstring Utility::toWstring(const std::string& str)
{
    std::wstring ret;
    if (str.length() > 0) {
        // std::string str = "Hello";
        ret = std::wstring(str.length(), L' '); // Make room for characters
        // Copy string to wstring.
        std::copy(str.begin(), str.end(), ret.begin());
    }
    return ret;
}

std::string Utility::dateTimePathString(const time_t& rawtime)
{
    // see http://www.cplusplus.com/reference/ctime/strftime/
    std::string ret = "";
    struct tm * timeinfo;
    char buffer[80];
    timeinfo = localtime (&rawtime);
    strftime (buffer,80,"%F_%H%M%S",timeinfo);
    // %F is same as %Y-%m-%d (zero-padded)
    ret.assign(buffer);
    return ret;
}

std::string Utility::dateTimeNowPathString()
{
    time_t rawtime;
    time (&rawtime);
    return dateTimePathString(rawtime);
}

irr::f32 Utility::toF32(wstring val)
{
    std::wstringstream ss(val);
    irr::f32 ret = 0;
    ss >> ret;
    return ret;
}

irr::f32 Utility::distance(const vector3df &start, const vector3df &end)
{
    vector3df offsetVec3(end.X - start.X, end.Y - start.Y, end.Z - start.Z);
    return offsetVec3.getLength();
}

bool Utility::isFile(const std::wstring& name)
{
    std::string name_s = toString(name);
    if (FILE* file = fopen(name_s.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

std::string Utility::toString(irr::f32 val)
{
    return std::to_string(val);
}

// don't do late instantiation (see header file)
// template<typename T>
// bool Utility::equalsApprox(T f1, T f2)
// {
//     return abs(f2-f1) < .00000001;  // TODO: kEpsilon? (see also
//     // <https://en.wikipedia.org/wiki/Machine_epsilon#How_to_determine_machine_epsilon>)
// }
