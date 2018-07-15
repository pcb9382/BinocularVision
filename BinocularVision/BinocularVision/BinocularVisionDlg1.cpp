// BinocularVisionDlg1.cpp : 实现文件
//

#include "stdafx.h"
#include "BinocularVision.h"
#include "BinocularVisionDlg1.h"
#include "afxdialogex.h"
#include "resource.h"
#include "BinocularVisionDlg.h"
// BinocularVisionDlg1 对话框

IMPLEMENT_DYNAMIC(BinocularVisionDlg1, CDialogEx)

BinocularVisionDlg1::BinocularVisionDlg1(CWnd* pParent /*=NULL*/)
	: CDialogEx(BinocularVisionDlg1::IDD, pParent)
	, m_Resolutiuon(0)                                             //默认为480*320像素
	, m_DisparityWindow(0)                                         //默认的视差范围为128pixel
	, m_SADWindowSize(0)                                           //默认的窗口大小为15pixel
{

}

BinocularVisionDlg1::~BinocularVisionDlg1()
{
}

void BinocularVisionDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_Resolutiuon);
	DDX_Radio(pDX, IDC_RADIO5, m_DisparityWindow);
	DDX_Radio(pDX, IDC_RADIO8, m_SADWindowSize);
}


BEGIN_MESSAGE_MAP(BinocularVisionDlg1, CDialogEx)
	ON_BN_CLICKED(IDOK, &BinocularVisionDlg1::OnBnClickedOk)
END_MESSAGE_MAP()


// BinocularVisionDlg1 消息处理程序


//子窗口的初始化函数
BOOL BinocularVisionDlg1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	CBinocularVisionDlg *pFatherDlg;                                      //创建一个父对话框的类
	pFatherDlg = (CBinocularVisionDlg*)GetParent();                       //创建一个父对话框的指针，用于和父对话框进行参数传递
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE

}


void BinocularVisionDlg1::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	//分辨率的选择
	switch (m_Resolutiuon)
	{
	case 0:
		ResolutiuonValue1 = 0;
		break;
	case 1:
		ResolutiuonValue1 = 1;
		break;
	case 2:
		ResolutiuonValue1 = 2;
		break;
	case 3:
		ResolutiuonValue1 = 3;
		break;
	default:
		MessageBox("请选择相应的图像分辨率！");
		break;
	}
	
	//视差窗口的选择
	switch (m_DisparityWindow)
	{
	case 0:
		DisparityWindowValue1 = 0;
		break;
	case 1:
		DisparityWindowValue1 = 1;
		break;
	case 2:
		DisparityWindowValue1 = 2;
		break;
	default:
		MessageBox("请选择相应的视差窗口！");
		break;
	}

	//SAD窗口大小的选择
	switch (m_SADWindowSize)
	{
	case 0:
		SADWindowSizeValue1 = 0;
		break;
	case 1:
		SADWindowSizeValue1 = 1;
		break;
	case 2:
		SADWindowSizeValue1 = 2;
		break;
	case 3:
		SADWindowSizeValue1 = 3;
		break;
	default:
		MessageBox("请选择相应的SAD窗口！");
		break;
	}

	//把子窗口的值传到父窗口中
	CBinocularVisionDlg *pFatherDlg = (CBinocularVisionDlg *)GetParent();
	pFatherDlg->ResolutiuonValue = ResolutiuonValue1;
	pFatherDlg->DisparityWindowValue = DisparityWindowValue1;
	pFatherDlg->SADWindowSizeValue = SADWindowSizeValue1;
	CDialogEx::OnOK();
}
