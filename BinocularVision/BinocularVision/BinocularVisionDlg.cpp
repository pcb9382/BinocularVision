
// BinocularVisionDlg.cpp : ʵ���ļ�
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
#include "conio.h"//Ϊ��ʹ��_getchar()����ʵ�ֲ�����س�ֱ�ӻ�ȡ�ڿ���̨������ַ�
#include <windows.h>
#include <process.h>
#include <pcl/visualization/cloud_viewer.h>                       //��CloudViewerͷ�ļ�����
#include <iostream>
#include <pcl/io/io.h>                                            //I/O���ͷ�ļ�
#include <pcl/io/pcd_io.h>                                        //PCD�ļ���ȡͷ�ļ�����
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


//�궨��
#define SIGN(x) ( (x)<0 ? -1:((x)>0?1:0 ) )  
#define DLG_FILTER "ALL Files(.) | . || "
#define DLG_EXT "*" 


using namespace std;
using namespace cv;
using namespace Pylon;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CBinocularVisionDlg �Ի���



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


// CBinocularVisionDlg ��Ϣ�������

BOOL CBinocularVisionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	
	OpenCamera_Thread = NULL;



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBinocularVisionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBinocularVisionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*****************************************************************************************/
/*                            �����̼߳��ȫ�ֱ���                                       */
/*****************************************************************************************/

volatile bool b_OpenCamera = true;                     //��������ر�־λ
volatile bool b_StereoCalibrationOpenCameraed =false;  //�����ж�����궨ʱ��������Ŀ���
int i_GrapStereoCalibrationPicture = 0;                //�����жϰ�������궨ʱ��ť�ǿ������������ץ����Ƭ
volatile bool b_StereoCalibrationGrapPicture = false;  //�����·�ץ��ͼ�������

IplImage * LeftCameraImage;                            //��ͼ������������߳�����ά�ؽ��߳�ͨ�ŵ�ȫ�ֱ�����
IplImage * RightCameraImage;                           //��ͼ������������߳�����ά�ؽ��߳�ͨ�ŵ�ȫ�ֱ�����
volatile bool b_ImageUsing = false;                    //����ȷ��ͼ����ȷд����ͨ�ŵı�־λ
volatile bool b_3DConstruction = false;                //���ڿ�ʼ�ͽ�����ά�ؽ��߳�
volatile bool Capture = false;                         //���ڿ�ʼ�ͽ�����ά�ؽ��߳�
volatile bool PointCloudShowRead ;                     //�����ж��Ӳ�ͼ�Ƿ��ڶ��ı�־λ
volatile bool PointCloudShowWrite ;                    //�����ж��Ӳ�ͼ�Ƿ���д�ı�־λ
volatile bool OpenCameraed=false;                      //�����ж�����Ƿ����ı�־λ
string strFileName_left = "";                          //������Ҫ����ͼ��ʱ����ͼ�������
string strFileName_right = "";                         //������Ҫ����ͼ��ʱ����ͼ�������

Vec3f point1;
vector<Vec3f>PointCloud;                               //�洢XYZ����Ϣ 
int glWinWidth = 640, glWinHeight = 480;
double eyex, eyey, eyez, atx, aty, atz;                // eye* - �����λ�ã�at* - ע�ӵ�λ��  
bool leftClickHold = false, rightClickHold = false;
int mx, my;                                            // ��갴��ʱ�� OpenGL ���ڵ�����  
int ry = 90, rx = 90;                                  // ��������ע�ӵ�Ĺ۲�Ƕ�  
double radius = 80.0;                                  // �������ע�ӵ�ľ��� 
void SetupRC();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void RenderScene();
void processNormalKeys(unsigned char key, int x, int y);
void timer(int p);

int k = 1;
Mat _3dImage;
Mat _3dImage1;                                         //�����Ӳ�ͼת���ɵ���
Vec3f point;
/*****************************************************************************************/
/*                    ������Picture�ռ�����ʾͼ��ĺ���                                  */
/*                    ����Ϊͼ��IplImageָ�룬�Լ��������ӦPicture�ؼ���ID              */
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
/*                          ������list�ؼ�����ʾ�б���Ϣ                                 */
/*                    ����Ϊ CString str����list�ռ������CString str������              */
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
/*                              �����Ӳ�ͼת����α��ɫͼ                                 */
/*****************************************************************************************/

void CBinocularVisionDlg::Disprity2Color(CvMat* gray_mat, CvMat* color_mat)
{
	if (color_mat)
		cvZero(color_mat);

	int stype = CV_MAT_TYPE(gray_mat->type), dtype = CV_MAT_TYPE(color_mat->type);
	int rows = gray_mat->rows, cols = gray_mat->cols;

	// �ж�����ĻҶ�ͼ�������α��ɫͼ�Ƿ��С��ͬ����ʽ�Ƿ����Ҫ��
	if (CV_ARE_SIZES_EQ(gray_mat, color_mat) && stype == CV_8UC1 && dtype == CV_8UC3)
	{
		CvMat* red = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);
		CvMat* green = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);
		CvMat* blue = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);
		CvMat* mask = cvCreateMat(gray_mat->rows, gray_mat->cols, CV_8U);

		// �������ɫͨ��������ֵ
		cvSubRS(gray_mat, cvScalar(255), blue);	                    // blue(I) = 255 - gray(I)
		cvCopy(gray_mat, red);			                            // red(I) = gray(I)
		cvCopy(gray_mat, green);			                        // green(I) = gray(I),if gray(I) < 128
		cvCmpS(green, 128, mask, CV_CMP_GE);	                    // green(I) = 255 - gray(I), if gray(I) >= 128
		cvSubRS(green, cvScalar(255), green, mask);
		cvConvertScale(green, green, 2.0, 0.0);

		// �ϳ�α��ɫͼ
		cvMerge(blue, green, red, NULL, color_mat);

		cvReleaseMat(&red);
		cvReleaseMat(&green);
		cvReleaseMat(&blue);
		cvReleaseMat(&mask);
	}

}

