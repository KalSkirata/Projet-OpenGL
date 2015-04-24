#if defined(__APPLE__) || defined(MACOSX)
# include <GLUT/glut.h> 
#else
# include <GL/glut.h>
#endif

#include<stdio.h>
#include"glimage.h"
#include"glm.h"

#define  GL_GLEXT_PROTOTYPES

#define INFO if(0) printf

#define XMAX 300
#define YMAX 300
#define ZMAX 500

#define NB_ENM 40
#define SIZE_ENM 10
#define SIZE_MISSILE 8
#define SPEED_MISSILE 2

#define ESC 27
#define SPACE_BAR 32

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void init();
void init_texture();
void init_ennemis();
void display();
void display_decor_fond();
void display_decor();
void drawShip();
void display_model();
void drawCube();
void drawModel(int);
static void InitBufferObject(GLuint, int, GLuint *, GLvoid *);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void SpecialInput(int, int, int);
void detectCollision();
void idle();
void shoot();
void shoot_ennemis();
void display_Obs_Affiche();
void display_missiles_ennemis();

GLMmodel *model_tie=NULL, *model_missile=NULL;
int pause = 0, level = 0, another_shoot = 1, display_missile = 1, hp = 3, shoot_enm=0;
//another_shoot : tirer à nouveau ?		display_missile : afficher le misssile ? (cas de collision)
GLuint idBoDecor = 0, idBoFond = 0, idBoEnnemis = 0, idBoFace = 0, idBoSommets = 0;
static GLfloat camx = 0.0, camy = YMAX/2, camz = ZMAX, missile_x, missile_y, missile_z;
static GLuint texDecor=0, texDeathStar = 0;

struct Obstacle{
  GLfloat x, y, z;
};
typedef struct Obstacle Obstacle;

Obstacle Obs[NB_ENM];
int Obs_Affiche[NB_ENM]; //tableau qui contiendra des booleans pour indiquer quels sont les vaisseaux à afficher

struct Missile_ennemis{
	GLfloat x,y,z;
	int another_shoot,display;
};
typedef struct Missile_ennemis Missile_ennemis;

Missile_ennemis Missiles_enm[NB_ENM];

//le décor contient 5 quads représentant les vues de devant, du dessus, du dessous, de la droite et de la gauche
GLfloat decor[] = {
							//FACE AVANT
							-XMAX,YMAX,0.0, //coordonnees du sommet
							0.0,1.0,			 //coordonnees de texture
							XMAX,YMAX,0.0,	
							1.0,1.0,
							XMAX,0.0,0.0,
							1.0,0.0,
							-XMAX,0.0,0.0,
							0.0,0.0,
							
							//FACE DESSOUS
							-XMAX,0.0,0.0,
							0.0,1.0,
							-XMAX,0.0,ZMAX,
							0.0,0.0,
							XMAX,0.0,ZMAX,
							1.0,0.0,
							XMAX,0.0,0.0,
							1.0,1.0,
							
							//FACE DROITE
							XMAX,0.0,0.0,
							0.0,0.0,
							XMAX,0.0,ZMAX,
							1.0,0.0,
							XMAX,YMAX,ZMAX,
							1.0,1.0,
							XMAX,YMAX,0.0,
							0.0,1.0,
							
							//FACE DESSUS
							XMAX,YMAX,0.0,
							1.0,1.0,
							XMAX,YMAX,ZMAX,
							1.0,0.0,
							-XMAX,YMAX,ZMAX,
							0.0,0.0,
							-XMAX,YMAX,0.0,
							0.0,1.0,
							
							//FACE GAUCHE
							-XMAX,YMAX,0.0,
							1.0,1.0,
							-XMAX,0.0,0.0,
							1.0,0.0,
							-XMAX,0.0,ZMAX,
							0.0,0.0,
							-XMAX,YMAX,ZMAX,
							0.0,1.0
};

