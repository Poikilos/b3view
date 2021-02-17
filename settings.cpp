#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm> // std::find

#include <assert.h>

#include "settings.h"
#include "Utility.h"
// #include "Debug.h"

using namespace std;

void Settings::init_default_symbols() {
    this->init(" = ", "# ");
}

void Settings::init(std::string assignmentOperator, std::string commentMark)
{
    this->ao_and_spacing = assignmentOperator;
    this->cm_and_spacing = commentMark;
    this->enable_autosave = true;
}

Settings::Settings()
{
    this->init_default_symbols();
}

Settings::Settings(std::string confPath)
{
    this->init_default_symbols();
    this->path = confPath;
    this->load(confPath);
}

void Settings::clear()
{
    this->section = "";
    this->sections.clear();
    this->table.clear();
}

void Settings::clear_types()
{
    this->types.clear();
}

bool Settings::load(std::string path)
{
    this->section = "";
    this->path = path;
    fstream newfile;
    this->pre = "";
    newfile.open(path,ios::in);
    std::string ao = this->ao_trimmed();
    std::string cm = this->cm_trimmed();
    if (newfile.is_open()){
        std::string line;
        int lineN = 0; // Set this to 1 before use.
        while(getline(newfile, line)) {
            lineN += 1;
            this->pre = this->path + ":" + std::to_string(lineN) + ": "; // must end with space for outputinspector
            line = Utility::trim(line);
            std::size_t signPos = line.find(ao);
            std::string typeStr = "string";
            if ((line.length() >= cm.length()) && (line.substr(0, cm.length()) == cm)) {
                // comment
            }
            else if (line.length() == 0) {
                // blank
            }
            else if (signPos != std::string::npos) {
                std::string name = Utility::trim(line.substr(0, signPos));
                std::string value = Utility::trim(line.substr(signPos+1));
                std::string::size_type iSz;
                std::string::size_type fSz;
                int valueI = std::stoi(value, &iSz);
                float valueF = std::stof(value, &fSz);
                // ^ radix (3rd param) default is 10 (base-10 number system)
                cerr << name << std::endl;
                cerr << "  valueI length: " << iSz << std::endl;
                cerr << "  valueF length: " << fSz << std::endl;
                if (fSz > iSz) {
                    typeStr = "float";
                }
                else if (iSz == value.length()) {
                    typeStr = "int";
                }
                else if (iSz > 0) {
                    cerr << this->pre << "WARNING: The file has a "
                         << typeStr << " for " << name << ", but "
                         << this->types[name] << " was expected."
                         << std::endl;
                }

                this->types[name] = typeStr;
                this->table[name] = value;
                if (this->types.find(name) != this->types.end()) {
                    if (this->types[name] != typeStr) {
                        cerr << this->pre << "WARNING: The file has a "
                             << typeStr << " for " << name << ", but "
                             << this->types[name] << " was expected."
                             << std::endl;
                    }
                }
                if (this->section.length() > 0) {
                    this->sections[name] = this->section;
                }
            }
            else {
                if (
                        (line.length() >= 3)
                        && Utility::startsWith(line, "[")
                        && Utility::endsWith(line, "]")
                ) {
                    std::string section = Utility::trim(line.substr(1, line.length()-2));
                    if (section.length() > 0) {
                        this->section = section;
                    }
                    else {
                        cerr << this->pre << "WARNING: The file has a blank section \""
                             << line << "\" (expected section name)."
                             << std::endl;
                    }
                }
                else {
                    cerr << this->pre << "WARNING: \""
                         << line << "\" is not understood (expected comment with '"
                         << cm <<"' (settable via this->setCM(commentMark)),"
                         << " section enclosed in '[' and ']' or assignment containing '"
                         << ao << "' [settable via this->setAO(assignmentOperator)])."
                         << std::endl;
                }
            }
        }
        newfile.close();
    }
    this->section = "";
    this->pre = "";
}

