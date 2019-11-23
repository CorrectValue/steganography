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
		//i - индекс в строке, j - индекс самой строки
		j=x%w;
		i=x/w;
		s = cvGet2D(sourceImg, i, j);
		s1.val[0] = s.val[0] + binWatermark[x] - (int(s.val[0]) % 2);
		s1.val[1] = s.val[1];
		s1.val[2] = s.val[2];
		cvSet2D(resultImg, i, j, s1);

	}


	cvShowImage("Исходное изображение", sourceImg);
	cvShowImage("Ватермарк", watermark);
	cvShowImage("Результирующее изображение", resultImg);



}

void labz::go()
{
	//загрузить изображения, аднака
	sourceImg = cvLoadImage("img.jpg");
	watermark = cvLoadImage("flame.png");
	h = sourceImg->height;
	w = sourceImg->width;

	//определить, чё там выбрано вообще для начала
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

	//подсчет метрик
	BER(watermark, resWatermark);
	PSNR(sourceImg, resultImg);
	SSIM(watermark, resWatermark);
	NCC(watermark, resWatermark);
	RMS(sourceImg, resultImg);
	//вывод разности изображений
	diffImg = difference(sourceImg, resultImg);
	diffWatermark = difference(watermark, resWatermark);
	cvShowImage("Разница между исходным и полученным изображением", diffImg);
	cvShowImage("Разница между исходным и полученным водяным знаком", diffWatermark);


}

