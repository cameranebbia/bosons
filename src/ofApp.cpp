#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetVerticalSync(false);
	ofSetLogLevel(OF_LOG_NOTICE);
	


	partNone.loadImage("partNone.jpg");

	

	drawWidth = 1280;
	drawHeight = 720;
	// process all but the density on 16th resolution
	flowWidth = drawWidth / 4;
	flowHeight = drawHeight / 4;
	

	// FLOW & MASK
	opticalFlow.setup(flowWidth, flowHeight);
	velocityMask.setup(drawWidth, drawHeight);
	
	// FLUID & PARTICLES
	fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	
	
	velocityDots.setup(flowWidth / 4, flowHeight / 4);
	
	// VISUALIZATION
	displayScalar.setup(flowWidth, flowHeight);
	velocityField.setup(flowWidth / 4, flowHeight / 4);
	temperatureField.setup(flowWidth / 4, flowHeight / 4);
	pressureField.setup(flowWidth / 4, flowHeight / 4);
	velocityTemperatureField.setup(flowWidth / 4, flowHeight / 4);
	
	// MOUSE DRAW
	mouseForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	

	didCamUpdate = false;
	cameraFbo.allocate(640, 480);
	cameraFbo.black();
	
	kinect.init(false, false); // disable video image (faster fps)

	kinect.open(0);

	//    kinect.setLed(ofxKinect::LED_OFF);

	

	grayImageIn.allocate(kinect.width, kinect.height);
	grayImageThresh.allocate(kinect.width, kinect.height);
	grayImageKinectOld.allocate(kinect.width, kinect.height);
	grayImageKinectDiff.allocate(kinect.width, kinect.height);

	// GUI
	setupGui();

	lastTime = ofGetElapsedTimef();

	background.loadSound("audio/bg.wav");
	sound.loadSound("audio/stein6.mp3");

	background.setVolume(backgroundVol);
	background.setLoop(true);
	sound.setVolume(0);
	sound.setLoop(true);

	background.play();
	sound.play();

	screenshotCount = 0;

}