bool Settings::save(std::string path)
{
    this->path = path;
    ofstream myfile;
    myfile.open(path, ios::out); // default is ios_base::out
    std::map<std::string, std::string>::iterator it;
    std::vector<std::string> sectionNames;
    sectionNames.push_back(" ");
    for (it = this->sections.begin(); it != this->sections.end(); it++) {
        if (std::find(sectionNames.begin(), sectionNames.end(), it->second) == sectionNames.end())
            sectionNames.push_back(it->second);
    }
    // Save each section consecutively, starting with variables that have no section.
    for (std::vector<std::string>::iterator vIt = sectionNames.begin() ; vIt != sectionNames.end(); ++vIt) {
        if (*vIt != " ") {
            myfile << "[" << *vIt << "]" << std::endl;
        }
        for (it = table.begin(); it != table.end(); it++) {
            if (this->sections.find(it->first) != this->sections.end()) {
                if (*vIt == this->sections[it->first]) {
                    myfile << it->first << this->ao_and_spacing << it->second << std::endl;
                }
            }
            else if (*vIt == " ") {
                // ^ " " is the global section
                //   (The variable is not in a section).
                myfile << it->first << this->ao_and_spacing << it->second << std::endl;
            }
        }
    }
    myfile.close();
    return true;
}

bool Settings::save()
{
    if (this->path.length() == 0) {
        throw std::string("There is no path during save().");
    }
    this->save(this->path);
}

string Settings::ao_trimmed()
{
    Utility::trim(this->ao_and_spacing);
}

string Settings::cm_trimmed()
{
    Utility::trim(this->cm_and_spacing);
}

bool Settings::check_type(std::string typeStr, std::string name)
{
    std::string currentTypeStr = get_type_str(name);
    if ((currentTypeStr != "") && (typeStr != currentTypeStr)) {
        cerr << this->pre << "WARNING: settings.set got a(n) "
             << typeStr << " (value \"" << this->table[name] << "\") for "
             << name << ", but expected a(n) "
             << currentTypeStr << "."
             << std::endl;
        return false;
    }
    return true;
}

string Settings::get_type_str(string name)
{
    std::string currentTypeStr = "";
    if (this->types.find(name) != this->types.end()) {
        currentTypeStr = this->types[name];
    }
    return currentTypeStr;
}

bool Settings::exists(string name)
{
    return (this->table.find(name) != this->table.end());
}

void Settings::set_ao_and_spacing(std::string assignmentOperator)
{
    this->ao_and_spacing = assignmentOperator;
}

void Settings::set_cm_and_spacing(std::string commentMark)
{
    this->cm_and_spacing = commentMark;
}

bool Settings::set_all_auto(std::map<std::string, std::string> table)
{
    std::map<std::string, std::string>::iterator it;
    for (it = table.begin(); it != table.end(); it++) {
        this->set_auto(it->first, it->second);
    }
}

void Settings::set_section(std::string sectionName)
{
    this->section = sectionName;
}

void Settings::set_raw(std::string name, std::string value) {
    std::string section = this->section;
    // In any case below, never change the section of a variable
    // that was already declared.
    if (this->sections.find(name) != this->sections.end()) {
        // Get use its section since it exists.
        section = this->sections[name];
    }
    else if (this->exists(name)) {
        section = "";
    }
    std::string previous = "";
    if (this->table.find(name) != this->table.end()) {
        previous = this->table[name];
    }
    this->table[name] = value;
    if (section.length() > 0) {
        this->sections[name] = section;
    }
    if (this->enable_autosave) {
        if (previous != value) {
            if (this->path.length() > 0) {
                this->save();
            }
        }
    }
}

bool Settings::set(std::string name, std::string value)
{
    bool match = this->check_type("string", name);
    set_raw(name, value);
    if (this->types.find(name) == this->types.end()) {
        this->types[name] = "string";
    }
    return match;
}

bool Settings::set_int(std::string name, int value)
{
    bool match = this->check_type("int", name);
    this->set_raw(name, std::to_string(value));
    if (this->types.find(name) == this->types.end()) {
        this->types[name] = "int";
    }
    return match;
}

bool Settings::set_float(std::string name, irr::f32 value)
{
    bool match = this->check_type("float", name);
    this->set_raw(name, std::to_string(value));
    if (this->types.find(name) == this->types.end()) {
        this->types[name] = "float";
    }
    return match;
}