void labz::attack()
{
	//узнать, что выбрано
	switch(ui.cmbAttack->currentIndex())
	{
	case 0:
		//изменение яркости
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
	//пересчёт метрик и декодирование
	//декодировать по тому же методу, по которому кодировали
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
	//пересчет метрик
	BER(watermark, resWatermark);
	PSNR(sourceImg, attackedImage);
	SSIM(watermark, resWatermark);
	NCC(watermark, resWatermark);
	RMS(sourceImg, attackedImage);
	//вывод разности изображений
	diffImg = difference(sourceImg, attackedImage);
	diffWatermark = difference(watermark, resWatermark);
	cvShowImage("Изображение с атакой", attackedImage);
	cvShowImage("Разница между исходным и полученным изображением", diffImg);
	cvShowImage("Разница между исходным и полученным водяным знаком", diffWatermark);

}

void labz::img2bin()
{
	int wH, wW; //высота и ширина ватермарки соответственно
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
	//декодер, пока не нужен
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
	//превращает бинарное сообщение обратно в картинку

	resWatermark = cvCreateImage(cvSize(watermark->width, watermark->height), IPL_DEPTH_8U, 3);
	QVector<int> byte;
	QVector<int> decTemp;

	CvScalar s1;
	//отсечь 8 значений
	for(int i = 0; i < watermark->width * watermark->height; i++)
	{
		//каждой точке водяного знака соответствует восемь бит
		for(int j = 0; j < 8; j++)
		{
			byte << returnedBinWatermark[i * 8 + j];
		}
		//перекодируем бинарное число в десятичное
		decTemp << bin2dec(byte);
		byte.clear();
		//
	}
	for(int j = 0; j < watermark->height; j++)
	{
		//отхоботить строку от общего
		for(int i = 0; i < watermark->width; i++)
		{
			s1.val[0] = decTemp[j * watermark->width + i];
			s1.val[1] = decTemp[j * watermark->width + i];
			s1.val[2] = decTemp[j * watermark->width + i];
			cvSet2D(resWatermark, j, i, s1);
		}
	}
	cvShowImage("Декодированный водяной знак", resWatermark);

}

void labz::BER(IplImage* img1, IplImage* img2)
{
	//метрика bit error rate
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
	//вывести результат в виджет
	ui.dspnBER->setValue(res);
}

void labz::PSNR(IplImage* img1, IplImage* img2)
{
	//метрика peak signal-to-noise ratio
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
	//вывести в виджет
	ui.dspnPSNR->setValue(res);
}

void labz::RMS(IplImage* img1, IplImage* img2)
{
	//метрика root mean square
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
	//вывести в виджет
	ui.dspnRMS->setValue(res);
}


int labz::bin2dec(QVector<int> bin)
{
	//переводит значение из бинарного в десятичное
	//получает вектор из восьми значений
	int res = 0;
	for(int i = 0; i < 8; i++)
	{
		res += pow(2.0, i) * bin[i];
	}
	return res;
}

void labz::changeBrightness()
{
	//реализует атаку изменением яркости
	attackedImage = cvCloneImage(resultImg);
	//преобразование в удобный формат
	Mat temp = Mat(attackedImage);
	//сама атака
	temp *= ui.dspnBrightness->value();
	//обратное преобразование
	attackedImage->imageData = (char*)temp.data;


}

IplImage* labz::difference(IplImage* img1, IplImage* img2)
{
	//выводит разность изображений
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
			//сравнить каждый пиксель
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
	//реализует метод псевдослучайного интервала
	CvScalar s, s1;
	//для начала определить, во сколько раз длина сообщения меньше размера контейнера
	double proportion = sourceImg->width * sourceImg->height / (watermark->width * watermark -> height * 8);
	//генерация случайного числа в границах от 1 до proportion
	interval = rand() % (int)(proportion) + 1;
	//алгоритм сокрытия
	int i = 0, j = 0;
	int prevIndex = 0;
	resultImg = cvCloneImage(sourceImg);
	for(int x = 0; x < binWatermark.size(); x++)
	{
		//i - индекс в строке, j - индекс самой строки

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
	cvShowImage("Исходное изображение", sourceImg);
	cvShowImage("Ватермарк", watermark);
	cvShowImage("Результирующее изображение", resultImg);
}

void labz::decodePRI(IplImage* img1)
{
	//декодирование методом псевдослучайного интервала
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
	//допустим, ваша картинка попала в Израиль
	//атака вырезанием куска изображения
	int x0, y0; //угол вырезаемого прямоугольника, задаётся рандомно
	int Wdt, Hgt; //ширина и высота этого прямоугольника, тоже рандом
	CvScalar s;
	Wdt = rand() % sourceImg->width + 1;
	Hgt = rand() % sourceImg->height + 1;
	x0 = rand() % (sourceImg->width - Wdt);
	y0 = rand() % (sourceImg->height - Hgt);
	//вырезание
	attackedImage = cvCreateImage(cvGetSize(resultImg), resultImg->depth, resultImg->nChannels);
	attackedImage = cvCloneImage(resultImg);
	for(int i = x0; i < x0 + Wdt; i++)
	{
		for(int j = y0; j < y0 + Hgt; j++)
		{
			//вырезание каждой точки, принадлежащей объявленному прямоугольнику
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
	int x = 0;//итератор вектора сообщения
	//пройти по всем пикселям исходного изображения, проверить, выделены ли они контурами
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] == 255)
			{
				//наткнулись на контур
				s1 = cvGet2D(img1, j, i);
				//встраиваем хоботу в пиксель
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
	//пройдены все контуры, самое интересное - хватило ли их для записи всего сообщения
	//x хранит в себе индекс последнего записанного бита сообщения В ЛЮБОМ СЛУЧАЕ

	cvShowImage("Исходное изображение", sourceImg);
	cvShowImage("Ватермарк", watermark);
	cvShowImage("Результирующее изображение", resultImg);

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
	//декодирует Pastorfide
	returnedBinWatermark.clear();
	//сначала нужно определить контуры изображения
	
	IplImage* edgeImg = Laplace(sourceImg);
	//cvShowImage("!", edgeImg);
	//а теперь самое интересное
	CvScalar s, s1;
	int x = 0;
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(edgeImg, j, i);
			if(s.val[0] > 0)
			{
				//наткнулись на контур
				s1 = cvGet2D(img1, j, i);
				//вынимаем сообщение из точки
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
	//расшифровать полученное сообщение
	bin2img();
}

void labz::encodeHempstalk()
{
	//реализован по файлу Hempstalk. Hiding behind the corners
	//идея - ищем контуры, прячем в них, распределяя рандомно
	//ищем контуры
	IplImage* edgeImg = Laplace(sourceImg);
	//cvShowImage("Исходное при кодировании", sourceImg);
	//cvShowImage("Контуры при кодировании", edgeImg);
	//checkImg(edgeImg);
	resultImg = cvCloneImage(sourceImg);
	//прячем в контуры свой водяной знак. рандомно. для этого нужно зерно
	//сначала составим вектор всех доступных точек, чтобы было удобнее рандомить
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
	//теперь у нас есть два вектора, которые вместе содержат точки контура
	//обратившись по одному индексу к обоим векторам, получаем обе координаты одной точки
	//теперь нужен генератор случайных чисел
	//идея, которая сработает при декодировании - если дать рандомайзеру то же самое зерно, он выдаст то же значение
	//генерируем рандомный сид
	srand(time(0));
	randNum = rand() % 256;//чтоб в восемь бит влезло
	//теперь используется этот ранднум в качестве зерна для последующей рандомизации. сначала надо его тоже куда-то 
	//записать, чтоб вынуть при декодировании
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
				//записать!
				s1.val[0] = s2.val[0] + binRandNum[x] - (int(s2.val[0]) % 2);
				s1.val[1] = s2.val[1];
				s1.val[2] = s2.val[2];
				cvSet2D(resultImg, j, i, s1);
				x++;
			}
		}
	}
	//после этого можно генерировать последовательность чисел для кодирования самого водяного знака
	//для этого пользуемся набитыми выше векторами
	//задаем сид для последовательности
	srand(randNum);
	int index;
	bool flag;
	int i, j;
	//создаём вектор точек, которые уже использованы: в последовательности могут встретиться повторы
	QVector<int> usedIndexes;//сюда добавляются индексы точек в векторах, которые уже были использованы
	//рандом может генерировать только значения от 0 до 32767, что адово портит всю малину
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

	cvShowImage("Исходное изображение", sourceImg);
	cvShowImage("Ватермарк", watermark);
	cvShowImage("Результирующее изображение", resultImg);
}

QVector<int> labz::dec2bin(int dec)
{
	//преобразование числа в бинарное
	QVector<int> res;
	for(int k = 0; k < 8; k++)
		res << (int)(dec/pow(2.0, k))%2;
	return res;
}

void labz::decodeHempstalk(IplImage* img1)
{
	//декодирование, соответственно...
	//определить контуры на поданном изображении
	IplImage* edgeImg = Laplace(sourceImg);
	//cvShowImage("Исходное при декодировании", sourceImg);
	//cvShowImage("Контуры при декодировании", edgeImg);
	//достать зерно рандомайзера
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
				//прочитать!
				s1 = cvGet2D(img1, j, i);
				retBinRandNum << (int(s1.val[0]) % 2);
				x++;
			}
		}
	}
	int retRandNum = bin2dec(retBinRandNum);
	//ретранднум кидаем в качестве сида гсч
	srand(retRandNum);
	//в итоге должна получиться та же последовательность случайных чисел, что и при кодировании
	//подготовить векторы
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

	//запустить гсч и смотреть, как он работает, попивая пивко
	QVector<int> usedIndexes;//сюда добавляются индексы точек в векторах, которые уже были использованы
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
	//дополнить, чтоб размерности совпадали, а то некультурно
	if(returnedBinWatermark.size() < binWatermark.size())
	{
		for(int i = returnedBinWatermark.size(); i < binWatermark.size(); i++)
		{
			returnedBinWatermark << 0;
		}
	}
	//расшыфровать полученное
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
	//по книге 023-ICPES2012
	//сперва исходное изображение раскладывается на палитры
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
	//вывести чтоль
	cvShowImage("Красный слой", Red);
	cvShowImage("Зеленый слой", Green);
	cvShowImage("Синий слой", Blue);

	//преобразовать цвета

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
			//пишем в картинку
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
	//вывести на экран, чо
	//cvShowImage("Картинка в YCbCr палитре", YCbCr);
	//cvShowImage("Цвет Y", Yimg);
	//найти контуры на изображении только с Y
	YimgEdges = Laplace(Yimg);
	//встроить цвз
	middle = cvCloneImage(YCbCr);
	int x = 0;
	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			s = cvGet2D(YimgEdges, j, i);
			if(s.val[0] > 0)
			{
				//наткнулись на контур
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
	//теперь надо обратно преобразовать это дело в нормальную ргб палитру

	for(int i = 0; i < sourceImg->width; i++)
	{
		for(int j = 0; j < sourceImg->height; j++)
		{
			s = cvGet2D(middle, j, i);
			R = s.val[0] + 0.956 * s.val[1] + 0.620 * s.val[2];
			G = s.val[0] - 0.272 * s.val[1] + 0.647 * s.val[2];
			B = s.val[0] - 1.108 * s.val[1] + 1.705 * s.val[2];
			//пишем в картинку
			s1.val[0] = B;
			s1.val[1] = G;
			s1.val[2] = R;
			cvSet2D(out, j, i, s1);
		}
	}
	cvShowImage("Исходное изображение", sourceImg);
	cvShowImage("Ватермарк", watermark);
	cvShowImage("Результирующее изображение", out);
	return out;
}
void labz::decodeICPES(IplImage* img1)
{
	//декодирует соответственно
	//сначала - преобразовать палитру
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
	//определить грани 
	Yedges = Laplace(Yimg);
	//вынуть цвз
	int x = 0;
	for(int i = 0; i < img1->width; i++)
	{
		for(int j = 0; j < img1->height; j++)
		{
			s = cvGet2D(Yedges, j, i);
			if(s.val[0] > 0)
			{
				//наткнулись на контур
				s1 = cvGet2D(YCbCr, j, i);
				//вынимаем сообщение из точки

				if(x < binWatermark.size())
				{

					returnedBinWatermark << (int(s1.val[0]) % 2);
					x++;
				}
			}
		}
	}
	//дополнить сообщение, если необходимо
	if(returnedBinWatermark.size() < binWatermark.size())
	{
		for(int i = returnedBinWatermark.size(); i < binWatermark.size(); i++)
		{
			returnedBinWatermark << 0;
		}
	}
	//ну и расшифровать полученное сообщение, правильно?
	bin2img();
}
IplImage* labz::encodeIEEE()
{
	//по файлу Adaptive Data Hiding in Edge Areas of Images with Spatial LSB Domain Systems
	//конвертнуть изображение в черно-белое
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
	//теперь разбить на домены чоль
	CvScalar s, s1, s2;
	//будем считать, что все пиксели со значением цвета меньше 50 принадлежат нижнему домену
	//от 50 до 150 - среднему
	//150-255 - верхнему
	//соответственно, в нижний домен будем записывать по одному биту информации, в средний - по два, в старший - по три
	//при таком подходе, логично, вся информация будет записана, так как все точки контейнера так или иначе
	//будут принадлежать какому-то домену
	//ну, если контейнера вообще хватит, чтобы вместить цвз
	int x = 0;
	for(int i = 0; i < workImg->width && x < binWatermark.size(); i++)
	{
		for(int j = 0; j < workImg->height && x < binWatermark.size(); j++)
		{
			s = cvGet2D(workImg, j, i);
			s2 = cvGet2D(sourceImg, j, i);
			if(s.val[0] <= 50)
			{
				//нижний домен
				s1.val[0] = s2.val[0] + binWatermark[x] - (int(s2.val[0]) % 2);
				s1.val[1] = s2.val[1];
				s1.val[2] = s2.val[2];
				x++;
			}
			else if(s.val[0] <= 150 && s.val[0] > 50)
			{
				//средний домен
				for(int k = 0; k < 2 && x < binWatermark.size(); k++)
				{
					s1.val[k] = s2.val[k] + binWatermark[x] - (int(s2.val[k]) % 2);
					x++;
				}
				s1.val[2] = s2.val[2];
			}
			else if(s.val[0] > 150)
			{
				//верхний домен
				for(int k = 0; k < 3 && x < binWatermark.size(); k++)
				{
					s1.val[k] = s2.val[k] + binWatermark[x] - (int(s2.val[k]) % 2);
					x++;
				}
			}
			cvSet2D(out, j, i, s1);
		}
	}
	//вернуть полученное изображение
	cvShowImage("Исходное изображение", sourceImg);
	cvShowImage("Ватермарк", watermark);
	cvShowImage("Результирующее изображение", out);
	return out;
}

void labz::decodeIEEE(IplImage* img1)
{
	//декодирование соответственно
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
				//нижний домен
				returnedBinWatermark << (int(s.val[0]) % 2);
				x++;
			}
			else if(s1.val[0] <= 150 && s1.val[0] > 50)
			{
				//средний домен
				for(int k = 0; k < 2 && x < binWatermark.size(); k++)
				{
					returnedBinWatermark << (int(s.val[k]) % 2);
					x++;
				}
			}
			else if(s1.val[0] > 150)
			{
				//верхний домен
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
	//вывести в виджет
	ui.dspnNCC->setValue(result);
} 

void labz::SSIM(IplImage* img1, IplImage* img2) 
{ 
	int imgW = img1->width; 
	int imgH = img1->height; 
	int size = imgW * imgH; 

	double result;
	//средние интенсивности
	double mF = 0;
	double mG = 0; 
	//средние контрастности
	double sigF = 0; 
	double sigG = 0; 
	double sigFG= 0; 
	//константы
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