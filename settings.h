#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>
#include <irrlicht/irrlicht.h>

class Settings
{
private:
    std::string path;
    std::map<std::string, std::string> table;
    std::map<std::string, std::string> types;
    std::map<std::string, std::string> sections;
    std::string section;
    std::string ao_and_spacing; // assignment operator
    std::string cm_and_spacing; // comment mark
    std::string pre; // debug prefix such as "filename:lineNumber: " (must end with space for outputinspector)
    void init(std::string assignmentOperatorAndSpacing, std::string commentMarkAndSpacing);
    void init_default_symbols();

public:
    bool enable_autosave;

    Settings();
    Settings(std::string confPath);
    void clear();
    void clear_types();
    bool load(std::string path);
    bool save(std::string path);
    bool save();
    std::string ao_trimmed();
    std::string cm_trimmed();
    bool check_type(std::string typeStr, std::string name);
    std::string get_type_str(std::string name);
    bool exists(std::string name);
    void set_ao_and_spacing(std::string assignmentOperator);
    void set_cm_and_spacing(std::string commentMark);
    void set_all_auto(std::map<std::string, std::string> table);
    void set_section(std::string sectionName);
    void set_raw(std::string name, std::string value);
    bool set(std::string name, std::string value);
    bool set_float(std::string name, irr::f32 value);
    bool set_int(std::string name, int value);
    void set_auto(std::string name, std::string value);
    std::string get(std::string name, bool& found);
    float get_float(std::string name, bool& found);
    int get_int(std::string name, bool& found);
};

class TestSettings {
    Settings settings;
public:
    TestSettings();
    void assert_equal(const std::string subject, const std::string expectedResult);
    void assert_set(const std::string& name, const std::string& value);
    void assert_get(const std::string& name, const std::string& expectedResult);
    void assert_type_warning_str_then_int();
    void assert_section_set_on_create();
};

#endif // SETTINGS_H