/*****************************************************************************************/
/*                                         �������                                    */
/*                                     ���ɼ�����ͼ���ڿؼ���������ʾ                    */
/*****************************************************************************************/
void CBinocularVisionDlg::OpenCamera()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//����������߳�
	ShowMessage("���������");
	b_OpenCamera = false;
	OpenCamera_Thread = AfxBeginThread(OpenCameraThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	//OpenCamera_Thread->ResumeThread();
	
}
	
	


/*****************************************************************************************/
/*                                         �ر������                                    */
/*****************************************************************************************/
void CBinocularVisionDlg::CloseCamera()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ShowMessage("�ر������");
	b_OpenCamera = true;
	//::WaitForSingleObject(OpenCamera_Thread->m_hThread, INFINITE);
	//OpenCamera_Thread = NULL;
}

/*****************************************************************************************/
/*                                        ������߳�                                     */
/*****************************************************************************************/
UINT CBinocularVisionDlg::OpenCameraThread(void *param)
{
	CBinocularVisionDlg* coperTerView = (CBinocularVisionDlg*)param;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int c_iCameraNum = 2;//�����������
	CInstantCameraArray cameras(2);                                         //���������������������
	int exitCode = 0;                                                       //�ж��쳣����
	int iLeftCameraIndex = 0, iRightCameraIndex = 0;
	
	CPylonImage imgLeft, imgRight;                                          //Pylon��ʽ��ͼ��
	CPylonImage pylonImageL, pylonImageR;                                   //����Basler�����ͼƬ��ʽ
	Mat OpencvImage_left, OpencvImage_right;                                //����pylonImageת����Mat��ʽ��ͼ��
	CGrabResultPtr ptrGrabResultL, ptrGrabResultR;                          //����ָ��
	CGrabResultPtr ptrGrabResultL1, ptrGrabResultR1;                        //����ָ��
	CImageFormatConverter formatConverter;                                  //ͼ��ĸ�ʽת��
	CDeviceInfo aDeviceInfos[2];
	string strDeviceSerialNumbers[2];
	DeviceInfoList_t devices;                                               // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
	IplImage *ImageLeft;
	IplImage *ImageRight;
	
	//�ж����õ�ͼ��ֱ���
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
		coperTerView->MessageBox("��Ч�ֱ��ʣ�");
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
		//�ж�������Ƿ�����
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

		//ȷ������������к�
		for (int i = 0; i < c_iCameraNum; i++)
		{
			aDeviceInfos[i] = cameras[i].GetDeviceInfo();
			strDeviceSerialNumbers[i] = aDeviceInfos[i].GetSerialNumber();

			//�涨���к�Ϊ��21770467�������Ϊ������
			if (strDeviceSerialNumbers[i] == "21900819")
			{
				iLeftCameraIndex = i;
			}
			else if (strDeviceSerialNumbers[i] == "21770468")
			{

				iRightCameraIndex = i; 
			}
		}

		

			cameras.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByUser);  //����ץȡͼ��

			
			
			while (cameras.IsGrabbing() && !b_OpenCamera)
			{
				cameras[iLeftCameraIndex].RetrieveResult(5000, ptrGrabResultL, TimeoutHandling_ThrowException);
				cameras[iRightCameraIndex].RetrieveResult(5000, ptrGrabResultR, TimeoutHandling_ThrowException);

				if (ptrGrabResultL->GrabSucceeded() && ptrGrabResultR->GrabSucceeded())
				{
					imgLeft.AttachGrabResultBuffer(ptrGrabResultL);
					imgRight.AttachGrabResultBuffer(ptrGrabResultR);
				}

				


				//BaslerSDKָ��ͼ���ָ��ת����CImageFormatConverter��ʽ��ͼ��

				formatConverter.Convert(pylonImageL, ptrGrabResultL);
				formatConverter.Convert(pylonImageR, ptrGrabResultR);

				// ��pylonImageͼ���ʽת����OpenCV��Mat��ʽ��ͼ��

				OpencvImage_left = Mat(ptrGrabResultL->GetHeight(), ptrGrabResultL->GetWidth(), CV_8UC1, (uint8_t *)pylonImageL.GetBuffer());
				OpencvImage_right = Mat(ptrGrabResultR->GetHeight(), ptrGrabResultR->GetWidth(), CV_8UC1, (uint8_t *)pylonImageR.GetBuffer());


				ImageLeft = &(IplImage)OpencvImage_left;
				ImageRight = &(IplImage)OpencvImage_right;
					
				//��ͼ����С����640*480��
				cvResize(ImageLeft, Normal_ImageLeft, CV_INTER_LINEAR);
				cvResize(ImageRight, Normal_ImageRight, CV_INTER_LINEAR);

				//cvSaveImage("left1.jpg", Normal_ImageLeft);
				//cvSaveImage("right1.jpg", Normal_ImageLeft);

					//���ú�����ͼ����ʾ��Picture�ؼ���
				coperTerView->ShowImage(ImageLeft, IDC_STATIC_PIC_Left);
				coperTerView->ShowImage(ImageRight, IDC_STATIC_PIC_Right);

					//�����ά�ؽ��߳���ϣ���־λb_ImageUsingΪfalse���������ץȡ��ͼ�񴫵ݸ���ά�ؽ��߳�,����������ά�ؽ��Լ�������ʾ
				if (!b_ImageUsing)
				{
					LeftCameraImage = Normal_ImageLeft;                     //��ͼ��
					RightCameraImage = Normal_ImageRight;                   //��ͼ��
					b_ImageUsing = true;
				}

				
				//cvReleaseImage(&ImageLeft);
				//cvReleaseImage(&ImageRight);

				
		}
	}
	catch (const GenericException &e)
	{

		// �쳣����
		cerr << "An exception occurred." << endl << e.GetDescription() << endl;
		exitCode = 1;

	}
	return 1;

}




