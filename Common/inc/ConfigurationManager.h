/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : ConfigurationManager.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Singleton configuration manager for application settings
 *
 ****************************************************************************/

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include "global.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

/**
 * @brief Configuration categories
 */
enum class ConfigCategory { DETECTION, CAMERA, COMMUNICATION, SYSTEM, LOGGING };

/**
 * @brief Configuration value types
 */
enum class ConfigValueType { INTEGER, FLOAT, STRING, BOOLEAN };

/**
 * @brief Configuration value container
 */
struct ConfigValue {
    ConfigValueType type;
    std::string stringValue;
    int intValue;
    float floatValue;
    bool boolValue;

    ConfigValue()
        : type(ConfigValueType::STRING), stringValue(""), intValue(0), floatValue(0.0f),
          boolValue(false) {}
    explicit ConfigValue(const std::string& val)
        : type(ConfigValueType::STRING), stringValue(val), intValue(0), floatValue(0.0f),
          boolValue(false) {}
    explicit ConfigValue(int val)
        : type(ConfigValueType::INTEGER), stringValue(""), intValue(val), floatValue(0.0f),
          boolValue(false) {}
    explicit ConfigValue(float val)
        : type(ConfigValueType::FLOAT), stringValue(""), intValue(0), floatValue(val),
          boolValue(false) {}
    explicit ConfigValue(bool val)
        : type(ConfigValueType::BOOLEAN), stringValue(""), intValue(0), floatValue(0.0f),
          boolValue(val) {}
};

/**
 * @brief Singleton configuration manager using Singleton pattern
 */
class ConfigurationManager {
  public:
    /**
     * @brief Get singleton instance
     * @return ConfigurationManager& Reference to singleton instance
     */
    static ConfigurationManager& getInstance();

    /**
     * @brief Load configuration from file
     * @param configFile Path to configuration file
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t loadFromFile(const std::string& configFile);

    /**
     * @brief Save configuration to file
     * @param configFile Path to configuration file
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t saveToFile(const std::string& configFile);

    /**
     * @brief Set configuration value
     * @param category Configuration category
     * @param key Configuration key
     * @param value Configuration value
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t setValue(ConfigCategory category, const std::string& key,
                          const ConfigValue& value);

    /**
     * @brief Get configuration value
     * @param category Configuration category
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return ConfigValue Configuration value or default
     */
    ConfigValue getValue(ConfigCategory category, const std::string& key,
                         const ConfigValue& defaultValue = ConfigValue()) const;

    /**
     * @brief Check if configuration key exists
     * @param category Configuration category
     * @param key Configuration key
     * @return bool True if key exists
     */
    bool hasKey(ConfigCategory category, const std::string& key) const;

    /**
     * @brief Remove configuration key
     * @param category Configuration category
     * @param key Configuration key
     * @return global::RC_t Return code indicating success or failure
     */
    global::RC_t removeKey(ConfigCategory category, const std::string& key);

    /**
     * @brief Clear all configuration values
     */
    void clear();

    /**
     * @brief Get all keys for a category
     * @param category Configuration category
     * @return std::vector<std::string> List of keys
     */
    std::vector<std::string> getKeys(ConfigCategory category) const;

    // Deleted copy constructor and assignment operator for singleton
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;

  private:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;

    mutable std::mutex m_configMutex;
    std::map<ConfigCategory, std::map<std::string, ConfigValue>> m_configuration;

    std::string categoryToString(ConfigCategory category) const;
    ConfigCategory stringToCategory(const std::string& categoryStr) const;
};

// Convenience macros for common configuration access
#define CONFIG_MGR ConfigurationManager::getInstance()
#define GET_CONFIG(category, key, defaultVal) CONFIG_MGR.getValue(category, key, defaultVal)
#define SET_CONFIG(category, key, val) CONFIG_MGR.setValue(category, key, val)

#endif /* CONFIGURATIONMANAGER_H */