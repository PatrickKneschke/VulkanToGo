#pragma once


#include <functional>
#include <vector>


namespace vktg
{


    class DeletionStack {

        public:

            void Push( std::function<void()> &&func) {

                mDeletors.push_back( std::move(func));
            }

            void Flush() {

                while ( !mDeletors.empty())
                {
                    mDeletors.back()();
                    mDeletors.pop_back();
                }
            }

        private:

            std::vector<std::function<void()>> mDeletors;
    };    

    
} // namespace vktg