/*****************************************************************************************/
/*                                         ��ʼ��ά�ؽ�                                  */
/*                       ���ɼ���ͼƬ��������У��������ƥ��͵�����ʾ                    */
/*****************************************************************************************/
void CBinocularVisionDlg::_3DConstruction()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�ж��ؽ�֮ǰ�Ƿ���������Ƿ��������������˳�
	if (b_OpenCamera)
	{
		MessageBox("���ȴ��������");
		ShowMessage("���ȴ��������");
		return;
	}
	UpdateData(TRUE);
	switch (m_StereoMatcging_SGBM)
	{
	case 0:
		ShowMessage("ʹ��SGBM����ƥ���㷨������ά�ؽ���");
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
		MessageBox("����ֻ����SGBM����ƥ���㷨��");
		return;
	}
	switch (m_ShowStylpe)
	{
	case 0:
		b_PseudoColorStyple = true;
		break;
		case 1:
		b_DepthMapStyple=true;
		MessageBox("Ŀǰ�޷���ʾ���ͼ��");
		return;
		case 2:
		b_DesprityMapStyple=true;
		break;
	default:
		MessageBox("��ѡ����ȷ����ʾ��ʽ��");
		return;
	}


	ShowMessage("�ؽ���ʼ��");
	b_3DConstruction = true;
	b_ImageUsing = false;
	_3DConstruction_Thread = AfxBeginThread(_3DConstructionThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	//_3DConstruction_Thread->ResumeThread();
	

}

/*****************************************************************************************/
/*                                        ֹͣ��ά�ؽ�                                   */
/*                               ֹͣ����У��������ƥ��͵�����ʾ                        */
/*****************************************************************************************/
void CBinocularVisionDlg::Stop3DConstruction()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ShowMessage("ֹͣ�ؽ���");
	b_3DConstruction = false;

	//����ʾ�ķ�ʽ����
	b_PseudoColorStyple = false;
	b_DepthMapStyple = false;
	b_DesprityMapStyple = false;
	//���������仰�ò��������Ͻ���ͻ�����
	//::WaitForSingleObject(_3DConstruction_Thread->m_hThread, INFINITE);
	//_3DConstruction_Thread = NULL;
}



