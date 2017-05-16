#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <SOIL.h>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void readFile(const char* );
void readTemperatures();
GLuint loadTexture(GLchar const * path);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(10.0f, 10.0f, 3000.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 3000.0f);

//global matrix for value of altitudes
GLfloat*** data;
//global matrix for temperature
GLfloat cellSize;
GLint N;
GLint M;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// OpenGL options
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	Shader ourShader("hellotriangle2.vs", "hellotriangle2.frag");
	Shader lavaShader("lava.vs","lava.frag");

	//read file
	readFile("altitudes.dat");
	readTemperatures();


	//creazione griglia verde, x e y li assegno direttamente per tutti i punti
	//GLfloat greenGrid[N+1][M+1][4];
	GLfloat*** greenGrid;
	cout << "creazione greenGrid" << endl;
	int greenN = N+1, greenM = M+1;

	greenGrid = new GLfloat **[greenN];
	for (int count = 0; count < greenN; count++)
	{
		greenGrid[count] = new GLfloat*[greenM];
	}
	for (int count = 0; count < greenN; count++)
	{
		for (int column = 0; column < greenM; column++)
			greenGrid[count][column] = new GLfloat[4];
	}

	cout << "creazione greenGrid fatta" << endl;


	for (int i = 0; i < greenN; i++)
	{
		for (int j = 0; j < greenM; j++)
		{
			greenGrid[i][j][0] = (GLfloat) (i*cellSize-(cellSize/2));
			greenGrid[i][j][1] = (GLfloat) (j*cellSize-(cellSize/2));
		}
	}

	//creazione punti esterni
	greenGrid[0][0][2] = data[0][0][2];
	greenGrid[N][0][2] = data[N-1][0][2];
	greenGrid[0][M][2] = data[0][M-1][2];
	greenGrid[N][M][2] = data[N-1][M-1][2];

	greenGrid[0][0][3] = data[0][0][3];
	greenGrid[N][0][3] = data[N-1][0][3];
	greenGrid[0][M][3] = data[0][M-1][3];
	greenGrid[N][M][3] = data[N-1][M-1][3];

	//creazione bordi esterni
	for (int i = 1; i < N; i++)
	{
		greenGrid[i][0][2] = (data[i-1][0][2]+data[i][0][2])/2;
		greenGrid[i][M][2] = (data[i-1][M-1][2]+data[i][M-1][2])/2;

		greenGrid[i][0][3] = (data[i-1][0][3]+data[i][0][3])/2;
		greenGrid[i][M][3] = (data[i-1][M-1][3]+data[i][M-1][3])/2;
	}
	for (int j = 1; j < M; j++)
	{
		greenGrid[0][j][2] = (data[0][j-1][2]+data[0][j][2])/2;
		greenGrid[N][j][2] = (data[N-1][j-1][2]+data[N-1][j][2])/2;

		greenGrid[0][j][3] = (data[0][j-1][3]+data[0][j][3])/2;
		greenGrid[N][j][3] = (data[N-1][j-1][3]+data[N-1][j][3])/2;
	}
	//inizializzazione griglia interna
	for (int i = 1; i < N; i++)
	{
		for (int j = 1; j < M; j++)
		{
			greenGrid[i][j][2] = (data[i-1][j-1][2]+data[i][j-1][2]+
					data[i-1][j][2]+data[i][j][2])/4;
			greenGrid[i][j][3] = (data[i-1][j-1][3]+data[i][j-1][3]+
					data[i-1][j][3]+data[i][j][3])/4;
		}
	}





	GLfloat vertices[greenN*greenM*3];
	int k = 0;
	for (int i = 0; i < greenN; i++)
	{
		for (int j = 0; j < greenM; j++)
		{
			vertices[k] = greenGrid[i][j][0];
			vertices[k+1] = greenGrid[i][j][1];
			vertices[k+2] = greenGrid[i][j][2];
			k+=3;
		}
	}

	//deallocate data
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < M; ++j)
		{
			delete [] data[i][j];
		}
		delete [] data[i];
	}
	delete [] data;


	//GLfloat temperatures[greenN*greenM];
	GLfloat lavaVertices[greenN*greenM*3];//FIXME andrebbe greenN*greenM*24 ma va in segmentation fault
	int numLavaVertices = 0;
	int kappa = 0;
	for (int i = 0; i < greenN; i++)
	{
		for (int j = 0; j < greenM; j++)
		{
			if (greenGrid[i][j][3] != 0){
				/*
				 * aggiungi i 4 vertici adiacenti a lavaVertices e i 6 indici a lavaIndices
				 */
				lavaVertices[kappa]   = greenGrid[i][j][0];
				lavaVertices[kappa+1] = greenGrid[i][j][1];
				lavaVertices[kappa+2] = greenGrid[i][j][2];
				lavaVertices[kappa+3] = greenGrid[i][j][3];

				lavaVertices[kappa+4] = greenGrid[i+1][j][0];
				lavaVertices[kappa+5] = greenGrid[i+1][j][1];
				lavaVertices[kappa+6] = greenGrid[i+1][j][2];
				lavaVertices[kappa+7] = greenGrid[i+1][j][3];

				lavaVertices[kappa+8]  = greenGrid[i][j+1][0];
				lavaVertices[kappa+9]  = greenGrid[i][j+1][1];
				lavaVertices[kappa+10] = greenGrid[i][j+1][2];
				lavaVertices[kappa+11] = greenGrid[i][j+1][3];

				lavaVertices[kappa+12] = greenGrid[i+1][j][0];
				lavaVertices[kappa+13] = greenGrid[i+1][j][1];
				lavaVertices[kappa+14] = greenGrid[i+1][j][2];
				lavaVertices[kappa+15] = greenGrid[i+1][j][3];

				lavaVertices[kappa+16] = greenGrid[i+1][j+1][0];
				lavaVertices[kappa+17] = greenGrid[i+1][j+1][1];
				lavaVertices[kappa+18] = greenGrid[i+1][j+1][2];
				lavaVertices[kappa+19] = greenGrid[i+1][j+1][3];

				lavaVertices[kappa+20]  = greenGrid[i][j+1][0];
				lavaVertices[kappa+21]  = greenGrid[i][j+1][1];
				lavaVertices[kappa+22] = greenGrid[i][j+1][2];
				lavaVertices[kappa+23] = greenGrid[i][j+1][3];
				numLavaVertices++;
				kappa+=24;
			}
		}
	}

	//deallocate greenGrid
	for (int i = 0; i < greenN; ++i)
	{
		for (int j = 0; j < greenM; ++j)
		{
			delete [] greenGrid[i][j];
		}
		delete [] greenGrid[i];
	}
	delete [] greenGrid;

	//calcolo normale per ogni cella: calcolo una normale per i due triangoli e poi faccio la media tra le due oppure assegno a tutti i vertici di uno stesso triangolo la stessa normale
	//calcolo normale per ogni face So for a triangle p1, p2, p3, if the vector U = p2 - p1 and the vector V = p3 - p1 then the normal N = U X V and can be calculated by: Nx = UyVz - UzVy; Ny = UzVx - UxVz; Nz = UxVy - UyVx

	/*vec3 U = gl_in[1].gl_Position - gl_in[0].gl_Position;
	vec3 V = gl_in[2].gl_Position - gl_in[0].gl_Position;
	vec3 N(U.z*V.x-U.z*V.y, U.z*V.x-U.x*V.z, U.x*V.y-U.y*V.x);
	 */

	GLfloat normals[greenN*greenM*3];

	int p = 0;
	for (int row = 0; row < greenN-1; row++)
	{
		for (int col = 0; col < greenM-1; col++)
		{
			GLfloat U[3];
			GLfloat V[3];
			U[0] = vertices[3*(((row+1)*greenM)+col)] - vertices[3*((row*greenM)+col)];
			U[1] = vertices[3*(((row+1)*greenM)+col)+1] - vertices[3*((row*greenM)+col)+1];
			U[2] = vertices[3*(((row+1)*greenM)+col)+2] - vertices[3*((row*greenM)+col)+2];
			V[0] = vertices[3*((row*greenM)+col+1)] - vertices[3*((row*greenM)+col)];
			V[1] = vertices[3*((row*greenM)+col+1)+1] - vertices[3*((row*greenM)+col)+1];
			V[2] = vertices[3*((row*greenM)+col+1)+2] - vertices[3*((row*greenM)+col)+2];

			//fare media tra le 6 normali invece che considerare solo quella della faccia corrente
			normals[p] = U[2]*V[0]-U[2]*V[1]; //x
			normals[p+1] = U[2]*V[0]-U[0]*V[2]; //y
			normals[p+2] = U[0]*V[1]-U[1]*V[0]; //z

			normals[p+3] = U[2]*V[0]-U[2]*V[1]; //x
			normals[p+4] = U[2]*V[0]-U[0]*V[2]; //y
			normals[p+5] = U[0]*V[1]-U[1]*V[0]; //z

			normals[3*(((row+1)*greenM)+col)] = U[2]*V[0]-U[2]*V[1]; //x
			normals[3*(((row+1)*greenM)+col)+1] = U[2]*V[0]-U[0]*V[2]; //y
			normals[3*(((row+1)*greenM)+col)+2] = U[0]*V[1]-U[1]*V[0]; //z

			p+=3;
		}
	}

	GLuint indices[N*M*6];//numero di triangoli da disegnare*6
	int t = 0;
	for (int row = 0; row < N; row++)
	{
		for (int col = 0; col < M; col++)
		{
			indices[t] = (row*greenM)+col;
			indices[t+1] = ((row+1)*greenM)+col;
			indices[t+2] = (row*greenM)+col+1;
			indices[t+3] = ((row+1)*greenM)+col;
			indices[t+4] = ((row+1)*greenM)+col+1;
			indices[t+5] = (row*greenM)+col+1;
			t+=6;
		}
	}

	GLfloat textCoords[greenN*greenM*2];
	int robin = 0;
	for (int i = 0; i < greenN; i++)
		for (int j = 0; j < greenM; j++)
		{
			textCoords[robin] = (cellSize*j)/(cellSize*(greenM-1));
			textCoords[robin+1] = ((cellSize*(greenN-1))-(cellSize*i))/(cellSize*(greenN-1));
			robin+=2;
		}

	cout << "numLavaVertices is " << numLavaVertices << endl;

	GLuint VBO, VAO, EBO, NMLS, TEXT;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NMLS);
	glGenBuffers(1, &TEXT);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//mappa da solo gli indici inseriti nell'array targettato in GL ELEMENT ARRAY (indices) con le coordinate inserite nell'array in GL_ARRAY_BUFFER (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//riempie il vertex array object con il buffer inserito in GL_ARRAY_BUFFER (vbo)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, NMLS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, TEXT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textCoords), textCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	//lavaVertices
	GLuint lavaVAO, lavaVBO;
	glGenVertexArrays(1, &lavaVAO);
	glGenBuffers(1,&lavaVBO);
	glBindVertexArray(lavaVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lavaVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lavaVertices), lavaVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(3);
	//	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//	glEnableVertexAttribArray(3);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	//unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	// Load and create a texture
	GLuint terrainTexture;
	// ====================
	// Texture 1
	// ====================
	glGenTextures(1, &terrainTexture);
	glBindTexture(GL_TEXTURE_2D, terrainTexture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	//	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	//	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int widthText, heightText;

	unsigned char* image = SOIL_load_image(FileSystem::getPath("resources/textures/texture.png").c_str(), &widthText, &heightText, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthText, heightText, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Load textures
//	GLuint terrainTexture = loadTexture(FileSystem::getPath("resources/textures/texture.png").c_str());


	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		Do_Movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// Activate shader
		ourShader.Use();
		//light
		GLint objectColorLoc = glGetUniformLocation(ourShader.Program, "objectColor");
		GLint lightColorLoc  = glGetUniformLocation(ourShader.Program, "lightColor");
		GLint lightPosLoc    = glGetUniformLocation(ourShader.Program, "lightPos");
		GLint viewPosLoc     = glGetUniformLocation(ourShader.Program, "viewPos");
		glUniform3f(objectColorLoc, 0.5f, 0.25f, 0.0f);
		glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);

		// Create transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 10000.0f);
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glm::mat4 model;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); //comando non indicizzato
		/*
		 * //comando indicizzato, si prende gli indici presenti nel buffer targettato in GL ELEMENT ARRAY,
		 *  il count si riferisce sempre al numero di vertici e non al numero di elementi disegnati.
		 *  In questo caso quindi si disegnano sempre 6 vertici e non 6 triangoli
		 */

		// Bind Textures using texture units
			glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, terrainTexture);
		glUniform1i(glGetUniformLocation(ourShader.Program, "terrainTexture"), 0);

