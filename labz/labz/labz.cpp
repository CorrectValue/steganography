#include "labz.h"
#include "wavelet.cpp"

using namespace std;
using namespace cv;

labz::labz(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	connect(ui.btnGo, SIGNAL(clicked()), this, SLOT(go()));
	connect(ui.btnAttack, SIGNAL(clicked()), this, SLOT(attack()));
	sourceImg = cvLoadImage("img.jpg");
	watermark = cvLoadImage("flame.png");
	resWatermark = cvLoadImage("flame.png");
	attackedImage = cvLoadImage("img.jpg");
	diffImg = cvLoadImage("img.jpg");
	diffWatermark = cvLoadImage("flame.png");
}

labz::~labz()
{
	cvReleaseImage(&sourceImg);

	cvReleaseImage(&watermark);
	cvReleaseImage(&resWatermark);
	cvReleaseImage(&attackedImage);
	cvReleaseImage(&diffImg);
	cvReleaseImage(&diffWatermark);

	binWatermark.clear();
	returnedBinWatermark.clear();
}

void labz::lsb()
{

	//resultImg = sourceImg;
	resultImg = cvCloneImage(sourceImg);

	CvScalar s, s1;

	int i = 0, j = 0;
	for(int x = 0; x < binWatermark.size(); x++)
	{
		//i - ������ � ������, j - ������ ����� ������
		j=x%w;
		i=x/w;
		s = cvGet2D(sourceImg, i, j);
		s1.val[0] = s.val[0] + binWatermark[x] - (int(s.val[0]) % 2);
		s1.val[1] = s.val[1];
		s1.val[2] = s.val[2];
		cvSet2D(resultImg, i, j, s1);

	}


	cvShowImage("�������� �����������", sourceImg);
	cvShowImage("���������", watermark);
	cvShowImage("�������������� �����������", resultImg);



}

void labz::go()
{
	//��������� �����������, ������
	sourceImg = cvLoadImage("img.jpg");
	watermark = cvLoadImage("flame.png");
	h = sourceImg->height;
	w = sourceImg->width;

	//����������, �� ��� ������� ������ ��� ������
	img2bin();
	switch(ui.cmbMethod->currentIndex())
	{
	case 0:
		//LSB method

		lsb();
		decodeLSB(resultImg);
		break;
	case 1:
		//pseudo random interval method
		encodePRI();
		decodePRI(resultImg);
		break;
	case 2:
		//Pastorfide
		encodePastorfide(sourceImg);
		decodePastorfide(resultImg);
		break;
	case 3:
		//Hempstalk
		encodeHempstalk();
		decodeHempstalk(resultImg);
		break;
	case 4:
		//ICPES
		resultImg = encodeICPES();
		decodeICPES(resultImg);
		break;
	case 5:
		//IEEE
		resultImg = encodeIEEE();
		decodeIEEE(resultImg);
		break;
	}

	//������� ������
	BER(watermark, resWatermark);
	PSNR(sourceImg, resultImg);
	SSIM(watermark, resWatermark);
	NCC(watermark, resWatermark);
	RMS(sourceImg, resultImg);
	//����� �������� �����������
	diffImg = difference(sourceImg, resultImg);
	diffWatermark = difference(watermark, resWatermark);
	cvShowImage("������� ����� �������� � ���������� ������������", diffImg);
	cvShowImage("������� ����� �������� � ���������� ������� ������", diffWatermark);


}

void labz::attack()
{
	//������, ��� �������
	switch(ui.cmbAttack->currentIndex())
	{
	case 0:
		//��������� �������
		changeBrightness();
		break;
	case 1:
		crop();
		break;
	case 2:
		blurAttack();
		break;
	case 3:
		medBlurAttack();
		break;
	case 4:
		noise();
		break;
	case 5:
		equIntensity();
		break;
	}
	//�������� ������ � �������������
	//������������ �� ���� �� ������, �� �������� ����������
	switch(ui.cmbMethod->currentIndex())
	{
	case 0:
		decodeLSB(attackedImage);
		break;
	case 1:
		decodePRI(attackedImage);
		break;
	case 2:
		decodePastorfide(attackedImage);
		break;
	case 3:
		decodeHempstalk(attackedImage);
		break;
	case 4:
		decodeICPES(attackedImage);
		break;
	case 5:
		decodeIEEE(attackedImage);
	}
	//�������� ������
	BER(watermark, resWatermark);
	PSNR(sourceImg, attackedImage);
	SSIM(watermark, resWatermark);
	NCC(watermark, resWatermark);
	RMS(sourceImg, attackedImage);
	//����� �������� �����������
	diffImg = difference(sourceImg, attackedImage);
	diffWatermark = difference(watermark, resWatermark);
	cvShowImage("����������� � ������", attackedImage);
	cvShowImage("������� ����� �������� � ���������� ������������", diffImg);
	cvShowImage("������� ����� �������� � ���������� ������� ������", diffWatermark);

}

