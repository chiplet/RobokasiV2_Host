#ifndef ROBOKASIV2_GUI_PROGRAMEDITOR_HPP
#define ROBOKASIV2_GUI_PROGRAMEDITOR_HPP

#include "kinematics/Program.hpp"
#include "gui/VisualizerConfig.hpp"
#include "gui/StepTypes.hpp"
#include "gui/DriveControl.hpp"
#include "hwio/SerialProto.hpp"

#include <imgui.h>

namespace gui {

    class ProgramEditor {
    public:
        ProgramEditor(kin::Program& program,
                      hwio::SerialProto& serialProto,
                      gui::DriveControl& driveControl,
                      gui::VisualizerConfig& visualizerConfig);
        void render();
    private:
        void removeStep(size_t i);

        kin::Program&       _program;
        hwio::SerialProto&  _serialProto;
        gui::DriveControl&   _driveControl;

        std::array<float, 6> _angles = {0.0f};
        std::array<float, 6> _poseVisualizer(void);

        gui::VisualizerConfig&  _visualizerConfig;
        size_t                  _visualizerSourceId;

        void _save(const char* path);
        void _load(const char *path);

        /* Pose editor */
        size_t _selectedPoseIdx = 0;

        /* ImGui doesn't deal with std::string */
        char _curPoseName[80] = {'\0'};
        size_t _newPoseID = 0;

        /* Sequence editor */
        int _selectedStepType = StepTypes::LinearDriveStep;

        size_t _selectedStepIdx = 0;

        char _curStepName[80] = {'\0'};
        size_t _newStepID = 0;
    };

}

#endif
