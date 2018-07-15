
// BinocularVisionDlg.h : ͷ�ļ�
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

//Basler�����Ҫ��ͷ�ļ�
#include <pylon/PylonIncludes.h>
#include "conio.h"//Ϊ��ʹ��_getchar()����ʵ�ֲ�����س�ֱ�ӻ�ȡ�ڿ���̨������ַ�
#include <windows.h>
#include <process.h>

#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <pcl/visualization/cloud_viewer.h>                       //��CloudViewerͷ�ļ�����
#include <pcl/io/io.h>                                            //I/O���ͷ�ļ�
#include <pcl/io/pcd_io.h>                                        //PCD�ļ���ȡͷ�ļ�����
#include <pcl/console/parse.h>
#include <GL/glut.h>
#include "afxwin.h"

using namespace std;
using namespace cv;
using namespace Pylon;

// CBinocularVisionDlg �Ի���
class CBinocularVisionDlg : public CDialogEx
{
// ����
public:
	CBinocularVisionDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BINOCULARVISION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();

	//��������߳�
	CWinThread*    OpenCamera_Thread;                                       //�����߳�
	static UINT OpenCameraThread(void *param);                              //�����̴߳�������̺߳���

	//��ά�ؽ��̣߳���������У������ά�ؽ����Ӳ�ͼ��ʾ�Լ������Ż�����ʾ��
	CWinThread*    _3DConstruction_Thread;                                  //�����߳�
	static UINT   _3DConstructionThread(void *param);                       //������ά�ؽ��̺߳���

	//������ʾ�̣߳�ʹ��OpenGL��ʾ����
	CWinThread*    PointCloudView_Thread;                                  //�����߳�
	static UINT   PointCloudViewThread(void *param);                       //�������ƿ��ӻ��̺߳���

	//�궨��������̣߳�ץȡͼƬ���ڱ궨
	CWinThread*    Calibration_OpenCamera_Thread;                          //�����߳�
	static UINT    CalibrationOpenCameraThread(void *param);               //�����̴߳�������̺߳���

public:


	//һЩ������һЩ�ռ�
	void ShowImage(IplImage*img, UINT ID);                                  //��ʾͼ��ĺ���
	CListBox m_list;
	void ShowMessage(const CString);                                        //�б����ʾ��Ϣ����
	int StereoMatch_SGBM(Mat &img1, Mat &img2, int SADWindowSize, int numberOfDisparities, Mat & disp, Mat & disp8);//����ƥ�亯��
	void DispToXYZ(const Mat& mat);
	void saveXYZ(const char* filename, const Mat& mat);
	void Disprity2Color(CvMat* gray_mat, CvMat* color_mat);                  //���Ӳ�ͼת����α��ɫͼ
	int Normal_ImageWidth;                                                   //ͼ��ֱ��� ��
	int Normal_ImageHigh;                                                    //ͼ��ֱ��� ��
	int DisparityValue;                                                      //����ƥ��ʱ���Ӳ�Ѱ�ҷ�Χ
	int SADSizeValue;                                                        //����ƥ��ʱSAD���ڵĴ�С
	int m_StereoMatcging_SGBM;                                               //ѡ��ͬ��ƥ���㷨
	int m_ShowStylpe;                                                        //��ʾ��ʽ
	bool b_PseudoColorStyple;                                                //α��ɫͼ��ʾ��ʽ
	bool b_DepthMapStyple;                                                   //���ͼ��ʾ��ʽ
	bool b_DesprityMapStyple;                                                //�Ӳ�ͼ��ʾ��ʽ
	int m_CalibrationResolution;                                             //�궨�ֱ���
	int m_InnerAnglePoint;                                                   //�ڽǵ���
	int m_CheckboardSize;                                                    //���̸��С
	int StereoCalibration_Normal_ImageWidth;                                 //�궨ʱ�趨��ͼ��ֱ��� ��
	int StereoCalibration_Normal_ImageHigh;                                  //�궨ʱ�趨��ͼ��ֱ��� ��
	int StereoCalibration_CheckboardWidthNum;                                //���̸��ȷ����ϵĽǵ���
	int StereoCalibration_CheckboardHighNum;                                 //���̸�߶ȷ����ϵĽǵ���
	int CheckboardSize;                                                      //���̸��С.



	//��ť���ɵĺ�������
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

	//һЩ����
	Mat disp, disp8;                                                          //�Ӳ�ͼ 
    CvMat *Q = cvCreateMat(4, 4, CV_64F);                                     //��ͶӰ����
	
	//���ڵõ��Ӵ��ڴ��ݹ�����ֵ
	int ResolutiuonValue;
	int DisparityWindowValue;
	int SADWindowSizeValue;

	
	
	
	
	
	afx_msg void BeginStereoCalibretion();
};






