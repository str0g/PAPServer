#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "07";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2010";
	static const char UBUNTU_VERSION_STYLE[] = "10.06";
	
	//Software Status
	static const char STATUS[] = "";
	static const char STATUS_SHORT[] = "";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 0;
	static const long BUILD = 1138;
	static const long REVISION = 6307;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1941;
	#define RC_FILEVERSION 0,0,1138,6307
	#define RC_FILEVERSION_STRING "0, 0, 1138, 6307\0"
	static const char FULLVERSION_STRING[] = "0.0.1138.6307";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