void labz::img2bin()
{
	int wH, wW; //������ � ������ ���������� ��������������
	wH = watermark->height;
	wW = watermark->width;
	CvScalar sWater;
	binWatermark.clear();

	for(int i = 0; i < wH; i++)
	{
		for(int j = 0; j < wW; j++)
		{
			sWater = cvGet2D(watermark, i, j);
			for(int k = 0; k < 8; k++)
				binWatermark.append((int)(sWater.val[0]/pow(2.0, k))%2);
		}
	}
}

void labz::decodeLSB(IplImage* resImg)
{
	//�������, ���� �� �����
	int i = 0, j = 0;
	w = resImg->width;
	h = resImg->height;
	CvScalar sWater;
	returnedBinWatermark.clear();
	for(int x = 0; x < binWatermark.size(); x++)
	{
		j=x%w;
		i=x/w;
		sWater = cvGet2D(resImg, i, j);
		returnedBinWatermark << (int(sWater.val[0]) % 2);
	}
	bin2img();
}

void labz::bin2img()
{
	//���������� �������� ��������� ������� � ��������

	resWatermark = cvCreateImage(cvSize(watermark->width, watermark->height), IPL_DEPTH_8U, 3);
	QVector<int> byte;
	QVector<int> decTemp;

	CvScalar s1;
	//������ 8 ��������
	for(int i = 0; i < watermark->width * watermark->height; i++)
	{
		//������ ����� �������� ����� ������������� ������ ���
		for(int j = 0; j < 8; j++)
		{
			byte << returnedBinWatermark[i * 8 + j];
		}
		//������������ �������� ����� � ����������
		decTemp << bin2dec(byte);
		byte.clear();
		//
	}
	for(int j = 0; j < watermark->height; j++)
	{
		//���������� ������ �� ������
		for(int i = 0; i < watermark->width; i++)
		{
			s1.val[0] = decTemp[j * watermark->width + i];
			s1.val[1] = decTemp[j * watermark->width + i];
			s1.val[2] = decTemp[j * watermark->width + i];
			cvSet2D(resWatermark, j, i, s1);
		}
	}
	cvShowImage("�������������� ������� ����", resWatermark);

}

void labz::BER(IplImage* img1, IplImage* img2)
{
	//������� bit error rate
	int w = img1->width;
	int h = img1->height;
	double res;
	CvScalar s1, s2;
	double N = w * h, Nerr = 0;

	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h; j++)
		{
			s1 = cvGet2D(img1, j, i);
			s2 = cvGet2D(img2, j, i);
			if(s1.val[0] != s2.val[0])
				Nerr++;
		}
	}
	res = Nerr/N;
	//������� ��������� � ������
	ui.dspnBER->setValue(res);
}

void labz::PSNR(IplImage* img1, IplImage* img2)
{
	//������� peak signal-to-noise ratio
	int w = img1->width;
	int h = img1->height;
	double res = 0, num = 0, denom = 0;
	CvScalar s1, s2;

	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h; j++)
		{
			s1 = cvGet2D(img1, j, i);
			s2 = cvGet2D(img2, j, i);

			denom += pow(abs(s1.val[0] - s2.val[0]), 2);
			denom += pow(abs(s1.val[1] - s2.val[1]), 2);
			denom += pow(abs(s1.val[2] - s2.val[2]), 2);
		}
	}

	num = w * h;
	num *= 255*255*3;

	res = 10 * log10(num/denom);
	//������� � ������
	ui.dspnPSNR->setValue(res);
}

void labz::RMS(IplImage* img1, IplImage* img2)
{
	//������� root mean square
	int w = img1->width;
	int h = img1->height;
	double res = 0;
	CvScalar s1, s2;
	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h; j++)
		{
			s1 = cvGet2D(img1, j, i);
			s2 = cvGet2D(img2, j, i);

			res += pow(abs(s1.val[0] - s2.val[0]), 2);
			res += pow(abs(s1.val[1] - s2.val[1]), 2);
			res += pow(abs(s1.val[2] - s2.val[2]), 2);
		}
	}
	res /= w * h * 3;
	res = sqrt(res);
	//������� � ������
	ui.dspnRMS->setValue(res);
}


int labz::bin2dec(QVector<int> bin)
{
	//��������� �������� �� ��������� � ����������
	//�������� ������ �� ������ ��������
	int res = 0;
	for(int i = 0; i < 8; i++)
	{
		res += pow(2.0, i) * bin[i];
	}
	return res;
}

