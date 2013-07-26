#include <v8.h>
#include <node.h>


#include "HomeControl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


using namespace node;
using namespace v8;

const int kPRODUCT_Elro = 1;
const int kPRODUCT_REV = 2;

static void SwitchElro(char* inSystemCode, char* inUnitCode, const bool switchOn) {
	int unitDigit;
	if (strcmp(inUnitCode, "A") == 0) {
	    unitDigit = 1;
	} else if (strcmp(inUnitCode, "B") == 0) {
	    unitDigit = 2;
	} else if (strcmp(inUnitCode, "C") == 0) {
	    unitDigit = 3;
	} else if (strcmp(inUnitCode, "D") == 0) {
	    unitDigit = 4;
	} else if (strcmp(inUnitCode, "E") == 0) {
	    unitDigit = 5;
	} else {
        ThrowException( Exception::TypeError( String::New( "UnitCode for Elro not supported" ) ) );
	}

	int kPIN = 0;
	HomeControl myHome = HomeControl();
	myHome.enableTransmit(kPIN);

	if(switchOn) {
	    printf("sending ON to Elro Switch: inSystemCode[%s] inUnitCode[%s]\n", inSystemCode, inUnitCode);
	    myHome.switchOn(inSystemCode, unitDigit);
	} else {
	    printf("sending OFF to Elro Switch: inSystemCode[%s] inUnitCode[%s]\n", inSystemCode, inUnitCode);
	    myHome.switchOff(inSystemCode, unitDigit);		
	}	
}

static void SwitchRev(char* inSystemCode, int inUnitCode, const bool switchOn) {
    char systemDigit;
    if (strcmp(inSystemCode, "A") == 0) {
        systemDigit = 'A';
    } else if (strcmp(inSystemCode, "B") == 0) {
        systemDigit = 'B';
    } else if (strcmp(inSystemCode, "C") == 0) {
        systemDigit = 'C';
    } else if (strcmp(inSystemCode, "D") == 0) {
        systemDigit = 'D';
	} else {
        ThrowException( Exception::TypeError( String::New( "SystemCode for REV not supported" ) ) );
	}

	int kPIN = 0;
	HomeControl myHome = HomeControl();
	myHome.enableTransmit(kPIN);

	if(switchOn) {
	    printf("sending ON to REV Switch: inSystemCode[%s] inUnitCode[%i]\n", inSystemCode, inUnitCode);
	    myHome.switchOnD(systemDigit, inUnitCode);
	} else {
	    printf("sending OFF to REV Switch: inSystemCode[%s] inUnitCode[%i]\n", inSystemCode, inUnitCode);
	    myHome.switchOffD(systemDigit, inUnitCode);		
	}	
}

static Handle<Value> Switch(const Arguments& args, const bool switchOn, const int product)
{
    HandleScope scope;

    if( args.Length() != 2 ) {
        ThrowException( Exception::TypeError( String::New( "Wrong number of arguments" ) ) );
        return scope.Close( Undefined() );
    }
    if( !args[0]->IsString() ) {
        ThrowException( Exception::TypeError( String::New( "Bad argument type" ) ) );
        return scope.Close( Undefined() );
    }
    v8::String::Utf8Value l_SystemCode(args[0]->ToString());
	char* inSystemCode = *l_SystemCode;

	if(product == kPRODUCT_Elro) {
	    if( !args[1]->IsString() ) {
	        ThrowException( Exception::TypeError( String::New( "Bad argument type" ) ) );
	        return scope.Close( Undefined() );
	    }
	    v8::String::Utf8Value l_UnitCode(args[1]->ToString());
		char* inUnitCode = *l_UnitCode;

		SwitchElro(inSystemCode, inUnitCode, switchOn);
	} else if(product == kPRODUCT_REV) {
	    if( !args[1]->IsNumber() ) {
	        ThrowException( Exception::TypeError( String::New( "Bad argument type" ) ) );
	        return scope.Close( Undefined() );
	    }
	    int inUnitCode = args[1]->NumberValue();

		SwitchRev(inSystemCode, inUnitCode, switchOn);
	}

    return scope.Close( Undefined() );
}

static Handle<Value> Switch_ELRO_ON(const Arguments& args) { return Switch(args, true, kPRODUCT_Elro); }
static Handle<Value> Switch_ELRO_OFF(const Arguments& args) { return Switch(args, false, kPRODUCT_Elro); }

static Handle<Value> Switch_REV_ON(const Arguments& args) { return Switch(args, true, kPRODUCT_REV); }
static Handle<Value> Switch_REV_OFF(const Arguments& args) { return Switch(args, false, kPRODUCT_REV); }


extern "C" void init(Handle<Object> target)
{
	if( -1 == wiringPiSetup() ) {
	    ThrowException( Exception::TypeError( String::New( "wiringPiSetup not able to initialize." ) ) );
	    return;
	}

	target->Set(String::NewSymbol("switchElroOn"), FunctionTemplate::New(Switch_ELRO_ON)->GetFunction());
	target->Set(String::NewSymbol("switchElroOff"), FunctionTemplate::New(Switch_ELRO_OFF)->GetFunction());
	target->Set(String::NewSymbol("switchRevOn"), FunctionTemplate::New(Switch_REV_ON)->GetFunction());
	target->Set(String::NewSymbol("switchRevOff"), FunctionTemplate::New(Switch_REV_OFF)->GetFunction());
}

NODE_MODULE(whomecontrolpi, init);
