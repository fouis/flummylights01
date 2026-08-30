#include "Arduino.h"
#include "flummylights.h"
#include "parameter.h"

Parameter::Parameter(const char * myKey)
{
    key=myKey;
}


void Parameter::printDebug(void)
{
    Serial.print("Hallo. Ich bin Parameter ");
    Serial.println(key);

}




Uint8Parameter::Uint8Parameter(const char * myKey, uint8_t* valp,uint8_t vald):Parameter(myKey)
{
    parval=valp;
    defaultVal=vald;
}


// read parameter from Preferences
void Uint8Parameter::readPers(Preferences * p)
{
    if (p->isKey(key))
        *parval = p->getUInt(key, defaultVal);
    else
        *parval = defaultVal;
}
//  write parameter to Preferences
void Uint8Parameter::writePers(Preferences * p)
{
    p->putUInt(key,*parval);
}

// filter the web output, replace the key with the actual parameter
void Uint8Parameter::filterWeb(String * s)
{
    s->replace(String("%")+key+"%",String(*parval));
}
// process the web form, setting new parameter
void Uint8Parameter::processForm(AsyncWebServerRequest *request)
{
    if(request->hasParam(key,true))
    {
        setFromString(request->getParam(key,true)->value());
    }
}

void Uint8Parameter::setFromString(const String& value)
{
    *parval=value.toInt();
}



/**
 * BoolCheckedParameter 
 */
BoolCheckedParameter::BoolCheckedParameter(const char * myKey, bool* valp,bool vald):Parameter(myKey)
{
    parval=valp;
    defaultVal=vald;
}


// read parameter from Preferences
void BoolCheckedParameter::readPers(Preferences * p)
{
        if (p->isKey(key))
        *parval = p->getBool(key, defaultVal);
    else
        *parval = defaultVal;
}
//  write parameter to Preferences
void BoolCheckedParameter::writePers(Preferences * p)
{
    p->putBool(key,*parval);
}

// filter the web output, replace the key with the actual parameter
void BoolCheckedParameter::filterWeb(String * s)
{
    if (*parval)
        s->replace(String("%")+key+"%","checked");
    else
        s->replace(String("%")+key+"%","");

}
// process the web form, setting new parameter
void BoolCheckedParameter::processForm(AsyncWebServerRequest *request)
{
    if(request->hasParam(key,true))
    {
        *parval=true;
    }
    else
    {
        *parval=false;
    }
}

void BoolCheckedParameter::setFromString(const String& value)
{
    *parval = value == "1" || value == "true" || value == "on" || value == "checked";
}




/**
 * RadioCheckedParameter 
 */
RadioCheckedParameter::RadioCheckedParameter(const char * myKey, int16_t* valp,int16_t vald):Parameter(myKey)
{
    parval=valp;
    defaultVal=vald;
}


// read parameter from Preferences
void RadioCheckedParameter::readPers(Preferences * p)
{
        if (p->isKey(key))
        *parval = p->getInt(key, defaultVal);
    else
        *parval = defaultVal;
}
//  write parameter to Preferences
void RadioCheckedParameter::writePers(Preferences * p)
{
    p->putInt(key,*parval);
}

// filter the web output, replace the key with the actual parameter
void RadioCheckedParameter::filterWeb(String * s)
{
    String replaceString=String("%")+key+String(*parval)+"%";
    s->replace(replaceString,"checked");
}
// process the web form, setting new parameter
void RadioCheckedParameter::processForm(AsyncWebServerRequest *request)
{
    if(request->hasParam(key,true))
    {
        setFromString(request->getParam(key,true)->value());
    }
}

void RadioCheckedParameter::setFromString(const String& value)
{
    *parval=value.toInt();
}

/**
 * SelectParameter 
 */
SelectParameter::SelectParameter(const char * myKey, int16_t* valp,int16_t vald):Parameter(myKey)
{
    parval=valp;
    defaultVal=vald;
}


// read parameter from Preferences
void SelectParameter::readPers(Preferences * p)
{
    if (p->isKey(key))
        *parval = p->getInt(key, defaultVal);
    else
        *parval = defaultVal;
}
//  write parameter to Preferences
void SelectParameter::writePers(Preferences * p)
{
    p->putInt(key,*parval);
}

// filter the web output, replace the key with the actual parameter
void SelectParameter::filterWeb(String * s)
{
    String replaceString=String("%")+key+String(*parval)+"%";
    s->replace(replaceString,"selected");
}
// process the web form, setting new parameter
void SelectParameter::processForm(AsyncWebServerRequest *request)
{
    if(request->hasParam(key,true))
    {
        setFromString(request->getParam(key,true)->value());
    }
}

void SelectParameter::setFromString(const String& value)
{
    *parval=value.toInt();
}


/**
 * FloatParameter 
 */

FloatParameter::FloatParameter(const char * myKey, float* valp,float vald):Parameter(myKey)
{
    parval=valp;
    defaultVal=vald;
}


// read parameter from Preferences
void FloatParameter::readPers(Preferences * p)
{
    if (p->isKey(key))
        *parval = p->getFloat(key, defaultVal);
    else
        *parval = defaultVal;
}
//  write parameter to Preferences
void FloatParameter::writePers(Preferences * p)
{
    p->putFloat(key,*parval);
}

// filter the web output, replace the key with the actual parameter
void FloatParameter::filterWeb(String * s)
{
    s->replace(String("%")+key+"%",String(*parval));
}
// process the web form, setting new parameter
void FloatParameter::processForm(AsyncWebServerRequest *request)
{
    if(request->hasParam(key,true))
    {
        setFromString(request->getParam(key,true)->value());
    }
}

void FloatParameter::setFromString(const String& value)
{
    *parval=value.toFloat();
}

/**
 * IntParameter 
 */

IntParameter::IntParameter(const char * myKey, int* valp,int vald):Parameter(myKey)
{
    parval=valp;
    defaultVal=vald;
}


// read parameter from Preferences
void IntParameter::readPers(Preferences * p)
{
    if (p->isKey(key))
        *parval = p->getInt(key, defaultVal);
    else
        *parval = defaultVal;
}
//  write parameter to Preferences
void IntParameter::writePers(Preferences * p)
{
    p->putInt(key,*parval);
}

// filter the web output, replace the key with the actual parameter
void IntParameter::filterWeb(String * s)
{
    s->replace(String("%")+key+"%",String(*parval));
}
// process the web form, setting new parameter
void IntParameter::processForm(AsyncWebServerRequest *request)
{
    if(request->hasParam(key,true))
    {
        setFromString(request->getParam(key,true)->value());
    }
}

void IntParameter::setFromString(const String& value)
{
    *parval=value.toInt();
}







