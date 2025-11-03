#include "server_manager.hpp"

namespace fs = std::filesystem;
logger _logger;

WebServerInfo ServerManager::detectWebServers() {
    WebServerInfo info;
    fs::path xamppConfig;
    std::string xamppPath;
    fs::path wampApache = "C:/wamp64/bin/apache";
    fs::path mampConfig = "/Applications/MAMP/conf/apache/extra/httpd-vhosts.conf";

    #ifdef WIN32
        xamppConfig = "C:/xampp/apache/conf/extra/httpd-vhosts.conf";
        xamppPath = "C:/xampp";
    #elif __APPLE__
        xamppConfig = "/Applications/XAMPP/etc/extra/httpd-vhosts.conf";
        xamppPath = "/Applications/XAMPP/";
    #else
        xamppConfig = ""; //TODO
    #endif

    if (fs::exists(xamppConfig)) {
        info.xamppFound = true;
        info.xamppPath = xamppPath;
        info.xamppConfigPath = xamppConfig.string();
    }

    // simply returns not found on other platforms
    if (fs::exists("C:/wamp/bin/apache")) {
        wampApache = "C:/wamp/bin/apache";
    }

    if (fs::exists(wampApache)) {
        for (const auto& entry : fs::directory_iterator(wampApache)) {
            if (entry.is_directory()) {
                fs::path vhostPath = entry.path() / "conf" / "extra" / "httpd-vhosts.conf";
                if (fs::exists(vhostPath)) {
                    info.wampFound = true;
                    info.wampPath = "C:/wamp64";
                    info.wampConfigPath = vhostPath.string();
                    break;
                }
            }
        }
    }

    //macOS only for now
    if (fs::exists(mampConfig)) {
        info.mampFound = true;
        info.mampPath = "/Applications/MAMP/";
        info.mampConfigPath = mampConfig.string();
    }

    return info;
}

bool ServerManager::restartApache(const WebServerInfo& info, std::string& placeholderText) {
    //TODO: fix linux ver of mamp, hardcoded to macOS rn.
    if(info.mampFound) {
        const char* stopScript = "/Applications/MAMP/bin/stopApache.sh";
        const char* startScript = "/Applications/MAMP/bin/startApache.sh";

        int stopResult = std::system(stopScript);
        if (stopResult != 0) {
            _logger.error("Failed to stop Apache. Exit code: " + std::to_string(stopResult));
        }

        int startResult = std::system(startScript);
        if (startResult != 0) {
            _logger.error("Failed to start Apache. Exit code: " + std::to_string(startResult));
            return false;
        }

        _logger.success("Apache successfully restarted via MAMP.");
        return true;
    }

    if (info.xamppFound) {
        #ifdef _WIN32
            service_helper serviceHelper;

            bool stopSuccess = serviceHelper.stopService(info.xamppServiceName);
            bool startSuccess = serviceHelper.startService(info.xamppServiceName);

            if (!stopSuccess) {
                placeholderText += "Failed to stop Apache (Xampp)!\n";
                _logger.error("Failed to stop Apache (Xampp). Exit code: " + std::to_string(GetLastError()));
                return false;
            }

            if (!startSuccess) {
                placeholderText += "Failed to start Apache (Xampp)!\n";
                _logger.error("Failed to start Apache (Xampp). Exit code: " + std::to_string(GetLastError()));
                return false;
            }

            return true;
        #endif

        #ifdef __APPLE__
            const char* stopScript = "/Applications/XAMPP/bin/stopApache.sh";
            const char* startScript = "/Applications/XAMPP/bin/startApache.sh";

            int stopCode = std::system(stopScript);
            int startCode = std::system(startScript);

            if (stopCode != 0) {
                _logger.error("Failed to stop Apache (XAMPP)!");
                return false;
            }

            if (startCode != 0) {
                _logger.error("Failed to start Apache (XAMPP)!");
                return false;
            }

            return true;
        #endif

        return false;
    }

#ifdef _WIN32
    if (info.wampFound) {
        service_helper serviceHelper;

        bool stopSuccess = serviceHelper.stopService(info.wampServiceName);
        bool startSuccess = serviceHelper.startService(info.wampServiceName);

        if (!stopSuccess) {
            placeholderText += "Failed to stop Apache (Wamp)!\n";
            _logger.error("Failed to stop Apache (Wamp). Exit code: " + std::to_string(GetLastError()));
            return false;
        }

        if (!startSuccess) {
            placeholderText += "Failed to start Apache (Wamp)!\n";
            _logger.error("Failed to start Apache (Wamp). Exit code: " + std::to_string(GetLastError()));
            return false;
        }

        return true;
    }
#endif
    return false;
}

