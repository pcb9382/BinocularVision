
// BinocularVisionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BinocularVision.h"
#include "BinocularVisionDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CvvImage.h"
#include <cstring>
#include <iostream>
#include "BinocularVisionDlg1.h"


#include <pylon/PylonIncludes.h>
#include "conio.h"//为了使用_getchar()函数实现不输入回车直接获取在控制台键入的字符
#include <windows.h>
#include <process.h>
#include <pcl/visualization/cloud_viewer.h>                       //类CloudViewer头文件声明
#include <iostream>
#include <pcl/io/io.h>                                            //I/O相关头文件
#include <pcl/io/pcd_io.h>                                        //PCD文件读取头文件声明
#include <pcl/console/parse.h>
#include <string.h>
#include <pcl/point_types.h>
#include <GL/glut.h>


#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


//宏定义
#define SIGN(x) ( (x)<0 ? -1:((x)>0?1:0 ) )  
#define DLG_FILTER "ALL Files(.) | . || "
#define DLG_EXT "*" 


using namespace std;
using namespace cv;
using namespace Pylon;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBinocularVisionDlg 对话框



CBinocularVisionDlg::CBinocularVisionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBinocularVisionDlg::IDD, pParent)
	, m_StereoMatcging_SGBM(0)
	, m_ShowStylpe(0)
	, m_CalibrationResolution(0)
	, m_InnerAnglePoint(0)
	, m_CheckboardSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBinocularVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Radio(pDX, IDC_RADIO1, m_StereoMatcging_SGBM);
	DDX_Radio(pDX, IDC_RADIO6, m_ShowStylpe);
	DDX_Radio(pDX, IDC_RADIO4, m_CalibrationResolution);
	DDX_Radio(pDX, IDC_RADIO10, m_InnerAnglePoint);
	DDX_Radio(pDX, IDC_RADIO13, m_CheckboardSize);
}

BEGIN_MESSAGE_MAP(CBinocularVisionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CBinocularVisionDlg::OpenCamera)
	ON_BN_CLICKED(IDC_BUTTON2, &CBinocularVisionDlg::CloseCamera)
	ON_BN_CLICKED(IDC_BUTTON3, &CBinocularVisionDlg::_3DConstruction)
	ON_BN_CLICKED(IDC_BUTTON4, &CBinocularVisionDlg::Stop3DConstruction)
	ON_BN_CLICKED(IDC_BUTTON5, &CBinocularVisionDlg::BeginShow3DPointCloud)
	ON_BN_CLICKED(IDC_BUTTON7, &CBinocularVisionDlg::OnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON6, &CBinocularVisionDlg::StopShow3DPointCloud)
	ON_BN_CLICKED(IDC_BUTTON8, &CBinocularVisionDlg::Setting)
	ON_BN_CLICKED(IDC_BUTTON9, &CBinocularVisionDlg::StereoCalibration_GrapPicture)
	ON_BN_CLICKED(IDC_BUTTON10, &CBinocularVisionDlg::AccomplishCalibration_GrapPicture)
	ON_BN_CLICKED(IDC_BUTTON11, &CBinocularVisionDlg::BeginStereoCalibretion)
END_MESSAGE_MAP()


// CBinocularVisionDlg 消息处理程序

BOOL CBinocularVisionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	
	OpenCamera_Thread = NULL;



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBinocularVisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBinocularVisionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBinocularVisionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*****************************************************************************************/
/*                            用于线程间的全局变量                                       */
/*****************************************************************************************/

volatile bool b_OpenCamera = true;                     //摄像机开关标志位
volatile bool b_StereoCalibrationOpenCameraed =false;  //用于判断立体标定时的摄像机的开关
int i_GrapStereoCalibrationPicture = 0;                //用于判断按下立体标定时按钮是开启摄像机还是抓拍照片
volatile bool b_StereoCalibrationGrapPicture = false;  //用于下发抓拍图像的命令

IplImage * LeftCameraImage;                            //左图像（用于摄像机线程向三维重建线程通信的全局变量）
IplImage * RightCameraImage;                           //右图像（用于摄像机线程向三维重建线程通信的全局变量）
volatile bool b_ImageUsing = false;                    //用于确保图像正确写入与通信的标志位
volatile bool b_3DConstruction = false;                //用于开始和结束三维重建线程
volatile bool Capture = false;                         //用于开始和结束三维重建线程
volatile bool PointCloudShowRead ;                     //用于判断视差图是否在读的标志位
volatile bool PointCloudShowWrite ;                    //用于判断视差图是否在写的标志位
volatile bool OpenCameraed=false;                      //用于判断相机是否开启的标志位
string strFileName_left = "";                          //用于需要保存图像时的左图像的名称
string strFileName_right = "";                         //用于需要保存图像时的右图像的名称

Vec3f point1;
vector<Vec3f>PointCloud;                               //存储XYZ点信息 
int glWinWidth = 640, glWinHeight = 480;
double eyex, eyey, eyez, atx, aty, atz;                // eye* - 摄像机位置，at* - 注视点位置  
bool leftClickHold = false, rightClickHold = false;
int mx, my;                                            // 鼠标按键时在 OpenGL 窗口的坐标  
int ry = 90, rx = 90;                                  // 摄像机相对注视点的观察角度  
double radius = 80.0;                                  // 摄像机与注视点的距离 
void SetupRC();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void RenderScene();
void processNormalKeys(unsigned char key, int x, int y);
void timer(int p);

int k = 1;
Mat _3dImage;
Mat _3dImage1;                                         //用于视差图转换成点云
Vec3f point;
/*****************************************************************************************/
/*                    用于在Picture空间上显示图像的函数                                  */
/*                    输入为图像IplImage指针，以及输出在相应Picture控件的ID              */
/*****************************************************************************************/
void CBinocularVisionDlg::ShowImage(IplImage*img, UINT ID)
{
	CDC* pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	SetRect(rect, rect.left, rect.top, rect.right, rect.bottom);
	CvvImage cimg;
	cimg.CopyOf(img);
	cimg.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);

}


/*****************************************************************************************/
/*                          用于在list控件上显示列表信息                                 */
/*                    输入为 CString str，在list空间上输出CString str的内容              */
/*****************************************************************************************/

void CBinocularVisionDlg::ShowMessage(const CString str)
{
	CTime TheTime = CTime::GetCurrentTime();
	CString str1, str2;
	str1.Format("%04d-%02d-%02d %02d:%02d:%02d |", TheTime.GetYear(), TheTime.GetMonth(), TheTime.GetDay(),
		TheTime.GetHour(), TheTime.GetMinute(), TheTime.GetSecond());
	str2 = str1 + str;
	int iSelIndex = m_list.AddString(str2);
	if (iSelIndex != LB_ERR && iSelIndex != LB_ERRSPACE)
	{
		m_list.SetCurSel(iSelIndex);
	}
}

/*****************************************************************************************/
/*                              用于视差图转换成伪彩色图                                 */
/*****************************************************************************************/

