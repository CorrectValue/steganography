#ifndef LABZ_H
#define LABZ_H

#include <QtGui/QMainWindow>
#include "ui_labz.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>

#include<string>

class labz : public QMainWindow
{
	Q_OBJECT

public:
	labz(QWidget *parent = 0, Qt::WFlags flags = 0);
	~labz();

	QString srcImgName;
	IplImage* sourceImg;
	IplImage* watermark;
	IplImage* resultImg;
	IplImage* resWatermark;
	IplImage* attackedImage;
	IplImage* diffImg;
	IplImage* diffWatermark;

	QVector<int> binWatermark;
	QVector<int> returnedBinWatermark;

	int h, w;
	int interval;
	int randNum;

	int imgWidth;
	int imgHeight;

	double metricSSIM;

	//String* srcImgPath;
	//String* wmImgPath;

	void lsb();
	void img2bin();
	void bin2img();
	void decodeLSB(IplImage*);
	int bin2dec(QVector<int> bin);
	QVector<int> dec2bin(int dec);
	void changeBrightness();
	IplImage* difference(IplImage* img1, IplImage* img2);


	void BER(IplImage* img1, IplImage* img2);
	void PSNR(IplImage* img1, IplImage* img2);
	void SSIM(IplImage* img1, IplImage* img2);
	void RMS(IplImage* img1, IplImage* img2);
	void NCC(IplImage* img1, IplImage* img2);

	double avg(IplImage* img);
	double var(IplImage* img, double mu);
	double cov(IplImage* img1, IplImage* img2, double mu1, double mu2);

	void encodePRI();
	void decodePRI(IplImage* img1);

	void crop();

	void blurAttack();

	void medBlurAttack();

	void noise();

	void equIntensity();

	void encodePastorfide(IplImage* img1);
	void decodePastorfide(IplImage* img1);

	IplImage* Laplace(IplImage* img1);

	void encodeHempstalk();
	void decodeHempstalk(IplImage* img1);
	void checkImg(IplImage* img1);

	IplImage* encodeICPES();
	void decodeICPES(IplImage* img1);

	IplImage* encodeIEEE();
	void decodeIEEE(IplImage* img1);


public slots:
	void go();
	void attack();

private:
	Ui::labzClass ui;
};

#endif // LABZ_H