GLfloat vbo_ennemis[] = {0.0,0.0,0.0, //face avant
								 0.0,0.0,
								 0.0,1.0,0.0,
								 0.0,1.0,
								 1.0,1.0,0.0,
								 1.0,1.0,
								 1.0,0.0,0.0,
								 1.0,0.0,
								 
								 1.0,0.0,0.0, //face dessous
								 1.0,0.0,
								 1.0,0.0,1.0,
								 1.0,1.0,
								 0.0,0.0,1.0,
								 0.0,0.1,
								 0.0,0.0,0.0,
								 0.0,0.0,
								 
								 0.0,0.0,0.0, //face gauche
								 0.0,0.0,
								 0.0,1.0,0.0,
								 0.0,1.0,
								 0.0,1.0,1.0,
								 1.0,1.0,
								 0.0,0.0,1.0,
								 1.0,0.0,
								 
								 0.0,0.0,1.0, //face arriere
								 0.0,0.0,
								 0.0,1.0,1.0,
								 0.0,1.0,
								 1.0,1.0,1.0,
								 1.0,1.0,
								 1.0,0.0,1.0,
								 1.0,0.0,
								 
								 1.0,0.0,1.0, //face droite
								 1.0,0.0,
								 1.0,0.0,0.0,
								 0.0,0.0,
								 1.0,1.0,0.0,
								 0.0,1.0,
								 1.0,1.0,1.0,
								 1.0,1.0,
								 
								 1.0,1.0,1.0, //face dessus
								 1.0,1.0,
								 0.0,1.0,1.0,
								 0.0,1.0,
								 0.0,1.0,0.0,
								 0.0,0.0,
								 1.0,1.0,0.0,
								 1.0,0.0
};

void init(){
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(decor), &idBoDecor, decor);
	InitBufferObject(GL_ARRAY_BUFFER, sizeof(vbo_ennemis), &idBoEnnemis, vbo_ennemis);
	glClearColor(0.0,0.0,0.0,0.0);
 	glShadeModel(GL_SMOOTH);
  	glEnable(GL_DEPTH_TEST);
  	glEnable(GL_TEXTURE_2D);
  	glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

	init_texture();
	init_ennemis();
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void init_texture(){
  glimageLoadAndBind("textures/space.jpg", &texDecor);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glimageLoadAndBind("textures/deathstar.jpg", &texDeathStar);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void init_ennemis(){
  int i;
  for(i=0;i<NB_ENM; i++){
  	 //Initlisation de la postion des vaisseaux ennemis
    Obs[i].x = rand()%(XMAX*2)-XMAX;
    Obs[i].y = rand()%YMAX;
    Obs[i].z = rand()%ZMAX;
    INFO("x=%f y=%f z=%f \n",Obs[i].x,Obs[i].y,Obs[i].z);
 
 	 //Au début du jeu, on affiche tous les vaisseaux ennemis
    Obs_Affiche[i] = 1;
    
    //Tous les vaisseaux ennemis peuvent tirer
    Missiles_enm[i].another_shoot=1;
  }
}
void display(){
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	glLoadIdentity();
  	gluLookAt(camx,camy,camz, camx,camy,0.0, 0.0,1.0,0.0);
  	display_decor_fond();
 	display_decor();
 	display_model();
	glutSwapBuffers();
}

//dessine le fond du décor
void display_decor_fond(){
  glBindBuffer(GL_ARRAY_BUFFER, idBoDecor);
  glBindTexture(GL_TEXTURE_2D, texDeathStar);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof (GLfloat)));
  
  glDrawArrays(GL_QUADS, 0, 4);
  
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//dessine le reste du décor
void display_decor(){
  glBindBuffer(GL_ARRAY_BUFFER, idBoDecor);
  glBindTexture(GL_TEXTURE_2D, texDecor);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glVertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
  glTexCoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof (GLfloat)));
  
  glDrawArrays(GL_QUADS, 0, 4*5);
  
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawShip(){
	/*glPushAttrib (GL_ALL_ATTRIB_BITS);
 	glDisable(GL_TEXTURE_2D);
 	glColor3f(1.0, 1.0, 1.0);
  	if (!model_tie) {
 	 model_tie = glmReadOBJ("obj/star-wars-x-wing.obj");
   //model_tie = glmReadOBJ("obj/xmas-ball.obj");
    if (!model_tie) exit(0);
	
    glmUnitize(model_tie);
    glmScale(model_tie, SIZE_ENM);

    glmFacetNormals(model_tie);
    glmVertexNormals(model_tie, 90.0);
  } 
  glmDraw(model_tie, GLM_SMOOTH | GLM_MATERIAL);
  glPopAttrib ();*/
}

void display_model(){
	int i;
	for(i=0;i<NB_ENM;i++){
		//Affichage des vaisseaux ennemis
		if(Obs_Affiche[i] == 1){
			glPushMatrix();
			glTranslatef(Obs[i].x, Obs[i].y, Obs[i].z);
			drawModel(0);
			glPopMatrix();
		}
		
		//Affichage des missiles ennemis
		if(Missiles_enm[i].x!=0 && Missiles_enm[i].y!=0 && Missiles_enm[i].z!=0 && Missiles_enm[i].display){
			glPushMatrix();
			glTranslatef(Missiles_enm[i].x, Missiles_enm[i].y, Missiles_enm[i].z);
			glRotatef(180.0,0.0,1.0,0.0);
			drawModel(1);
			glPopMatrix();
		}
	}
	
	//Affichage du missile lancé depuis le vaisseau
	if(/*missile_x!=0 && missile_y!=0 && missile_z!=0 && */display_missile){
		glPushMatrix();
		glTranslatef(missile_x, missile_y, missile_z);
		drawModel(1);
		glPopMatrix();
	}
}