void labz::changeBrightness()
{
	//��������� ����� ���������� �������
	attackedImage = cvCloneImage(resultImg);
	//�������������� � ������� ������
	Mat temp = Mat(attackedImage);
	//���� �����
	temp *= ui.dspnBrightness->value();
	//�������� ��������������
	attackedImage->imageData = (char*)temp.data;


}

IplImage* labz::difference(IplImage* img1, IplImage* img2)
{
	//������� �������� �����������
	int w = img1->width;
	int h = img1->height;
	CvScalar s1, s2, s3;
	IplImage* diff = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h; j++)
		{
			s3.val[0] = 0;
			s3.val[1] = 0;
			s3.val[2] = 0;
			//�������� ������ �������
			s1 = cvGet2D(img1, j, i);
			s2 = cvGet2D(img2, j, i);
			if(s1.val[0] != s2.val[0])
				s3.val[0] += 100;
			if(s1.val[1] != s2.val[1])
				s3.val[1] += 100;
			if(s1.val[2] != s2.val[2])
				s3.val[2] += 100;
			cvSet2D(diff, j, i, s3);
		}
	}
	return diff;
}

void labz::encodePRI()
{
	//��������� ����� ���������������� ���������
	CvScalar s, s1;
	//��� ������ ����������, �� ������� ��� ����� ��������� ������ ������� ����������
	double proportion = sourceImg->width * sourceImg->height / (watermark->width * watermark -> height * 8);
	//��������� ���������� ����� � �������� �� 1 �� proportion
	interval = rand() % (int)(proportion) + 1;
	//�������� ��������
	int i = 0, j = 0;
	int prevIndex = 0;
	resultImg = cvCloneImage(sourceImg);
	for(int x = 0; x < binWatermark.size(); x++)
	{
		//i - ������ � ������, j - ������ ����� ������

		s = cvGet2D(sourceImg, j, i);
		s1.val[0] = s.val[0] + binWatermark[x] - (int(s.val[0]) % 2);
		s1.val[1] = s.val[1];
		s1.val[2] = s.val[2];
		cvSet2D(resultImg, j, i, s1);
		i += interval;

		if(i >= sourceImg->width)
		{
			i -= sourceImg->width;
			j++;
		}
	}
	cvShowImage("�������� �����������", sourceImg);
	cvShowImage("���������", watermark);
	cvShowImage("�������������� �����������", resultImg);
}

void labz::decodePRI(IplImage* img1)
{
	//������������� ������� ���������������� ���������
	int i = 0, j = 0;
	w = img1->width;
	h = img1->height;
	CvScalar sWater;
	returnedBinWatermark.clear();
	for(int x = 0; x < binWatermark.size(); x++)
	{
		sWater = cvGet2D(img1, j, i);
		returnedBinWatermark << (int(sWater.val[0]) % 2);
		i += interval;

		if(i >= sourceImg->width)
		{
			i -= sourceImg->width;
			j++;
		}
	}
	bin2img();
}

void labz::crop()
{
	//��������, ���� �������� ������ � �������
	//����� ���������� ����� �����������
	int x0, y0; //���� ����������� ��������������, ������� ��������
	int Wdt, Hgt; //������ � ������ ����� ��������������, ���� ������
	CvScalar s;
	Wdt = rand() % sourceImg->width + 1;
	Hgt = rand() % sourceImg->height + 1;
	x0 = rand() % (sourceImg->width - Wdt);
	y0 = rand() % (sourceImg->height - Hgt);
	//���������
	attackedImage = cvCreateImage(cvGetSize(resultImg), resultImg->depth, resultImg->nChannels);
	attackedImage = cvCloneImage(resultImg);
	for(int i = x0; i < x0 + Wdt; i++)
	{
		for(int j = y0; j < y0 + Hgt; j++)
		{
			//��������� ������ �����, ������������� ������������ ��������������
			s.val[0] = 0;
			s.val[1] = 0;
			s.val[2] = 0;
			cvSet2D(attackedImage, j, i, s);
		}
	}

}


void labz::encodePastorfide(IplImage* img1)
{
	//
	IplImage* edgeImg = Laplace(img1);


	CvScalar s, s1, s2;
	resultImg = cvCloneImage(sourceImg);
	int x = 0;//�������� ������� ���������
	//������ �� ���� �������� ��������� �����������, ���������, �������� �� ��� ���������
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] == 255)
			{
				//���������� �� ������
				s1 = cvGet2D(img1, j, i);
				//���������� ������ � �������
				if(x < binWatermark.size())
				{
					s2.val[0] = s1.val[0] + binWatermark[x] - (int(s1.val[0]) % 2);
					s2.val[1] = s1.val[1];
					s2.val[2] = s1.val[2];
					cvSet2D(resultImg, j, i, s2);
					x++;
				}

			}

		}
	}
	//�������� ��� �������, ����� ���������� - ������� �� �� ��� ������ ����� ���������
	//x ������ � ���� ������ ���������� ����������� ���� ��������� � ����� ������

	cvShowImage("�������� �����������", sourceImg);
	cvShowImage("���������", watermark);
	cvShowImage("�������������� �����������", resultImg);

}