//��ά�ؽ��߳�
UINT CBinocularVisionDlg::_3DConstructionThread(void *param)
{
	
	//����һЩ�궨��صĲ���
	CString cstrFilePath = "..\\Config\\";
	CvMat *Intrinsics_Camera_Left = (CvMat*)cvLoad("M1.xml");                         //��������������ڲ�������
	CvMat *Intrinsics_Camera_Right = (CvMat*)cvLoad("M2.xml");                        //��������������ڲ�������
	CvMat *Distortion_Camera_Left = (CvMat*)cvLoad("D1.xml");                         //����������Ļ������
	CvMat *Distortion_Camera_Right = (CvMat*)cvLoad("D2.xml");                        //����������Ļ������
	CvMat *Translation_matlab = (CvMat*)cvLoad("T.xml");                              //�������������ƽ�ƾ���
	CvMat *RotRodrigues_matlab = (CvMat*)cvLoad("R.xml");                             //���������������ת����
	CvMat *Q = cvCreateMat(4, 4, CV_64F);                                             //��ͶӰ����
	CvSize imageSize = cvGetSize(RightCameraImage);                                   //�õ��ɼ�ͼ��Ĵ�С
	                                                                                  //�����Ӳ�ͼת���ɵ���
	string strPointCloudFullPath ="PC.txt";                                           //���ڱ�����Ƶ�txt�ļ�
	CvMat* img1r = cvCreateMat(imageSize.height,imageSize.width, CV_8UC1);            //У��֮�����ͼ��
	CvMat* img2r = cvCreateMat(imageSize.height,imageSize.width, CV_8UC1);            //У��֮�����ͼ��
	CvMat* Disparity2RGB = cvCreateMat(imageSize.height, imageSize.width, CV_8UC3);   //�Ӳ�ͼתα��ɫͼ
	CvMat*Cvdisp8 = cvCreateMat(imageSize.height, imageSize.width, CV_8UC1);                                                                  //��Mat���͵�Disp8ת����CvMat
	CvMat Temp;                                                                       //��Mat���͵�Disp8ת����CvMat���м����  

	CvMat* mx1 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* my1 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* mx2 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* my2 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
	CvMat _R1 = cvMat(3, 3, CV_64F, R1);
	CvMat _R2 = cvMat(3, 3, CV_64F, R2);
	CvMat _P1 = cvMat(3, 4, CV_64F, P1);
	CvMat _P2 = cvMat(3, 4, CV_64F, P2);
	//�жϲ����Ƿ���سɹ�
	if (Intrinsics_Camera_Left == NULL || Intrinsics_Camera_Right == NULL || Distortion_Camera_Left == NULL ||
		Distortion_Camera_Right == NULL || Translation_matlab == NULL || RotRodrigues_matlab == NULL)
	{
		return -1;
	}
	CBinocularVisionDlg* coperTerView = (CBinocularVisionDlg*)param;

	//�ж������е��ӲΧֵ
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
		coperTerView->MessageBox("��Ч�Ӳ�ֵ��");
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
		coperTerView->MessageBox("��ЧSADֵ��");
		break;
	}

	//�ж�����Ҫ�����Ӳ�ͼ�Լ������ļ�,�����Ҫ�򴴽��µ��ļ������ڱ���
	string Disprity_PointCloud_Namefolder1;                                                           //���ڴ�ű����ļ��е�����
	long Disprity_cntImagesNum = 0;                                                                   //���ڼ����Ӳ�ͼ���������
	long PointCloud_cntImagesNum = 0;                                                                 //���ڼ���������ݱ��������
	long Picture_CntImagesNum = 0;                                                                    //����������Ƭ������
	string strFileName_Disprity = "";                                                                 //�Ӳ�ͼ������
	string strFileName_PointCloud = "";                                                               //�������ݵ�����
	if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1 || 
		coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePointCloud) == 1||
		coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1)
	{

		CTime TheTime = CTime::GetCurrentTime();                                                      //��ȡϵͳʱ��
		CString Disprity_PointCloud_Timefolder;                                                       //ͼƬ��ʱ������
		CString Disprity_PointCloud_Namefolder;                                                       //ʵʱץ��ͼƬ�洢���ļ��������Լ�·��
		Disprity_PointCloud_Timefolder.Format("%04d%02d%02d_%02d%02d%02d", TheTime.GetYear(),
			TheTime.GetMonth(), TheTime.GetDay(), TheTime.GetHour(),
			TheTime.GetMinute(), TheTime.GetSecond());                                                //�õ�ϵͳʱ�䲢���
		Disprity_PointCloud_Namefolder = "Disprity_PointCloud_" + Disprity_PointCloud_Timefolder;
	    Disprity_PointCloud_Namefolder1 = CT2A(Disprity_PointCloud_Namefolder.GetBuffer());           //��CStringת����string
		// �ж�·���Ƿ����
		if (!PathIsDirectory(Disprity_PointCloud_Namefolder))
		{
			CString strMsg;
			if (!CreateDirectory(Disprity_PointCloud_Namefolder, NULL))
			{
				strMsg.Format("����·��\"%s\"ʧ�ܣ��Ƿ����?", Disprity_PointCloud_Namefolder);
				if (AfxMessageBox(strMsg, MB_YESNO) == IDYES)
				{
					return 0;
				}
			}

		}

	}

	
	//���￪ʼ����ѭ����У����ƥ�����
	while (b_3DConstruction)
	{

		//�ж�������Ƿ��������û�������˳�
		if (b_OpenCamera)
		{
			coperTerView->MessageBox("������ѹرգ���ά�ؽ�ֹͣ��");
			coperTerView->ShowMessage("������ѹرգ���ά�ؽ�ֹͣ��");
			return 0;
		}
		//�����������У��������ƥ���㷨
		

/********************************����У��****************************************************/
		
		//������������У����Ҫ���м���̾���
		


		//����У���õ�P1��P2
		cvStereoRectify(Intrinsics_Camera_Left, Intrinsics_Camera_Right, Distortion_Camera_Left,
			Distortion_Camera_Right, imageSize, RotRodrigues_matlab, Translation_matlab,&_R1, &_R2, &_P1, &_P2, Q,
			0/*CV_CALIB_ZERO_DISPARITY*/);

		//ʹ������У���õ���P1��P2���õ� ��ͼ���mx1, my1����ͼ���mx2, my2
		cvInitUndistortRectifyMap(Intrinsics_Camera_Left, Distortion_Camera_Left, &_R1, &_P1, mx1, my1);
		cvInitUndistortRectifyMap(Intrinsics_Camera_Right, Distortion_Camera_Right, &_R2, &_P2, mx2, my2);

		//ʹ����ͼ���mx1, my1����ͼ���mx2, my2��У������ͼ��
		if (LeftCameraImage && RightCameraImage)
		{
			cvRemap(LeftCameraImage, img1r, mx1, my1);
			cvRemap(RightCameraImage, img2r, mx2, my2);
		}

		//����У���õ�����ͼ��
		//cvSaveImage("img1r.jpg", img1r);
		//cvSaveImage("img2r.jpg", img2r);
		
/*******************************����ƥ��*****************************************************/
		//����ƥ������ʱ�����ʱ��
		int64 t = getTickCount();
		int Time = 0;
		//��������ƥ�亯��SGBM
		coperTerView->StereoMatch_SGBM((Mat)img1r, (Mat)img2r, coperTerView->SADSizeValue,coperTerView->DisparityValue, coperTerView->disp, coperTerView->disp8);
		
		t = getTickCount() - t;
		Time = t * 1000 / getTickFrequency();
		CString time;
		time.Format("%d", Time);
		coperTerView->ShowMessage("����ƥ��ʱ��:" + time + "ms");
		
		//�ж���ʾ��ʽ
		//��ʾ�Ӳ�ͼ
		IplImage *Idisp8 = &(IplImage)coperTerView->disp8;
		if (coperTerView->b_DesprityMapStyple == true)
		{
			
			coperTerView->ShowImage(Idisp8, IDC_STATIC_PIC_Parallax);
		}
		
		//��ʾα��ɫͼ
		if (coperTerView->b_PseudoColorStyple == true)
		{
			//��Mat���͵�disp8ת����CvMat���͵�
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

		//�ж��Ƿ���Ҫ��������ͼƬ���Ӳ�ͼ�Լ������ļ�
		if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SaveDisprity) == 1 ||
			coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePointCloud) == 1||
			coperTerView->IsDlgButtonChecked(IDC_CHECK_SavePicture) == 1)
		{
			CTime TheTime = CTime::GetCurrentTime();                            //��ȡϵͳʱ��
			CString Namedtime;                                                  //ͼƬ��ʱ������
			Namedtime.Format("%04d%02d%02d_%02d%02d%02d", TheTime.GetYear(),
				TheTime.GetMonth(), TheTime.GetDay(), TheTime.GetHour(),
				TheTime.GetMinute(), TheTime.GetSecond());                     //�õ�ϵͳʱ�䲢���
			string Namedtime1 = CT2A(Namedtime.GetBuffer());                   //��CStringת����string



			//������ͼƬ������������ͼ�񣬸�ʽΪJpg
			if (coperTerView->IsDlgButtonChecked(IDC_CHECK_SaveDisprity) == 1)
			{

				strFileName_Disprity = "Disprity_" + to_string(Disprity_cntImagesNum) + "_" + Namedtime1 + ".bmp";
				string Namefolder2 = Disprity_PointCloud_Namefolder1 + "\\" + strFileName_Disprity;
				const char *Disparity_Name = Namefolder2.c_str();       //�����ָ�벻���Ըı䣬����ָ��ָ���ַ�������ǿ��Ըı��
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
				//������ͼƬ������������ͼ�񣬸�ʽΪJpg
				strFileName_left = "Basler_left_" + to_string(Picture_CntImagesNum) + "_" + Namedtime1 + ".jpg";
				string Namefolder4 = Disprity_PointCloud_Namefolder1 + "\\" + strFileName_left;
				const char *Picture_Name_Left = Namefolder4.c_str();
				cvSaveImage(Picture_Name_Left, LeftCameraImage);
				////�Ҳ����ͼƬ�����������棬��ʽΪJpg

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
	//�ͷ��ڴ���Դ
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
/*                                    ����ƥ���㷨SGBM                                   */
/*                  ������ͼ����ͼ���������ڴ�С���ӲΧ������Ӳ�ͼ               */
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
/*                          ���Ӳ�ͼת������ά��������                                   */
/*                                    �����Ӳ�ͼ                                         */
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
/*                     ���Ӳ�ͼת������ά��������                                        */
/*                     �����Ӳ�ͼ  ���PC.txt�ļ�                                        */
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
/*                          ������ά������ʾ���߳�                                       */
/*                          ��ʾʵʱ���±任�ĵ���                                       */
/*****************************************************************************************/

void CBinocularVisionDlg::BeginShow3DPointCloud()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������


	PointCloudShowRead = false;
	PointCloudShowWrite = false;
	
	PointCloudView_Thread = AfxBeginThread(PointCloudViewThread, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
    //PointCloudView_Thread->ResumeThread();

	
}

void CBinocularVisionDlg::StopShow3DPointCloud()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	PointCloudShowWrite = true;
	PointCloudShowRead = true;
	//::WaitForSingleObject(PointCloudView_Thread->m_hThread, INFINITE);
	//TerminateThread(PointCloudView_Thread->m_hThread, 0);
	//delete PointCloudView_Thread;
	//PointCloudView_Thread = NULL;
}
/*****************************************************************************************/
/*                              ��ά������ʾ���߳�                                       */
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
	//ÿ�ν���glutMainLoop��������ѭ��������������
	glutTimerFunc(100, timer, 0);
	glutMainLoop();
	return 0;

}

