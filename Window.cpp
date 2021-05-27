////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"
#include "time.h"

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Skin";

// Objects to render
Cube * Window::ground;
Skeleton* Window::skeleton;
Skin* Window::skin;
Animation* Window::animation;
Animator* Window::animator;
Cloth* Window::cloth;
ParticleSystem* Window::particleSystem;
// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;
bool CamEnabled = false;
bool AnimPlaying = false;
bool BackFaceCulling = false;
bool skinEnabled = false;
bool skelEnabled = false;
bool ambientLighting = false;
bool drawWireframe = false;

int numParticles = 10;
int totalParticles = 0;
float sizeX = 0;
float sizeY = 0;

float windX = 0;
float windY = 0;
float windZ = 0;

float x = 0.0f;
float y = 0.0f;
float z = 0.0f;

// The shader program id
GLuint Window::shaderProgram;
float startTime;
float currentTime;


////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}
	startTime = clock();
	return true;
}

bool Window::initializeObjects(std::string modelName)
{
	/*std::cout << "Number of Particles:";
	numParticles = 0;
	std::cin >> numParticles; 
	std::cout << "Size X: ";
	sizeX = 0;
	std::cin >> sizeX;
	std::cout << "Size Y: ";
	sizeY = 0;
	std::cin >> sizeY;
	cloth = new Cloth(numParticles, sizeX, sizeY, glm::mat4(1));
	*/
	particleSystem = new ParticleSystem(glm::mat4(1));

	/*std::cout << "Loading Model: " << modelName << "..." << std::endl;
	std::string skeletonName = "skeletons/" + modelName + ".skel";
	std::string skinName = "skins/" + modelName + ".skin";*/
	// Create a cube
	//cube = new Cube();
	ground = new Cube(glm::vec3(-100, -0.5f, -100), glm::vec3(100, -0.6f, 100));
	/*skeleton = new Skeleton();
	skelEnabled = skeleton->load(skeletonName.c_str());
	if (!skelEnabled) skeleton = nullptr;
	//skeleton->print();
	skin = new Skin();
	skinEnabled = skin->load(skinName.c_str(), skeleton);
	if (!skinEnabled) skin = nullptr;
	animation = new Animation();
	if (strcmp(modelName.c_str(), "wasp") == 0) {
		animation->load("animations/wasp_walk.anim");
		animator = new Animator(skeleton, animation);
		//animation->print();
	}
	else {
		animation = nullptr;
		animator = nullptr;
	}
	//skin->print();*/
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	//delete cube;
	delete cloth;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

////////////////////////////////////////////////////////////////////////////////

// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui::StyleColorsDark();
	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

// update and draw functions
void Window::idleCallback()
{
	// Perform any updates as necessary. 
	Cam->Update();
	if (skeleton != nullptr) skeleton->update();
	if (skin != nullptr) skin->update();
	if (AnimPlaying) currentTime = (clock() - startTime) / 100000000.0f;
	else currentTime = 0;
	if (animator != nullptr) animator->play(currentTime);
	particleSystem->CreateParticles(currentTime);
	totalParticles = particleSystem->Simulate(currentTime, ground);
	//cube->update();
	//cloth->update(currentTime, windX, windY, windZ, ground);
	//cloth->moveFixed(x, y, z, currentTime);
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
	ground->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	particleSystem->draw(Cam->GetViewProjectMtx(), Window::shaderProgram, ambientLighting);
	//cloth->draw(Cam->GetViewProjectMtx(), Window::shaderProgram, ambientLighting, drawWireframe);

	if (BackFaceCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	if (skelEnabled) skeleton->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	if (skinEnabled) skin->draw(Cam->GetViewProjectMtx(), Window::shaderProgram, ambientLighting);

	if (BackFaceCulling) glDisable(GL_CULL_FACE);
	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (ImGui::Button("Toggle Camera")) {
		CamEnabled = !CamEnabled;
	}
	ImGui::Text("Camera: %s", CamEnabled ? "true" : "false");
	/*if (ImGui::Button("WireFrame")) {
		drawWireframe = !drawWireframe;
	}*/
	//ImGui::Text("Wireframe: %s", drawWireframe ? "true" : "false");
	if (ImGui::Button("Reset")) {
		particleSystem->clear();
		//delete(cloth);
		//cloth = new Cloth(numParticles, sizeX, sizeY, glm::mat4(1));
	}
	if (ImGui::Button("Play/Pause")) {
		AnimPlaying = !AnimPlaying;
	}
	ImGui::Text("Playing: %s", AnimPlaying ? "true" : "false");
	if (ImGui::Button("Create Particles")) {
		particleSystem->CreateParticles(1);
	}

	ImGui::SliderFloat("PosX", &particleSystem->sourcePos.x, -100.0f, 100.0f);
	ImGui::SliderFloat("PosY", &particleSystem->sourcePos.y, -100.0f, 100.0f);
	ImGui::SliderFloat("PosZ", &particleSystem->sourcePos.z, -100.0f, 100.0f);

	ImGui::Text("Num Particles: %d", totalParticles);
	ImGui::SliderInt("MaxParticles", &particleSystem->maxParticles, 1, 10000);
	ImGui::SliderInt("Creation Rate", &particleSystem->particlesPerSecond, 1, 10000);
	ImGui::SliderInt("MaxLifeSpan", &particleSystem->maxLifeSpan, 2, 100);
	ImGui::SliderInt("MaxSpeed", &particleSystem->maxSpeed, 1, 100);
	ImGui::SliderFloat("Gravity", &particleSystem->gravity, -100, 100);
	ImGui::SliderFloat("Air Density", &particleSystem->airDensity, 0, 10);
	ImGui::SliderFloat("Drag", &particleSystem->dragCoefficient, 0, 1);
	ImGui::SliderFloat("Radius", &particleSystem->particleRadius, 1, 100);
	ImGui::SliderFloat("Elasticity", &particleSystem->elasticity, 0, 1);
	ImGui::SliderFloat("Friction", &particleSystem->friction, 0, 1);

	/*if (ImGui::Button("Ambient Lighting")) {
		ambientLighting = !ambientLighting;
	}*/
	//ImGui::Text("Ambient Lighting: %s", ambientLighting ? "true" : "false");
	//float mass = cloth->GetMass();
	//ImGui::SliderFloat("Mass", &mass, 0.001f, 1.0f);
	//cloth->SetMass(mass);
	//ImGui::SliderFloat("Density", &cloth->Density, 0, 10);
	//ImGui::SliderFloat("Drag Force", &cloth->DragForce, 0, 10);
	//float springConstant = cloth->springDampeners.front()->SpringConstant;
	//float dampingConstant = cloth->springDampeners.front()->DampingConstant;
	//ImGui::SliderFloat("Spring Constant", &springConstant, 0, 100000);
	
	//ImGui::SliderFloat("WindX", &windX, -100.0f, 100.0f);
	//ImGui::SliderFloat("WindY", &windY, -100.0f, 100.0f);
	//ImGui::SliderFloat("WindZ", &windZ, -100.0f, 100.0f);

	//ImGui::Text("Move Fixed");
	//ImGui::SliderFloat("X", &x, -100, 100);
	//ImGui::SliderFloat("Y", &y, -100, 100);
	//ImGui::SliderFloat("Z", &z, -100, 100);

	/*for (Particle* p : cloth->particles) {
		ImGui::Text("Particle: %f, %f, %f\n", p->Position.x, p->Position.y, p->Position.z);
		if (ImGui::Button(p->Fixed ? "Free" : "Fix")) {
			p->Fixed = !p->Fixed;
		}
	}
	for (SpringDampener* sp : cloth->springDampeners) {
		sp->SpringConstant = springConstant;
		sp->DampingConstant = dampingConstant;
	}*/

	/*if (ImGui::Button("BackFace Culling")) {
		BackFaceCulling = !BackFaceCulling;
	}
	ImGui::Text("BackFace Culling: %s", BackFaceCulling ? "true" : "false");
	if (skin != nullptr && ImGui::Button("Skin Mode")) {
		skinEnabled = !skinEnabled;
	}
	ImGui::Text("Skin Mode: %s", skinEnabled ? "true" : "false");
	if (skeleton != nullptr && ImGui::Button("Skeleton Mode")) {
		skelEnabled = !skelEnabled;
	}
	ImGui::Text("Skeleton Mode: %s", skelEnabled ? "true" : "false");
	
	if (skeleton != nullptr) skeleton->root->SetSliders();
	*/
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	

	// Swap buffers.
	glfwSwapBuffers(window);
	
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;
		case GLFW_KEY_D:
			CamEnabled = !CamEnabled;
			break;


		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (!CamEnabled) return;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////