void CBinocularVisionDlg::Disprity2Color(CvMat* gray_mat, CvMat* color_mat)
{
	if (color_mat)
		cvZero(color_mat);

	int stype = CV_MAT_TYPE(gray_mat->type), dtype = CV_MAT_TYPE(color_mat->type);
	int rows = gray_mat->rows, cols = gray_mat->cols;

	// 判断输入的灰度图和输出的伪彩色图是否大小相同、格式是否符合要求
	if (CV_ARE_SIZES_EQ(gray_mat, color_mat) && stype == CV_8UC1 && dtype == CV_8UC3)
	{
		CvMat* red = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);
		CvMat* green = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);
		CvMat* blue = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);
		CvMat* mask = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);

		// 计算各彩色通道的像素值
		cvSubRS(gray_mat, cvScalar(255), blue);	                    // blue(I) = 255 - gray(I)
		cvCopy(gray_mat, red);			                            // red(I) = gray(I)
		cvCopy(gray_mat, green);			                        // green(I) = gray(I),if gray(I) < 128
		cvCmpS(green, 128, mask, CV_CMP_GE);	                    // green(I) = 255 - gray(I), if gray(I) >= 128
		cvSubRS(green, cvScalar(255), green, mask);
		cvConvertScale(green, green, 2.0, 0.0);

		// 合成伪彩色图
		cvMerge(blue, green, red, NULL, color_mat);

		cvReleaseMat(&red);
		cvReleaseMat(&green);
		cvReleaseMat(&blue);
		cvReleaseMat(&mask);
	}

}

/*****************************************************************************************/
/*                                         打开摄像机                                    */
/*                                     将采集到的图像在控件窗口中显示                    */
/*****************************************************************************************/
void CBinocularVisionDlg::OpenCamera()
{
	// TODO:  在此添加控件通知处理程序代码
	//开启摄像机线程
	ShowMessage("开启摄像机");
	b_OpenCamera = false;
	OpenCamera_Thread = AfxBeginThread(OpenCameraThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	//OpenCamera_Thread->ResumeThread();
	
}
	
	


/*****************************************************************************************/
/*                                         关闭摄像机                                    */
/*****************************************************************************************/
void CBinocularVisionDlg::CloseCamera()
{
	// TODO:  在此添加控件通知处理程序代码
	ShowMessage("关闭摄像机");
	b_OpenCamera = true;
	//::WaitForSingleObject(OpenCamera_Thread->m_hThread, INFINITE);
	//OpenCamera_Thread = NULL;
}

/*****************************************************************************************/
/*                                        摄像机线程                                     */
/*****************************************************************************************/
UINT CBinocularVisionDlg::OpenCameraThread(void *param)
{
	CBinocularVisionDlg* coperTerView = (CBinocularVisionDlg*)param;
	// TODO:  在此添加控件通知处理程序代码
	int c_iCameraNum = 2;//摄像机的数量
	CInstantCameraArray cameras(2);                                         //创建两个两个摄像机对象
	int exitCode = 0;                                                       //判断异常代码
	int iLeftCameraIndex = 0, iRightCameraIndex = 0;
	
	CPylonImage imgLeft, imgRight;                                          //Pylon格式的图像
	CPylonImage pylonImageL, pylonImageR;                                   //创建Basler相机的图片格式
	Mat OpencvImage_left, OpencvImage_right;                                //用于pylonImage转换成Mat格式的图像
	CGrabResultPtr ptrGrabResultL, ptrGrabResultR;                          //智能指针
	CGrabResultPtr ptrGrabResultL1, ptrGrabResultR1;                        //智能指针
	CImageFormatConverter formatConverter;                                  //图像的格式转换
	CDeviceInfo aDeviceInfos[2];
	string strDeviceSerialNumbers[2];
	DeviceInfoList_t devices;                                               // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
	IplImage *ImageLeft;
	IplImage *ImageRight;
	
	//判断设置的图像分辨率
	//coperTerView->UpdateData(TRUE);
	switch (coperTerView->ResolutiuonValue)
	{
	case 0:
		coperTerView->Normal_ImageWidth = 640;
		coperTerView->Normal_ImageHigh = 480;
		break;
	case 1:
		coperTerView->Normal_ImageWidth = 480;
		coperTerView->Normal_ImageHigh = 320;
		break;
	case 2:
		coperTerView->Normal_ImageWidth = 960;
		coperTerView->Normal_ImageHigh = 720;
		break;
	case 3:
		coperTerView->Normal_ImageWidth = 1280;
		coperTerView->Normal_ImageHigh = 960;
		break;
	default:
		coperTerView->MessageBox("无效分辨率！");
		break;
	}

	IplImage *Normal_ImageLeft = cvCreateImage(cvSize(coperTerView->Normal_ImageWidth, coperTerView->Normal_ImageHigh), IPL_DEPTH_8U, 1);
	IplImage *Normal_ImageRight = cvCreateImage(cvSize(coperTerView->Normal_ImageWidth, coperTerView->Normal_ImageHigh), IPL_DEPTH_8U, 1);

	PylonInitialize();

	try
	{
		// Get the transport layer factory.
		CTlFactory& tlFactory = CTlFactory::GetInstance();

		//// Get all attached devices and exit application if no device is found.
		//DeviceInfoList_t devices;
		//判断摄像机是否连接
		if (tlFactory.EnumerateDevices(devices) == 0)
		{
			throw RUNTIME_EXCEPTION("No camera present.");
		}

		//// Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
		//CInstantCameraArray cameras(min(devices.size(), c_maxCamerasToUse));

		// Create and attach all Pylon Devices.
		for (size_t i = 0; i < cameras.GetSize(); ++i)
		{
			cameras[i].Attach(tlFactory.CreateDevice(devices[i]));

			// Print the model name of the camera.
			std::cout << "Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
		}

		//确定摄像机的序列号
		for (int i = 0; i < c_iCameraNum; i++)
		{
			aDeviceInfos[i] = cameras[i].GetDeviceInfo();
			strDeviceSerialNumbers[i] = aDeviceInfos[i].GetSerialNumber();

			//规定序列号为“21770467”的相机为左侧相机
			if (strDeviceSerialNumbers[i] == "21900819")
			{
				iLeftCameraIndex = i;
			}
			else if (strDeviceSerialNumbers[i] == "21770468")
			{

				iRightCameraIndex = i; 
			}
		}

		

			cameras.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByUser);  //开启抓取图像

			
			
			while (cameras.IsGrabbing() && !b_OpenCamera)
			{
				cameras[iLeftCameraIndex].RetrieveResult(5000, ptrGrabResultL, TimeoutHandling_ThrowException);
				cameras[iRightCameraIndex].RetrieveResult(5000, ptrGrabResultR, TimeoutHandling_ThrowException);

				if (ptrGrabResultL->GrabSucceeded() && ptrGrabResultR->GrabSucceeded())
				{
					imgLeft.AttachGrabResultBuffer(ptrGrabResultL);
					imgRight.AttachGrabResultBuffer(ptrGrabResultR);
				}

				


				//BaslerSDK指向图像的指针转换成CImageFormatConverter格式的图像

				formatConverter.Convert(pylonImageL, ptrGrabResultL);
				formatConverter.Convert(pylonImageR, ptrGrabResultR);

				// 把pylonImage图像格式转换成OpenCV里Mat格式的图像

				OpencvImage_left = Mat(ptrGrabResultL->GetHeight(), ptrGrabResultL->GetWidth(), CV_8UC1, (uint8_t *)pylonImageL.GetBuffer());
				OpencvImage_right = Mat(ptrGrabResultR->GetHeight(), ptrGrabResultR->GetWidth(), CV_8UC1, (uint8_t *)pylonImageR.GetBuffer());


				ImageLeft = &(IplImage)OpencvImage_left;
				ImageRight = &(IplImage)OpencvImage_right;
					
				//将图像缩小到（640*480）
				cvResize(ImageLeft, Normal_ImageLeft, CV_INTER_LINEAR);
				cvResize(ImageRight, Normal_ImageRight, CV_INTER_LINEAR);

				//cvSaveImage("left1.jpg", Normal_ImageLeft);
				//cvSaveImage("right1.jpg", Normal_ImageLeft);

					//调用函数将图像显示在Picture控件中
				coperTerView->ShowImage(ImageLeft, IDC_STATIC_PIC_Left);
				coperTerView->ShowImage(ImageRight, IDC_STATIC_PIC_Right);

					//如果三维重建线程完毕，标志位b_ImageUsing为false，则把最新抓取的图像传递给三维重建线程,继续进行三维重建以及点云显示
				if (!b_ImageUsing)
				{
					LeftCameraImage = Normal_ImageLeft;                     //左图像
					RightCameraImage = Normal_ImageRight;                   //右图像
					b_ImageUsing = true;
				}

				
				//cvReleaseImage(&ImageLeft);
				//cvReleaseImage(&ImageRight);

				
		}
	}
	catch (const GenericException &e)
	{

		// 异常处理
		cerr << "An exception occurred." << endl << e.GetDescription() << endl;
		exitCode = 1;

	}
	return 1;

}




