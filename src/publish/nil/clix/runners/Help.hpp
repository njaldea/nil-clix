#pragma once

#include <ostream>

namespace nil::clix
{
    class Options;

    namespace runners
    {
        class Help
        {
        public:
            explicit Help(std::ostream& init_oss);
            int operator()(const Options& options) const;

        private:
            std::ostream* oss;
        };
    }
}
