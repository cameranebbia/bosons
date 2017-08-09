#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	
    ofGLFWWindowSettings windowSettings;
#ifdef USE_PROGRAMMABLE_GL
    windowSettings.setGLVersion(4, 1);
#endif
    windowSettings.width = 1920;
    windowSettings.height = 1080;
    windowSettings.windowMode = OF_FULLSCREEN;
    
    ofCreateWindow(windowSettings);
    
    ofRunApp(new ofApp());
}