/*****************************************************************************************/
/*                                         开始三维重建                                  */
/*                       将采集的图片进行立体校正，立体匹配和点云显示                    */
/*****************************************************************************************/
void CBinocularVisionDlg::_3DConstruction()
{
	// TODO:  在此添加控件通知处理程序代码
	//判断重建之前是否开启摄像机是否开启，不开启则退出
	if (b_OpenCamera)
	{
		MessageBox("请先打开摄像机！");
		ShowMessage("请先打开摄像机！");
		return;
	}
	UpdateData(TRUE);
	switch (m_StereoMatcging_SGBM)
	{
	case 0:
		ShowMessage("使用SGBM立体匹配算法进行三维重建！");
		break;
		/*
		case 1:
		OnClickedButtoncallvideo();
		break;
		case 2:
		OnClickedButtoncallpicture();
		break;
		*/
	default:
		MessageBox("现在只可用SGBM立体匹配算法！");
		return;
	}
	switch (m_ShowStylpe)
	{
	case 0:
		b_PseudoColorStyple = true;
		break;
		case 1:
		b_DepthMapStyple=true;
		MessageBox("目前无法显示深度图！");
		return;
		case 2:
		b_DesprityMapStyple=true;
		break;
	default:
		MessageBox("请选择正确的显示方式！");
		return;
	}


	ShowMessage("重建开始！");
	b_3DConstruction = true;
	b_ImageUsing = false;
	_3DConstruction_Thread = AfxBeginThread(_3DConstructionThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	//_3DConstruction_Thread->ResumeThread();
	

}

/*****************************************************************************************/
/*                                        停止三维重建                                   */
/*                               停止立体校正，立体匹配和点云显示                        */
/*****************************************************************************************/
void CBinocularVisionDlg::Stop3DConstruction()
{
	// TODO:  在此添加控件通知处理程序代码
	ShowMessage("停止重建！");
	b_3DConstruction = false;

	//将显示的方式重置
	b_PseudoColorStyple = false;
	b_DepthMapStyple = false;
	b_DesprityMapStyple = false;
	//下面这两句话用不到，加上界面就会死掉
	//::WaitForSingleObject(_3DConstruction_Thread->m_hThread, INFINITE);
	//_3DConstruction_Thread = NULL;
}



//三维重建线程
UINT CBinocularVisionDlg::_3DConstructionThread(void *param)
{
	
	//加载一些标定相关的参数
	CString cstrFilePath = "..\\Config\\";
	CvMat *Intrinsics_Camera_Left = (CvMat*)cvLoad("M1.xml");                         //加载左摄像机的内参数矩阵
	CvMat *Intrinsics_Camera_Right = (CvMat*)cvLoad("M2.xml");                        //加载右摄像机的内参数矩阵
	CvMat *Distortion_Camera_Left = (CvMat*)cvLoad("D1.xml");                         //加载左相机的畸变参数
	CvMat *Distortion_Camera_Right = (CvMat*)cvLoad("D2.xml");                        //加载右相机的畸变参数
	CvMat *Translation_matlab = (CvMat*)cvLoad("T.xml");                              //加载左右相机的平移矩阵
	CvMat *RotRodrigues_matlab = (CvMat*)cvLoad("R.xml");                             //加载左右相机的旋转矩阵
	CvMat *Q = cvCreateMat(4, 4, CV_64F);                                             //重投影矩阵
	CvSize imageSize = cvGetSize(RightCameraImage);                                   //得到采集图像的大小
	                                                                                  //用于视差图转换成点云
	string strPointCloudFullPath ="PC.txt";                                           //用于保存点云的txt文件
	CvMat* img1r = cvCreateMat(imageSize.height,imageSize.width, CV_8UC1);            //校正之后的左图像
	CvMat* img2r = cvCreateMat(imageSize.height,imageSize.width, CV_8UC1);            //校正之后的右图像
	CvMat* Disparity2RGB = cvCreateMat(imageSize.height, imageSize.width, CV_8UC3);   //视差图转伪彩色图
	CvMat*Cvdisp8 = cvCreateMat(imageSize.height, imageSize.width, CV_8UC1);                                                                  //由Mat类型的Disp8转换成CvMat
	CvMat Temp;                                                                       //由Mat类型的Disp8转换成CvMat的中间变量  

	CvMat* mx1 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* my1 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* mx2 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* my2 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
	CvMat _R1 = cvMat(3, 3, CV_64F, R1);
	CvMat _R2 = cvMat(3, 3, CV_64F, R2);
	CvMat _P1 = cvMat(3, 4, CV_64F, P1);
	CvMat _P2 = cvMat(3, 4, CV_64F, P2);
	//判断参数是否加载成功
	if (Intrinsics_Camera_Left == NULL || Intrinsics_Camera_Right == NULL || Distortion_Camera_Left == NULL ||
		Distortion_Camera_Right == NULL || Translation_matlab == NULL || RotRodrigues_matlab == NULL)
	{
		return -1;
	}
	CBinocularVisionDlg* coperTerView = (CBinocularVisionDlg*)param;

	//判断设置中的视差范围值
	//coperTerView->UpdateData(TRUE);
	switch (coperTerView->DisparityWindowValue)
	{
	case 0:
		coperTerView->DisparityValue = 128;
		break;
	case 1:
		coperTerView->DisparityValue = 256;
		break;
	case 2:
		coperTerView->DisparityValue = 512;
		break;
	default:
		coperTerView->MessageBox("无效视差值！");
		break;
	}

	switch (coperTerView->SADWindowSizeValue)
	{
	case 0:
		coperTerView->SADSizeValue = 15;
		break;
	case 1:
		coperTerView->SADSizeValue = 10;
		break;
	case 2:
		coperTerView->SADSizeValue = 20;
		break;
	case 3:
		coperTerView->SADSizeValue = 25;
		break;
	default:
		coperTerView->MessageBox("无效SAD值！");
		break;
	}

	//判断是需要保存视差图以及点云文件,如果需要则创建新的文件夹用于保存
	string Disprity_PointCloud_Namefolder1;                                                           //用于存放保存文件夹的名字
	long Disprity_cntImagesNum = 0;                                                                   //用于计算视差图保存的数量
	long PointCloud_cntImagesNum = 0;                                                                 //用于计算点云数据保存的数量
	long Picture_CntImagesNum = 0;                                                                    //计算拍摄照片的数量
	string strFileName_Disprity = "";                                                                 //视差图的名称
	string strFileName_PointCloud = "";                                                               //点云数据的名称
	if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1 || 
		coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePointCloud) == 1||
		coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1)
	{

		CTime TheTime = CTime::GetCurrentTime();                                                      //获取系统时间
		CString Disprity_PointCloud_Timefolder;                                                       //图片以时间命名
		CString Disprity_PointCloud_Namefolder;                                                       //实时抓拍图片存储的文件夹名称以及路径
		Disprity_PointCloud_Timefolder.Format("%04d%02d%02d_%02d%02d%02d", TheTime.GetYear(),
			TheTime.GetMonth(), TheTime.GetDay(), TheTime.GetHour(),
			TheTime.GetMinute(), TheTime.GetSecond());                                                //得到系统时间并输出
		Disprity_PointCloud_Namefolder = "Disprity_PointCloud_" + Disprity_PointCloud_Timefolder;
	    Disprity_PointCloud_Namefolder1 = CT2A(Disprity_PointCloud_Namefolder.GetBuffer());           //由CString转换成string
		// 判断路径是否存在
		if (!PathIsDirectory(Disprity_PointCloud_Namefolder))
		{
			CString strMsg;
			if (!CreateDirectory(Disprity_PointCloud_Namefolder, NULL))
			{
				strMsg.Format("创建路径\"%s\"失败！是否继续?", Disprity_PointCloud_Namefolder);
				if (AfxMessageBox(strMsg, MB_YESNO) == IDYES)
				{
					return 0;
				}
			}

		}

	}

	
	//这里开始进行循环的校正、匹配过程
	while (b_3DConstruction)
	{

		//判断摄像机是否开启，如果没开启则退出
		if (b_OpenCamera)
		{
			coperTerView->MessageBox("摄像机已关闭，三维重建停止！");
			coperTerView->ShowMessage("摄像机已关闭，三维重建停止！");
			return 0;
		}
		//这里放置立体校正和立体匹配算法
		

/********************************立体校正****************************************************/
		
		//创建用于立体校正必要的中间过程矩阵
		


		//立体校正得到P1和P2
		cvStereoRectify(Intrinsics_Camera_Left, Intrinsics_Camera_Right, Distortion_Camera_Left,
			Distortion_Camera_Right, imageSize, RotRodrigues_matlab, Translation_matlab,&_R1, &_R2, &_P1, &_P2, Q,
			0/*CV_CALIB_ZERO_DISPARITY*/);

		//使用立体校正得到的P1和P2来得到 左图像的mx1, my1和右图像的mx2, my2
		cvInitUndistortRectifyMap(Intrinsics_Camera_Left, Distortion_Camera_Left, &_R1, &_P1, mx1, my1);
		cvInitUndistortRectifyMap(Intrinsics_Camera_Right, Distortion_Camera_Right, &_R2, &_P2, mx2, my2);

		//使用左图像的mx1, my1和右图像的mx2, my2来校正左右图像
		if (LeftCameraImage && RightCameraImage)
		{
			cvRemap(LeftCameraImage, img1r, mx1, my1);
			cvRemap(RightCameraImage, img2r, mx2, my2);
		}

		//保存校正好的左右图像
		//cvSaveImage("img1r.jpg", img1r);
		//cvSaveImage("img2r.jpg", img2r);
		
/*******************************立体匹配*****************************************************/
		//立体匹配所用时间计算时间
		int64 t = getTickCount();
		int Time = 0;
		//调用立体匹配函数SGBM
		coperTerView->StereoMatch_SGBM((Mat)img1r, (Mat)img2r, coperTerView->SADSizeValue,coperTerView->DisparityValue, coperTerView->disp, coperTerView->disp8);
		
		t = getTickCount() - t;
		Time = t * 1000 / getTickFrequency();
		CString time;
		time.Format("%d", Time);
		coperTerView->ShowMessage("立体匹配时间:" + time + "ms");
		
		//判断显示方式
		//显示视差图
		IplImage *Idisp8 = &(IplImage)coperTerView->disp8;
		if (coperTerView->b_DesprityMapStyple == true)
		{
			
			coperTerView->ShowImage(Idisp8, IDC_STATIC_PIC_Parallax);
		}
		
		//显示伪彩色图
		if (coperTerView->b_PseudoColorStyple == true)
		{
			//将Mat类型的disp8转换成CvMat类型的
			Temp = coperTerView->disp8;
			cvCopy(&Temp, Cvdisp8);
			coperTerView->Disprity2Color(Cvdisp8, Disparity2RGB);


			IplImage *Idisp = cvCreateImage(cvSize(imageSize.height, imageSize.width), 8, 3);

			cvGetImage(Disparity2RGB, Idisp);
			coperTerView->ShowImage(Idisp, IDC_STATIC_PIC_Parallax);
		}
		
		if (!PointCloudShowRead)
		{
			PointCloudShowWrite = true;
			//PointCloud.clear();
			Mat _Q(Q);
			reprojectImageTo3D(coperTerView->disp, _3dImage1, _Q, true);
			
			_3dImage =_3dImage1;
			k = 2;
           // PointCloudShowing = false;
			PointCloudShowWrite = false;
         }

		//判断是否需要保存拍照图片，视差图以及点云文件
		if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SaveDisprity) == 1 ||
			coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePointCloud) == 1||
			coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1)
		{
			CTime TheTime = CTime::GetCurrentTime();                            //获取系统时间
			CString Namedtime;                                                  //图片以时间命名
			Namedtime.Format("%04d%02d%02d_%02d%02d%02d", TheTime.GetYear(),
				TheTime.GetMonth(), TheTime.GetDay(), TheTime.GetHour(),
				TheTime.GetMinute(), TheTime.GetSecond());                     //得到系统时间并输出
			string Namedtime1 = CT2A(Namedtime.GetBuffer());                   //由CString转换成string



			//左侧相机图片命名，并保存图像，格式为Jpg
			if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SaveDisprity) == 1)
			{

				strFileName_Disprity = "Disprity_" + to_string(Disprity_cntImagesNum) + "_" + Namedtime1 + ".bmp";
				string Namefolder2 = Disprity_PointCloud_Namefolder1 + "\\" + strFileName_Disprity;
				const char *Disparity_Name = Namefolder2.c_str();       //这里的指针不可以改变，但是指针指向地址的内容是可以改变的
				cvSaveImage(Disparity_Name, Idisp8);
			}
			if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePointCloud) == 1)
			{

				strFileName_PointCloud = "PointCloud_" + to_string(PointCloud_cntImagesNum) + "_" + Namedtime1 + ".txt";
				string Namefolder3 = Disprity_PointCloud_Namefolder1 + "\\" + strFileName_PointCloud;
				const char *PointCloud_Name = Namefolder3.c_str();
				coperTerView->saveXYZ(PointCloud_Name, _3dImage1);

			}
			if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1)
			{
				//左侧相机图片命名，并保存图像，格式为Jpg
				strFileName_left = "Basler_left_" + to_string(Picture_CntImagesNum) + "_" + Namedtime1 + ".jpg";
				string Namefolder4 = Disprity_PointCloud_Namefolder1 + "\\" + strFileName_left;
				const char *Picture_Name_Left = Namefolder4.c_str();
				cvSaveImage(Picture_Name_Left, LeftCameraImage);
				////右侧相机图片命名，并保存，格式为Jpg

				strFileName_right = "Basler_right_" + to_string(Picture_CntImagesNum) + "_" + Namedtime1 + ".jpg";
				string Namefolder5 = Disprity_PointCloud_Namefolder1 + "\\" + strFileName_right;
				const char *Picture_Name_Right = Namefolder5.c_str();
				cvSaveImage(Picture_Name_Right, LeftCameraImage);

			}

			Disprity_cntImagesNum++;
			PointCloud_cntImagesNum++;
			Picture_CntImagesNum++;
		}
		
		
		b_ImageUsing = false;

	}
	//释放内存资源
	cvReleaseMat(&Intrinsics_Camera_Left);
	cvReleaseMat(&Intrinsics_Camera_Right);
	cvReleaseMat(&Distortion_Camera_Left);
	cvReleaseMat(&Distortion_Camera_Right);
	cvReleaseMat(&Translation_matlab);
	cvReleaseMat(&RotRodrigues_matlab);
	cvReleaseMat(&Q);
	cvReleaseMat(&img1r);
	cvReleaseMat(&img2r);
	cvReleaseMat(&Disparity2RGB);
	cvReleaseMat(&Cvdisp8);

	cvReleaseMat(&mx1);
	cvReleaseMat(&my1);
	cvReleaseMat(&mx2);
	cvReleaseMat(&my2);

	return 1;
	
}


