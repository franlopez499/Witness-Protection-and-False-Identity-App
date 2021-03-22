#include "controlador.h"
#include "facedetector.h"
#include "facedetectorcascade.h"
#include <QFileDialog>
#include <math.h>
#include <time.h>

Controlador* Controlador::instance = nullptr;
static VideoCapture cap;
static VideoWriter writer;
static bool grabarActivado = false;
static bool redNeuronalProfunda = true;
Controlador::Controlador()
{

}
Controlador::~Controlador()
{
    delete instance;
}
Controlador* Controlador::getInstance()
{
    if (instance == nullptr) instance = new Controlador();
    return instance;
}

void Controlador::activarOpcionGrabar(bool opcion) {
    grabarActivado = opcion;
}
void Controlador::activarAlgoritmoDeteccion(bool opcion) {
    redNeuronalProfunda = opcion;
}

void Controlador::drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers);
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3];
    merge(rgb, 3, transp);
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}


void Controlador::blurFacesPrecise(QString &selector)
{
    if(selector == nullptr){
        cap.open(0);
    }else {
        // Abrir con nombre
        cap.open(selector.toLatin1().data());
    }
    QString nombre = "";
    if (grabarActivado) {

        nombre = QFileDialog::getSaveFileName();
    }

    if(cap.isOpened()){
        Mat frame;
        int num_frames = 0;
        time_t start, end;
        double fps = -1;
        time(&start);
        while (waitKey(1)==-1) {
            Mat frame;

            if (!cap.read(frame))
                break;
            num_frames++;
            if(num_frames == 120 && grabarActivado){
                time(&end);

                double seconds = difftime (end, start);

                fps  = 120 / seconds;
                writer.open(nombre.toLatin1().data(), VideoWriter::fourcc('D','I','V','X'),
                            fps, Size(cap.get(CAP_PROP_FRAME_WIDTH),cap.get(CAP_PROP_FRAME_HEIGHT)));
                if (!writer.isOpened()) {
                    qDebug("No se puede crear %s.", nombre.toLatin1().data());
                    return;
                }
            }
            vector<Rect> rectangles;
            if(redNeuronalProfunda)
                rectangles = FaceDetector::getInstance()->detect_face_rectangles(frame);
            else{
                rectangles = FaceDetectorCascade::getInstance()->detectFaces(frame);
            }
            Scalar color(0, 105, 205);

            for(const auto & r : rectangles){
                blur(frame(r),frame(r), Size(51,51));
            }


            if (!grabarActivado){
                namedWindow("Presione ESC para salir", 0);
                imshow("Presione ESC para salir", frame);
            }else if(num_frames >= 120){
                namedWindow("Presione ESC para salir", 0);
                imshow("Presione ESC para salir", frame);
                writer << frame;
            }
        }

    }
    if (grabarActivado) {
        writer.release();
        grabarActivado = false;
    }
}