//--------------------------------------------------------------
void ofApp::setupGui() {

	gui.setup("SETTINGS");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.setPosition(20, 20);

	guiEffect.setup("EFFECT", "settingsEffect.xml");
	guiEffect.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	guiEffect.setDefaultFillColor(ofColor(160, 160, 160, 160));
	guiEffect.setPosition(1680, 20);

	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	gui.add(guiMinFPS.set("minimum FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", true));
	doFullScreen.addListener(this, &ofApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(doDrawCamBackground.set("DRAW SOURCE (C)", true));
	gui.add(showKinect.set("SHOW KINECT", true));
	gui.add(mirrorX.set("mirror X", true));
	gui.add(mirrorY.set("mirror Y", false));
	gui.add(presence.set("presence", true));
	gui.add(minAreaPresIn.set("minAreaPresIn", 1000, 0, 25000));
	gui.add(minAreaPresOut.set("minAreaPresOut", 500, 0, 25000));
	
	gui.add(farThreshold.set("farThreshold", 150, 0, 255));
	gui.add(erode.set("erode", 0, 0, 40));
	gui.add(dilate.set("dilate", 0, 0, 40));

	gui.add(alphaNoneVel.set("alphaNoneVel", 0, 0, 5000));
	gui.add(alphaNone.set("alphaNone", 0, 0, 255));


	gui.add(cropLeft.set("cropLeft", 0, -20, 960));
	gui.add(cropRight.set("cropRight", 0, -20, 960));
	gui.add(showCrop.set("showCrop", false));
	
	gui.add(backgroundVol.set("backgroundVol", 0, 0, 1));
	gui.add(soundVol.set("soundVol", 0, 0, 1));


	gui.add(kinectDiffSkip.set("kinectDiffSkip", 0, 0, 10));

	
	gui.add(soundMoveScaleDown.set("soundMoveScaleDown", 10000, 1, 100000));
	gui.add(soundMoveSpeed.set("soundMoveSpeed", 0, 0, 1));
	
	gui.add(threshMoveAudioMin.set("threshMoveAudioMin", 0, 0, 1));
	gui.add(threshMoveAudioMax.set("threshMoveAudioMax", 0, 0, 1));

	gui.add(volFade.set("volFade", 0.04, 0, 0.2));
	
	gui.add(soundVolMove.set("soundVolMove", 0, 0, 1));

	gui.add(partNoneRotVel.set("partNoneRotVel ", 0.7, 0, 4));
	gui.add(partNoneRotAmount.set("partNoneRotAmount", 10, 0, 100));





	guiEffect.add(drawMode.set("draw mode", DRAW_COMPOSITE, DRAW_COMPOSITE, DRAW_MOUSE));
	drawMode.addListener(this, &ofApp::drawModeSetName);
	guiEffect.add(drawName.set("MODE", "draw name"));

	guiEffect.add(toggleGuiEffectDraw.set("show effect gui (J)", false));
	
	int guiColorSwitch = 0;
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(opticalFlow.parameters);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(velocityMask.parameters);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(fluidSimulation.parameters);

	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(particleFlow.parameters);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(mouseForces.leftButtonParameters);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(mouseForces.rightButtonParameters);
	
	visualizeParameters.setName("visualizers");
	visualizeParameters.add(showScalar.set("show scalar", true));
	visualizeParameters.add(showField.set("show field", true));
	visualizeParameters.add(displayScalarScale.set("scalar scale", 0.15, 0.05, 1.0));
	displayScalarScale.addListener(this, &ofApp::setDisplayScalarScale);
	visualizeParameters.add(velocityFieldScale.set("velocity scale", 0.1, 0.0, 0.5));
	velocityFieldScale.addListener(this, &ofApp::setVelocityFieldScale);
	visualizeParameters.add(temperatureFieldScale.set("temperature scale", 0.1, 0.0, 0.5));
	temperatureFieldScale.addListener(this, &ofApp::setTemperatureFieldScale);
	visualizeParameters.add(pressureFieldScale.set("pressure scale", 0.02, 0.0, 0.5));
	pressureFieldScale.addListener(this, &ofApp::setPressureFieldScale);
	visualizeParameters.add(velocityLineSmooth.set("line smooth", false));
	velocityLineSmooth.addListener(this, &ofApp::setVelocityLineSmooth);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(visualizeParameters);
	
	guiEffect.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	guiEffect.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	guiEffect.add(velocityDots.parameters);

	// if the settings file is not present the parameters will not be set during this setup
	if (!ofFile("settingsEffect.xml"))
		guiEffect.saveToFile("settingsEffect.xml");
	
	guiEffect.loadFromFile("settingsEffect.xml");

	toggleGuiEffectDraw= false;


	guiEffect.minimizeAll();
	
	if (!ofFile("settings.xml"))
		gui.saveToFile("settings.xml");

	gui.loadFromFile("settings.xml");
	

	toggleGuiDraw = false;

	doFullScreen = true;
	presence = false;
	alphaNone = 0;
	showCrop = false;

	soundMoveSpeed = 0;
	soundVolMove = 0;

	kinectDiffCount = 0;
	
}

//--------------------------------------------------------------
void ofApp::update(){
	didCamUpdate = false;

	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	
	kinect.update();
	
	if (kinect.isFrameNew()) {
		didCamUpdate = true;
		

		grayImageIn.setFromPixels(kinect.getDepthPixels());

		grayImageIn.mirror(mirrorY, mirrorX);

		int type = CV_THRESH_TOZERO;

		cvThreshold(grayImageIn.getCvImage(), grayImageThresh.getCvImage(), (255-farThreshold), 255, type);

		grayImageThresh.flagImageChanged();

		for (int i = 0; i < erode; i++) {
			grayImageThresh.erode();
		}
		for (int i = 0; i < dilate; i++) {
			grayImageThresh.dilate();
		}
		/////////////// SOUNDS \\\\\\\\\\\\\\\\

		kinectDiffCount++;
		if (kinectDiffCount >= kinectDiffSkip) {
			kinectDiffCount = 0;
			grayImageKinectDiff = grayImageThresh;
			grayImageKinectDiff.absDiff(grayImageKinectOld);
			grayImageKinectOld = grayImageThresh;

			soundMoveSpeed = grayImageKinectDiff.countNonZeroInRegion(0, 0, grayImageKinectDiff.width, grayImageKinectDiff.height) / (float)soundMoveScaleDown;

		}
		
		
		int areaPres = grayImageThresh.countNonZeroInRegion(0, 0, grayImageThresh.width, grayImageThresh.height);
		if (areaPres < minAreaPresOut) {
			presence = false;
			grayImageKinectOld = grayImageThresh;
			soundVolMove += (-soundVolMove) * volFade;

		}
		else if (areaPres > minAreaPresIn)
		{
			presence = true;
			float soundVolMoveMap = ofMap(soundMoveSpeed, threshMoveAudioMin, threshMoveAudioMax, 0, 1, true);
			soundVolMove += (soundVolMoveMap - soundVolMove) * volFade;

		}
		

		sound.setVolume(soundVolMove*soundVol);

		background.setVolume(backgroundVol);
	}


	


	


	fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
	fluidSimulation.addDensity(velocityMask.getColorMask());
	fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
	
	mouseForces.update(deltaTime);
	
	for (int i=0; i<mouseForces.getNumForces(); i++) {
		if (mouseForces.didChange(i)) {
			switch (mouseForces.getType(i)) {
				case FT_DENSITY:
					fluidSimulation.addDensity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
					break;
				case FT_VELOCITY:
					fluidSimulation.addVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
					particleFlow.addFlowVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
					break;
				case FT_TEMPERATURE:
					fluidSimulation.addTemperature(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
					break;
				case FT_PRESSURE:
					fluidSimulation.addPressure(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
					break;
				case FT_OBSTACLE:
					fluidSimulation.addTempObstacle(mouseForces.getTextureReference(i));
				default:
					break;
			}
		}
	}
	
	fluidSimulation.update();
	
	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluidSimulation.getSpeed());
		particleFlow.setCellSize(fluidSimulation.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
//		particleFlow.addDensity(fluidSimulation.getDensity());
		particleFlow.setObstacle(fluidSimulation.getObstacle());
	}
	particleFlow.update();

	if (didCamUpdate) {
	
		ofSetColor(255);

		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.begin();

		if (presence) {
			if (alphaNone >0) {
				alphaNone -= deltaTime * alphaNoneVel;
			}
			if (alphaNone < 0) {
				alphaNone = 0;
			}
		}
		else {
			if (alphaNone < 255) {
				alphaNone += deltaTime * alphaNoneVel;
			}
			if (alphaNone > 255) {
				alphaNone = 255;
			}
		}

		if (alphaNone <255) {
			grayImageThresh.draw(0, 0);
		}

		if (alphaNone > 0) {
			ofEnableAlphaBlending();
			//ofPushMatrix();
			ofTranslate(-cos(lastTime * partNoneRotVel) * partNoneRotAmount, sin(lastTime * partNoneRotVel) * partNoneRotAmount);
			//ofTranslate(cameraFbo.getWidth() / 2, cameraFbo.getHeight() / 2);
			ofSetColor(255, alphaNone);
			
//			partNone.draw(-1920 / 2, -1200 / 2, 1920, 1200);
			partNone.draw(0, 0, cameraFbo.getWidth(), cameraFbo.getHeight());
			ofSetColor(255);
		//	ofPopMatrix();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		}
		cameraFbo.end();
		ofPopStyle();

		opticalFlow.setSource(cameraFbo.getTexture());

		// opticalFlow.update(deltaTime);
		// use internal deltatime instead
		opticalFlow.update();

		velocityMask.setDensity(cameraFbo.getTexture());
		velocityMask.setVelocity(opticalFlow.getOpticalFlow());
		velocityMask.update();

	}
	
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 'G':
		case 'g': toggleGuiDraw = !toggleGuiDraw; break;
		case 'j': toggleGuiEffectDraw = !toggleGuiEffectDraw; break;
		case 'f':
		case 'F': 
			doFullScreen.set(!doFullScreen.get()); 
			if (!doFullScreen) {
				ofSetWindowPosition(50, 50);
			}
			break;
		case 'c':
		case 'C': doDrawCamBackground.set(!doDrawCamBackground.get()); break;
			
		case '1': drawMode.set(DRAW_COMPOSITE); break;
		case '2': drawMode.set(DRAW_PARTICLES); break;
			//case '9': drawMode.set(DRAW_FLUID_FIELDS); break;
		case '3': drawMode.set(DRAW_SOURCE); break;
		case '4': drawMode.set(DRAW_FLUID_VELOCITY); break;
			//case '5': drawMode.set(DRAW_FLUID_TEMPERATURE); break;
			//case '6': drawMode.set(DRAW_OPTICAL_FLOW); break;
			//case '7': drawMode.set(DRAW_FLOW_MASK); break;
			//case '8': drawMode.set(DRAW_MOUSE); break;
			
		case 'r':
		case 'R':
			fluidSimulation.reset();
			mouseForces.reset();
			break;

		case 's':
			gui.saveToFile("settings.xml");
			break;
		
		case 'l':
			guiEffect.loadFromFile("settingsEffect.xml");
			gui.loadFromFile("settings.xml");
			break;

		case 'x':
			screenshot.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			screenshot.save("screenshot" + ofToString(screenshotCount) + ".jpg");
			screenshotCount++;
			break;

		default: break;
	}
}

//--------------------------------------------------------------
void ofApp::drawModeSetName(int &_value) {
	switch(_value) {
		case DRAW_COMPOSITE:		drawName.set("Composite      (1)"); break;
		case DRAW_PARTICLES:		drawName.set("Particles      (2)"); break;
			//case DRAW_FLUID_FIELDS:		drawName.set("Fluid Fields   (2)"); break;
			//case DRAW_FLUID_DENSITY:	drawName.set("Fluid Density  "); break;
		case DRAW_FLUID_VELOCITY:	drawName.set("Fluid Velocity (3)"); break;
			//case DRAW_FLUID_PRESSURE:	drawName.set("Fluid Pressure (4)"); break;
			//case DRAW_FLUID_TEMPERATURE:drawName.set("Fld Temperature(5)"); break;
			//case DRAW_FLUID_DIVERGENCE: drawName.set("Fld Divergence "); break;
			//case DRAW_FLUID_VORTICITY:	drawName.set("Fluid Vorticity"); break;
			//case DRAW_FLUID_BUOYANCY:	drawName.set("Fluid Buoyancy "); break;
			//case DRAW_FLUID_OBSTACLE:	drawName.set("Fluid Obstacle "); break;
			//case DRAW_OPTICAL_FLOW:		drawName.set("Optical Flow   (6)"); break;
			//case DRAW_FLOW_MASK:		drawName.set("Flow Mask      (7)"); break;
		case DRAW_SOURCE:			drawName.set("Source         (4)"); break;
			//case DRAW_MOUSE:			drawName.set("Left Mouse     (8)"); break;
			//case DRAW_VELDOTS:			drawName.set("VelDots        (0)"); break;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0,0);
	if (doDrawCamBackground.get())
		drawSource();
		
		
	if (!toggleGuiDraw) {
		ofHideCursor();
		drawParticles();
	}
	else {
		ofShowCursor();
		switch(drawMode.get()) {
			case DRAW_COMPOSITE: drawComposite(); break;
			case DRAW_PARTICLES: drawParticles(); break;
			//case DRAW_FLUID_FIELDS: drawFluidFields(); break;
			//case DRAW_FLUID_DENSITY: drawFluidDensity(); break;
			case DRAW_FLUID_VELOCITY: drawFluidVelocity(); break;
			case DRAW_SOURCE: drawSource(); break;
				//case DRAW_FLUID_PRESSURE: drawFluidPressure(); break;
				//case DRAW_FLUID_TEMPERATURE: drawFluidTemperature(); break;
				//case DRAW_FLUID_DIVERGENCE: drawFluidDivergence(); break;
				//case DRAW_FLUID_VORTICITY: drawFluidVorticity(); break;
				//case DRAW_FLUID_BUOYANCY: drawFluidBuoyance(); break;
				//case DRAW_FLUID_OBSTACLE: drawFluidObstacle(); break;
				//case DRAW_FLOW_MASK: drawMask(); break;
				//case DRAW_OPTICAL_FLOW: drawOpticalFlow(); break;
				
				//case DRAW_MOUSE: drawMouseForces(); break;
				//case DRAW_VELDOTS: drawVelocityDots(); break;
		}

		ofSetColor(0, 255);
		if (showCrop)
			ofSetColor(255, 0, 0, 255);
		ofDrawRectangle(-2, 0, cropLeft, 1200);
		ofDrawRectangle(1922, 0, -cropRight, 1200);
		ofSetColor(255, 255);

		drawGui();
		
		ofDrawBitmapString("PRESS G -> TOGGLE GUI \nF -> TOGGLE FULLSCREEN \nS -> SAVE SETTINGS.XML \nL -> (RE)LOAD SETTINGS.XML", 20, 650);

		if (showKinect) {
			ofDrawRectangle(300 - 5, 200 - 5, 320 + 10, 240 + 10);
			kinect.drawDepth(300, 200, 320, 240);
			ofDrawRectangle(700 - 5, 200 - 5, 320 + 10, 240 + 10);
			grayImageThresh.draw(700, 200, 320, 240);
			ofDrawRectangle(1100 - 5, 200 - 5, 320 + 10, 240 + 10);
			cameraFbo.draw(1100, 200, 320, 240);
			ofDrawRectangle(1500 - 5, 200 - 5, 320 + 10, 240 + 10);
			grayImageKinectDiff.draw(1500, 200, 320, 240);
			ofDrawBitmapStringHighlight("KINECT DEPTH", 300, 470);
			ofDrawBitmapStringHighlight("KINECT THRESHOLD", 700, 470);
			ofDrawBitmapStringHighlight("FBO FOR FLOW", 1100, 470);
			ofDrawBitmapStringHighlight("KINECT DIFFERENCE", 1500, 470);

		}
		if (toggleGuiEffectDraw) {
			guiEffect.draw();
		}
	}
}

//--------------------------------------------------------------
void ofApp::drawComposite(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	fluidSimulation.draw(_x, _y, _width, _height);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	
	
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawParticles(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	
	
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidFields(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	pressureField.setPressure(fluidSimulation.getPressure());
	pressureField.draw(_x, _y, _width, _height);
	velocityTemperatureField.setVelocity(fluidSimulation.getVelocity());
	velocityTemperatureField.setTemperature(fluidSimulation.getTemperature());
	velocityTemperatureField.draw(_x, _y, _width, _height);
	temperatureField.setTemperature(fluidSimulation.getTemperature());
	
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidDensity(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	
	fluidSimulation.draw(_x, _y, _width, _height);
	
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidVelocity(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofClear(0,0);
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	//	ofEnableBlendMode(OF_BLENDMODE_DISABLED); // altenate mode
		displayScalar.setSource(fluidSimulation.getVelocity());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		velocityField.setVelocity(fluidSimulation.getVelocity());
		velocityField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidPressure(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofClear(128);
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getPressure());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		pressureField.setPressure(fluidSimulation.getPressure());
		pressureField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidTemperature(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getTemperature());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		temperatureField.setTemperature(fluidSimulation.getTemperature());
		temperatureField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidDivergence(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getDivergence());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		temperatureField.setTemperature(fluidSimulation.getDivergence());
		temperatureField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidVorticity(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getConfinement());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(255, 255, 255, 255);
		velocityField.setVelocity(fluidSimulation.getConfinement());
		velocityField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidBuoyance(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getSmokeBuoyancy());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		velocityField.setVelocity(fluidSimulation.getSmokeBuoyancy());
		velocityField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawFluidObstacle(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	fluidSimulation.getObstacle().draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawMask(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	velocityMask.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawOpticalFlow(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		displayScalar.setSource(opticalFlow.getOpticalFlowDecay());
		displayScalar.draw(0, 0, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		velocityField.setVelocity(opticalFlow.getOpticalFlowDecay());
		velocityField.draw(0, 0, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawSource(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	cameraFbo.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawMouseForces(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofClear(0,0);
	
	for(int i=0; i<mouseForces.getNumForces(); i++) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		if (mouseForces.didChange(i)) {
			ftDrawForceType dfType = mouseForces.getType(i);
			if (dfType == FT_DENSITY)
				mouseForces.getTextureReference(i).draw(_x, _y, _width, _height);
		}
	}
	
	for(int i=0; i<mouseForces.getNumForces(); i++) {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		if (mouseForces.didChange(i)) {
			switch (mouseForces.getType(i)) {
				case FT_VELOCITY:
					velocityField.setVelocity(mouseForces.getTextureReference(i));
					velocityField.draw(_x, _y, _width, _height);
					break;
				case FT_TEMPERATURE:
					temperatureField.setTemperature(mouseForces.getTextureReference(i));
					temperatureField.draw(_x, _y, _width, _height);
					break;
				case FT_PRESSURE:
					pressureField.setPressure(mouseForces.getTextureReference(i));
					pressureField.draw(_x, _y, _width, _height);
					break;
				default:
					break;
			}
		}
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawVelocityDots(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	velocityDots.setVelocity(fluidSimulation.getVelocity());
	velocityDots.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	
	// calculate minimum fps
	deltaTimeDeque.push_back(deltaTime);
	
	while (deltaTimeDeque.size() > guiFPS.get())
		deltaTimeDeque.pop_front();
	
	float longestTime = 0;
	for (int i=0; i<deltaTimeDeque.size(); i++){
		if (deltaTimeDeque[i] > longestTime)
			longestTime = deltaTimeDeque[i];
	}
	
	guiMinFPS.set(1.0 / longestTime);
	
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui.draw();
	
	// HACK TO COMPENSATE FOR DISSAPEARING MOUSE
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 300.0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 600.0);
	ofPopStyle();
}