IplImage* labz::Laplace(IplImage* img1)
{
	IplImage* copy = cvCloneImage(img1);
	Mat src, src_gray, dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	int c;
	src = Mat(copy);
	/// Remove noise by blurring with a Gaussian filter
	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

	/// Convert the image to grayscale
	
	cvtColor( src, src_gray, CV_BGR2GRAY );
	
	/// Apply Laplace function
	Mat abs_dst;

	Laplacian( src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( dst, abs_dst );
	Mat out;
	int thresh = 100;
	switch(ui.cmbMethod->currentIndex())
	{
	case 3:
		thresh = 100;
		break;
	case 4:
		thresh = 0;
		break;
	}
	threshold(abs_dst, out, thresh, 255, THRESH_BINARY);
	imshow("Edge detection", abs_dst);
	IplImage* edgeImg = cvCreateImage(cvSize(img1->width, img1->height), IPL_DEPTH_8U, 3);;
	//edgeImg = cvCloneImage(img1);
	//edgeImg->imageData = (char*)abs_dst.data;
	edgeImg= cvCloneImage(&(IplImage)out);
	return edgeImg;
}

void labz::decodePastorfide(IplImage* img1)
{
	//���������� Pastorfide
	returnedBinWatermark.clear();
	//������� ����� ���������� ������� �����������
	
	IplImage* edgeImg = Laplace(sourceImg);
	//cvShowImage("!", edgeImg);
	//� ������ ����� ����������
	CvScalar s, s1;
	int x = 0;
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] > 0)
			{
				//���������� �� ������
				s1 = cvGet2D(img1, j, i);
				//�������� ��������� �� �����
				for(int k = 0; k < 1; k++)//k < 3
				{
					if(x < binWatermark.size())
					{

						returnedBinWatermark << (int(s1.val[k]) % 2);
						x++;
					}
				}
			}
		}
	}
	if(returnedBinWatermark.size() < binWatermark.size())
	{
		for(int i = returnedBinWatermark.size(); i < binWatermark.size(); i++)
			returnedBinWatermark << 0;
	}
	//������������ ���������� ���������
	bin2img();
}

void labz::encodeHempstalk()
{
	//���������� �� ����� Hempstalk. Hiding behind the corners
	//���� - ���� �������, ������ � ���, ����������� ��������
	//���� �������
	IplImage* edgeImg = Laplace(sourceImg);
	//cvShowImage("�������� ��� �����������", sourceImg);
	//cvShowImage("������� ��� �����������", edgeImg);
	//checkImg(edgeImg);
	resultImg = cvCloneImage(sourceImg);
	//������ � ������� ���� ������� ����. ��������. ��� ����� ����� �����
	//������� �������� ������ ���� ��������� �����, ����� ���� ������� ���������
	QVector<int> iVec;
	QVector<int> jVec;
	CvScalar s, s1, s2;
	int randNum;
	for(int i = 0; i < edgeImg->width; i++)
	{
		for(int j = 0; j < edgeImg->height; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] == 255)
			{
				iVec << i;
				jVec << j;
			}
		}
	}
	//������ � ��� ���� ��� �������, ������� ������ �������� ����� �������
	//����������� �� ������ ������� � ����� ��������, �������� ��� ���������� ����� �����
	//������ ����� ��������� ��������� �����
	//����, ������� ��������� ��� ������������� - ���� ���� ������������ �� �� ����� �����, �� ������ �� �� ��������
	//���������� ��������� ���
	srand(time(0));
	randNum = rand() % 256;//���� � ������ ��� ������
	//������ ������������ ���� ������� � �������� ����� ��� ����������� ������������. ������� ���� ��� ���� ����-�� 
	//��������, ���� ������ ��� �������������
	QVector<int> binRandNum = dec2bin(randNum);
	int x = 0;
	for(int i = 0; i < edgeImg->width && x < 8; i++)
	{
		for(int j = 0; j < edgeImg->height && x < 8; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			
			if(s.val[0] == 0)
			{
				s2 = cvGet2D(sourceImg, j, i);
				//��������!
				s1.val[0] = s2.val[0] + binRandNum[x] - (int(s2.val[0]) % 2);
				s1.val[1] = s2.val[1];
				s1.val[2] = s2.val[2];
				cvSet2D(resultImg, j, i, s1);
				x++;
			}
		}
	}
	//����� ����� ����� ������������ ������������������ ����� ��� ����������� ������ �������� �����
	//��� ����� ���������� �������� ���� ���������
	//������ ��� ��� ������������������
	srand(randNum);
	int index;
	bool flag;
	int i, j;
	//������ ������ �����, ������� ��� ������������: � ������������������ ����� ����������� �������
	QVector<int> usedIndexes;//���� ����������� ������� ����� � ��������, ������� ��� ���� ������������
	//������ ����� ������������ ������ �������� �� 0 �� 32767, ��� ����� ������ ��� ������
	int endVal;
	if(binWatermark.size() > 32768)
		endVal = 32768;
	else
		endVal = binWatermark.size();

	if(endVal > iVec.size())
		endVal = iVec.size();


	for(int l = 0; l < endVal; l++)
	{

		flag = false;
		do{
			index = rand() % iVec.size();
			if(!usedIndexes.contains(index))
			{
				usedIndexes << index;
				i = iVec[index];
				j = jVec[index];
				s = cvGet2D(sourceImg, j, i); 
				s1.val[0] = s.val[0] + binWatermark[l] - (int(s.val[0]) % 2);
				s1.val[1] = s.val[1];
				s1.val[2] = s.val[2];
				cvSet2D(resultImg, j, i, s1);
				flag = true;
			}
		}while(!flag);
	}

	cvShowImage("�������� �����������", sourceImg);
	cvShowImage("���������", watermark);
	cvShowImage("�������������� �����������", resultImg);
}