void Settings::set_auto(std::string name, std::string value)
{
    std::string typeStr = "string";
    if (this->types.find(name) != this->types.end()) {
        std::string typeStr = this->types[name];
    }
    if (typeStr == "int") {
        std::string::size_type iSz;
        int valueI = std::stoi(value, &iSz);
        this->set_int(name, valueI);
    }
    else if (typeStr == "float") {
        std::string::size_type fSz;
        float valueF = std::stof(value, &fSz);
        this->set_float(name, valueF);
    }
    else {
        // Treat typeStr as string if blank
        // (implement more types above to avoid errors in
        // "set" which always assumes that string is the type).
        this->set(name, value);
    }
}

string Settings::get(string name, bool& found)
{
    bool match = this->check_type("string", name);
    if (this->table.find(name) == this->table.end()) {
        found = false;
        return "";
    }
    found = true;
    return this->table[name];
}

float Settings::get_float(string name, bool &found)
{
    bool match = this->check_type("float", name);
    if (this->table.find(name) == this->table.end()) {
        found = false;
        return 0.0f;
    }
    found = true;
    size_t sz;
    std::string value = this->table[name];
    float v = std::stof(value, &sz);
    if (sz != name.length()) {
        cerr << this->pre << "WARNING: only \""
             << value.substr(0, sz) << "\" part of \""
             << value << "\" for the variable named \""
             << name << "\" is a number."
             << endl;
    }
    return v;
}

int Settings::get_int(string name, bool &found)
{
    bool match = this->check_type("int", name);
    if (this->table.find(name) == this->table.end()) {
        found = false;
        return 0;
    }
    found = true;
    size_t sz;
    std::string value = this->table[name];
    int v = std::stoi(value, &sz);
    if (sz != name.length()) {
        cerr << this->pre << "WARNING: only \""
             << value.substr(0, sz) << "\" part of \""
             << value << "\" for the variable named \""
             << name << "\" is a number."
             << endl;
    }
    return v;
}




///////////////////////// TESTS /////////////////////////

TestSettings::TestSettings() {
    cerr << "TestSettings..." << std::flush;
    assert_type_warning_str_then_int();
    assert_set("what", "bluedragon");
    assert_set("what", "greendragon");
    assert_get("what", "greendragon");
    assert_section_set_on_create();
    cerr << "OK" << std::endl;
}

void TestSettings::assert_equal(const std::string subject, const std::string expectedResult)
{
    if (subject != expectedResult) {
        cerr << "The test expected \"" << expectedResult << "\" but got \"" << subject << "\"" << std::endl;
    }
    assert(subject == expectedResult);
}

void TestSettings::assert_set(const std::string &name, const std::string &value)
{
    settings.set(name, value);
    bool found = false;
    std::string result = settings.get(name, found);
    assert_equal(result, value);
    assert(found);
}

void TestSettings::assert_get(const std::string &name, const std::string &expectedResult)
{
    bool found = false;
    std::string result = settings.get(name, found);
    assert(found);
    assert_equal(result, expectedResult);
}

void TestSettings::assert_type_warning_str_then_int()
{
    settings.clear();
    settings.set("username", "Poikilos");
    bool match = settings.set_int("username", 1);
    assert(!match);
    cerr << "(The warning above is expected during the test: expected string, got int)" << endl;
}

void TestSettings::assert_section_set_on_create()
{
    std::string tmpPath = "test.conf";
    settings.clear();
    settings.set_int("a", 1);
    settings.set_section("more");
    settings.set_int("b", 2);
    settings.set_int("a", 3);
    settings.save(tmpPath);
    settings.clear();
    ifstream myfile(tmpPath);
    if (myfile.is_open())
    {
        std::string line;
        assert(getline(myfile,line));
        assert_equal(line, "a = 3");
        assert(getline(myfile,line));
        assert_equal(line, "[more]");
        assert(getline(myfile,line));
        assert_equal(line, "b = 2");
        myfile.close();
    }
}

static TestSettings testsettings;  // Run tests (Creating the first instance runs the static constructor).
