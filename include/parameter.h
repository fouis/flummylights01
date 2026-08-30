#ifndef PARAMETER_H
#define PARAMETER_H


#include "Arduino.h"
#include <Preferences.h>
#include "ESPAsyncWebServer.h"

class Parameter
{
    public:
        const char * key;
        Parameter (const char * myKey);
        void printDebug(void);
        // read parameter from Preferences
        virtual void readPers(Preferences * p)=0;
        //  write parameter to Preferences
        virtual void writePers(Preferences * p)=0;
        // filter the web output, replace the key with the actual parameter
        virtual void filterWeb(String * s) = 0;
        // process the web form, setting new parameter
        virtual void processForm(AsyncWebServerRequest *request) = 0;
    protected:

    private:
};

class Uint8Parameter:public Parameter
{
    public:
    // pointer to the actual value
        uint8_t * parval;
        // default value for the paraemmter
        uint8_t defaultVal;
        Uint8Parameter(const char * myKey, uint8_t* valp,uint8_t vald);
        
        // read parameter from Preferences
        void readPers(Preferences * p);
        
        //  write parameter to Preferences
        void writePers(Preferences * p);
        
        // filter the web output, replace the key with the actual parameter
        void filterWeb(String * s);
        
        // process the web form, setting new parameter
        void processForm(AsyncWebServerRequest *request);
        
};

class BoolCheckedParameter:public Parameter
{
    public:
    // pointer to the actual value
        bool * parval;
        // default value for the paraemmter
        bool defaultVal;
        BoolCheckedParameter(const char * myKey, bool* valp,bool vald);
        
        // read parameter from Preferences
        void readPers(Preferences * p);
        
        //  write parameter to Preferences
        void writePers(Preferences * p);
        
        // filter the web output, replace the key with the actual parameter
        void filterWeb(String * s);
        
        // process the web form, setting new parameter
        void processForm(AsyncWebServerRequest *request);
        

};

class RadioCheckedParameter:public Parameter
{
    public:
    // pointer to the actual value
        int16_t * parval;
        // default value for the paraemmter
        int16_t defaultVal;
        RadioCheckedParameter(const char * myKey, int16_t* valp,int16_t vald);
        
        // read parameter from Preferences
        void readPers(Preferences * p);
        
        //  write parameter to Preferences
        void writePers(Preferences * p);
        
        // filter the web output, replace the key with the actual parameter
        void filterWeb(String * s);
        
        // process the web form, setting new parameter
        void processForm(AsyncWebServerRequest *request);
        

};

class SelectParameter:public Parameter
{
    public:
    // pointer to the actual value
        int16_t * parval;
        // default value for the paraemmter
        int16_t defaultVal;
        SelectParameter(const char * myKey, int16_t* valp,int16_t vald);
        
        // read parameter from Preferences
        void readPers(Preferences * p);
        
        //  write parameter to Preferences
        void writePers(Preferences * p);
        
        // filter the web output, replace the key with the actual parameter
        void filterWeb(String * s);
        
        // process the web form, setting new parameter
        void processForm(AsyncWebServerRequest *request);
        

};

class FloatParameter:public Parameter
{
    public:
    // pointer to the actual value
        float * parval;
        // default value for the paraemmter
        float defaultVal;
        FloatParameter(const char * myKey, float* valp,float vald);
        
        // read parameter from Preferences
        void readPers(Preferences * p);
        
        //  write parameter to Preferences
        void writePers(Preferences * p);
        
        // filter the web output, replace the key with the actual parameter
        void filterWeb(String * s);
        
        // process the web form, setting new parameter
        void processForm(AsyncWebServerRequest *request);
        
};

class IntParameter:public Parameter
{
    public:
    // pointer to the actual value
        int * parval;
        // default value for the paraemmter
        int defaultVal;
        IntParameter(const char * myKey, int* valp,int vald);
        
        // read parameter from Preferences
        void readPers(Preferences * p);
        
        //  write parameter to Preferences
        void writePers(Preferences * p);
        
        // filter the web output, replace the key with the actual parameter
        void filterWeb(String * s);
        
        // process the web form, setting new parameter
        void processForm(AsyncWebServerRequest *request);
        
};




#endif
