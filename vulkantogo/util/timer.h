
#pragma once


#include <chrono>
#include <string>


namespace vktg
{


    class Timer {

        using Nanoseconds = std::chrono::nanoseconds;
        using HighResClock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<HighResClock>;

        public:

            static std::string GetDateTimeStr();
            static std::string GetDateStr();
            static std::string GetTimeStr();

            Timer();
            ~Timer() = default;

            void Start();
            void Stop();
            void Reset();

            void Update();

            void TimeScale(float scale);
            float TimeScale() const;

            double DeltaTime() const;
            double ElapsedTime() const;
            double ElapsedUnscaledTime() const;
            bool IsRunning();


        private:

            TimePoint mStartTime;
            TimePoint mCurrTime;
            
            bool mRunning;
            float mTimeScale;

            double mDeltaTime;
            double mElapsedTime;
            double mElapsedUnscaledTime;
    };

        
} // namespace vktg