std::string ServerManager::createHost(const WebServerInfo &info) {
    Config config;
    std::string resultMessages;

    if (info.xamppFound) {
        bool success = config.writeToConfig(XAMPP_ID, info, projectName, projectDirectory);
        if (success) {
            resultMessages += "Success: Custom vhost written to XAMPP config!\n";
        } else {
            resultMessages += "Failure: Writing to XAMPP config!\n";
        }
    }

    if (info.wampFound) {
        bool success = config.writeToConfig(WAMP_ID, info, projectName, projectDirectory);
        if (success) {
            resultMessages += "Success: Custom vhost written to WAMP config!\n";
        } else {
            resultMessages += "Failure: Writing to WAMP config!\n";
        }
    }

    if (info.mampFound) {
        bool success = config.writeToConfig(MAMP_ID, info, projectName, projectDirectory);
        if (success) {
            resultMessages += "Success: Custom vhost written to MAMP config!\n";
        } else {
            resultMessages += "Failure: Writing to MAMP config!\n";
        }
    }

    return resultMessages;
}

bool Config::writeToConfig(int serverID, const WebServerInfo& info, const std::string & serverName, const std::string& documentRoot) {
    std::string normalizedDocRoot = documentRoot;
    std::string normalizedDirPath = documentRoot;

   if (serverID == XAMPP_ID) {
       _logger.success("we f ound em");
        if (!info.xamppFound) return false;
        stackConfigPath = info.xamppConfigPath;
        vHostPort = "80";
    } else if (serverID == WAMP_ID) {
        if (!info.wampFound) return false;
        stackConfigPath = info.wampConfigPath;
        vHostPort = "80";
    } else if (serverID == MAMP_ID) {
        if (!info.mampFound) return false;
        stackConfigPath = info.mampConfigPath;
        vHostPort = "8888";
    } else {
        return false;
    }

#ifdef _WIN32
    std::replace(normalizedDocRoot.begin(), normalizedDocRoot.end(), '\\', '/');
    std::replace(normalizedDirPath.begin(), normalizedDirPath.end(), '\\', '/');
#endif

    if (!normalizedDirPath.empty() && normalizedDirPath.back() != '/') {
        normalizedDirPath += '/';
    }

        // duplicate check
    std::ifstream configIn(stackConfigPath);
    std::string configLine;
    bool vhostExists = false;

    while (std::getline(configIn, configLine)) {
        if (configLine.find("ServerName " + serverName) != std::string::npos) {
            vhostExists = true;
            break;
        }
    }
    configIn.close();

    if (vhostExists) {
        _logger.error("vhost already exists");
        return false;
    }

    std::string vhostConfig = R"(
<VirtualHost *:)" + vHostPort + R"(>
    ServerName )" + serverName + R"(
    DocumentRoot ")" + normalizedDocRoot + R"("
    <Directory ")" + normalizedDirPath + R"(">
        AllowOverride All
        Require all granted
    </Directory>
</VirtualHost>
)";

#ifdef _WIN32
    std::string hostPath = R"(C:\Windows\System32\drivers\etc\hosts)";
#elif defined(__APPLE__) || defined(__linux__)
    std::string hostPath = R"(/etc/hosts)";
#endif

    // dupe check
    std::ifstream hostsIn(hostPath);
    std::string hostLine;
    bool hostExists = false;
    while (std::getline(hostsIn, hostLine)) {
        if (hostLine.find("127.0.0.1 " + serverName) != std::string::npos) {
            hostExists = true;
            break;
        }
    }
    hostsIn.close();

    if (!hostExists) {
        std::ofstream hostsFile(hostPath, std::ios::app);

        if (!hostsFile.is_open()) {
            _logger.error("config writer: couldnt open log file");
            return false;
        }

        hostsFile << "127.0.0.1 " << serverName << "\n";
        hostsFile.close();
    }

    std::ofstream configFile(stackConfigPath, std::ios::app);
    if (!configFile.is_open()) {
        _logger.error("config writer: couldnt open log file");
        return false;
    }

    configFile << vhostConfig << "\n";
    configFile.close();
    return true;
}
