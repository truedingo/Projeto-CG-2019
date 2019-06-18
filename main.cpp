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


//==================================================================== Definir cores
#define AZUL     0.0, 0.0, 1.0, 1.0
#define VERMELHO 1.0, 0.0, 0.0, 1.0
#define AMARELO  1.0, 1.0, 0.0, 1.0
#define VERDE    0.0, 1.0, 0.0, 1.0
#define LARANJA  0.8, 0.6, 0.1, 1.0
#define WHITE    1.0, 1.0, 1.0, 1.0
#define BLACK    0.0, 0.0, 0.0, 1.0
#define GRAY1    0.2, 0.2, 0.2, 1.0
#define GRAY2    0.9, 0.9, 0.9, 1.0
#define PRETO    0.0, 0.0, 0.0, 1.0

#define   PI 3.14159

using namespace std;

//================================================================================
//===========================================================Variaveis e constantes

//------------------------------------------------------------ Sistema Coordenadas
GLfloat  xC=16.0, zC=15.0, xC2=8.0;

GLint    wScreen=800, hScreen=500;
GLint    msec     = 100;                    //.. definicao do timer (actualizacao)

//------------------------------------------------------------ Observador
GLfloat  rVisao=1.0, aVisao=0.5*PI, incVisao=0.1;
GLfloat  angPersp=109.0;
GLfloat  obsPini[] ={0, 6.0, static_cast<GLfloat>(0.5*xC)};
GLfloat  obsPfin[] ={obsPini[0]-rVisao*cos(aVisao), obsPini[1], obsPini[2]-rVisao*sin(aVisao)};

//thread para movimento
pthread_t ball2_thread, ball1_thread;

//flags de controlo
bool col_flag1=0;
bool col_flag2=0;
bool accel_flag1=0;
bool accel_flag2=0;


//=================================================================== TEXTURAS

//------------------------------------------------------------ Skybox
GLUquadricObj*  bola = gluNewQuadric ( );

//ball coordinates
GLfloat b1x     = -13.5;
GLfloat b1y     = 7.8;
GLfloat b1z     = -6;
GLfloat b2x     = 13.5;
GLfloat b2y     = 7.8;
GLfloat b2z     = -6;

//reset ball coordinates
GLfloat rb1x     = -13.5;
GLfloat rb1y     = 7.8;
GLfloat rb1z     = -6;
GLfloat rb2x     = 13.5;
GLfloat rb2y     = 7.8;
GLfloat rb2z     = -6;



//collision particles
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

Particle  col_particulas1[MAX_PARTICULAS];
Particle  col_particulas2[MAX_PARTICULAS];

//------------------------------------------------------------ Texturas
GLuint   texture[6];
RgbImage imag;

//------------------------------------------------------------ Iluminacao Ambiente
GLint   dia=0;
GLfloat intensidade=0.5;
GLfloat luzGlobalCorAmb[4]={intensidade,intensidade,intensidade,1.0};   // 


//--------------------------------------------- Bola e Escada
//------------------------------
int   verticesESC= 3*52;    // 53 vertices (x,y,z) = 53*3
int   facesESC   = 10;        // 53/4

