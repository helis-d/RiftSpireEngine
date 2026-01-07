#pragma once

#include "Application.h"

#ifdef RS_PLATFORM_WINDOWS

extern RiftSpire::Application* RiftSpire::CreateApplication();

int main(int argc, char** argv)
{
    auto app = RiftSpire::CreateApplication();
    app->Run();
    delete app;
    return 0;
}

#endif
