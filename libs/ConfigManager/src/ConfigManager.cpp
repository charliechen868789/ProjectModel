// ConfigManager.cpp
#include "ConfigManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <rapidjson/stringbuffer.h>

bool ConfigManager::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return false;
    }
    
    // Read entire file into string
    std::string jsonString((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    file.close();
    
    loadFromString(jsonString);
    currentConfigFile_ = filename;
    
    if (document_.HasParseError()) {
        std::cerr << "JSON parse error in " << filename << " at offset " 
                  << document_.GetErrorOffset() << ": " 
                  << rapidjson::GetParseError_En(document_.GetParseError()) << std::endl;
        return false;
    }
    
    std::cout << "Configuration loaded from: " << filename << std::endl;
    return true;
}

void ConfigManager::loadFromString(const std::string& jsonString) {
    document_.Parse(jsonString.c_str());
}

bool ConfigManager::saveConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document_.Accept(writer);
    
    file << buffer.GetString();
    file.close();
    
    std::cout << "Configuration saved to: " << filename << std::endl;
    return true;
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    const rapidjson::Value* value = findValue(key);
    if (value && value->IsString()) {
        return value->GetString();
    }
    return defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    const rapidjson::Value* value = findValue(key);
    if (value && value->IsInt()) {
        return value->GetInt();
    }
    return defaultValue;
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    const rapidjson::Value* value = findValue(key);
    if (value && value->IsNumber()) {
        return value->GetDouble();
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    const rapidjson::Value* value = findValue(key);
    if (value && value->IsBool()) {
        return value->GetBool();
    }
    return defaultValue;
}

std::vector<std::string> ConfigManager::getStringArray(const std::string& key) const {
    std::vector<std::string> result;
    const rapidjson::Value* value = findValue(key);
    
    if (value && value->IsArray()) {
        for (rapidjson::SizeType i = 0; i < value->Size(); i++) {
            if ((*value)[i].IsString()) {
                result.push_back((*value)[i].GetString());
            }
        }
    }
    return result;
}

std::string ConfigManager::getString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    const rapidjson::Value* value = findValue(section, key);
    if (value && value->IsString()) {
        return value->GetString();
    }
    return defaultValue;
}

int ConfigManager::getInt(const std::string& section, const std::string& key, int defaultValue) const {
    const rapidjson::Value* value = findValue(section, key);
    if (value && value->IsInt()) {
        return value->GetInt();
    }
    return defaultValue;
}

double ConfigManager::getDouble(const std::string& section, const std::string& key, double defaultValue) const {
    const rapidjson::Value* value = findValue(section, key);
    if (value && value->IsNumber()) {
        return value->GetDouble();
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& section, const std::string& key, bool defaultValue) const {
    const rapidjson::Value* value = findValue(section, key);
    if (value && value->IsBool()) {
        return value->GetBool();
    }
    return defaultValue;
}

std::vector<std::string> ConfigManager::getStringArray(const std::string& section, const std::string& key) const {
    std::vector<std::string> result;
    const rapidjson::Value* value = findValue(section, key);
    
    if (value && value->IsArray()) {
        for (rapidjson::SizeType i = 0; i < value->Size(); i++) {
            if ((*value)[i].IsString()) {
                result.push_back((*value)[i].GetString());
            }
        }
    }
    return result;
}

rapidjson::Value* ConfigManager::getObject(const std::string& key) {
    rapidjson::Value* value = findValue(key);
    return (value && value->IsObject()) ? value : nullptr;
}

const rapidjson::Value* ConfigManager::getObject(const std::string& key) const {
    const rapidjson::Value* value = findValue(key);
    return (value && value->IsObject()) ? value : nullptr;
}

rapidjson::Value* ConfigManager::getArray(const std::string& key) {
    rapidjson::Value* value = findValue(key);
    return (value && value->IsArray()) ? value : nullptr;
}

const rapidjson::Value* ConfigManager::getArray(const std::string& key) const {
    const rapidjson::Value* value = findValue(key);
    return (value && value->IsArray()) ? value : nullptr;
}

bool ConfigManager::hasKey(const std::string& key) const {
    return findValue(key) != nullptr;
}

bool ConfigManager::hasSection(const std::string& section) const {
    if (document_.HasMember(section.c_str())) {
        return document_[section.c_str()].IsObject();
    }
    return false;
}

void ConfigManager::printConfig() const {
    std::cout << "Configuration:" << std::endl;
    std::cout << toString() << std::endl;
}

std::string ConfigManager::toString() const {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document_.Accept(writer);
    return buffer.GetString();
}

const rapidjson::Value* ConfigManager::findValue(const std::string& key) const {
    if (document_.HasMember(key.c_str())) {
        return &document_[key.c_str()];
    }
    return nullptr;
}

rapidjson::Value* ConfigManager::findValue(const std::string& key) {
    if (document_.HasMember(key.c_str())) {
        return &document_[key.c_str()];
    }
    return nullptr;
}

const rapidjson::Value* ConfigManager::findValue(const std::string& section, const std::string& key) const {
    if (document_.HasMember(section.c_str())) {
        const rapidjson::Value& sectionObj = document_[section.c_str()];
        if (sectionObj.IsObject() && sectionObj.HasMember(key.c_str())) {
            return &sectionObj[key.c_str()];
        }
    }
    return nullptr;
}

// Setter implementations (for completeness)
void ConfigManager::setString(const std::string& key, const std::string& value) {
    rapidjson::Value keyValue(key.c_str(), document_.GetAllocator());
    rapidjson::Value stringValue(value.c_str(), document_.GetAllocator());
    document_.AddMember(keyValue, stringValue, document_.GetAllocator());
}

void ConfigManager::setInt(const std::string& key, int value) {
    rapidjson::Value keyValue(key.c_str(), document_.GetAllocator());
    document_.AddMember(keyValue, rapidjson::Value(value), document_.GetAllocator());
}

void ConfigManager::setDouble(const std::string& key, double value) {
    rapidjson::Value keyValue(key.c_str(), document_.GetAllocator());
    document_.AddMember(keyValue, rapidjson::Value(value), document_.GetAllocator());
}

void ConfigManager::setBool(const std::string& key, bool value) {
    rapidjson::Value keyValue(key.c_str(), document_.GetAllocator());
    document_.AddMember(keyValue, rapidjson::Value(value), document_.GetAllocator());
}