static GLuint    poligono[] = {};
static GLfloat vertices[]={
    -0.500000, -0.000000, -0.960000, 0.500000, -0.000000, -0.960000, 0.500000, 0.000000, -0.000000, -0.500000, 0.000000, -0.000000, -0.500000, 0.000000, -0.960000, 0.500000, 0.000000, -0.960000, 0.500000, 0.160000, -0.960000, -0.500000, 0.160000, -0.960000, -0.500000, 0.160000, -1.280000, 0.500000, 0.160000, -1.280000, 0.500000, 0.160000, -0.960000, -0.500000, 0.160000, -0.960000, -0.500000, 0.160000, -1.280000, 0.500000, 0.160000, -1.280000, 0.500000, 0.320000, -1.280000, -0.500000, 0.320000, -1.280000, -0.500000, 0.320000, -1.600000, 0.500000, 0.320000, -1.600000, 0.500000, 0.320000, -1.280000, -0.500000, 0.320000, -1.280000, -0.500000, 0.320000, -1.600000, 0.500000, 0.320000, -1.600000, 0.500000, 0.480000, -1.600000, -0.500000, 0.480000, -1.600000, -0.500000, 0.480000, -2.560000, 0.500000, 0.480000, -2.560000, 0.500000, 0.480000, -1.600000, -0.500000, 0.480000, -1.600000, -0.500000, 0.480000, -2.560000, 0.500000, 0.480000, -2.560000, 0.500000, 0.640000, -2.560000, -0.500000, 0.640000, -2.560000, -0.500000, 0.640000, -2.880000, 0.500000, 0.640000, -2.880000, 0.500000, 0.640000, -2.560000, -0.500000, 0.640000, -2.560000, -0.500000, 0.640000, -2.880000, 0.500000, 0.640000, -2.880000, 0.500000, 0.800000, -2.880000, -0.500000, 0.800000, -2.880000, -0.500000, 0.800000, -3.200000, 0.500000, 0.800000, -3.200000, 0.500000, 0.800000, -2.880000, -0.500000, 0.800000, -2.880000, -0.500000, 0.800000, -3.200000, 0.500000, 0.800000, -3.200000, 0.500000, 0.960000, -3.200000, -0.500000, 0.960000, -3.200000, -0.500000, 0.960000, -4.160000, 0.500000, 0.960000, -4.160000, 0.500000, 0.960000, -3.200000, -0.500000, 0.960000, -3.200000
};
static GLfloat normais[] = {
    0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000
};
static GLfloat cores[]={
    0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 1.000000, 0.000000
};
static GLfloat arrayTexture[]={
    0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1,  0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1
};

//����������������������������������������������������� Lanterna (local) - FOCO
GLint   ligaFoco=0;
GLfloat rFoco=1.1, aFoco=aVisao;
GLfloat incH =0.0, incV=0.0;
GLfloat incMaxH =0.5, incMaxV=0.35;   
GLfloat focoPini[]= { obsPini[0], obsPini[1], obsPini[2], 1.0 };
GLfloat focoPfin[]= { obsPini[0]-rFoco*cos(aFoco), obsPini[1], obsPini[2]-rFoco*sin(aFoco), 1.0};
GLfloat focoDir[] = { focoPfin[0]-focoPini[0], 0, focoPfin[2]-focoPini[2]};
GLfloat focoExp   = 10.0;
GLfloat focoCut   = 60.0;

GLfloat focoCorDif[4] ={ 10.0, 10.0, 10.0, 1.0}; 
GLfloat focoCorEsp[4] ={ 1.0, 1.0, 1.0, 1.0};


GLfloat lPos[] = {0.0, 30.0, 50.0, 0.0};   //luz pontual
GLfloat lAmb[4]={0.5,0.5,0.5,1.0};					//intensidade da luz ambiente a 50%
GLfloat lEspec[4]={0.7,0.7, 0.7,0.7};// "brilho"
GLfloat lDif[4]={0.3,0.3,0.3,0.3};	   // "cor"

bool active=false;

//================================================================================
//=========================================================================== INIT
//================================================================================
void initTexturas()
{
    //----------------------------------------- Chao
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    imag.LoadBmpFile("chao.bmp");
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());
    
    
    //----------------------------------------- Bola
    glGenTextures(1, &texture[1]);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    imag.LoadBmpFile("wall.bmp");
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());
    
    //-----------------------------------------  Cobertor
    glGenTextures(1, &texture[2]);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    imag.LoadBmpFile("wood.bmp");
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());
    //-----------------------------------------  Piso
    glGenTextures(1, &texture[3]);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    imag.LoadBmpFile("wood.bmp");
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());
    

    //-----------------------------------------  Bola 1

    glGenTextures(1, &texture[4]);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    imag.LoadBmpFile("2.bmp");
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());

    glGenTextures(1, &texture[5]);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    imag.LoadBmpFile("3.bmp");
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());



}


//========================================================= ILUMINACAO                 
void initLights(void){
	//����������������������������������������������������� Ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lAmb);
	//����������������������������������������������������� Foco
	glLightfv(GL_LIGHT1, GL_POSITION,      focoPini );
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,focoDir );
	glLightf (GL_LIGHT1, GL_SPOT_EXPONENT ,focoExp);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF,   focoCut);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,       focoCorDif );   
	glLightfv(GL_LIGHT1, GL_SPECULAR,      focoCorEsp  );

        // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lAmb);
    glLightfv(GL_LIGHT0, GL_POSITION, lPos );
	glLightfv(GL_LIGHT0, GL_SPECULAR, lEspec);
	
	if (ligaFoco)
	   glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);
	
}

