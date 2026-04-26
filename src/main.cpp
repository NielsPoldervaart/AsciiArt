#include "CliApp.h"
#include "GuiApp.h"

int main(const int argc, char* argv[])
{
    if (argc > 1)
    {
        return RunCliApp(argc, argv);
    }

    return RunGuiApp();
}