#include "Debug.h"
#include "Utility.h"

#include <algorithm>
#include <clocale>
#include <cmath>
#include <cwctype> // #include <cwtype>
#include <iostream>
#include <filesystem>
#include <locale>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <sys/stat.h>
// NOTE: to use filesystem, you must also include the fs library such
// as via the `-lstdc++fs` linker option -- see b3view.pro
// #include <filesystem>  // requires C++17
#include <experimental/filesystem> // requires C++14 such as gcc 8.2.1


// C++14: namespace filesystem = std::experimental::filesystem;
// namespace fs = std::filesystem;  // doesn't work (not a namespace in gcc's C++17)
// using namespace std::filesystem;  // doesn't work (not a namespace in gcc's C++17)
namespace fs = std::experimental::filesystem;
// namespace fs = std::filesystem;  // doesn't work (not a namespace in gcc's C++17)

using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace std;

const std::string Utility::WHITESPACE = " \n\r\t\f\v";

void Utility::dumpVectorToConsole(const vector3df& vector)
{
    debug() << "X: " << vector.X << " Y: " << vector.Y << " Z: " << vector.Z << endl;
}

int Utility::getTextureCount(const SMaterial& material) {
    int count = 0;
    for (irr::u32 ti = 0; ti < MATERIAL_MAX_TEXTURES; ti++)
        if (material.getTexture(ti) != nullptr)
            count++;
    return count;
}
int Utility::getTextureCount(IAnimatedMeshSceneNode* node) {
    int count = 0;
    for (irr::u32 matIndex = 0; matIndex < node->getMaterialCount(); matIndex++) {
        count += getTextureCount(node->getMaterial(matIndex));
    }
    return count;
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
        debug() << "[MESH]:      # of textures       : " << Utility::getTextureCount(material) << endl;
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

/// Filename without path (may still have extension)
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

/// Get any substring to the right of the first delimiter.
/// allIfNotFound: Return whole string on no delimiter, vs empty string.
wstring Utility::rightOf(const wstring& path, const wstring& delimiter, bool allIfNotFound)
{
    std::wstring ret = allIfNotFound ? path : L"";
    std::wstring::size_type lastPos = path.find(delimiter);
    if (lastPos != std::wstring::npos) {
        ret = path.substr(lastPos + delimiter.length());
    }
    return ret;
}

/// Get any substring to the right of the last delimiter.
/// allIfNotFound: Return whole string on no delimiter, vs empty string.
wstring Utility::rightOfLast(const wstring& path, const wstring& delimiter, bool allIfNotFound)
{
    std::wstring ret = allIfNotFound ? path : L"";
    std::wstring::size_type lastPos = path.find_last_of(delimiter);
    if (lastPos != std::wstring::npos) {
        ret = path.substr(lastPos + delimiter.length());
    }
    return ret;
}

/// Get any substring to the left of the first delimiter.
/// allIfNotFound: Return whole string on no delimiter, vs empty string.
wstring Utility::leftOf(const wstring& path, const wstring& delimiter, bool allIfNotFound)
{
    std::wstring ret = allIfNotFound ? path : L"";
    std::wstring::size_type lastPos = path.find(delimiter);
    if (lastPos != std::wstring::npos) {
        ret = path.substr(0, lastPos);
    }
    return ret;
}

bool Utility::startsWith(const std::wstring& haystack, const std::wstring& needle) {
    bool found = false;
    if (haystack.length() >= needle.length()) {
        if (haystack.substr(0, needle.length())==needle) {
            found = true;
        }
    }
    return found;
}

bool Utility::endsWith(const std::wstring& haystack, const std::wstring& needle) {
    bool found = false;
    if (haystack.length() >= needle.length()) {
        if (haystack.substr(haystack.length()-needle.length())==needle) {
            found = true;
        }
    }
    return found;
}

bool Utility::startsWith(const std::string& haystack, const std::string& needle) {
    bool found = false;
    if (haystack.length() >= needle.length()) {
        if (haystack.substr(0, needle.length())==needle) {
            found = true;
        }
    }
    return found;
}

bool Utility::endsWith(const std::string& haystack, const std::string& needle) {
    bool found = false;
    if (haystack.length() >= needle.length()) {
        if (haystack.substr(haystack.length()-needle.length())==needle) {
            found = true;
        }
    }
    return found;
}

wstring Utility::replaceAll(const wstring &subject, const wstring &from, const wstring &to)
{
    size_t i = 0;
    if (from.length() == 0) {
        return subject;
    }
    wstring result = subject;
    while (i < result.length()) {
        if (result.substr(i, from.length()) == from) {
            result = result.substr(0, i) + to + result.substr(i + from.length());
            i += to.length();
        }
        else {
            i++;
        }
    }
    return result;
}

std::string Utility::replaceAll(const std::string &subject, const std::string &from, const std::string &to)
{
    size_t i = 0;
    if (from.length() == 0) {
        return subject;
    }
    std::string result = subject;
    while (i < result.length()) {
        if (result.substr(i, from.length()) == from) {
            result = result.substr(0, i) + to + result.substr(i + from.length());
            i += to.length();
        }
        else {
            i++;
        }
    }
    return result;
}

bool Utility::startsWithAny(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI) {
    return getPrefix(haystack, needles, CI).length() > 0;
}

bool Utility::endsWithAny(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI) {
    return getSuffix(haystack, needles, CI).length() > 0;
}

std::wstring Utility::getPrefix(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI) {
    if (CI) {
        std::wstring haystackLower = Utility::toLower(haystack);
        for (auto needle : needles) {
            if (Utility::startsWith(haystackLower, Utility::toLower(needle)))
                return needle;
        }
    }
    else {
        for (auto needle : needles) {
            if (Utility::startsWith(haystack, needle))
                return needle;
        }
    }
    return L"";
}

std::wstring Utility::getSuffix(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI) {
    if (CI) {
        std::wstring haystackLower = Utility::toLower(haystack);
        for (auto needle : needles) {
            if (Utility::endsWith(haystackLower, Utility::toLower(needle)))
                return needle;
        }
    }
    else {
        for (auto needle : needles) {
            if (Utility::endsWith(haystack, needle))
                return needle;
        }
    }
    return L"";
}

/// Get any substring to the left of the last delimiter.
/// allIfNotFound: Return whole string on no delimiter, vs empty string.
wstring Utility::leftOfLast(const wstring& path, const wstring& delimiter, bool allIfNotFound)
{
    std::wstring ret = allIfNotFound ? path : L"";
    std::wstring::size_type lastPos = path.find_last_of(delimiter);
    if (lastPos != std::wstring::npos) {
        ret = path.substr(0, lastPos);
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

bool Utility::is_directory(const std::string &path)
{
    return fs::is_directory(fs::status(path));
    /*
    //pre C++17:
    struct stat info;
    if (stat(path, &info) != 0) {
        // TODO: ^ fails on OSX 10.11 according to a comment on the
        // accepted answer on
        // https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
    }
    */
}
bool Utility::is_directory(const std::wstring &path)
{
    return fs::is_directory(fs::status(path));
    /*
    //pre C++17:
    struct stat info;
    if (stat(path, &info) != 0) {
        // TODO: ^ fails on OSX 10.11 according to a comment on the
        // accepted answer on
        // https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
    }
    */
}

void Utility::create_directory(const std::string &path) {
    fs::create_directory(path);
}

std::string Utility::toString(int val)
{
    return std::to_string(val);
    // The only throw is std::bad_alloc according to
    // <https://en.cppreference.com/w/cpp/string/basic_string/to_string>
}

std::string Utility::toString(irr::f32 val)
{
    return std::to_string(val);
    // The only throw is std::bad_alloc according to
    // <https://en.cppreference.com/w/cpp/string/basic_string/to_string>
}

std::string Utility::ltrim(const std::string& s)
{
    // based on https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
    size_t start = s.find_first_not_of(Utility::WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Utility::rtrim(const std::string& s)
{
    // based on https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
    size_t end = s.find_last_not_of(Utility::WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Utility::trim(const std::string& s)
{
    // based on https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
    return rtrim(ltrim(s));
}

// don't do late instantiation (see header file)
// template<typename T>
// bool Utility::equalsApprox(T f1, T f2)
// {
//     return abs(f2-f1) < .00000001;  // TODO: kEpsilon? (see also
//     // <https://en.wikipedia.org/wiki/Machine_epsilon#How_to_determine_machine_epsilon>)
// }
TestUtility::TestUtility() {
    std::cerr << "TestUtility..." << std::flush;
    testReplaceAll(L"***water_dragon***", L"_", L"", L"***waterdragon***");
    testReplaceAll(L"*water_dragon*", L"*", L"***", L"***water_dragon***");

    testReplaceAll(L"***water_dragon***", L"***", L"", L"water_dragon");
    testReplaceAll(L"***water_dragon***", L"", L"***", L"***water_dragon***"); // do nothing
    testLTrim(" apple", "apple");
    testLTrim(" banana  ", "banana  ");
    testRTrim(" cantelope  ", " cantelope");
    testRTrim("dragon fruit ", "dragon fruit");
    testTrim(" elderberry  ", "elderberry");
    testTrim("fig ", "fig");
    testTrim(" grape", "grape");
    testTrim("horned melon ", "horned melon");
    std::cerr << "OK" << std::endl;
}

void TestUtility::testReplaceAll(const wstring &subject, const wstring &from, const wstring &to, const wstring &expectedResult)
{
    this->assertEqual(Utility::replaceAll(subject, from, to), expectedResult);
};
void TestUtility::testReplaceAll(const std::string &subject, const std::string &from, const std::string &to, const std::string &expectedResult)
{
    std::string result = Utility::replaceAll(subject, from, to);
    this->assertEqual(result, expectedResult);
}

void TestUtility::testTrim(const std::string &subject, const std::string &expectedResult)
{
    assertEqual(Utility::trim(subject), expectedResult);
}

void TestUtility::testLTrim(const std::string &subject, const std::string &expectedResult)
{
    assertEqual(Utility::ltrim(subject), expectedResult);
}

void TestUtility::testRTrim(const std::string &subject, const std::string &expectedResult)
{
    assertEqual(Utility::rtrim(subject), expectedResult);
}

void TestUtility::assertEqual(const wstring& subject, const wstring& expectedResult)
{
    if (subject != expectedResult) {
        cerr << "The test expected \"" << Utility::toString(expectedResult) << "\" but got \"" << Utility::toString(subject) << "\"" << std::endl;
    }
    assert(subject == expectedResult);
}
void TestUtility::assertEqual(const std::string subject, const std::string expectedResult)
{
    if (subject != expectedResult) {
        cerr << "The test expected \"" << expectedResult << "\" but got \"" << subject << "\"" << std::endl;
    }
    assert(subject == expectedResult);
}

#ifdef DEBUG
static TestUtility testutility;  // Run tests (Creating the first instance runs the static constructor).
#elif QT_DEBUG
static TestUtility testutility;  // Run tests (Creating the first instance runs the static constructor).
#endif
