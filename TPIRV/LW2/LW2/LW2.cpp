#include "pch.h"
#include <omp.h>
#include <conio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <chrono>
#include <iostream>

#define _THREADS _Thrd_hardware_concurrency()

using namespace cv;
//using namespace cuda;
using namespace std;
using namespace std::chrono;

const unsigned int msize = 4096; //4096;

unsigned long long sum = 0;

Mat IntensityCalculate(Mat img) {
    Mat result = Mat::zeros(img.rows, img.cols, CV_8UC1);
    
    for (int x = 0; x < img.cols; x++) {
        for (int y = 0; y < img.rows; y++) {
            Vec3b& color = img.at<Vec3b>(y, x);
            result.at<uchar>(y, x) = (color[0] + color[1] + color[2]) / 3.;
        }
    }

    return result;
}

Mat IntensityParCalculate(Mat img) {
    Mat result = Mat::zeros(img.rows, img.cols, CV_8UC1);

#pragma omp parallel num_threads(_THREADS)
    {
#pragma omp for 
        for (int x = 0; x < img.cols; x++) {
            for (int y = 0; y < img.rows; y++) {
                Vec3b& color = img.at<Vec3b>(y, x);
                result.at<uchar>(y, x) = (color[0] + color[1] + color[2]) / 3.;
            }
        }
    }

    return result;
}

int xGradient(Mat image, int x, int y)
{
    return image.at<uchar>(y - 1, x - 1) +
        2 * image.at<uchar>(y, x - 1) +
        image.at<uchar>(y + 1, x - 1) -
        image.at<uchar>(y - 1, x + 1) -
        2 * image.at<uchar>(y, x + 1) -
        image.at<uchar>(y + 1, x + 1);
}

int yGradient(Mat image, int x, int y)
{
    return image.at<uchar>(y - 1, x - 1) +
        2 * image.at<uchar>(y - 1, x) +
        image.at<uchar>(y - 1, x + 1) -
        image.at<uchar>(y + 1, x - 1) -
        2 * image.at<uchar>(y + 1, x) -
        image.at<uchar>(y + 1, x + 1);
}

Mat MatrixMR(Mat img) {
    Mat res = Mat::zeros(img.rows, img.cols, CV_8UC1);

    int gx, gy, sum;

    int max = 0;
    
    for (int x = 1; x < img.cols - 1; x++) {
        for (int y = 1; y < img.rows - 1; y++) {
            gx = xGradient(img, x, y);
            gy = yGradient(img, x, y);
            sum = sqrt(gx * gx + gy * gy);

            if (max < sum) {
                max = sum;
            }
            res.at<uchar>(y, x) = sum;
        }
    }

    for (int x = 1; x < img.cols - 1; x++) {
        for (int y = 1; y < img.rows - 1; y++) {
            if (res.at<uchar>(y, x) != 0) {
                int pix = res.at<uchar>(y, x);
                res.at<uchar>(y, x) = pix * 255 / max;
            }
        }
    }

    return res;
}

Mat ParMatrixMR(Mat img) {
    Mat res = Mat::zeros(img.rows, img.cols, CV_8UC1);

    

    int max = 0;
#pragma omp parallel num_threads(_THREADS)
    {

#pragma omp for 
        for (int x = 1; x < img.cols - 1; x++) {
            for (int y = 1; y < img.rows - 1; y++) {
                int gx, gy, sum;
                gx = xGradient(img, x, y);
                gy = yGradient(img, x, y);
                sum = sqrt(gx * gx + gy * gy);

#pragma omp critical
                {
                    if (max < sum) {
                        max = sum;
                    }
                }
                res.at<uchar>(y, x) = sum;
            }
        }

#pragma omp for 
        for (int x = 1; x < img.cols - 1; x++) {
            for (int y = 1; y < img.rows - 1; y++) {
                if (res.at<uchar>(y, x) != 0) {
                    int pix = res.at<uchar>(y, x);
                    res.at<uchar>(y, x) = pix * 255 / max;
                }
            }
        }
    }

    return res;
}

void WriteImgToCSV(std::string name, Mat img) {
    ofstream out;
    out.open("C:/distr/_TPU/TPIRV/LW2/" + name);
        for (int x = 0; x < img.cols; x++) {
            for (int y = 0; y < img.rows; y++)
                out << (int)img.at<uchar>(y, x) << ";";
            out << endl;
        }    
    out.close();
}

void ParWriteImgToCSV(std::string name, Mat img) {
    ofstream out;
    out.open("C:/distr/_TPU/TPIRV/LW2/" + name);

#pragma omp parallel num_threads(_THREADS)
    {
        for (int x = 0; x < img.cols; x++) {
#pragma omp for 
            for (int y = 0; y < img.rows; y++)
                out << (int)img.at<uchar>(y, x) << ";";
            out << endl;
        }
    }
    out.close();
}

