/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ConfigurationManager.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Singleton configuration manager implementation
 *
 ****************************************************************************/

#include "../inc/ConfigurationManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

ConfigurationManager& ConfigurationManager::getInstance() {
    static ConfigurationManager instance;
    return instance;
}

global::RC_t ConfigurationManager::loadFromFile(const string& configFile) {
    lock_guard<mutex> lock(m_configMutex);
    
    ifstream file(configFile);
    if (!file.is_open()) {
        cerr << "ERROR: Cannot open configuration file: " << configFile << endl;
        return global::RC_ERROR_OPEN;
    }
    
    string line;
    ConfigCategory currentCategory = ConfigCategory::SYSTEM;
    
    while (getline(file, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for category header [CATEGORY]
        if (line[0] == '[' && line.back() == ']') {
            string categoryStr = line.substr(1, line.length() - 2);
            currentCategory = stringToCategory(categoryStr);
            continue;
        }
        
        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos == string::npos) {
            continue;
        }
        
        string key = line.substr(0, equalPos);
        string valueStr = line.substr(equalPos + 1);
        
        // Remove whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        valueStr.erase(0, valueStr.find_first_not_of(" \t"));
        valueStr.erase(valueStr.find_last_not_of(" \t") + 1);
        
        // Parse value based on type
        ConfigValue value;
        if (valueStr == "true" || valueStr == "false") {
            value = ConfigValue(valueStr == "true");
        } else if (valueStr.find('.') != string::npos) {
            try {
                value = ConfigValue(stof(valueStr));
            } catch (...) {
                value = ConfigValue(valueStr);
            }
        } else {
            try {
                value = ConfigValue(stoi(valueStr));
            } catch (...) {
                value = ConfigValue(valueStr);
            }
        }
        
        m_configuration[currentCategory][key] = value;
    }
    
    file.close();
    return global::RC_SUCCESS;
}

global::RC_t ConfigurationManager::saveToFile(const string& configFile) {
    lock_guard<mutex> lock(m_configMutex);
    
    ofstream file(configFile);
    if (!file.is_open()) {
        cerr << "ERROR: Cannot create configuration file: " << configFile << endl;
        return global::RC_ERROR_OPEN;
    }
    
    file << "# Pedestrian Detection System Configuration\n";
    file << "# Generated automatically\n\n";
    
    for (const auto& categoryPair : m_configuration) {
        file << "[" << categoryToString(categoryPair.first) << "]\n";
        
        for (const auto& keyValuePair : categoryPair.second) {
            file << keyValuePair.first << " = ";
            
            switch (keyValuePair.second.type) {
                case ConfigValueType::INTEGER:
                    file << keyValuePair.second.intValue;
                    break;
                case ConfigValueType::FLOAT:
                    file << keyValuePair.second.floatValue;
                    break;
                case ConfigValueType::BOOLEAN:
                    file << (keyValuePair.second.boolValue ? "true" : "false");
                    break;
                case ConfigValueType::STRING:
                default:
                    file << keyValuePair.second.stringValue;
                    break;
            }
            file << "\n";
        }
        file << "\n";
    }
    
    file.close();
    return global::RC_SUCCESS;
}

global::RC_t ConfigurationManager::setValue(ConfigCategory category, const string& key, const ConfigValue& value) {
    if (key.empty()) {
        return global::RC_ERROR_BAD_PARAM;
    }
    
    lock_guard<mutex> lock(m_configMutex);
    m_configuration[category][key] = value;
    return global::RC_SUCCESS;
}

ConfigValue ConfigurationManager::getValue(ConfigCategory category, const string& key, const ConfigValue& defaultValue) const {
    lock_guard<mutex> lock(m_configMutex);
    
    auto categoryIt = m_configuration.find(category);
    if (categoryIt != m_configuration.end()) {
        auto keyIt = categoryIt->second.find(key);
        if (keyIt != categoryIt->second.end()) {
            return keyIt->second;
        }
    }
    
    return defaultValue;
}

bool ConfigurationManager::hasKey(ConfigCategory category, const string& key) const {
    lock_guard<mutex> lock(m_configMutex);
    
    auto categoryIt = m_configuration.find(category);
    if (categoryIt != m_configuration.end()) {
        return categoryIt->second.find(key) != categoryIt->second.end();
    }
    
    return false;
}

global::RC_t ConfigurationManager::removeKey(ConfigCategory category, const string& key) {
    lock_guard<mutex> lock(m_configMutex);
    
    auto categoryIt = m_configuration.find(category);
    if (categoryIt != m_configuration.end()) {
        auto keyIt = categoryIt->second.find(key);
        if (keyIt != categoryIt->second.end()) {
            categoryIt->second.erase(keyIt);
            return global::RC_SUCCESS;
        }
    }
    
    return global::RC_ERROR_NOT_MATCH;
}

void ConfigurationManager::clear() {
    lock_guard<mutex> lock(m_configMutex);
    m_configuration.clear();
}

vector<string> ConfigurationManager::getKeys(ConfigCategory category) const {
    lock_guard<mutex> lock(m_configMutex);
    vector<string> keys;
    
    auto categoryIt = m_configuration.find(category);
    if (categoryIt != m_configuration.end()) {
        for (const auto& keyValuePair : categoryIt->second) {
            keys.push_back(keyValuePair.first);
        }
    }
    
    return keys;
}

string ConfigurationManager::categoryToString(ConfigCategory category) const {
    switch (category) {
        case ConfigCategory::DETECTION: return "DETECTION";
        case ConfigCategory::CAMERA: return "CAMERA";
        case ConfigCategory::COMMUNICATION: return "COMMUNICATION";
        case ConfigCategory::SYSTEM: return "SYSTEM";
        case ConfigCategory::LOGGING: return "LOGGING";
        default: return "UNKNOWN";
    }
}

ConfigCategory ConfigurationManager::stringToCategory(const string& categoryStr) const {
    string upperStr = categoryStr;
    transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    
    if (upperStr == "DETECTION") return ConfigCategory::DETECTION;
    if (upperStr == "CAMERA") return ConfigCategory::CAMERA;
    if (upperStr == "COMMUNICATION") return ConfigCategory::COMMUNICATION;
    if (upperStr == "SYSTEM") return ConfigCategory::SYSTEM;
    if (upperStr == "LOGGING") return ConfigCategory::LOGGING;
    
    return ConfigCategory::SYSTEM;
}