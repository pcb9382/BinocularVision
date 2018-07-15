
// BinocularVisionDlg.h : 头文件
//

#pragma once
#include <GL/glut.h>
#include <afx.h>
#include <Windows.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "cvaux.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include "BinocularVisionDlg1.h"

//Basler相机需要的头文件
#include <pylon/PylonIncludes.h>
#include "conio.h"//为了使用_getchar()函数实现不输入回车直接获取在控制台键入的字符
#include <windows.h>
#include <process.h>

#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <pcl/visualization/cloud_viewer.h>                       //类CloudViewer头文件声明
#include <pcl/io/io.h>                                            //I/O相关头文件
#include <pcl/io/pcd_io.h>                                        //PCD文件读取头文件声明
#include <pcl/console/parse.h>
#include <GL/glut.h>
#include "afxwin.h"

using namespace std;
using namespace cv;
using namespace Pylon;

// CBinocularVisionDlg 对话框
class CBinocularVisionDlg : public CDialogEx
{
// 构造
public:
	CBinocularVisionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BINOCULARVISION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();

	//打开摄像机线程
	CWinThread*    OpenCamera_Thread;                                       //声明线程
	static UINT OpenCameraThread(void *param);                              //声明线程打开摄像机线程函数

	//三维重建线程（包括立体校正，三维重建，视差图显示以及点云优化及显示）
	CWinThread*    _3DConstruction_Thread;                                  //声明线程
	static UINT   _3DConstructionThread(void *param);                       //声明三维重建线程函数

	//点云显示线程，使用OpenGL显示点云
	CWinThread*    PointCloudView_Thread;                                  //声明线程
	static UINT   PointCloudViewThread(void *param);                       //声明点云可视化线程函数

	//标定打开摄像机线程，抓取图片用于标定
	CWinThread*    Calibration_OpenCamera_Thread;                          //声明线程
	static UINT    CalibrationOpenCameraThread(void *param);               //声明线程打开摄像机线程函数

public:


	//一些函数和一些空间
	void ShowImage(IplImage*img, UINT ID);                                  //显示图像的函数
	CListBox m_list;
	void ShowMessage(const CString);                                        //列表框显示消息函数
	int StereoMatch_SGBM(Mat &img1, Mat &img2, int SADWindowSize, int numberOfDisparities, Mat & disp, Mat & disp8);//立体匹配函数
	void DispToXYZ(const Mat& mat);
	void saveXYZ(const char* filename, const Mat& mat);
	void Disprity2Color(CvMat* gray_mat, CvMat* color_mat);                  //由视差图转换成伪彩色图
	int Normal_ImageWidth;                                                   //图像分辨率 宽
	int Normal_ImageHigh;                                                    //图像分辨率 高
	int DisparityValue;                                                      //立体匹配时的视差寻找范围
	int SADSizeValue;                                                        //立体匹配时SAD窗口的大小
	int m_StereoMatcging_SGBM;                                               //选择不同的匹配算法
	int m_ShowStylpe;                                                        //显示方式
	bool b_PseudoColorStyple;                                                //伪彩色图显示方式
	bool b_DepthMapStyple;                                                   //深度图显示方式
	bool b_DesprityMapStyple;                                                //视差图显示方式
	int m_CalibrationResolution;                                             //标定分辨率
	int m_InnerAnglePoint;                                                   //内角点数
	int m_CheckboardSize;                                                    //棋盘格大小
	int StereoCalibration_Normal_ImageWidth;                                 //标定时设定的图像分辨率 宽
	int StereoCalibration_Normal_ImageHigh;                                  //标定时设定的图像分辨率 高
	int StereoCalibration_CheckboardWidthNum;                                //棋盘格宽度方向上的角点数
	int StereoCalibration_CheckboardHighNum;                                 //棋盘格高度方向上的角点数
	int CheckboardSize;                                                      //棋盘格大小.



	//按钮生成的函数声明
	afx_msg void OpenCamera();
	afx_msg void CloseCamera();
	afx_msg void _3DConstruction();
	afx_msg void Stop3DConstruction();
	afx_msg void BeginShow3DPointCloud();
	afx_msg void OnClickedButtonExit();
	afx_msg void StopShow3DPointCloud();
	afx_msg void Setting();
	afx_msg void StereoCalibration_GrapPicture();
	afx_msg void AccomplishCalibration_GrapPicture();

	//一些变量
	Mat disp, disp8;                                                          //视差图 
    CvMat *Q = cvCreateMat(4, 4, CV_64F);                                     //重投影矩阵
	
	//用于得到子窗口传递过来的值
	int ResolutiuonValue;
	int DisparityWindowValue;
	int SADWindowSizeValue;

	
	
	
	
	
	afx_msg void BeginStereoCalibretion();
};






