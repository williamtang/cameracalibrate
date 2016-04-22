/****************************************************
 *                                                  *
 *           image undistortion demo                *
 *                                                  *
 * usage: 											*
 *        exe [config.txt] input_image              *
 * date :                                           *
 *        2016/04/22                                *
 ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson\filereadstream.h"

using namespace std;
using namespace cv;
using namespace rapidjson;

class Config;

// parse config data from file
void parse_cfg(const string &cfg_path, Mat &cameraMatrix, Mat &distCoeffs);
double str2double(const string str);

int main(int argc, char* argv[])
{
	string input_path;
	string cfg_path;

	if (argc < 2 || argc > 3) {
		printf("usage: exe [config] input\n");
		return -1;
	} else if (argc == 2) {
		cfg_path   = "config.json";
		input_path = string(argv[1]);
	} else if (argc == 3) {
		cfg_path   = string(argv[1]);
		input_path = string(argv[2]);
	}

	// initilize camera matrix and distort coefficient
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distCoeffs   = Mat::zeros(4, 1, CV_64F);

	parse_cfg(cfg_path, cameraMatrix, distCoeffs);

	// undistort
	Mat src = imread(input_path);
	Mat dst;
	undistort(src, dst, cameraMatrix, distCoeffs );
	imshow( "src", src );
	imshow( "dst", dst);
	waitKey(0);

	return 0;
}

void parse_cfg(const string &cfg_path, Mat &cameraMatrix, Mat &distCoeffs)
{
	FILE *fp = fopen( cfg_path.c_str(), "rb" );
	char readBuf[65536];
	FileReadStream is( fp, readBuf, sizeof( readBuf ) );
	rapidjson::Document doc;
	doc.ParseStream( is );
	vector<string> camera_matrix[3];
	vector<string> dist_coeffs;

	// parse camera matrix params
	if (doc.HasMember("cameraMatrix")) {
		const Value &cmjson = doc["cameraMatrix"];
		for (SizeType i = 0; i < cmjson.Size(); ++i) {
			if (cmjson[i].HasMember("1")) {
				camera_matrix[0].push_back(cmjson[i]["1"].GetString());
				cout << "a1" << i << ": " << cmjson[i]["1"].GetString() << endl;
			}
			if (cmjson[i].HasMember("2")) {
				camera_matrix[1].push_back(cmjson[i]["2"].GetString());
				cout << "a2" << i <<": " << cmjson[i]["2"].GetString() << endl;
			}
			if (cmjson[i].HasMember("3")) {
				camera_matrix[2].push_back(cmjson[i]["3"].GetString());
				cout << "a3" << i <<": " << cmjson[i]["3"].GetString() << endl;
			}
		}
	}

	// parse distort coefficients params
	if (doc.HasMember("distCoeffs")) {
		const Value &dcjson = doc["distCoeffs"];
		for (int i = 0; i < dcjson.Size(); ++i) {
			if (dcjson[i].HasMember("1")) {
				dist_coeffs.push_back(dcjson[i]["1"].GetString());
			}
			if (dcjson[i].HasMember("2")) {
				dist_coeffs.push_back(dcjson[i]["2"].GetString());
			} 
			if (dcjson[i].HasMember("3")) {
				dist_coeffs.push_back(dcjson[i]["3"].GetString());
			}
			if (dcjson[i].HasMember("4")) {
				dist_coeffs.push_back(dcjson[i]["4"].GetString());
			}
		}
	}

	fclose( fp );

	// convert vector to mat
	for (int i = 0; i < 3; i++) {
		// row
		for (int j = 0; j < camera_matrix[i].size(); j++) {
			cameraMatrix.at<double>(i,j) = str2double(camera_matrix[i][j]);
		}
	}

	for (int i = 0; i < dist_coeffs.size(); i++) {
		distCoeffs.at<double>(i,0) = str2double(dist_coeffs[i]);
	}
}

double str2double(const string str)
{
	return atof(str.c_str());
}