/*****************************************************************************************/
/*                                    立体匹配算法SGBM                                   */
/*                  输入左图像、右图像，搜索窗口大小、视差范围；输出视差图               */
/*****************************************************************************************/
int CBinocularVisionDlg::StereoMatch_SGBM(Mat &img1, Mat &img2, int SADWindowSize, int numberOfDisparities, Mat & disp, Mat & disp8)
{
	StereoSGBM sgbm;
	Rect roi1, roi2;
	sgbm.preFilterCap = 63;
	sgbm.SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 3;

	int cn = img1.channels();
	sgbm.P1 = 4 * cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.P2 = 32 * cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.minDisparity = 0;
	sgbm.numberOfDisparities = numberOfDisparities;
	sgbm.uniquenessRatio = 10;
	sgbm.speckleWindowSize = 100;
	sgbm.speckleRange = 32;
	sgbm.disp12MaxDiff = 1;
	sgbm.fullDP = false;

	sgbm(img1, img2, disp);
	disp.convertTo(disp8, CV_8U, 255 / (numberOfDisparities*16.));

	return 0;
	
	
}


/*****************************************************************************************/
/*                          把视差图转换长三维点云坐标                                   */
/*                                    输入视差图                                         */
/*****************************************************************************************/

void CBinocularVisionDlg::DispToXYZ(const Mat& mat)
{
	const double max_z = 150;
	for (int y = 0; y < mat.rows; y=y+1)
	{
		for (int x = 0; x < mat.cols; x=x+1)
		{
			point1 = mat.at<Vec3f>(y, x);
			if (fabs(point1[2] - max_z) < FLT_EPSILON || fabs(point1[2]) > max_z) 
				continue;
			PointCloud.push_back(point1);
		}
	}

}