void  SetupRC()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1);  //��RGB(0,0,0)����ɫ�������ɫ������
	glColor3f(1.0f, 0.0f, 0.0f);        //��RGB(1,0,0)����ɫ������ͼ��
}

/*****************************************************************************************/
/*                                     �����Ӧ����                                      */
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
/*                                     ������Ӧ����                                      */
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
/*                                 ���Ƹ�ֵ�ػ溯��                                      */
/*****************************************************************************************/
void RenderScene()
{
	
	   
		// clear screen and depth buffer  
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glClearColor(1.0, 1.0, 0.6, 1.0);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);                            //��ͶӰ�������
		// Reset the coordinate system before modifying   
		glLoadIdentity();                                       //���óɵ�λ����
		
		// set the camera position  
		atx = 0.0f;
		aty = 0.0f;
		atz = 100;
		eyex = atx + radius * sin(CV_PI * ry / 180.0f) * cos(CV_PI * rx / 180.0f);
		eyey = aty + radius * cos(CV_PI * ry / 180.0f);
		eyez = atz + radius * sin(CV_PI * ry / 180.0f) * sin(CV_PI * rx / 180.0f);

		glOrtho(-50, 50, -50, 50, 20, 150);
		gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0.0, 1.0, 0.0);

		glPointSize(1.0f);//ָ����Ĵ�С��1�����ص�λ
		glColor3f(1.0f, 0.0f, 0.0f);                     //ָ�������ɫ
		//glColor3f(0.0f, 1.0f, 0.0f);                     //ָ�������ɫ
		glPointSize(1.0f);                               //ָ����Ĵ�С��1�����ص�λ
		//glVertex3f(0, 0, 0);                           // ������Ϊ(0,0,0)�ĵط�������һ����
		glBegin(GL_POINTS);                             //��ʼ����
		
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
					glVertex3f(point[0], point[1], point[2]); // ������Ϊ(0,0,0)�ĵط�������һ����
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
/*                   ��ʱ�����������ϵ��ø��� RenderScene()����                          */
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
/*                                        �˳�����                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::OnClickedButtonExit()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (MessageBox("ȷ���˳�", "", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}
	CDialog::OnOK();
}

/*****************************************************************************************/
/*                                        ���ú���                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::Setting()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BinocularVisionDlg1 dlg1;
	dlg1.DoModal();
	return;

}


/*****************************************************************************************/
/*                                    ����궨����                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::StereoCalibration_GrapPicture()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	//�����û�ѡ��
	if (i_GrapStereoCalibrationPicture == 0)
	{

		UpdateData(TRUE);
		//�ж�
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
			MessageBox("��ѡ����ȷ�ı궨ͼ��ֱ��ʣ�");
			return;
		} 
		i_GrapStereoCalibrationPicture++;           //�����ж��ǿ������������ץ��ͼ�����ڱ궨         
		b_StereoCalibrationOpenCameraed = true;     //���ڽ�������궨�̵߳ı����߳�
		
		//��������߳�
		Calibration_OpenCamera_Thread = AfxBeginThread(CalibrationOpenCameraThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);         

	}
	else
	{
		b_StereoCalibrationGrapPicture = true;
		i_GrapStereoCalibrationPicture++;
	}

}

/*****************************************************************************************/
/*                                    �������궨                                       */
/*****************************************************************************************/
void CBinocularVisionDlg::AccomplishCalibration_GrapPicture()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	
	b_StereoCalibrationOpenCameraed = false;                          //�ر������
	

	//���̸�ͼ��Ĵ�С�趨����
	StereoCalibration_CheckboardWidthNum = 0;                           
	StereoCalibration_CheckboardHighNum = 0;
	
	//���̸���ߵĸ�������
	StereoCalibration_CheckboardWidthNum =0;
	StereoCalibration_CheckboardHighNum = 0;
	
	//���̸��С����
	CheckboardSize = 0;
}


