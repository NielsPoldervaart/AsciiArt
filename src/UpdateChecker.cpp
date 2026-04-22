#include "UpdateChecker.h"
#include <iostream>
#include <array>
#include <memory>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace
{
    std::string ExecCommand(const char* cmd)
    {
        char buffer[128];
        std::string result;
        const std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(cmd, "r"), PCLOSE);

        if (!pipe) return "";

        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr)
            result += buffer;

        return result;
    }
}

UpdateInfo UpdateChecker::CheckForUpdates(const std::string& repoOwner, const std::string& repoName,
                                          const std::string& currentVersion)
{
    UpdateInfo info;
    info.updateAvailable = false;

    std::string url = "https://api.github.com/repos/" + repoOwner + "/" + repoName + "/releases/latest";
    std::string cmd = "curl -s -H \"User-Agent: AsciiArtApp\" " + url;

    std::string jsonResponse = ExecCommand(cmd.c_str());

    if (jsonResponse.empty() || jsonResponse.find(R"("message":"Not Found")") != std::string::npos)
        return info;

    std::string searchString = R"("tag_name":")";
    if (size_t startPos = jsonResponse.find(searchString); startPos != std::string::npos)
    {
        startPos += searchString.length();

        if (size_t endPos = jsonResponse.find('\"', startPos); endPos != std::string::npos)
        {
            info.latestVersion = jsonResponse.substr(startPos, endPos - startPos);
            info.releaseUrl = "https://github.com/" + repoOwner + "/" + repoName + "/releases/latest";

            if (info.latestVersion != currentVersion)
                info.updateAvailable = true;
        }
    }

    return info;
}