/*
 * callbacks.c
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#define GLEW_STATIC /* Necessario se houver problemas com a lib */
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include "globals.hpp"
#include "callbacks.hpp"
#include "consoleIO.hpp"

using namespace std;
/* Callback functions */
void myAnimationTimer(int value);
void produceModelsShading(GraphicModelChess *obj)
{
    glEnableVertexAttribArray(attribute_coord3d);
    glEnableVertexAttribArray(attribute_normal3d);
    GLfloat ambientTerm[3];
    GLfloat diffuseTerm[3];
    GLfloat specularTerm[3];
    /* Caracteristicas do array de coordenadas */
    glVertexAttribPointer(attribute_coord3d,        // attribute
                          3,                        // number of elements per vertex, here (x,y,z)
                          GL_FLOAT,                 // the type of each element
                          GL_FALSE,                 // take our values as-is
                          0,                        // no extra data between each position
                          obj->arrayVertices);      // pointer to the C array

    glVertexAttribPointer(attribute_normal3d,       // attribute
                          3,                        // number of elements per vertex, here (R,G,B)
                          GL_FLOAT,                 // the type of each element
                          GL_FALSE,                 // take our values as-is
                          0,                        // no extra data between each position
                          obj->arrayNormais);       // pointer to the C array
    

    matrizModelView = IDENTITY_MATRIX;
    Translate(&matrizModelView, obj->desl.x, obj->desl.y, obj->desl.z);
    RotateAboutX(&matrizModelView, DegreesToRadians(obj->anguloRot.x));
    RotateAboutY(&matrizModelView, DegreesToRadians(obj->anguloRot.y));
    RotateAboutZ(&matrizModelView, DegreesToRadians(obj->anguloRot.z));
    /* Diminuir o tamanho do modelo para nao sair fora do view volume */
    Scale(&matrizModelView, obj->factorEsc.x, obj->factorEsc.y, obj->factorEsc.z);
    /* Matriz de projeccao e Matriz de transformacao */
    glUniformMatrix4fv(glGetUniformLocation(programaGLSL, "matrizProj"), 1, GL_FALSE, matrizProj.m);
    glUniformMatrix4fv(glGetUniformLocation(programaGLSL, "matrizModelView"), 1, GL_FALSE, matrizModelView.m);

    for (int i = 0; i < 3; i++)
    {
        ambientTerm[i] = obj->kAmb[i] * lights->intensidadeLuzAmbiente[i];
        diffuseTerm[i] = obj->kDif[i] * lights->intensidadeFLuz[i];
        specularTerm[i] = obj->kEsp[i] * lights->intensidadeFLuz[i];
    }

    glUniform4fv(glGetUniformLocation(programaGLSL, "ambientTerm"), 1, ambientTerm);
    glUniform4fv(glGetUniformLocation(programaGLSL, "diffuseTerm"), 1, diffuseTerm);
    glUniform4fv(glGetUniformLocation(programaGLSL, "specularTerm"), 1, specularTerm);
    glUniform4fv(glGetUniformLocation(programaGLSL, "posicaoFLuz"), 1, lights->posicaoFLuz);
    glUniform1f(glGetUniformLocation(programaGLSL, "coefPhong"), obj->coefPhong);
    glDrawArrays(GL_TRIANGLES, 0, obj->numVertices);

    glDisableVertexAttribArray(attribute_coord3d);
    glDisableVertexAttribArray(attribute_normal3d);
}
void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programaGLSL);

    /* Chess pieces */
    /* [0-15] - Player 1 */ 
    /* [16-31] - Player 2 */
    for (int modelId = 0; modelId < pieceModels.size(); modelId++)
    {
        GraphicModelChess *obj = &pieceModels[modelId];

        Point2D<float> nPos = GraphicModelChess::convertChessPos(chess->getPosition(obj->piece));
        obj->desl.x = nPos.x;
        obj->desl.y = nPos.y;
        produceModelsShading(obj);
    }
    /* Secondary models includes only chess table atm */
    if (chessTable != NULL)
        produceModelsShading(chessTable);
    if (selectedFrame != NULL)
        produceModelsShading(selectedFrame);

    for (int modelId = 0; modelId < previewPositions.size(); modelId++)
    {
        GraphicModelChess *obj = &previewPositions[modelId];
        produceModelsShading(obj);
    }
    glutSwapBuffers();
}