void Controlador::blurFacesEfficient(QString &selector)
{
    Scalar color(0, 105, 205);

    long frameCounter = 0;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);
    const int MAX_COUNT = 500;
    Mat image, prevGray,gray; //LK variables
    vector<vector<Point2f>> points[2]; // vector de pares de puntos de tamaño roi´s detectados.

    if(selector == nullptr){
        cap.open(0);
    }else {
        // Abrir con nombre
        cap.open(selector.toLatin1().data());
    }
    int contador = 0;
    QString nombre = "";
    if (grabarActivado) {
        nombre= QFileDialog::getSaveFileName();

    }

    if(cap.isOpened()){
        Mat frame;
        int num_frames = 0;
        time_t start, end;
        double fps = -1;
        time(&start);
        while (waitKey(1)==-1) {
            if (!cap.read(frame))
                return;
            num_frames++;
            if(num_frames == 120 && grabarActivado){
                time(&end);

                double seconds = difftime (end, start);

                fps  = 120 / seconds;
                writer.open(nombre.toLatin1().data(), VideoWriter::fourcc('D','I','V','X'),
                            fps, Size(cap.get(CAP_PROP_FRAME_WIDTH),cap.get(CAP_PROP_FRAME_HEIGHT)));
                if (!writer.isOpened()) {
                    qDebug("No se puede crear %s.", nombre.toLatin1().data());
                    return;
                }
            }

            frame.copyTo(image);
            cvtColor(image, gray, COLOR_BGR2GRAY);



            if(frameCounter % 10 == 0){
                vector<Rect> rectangles;
                if(redNeuronalProfunda){
                    rectangles = FaceDetector::getInstance()->detect_face_rectangles(frame);
                }
                else{
                    rectangles = FaceDetectorCascade::getInstance()->detectFaces(frame);
                }


                contador = rectangles.size();
                points[0].clear();points[1].clear();
                for(const auto & r : rectangles) {


                    vector<Point2f> vectorAnterior;
                    vector<Point2f> vectorSiguiente;
                    points[0].push_back(vectorAnterior);

                    Mat mask = Mat::zeros(frame.size(), CV_8UC1);
                    Mat roi(mask, r);
                    roi = Scalar(255, 255, 255);
                    vector<Point2f> tmp;
                    goodFeaturesToTrack(gray, tmp, MAX_COUNT, 0.01, 10, mask, 3, 3, 0, 0.04); // Mascara del mismo tamaño que el frame
                    cornerSubPix(gray, tmp, subPixWinSize, Size(-1,-1), termcrit);
                    for(auto & p : tmp){
                        vectorSiguiente.push_back(p);

                    }

                    points[1].push_back(vectorSiguiente);


                    blur(image(r),image(r),Size(51,51));

                }



            } else {


                vector<int> posicionesOK;
                for(int i = 0; i < contador; i++){
                    if(!points[0].empty() && !points[0][i].empty()){
                        posicionesOK.push_back(i);
                    }
                }


                int p = 0;
                vector<vector<uchar>> status(contador,vector<uchar>());
                for(int x = 0; x < contador;x++){

                    RotatedRect box;
                    vector<uchar> st;
                    status.push_back(st);
                    vector<float> err;
                    if(posicionesOK.empty() || x != posicionesOK[p]){
                        continue;
                    }else{
                        p++;
                    }
                    if(prevGray.empty())
                        gray.copyTo(prevGray);



                    if(points[0][x].size() > 5) {

                        box = fitEllipse(points[0][x]);
                        //ellipse(image, box, color, 3, LINE_AA);
                        Rect roi = box.boundingRect();
                        Rect region = roi;

                        if(roi.x<0){region.x=0;region.width+=roi.x;}
                        if(roi.y<0){region.y=0;region.height+=roi.y;}
                        if(roi.x+roi.width>image.cols)region.width=image.cols-roi.x;
                        if(roi.y+roi.height>image.rows)region.height=image.rows-roi.y;
                        if(region.width>image.cols)region.width=image.cols;
                        if(region.height>image.rows)region.height=image.rows;


                        blur(image(region), image(region), Size(51, 51));
                    }

                    if(points[0].size() > 0 && points[0][x].size() > 0)
                        calcOpticalFlowPyrLK(prevGray, gray, points[0][x], points[1][x], status[x], err, winSize, 3, termcrit, 0, 0.001);
                    size_t i, k;
                    for( i = k = 0; i < points[1][x].size(); i++ )
                    {
                        if( !status[x][i] )
                            continue;

                        points[1][x][k++] = points[1][x][i];

                    }

                    points[1][x].resize(k);

                }




            }
            if(!grabarActivado || num_frames >= 120){
                namedWindow("Presione ESC para salir", 0);
                imshow("Presione ESC para salir", image);
            }
            frameCounter++;
            for(int x = 0; x < contador;x++) {

                swap(points[1][x], points[0][x]);
            }
            swap(prevGray, gray);
            if (grabarActivado && num_frames >= 120)
                writer << image;
        }
    }
    if (grabarActivado) {
        writer.release();
        grabarActivado = false;
    }
}


