/*
 * init.c
 *
 * Ficheiro de implementacao do modulo INIT.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define GLEW_STATIC /* Necessario se houver problemas com a lib */
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "globals.hpp"
#include "utils/mathUtils.hpp"
#include "models.hpp"

void inicializarEstado(void)
{
	/* DOUBLE-BUFFERING + DEPTH-TESTING */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/* Definir a cor do fundo */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	/* Atributos das primitivas */
	glPointSize(4.0);
	glLineWidth(3.0);
	/* Modo de desenho dos poligonos */
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/* Back-Face Culling */
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Matriz de projeccao � inicialmente a IDENTIDADE => Proj. Paralela Ortogonal */
	matrizProj = IDENTITY_MATRIX;
	matrizModelView = IDENTITY_MATRIX;
	chess = new Chess();
}


void inicializarJanela(void)
{
	/* Caracteristicas da janela de saida */
	glutInitWindowSize(500, 500);   /* Usar variaveis GLOBAIS para estes parametros */
	glutInitWindowPosition(200, 200);
	/* Para terminar de modo apropriado */
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);
	/* Criar a janela de saida */
	windowHandle = glutCreateWindow("OpenGL_ex_13");
	if (windowHandle < 1)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}
}

void inicializarFontesDeLuz(void)
{
	/* Intensidade Luminosa */
	// IL
	float intensidadeFLuz_0[4];
	intensidadeFLuz_0[0] = 1.0;
	intensidadeFLuz_0[1] = 1.0;
	intensidadeFLuz_0[2] = 1.0;
	intensidadeFLuz_0[3] = 1.0;
	/* Posicao */
	float posicaoFLuz_0[4];
	posicaoFLuz_0 [0] = 0.0;
	posicaoFLuz_0 [1] = 0.0;
	posicaoFLuz_0 [2] = 10.0;
	posicaoFLuz_0 [3] = 0.0;
	/* Luz Ambiente */
	// IA
	float intensidadeLuzAmbiente[4];
	intensidadeLuzAmbiente[0] = 0.2;
	intensidadeLuzAmbiente[1] = 0.2;
	intensidadeLuzAmbiente[2] = 0.2;
	intensidadeLuzAmbiente[3] = 1.0;

	lights = new LightModel(intensidadeFLuz_0, posicaoFLuz_0, intensidadeLuzAmbiente);
}

void inicializarModelos(void)
{
	vector<ChessPiece*> list = chess->getListPieces();
	for(vector<ChessPiece*>::iterator it = list.begin(); it != list.end(); ++it) {
		GraphicModelChess empty;
		models.push_back(empty);
		
		GraphicModelChess * obj = &models[0];
		obj->piece = *it;
		if (obj->piece->getType() == "Queen")
			lerVerticesDeFicheiro("models/queen.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
		else if (obj->piece->getType() == "Bishop")
			lerVerticesDeFicheiro("models/bishop.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
		else if (obj->piece->getType() == "Pawn")
			lerVerticesDeFicheiro("models/pawn.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
		else if (obj->piece->getType() == "King")
			lerVerticesDeFicheiro("models/king.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
		else if (obj->piece->getType() == "Knight")
			lerVerticesDeFicheiro("models/knight.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
		else if (obj->piece->getType() == "Tower")
			lerVerticesDeFicheiro("models/rook.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
		
		obj->arrayCores = (GLfloat *) calloc(3 * obj->numVertices, sizeof(GLfloat));
		
		/* Propriedades do material */
		obj->kAmb[0] = 0.2;
		obj->kAmb[1] = 0.2;
		obj->kAmb[2] = 0.2;
		obj->kAmb[3] = 1.0;

		obj->kDif[0] = 0.5;
		obj->kDif[1] = 0.5;
		obj->kDif[2] = 0.5;
		obj->kDif[3] = 1.0;

		obj->kEsp[0] = 0.7;
		obj->kEsp[1] = 0.7;
		obj->kEsp[2] = 0.7;
		obj->kEsp[3] = 1.0;
		obj->coefPhong = 100;
		/* Parametros das transformacoes */
		obj->desl.x = 0;
		obj->desl.y = 0;
		obj->desl.z = -0.5;
		obj->anguloRot.x = 0;
		obj->anguloRot.y = 0;
		obj->anguloRot.z = 0;
		obj->factorEsc.x = 0.25;
		obj->factorEsc.y = 0.25;
		obj->factorEsc.z = 0.25;
	}
}

void libertarArraysGlobais(void)
{
	delete lights;
	delete chess;

	for(vector<GraphicModelChess>::iterator it = models.begin(); it != models.end(); ++it) {
		free(it->arrayVertices);
		free(it->arrayNormais);
		free(it->arrayCores);
	}
}
