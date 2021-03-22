#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QColorDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

#include "FaceDetector.h"
#include "facedetectorcascade.h"
#include "controlador.h"
using namespace cv;
using namespace std;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_4_clicked()
{

    QString selector = nullptr;
    if(ui->radioButton->isChecked()){
        cout << "Camara" << endl;
    }else{
        selector = QFileDialog::getOpenFileName();

    }

    Controlador::getInstance()->activarOpcionGrabar(ui->check_Box->isChecked());
    Controlador::getInstance()->activarAlgoritmoDeteccion(ui->radioButton_3->isChecked());


    Controlador::getInstance()->blurFacesPrecise(selector);


}




void MainWindow::on_pushButton_6_clicked()
{


    QString selector = nullptr;
    if(ui->radioButton->isChecked()){
        cout << "Camara" << endl;
    }else{
        selector = QFileDialog::getOpenFileName();
    }

    Controlador::getInstance()->activarOpcionGrabar(ui->check_Box->isChecked());
    Controlador::getInstance()->activarAlgoritmoDeteccion(ui->radioButton_3->isChecked());
    Controlador::getInstance()->blurFacesEfficient(selector);
}

void MainWindow::on_pushButton_7_clicked()
{

    QString selector = nullptr;
    if(ui->radioButton->isChecked()){
        cout << "Camara" << endl;
    }else{
        selector = QFileDialog::getOpenFileName();
    }

    Controlador::getInstance()->activarOpcionGrabar(ui->check_Box->isChecked());
    Controlador::getInstance()->activarAlgoritmoDeteccion(ui->radioButton_3->isChecked());
    Controlador::getInstance()->putGlasses(selector);
}