QVector<int> labz::dec2bin(int dec)
{
	//�������������� ����� � ��������
	QVector<int> res;
	for(int k = 0; k < 8; k++)
		res << (int)(dec/pow(2.0, k))%2;
	return res;
}

void labz::decodeHempstalk(IplImage* img1)
{
	//�������������, ��������������...
	//���������� ������� �� �������� �����������
	IplImage* edgeImg = Laplace(sourceImg);
	//cvShowImage("�������� ��� �������������", sourceImg);
	//cvShowImage("������� ��� �������������", edgeImg);
	//������� ����� ������������
	int x = 0;
	QVector<int> retBinRandNum;
	QVector<int> iVec;
	QVector<int> jVec;
	CvScalar s, s1;
	for(int i = 0; i < img1->width && x < 8; i++)
	{
		for(int j = 0; j < img1->height && x < 8; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] < 150)
			{
				//���������!
				s1 = cvGet2D(img1, j, i);
				retBinRandNum << (int(s1.val[0]) % 2);
				x++;
			}
		}
	}
	int retRandNum = bin2dec(retBinRandNum);
	//���������� ������ � �������� ���� ���
	srand(retRandNum);
	//� ����� ������ ���������� �� �� ������������������ ��������� �����, ��� � ��� �����������
	//����������� �������
	for(int i = 0; i < edgeImg->width; i++)
	{
		for(int j = 0; j < edgeImg->height; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] == 255)
			{
				iVec << i;
				jVec << j;
			}
		}
	}

	//��������� ��� � ��������, ��� �� ��������, ������� �����
	QVector<int> usedIndexes;//���� ����������� ������� ����� � ��������, ������� ��� ���� ������������
	bool flag;
	returnedBinWatermark.clear();
	int index;
	int endVal;
	if(binWatermark.size() > 32768)
		endVal = 32768;
	else
		endVal = binWatermark.size();
	if(endVal > iVec.size())
		endVal = iVec.size();
	int i, j;
	for(int l = 0; l < endVal; l++)
	{

		flag = false;
		do{
			index = rand() % iVec.size();
			if(!usedIndexes.contains(index))
			{
				usedIndexes << index;
				i = iVec[index];
				j = jVec[index];
				s = cvGet2D(img1, j, i);
				returnedBinWatermark << (int(s.val[0]) % 2);
				flag = true;
			}
		}while(!flag);
	}
	//���������, ���� ����������� ���������, � �� �����������
	if(returnedBinWatermark.size() < binWatermark.size())
	{
		for(int i = returnedBinWatermark.size(); i < binWatermark.size(); i++)
		{
			returnedBinWatermark << 0;
		}
	}
	//������������ ����������
	bin2img();
}

void labz::checkImg(IplImage* img1)
{
	//IplImage* img2 = Laplace(img1);
	CvScalar s;
	QVector<int> vec;
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(img1, i, j);
			if(!vec.contains(s.val[0]))
				vec << s.val[0];
		}
	}
}