/*****************************************************************************************/
/*                     把视差图转换长三维点云坐标                                        */
/*                     输入视差图  输出PC.txt文件                                        */
/*****************************************************************************************/

void CBinocularVisionDlg::saveXYZ(const char* filename, const Mat& mat)
{
	const double max_z = 150;
	FILE* fp = fopen(filename, "wt");
	for (int y = 0; y < mat.rows; y=y+10)
	{
		for (int x = 0; x < mat.cols; x=x+10)
		{
			Vec3f point = mat.at<Vec3f>(y, x);
			if (fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
			fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
		}
	}
	fclose(fp);
}


/*****************************************************************************************/
/*                          开启三维点云显示的线程                                       */
/*                          显示实时更新变换的点云                                       */
/*****************************************************************************************/

void CBinocularVisionDlg::BeginShow3DPointCloud()
{
	// TODO:  在此添加控件通知处理程序代码


	PointCloudShowRead = false;
	PointCloudShowWrite = false;
	
	PointCloudView_Thread = AfxBeginThread(PointCloudViewThread, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
    //PointCloudView_Thread->ResumeThread();

	
}

void CBinocularVisionDlg::StopShow3DPointCloud()
{
	// TODO:  在此添加控件通知处理程序代码
	PointCloudShowWrite = true;
	PointCloudShowRead = true;
	//::WaitForSingleObject(PointCloudView_Thread->m_hThread, INFINITE);
	//TerminateThread(PointCloudView_Thread->m_hThread, 0);
	//delete PointCloudView_Thread;
	//PointCloudView_Thread = NULL;
}
/*****************************************************************************************/
/*                              三维点云显示的线程                                       */
/*****************************************************************************************/
UINT CBinocularVisionDlg::PointCloudViewThread(void *param)
{
	CBinocularVisionDlg* PointCloudView = (CBinocularVisionDlg*)param;
	int argc = 1;
	char* argv[] = { "MFC_GLUT" };
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("3DPointCloud");
	
	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutDisplayFunc(RenderScene);
	//glutIdleFunc(RenderScene);
	//glutKeyboardFunc(myKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(processNormalKeys);
	SetupRC();
	//每次进入glutMainLoop都会无限循环，不会跳出；
	glutTimerFunc(100, timer, 0);
	glutMainLoop();
	return 0;

}

void  SetupRC()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1);  //以RGB(0,0,0)即黑色来清空颜色缓冲区
	glColor3f(1.0f, 0.0f, 0.0f);        //以RGB(1,0,0)即红色来绘制图形
}

/*****************************************************************************************/
/*                                     鼠标响应函数                                      */
/*****************************************************************************************/

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			leftClickHold = true;
		}
		else
		{
			leftClickHold = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			rightClickHold = true;
		}
		else
		{
			rightClickHold = false;
		}
	}

}

/*****************************************************************************************/
/*                                     窗口响应函数                                      */
/*****************************************************************************************/

void  motion(int x, int y)
{

	int rstep = 10;
	if (leftClickHold == true)
	{
		if (abs(x - mx) > abs(y - my))
		{
			rx -= SIGN(x - mx)*rstep;
		}
		else
		{
			ry += SIGN(y - my)*rstep;
		}

		mx = x;
		my = y;
		glutPostRedisplay();
	}
	if (rightClickHold == true)
	{
		if (y - my > 0)
		{
			radius += 100.0;
		}
		else if (y - my < 0)
		{
			radius -= 100.0;
		}
		radius = std::max(radius, 100.0);
		mx = x;
		my = y;
		glutPostRedisplay();
	}
}