//==========================================================
void initParticulas(Particle *particula, int ballid)
{
 GLfloat v, theta, phi;
 int i;
 GLfloat px, py, pz;
 GLfloat ps;

    if(ballid == 1){
        px = b1x;
	    py = b1y;
	    pz = b1z;
	    ps = 0.05;
    }
    else{
        px = b2x;
	    py = b2y;
	    pz = b2z;
	    ps = 0.05;
    }




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
	particula[i].b = 0.0f;	
	particula[i].life = 1.0f;		                
	particula[i].fade = 0.001f;	// Em 100=1/0.01 iteracoes desaparece
	}
}

void showParticulas(Particle *particula, int sistema, int texture_no) {
 int i;
 int numero;

 numero=(int) (frand()*10.0);
 
 for (i=0; i<MAX_PARTICULAS; i++)
	{
     glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D,texture[texture_no]);
	//glColor4f(particula[i].r,particula[i].g,particula[i].b, particula[i].life);
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
     glDisable(GL_TEXTURE_2D);
	}
}


//================================================================================
//================== OBJECTOS
//================================================================================

void drawChao(){
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Chao y=0
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    //glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0,0,0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);       glVertex3i( -xC,  0, -xC );
    glTexCoord2f(1.0f, 0.0f);      glVertex3i( -xC,   0,  xC );
    glTexCoord2f(1.0f, 1.0f);    glVertex3i(  xC,   0,  xC);
    glTexCoord2f(0.0f, 1.0f);    glVertex3i(  xC,     0,  -xC);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}



void drawParedeFrente(){
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Chao y=0
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(-90,1,0,0);
    glTranslatef(0,16,8);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);       glVertex3i( -xC,  0, -xC2 );
    glTexCoord2f(1.0f, 0.0f);      glVertex3i( -xC,   0,  xC2 );
    glTexCoord2f(1.0f, 1.0f);    glVertex3i(  xC,   0,  xC2);
    glTexCoord2f(0.0f, 1.0f);    glVertex3i(  xC,     0,  -xC2);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawParedeDireita(){
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Chao y=0
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(-90,0,0,1);
    glTranslatef(-8,16,0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);       glVertex3i( -xC2,  0, -xC );
    glTexCoord2f(1.0f, 0.0f);      glVertex3i( -xC2,   0,  xC );
    glTexCoord2f(1.0f, 1.0f);    glVertex3i(  xC2,   0,  xC);
    glTexCoord2f(0.0f, 1.0f);    glVertex3i(  xC2,     0,  -xC);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawParedeEsquerda(){
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Chao y=0
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(-90,0,0,1);
    glTranslatef(-8,-16,0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);       glVertex3i( -xC2,  0, -xC );
    glTexCoord2f(1.0f, 0.0f);      glVertex3i( -xC2,   0,  xC );
    glTexCoord2f(1.0f, 1.0f);    glVertex3i(  xC2,   0,  xC);
    glTexCoord2f(0.0f, 1.0f);    glVertex3i(  xC2,     0,  -xC);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawBola( )
{
	//------------------------- Bola
    
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture[4]);
	glPushMatrix();		
    	glTranslatef (b1x,b1y,b1z);
    	gluQuadricDrawStyle ( bola, GLU_FILL   );
		gluQuadricNormals   ( bola, GLU_SMOOTH );
		gluQuadricTexture   ( bola, GL_TRUE    );
		gluSphere ( bola, 0.5, 150, 150);
     glPopMatrix();	
	glDisable(GL_TEXTURE_2D);
        glFlush();
    glutPostRedisplay();


}

void drawBola2( )
{
	//------------------------- Bola
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture[5]);
	glPushMatrix();	
    	glTranslatef (b2x,b2y,b2z);
    	gluQuadricDrawStyle ( bola, GLU_FILL   );
		gluQuadricNormals   ( bola, GLU_SMOOTH );
		gluQuadricTexture   ( bola, GL_TRUE    );
		gluSphere ( bola, 0.5, 150, 150);
     glPopMatrix();	
	glDisable(GL_TEXTURE_2D);
    glFlush();
    glutPostRedisplay();

}