IplImage* labz::encodeICPES()
{
	//�� ����� 023-ICPES2012
	//������ �������� ����������� �������������� �� �������
	CvScalar sR, sG, sB, s, s1, s2;
	//CvScalar Y, Cr, Cb;
	double Y, Cr, Cb;
	double R, G, B;
	IplImage* Red = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* Green = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* Blue = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* YCbCr = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* out = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* middle = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* Yimg = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* YimgEdges = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			s = cvGet2D(sourceImg, j, i);
			sR.val[0] = 0;
			sR.val[1] = 0;
			sR.val[2] = s.val[2];
			sG.val[0] = 0;
			sG.val[1] = s.val[1];
			sG.val[2] = 0;
			sB.val[0] = s.val[0];
			sB.val[1] = 0;
			sB.val[2] = 0;
			cvSet2D(Red, j, i, sR);
			cvSet2D(Green, j, i, sG);
			cvSet2D(Blue, j, i, sB);
		}
	}
	//������� �����
	cvShowImage("������� ����", Red);
	cvShowImage("������� ����", Green);
	cvShowImage("����� ����", Blue);

	//������������� �����

	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			sR = cvGet2D(Red, j, i);
			sG = cvGet2D(Green, j, i);
			sB = cvGet2D(Blue, j, i);
			Y = 0.299 * sR.val[2] + 0.587 * sG.val[1] + 0.114 * sB.val[0];
			Cb = 0.596 * sR.val[2] - 0.275 * sG.val[1] - 0.321 * sB.val[0];
			Cr = 0.212 * sR.val[2] - 0.523 * sG.val[1] - 0.311 * sB.val[0];
			//����� � ��������
			s.val[0] = Y;
			s.val[1] = Cb;
			s.val[2] = Cr;

			s1.val[0] = Y;
			s1.val[1] = 0;
			s1.val[2] = 0;

			cvSet2D(YCbCr, j, i, s);
			cvSet2D(Yimg, j, i, s1);
		}
	}
	//������� �� �����, ��
	//cvShowImage("�������� � YCbCr �������", YCbCr);
	//cvShowImage("���� Y", Yimg);
	//����� ������� �� ����������� ������ � Y
	YimgEdges = Laplace(Yimg);
	//�������� ���
	middle = cvCloneImage(YCbCr);
	int x = 0;
	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			s = cvGet2D(YimgEdges, j, i);
			if(s.val[0] > 0)
			{
				//���������� �� ������
				s1 = cvGet2D(YCbCr, j, i);
				if(x < binWatermark.size())
				{
					s2.val[0] = s1.val[0] + binWatermark[x] - (int(s1.val[0]) % 2);
					s2.val[1] = s1.val[1];
					s2.val[2] = s1.val[2];
					cvSet2D(middle, j, i, s2);
					x++;
				}

			}

		}
	}
	//������ ���� ������� ������������� ��� ���� � ���������� ��� �������

	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			s = cvGet2D(middle, j, i);
			R = s.val[0] + 0.956 * s.val[1] + 0.620 * s.val[2];
			G = s.val[0] - 0.272 * s.val[1] + 0.647 * s.val[2];
			B = s.val[0] - 1.108 * s.val[1] + 1.705 * s.val[2];
			//����� � ��������
			s1.val[0] = B;
			s1.val[1] = G;
			s1.val[2] = R;
			cvSet2D(out, j, i, s1);
		}
	}
	cvShowImage("�������� �����������", sourceImg);
	cvShowImage("���������", watermark);
	cvShowImage("�������������� �����������", out);
	return out;
}
void labz::decodeICPES(IplImage* img1)
{
	//���������� ��������������
	//������� - ������������� �������
	double R, G, B, Y, Cb, Cr, R1, G1, B1, Y1, Cb1, Cr1;
	CvScalar s, s1, s2, s0;
	IplImage* YCbCr = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* Yimg = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* Yedges = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	returnedBinWatermark.clear();
	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			s = cvGet2D(sourceImg, j, i);
			s0 = cvGet2D(img1, j, i);
			R = s.val[2];
			G = s.val[1];
			B = s.val[0];

			R1 = s0.val[2];
			G1 = s0.val[1];
			B1 = s0.val[0];

			Y = 0.299 * R + 0.587 * G + 0.114 * B;

			Y1 = 0.299 * R1 + 0.587 * G1 + 0.114 * B1;
			Cb1 = 0.596 * R1 - 0.275 * G1 - 0.321 * B1;
			Cr1 = 0.212 * R1 - 0.523 * G1 - 0.311 * B1;

			s1.val[0] = Y1;
			s1.val[1] = Cb1;
			s1.val[2] = Cr1;
			cvSet2D(YCbCr, j, i, s1);
			s1.val[0] = Y;
			s1.val[1] = 0;
			s1.val[2] = 0;
			cvSet2D(Yimg, j, i, s1);
		}
	}
	//���������� ����� 
	Yedges = Laplace(Yimg);
	//������ ���
	int x = 0;
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(Yedges, j, i);
			if(s.val[0] > 0)
			{
				//���������� �� ������
				s1 = cvGet2D(YCbCr, j, i);
				//�������� ��������� �� �����

				if(x < binWatermark.size())
				{

					returnedBinWatermark << (int(s1.val[0]) % 2);
					x++;
				}
			}
		}
	}
	//��������� ���������, ���� ����������
	if(returnedBinWatermark.size() < binWatermark.size())
	{
		for(int i = returnedBinWatermark.size(); i < binWatermark.size(); i++)
		{
			returnedBinWatermark << 0;
		}
	}
	//�� � ������������ ���������� ���������, ���������?
	bin2img();
}
IplImage* labz::encodeIEEE()
{
	//�� ����� Adaptive Data Hiding in Edge Areas of Images with Spatial LSB Domain Systems
	//����������� ����������� � �����-�����
	IplImage* copy = cvCloneImage(sourceImg);
	Mat src, src_gray;
	src = Mat(copy);
	cvtColor( src, src_gray, CV_BGR2GRAY );
	IplImage* workImg = cvCloneImage(&(IplImage)src_gray);
	IplImage* out = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* L = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* H = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	IplImage* M = cvCreateImage(cvSize(sourceImg->width, sourceImg->height), IPL_DEPTH_8U, 3);
	out = cvCloneImage(sourceImg);
	//������ ������� �� ������ ����
	CvScalar s, s1, s2;
	//����� �������, ��� ��� ������� �� ��������� ����� ������ 50 ����������� ������� ������
	//�� 50 �� 150 - ��������
	//150-255 - ��������
	//��������������, � ������ ����� ����� ���������� �� ������ ���� ����������, � ������� - �� ���, � ������� - �� ���
	//��� ����� �������, �������, ��� ���������� ����� ��������, ��� ��� ��� ����� ���������� ��� ��� �����
	//����� ������������ ������-�� ������
	//��, ���� ���������� ������ ������, ����� �������� ���
	int x = 0;
	for(int i = 0; i < workImg->width && x < binWatermark.size(); i++)
	{
		for(int j = 0; j < workImg->height && x < binWatermark.size(); j++)
		{
			s = cvGet2D(workImg, j, i);
			s2 = cvGet2D(sourceImg, j, i);
			if(s.val[0] <= 50)
			{
				//������ �����
				s1.val[0] = s2.val[0] + binWatermark[x] - (int(s2.val[0]) % 2);
				s1.val[1] = s2.val[1];
				s1.val[2] = s2.val[2];
				x++;
			}
			else if(s.val[0] <= 150 && s.val[0] > 50)
			{
				//������� �����
				for(int k = 0; k < 2 && x < binWatermark.size(); k++)
				{
					s1.val[k] = s2.val[k] + binWatermark[x] - (int(s2.val[k]) % 2);
					x++;
				}
				s1.val[2] = s2.val[2];
			}
			else if(s.val[0] > 150)
			{
				//������� �����
				for(int k = 0; k < 3 && x < binWatermark.size(); k++)
				{
					s1.val[k] = s2.val[k] + binWatermark[x] - (int(s2.val[k]) % 2);
					x++;
				}
			}
			cvSet2D(out, j, i, s1);
		}
	}
	//������� ���������� �����������
	cvShowImage("�������� �����������", sourceImg);
	cvShowImage("���������", watermark);
	cvShowImage("�������������� �����������", out);
	return out;
}