void drawModel(int choice){  
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);
	if(choice==0){ //vaisseaux ennemis
		if(!model_tie) {
			model_tie = glmReadOBJ("obj/TieFighter.obj");
			if (!model_tie) exit(0);
	
			glmUnitize(model_tie);
			glmScale(model_tie, SIZE_ENM);

			glmFacetNormals(model_tie);
			glmVertexNormals(model_tie, 90.0);
		} 
		glmDraw(model_tie, GLM_SMOOTH | GLM_MATERIAL);
	}else{ //missile
		if(!model_missile){
			model_missile = glmReadOBJ("obj/missile/missile.obj");
			if (!model_missile) exit(0);
	
			glmUnitize(model_missile);
			glmScale(model_missile, SIZE_MISSILE);

			glmFacetNormals(model_missile);
			glmVertexNormals(model_missile, 90.0);
		} 
		glmDraw(model_missile, GLM_SMOOTH | GLM_MATERIAL);
	}
	glPopAttrib();
}

static void InitBufferObject(GLuint TYPE, int size, GLuint *id, GLvoid *data){
  int sizeInByte;
  if (!glIsBuffer(*id)) glGenBuffers(1, id);

  glBindBuffer(TYPE, *id);
  glBufferData(TYPE, size, data, GL_STATIC_DRAW);

  glGetBufferParameteriv(TYPE, GL_BUFFER_SIZE, &sizeInByte );
  INFO("Taille du Buffer %s : \t %d octets\n", (TYPE==GL_ARRAY_BUFFER)?"Buffer Array":"Element Array", sizeInByte);
  glBindBuffer(TYPE, 0);
}

void reshape(int w, int h){
  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  glMatrixMode (GL_PROJECTION); 
  glLoadIdentity (); 
  gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.1, 1000.0); 
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
}

void keyboard (unsigned char key, int x, int y) { 
	switch (key){
		case ESC :
			exit(0);
			break;
		case SPACE_BAR :
			shoot();
			break;
		case 'p' :
			if(pause==0){
				glutIdleFunc(NULL);
				glutSpecialFunc(NULL);
				pause = 1;
			}else{
				glutIdleFunc(idle);
				glutSpecialFunc(SpecialInput);
				pause = 0;
			}
			break;
		default :
			break;
	}
	glutPostRedisplay();
}

void SpecialInput(int key, int x, int y){
	switch(key){
		case GLUT_KEY_UP:
			if(camy < YMAX-20) camy++;
 			break;
		case GLUT_KEY_DOWN:
			if(camy > 20) camy--;
			break;
		case GLUT_KEY_LEFT:
			if(camx > -XMAX+20) camx--;
			break;
		case GLUT_KEY_RIGHT:
			if(camx < XMAX-20) camx++;
			break;
	}
	glutPostRedisplay();
}

//collision cubique
void detectCollision(){
	int i;
	for(i=0; i<NB_ENM; i++){
		if(Obs_Affiche[i]){ //ne prendre en compte que les ennemis affichés
			//Collision vaisseau-ennemis
			if ((camx<=Obs[i].x+SIZE_ENM) && (camx>=Obs[i].x-SIZE_ENM) &&
			(camy<=Obs[i].y+SIZE_ENM) && (camy>=Obs[i].y-SIZE_ENM) &&
			(camz<=Obs[i].z+SIZE_ENM) && (camz>=Obs[i].z-SIZE_ENM)){
				printf("You're dead, Captain but %d ennemies are dead with you\n",shoot_enm);
				exit(0);
			}
			
			//Collision missile-ennemis
			if ((missile_x<=Obs[i].x+SIZE_ENM) && (missile_x>=Obs[i].x-SIZE_ENM) &&
			(missile_y<=Obs[i].y+SIZE_ENM) && (missile_y>=Obs[i].y-SIZE_ENM) &&
			(missile_z<=Obs[i].z+SIZE_ENM) && (missile_z>=Obs[i].z-SIZE_ENM)){
				Obs_Affiche[i] = 0;
				another_shoot = 1;
				display_Obs_Affiche();
				display_missile = 0;
				shoot_enm++;
			}
			
			//Collision missile-vaisseau
			if((camx<=Missiles_enm[i].x+SIZE_MISSILE) && (camx>=Missiles_enm[i].x-SIZE_MISSILE) &&
			(camy<=Missiles_enm[i].y+SIZE_MISSILE) && (camy>=Missiles_enm[i].y-SIZE_MISSILE) &&
			(camz<=Missiles_enm[i].z+SIZE_MISSILE) && (camz>=Missiles_enm[i].z-SIZE_MISSILE)){
				//le joueur perd un point de vie
				if(hp>0){
				hp--;
				printf("HP = %d \n",hp);			
				Missiles_enm[i].another_shoot=1; //l'ennemi peut tirer un autre missile
				shoot_ennemis(); //réinitialisation de la postion du missile
				}else{
					printf("You're dead, Captain but %d ennemies are dead with you\n",shoot_enm);
					exit(0);
				}
			}
			
			//Collision missile-missile
			if ((missile_x<=Missiles_enm[i].x+SIZE_MISSILE) && (missile_x>=Missiles_enm[i].x-SIZE_MISSILE) &&
			(missile_y<=Missiles_enm[i].y+SIZE_MISSILE) && (missile_y>=Missiles_enm[i].y-SIZE_MISSILE) &&
			(missile_z<=Missiles_enm[i].z+SIZE_MISSILE) && (missile_z>=Missiles_enm[i].z-SIZE_MISSILE)){
				another_shoot = 1;
				display_Obs_Affiche();
				display_missile = 0;
				
				Missiles_enm[i].another_shoot=1;
				Missiles_enm[i].display=0;
			}
		}
	}
}

