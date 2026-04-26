#pragma once
#include <string>

struct UpdateInfo
{
    bool updateAvailable = false;
    std::string latestVersion;
    std::string releaseUrl;
};

class UpdateChecker
{
public:
    UpdateChecker() = delete;

    static UpdateInfo CheckForUpdates(const std::string& repoOwner, const std::string& repoName,
                                      const std::string& currentVersion);
};