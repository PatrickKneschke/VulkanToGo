#pragma once

#include "timer.h"

#include <functional>
#include <string>
#include <unordered_map>


namespace vktg
{


    class FrameHandler {

        public:

            FrameHandler( const uint8_t frameOverlap);
            
            uint8_t FrameOverlap();
            uint64_t FrameCount();
            uint8_t CurrentFrameIndex();
            double DeltaTime();

            void EarlyUpdate();
            void LateUpdate();

            void RegisterEarlyCallback( std::string_view key, std::function<void()> callback);
            void RegisterLateCallback( std::string_view key, std::function<void()> callback);
            void UnregisterEarlyCallback( std::string_view key);
            void UnregisterLateCallback( std::string_view key);

        private:

            vktg::Timer mFrameTimer;
            const uint8_t mFrameOverlap;
            uint64_t mFrameCount;

            std::unordered_map<std::string, std::function<void()>> mEarlyFrameCallbacks;
            std::unordered_map<std::string, std::function<void()>> mLateFrameCallbacks;
    };

    
} // namespace vktg