void drawEscada( )
{
     glEnable(GL_TEXTURE_2D);
    //------------------------- ESCADA
    glPushMatrix();
    glTranslatef (0, -0.01, 10.0);
    glScalef( 6, 5 ,5);
    //glColor3f(1.0f, 1.0f, 1.0f);
    
   
    for( int p = 0; p < facesESC; p++ ){
        if (p%2==0)
            glBindTexture(GL_TEXTURE_2D,texture[2]);
        else
            glBindTexture(GL_TEXTURE_2D,texture[2]);
        
        poligono[0]= p*4+0;
        poligono[1]= p*4+1;
        poligono[2]= p*4+2;
        poligono[3]= p*4+3;
        glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, poligono);
    }
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
}

void drawEscadaDireita( )
{
    glEnable(GL_TEXTURE_2D);
    //------------------------- ESCADA
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glTranslatef (-5.9, 3.99, 0.0);
    glScalef( 3, 5 ,5);
    
    for( int p = 0; p < facesESC; p++ ){
        if (p%2==0)
            glBindTexture(GL_TEXTURE_2D,texture[2]);
        else
            glBindTexture(GL_TEXTURE_2D,texture[3]);
        
        poligono[0]= p*4+0;
        poligono[1]= p*4+1;
        poligono[2]= p*4+2;
        poligono[3]= p*4+3;
        glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, poligono);
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawEscadaEsquerda( )
{
    glEnable(GL_TEXTURE_2D);
    //------------------------- ESCADA
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef (5.9, 3.99, 0.0);
    glScalef( 3, 5 ,5);
    
    
    for( int p = 0; p < facesESC; p++ ){
        if (p%2==0)
            glBindTexture(GL_TEXTURE_2D,texture[2]);
        else
            glBindTexture(GL_TEXTURE_2D,texture[3]);
        
        poligono[0]= p*4+0;
        poligono[1]= p*4+1;
        poligono[2]= p*4+2;
        poligono[3]= p*4+3;
        glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, poligono);
    }
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
}

void init(void) {
    glClearColor(WHITE);
    glShadeModel(GL_SMOOTH);
    
    initTexturas();
    glEnable(GL_DEPTH_TEST);
    
    //------------------------------- ESCADA
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normais);
    glEnableClientState(GL_NORMAL_ARRAY);
    //glColorPointer(3, GL_FLOAT, 0, cores);
    //glEnableClientState(GL_COLOR_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, arrayTexture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    	//����������������������������������������������� ILUMINACAO
	glEnable(GL_LIGHTING);  
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);    // dos dois lados
	  
	initLights();

    initParticulas(col_particulas1, 1);
    initParticulas(col_particulas2, 2);

}




GLvoid resize(GLsizei width, GLsizei height)
{
    wScreen=width;
    hScreen=height;
    glViewport( 0, 0, wScreen,hScreen );
    glutPostRedisplay();
}


void drawOrientacao()
{
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);  
    //----------------------- OBSERVADOR
    glPushMatrix();
    glColor4f(VERDE);
    glTranslatef (obsPini[0],obsPini[1],obsPini[2]);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glColor4f(PRETO);
    glTranslatef (obsPfin[0],obsPfin[1],obsPfin[2]);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);  
    
    //----------------------- EIXOS
    glColor4f(VERMELHO);
    glBegin(GL_LINES);
    glVertex3i(0,0,-xC);
    glVertex3i(0,0, xC);
    glEnd();
    glColor4f(VERDE);
    glBegin(GL_LINES);
    glVertex3i(0,-xC,0);
    glVertex3i(0, xC,0);
    glEnd();
    glColor4f(AZUL);
    glBegin(GL_LINES);
    glVertex3i(-xC,0,0);
    glVertex3i( xC,0,0);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);  
    
}