void labz::decodeIEEE(IplImage* img1)
{
	//������������� ��������������
	IplImage* Ccopy = cvCloneImage(sourceImg);
	Mat src, src_gray;
	src = Mat(Ccopy);
	cvtColor( src, src_gray, CV_BGR2GRAY );
	IplImage* workImg = cvCloneImage(&(IplImage)src_gray);

	IplImage* copy = cvCloneImage(img1);
	CvScalar s, s1;
	returnedBinWatermark.clear();

	int x = 0;
	for(int i = 0; i < copy->width && x < binWatermark.size(); i++)
	{
		for(int j = 0; j < copy->height && x < binWatermark.size(); j++)
		{
			s = cvGet2D(copy, j, i);
			s1 = cvGet2D(workImg, j, i);
			if(s1.val[0] <= 50)
			{
				//������ �����
				returnedBinWatermark << (int(s.val[0]) % 2);
				x++;
			}
			else if(s1.val[0] <= 150 && s1.val[0] > 50)
			{
				//������� �����
				for(int k = 0; k < 2 && x < binWatermark.size(); k++)
				{
					returnedBinWatermark << (int(s.val[k]) % 2);
					x++;
				}
			}
			else if(s1.val[0] > 150)
			{
				//������� �����
				for(int k = 0; k < 3 && x < binWatermark.size(); k++)
				{
					returnedBinWatermark << (int(s.val[k]) % 2);
					x++;
				}
			}
		}
	}
	bin2img();
}
void labz::blurAttack()
{
	IplImage* copy = cvCloneImage(resultImg);
	Mat src;
	src = Mat(copy);
	/// Remove noise by blurring with a Gaussian filter
	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
	attackedImage = cvCloneImage(&(IplImage)src);
}