void Controlador::putGlasses(QString & selector)
{
    Scalar color(0, 105, 205);

    long frameCounter = 0;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);


    Mat image,prevGray,gray; //LK variables
    vector<vector<Point2f>> points[2];
    int contador = 0;
    VideoCapture cap;
    if(selector == nullptr){
        cap.open(0);
    }else {
        // Abrir con nombre
        cap.open(selector.toLatin1().data());
    }
    QString nombre = "";
    if (grabarActivado) {
        nombre= QFileDialog::getSaveFileName();

    }

    if(cap.isOpened()){
        Mat frame;
        int num_frames = 0;
        time_t start, end;
        double fps = -1;
        time(&start);
        while (waitKey(1)==-1) {
            if (!cap.read(frame))
                return;
            num_frames++;
            if(num_frames == 120 && grabarActivado){
                time(&end);

                double seconds = difftime (end, start);

                fps  = 120 / seconds;
                writer.open(nombre.toLatin1().data(), VideoWriter::fourcc('D','I','V','X'),
                            fps, Size(cap.get(CAP_PROP_FRAME_WIDTH),cap.get(CAP_PROP_FRAME_HEIGHT)));
                if (!writer.isOpened()) {
                    qDebug("No se puede crear %s.", nombre.toLatin1().data());
                    return;
                }

            }

            frame.copyTo(image);
            cvtColor(image, gray, COLOR_BGR2GRAY);
            if(frameCounter % 10 == 0){
                vector<Rect> rectangles;
                if(redNeuronalProfunda)
                    rectangles = FaceDetector::getInstance()->detect_face_rectangles(image);
                else{
                    rectangles = FaceDetectorCascade::getInstance()->detectFaces(image);
                }
                contador = rectangles.size();
                points[0].clear();points[1].clear();
                for(const auto & r : rectangles){

                    vector<Point2f> vectorAnterior;
                    vector<Point2f> vectorSiguiente;
                    points[0].push_back(vectorAnterior);



                    Mat mask = Mat::zeros(image.size(), CV_8UC1);
                    Mat roi(mask, r);
                    roi = Scalar(255, 255, 255);

                    vector<Rect_<int>> eyes = FaceDetectorCascade::getInstance()->detectEyes(gray(Rect(r.x, r.y, r.width, r.height)));


                    if(eyes.size() == 2){
                        Rect shiftedRectFirstEye = Rect(eyes[0] + Point(r.x, r.y));
                        Rect shiftedRectSecondEye = Rect(eyes[1] + Point(r.x, r.y));
                        if(shiftedRectFirstEye.x > shiftedRectSecondEye.x){
                            swap(shiftedRectFirstEye,shiftedRectSecondEye);
                        }

                        float first_point_x = shiftedRectFirstEye.x+ shiftedRectFirstEye.width/2;
                        float first_point_y = shiftedRectFirstEye.y+ shiftedRectFirstEye.height/2;

                        float second_point_x = shiftedRectSecondEye.x+ shiftedRectSecondEye.width/2;
                        float second_point_y = shiftedRectSecondEye.y+ shiftedRectSecondEye.height/2;
                        int umbral = 50;
                        // Cabeza girada al maximo es 48
                        double rotacion = (atan2(second_point_y-first_point_y,second_point_x-first_point_x) * 180 )/ M_PI;

                        // Comprobamos que sean dos "ojos" y no sean falsos positivos (¿estan en linea los ojos?)
                        if(abs(rotacion) < umbral){


                            Point2f vPerpendicular = Point2f(first_point_y-second_point_y, second_point_x-first_point_x);
                            Point2f vectorUnitario = Point2f(second_point_x-first_point_x,second_point_y-first_point_y);
                            Point2f ojoIzq = Point2f(first_point_x,first_point_y);
                            Point2f ojoDer = Point2f(second_point_x,second_point_y);
                            // arribaIzq ((x1,y1) -0.2 * v) - (y1-y2,x2-x1)

                            Point2f pruebaArribaIzq = Point2f(ojoIzq - 0.5*vectorUnitario) - (0.4* vPerpendicular);
                            Point2f pruebaAbajoIzq = Point2f(ojoIzq - 0.5*vectorUnitario) + (0.3 * vPerpendicular);

                            // arribaDerecha ((x2,y2) + 0.2*v) -0.3 * (y1-y2,x2-x1)
                            Point2f pruebaArribaDer = Point2f(ojoDer + 0.5*vectorUnitario) - (0.4* vPerpendicular);
                            Point2f pruebaAbajoDer = Point2f(ojoDer + 0.5*vectorUnitario) + (0.3* vPerpendicular);


                            Mat gafas = imread(PROJECT_PATH "gafus.png",IMREAD_UNCHANGED);

                            int gafasWidth = gafas.cols;
                            int gafasHeight = gafas.rows;

                            Point2f puntosSalida[4] = {pruebaArribaIzq,pruebaAbajoIzq,pruebaAbajoDer,pruebaArribaDer};
                            Point2f puntosEntrada[4] = {Point2f(0,0),Point2f(0,gafasHeight),Point2f(gafasWidth,gafasHeight),Point2f(gafasWidth,0)};

                            Mat M = getAffineTransform(puntosEntrada,puntosSalida);
                            Mat imageWarped;
                            cvtColor(image,imageWarped, COLOR_BGR2BGRA);
                            warpAffine(gafas,imageWarped,M, imageWarped.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0,0));
                            drawTransparency(image,imageWarped,0,0);


                            vectorSiguiente.push_back(ojoIzq);
                            vectorSiguiente.push_back(ojoDer);
                        }

                    }
                    points[1].push_back(vectorSiguiente);
                }


            } else {


                vector<int> posicionesOK;
                for(int i = 0; i < contador; i++){
                    if(!points[0].empty() && !points[0][i].empty()){
                        posicionesOK.push_back(i);
                    }
                }


                int p = 0;
                vector<vector<uchar>> status(contador,vector<uchar>());
                for(int x = 0; x < contador;x++){

                    RotatedRect box;
                    vector<uchar> st;
                    status.push_back(st);
                    vector<float> err;
                    if(posicionesOK.empty() || x != posicionesOK[p]){
                        continue;
                    }else{
                        p++;
                    }
                    if(prevGray.empty())
                        gray.copyTo(prevGray);

                    if(points[0].size() > 0 && points[0][x].size() > 0)
                        calcOpticalFlowPyrLK(prevGray, gray, points[0][x], points[1][x], status[x], err, winSize, 3, termcrit, 0, 0.001);

                    Mat gafas = imread(PROJECT_PATH "gafus.png",IMREAD_UNCHANGED);
                    int gafasWidth = gafas.cols;
                    int gafasHeight = gafas.rows;

                    float first_point_x = points[1][x][0].x;
                    float first_point_y = points[1][x][0].y;
                    float second_point_x = points[1][x][1].x;
                    float second_point_y = points[1][x][1].y;
                    // Vperpendicular= (-y,x)
                    // v (unitario) = (x2-x1, y2-y1)
                    // Fórmula: (x1,y1) + a*v + b*Vperpendicular


                    Point2f vPerpendicular = Point2f(first_point_y-second_point_y, second_point_x-first_point_x);
                    Point2f vectorUnitario = Point2f(second_point_x-first_point_x,second_point_y-first_point_y);

                    // arribaIzq ((x1,y1) -0.2 * v) - (y1-y2,x2-x1)

                    Point2f pruebaArribaIzq = Point2f(points[1][x][0] - 0.5*vectorUnitario) - (0.4* vPerpendicular);
                    Point2f pruebaAbajoIzq = Point2f(points[1][x][0] - 0.5*vectorUnitario) + (0.3 * vPerpendicular);

                    // arribaDerecha ((x2,y2) + 0.2*v) -0.3 * (y1-y2,x2-x1)
                    Point2f pruebaArribaDer = Point2f(points[1][x][1] + 0.5*vectorUnitario) - (0.4* vPerpendicular);
                    Point2f pruebaAbajoDer = Point2f(points[1][x][1] + 0.5*vectorUnitario) + (0.3* vPerpendicular);



                    Point2f puntosSalida[4] = {pruebaArribaIzq, pruebaAbajoIzq, pruebaAbajoDer, pruebaArribaDer};
                    Point2f puntosEntrada[4] = {Point2f(0,0),Point2f(0,gafasHeight),Point2f(gafasWidth,gafasHeight),Point2f(gafasWidth,0)};

                    Mat M = getAffineTransform(puntosEntrada,puntosSalida);
                    Mat imageWarped;
                    cvtColor(image,imageWarped, COLOR_BGR2BGRA);
                    warpAffine(gafas,imageWarped,M, imageWarped.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0,0));
                    drawTransparency(image,imageWarped,0,0);


                    size_t i, k;
                    for( i = k = 0; i < points[1][x].size(); i++ )
                    {

                        points[1][x][k++] = points[1][x][i];

                    }

                    points[1][x].resize(k);

                }


            }
            if(!grabarActivado || num_frames >= 120){
                namedWindow("Presione ESC para salir", 0);
                imshow("Presione ESC para salir", image);
            }
            frameCounter++;
            for(int x = 0; x < contador;x++) {

                swap(points[1][x], points[0][x]);
            }
            swap(prevGray, gray);
            if (grabarActivado && num_frames >=120)
                writer << image;

        }
    }
    if (grabarActivado) {
        writer.release();
        grabarActivado = false;
    }
}