/*****************************************************************************************/
/*                               ����궨ץ��ͼ����߳�                                  */
/*****************************************************************************************/
UINT CBinocularVisionDlg::CalibrationOpenCameraThread(void *param)
{
	CBinocularVisionDlg* StereoCalibration = (CBinocularVisionDlg*)param;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int i_StereoCalibrationCameraNum = 2;                                             //����궨ʱ�����������
	CInstantCameraArray StereoCalibrationCameras(2);                                  //���������������������
	int exitCode = 0;                                                                 //�ж��쳣����
	int i_StereoCalibrationLeftCameraIndex = 0, i_StereoCalibrationRightCameraIndex = 0;

	CPylonImage StereoCalibrationImgLeft, StereoCalibrationImgRight;                  //Pylon��ʽ��ͼ��
	CPylonImage StereoCalibrationPylonImageL, StereoCalibrationPylonImageR;           //����Basler�����ͼƬ��ʽ
	Mat StereoCalibration_OpencvImage_left, StereoCalibration_OpencvImage_right;      //����pylonImageת����Mat��ʽ��ͼ��
	CGrabResultPtr StereoCalibration_ptrGrabResultL, StereoCalibration_ptrGrabResultR;//����ָ��                 
	CImageFormatConverter StereoCalibration_formatConverter;                          //ͼ��ĸ�ʽת��
	CDeviceInfo StereoCalibration_aDeviceInfos[2];
	string StereoCalibration_strDeviceSerialNumbers[2];
	DeviceInfoList_t StereoCalibration_devices;                                       // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
	IplImage *StereoCalibration_ImageLeft;
	IplImage *StereoCalibration_ImageRight;




	//�����洢ץ����Ƭ���ļ���
	CTime TheTime = CTime::GetCurrentTime();                                           //��ȡϵͳʱ��
	CString GrapPicture_Timefolder;                                                    //ͼƬ��ʱ������
	CString  GrapPicture_Namefolder;                                                   //ʵʱץ��ͼƬ�洢���ļ��������Լ�·��
	string  GrapPicture_Namefolder1;
	GrapPicture_Timefolder.Format("%04d%02d%02d_%02d%02d%02d", TheTime.GetYear(),
			TheTime.GetMonth(), TheTime.GetDay(), TheTime.GetHour(),
			TheTime.GetMinute(), TheTime.GetSecond());                                 //�õ�ϵͳʱ�䲢���
	GrapPicture_Namefolder = "GrapPicture_" + GrapPicture_Timefolder;
	GrapPicture_Namefolder1 = CT2A(GrapPicture_Namefolder.GetBuffer());      //��CStringת����string
	// �ж�·���Ƿ����
	if (!PathIsDirectory(GrapPicture_Namefolder))
	{
		CString strMsg;
		if (!CreateDirectory(GrapPicture_Namefolder, NULL))
		{
			strMsg.Format("����·��\"%s\"ʧ�ܣ��Ƿ����?", GrapPicture_Namefolder);
			if (AfxMessageBox(strMsg, MB_YESNO) == IDYES)
			{
				return 0;
			}
		}

	}

	

	//�ж����õ�ͼ��ֱ���
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
		//�ж�������Ƿ�����
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

		//ȷ������������к�
		for (int i = 0; i < i_StereoCalibrationCameraNum ; i++)
		{
			StereoCalibration_aDeviceInfos[i] = StereoCalibrationCameras[i].GetDeviceInfo();
			StereoCalibration_strDeviceSerialNumbers[i] = StereoCalibration_aDeviceInfos[i].GetSerialNumber();

			//�涨���к�Ϊ��21770467�������Ϊ������
			if (StereoCalibration_strDeviceSerialNumbers[i] == "21900819")
			{
				i_StereoCalibrationLeftCameraIndex = i;
			}
			else if (StereoCalibration_strDeviceSerialNumbers[i] == "21770468")
			{

				i_StereoCalibrationRightCameraIndex = i;
			}
		}



		StereoCalibrationCameras.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByUser);  //����ץȡͼ��



		while (StereoCalibrationCameras.IsGrabbing() && b_StereoCalibrationOpenCameraed)
		{
			StereoCalibrationCameras[i_StereoCalibrationLeftCameraIndex].RetrieveResult(5000, StereoCalibration_ptrGrabResultL, TimeoutHandling_ThrowException);
			StereoCalibrationCameras[i_StereoCalibrationRightCameraIndex].RetrieveResult(5000, StereoCalibration_ptrGrabResultR, TimeoutHandling_ThrowException);

			if (StereoCalibration_ptrGrabResultL->GrabSucceeded() && StereoCalibration_ptrGrabResultR->GrabSucceeded())
			{
				StereoCalibrationImgLeft.AttachGrabResultBuffer(StereoCalibration_ptrGrabResultL);
				StereoCalibrationImgRight.AttachGrabResultBuffer(StereoCalibration_ptrGrabResultR);
			}




			//BaslerSDKָ��ͼ���ָ��ת����CImageFormatConverter��ʽ��ͼ��

			StereoCalibration_formatConverter.Convert(StereoCalibrationPylonImageL, StereoCalibration_ptrGrabResultL);
			StereoCalibration_formatConverter.Convert(StereoCalibrationPylonImageR, StereoCalibration_ptrGrabResultR);

			// ��pylonImageͼ���ʽת����OpenCV��Mat��ʽ��ͼ��

			StereoCalibration_OpencvImage_left = Mat(StereoCalibration_ptrGrabResultL->GetHeight(), StereoCalibration_ptrGrabResultL->GetWidth(), CV_8UC1,
				                                      (uint8_t *)StereoCalibrationPylonImageL.GetBuffer());
			StereoCalibration_OpencvImage_right = Mat(StereoCalibration_ptrGrabResultR->GetHeight(), StereoCalibration_ptrGrabResultR->GetWidth(), CV_8UC1, 
				                                      (uint8_t *)StereoCalibrationPylonImageR.GetBuffer());


			StereoCalibration_ImageLeft = &(IplImage)StereoCalibration_OpencvImage_left;
			StereoCalibration_ImageRight = &(IplImage)StereoCalibration_OpencvImage_right;

			//��ͼ����С��()
			cvResize(StereoCalibration_ImageLeft, Normal_ImageLeft, CV_INTER_LINEAR);
			cvResize(StereoCalibration_ImageRight, Normal_ImageRight, CV_INTER_LINEAR);

			//cvSaveImage("left1.jpg", Normal_ImageLeft);
			//cvSaveImage("right1.jpg", Normal_ImageLeft);

			//���ú�����ͼ����ʾ��Picture�ؼ���
			StereoCalibration->ShowImage(StereoCalibration_ImageLeft, IDC_STATIC_PIC_Left);
			StereoCalibration->ShowImage(StereoCalibration_ImageRight, IDC_STATIC_PIC_Right);

			//�ж�Ҫ��Ҫץ��
			if (b_StereoCalibrationGrapPicture)
			{
				string LeftName = "left" + to_string(i_GrapStereoCalibrationPicture-1) + ".jpg";      //����ͼ����
				string RightName= "right" + to_string(i_GrapStereoCalibrationPicture-1) + ".jpg";     //����ͼ����
				string LeftName1 = GrapPicture_Namefolder1 + "\\" + LeftName;
				string RightName1 = GrapPicture_Namefolder1 + "\\" + RightName;
				cvSaveImage(LeftName1.c_str(), Normal_ImageLeft);                         //������ͼ��
				cvSaveImage(RightName1.c_str(), Normal_ImageRight);                       //������ͼ��
				StereoCalibration->ShowImage(StereoCalibration_ImageLeft, IDC_STATIC_PIC_Left);
				StereoCalibration->ShowImage(StereoCalibration_ImageRight, IDC_STATIC_PIC_Right);
				Sleep(1000);
				b_StereoCalibrationGrapPicture = false;

				
			}

		}
	}
	catch (const GenericException &e)
	{

		// �쳣����
		cerr << "An exception occurred." << endl << e.GetDescription() << endl;
		exitCode = 1;

	}
	return 1;

}