//___________________________________________________   jh
void display(void){
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    //================================================================= Viewport1
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glViewport (0, hScreen/4, wScreen/6, hScreen/5);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-xC,xC, -xC,xC, -zC,zC);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( 0, 5,0, 0,0,0, 0, 0, -1);
    //--------------------- desenha objectos
    drawOrientacao();
    drawChao();
    drawParedeFrente();
    drawParedeDireita();
    drawParedeEsquerda();
    drawBola();
    drawBola2();
    drawEscada();
    drawEscadaDireita();
    drawEscadaEsquerda();
    
    
    //================================================================= Viewport2
    glEnable(GL_LIGHTING);  
    glViewport (wScreen/5, 0, 0.75*wScreen, hScreen);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angPersp, (float)wScreen/hScreen, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obsPini[0], obsPini[1], obsPini[2], obsPfin[0], obsPfin[1], obsPfin[2], 0, 1, 0);
    
    initLights();
    drawChao();
    drawParedeFrente();
    drawParedeDireita();
    drawParedeEsquerda();

    drawEscada();
    drawEscadaDireita();
    drawEscadaEsquerda();
    drawBola();
    drawBola2();
    if(active == true){
        showParticulas(col_particulas1, 1, 4);
        showParticulas(col_particulas2, 1, 5);  
        
    }
    
    
    glutSwapBuffers();
}


void updateVisao(){
    
    obsPfin[0] =obsPini[0]+rVisao*cos(aVisao);
    obsPfin[2] =obsPini[2]-rVisao*sin(aVisao);

    focoPini[0]=obsPini[0];
	focoPini[2]=obsPini[2];
	focoPfin[0]=focoPini[0]-rFoco*cos(aFoco+incH);
	focoPfin[2]=focoPini[2]-rFoco*sin(aFoco+incH);
	focoPini[1]=obsPini[1];
	focoPini[2]=obsPini[2];
	focoDir[0] =focoPfin[0]-focoPini[0];
	focoDir[1] =incV;
	focoDir[2] =focoPfin[2]-focoPini[2];

    glutPostRedisplay();
}

