
#include "timer.h"


namespace vktg
{


    std::string Timer::GetDateTimeStr() {

        auto now = std::time(nullptr);
        char buffer[sizeof("YYYY-MM-DD_HH:MM:SS")];    
        auto dateTimeStr = std::string(buffer, buffer + std::strftime(buffer, sizeof(buffer),"%F_%T", std::gmtime(&now)));

        return dateTimeStr;
    }

    std::string Timer::GetDateStr() {

        return GetDateTimeStr().substr(0, 10);
    }

    std::string Timer::GetTimeStr() {

        return GetDateTimeStr().substr(11);
    }


    Timer::Timer() {

        Reset();
    }


    void Timer::Start() {

        mRunning = true;
    }


    void Timer::Stop() {

        mRunning = false;
    }


    void Timer::Reset() {

        mStartTime = HighResClock::now();
        mCurrTime = mStartTime;
        mRunning = false;
        mTimeScale = 1.0f;
        mDeltaTime = 0.0;
        mElapsedTime = 0.0;
        mElapsedUnscaledTime = 0.0;
        mElapsedFrames = 0;
    }


    void Timer::Update() {

        ++mElapsedFrames;
        mDeltaTime = 0.0;
        if (mRunning)
        {
            auto now = HighResClock::now();
            mDeltaTime = mTimeScale * (double)std::chrono::duration_cast<Nanoseconds>(now - mCurrTime).count() * 1e-9;
            mElapsedUnscaledTime = (double)std::chrono::duration_cast<Nanoseconds>(now - mStartTime).count() * 1e-9;
            mElapsedTime += mDeltaTime;
            mCurrTime = now;
        }  
    }


    float Timer::TimeScale() const {

        return mTimeScale;
    }


    void Timer::TimeScale(float scale) {

        mTimeScale = scale;
    }


    double Timer::DeltaTime() const {

        return mDeltaTime;
    }


    double Timer::ElapsedTime() const {
        
        return mElapsedTime;
    }


    double Timer::ElapsedUnscaledTime() const {

        return mElapsedUnscaledTime;
    }


    uint64_t Timer::ElapsedFrames() const {

        return mElapsedFrames;
    }


    bool Timer::IsRunning() {

        return mRunning;
    }

    
} // namespace vktg