#pragma once
#include <iostream>
#include <filesystem>

#include <sentinel/core/logger.h>

#define XAMPP_ID 0
#define WAMP_ID 1
#define MAMP_ID 2

struct WebServerInfo {
    bool xamppFound = false;
    bool wampFound = false;
	bool mampFound = false;	

    std::string xamppPath;
    std::string wampPath;
	std::string mampPath;

    std::string xamppConfigPath;
    std::string wampConfigPath;
	std::string mampConfigPath;

    std::string xamppServiceName = "Apache2.4";
    std::string wampServiceName = "wampapache64";
};

class ServerManager {
    public:
        std::string projectDirectory;
        std::string projectName;
    
        WebServerInfo detectWebServers();
        bool restartApache(const WebServerInfo &info, std::string& placeholderText);
        std::string createHost(const WebServerInfo &info);
};

// dont like this but i need to access this var right here v
class Config {
    public:
        bool writeToConfig(int serverID, const WebServerInfo& info, const std::string& serverName, const std::string& documentRoot);

    private:
        std::string stackConfigPath;
        std::string vHostPort; // because we pass it to a text file
};
