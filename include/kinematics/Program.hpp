#ifndef ROBOKASIV2_KINEMATICS_PROGRAM_HPP
#define ROBOKASIV2_KINEMATICS_PROGRAM_HPP

#include "kinematics/Puma560.hpp"

#include <memory>
#include <vector>
#include <string>

namespace kin {

    struct Puma560StepFrame : public kin::Puma560 {
        Puma560StepFrame() = default;
        Puma560StepFrame(const Puma560& puma, int dt);
        int dt;
    };

    struct ProgramStep {
        ProgramStep(std::string name);
        virtual ~ProgramStep() {}
        virtual std::vector<Puma560StepFrame> generate(const Puma560& poseA,
                                                       const Puma560& poseB) = 0;
        virtual void edit(void) { }
        std::string name;
        size_t endPoseIdx = 0;
    };

    struct ProgramPose {
        Puma560 pose;
        std::string name;
    };

    struct Program {
        template <typename StepType, typename... Args>
        void addStep(Args&&... args)
        {
            steps.emplace_back(std::make_unique<StepType>(std::forward<Args>(args)...));
        }
        std::vector<std::unique_ptr<ProgramStep>> steps;
        std::vector<ProgramPose> poses;
    };

};

#endif