/*****************************************************************************************/
/*                                 点云赋值重绘函数                                      */
/*****************************************************************************************/
void RenderScene()
{
	
	   
		// clear screen and depth buffer  
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glClearColor(1.0, 1.0, 0.6, 1.0);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);                            //对投影矩阵操作
		// Reset the coordinate system before modifying   
		glLoadIdentity();                                       //设置成单位矩阵
		
		// set the camera position  
		atx = 0.0f;
		aty = 0.0f;
		atz = 100;
		eyex = atx + radius * sin(CV_PI * ry / 180.0f) * cos(CV_PI * rx / 180.0f);
		eyey = aty + radius * cos(CV_PI * ry / 180.0f);
		eyez = atz + radius * sin(CV_PI * ry / 180.0f) * sin(CV_PI * rx / 180.0f);

		glOrtho(-50, 50, -50, 50, 20, 150);
		gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0.0, 1.0, 0.0);

		glPointSize(1.0f);//指定点的大小，1个像素单位
		glColor3f(1.0f, 0.0f, 0.0f);                     //指定点的颜色
		//glColor3f(0.0f, 1.0f, 0.0f);                     //指定点的颜色
		glPointSize(1.0f);                               //指定点的大小，1个像素单位
		//glVertex3f(0, 0, 0);                           // 在坐标为(0,0,0)的地方绘制了一个点
		glBegin(GL_POINTS);                             //开始画点
		
		int cntPts = 0;

		if (!PointCloudShowWrite)
		{
			//glVertex3f(0, 0, 0);
			
			PointCloudShowRead = true;
			const double max_z = 300;
			for (int y = 0; y < _3dImage.rows; y++)
			{
				for (int x = 0; x < _3dImage.cols; x++)
				{
					point = _3dImage.at<Vec3f>(y, x);
					if (fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z)
						continue;
					cntPts++;
					glVertex3f(point[0], point[1], point[2]); // 在坐标为(0,0,0)的地方绘制了一个点
				}
			}
			PointCloudShowRead = false;
			//PointCloudShowed = false;
		/*
		for (int i = 0; i < PointCloud.size(); i++)
		{
			point[0] = PointCloud[i].val[0];
			point[1] = PointCloud[i].val[1];
			point[2] = PointCloud[i].val[2];
			glVertex3f(point[0], point[1], point[2]);
		}
		*/
		

	}
		
		glEnd();

		// enable blending  
		glEnable(GL_BLEND);
		// enable read-only depth buffer  
		glDepthMask(GL_FALSE);
		// set the blend function to what we use for transparency  
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		// set back to normal depth buffer mode (writable)  
		glDepthMask(GL_TRUE);
		// disable blending  
		glDisable(GL_BLEND);

		//glFlush();
		glutSwapBuffers();
		
		
}

/*****************************************************************************************/
/*                   定时器函数，不断调用更新 RenderScene()函数                          */
/*****************************************************************************************/
void timer(int p)

{
   
   RenderScene();
   glutTimerFunc(100, timer, 0);

}

void processNormalKeys(unsigned char key, int x, int y) 
{

	if (key == 27)
		exit(0);
}

