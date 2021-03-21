#ifndef UTILS_H
#define UTILS_H

#include <irrlicht/irrlicht.h>

#include <ctime>
#include <string>
#include <vector>

const char path_separator_s =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif
const wchar_t path_separator_ws =
#ifdef _WIN32
                            L'\\';
#else
                            L'/';
#endif

class Utility {
public:
    static const std::string WHITESPACE;
    static void dumpVectorToConsole(const irr::core::vector3df& vector);
    static int getTextureCount(const irr::video::SMaterial& material);
    static int getTextureCount(irr::scene::IAnimatedMeshSceneNode* node);
    static void dumpMeshInfoToConsole(irr::scene::IAnimatedMeshSceneNode* node);
    static std::wstring parentOfPath(const std::wstring& path);
    static std::wstring basename(const std::wstring& path);
    static std::wstring leftOf(const std::wstring& path, const std::wstring& delimiter, bool allIfNotFound);
    static std::wstring leftOfLast(const std::wstring& path, const std::wstring& delimiter, bool allIfNotFound);
    static std::wstring rightOf(const std::wstring& path, const std::wstring& delimiter, bool allIfNotFound);
    static std::wstring rightOfLast(const std::wstring& path, const std::wstring& delimiter, bool allIfNotFound);
    static bool startsWith(const std::wstring& haystack, const std::wstring& needle);
    static bool endsWith(const std::wstring& haystack, const std::wstring& needle);
    static bool startsWith(const std::string& haystack, const std::string& needle);
    static bool endsWith(const std::string& haystack, const std::string& needle);
    static std::wstring replaceAll(const std::wstring& subject, const std::wstring& from, const std::wstring& to);
    static std::string replaceAll(const std::string& subject, const std::string& from, const std::string& to);
    static std::wstring getPrefix(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI);
    static std::wstring getSuffix(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI);
    static bool startsWithAny(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI);
    static bool endsWithAny(const std::wstring& haystack, const std::vector<std::wstring>& needles, bool CI);
    static std::wstring withoutExtension(const std::wstring& path);
    static std::wstring extensionOf(const std::wstring& path);
    static std::wstring delimiter(const std::wstring& path);
    static bool isFile(const std::string& name);
    static bool isFile(const std::wstring& name);
    static bool is_directory(const std::string& name);
    static bool is_directory(const std::wstring &path);
    static void create_directory(const std::string &path);
    static std::string toString(int val);
    static std::string toString(irr::f32 val);
    static std::string toString(const std::wstring& name);
    static std::string toLower(const std::string& s);
    static std::wstring toLower(const std::wstring& s);
    static std::wstring toWstring(irr::f32 val);
    static std::wstring toWstring(int val);
    static std::wstring toWstring(const std::string& str);
    static std::string dateTimePathString(const time_t& rawtime);
    static std::string dateTimeNowPathString();
    static irr::f32 toF32(std::wstring val);
    static irr::f32 distance(const irr::core::vector3df& start, const irr::core::vector3df& end);
    // compiler doesn't like template function when class is not a template--instantiate immediately
    // see http://processors.wiki.ti.com/index.php/C%2B%2B_Template_Instantiation_Issues
    template <typename T>
    static bool equalsApprox(T f1, T f2)
    {
        return abs(f2 - f1) < .00000001; // TODO: kEpsilon? (see also <https://en.wikipedia.org/wiki/Machine_epsilon#How_to_determine_machine_epsilon>)
    }

    static std::string ltrim(const std::string &s);
    static std::string rtrim(const std::string &s);
    static std::string trim(const std::string &s);
};

class TestUtility {
public:
    TestUtility();
    void assertEqual(const std::wstring& subject, const std::wstring& expectedResult);
    void assertEqual(const std::string subject, const std::string expectedResult);
    void testReplaceAll(const std::wstring& subject, const std::wstring& from, const std::wstring& to, const std::wstring& expectedResult);
    void testReplaceAll(const std::string& subject, const std::string& from, const std::string& to, const std::string& expectedResult);
    void testTrim(const std::string& subject, const std::string &expectedResult);
    void testRTrim(const std::string& subject, const std::string &expectedResult);
    void testLTrim(const std::string& subject, const std::string &expectedResult);
};

#endif // UTILS_H
