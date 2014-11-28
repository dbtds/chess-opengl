#include "GraphicModelChess.hpp"
#include <iostream>
using namespace std;

GraphicModelChess::GraphicModelChess()
{
}

GraphicModelChess::~GraphicModelChess()
{
}

Point2D<float> GraphicModelChess::convertBackToChessPos(float x, float y)
{
    Point2D<float> point;
    point.x = (y + 1.75) / 0.5;
    point.y = (x + 1.75) / 0.5;
    return point;
}

GraphicModelChess* GraphicModelChess::generatePreviewSquare(Point2D<float> p, float R, float G, float B, float scale, float zIndex)
{
    GraphicModelChess *obj = new GraphicModelChess();
    obj->piece = NULL;
    lerVerticesDeFicheiro("../src/models/marker.obj", &obj->numVertices, &obj->arrayVertices, &obj->arrayNormais);
    obj->kAmb[0] = 0.1 + 0.9 * R;
    obj->kAmb[1] = 0.1 + 0.9 * G;
    obj->kAmb[2] = 0.1 + 0.9 * B;
    obj->kAmb[3] = 1.0;
    obj->kDif[0] = 0.64 + 0.3 * R;
    obj->kDif[1] = 0.64 + 0.3 * G;
    obj->kDif[2] = 0.64 + 0.3 * B;
    obj->kDif[3] = 1.0;
    obj->kEsp[0] = 0.9;
    obj->kEsp[1] = 0.9;
    obj->kEsp[2] = 0.9;
    obj->kEsp[3] = 1.0;
    obj->coefPhong = 100;
    obj->desl.x = p.x;
    obj->desl.y = p.y;
    obj->desl.z = zIndex;
    obj->anguloRot.x = 0;
    obj->anguloRot.y = 0;
    obj->anguloRot.z = 0;
    obj->factorEsc.x = scale;
    obj->factorEsc.y = scale;
    obj->factorEsc.z = scale;
    return obj;
}