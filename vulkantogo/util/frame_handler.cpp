
#include "frame_handler.h"


namespace vktg
{


    FrameHandler::FrameHandler(const uint8_t frameOverlap) : mFrameOverlap{frameOverlap}, mFrameCount{0} {

        mFrameTimer.Start();
    }


    uint8_t FrameHandler::FrameOverlap() {

        return mFrameOverlap;
    }


    uint64_t FrameHandler::FrameCount() {
        
        return mFrameCount;
    }


    uint8_t FrameHandler::CurrentFrameIndex() {

        return mFrameCount % mFrameOverlap;
    }

    
    double FrameHandler::DeltaTime() {

        return mFrameTimer.DeltaTime();
    }


    void FrameHandler::EarlyUpdate() {

        for (auto &[_, callback] : mEarlyFrameCallbacks)
        {
            callback();
        }
    }


    void FrameHandler::LateUpdate() {

        ++mFrameCount;
        mFrameTimer.Update();

        for (auto &[_, callback] : mLateFrameCallbacks)
        {
            callback();
        }
    }


    void FrameHandler::RegisterEarlyCallback(std::string_view key, std::function<void()> callback) {

        mEarlyFrameCallbacks[key.data()] = callback;
    }


    void FrameHandler::RegisterLateCallback(std::string_view key, std::function<void()> callback) {
        
        mLateFrameCallbacks[key.data()] = callback;
    }


    void FrameHandler::UnregisterEarlyCallback(std::string_view key) {

        mEarlyFrameCallbacks.erase( key.data());
    }


    void FrameHandler::UnregisterLateCallback(std::string_view key) {

        mLateFrameCallbacks.erase( key.data());
    }


} // namespace vktg