/*****************************************************************************************/
/*                                    ����궨���߳�                                     */
/*****************************************************************************************/
void CBinocularVisionDlg::BeginStereoCalibretion()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	

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
		MessageBox("��ѡ����ȷ�����̸�������");
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
		MessageBox("��ѡ����ȷ�����̸��С��");
		return;
	}




	CString StereoCalibretion_strFilePathTxt1;                            //���ڼ���txt�����ļ��ĵ�ַ
	vector<CString> vecResult;                                          //���ڴ洢��ȡtxt����������                                                                     
	int displayCorners = 1;
	int showUndistorted = 1;
	bool isVerticalStereo = false;//OpenCV can handle left-right
	//or up-down camera arrangements
	const int maxScale = 1;
	const float squareSize = CheckboardSize; //Set this to your actual square size
	int i, j, lr, nframes;
	int n = StereoCalibration_CheckboardWidthNum*StereoCalibration_CheckboardHighNum;
	int N = 0;
	vector<string> imageNames[2];                      //��������ͼƬ��·����0Ϊ��1Ϊ��
	vector<CvPoint3D32f> objectPoints;                 //����nframes���ǵ����ꣻ
	vector<CvPoint2D32f> points[2];                    //��������ͼƬ���ܽǵ����ꣻ
	vector<int> npoints;                               //�ǵ������Ŀ��
	vector<uchar> active[2];                           //�������ҽǵ������ķ���ֵ
	vector<CvPoint2D32f> temp(n);                      //����ÿ�����̽ǵ�����ꣻ
	CvSize imageSize = { 0, 0 };


	double M1[3][3], M2[3][3], D1[5], D2[5];
	double R[3][3], T[3], E[3][3], F[3][3];
	CvMat C_M1 = cvMat(3, 3, CV_64F, M1);               //��������ڲ�������
	CvMat C_M2 = cvMat(3, 3, CV_64F, M2);               //��������ڲ�������
	CvMat C_D1 = cvMat(1, 5, CV_64F, D1);               //��������������
	CvMat C_D2 = cvMat(1, 5, CV_64F, D2);               //��������������
	CvMat C_R = cvMat(3, 3, CV_64F, R);                 //��ת����
	CvMat C_T = cvMat(3, 1, CV_64F, T);                 //ƽ�ƾ���
	CvMat C_E = cvMat(3, 3, CV_64F, E);                 //��������
	CvMat C_F = cvMat(3, 3, CV_64F, F);                 //��������

	//��ʼ��ȡ������յ��ļ���
	//����ͼƬ��·��
	

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files|*.*|txt|*.txt||");
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return;
	}

	StereoCalibretion_strFilePathTxt1 = dlg.GetPathName();


	if (!PathFileExists(StereoCalibretion_strFilePathTxt1))
	{
		MessageBox("�ļ������ڣ�");
		return;
	}
	

	//��ȡ���ļ�������
	if (!PathFileExists(StereoCalibretion_strFilePathTxt1))
	{
		return;
	}
	CString cstrFilePath = "";
	char fatherPath[256] = { 0 };
	strcpy(fatherPath, StereoCalibretion_strFilePathTxt1);
	*(strrchr(fatherPath, '\\')) = '\0';                                            //ɾ���ļ�����ֻ���¼��ص�Ŀ¼
	cstrFilePath = fatherPath;                                                      //���Ǹ�Ŀ¼
	string felod = cstrFilePath.GetBuffer();                                       //��һ��Ŀ¼��string
	string felod1 = felod+"\\";
	CString strValue = _T("");





	CStdioFile file;
	//�ж��Ƿ���ȷ��txt�ļ�
	if (!file.Open(StereoCalibretion_strFilePathTxt1, CFile::modeRead))
	{
		return;
	}

	
	while (file.ReadString(strValue))
	{
		vecResult.push_back(strValue);
	}
	//�ر�txt�ļ�
	file.Close();
	ShowMessage("����궨��ʼ��");



	for (i = 0; i<vecResult.size(); i++)
	{
		
		int count = 0, result = 0;
		lr = i % 2;
		vector<CvPoint2D32f>& pts = points[lr];

		string bif1 = vecResult[i].GetBuffer();
		string ImageName = felod1 +  bif1;
		IplImage* img = cvLoadImage(ImageName.c_str(), 0);          //����ͼ��

		if (!img)                                     //�ж�ͼ���Ƿ�Ϊ��
		{
			break;
		}

		imageSize = cvGetSize(img);                  //�õ�ͼ��Ĵ�С
		imageNames[lr].push_back(bif1.c_str());      //��ͼƬ��·������imageNames��


		for (int s = 1; s <= maxScale; s++)
		{
			IplImage* timg = img;
			if (s > 1)
			{
				timg = cvCreateImage(cvSize(img->width*s, img->height*s), img->depth, img->nChannels); //�õ�imgͼ����ͬ�Ĵ�С����Ⱥ�ͨ��
				cvResize(img, timg, CV_INTER_CUBIC);                                                  //�õ���ͬ�Ĵ�С
			}

			//Ѱ�ҽǵ�����
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
				break;                 //��ʾѰ�ҽǵ�ʧ��
			}
		}

		
		
		cvDrawChessboardCorners(img, cvSize(StereoCalibration_CheckboardWidthNum, StereoCalibration_CheckboardHighNum), &temp[0],count, result);
		ShowImage(img, IDC_STATIC_PIC_Left);
		ShowMessage(bif1.c_str());      //���ͼ���·��
		Sleep(2000);
		

			
		N = pts.size();
		pts.resize(N + n, cvPoint2D32f(0, 0));
		active[lr].push_back((uchar)result);
		//assert( result != 0 );
		if (result)
		{
			//��ʼѰ�������ؽǵ�����
			cvFindCornerSubPix(img, &temp[0], count, cvSize(11, 11), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));
			copy(temp.begin(), temp.end(), pts.begin() + N);            //û����ʲô��˼��Ӧ���ǰ�temp�Ľǵ���߷ŵ�point��ȥ�ˣ�
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
	CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0]);       //��ߵ�0��ͼ��Ľǵ�����
	CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0]);       //�ұߵ�0��ͼ��Ľǵ�����
	CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0]);
	cvSetIdentity(&C_M1);                                              //���к�����ȵ�����Ϊ1
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
	

	ShowMessage("����궨������");
	return;

}
