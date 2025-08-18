// ConfigManager.h - Using RapidJSON
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>

class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    bool loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename) const;
    void loadFromString(const std::string& jsonString);
    
    // Getters with default values
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getStringArray(const std::string& key) const;
    
    // Nested key support (e.g., "network.port" or section, key)
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& section, const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getStringArray(const std::string& section, const std::string& key) const;
    
    // Object/Array access
    rapidjson::Value* getObject(const std::string& key);
    const rapidjson::Value* getObject(const std::string& key) const;
    rapidjson::Value* getArray(const std::string& key);
    const rapidjson::Value* getArray(const std::string& key) const;
    
    // Setters
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    
    // Check if key exists
    bool hasKey(const std::string& key) const;
    bool hasSection(const std::string& section) const;
    
    // Direct document access for advanced usage
    const rapidjson::Document& getDocument() const { return document_; }
    rapidjson::Document& getDocument() { return document_; }
    
    // Print configuration
    void printConfig() const;
    std::string toString() const;

private:
    ConfigManager() = default;
    
    // Helper functions for nested key access
    const rapidjson::Value* findValue(const std::string& key) const;
    rapidjson::Value* findValue(const std::string& key);
    const rapidjson::Value* findValue(const std::string& section, const std::string& key) const;
    
    rapidjson::Document document_;
    std::string currentConfigFile_;
};
