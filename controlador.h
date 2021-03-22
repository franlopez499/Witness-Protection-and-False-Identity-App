#ifndef CONTROLADOR_H
#define CONTROLADOR_H
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <QString>
#include <QFileDialog>
using namespace cv;
using namespace std;

class Controlador
{
public:
    static Controlador* getInstance();
    void blurFacesEfficient(QString &selector);
    void blurFacesPrecise(QString &selector);
    void putGlasses(QString &selector);
    void activarAlgoritmoDeteccion(bool opcion);
    void activarOpcionGrabar(bool opcion);
private:
    static Controlador* instance;
    ~Controlador();
    void drawTransparency(Mat frame, Mat transp, int xPos, int yPos);
    explicit Controlador();
};

#endif // CONTROLADOR_H