bool collisionDetectionBalls(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat r1, GLfloat r2){

    return fabs((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) <= (r1+r2)*(r1+r2);
}

void *move_ball2(void *vargp){
    while(b2x > 12.5 && accel_flag2 == 0){
        usleep(5);
        b2x-=.0001;
    }
    while(b2y > 6.90 && accel_flag2 == 0){
        usleep(5);
        b2y-=.0003;
    }
    while(b2x > 7.6 && accel_flag2 == 0){
        usleep(5);
        b2x-=.0001;
    }
    while(b2y > 6 && accel_flag2 == 0){
        usleep(5);
        b2y-=.0003;
    }
    while(b2x > 6 && accel_flag2 == 0){
        usleep(5);
        b2x-=.0001;
    }
    while(b2y > 5.4 && accel_flag2 == 0){
        usleep(5);
        b2y-=.0003;
    }
    while(b2x > 4.7 && accel_flag2 == 0){
        usleep(5);
        b2x-=.0001;
    }
    while(b2y > 4.4 && accel_flag2 == 0){
        if(collisionDetectionBalls(b1x, b2x, b1y, b2y, 0.5, 0.5)){
            col_flag2 = 1;
            initParticulas(col_particulas2, 2);
            active=true;
            break;
        }
        usleep(5);
        b2y-=.0003;
    }
    while(b2x > -4 && col_flag2 == 0 && accel_flag2 == 0){
        if(collisionDetectionBalls(b1x, b2x, b1y, b2y, 0.5, 0.5)){
             //distance between ball centers

            float distance = sqrtf((b1x - b2x)*(b1x-b2x)+(b1y-b2y)*(b1y-b2y));
            float overlap = (distance - 1)/2;

            //ajustar a distância da colisão
            b2x-= overlap * (b2x - b1x) / distance;

            accel_flag2=1;
            
            //resolver colisão
            initParticulas(col_particulas2, 2);
            active=true;
            break;
        }
        usleep(5);
        b2x-=.0001;
    }
    float accel = 0.2;
    while(accel > 0){
        usleep(10000);
        b2x+=accel;
        accel-= 0.03;
        if(b2x > 4.2){
            break;
        }
    }
}

void *move_ball1(void *vargp){
    while(b1x < -12.5 && accel_flag1 == 0){
        usleep(5);
        b1x+=.0001;
    }
    while(b1y > 6.90 && accel_flag1 == 0){
        usleep(5);
        b1y-=.0003;
    }
    while(b1x < -7.6 && accel_flag1 == 0){
        usleep(5);
        b1x+=.0001;
    }
    while(b1y > 6 && accel_flag1 == 0){
        usleep(5);
        b1y-=.0003;
    }
    while(b1x < -6 && accel_flag1 == 0){
        usleep(5);
        b1x+=.0001;
    }
    while(b1y > 5.4 && accel_flag1 == 0){
        usleep(5);
        b1y-=.0003;
    }
    while(b1x < -4.7 && accel_flag1 == 0){
        usleep(5);
        b1x+=.0001;
    }
    while(b1y > 4.4 && accel_flag1 == 0){
        if(collisionDetectionBalls(b1x, b2x, b1y, b2y, 0.5, 0.5)){
            col_flag1 = 1;
            initParticulas(col_particulas1, 1);
            active=true;
            break;
        }
        usleep(5);
        b1y-=.0003;
    }
    while(b1x < 4 && col_flag1 == 0 && accel_flag1 == 0){
        if(collisionDetectionBalls(b1x, b2x, b1y, b2y, 0.5, 0.5)){
            //distance between ball centers
            float distance = sqrtf((b1x - b2x)*(b1x-b2x)+(b1y-b2y)*(b1y-b2y));
            float overlap = (distance - 1)/2;
            //ajustar a distância da colisão
            b1x+= overlap * (b1x - b2x) / distance;
            accel_flag1=1;
            initParticulas(col_particulas1, 1);
            active=true;
            break;
        }
        usleep(5);
        b1x+=.0001;
    }
    
    float accel = 0.2;
    while(accel > 0){
        usleep(10000);
        b1x-=accel;
        accel-= 0.03;
        if(b1x < -4.2){
            break;
        }
    }
}

void reset_ball1(){
    b1x = rb1x;
    b1y = rb1y;
}

void reset_ball2(){
    b2x = rb2x;
    b2y = rb2y;
}


//======================================================= EVENTOS
void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'w':
        case 'W':
            obsPini[1]=obsPini[1]+incVisao*cos(aVisao)*10;
            glutPostRedisplay();
            break;
        case 's':
        case 'S':
            obsPini[1]=obsPini[1]-incVisao*cos(aVisao)*10;
            glutPostRedisplay();
            break;
        case 'f':
	    case 'F':
		    ligaFoco=!ligaFoco;
		    glutPostRedisplay();
	        break;
        case 'l':
        case 'L':
            if (glIsEnabled(GL_LIGHT0))
                glDisable(GL_LIGHT0);
            else
                glEnable(GL_LIGHT0);
            glutPostRedisplay();
            break;
        case '1':
            col_flag1=0;
            accel_flag1=0;
            pthread_create(&ball1_thread, NULL, move_ball1, NULL);
            if(active==true){
                cout<<"entra aqui"<<endl;
                active=false;
            }
            //glutPostRedisplay();
            break;
        case '2':
            accel_flag2=0;
            col_flag2=0;
            pthread_create(&ball2_thread, NULL, move_ball2, NULL);
            glutPostRedisplay();
            break;
        case '3':
            col_flag1=0;
            accel_flag1=0;
            reset_ball1();
            break;
        case '4':
            accel_flag2=0;
            col_flag2=0;
            reset_ball2();
            break;
            //--------------------------- Escape
        case '7':
            active=false;
            break;
        case 27:
            exit(0);
            break;

            //COLLISION FLAG E VER MELHOR AS CENAS
            
    }
}

void teclasNotAscii(int key, int x, int y)
{
    if(key == GLUT_KEY_UP) {
        obsPini[0]=obsPini[0]+incVisao*cos(aVisao)*10;
        obsPini[2]=obsPini[2]-incVisao*sin(aVisao)*10;
    }
    if(key == GLUT_KEY_DOWN) {
        obsPini[0]=obsPini[0]-incVisao*cos(aVisao)*10;
        obsPini[2]=obsPini[2]+incVisao*sin(aVisao)*10;
    }
    if(key == GLUT_KEY_LEFT){
        aVisao = (aVisao + 0.1);
        aFoco=aFoco-0.1;
    }		
    if(key == GLUT_KEY_RIGHT){
        aVisao = (aVisao - 0.1);
        aFoco=aFoco+0.1;
    }
    
    updateVisao();
}


void Timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(msec,Timer, 1);
}


//======================================================= MAIN
int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize (wScreen, hScreen);
    glutInitWindowPosition (400, 100);
    glutCreateWindow ("{jh,nbarreto}@dei.uc.pt-CG      ");
    
    init();
    glutSpecialFunc(teclasNotAscii);
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(msec, Timer, 1);

    glutMainLoop();
    
    return 0;
}