void ISEETHEPICTURE(std::string windowName, Mat img) {
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, img);
}

void ImgTesting(int i) {
    Mat img[4];
    double start, end;

    img[0] = imread("C:/distr/_TPU/TPIRV/LW2/kitten0.jpg");
    img[1] = imread("C:/distr/_TPU/TPIRV/LW2/kitten1.jpg");
    img[2] = imread("C:/distr/_TPU/TPIRV/LW2/kitten2.jpg");
    

    //ISEETHEPICTURE("First OpenCV Application", MatrixMR(IntensityCalculate(img[0])));
    //ISEETHEPICTURE("Second OpenCV Application", MatrixMR(IntensityCalculate(img[1])));
    //ISEETHEPICTURE("Third OpenCV Application", MatrixMR(IntensityCalculate(img[2])));
    //moveWindow("First OpenCV Application", 0, 45);
    //waitKey(0);
    //destroyAllWindows();

    cout << endl << "------------------------ " << i + 1 << " ------------------------" << endl << endl;

    start = omp_get_wtime();
    end = omp_get_wtime();
    img[3] = MatrixMR(IntensityCalculate(img[0]));
    cout << "kitten0: " << end - start << " seconds" << endl;
    WriteImgToCSV("kitten0.csv", img[3]);

    start = omp_get_wtime();
    img[3] = MatrixMR(IntensityCalculate(img[1]));
    end = omp_get_wtime();
    cout << "kitten1: " << end - start << " seconds" << endl;
    WriteImgToCSV("kitten1.csv", img[3]);

    start = omp_get_wtime();
    img[3] = MatrixMR(IntensityCalculate(img[2]));
    end = omp_get_wtime();
    cout << "kitten2: " << end - start << " seconds" << endl << endl;
    WriteImgToCSV("kitten2.csv", img[3]);


    start = omp_get_wtime();
    img[3] = ParMatrixMR(IntensityParCalculate(img[0]));
    end = omp_get_wtime();
    cout << "parallel kitten0: " << end - start << " seconds" << endl;
    WriteImgToCSV("kitten0.csv", img[3]);

    start = omp_get_wtime();
    img[3] = ParMatrixMR(IntensityParCalculate(img[1]));
    end = omp_get_wtime();
    cout << "parallel kitten1: " << end - start << " seconds" << endl;
    WriteImgToCSV("kitten0.csv", img[3]);

    start = omp_get_wtime();
    img[3] = ParMatrixMR(IntensityParCalculate(img[2]));
    end = omp_get_wtime();
    cout << "parallel kitten2: " << end - start << " seconds" << endl;
    WriteImgToCSV("kitten0.csv", img[3]);
}

void scalar_mul(int32_t** A, int32_t** B, int32_t** C, unsigned int N, int start, int step) {
    for (int i = start; i < N; i += step) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void scalar_mul_with_threads(int32_t** A, int32_t** B, int32_t** C, unsigned int N, int start, int step) {
#pragma omp parallel num_threads(_THREADS)
    {
#pragma omp for
        for (int i = start; i < N; i += step) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
}

string matrix_comparing(int32_t** A, int32_t** B, unsigned int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (A[i][j] != B[i][j]) {
                return "false";
            }
        }
    }
    return "true";
}


void MatrixTesting(int i) {
    
    cout << endl << "------------------------ " << i + 1 << " ------------------------" << endl << endl;

    alignas(16) int32_t** A = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        A[i] = new int32_t[msize];
    }
    alignas(16) int32_t** B = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        B[i] = new int32_t[msize];
    }
    alignas(16) int32_t** C_scalar = new int32_t * [msize];
    for (int32_t i = 0; i < msize; i++) {
        C_scalar[i] = new int32_t[msize];
    }

    double start, end;

    start = omp_get_wtime();
    scalar_mul(A, B, C_scalar, msize, 0, 1);
    end = omp_get_wtime();
    cout << "matrix calculation: " << end - start << " seconds" << endl;

    start = omp_get_wtime();
    scalar_mul_with_threads(A, B, C_scalar, msize, 0, 1);
    end = omp_get_wtime();
    cout << "parallel matrix calculation: " << end - start << " seconds" << endl;


    for (int i = 0; i < msize; i++) {
        delete[] A[i];
    }
    delete[] A;
    for (int i = 0; i < msize; i++) {
        delete[] B[i];
    }
    delete[] B;
    for (int i = 0; i < msize; i++) {
        delete[] C_scalar[i];
    }
    delete[] C_scalar;
}

int main()
{
    for (int i = 0; i < 10; i++)
        ImgTesting(i);
    for (int i = 0; i < 10; i++)
        //MatrixTesting(i);

    return 0;
}
