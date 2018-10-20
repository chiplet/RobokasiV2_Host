//
// Created by Lehdari on 2018-10-10.
//

#include <GL/gl3w.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <libserialport.h>

#include "SDLApp.hpp"

#include "hwio/Hwio.hpp"

SerialConfigGui::SerialConfigGui(hwio::Hwio& hwio) :
    _ports(nullptr),
    _port_idx(0),
    _port_baud(115200),
    _hwio(hwio)
{
    sp_list_ports(&_ports);
}

SerialConfigGui::~SerialConfigGui()
{
    /* TODO */
}

void SerialConfigGui::render()
{

    ImGui::Begin("Serial Config");

    if (ImGui::Button("Refresh ports"))
        sp_list_ports(&_ports);

    if (ImGui::BeginCombo("Device", sp_get_port_name(_ports[_port_idx]))) {
        if (_ports) {
            for (size_t i = 0; _ports[i]; ++i) {
                if (ImGui::Selectable(sp_get_port_name(_ports[i]), i == _port_idx))
                    _port_idx = i;
                if (i == _port_idx)
                    ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::InputInt("Baud rate", &_port_baud);

    if (ImGui::Button("Connect"))
        _hwio.connect(sp_get_port_name(_ports[_port_idx]), _port_baud);


    ImGui::End();
}



SDLApp::SDLApp(const SDLApp::Settings &settings) :
    _settings       (settings),
    _window         (nullptr),
    _quit           (false),
    _hwio           (),
    _serialConfigGui(_hwio)
{
    int err;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: Could not initialize SDL!\n");
        return;
    }

    _window = SDL_CreateWindow(
         _settings.windowName.c_str(),
         SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED,
         (int)_settings.windowWidth,
         (int)_settings.windowHeight,
         SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (_window == nullptr) {
        printf("Error: SDL Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, _settings.glContextMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, _settings.glContextMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, _settings.glContextFlags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, _settings.glProfileMask);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, _settings.glDoubleBuffer);

    _glCtx = SDL_GL_CreateContext(_window);

    if (_glCtx == nullptr) {
        printf("Error: SDL OpenGL context could not be created! SDL_Error: %s\n",
               SDL_GetError());
        return;
    }

    err = gl3wInit();
    if (err) {
        printf("Error: gl3wInit failed\n");
        return;
    }

    ImGui::CreateContext();
    ImGuiIO &imgui_io = ImGui::GetIO();
    ImGui_ImplSDL2_InitForOpenGL(_window, _glCtx);
    ImGui_ImplOpenGL3_Init("#version 150");
}

SDLApp::~SDLApp()
{
    // Destroy window and quit SDL subsystems
    SDL_GL_DeleteContext(_glCtx);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void SDLApp::loop(void)
{
    // Application main loop
    while (!_quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            handleEvents(event);
        }

        render();
    }
}


void SDLApp::handleEvents(SDL_Event& event)
{
    // Handle SDL events
    switch (event.type) {
        case SDL_QUIT:
            _quit = true;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    _quit = true;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void SDLApp::render(void)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();

    ImGui::Begin("Window");

    ImGui::End();

    _serialConfigGui.render();

    ImGui::Render();

    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(_window);
}