/*****************************************************************************************/
/*                                        退出函数                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::OnClickedButtonExit()
{
	// TODO:  在此添加控件通知处理程序代码
	if (MessageBox("确认退出", "", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}
	CDialog::OnOK();
}

/*****************************************************************************************/
/*                                        设置函数                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::Setting()
{
	// TODO:  在此添加控件通知处理程序代码
	BinocularVisionDlg1 dlg1;
	dlg1.DoModal();
	return;

}


/*****************************************************************************************/
/*                                    立体标定函数                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::StereoCalibration_GrapPicture()
{
	// TODO:  在此添加控件通知处理程序代码
	
	//更新用户选项
	if (i_GrapStereoCalibrationPicture == 0)
	{

		UpdateData(TRUE);
		//判断
		switch (m_CalibrationResolution)
		{
		case 0:
			StereoCalibration_Normal_ImageWidth = 480;
			StereoCalibration_Normal_ImageHigh = 320;
			break;
		case 1:
			StereoCalibration_Normal_ImageWidth = 640;
			StereoCalibration_Normal_ImageHigh = 480;
			break;
		case 2:
			StereoCalibration_Normal_ImageWidth = 1280;
			StereoCalibration_Normal_ImageHigh = 960;
			break;
		default:
			MessageBox("请选择正确的标定图像分辨率！");
			return;
		} 
		i_GrapStereoCalibrationPicture++;           //用于判断是开启摄像机还是抓拍图像用于标定         
		b_StereoCalibrationOpenCameraed = true;     //用于结束立体标定线程的变量线程
		
		//打开摄像机线程
		Calibration_OpenCamera_Thread = AfxBeginThread(CalibrationOpenCameraThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);         

	}
	else
	{
		b_StereoCalibrationGrapPicture = true;
		i_GrapStereoCalibrationPicture++;
	}

}

/*****************************************************************************************/
/*                                    完成立体标定                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::AccomplishCalibration_GrapPicture()
{
	// TODO:  在此添加控件通知处理程序代码

	
	b_StereoCalibrationOpenCameraed = false;                          //关闭摄像机
	

	//棋盘格图像的大小设定清零
	StereoCalibration_CheckboardWidthNum = 0;                           
	StereoCalibration_CheckboardHighNum = 0;
	
	//棋盘格宽，高的个数清零
	StereoCalibration_CheckboardWidthNum =0;
	StereoCalibration_CheckboardHighNum = 0;
	
	//棋盘格大小清零
	CheckboardSize = 0;
}


/*****************************************************************************************/
/*                               立体标定抓拍图像的线程                                  */
/*****************************************************************************************/
UINT CBinocularVisionDlg::CalibrationOpenCameraThread(void *param)
{
	CBinocularVisionDlg* StereoCalibration = (CBinocularVisionDlg*)param;
	// TODO:  在此添加控件通知处理程序代码
	int i_StereoCalibrationCameraNum = 2;                                             //立体标定时摄像机的数量
	CInstantCameraArray StereoCalibrationCameras(2);                                  //创建两个两个摄像机对象
	int exitCode = 0;                                                                 //判断异常代码
	int i_StereoCalibrationLeftCameraIndex = 0, i_StereoCalibrationRightCameraIndex = 0;

	CPylonImage StereoCalibrationImgLeft, StereoCalibrationImgRight;                  //Pylon格式的图像
	CPylonImage StereoCalibrationPylonImageL, StereoCalibrationPylonImageR;           //创建Basler相机的图片格式
	Mat StereoCalibration_OpencvImage_left, StereoCalibration_OpencvImage_right;      //用于pylonImage转换成Mat格式的图像
	CGrabResultPtr StereoCalibration_ptrGrabResultL, StereoCalibration_ptrGrabResultR;//智能指针                 
	CImageFormatConverter StereoCalibration_formatConverter;                          //图像的格式转换
	CDeviceInfo StereoCalibration_aDeviceInfos[2];
	string StereoCalibration_strDeviceSerialNumbers[2];
	DeviceInfoList_t StereoCalibration_devices;                                       // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
	IplImage *StereoCalibration_ImageLeft;
	IplImage *StereoCalibration_ImageRight;




	//创建存储抓拍照片的文件夹
	CTime TheTime = CTime::GetCurrentTime();                                           //获取系统时间
	CString GrapPicture_Timefolder;                                                    //图片以时间命名
	CString  GrapPicture_Namefolder;                                                   //实时抓拍图片存储的文件夹名称以及路径
	string  GrapPicture_Namefolder1;
	GrapPicture_Timefolder.Format("%04d%02d%02d_%02d%02d%02d", TheTime.GetYear(),
			TheTime.GetMonth(), TheTime.GetDay(), TheTime.GetHour(),
			TheTime.GetMinute(), TheTime.GetSecond());                                 //得到系统时间并输出
	GrapPicture_Namefolder = "GrapPicture_" + GrapPicture_Timefolder;
	GrapPicture_Namefolder1 = CT2A(GrapPicture_Namefolder.GetBuffer());      //由CString转换成string
	// 判断路径是否存在
	if (!PathIsDirectory(GrapPicture_Namefolder))
	{
		CString strMsg;
		if (!CreateDirectory(GrapPicture_Namefolder, NULL))
		{
			strMsg.Format("创建路径\"%s\"失败！是否继续?", GrapPicture_Namefolder);
			if (AfxMessageBox(strMsg, MB_YESNO) == IDYES)
			{
				return 0;
			}
		}

	}

	

	//判断设置的图像分辨率
	//coperTerView->UpdateData(TRUE);
	
	IplImage *Normal_ImageLeft = cvCreateImage(cvSize(StereoCalibration->StereoCalibration_Normal_ImageWidth, StereoCalibration->StereoCalibration_Normal_ImageHigh), IPL_DEPTH_8U, 1);
	IplImage *Normal_ImageRight = cvCreateImage(cvSize(StereoCalibration->StereoCalibration_Normal_ImageWidth, StereoCalibration->StereoCalibration_Normal_ImageHigh), IPL_DEPTH_8U, 1);

	PylonInitialize();

	try
	{
		// Get the transport layer factory.
		CTlFactory& tlFactory = CTlFactory::GetInstance();

		//// Get all attached devices and exit application if no device is found.
		//DeviceInfoList_t devices;
		//判断摄像机是否连接
		if (tlFactory.EnumerateDevices(StereoCalibration_devices) == 0)
		{
			throw RUNTIME_EXCEPTION("No camera present.");
		}

		//// Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
		//CInstantCameraArray cameras(min(devices.size(), c_maxCamerasToUse));

		// Create and attach all Pylon Devices.
		for (size_t i = 0; i < StereoCalibrationCameras.GetSize(); ++i)
		{
			StereoCalibrationCameras[i].Attach(tlFactory.CreateDevice(StereoCalibration_devices[i]));

			// Print the model name of the camera.
			std::cout << "Using device " << StereoCalibrationCameras[i].GetDeviceInfo().GetModelName() << endl;
		}

		//确定摄像机的序列号
		for (int i = 0; i < i_StereoCalibrationCameraNum ; i++)
		{
			StereoCalibration_aDeviceInfos[i] = StereoCalibrationCameras[i].GetDeviceInfo();
			StereoCalibration_strDeviceSerialNumbers[i] = StereoCalibration_aDeviceInfos[i].GetSerialNumber();

			//规定序列号为“21770467”的相机为左侧相机
			if (StereoCalibration_strDeviceSerialNumbers[i] == "21900819")
			{
				i_StereoCalibrationLeftCameraIndex = i;
			}
			else if (StereoCalibration_strDeviceSerialNumbers[i] == "21770468")
			{

				i_StereoCalibrationRightCameraIndex = i;
			}
		}



		StereoCalibrationCameras.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByUser);  //开启抓取图像



		while (StereoCalibrationCameras.IsGrabbing() && b_StereoCalibrationOpenCameraed)
		{
			StereoCalibrationCameras[i_StereoCalibrationLeftCameraIndex].RetrieveResult(5000, StereoCalibration_ptrGrabResultL, TimeoutHandling_ThrowException);
			StereoCalibrationCameras[i_StereoCalibrationRightCameraIndex].RetrieveResult(5000, StereoCalibration_ptrGrabResultR, TimeoutHandling_ThrowException);

			if (StereoCalibration_ptrGrabResultL->GrabSucceeded() && StereoCalibration_ptrGrabResultR->GrabSucceeded())
			{
				StereoCalibrationImgLeft.AttachGrabResultBuffer(StereoCalibration_ptrGrabResultL);
				StereoCalibrationImgRight.AttachGrabResultBuffer(StereoCalibration_ptrGrabResultR);
			}




			//BaslerSDK指向图像的指针转换成CImageFormatConverter格式的图像

			StereoCalibration_formatConverter.Convert(StereoCalibrationPylonImageL, StereoCalibration_ptrGrabResultL);
			StereoCalibration_formatConverter.Convert(StereoCalibrationPylonImageR, StereoCalibration_ptrGrabResultR);

			// 把pylonImage图像格式转换成OpenCV里Mat格式的图像

			StereoCalibration_OpencvImage_left = Mat(StereoCalibration_ptrGrabResultL->GetHeight(), StereoCalibration_ptrGrabResultL->GetWidth(), CV_8UC1,
				                                      (uint8_t *)StereoCalibrationPylonImageL.GetBuffer());
			StereoCalibration_OpencvImage_right = Mat(StereoCalibration_ptrGrabResultR->GetHeight(), StereoCalibration_ptrGrabResultR->GetWidth(), CV_8UC1, 
				                                      (uint8_t *)StereoCalibrationPylonImageR.GetBuffer());


			StereoCalibration_ImageLeft = &(IplImage)StereoCalibration_OpencvImage_left;
			StereoCalibration_ImageRight = &(IplImage)StereoCalibration_OpencvImage_right;

			//将图像缩小到()
			cvResize(StereoCalibration_ImageLeft, Normal_ImageLeft, CV_INTER_LINEAR);
			cvResize(StereoCalibration_ImageRight, Normal_ImageRight, CV_INTER_LINEAR);

			//cvSaveImage("left1.jpg", Normal_ImageLeft);
			//cvSaveImage("right1.jpg", Normal_ImageLeft);

			//调用函数将图像显示在Picture控件中
			StereoCalibration->ShowImage(StereoCalibration_ImageLeft, IDC_STATIC_PIC_Left);
			StereoCalibration->ShowImage(StereoCalibration_ImageRight, IDC_STATIC_PIC_Right);

			//判断要不要抓拍
			if (b_StereoCalibrationGrapPicture)
			{
				string LeftName = "left" + to_string(i_GrapStereoCalibrationPicture-1) + ".jpg";      //给左图像编号
				string RightName= "right" + to_string(i_GrapStereoCalibrationPicture-1) + ".jpg";     //给右图像编号
				string LeftName1 = GrapPicture_Namefolder1 + "\\" + LeftName;
				string RightName1 = GrapPicture_Namefolder1 + "\\" + RightName;
				cvSaveImage(LeftName1.c_str(), Normal_ImageLeft);                         //保存左图像
				cvSaveImage(RightName1.c_str(), Normal_ImageRight);                       //保存右图像
				StereoCalibration->ShowImage(StereoCalibration_ImageLeft, IDC_STATIC_PIC_Left);
				StereoCalibration->ShowImage(StereoCalibration_ImageRight, IDC_STATIC_PIC_Right);
				Sleep(1000);
				b_StereoCalibrationGrapPicture = false;

				
			}

		}
	}
	catch (const GenericException &e)
	{

		// 异常处理
		cerr << "An exception occurred." << endl << e.GetDescription() << endl;
		exitCode = 1;

	}
	return 1;

}