void refreshPreviewPanels()
{
    previewPositions.clear();
    vector<Point2D<int> > pp = chess->getPossiblePositions(pieceModels[selectedModel].piece);
    previewPositions.push_back(GraphicModelChess::generatePreviewSquare(
                                   GraphicModelChess::convertChessPos(
                                       chess->getPosition(pieceModels[selectedModel].piece)
                                   ), 1, 1, 0, 0.9, 0.01)
                              );
    for (vector<Point2D<int> >::iterator it = pp.begin(); it != pp.end(); ++it)
    {
        if (!chess->isFieldEmpty(*it))
            previewPositions.push_back(GraphicModelChess::generatePreviewSquare(
                                           GraphicModelChess::convertChessPos(*it), 1, 0, 0, 0.9, 0.01)
                                      );
        else
            previewPositions.push_back(GraphicModelChess::generatePreviewSquare(
                                           GraphicModelChess::convertChessPos(*it), 0, 1, 0, 0.9, 0.01)
                                      );
    }
}

void refreshSelectedPosition() {
    if (selectedPosition == -1) {
        selectedFrame = NULL;
        return;
    }

    Point2D<int> vec = chess->getPossiblePositions(pieceModels[selectedModel].piece)[selectedPosition];
    selectedFrame = new GraphicModelChess(GraphicModelChess::generatePreviewSquare(
                                            GraphicModelChess::convertChessPos(vec), 0, 1, 1, 0.8, 0.02)
                                          );
}

void myKeyboard(unsigned char key, int x, int y)
{
    int i;
    GraphicModelChess *obj;
    vector<Point2D<int> > pp;

    bool changed;
    switch (key)
    {
    case 'Q' :
    case 'q' :
    case 27  :  exit(EXIT_SUCCESS);
    case '+' :
        if ((chess->getCurrentPlayer() == ONE && selectedModel < 15) || 
            (chess->getCurrentPlayer() == TWO && selectedModel < 31)) 
        {
            selectedModel++;
            selectedPosition = -1;
            refreshPreviewPanels();
            refreshSelectedPosition();
            glutPostRedisplay();
        }
        break;
    case '-' :
        if ((chess->getCurrentPlayer() == ONE && selectedModel > 0) || 
            (chess->getCurrentPlayer() == TWO && selectedModel > 16)) 
        {
            selectedModel--;
            selectedPosition = -1;
            refreshPreviewPanels();
            refreshSelectedPosition();
            glutPostRedisplay();
        }
        break;
    case '.':
        obj = &pieceModels[selectedModel];
        pp = chess->getPossiblePositions(obj->piece);
        if (pp.size() != 0) {
            selectedPosition = (selectedPosition + 1) % pp.size();
            refreshSelectedPosition();
            glutPostRedisplay();
        }
        break;
    case ',':
        obj = &pieceModels[selectedModel];
        pp = chess->getPossiblePositions(obj->piece);
        obj = &pieceModels[selectedModel];
        pp = chess->getPossiblePositions(obj->piece);
        if (pp.size() != 0) {
            selectedPosition = (selectedPosition - 1) % pp.size();
            refreshSelectedPosition();
            glutPostRedisplay();
        }
        break;
    case 'p':
    case 'P':
        obj = &pieceModels[selectedModel];
        cout << chess->getCurrentPlayer() << ", player: " << obj->piece->player << endl;
        cout << obj->piece->getType() << endl;
        pp = chess->getPossiblePositions(obj->piece);
        for (vector<Point2D<int> >::iterator it = pp.begin(); it != pp.end(); ++it)
            cout << it->x << "," << it->y << " | ";
        cout << endl << *chess;
        break;
    case 'm':
    case 'M':
        obj = &pieceModels[selectedModel];
        pp = chess->getPossiblePositions(obj->piece);
        if (selectedPosition < pp.size() && selectedPosition >= 0)
        {
            if (chess->move(obj->piece, pp[selectedPosition])) {
                glutTimerFunc(1000, myAnimationTimer, 0);
                if (chess->getCurrentPlayer() == ONE)
                    selectedModel = 0;
                else
                    selectedModel = 16;
                selectedPosition = -1;
                refreshSelectedPosition();
                refreshPreviewPanels();
                glutPostRedisplay();
            }
        }
        
    }
}


void mySpecialKeys(int key, int x, int y)
{
    GraphicModelChess *obj = chessTable;
    switch (key)
    {
    case GLUT_KEY_LEFT :
        RotateAboutZ(&matrizProj, DegreesToRadians(-1));
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT :
        RotateAboutZ(&matrizProj, DegreesToRadians(1));
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP :
        RotateAboutY(&matrizProj, DegreesToRadians(1));
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN :
        RotateAboutY(&matrizProj, DegreesToRadians(-1));
        glutPostRedisplay();
        break;
    }
}



void onMouse(int button, int state, int x, int y)
{
    if (state != GLUT_DOWN)
        return;
}

void myAnimationTimer(int value)
{
    RotateAboutZ(&matrizProj, DegreesToRadians(1));
    glutPostRedisplay();
    if (value + 1 < 180)
        glutTimerFunc(10, myAnimationTimer, value + 1);
}

void registarCallbackFunctions(void)
{
    refreshPreviewPanels();
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecialKeys);
    glutMouseFunc(onMouse);
}