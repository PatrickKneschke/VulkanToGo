#pragma once


#include <functional>
#include <vector>


namespace vktg
{


    /// @brief Collects deletion/deallocation functions for created/allocated objects. Used to guarantee correct deletion order.
    class DeletionStack {

        public:

            /// @brief Adds new deletion function to top of stack.
            /// @param func Deletion function.
            void Push( std::function<void()> &&func) {

                mDeletors.push_back( std::move(func));
            }

            /// @brief Calls all recorded deletion functions in reverse order of submission.
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