//		glBindTexture(GL_TEXTURE_2D, terrainTexture);
		glDrawElements(GL_TRIANGLES, N*M*6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// Draw lava vertices
		lavaShader.Use();
		modelLoc = glGetUniformLocation(lavaShader.Program, "model");
		viewLoc  = glGetUniformLocation(lavaShader.Program, "view");
		projLoc  = glGetUniformLocation(lavaShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(lavaVAO);
		//TODO creare indici per disegnare i vertici della lava!
		//FIXME calcolare il vero numero di elementi da disegnare
		glDrawArrays(GL_TRIANGLES, 0, numLavaVertices*6);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if(keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime*100);
	if(keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime*100);
	if(keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime*100);
	if(keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime*100);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(action == GLFW_PRESS)
		keys[key] = true;
	else if(action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void readFile(const char* file)
{
	string line;
	ifstream myfile ("/home/napster/altitudes.dat");
	int nLine = 0;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			nLine++;
			istringstream iss(line);
			string token;
			if (nLine == 1 || nLine == 2 || nLine == 5)
			{
				int nToken = 0;
				while (std::getline(iss, token, '\t'))
				{
					if (token != "")
					{
						if (nToken == 1) {
							if (nLine == 1){
								M = atoi(token.c_str());
							}
							else if (nLine == 2){
								N = atoi(token.c_str());
								const GLint EN = N;
								const GLint EM = M;
								data = new GLfloat **[EN];
								for (int count = 0; count < EN; count++)
								{
									data[count] = new GLfloat*[EM];
								}
								for (int count = 0; count < EN; count++)
								{
									for (int column = 0; column < EM; column++)
										data[count][column] = new GLfloat[4];
								}
							}
							else if (nLine == 5)
							{
								std::string::size_type sz;     // alias of size_t
								cellSize = stod(token,&sz);
							}
						}
						nToken++;
					}
				}
			}

			else if(nLine > 6)
			{
				int j = 0;
				while (std::getline(iss, token, ' '))
				{
					if (token != "" && j < M)
					{
						std::string::size_type sz;     // alias of size_t
						double altit = stod(token,&sz);
						/*
						 * scorri tutta la riga e assegna ogni valore trovato, dopo averlo convertito a double
						 * ad un valore della matrice, colonna data dal num di token
						 */
						data[nLine-7][j][0] = (GLfloat) ((nLine-7)*cellSize);
						data[nLine-7][j][1] = (GLfloat) (j*cellSize);
						data[nLine-7][j][2] = (GLfloat) (altit);
						j++;
						//cout << (nLine-7) << " " << data[nLine-7][j][0] << " " << data[nLine-7][j][1] << endl;
					}
				}
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

void readTemperatures() {

	string line;
	ifstream myfile ("/home/napster/temperature.dat");
	int nLine = 0;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			nLine++;
			istringstream iss(line);
			string token;

			if(nLine > 6)
			{
				int j = 0;
				while (std::getline(iss, token, ' '))
				{
					if (token != "" && j < M)
					{
						std::string::size_type sz;     // alias of size_t
						double temp = stod(token,&sz);
						/*
						 * assegna il valore della temperatura per quel vertice
						 */
						data[nLine-7][j][3] = (GLfloat) (temp);
						j++;
					}
				}
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";

}

GLuint loadTexture(GLchar const * path)
{
	// Generate texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width,height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;

}


