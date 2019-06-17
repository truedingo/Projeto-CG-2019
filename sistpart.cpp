#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include "RgbImage.h"
#include <time.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#define frand()			((float)rand()/RAND_MAX)
#define MAX_PARTICULAS  2500



//---------------------------------------- Particle attributes
typedef struct {
	float   size;		// tamanho
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posicao
	GLfloat vx, vy, vz; // velocidade 
    GLfloat ax, ay, az; // aceleracao
} Particle;

Particle  particula1[MAX_PARTICULAS];
GLint    milisec = 1000; 


//---------------------------------------------- Texturas
GLuint  texture[1];
RgbImage imag;

char	 filename[1][12] = {	"brilho0.bmp" };

void initTextures()
{   
	//----------------------------------------- Cubo
	glGenTextures(1, texture);
    for (int i=0;i<1;i++)
    {
        imag.LoadBmpFile(filename[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, 3,
                     imag.GetNumCols(),
                     imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                     imag.ImageData());        
    }	
}


//==========================================================
void iniParticulas(Particle *particula)
{
 GLfloat v, theta, phi;
 int i;
 GLfloat px, py, pz;
 GLfloat ps;

	px = -20.0;
	py = 200.0;
	pz = -200.0;
	ps = 4.5;



 for(i=0; i<MAX_PARTICULAS; i++)  {

	//---------------------------------  
	v     = 1*frand()+0.02;
    theta = 2.0*frand()*M_PI;   // [0..2pi]
	phi   = frand()*M_PI;		// [0.. pi]
    
    particula[i].size = ps ;		// tamanh de cada particula
    particula[i].x	  = px + 0.1*frand()*px;    // [-200 200]
    particula[i].y	  = py + 0.1*frand()*py;	// [-200 200]
    particula[i].z	  = pz + 0.1*frand()*pz;	// [-200 200]
        
	particula[i].vx = v * cos(theta) * sin(phi);	// esferico
    particula[i].vy = v * cos(phi);
    particula[i].vz = v * sin(theta) * sin(phi);
	particula[i].ax = 0.01f;
    particula[i].ay = -0.01f;
    particula[i].az = 0.015f;

	particula[i].r = 1.0f;
	particula[i].g = 1.0f;	
	particula[i].b = 1.0f;	
	particula[i].life = 1.0f;		                
	particula[i].fade = 0.001f;	// Em 100=1/0.01 iteracoes desaparece
	}
}


//==================================================================== INIT _ openGL
void iniOpenGL(void)
{
 glClearColor(0.0f,0.0f,0.0f,0.0f);      
 glEnable(GL_DEPTH_TEST);
 glEnable(GL_BLEND);	        		
 glBlendFunc(GL_SRC_ALPHA,GL_ONE);	
 glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
 glEnable(GL_TEXTURE_2D);
 initTextures();
 iniParticulas(particula1);
 
}


void showParticulas(Particle *particula, int sistema) {
 int i;
 int numero;

 numero=(int) (frand()*10.0);
 
 for (i=0; i<MAX_PARTICULAS; i++)
	{

	glColor4f(1,1,1, particula[i].life);
 	glBegin(GL_QUADS);				        
		glTexCoord2d(0,0); glVertex3f(particula[i].x -particula[i].size, particula[i].y -particula[i].size, particula[i].z);      
		glTexCoord2d(1,0); glVertex3f(particula[i].x +particula[i].size, particula[i].y -particula[i].size, particula[i].z);        
		glTexCoord2d(1,1); glVertex3f(particula[i].x +particula[i].size, particula[i].y +particula[i].size, particula[i].z);            
		glTexCoord2d(0,1); glVertex3f(particula[i].x -particula[i].size, particula[i].y +particula[i].size, particula[i].z);       
	glEnd();	
	particula[i].x += particula[i].vx;
    particula[i].y += particula[i].vy;
    particula[i].z += particula[i].vz;
    particula[i].vx += particula[i].ax;
    particula[i].vy += particula[i].ay;
    particula[i].vz += particula[i].az;
	particula[i].life -= particula[i].fade;	
	}

}
//================================================================ DRAW  main
void display(void)
{


 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(75.0f,1.0f,0.1f,1400.0f);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt (250.0, 200.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


 showParticulas(particula1, 1);
 
 glutSwapBuffers();
 
}


//==========================================  Functions
void idle(void)
{

 glutPostRedisplay();

}

void Timer(int value) 
{
	iniParticulas(particula1);

	glutPostRedisplay();
	glutTimerFunc(milisec ,Timer, 1);   //.. Espera msecDelay milisegundos
}


void keyboard(unsigned char key, int x, int y)
{
 int numero;

 switch (key)
   {
	case 'E':  case 'e':
	iniParticulas(particula1);
	
 
	break;
	//--------------------------- Escape
	case 27:
		exit(0);
		break;
   }
}

//================================================================
int main(int argc, char **argv)
{
 glutInit(&argc, argv);
 glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowSize (600, 600);
 glutInitWindowPosition (200, 100);
 glutCreateWindow ("Sistema de Particulas en OpenGL");
 iniOpenGL();
 glutDisplayFunc(display);
 glutKeyboardFunc(keyboard);
 glutIdleFunc(idle);
 glutTimerFunc(milisec , Timer, 1);     //.. fun��o callback

 glutMainLoop();
 return 0;
}