void labz::medBlurAttack()
{
	IplImage* copy = cvCloneImage(resultImg);
	Mat src;
	src = Mat(copy);
	medianBlur(src, src, 3);
	attackedImage = cvCloneImage(&(IplImage)src);
}

void labz::noise()
{
	// imGray is the grayscale of the input image
	Mat imGray;
	IplImage* copy = cvCloneImage(resultImg);
	Mat input = Mat(copy);
	Mat result;
	cvtColor(input,imGray,CV_BGR2GRAY);
	Mat noise = Mat(imGray.size(),CV_64F);
	normalize(imGray, result, 0.0, 1.0, CV_MINMAX, CV_64F);
	randn(noise, 0, 0.05);
	result = result + noise;
	normalize(result, result, 0.0, 1.0, CV_MINMAX, CV_64F);
	attackedImage = cvCloneImage(&(IplImage)result);
}

void labz::equIntensity()
{
	Mat ycrcb;
	IplImage* copy = cvCloneImage(resultImg);
	Mat inputImage = Mat(copy);

	cvtColor(inputImage,ycrcb,CV_BGR2YCrCb);

	vector<Mat> channels;
	split(ycrcb,channels);

	equalizeHist(channels[0], channels[0]);

	Mat result;
	merge(channels,ycrcb);

	cvtColor(ycrcb,result,CV_YCrCb2BGR);
	attackedImage = cvCloneImage(&(IplImage)result);
}
void labz::NCC(IplImage* img1, IplImage* img2) 
{ 
	int imgW = img1->width; 
	int imgH = img1->height; 
	double result; 
	double	num = 0;
	double	denum = 0; 
	double	denumPart1 = 0; 
	double	denumPart2 = 0; 
	CvScalar s1, s2; 

	for(int i = 0; i < imgW; i++) 
	{ 
		for(int j = 0; j < imgH; j++) 
		{ 
			s1 = cvGet2D(img1, i, j); 
			s2 = cvGet2D(img2, i, j); 

			num += s1.val[0] * s2.val[0]; 
			denumPart1 += pow(abs(s1.val[0]), 2.0); 
			denumPart2 += pow(abs(s2.val[0]), 2.0); 
		} 
	} 

	denum = sqrt(denumPart1 * denumPart2); 
	result = num / denum; 
	//������� � ������
	ui.dspnNCC->setValue(result);
} 

void labz::SSIM(IplImage* img1, IplImage* img2) 
{ 
	int imgW = img1->width; 
	int imgH = img1->height; 
	int size = imgW * imgH; 

	double result;
	//������� �������������
	double mF = 0;
	double mG = 0; 
	//������� �������������
	double sigF = 0; 
	double sigG = 0; 
	double sigFG= 0; 
	//���������
	double C1 = pow(0.01 * 255, 2);   
	double C2 = pow(0.03 * 255, 2); 
	double C3 = C2 / 2; 
	double	l, c, s; 
	CvScalar s1, s2; 

	for(int i = 0; i < imgW; i++) 
	{ 
		for(int j = 0; j < imgH; j++) 
		{ 
			s1 = cvGet2D(img1, i, j); 
			s2 = cvGet2D(img2, i, j); 

			mF += s1.val[0]; 
			mG += s2.val[0]; 
		} 
	} 
	mF /= size; 
	mG /= size; 

	for(int i = 0; i < imgW; i++) 
	{ 
		for(int j = 0; j < imgH; j++) 
		{ 
			s1 = cvGet2D(img1, i, i); 
			s2 = cvGet2D(img2, i, i); 

			sigF += pow(s1.val[0] - mF, 2); 
			sigG += pow(s2.val[0] - mG, 2); 
			sigFG += (s1.val[0] - mF) * (s2.val[0] - mG); 
		} 
	} 
	sigF = sqrt(sigF /(size - 1)); 
	sigG = sqrt(sigG /(size - 1)); 
	sigFG /= size - 1; 
	result = ((2 * mF * mG + C1) * (2 * sigFG + C2)) / ((mF * mF + mG * mG +C1) * (sigF * sigF + sigG * sigG +C2)); 
	ui.dspnSSIM->setValue(result); 
}