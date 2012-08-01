#include "iSentryConfig.hxx"

ISentry::iSentryConfig *ISentry::iSentryConfig::instance(NULL);

ISentry::iSentryConfig *ISentry::iSentryConfig::getInstance()
{
    if(!instance)
        instance = new iSentryConfig();
    return instance;
}


