#ifndef __ISENTRY_CONFIG_HXX__
#define __ISENTRY_CONFIG_HXX__

// Using http://www.hyperrealm.com/libconfig/
#include <libconfig.h++>

namespace ISentry
{
    class iSentryConfig:public libconfig::Config
    {

    public:
        static iSentryConfig *getInstance();

    private:
        static iSentryConfig *instance;
    };
};


#endif // __ISENTRY_CONFIG_HXX__