/*****************************************************************************************/
/*                                    立体标定的线程                                     */
/*****************************************************************************************/
void CBinocularVisionDlg::BeginStereoCalibretion()
{
	// TODO:  在此添加控件通知处理程序代码
	

	UpdateData(TRUE);
	switch (m_InnerAnglePoint)
	{
	case 0:
		StereoCalibration_CheckboardWidthNum = 12;
		StereoCalibration_CheckboardHighNum = 9;
		break;
	case 1:
		StereoCalibration_CheckboardWidthNum = 9;
		StereoCalibration_CheckboardHighNum = 9;
		break;
	case 2:
		StereoCalibration_CheckboardWidthNum = 9;
		StereoCalibration_CheckboardHighNum = 6;
		break;
	default:
		MessageBox("请选择正确的棋盘格数量！");
		return;
	}

	switch (m_CheckboardSize)
	{
	case 0:
		CheckboardSize = 40;
		break;
	case 1:
		CheckboardSize = 25;
		break;
	case 2:
		CheckboardSize = 10;
		break;
	default:
		MessageBox("请选择正确的棋盘格大小！");
		return;
	}




	CString StereoCalibretion_strFilePathTxt1;                            //用于加载txt处理文件的地址
	vector<CString> vecResult;                                          //用于存储读取txt中内容向量                                                                     
	int displayCorners = 1;
	int showUndistorted = 1;
	bool isVerticalStereo = false;//OpenCV can handle left-right
	//or up-down camera arrangements
	const int maxScale = 1;
	const float squareSize = CheckboardSize; //Set this to your actual square size
	int i, j, lr, nframes;
	int n = StereoCalibration_CheckboardWidthNum*StereoCalibration_CheckboardHighNum;
	int N = 0;
	vector<string> imageNames[2];                      //保存左右图片的路径，0为左，1为右
	vector<CvPoint3D32f> objectPoints;                 //保存nframes个角点坐标；
	vector<CvPoint2D32f> points[2];                    //保存左右图片的总角点坐标；
	vector<int> npoints;                               //角点的总数目；
	vector<uchar> active[2];                           //保存左右角点检测结果的返回值
	vector<CvPoint2D32f> temp(n);                      //保存每个棋盘角点的坐标；
	CvSize imageSize = { 0, 0 };


	double M1[3][3], M2[3][3], D1[5], D2[5];
	double R[3][3], T[3], E[3][3], F[3][3];
	CvMat C_M1 = cvMat(3, 3, CV_64F, M1);               //左摄像机内参数矩阵
	CvMat C_M2 = cvMat(3, 3, CV_64F, M2);               //右摄像机内参数矩阵
	CvMat C_D1 = cvMat(1, 5, CV_64F, D1);               //左摄像机畸变矩阵
	CvMat C_D2 = cvMat(1, 5, CV_64F, D2);               //右摄像机畸变矩阵
	CvMat C_R = cvMat(3, 3, CV_64F, R);                 //旋转矩阵
	CvMat C_T = cvMat(3, 1, CV_64F, T);                 //平移矩阵
	CvMat C_E = cvMat(3, 3, CV_64F, E);                 //本征矩阵
	CvMat C_F = cvMat(3, 3, CV_64F, F);                 //基础矩阵

	//开始读取存放拍照的文件夹
	//加载图片的路径
	

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files|*.*|txt|*.txt||");
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return;
	}

	StereoCalibretion_strFilePathTxt1 = dlg.GetPathName();


	if (!PathFileExists(StereoCalibretion_strFilePathTxt1))
	{
		MessageBox("文件不存在！");
		return;
	}
	

	//读取的文件不存在
	if (!PathFileExists(StereoCalibretion_strFilePathTxt1))
	{
		return;
	}
	CString cstrFilePath = "";
	char fatherPath[256] = { 0 };
	strcpy(fatherPath, StereoCalibretion_strFilePathTxt1);
	*(strrchr(fatherPath, '\\')) = '\0';                                            //删除文件名，只留下加载的目录
	cstrFilePath = fatherPath;                                                      //这是父目录
	string felod = cstrFilePath.GetBuffer();                                       //上一级目录的string
	string felod1 = felod+"\\";
	CString strValue = _T("");





	CStdioFile file;
	//判断是否正确打开txt文件
	if (!file.Open(StereoCalibretion_strFilePathTxt1, CFile::modeRead))
	{
		return;
	}

	
	while (file.ReadString(strValue))
	{
		vecResult.push_back(strValue);
	}
	//关闭txt文件
	file.Close();
	ShowMessage("立体标定开始！");



	for (i = 0; i<vecResult.size(); i++)
	{
		
		int count = 0, result = 0;
		lr = i % 2;
		vector<CvPoint2D32f>& pts = points[lr];

		string bif1 = vecResult[i].GetBuffer();
		string ImageName = felod1 +  bif1;
		IplImage* img = cvLoadImage(ImageName.c_str(), 0);          //加载图像

		if (!img)                                     //判断图像是否为空
		{
			break;
		}

		imageSize = cvGetSize(img);                  //得到图像的大小
		imageNames[lr].push_back(bif1.c_str());      //把图片的路径放入imageNames中


		for (int s = 1; s <= maxScale; s++)
		{
			IplImage* timg = img;
			if (s > 1)
			{
				timg = cvCreateImage(cvSize(img->width*s, img->height*s), img->depth, img->nChannels); //得到img图像相同的大小，深度和通道
				cvResize(img, timg, CV_INTER_CUBIC);                                                  //得到相同的大小
			}

			//寻找角点坐标
			result = cvFindChessboardCorners(timg, cvSize(StereoCalibration_CheckboardWidthNum, StereoCalibration_CheckboardHighNum), &temp[0], &count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
			if (timg != img)
			{
				cvReleaseImage(&timg);
			}

			if (result || s == maxScale)
			for (j = 0; j < count; j++)
			{
				temp[j].x /= s;
				temp[j].y /= s;
			}
			if (result)
			{
				break;                 //表示寻找角点失败
			}
		}

		
		
		cvDrawChessboardCorners(img, cvSize(StereoCalibration_CheckboardWidthNum, StereoCalibration_CheckboardHighNum), &temp[0],count, result);
		ShowImage(img, IDC_STATIC_PIC_Left);
		ShowMessage(bif1.c_str());      //输出图像的路径
		Sleep(2000);
		

			
		N = pts.size();
		pts.resize(N + n, cvPoint2D32f(0, 0));
		active[lr].push_back((uchar)result);
		//assert( result != 0 );
		if (result)
		{
			//开始寻找亚像素角点坐标
			cvFindCornerSubPix(img, &temp[0], count, cvSize(11, 11), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));
			copy(temp.begin(), temp.end(), pts.begin() + N);            //没看懂什么意思，应该是把temp的角点左边放到point中去了；
		}
		cvReleaseImage(&img);
	}
	
	// HARVEST CHESSBOARD 3D OBJECT POINT LIST:
	nframes = active[0].size();//Number of good chessboads found
	objectPoints.resize(nframes*n);
	for (i = 0; i < StereoCalibration_CheckboardHighNum; i++)
	for (j = 0; j < StereoCalibration_CheckboardWidthNum; j++)
		objectPoints[i*StereoCalibration_CheckboardWidthNum + j] =
		cvPoint3D32f(i*squareSize, j*squareSize, 0);
	for (i = 1; i < nframes; i++)
		copy(objectPoints.begin(), objectPoints.begin() + n,
		objectPoints.begin() + i*n);
	npoints.resize(nframes, n);
	N = nframes*n;
	CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0]);
	CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0]);       //左边第0幅图像的角点坐标
	CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0]);       //右边第0幅图像的角点坐标
	CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0]);
	cvSetIdentity(&C_M1);                                              //将行和列相等的设置为1
	cvSetIdentity(&C_M2);
	cvZero(&C_D1);
	cvZero(&C_D2);

	// CALIBRATE THE STEREO CAMERAS
	fflush(stdout);
	cvStereoCalibrate(&_objectPoints, &_imagePoints1, &_imagePoints2, &_npoints, &C_M1, &C_D1, &C_M2, &C_D2, imageSize, &C_R, &C_T, &C_E, &C_F, cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5),
		CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH);

	
	cvSave("M1.xml", &C_M1);
	cvSave("M2.xml", &C_M2);
	cvSave( "D1.xml", &C_D1);
	cvSave( "D2.xml", &C_D2);
	cvSave( "R.xml", &C_R);
	cvSave( "T.xml", &C_T);
	cvSave( "E.xml", &C_E);
	cvSave( "F.xml", &C_F);
	

	ShowMessage("立体标定结束！");
	return;

}