//effectue le mouvement automatique du vaisseau et des missiles (alliés et ennemis)
void idle(){
	if(camz < 100){
		printf("Level %d completed\n",level);
		level++;
		
		camx = 0.0;
		camy = YMAX/2;
		camz = ZMAX;
		
		init_ennemis();
	}else{
		//Mouvement du vaisseau du joueur
		camz-=level+1;
		detectCollision();
		
		//Mouvement du missile allié
		missile_z-=level+SPEED_MISSILE;
		if(missile_z>=0 && missile_z<=10) another_shoot=1;
		
		//Mouvement des missiles ennemis
		
		shoot_ennemis(); //initialisation de la position du missile
		//Mouvement du missile
		int i;
		for(i=0; i<NB_ENM; i++){
			//Mouvement du missile
			if(/*Obs_Affiche[i] && */Missiles_enm[i].x!=0 && Missiles_enm[i].y!=0 && Missiles_enm[i].z!=0) Missiles_enm[i].z+=SPEED_MISSILE;
			//Si le missile ennemi passe derrière le joueur, l'ennemi peut tirer un autre missile
			if(Missiles_enm[i].z > camz){
				Missiles_enm[i].another_shoot=1;
				shoot_ennemis(); //on réinitialise la position du missile
			}
		}
	}
 	display_missiles_ennemis();
	glutPostRedisplay();
}

//gère le tir du joueur
void shoot(){
	INFO("another_shoot=%d display_missile=%d x=%f y=%f z=%f\n",another_shoot,display_missile,missile_x,missile_y,missile_z);
	if(another_shoot){
		another_shoot = 0;
		display_missile = 1;
		missile_x = camx;
		missile_y = camy;
		missile_z = camz-10;
	}
}

//initialise la position du missile enemi
void shoot_ennemis(){
	int i;
	for(i=0; i<NB_ENM; i++){
		//en ne prenant en compte que les ennemis encore présents
		//si le vaisseau du joueur approche d'un vaisseau ennemi, ce dernier tire
		if(Obs_Affiche[i] && camx>=Obs[i].x-50 && camx<=Obs[i].x+50 && camy<=Obs[i].y+50 && camx>=Obs[i].x-50 && Missiles_enm[i].another_shoot){
			Missiles_enm[i].x=Obs[i].x;
			Missiles_enm[i].y=Obs[i].y;
			Missiles_enm[i].z=Obs[i].z+10;
			Missiles_enm[i].another_shoot=0;
			Missiles_enm[i].display=1;
		}
	}
}

//Affiche la liste des vaisseaux ennemis à afficher
void display_Obs_Affiche(){
	INFO("Vaisseaux a afficher");
	int i;
	for(i=0; i<NB_ENM; i++){
		if(i%10 == 0) INFO("\n");
		INFO("%d",Obs_Affiche[i]);
	}
	INFO("\n");
}

//Affiche la liste des missiles ennemis
void display_missiles_ennemis(){
	INFO("Missile ennemis \n");
	int i;
	for(i=0; i<NB_ENM; i++){
		INFO("%f %f %f %d %d\n", Missiles_enm[i].x, Missiles_enm[i].y, Missiles_enm[i].z, Missiles_enm[i].another_shoot,Missiles_enm[i].display);
	}
	INFO("\n");
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	
	init();
		
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialInput);
	glutIdleFunc(idle);	
	
	glutMainLoop();
   return 0;
}
