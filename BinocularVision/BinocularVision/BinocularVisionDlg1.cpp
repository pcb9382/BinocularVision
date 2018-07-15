// BinocularVisionDlg1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BinocularVision.h"
#include "BinocularVisionDlg1.h"
#include "afxdialogex.h"
#include "resource.h"
#include "BinocularVisionDlg.h"
// BinocularVisionDlg1 �Ի���

IMPLEMENT_DYNAMIC(BinocularVisionDlg1, CDialogEx)

BinocularVisionDlg1::BinocularVisionDlg1(CWnd* pParent /*=NULL*/)
	: CDialogEx(BinocularVisionDlg1::IDD, pParent)
	, m_Resolutiuon(0)                                             //Ĭ��Ϊ480*320����
	, m_DisparityWindow(0)                                         //Ĭ�ϵ��ӲΧΪ128pixel
	, m_SADWindowSize(0)                                           //Ĭ�ϵĴ��ڴ�СΪ15pixel
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


// BinocularVisionDlg1 ��Ϣ�������


//�Ӵ��ڵĳ�ʼ������
BOOL BinocularVisionDlg1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	CBinocularVisionDlg *pFatherDlg;                                      //����һ�����Ի������
	pFatherDlg = (CBinocularVisionDlg*)GetParent();                       //����һ�����Ի����ָ�룬���ں͸��Ի�����в�������
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE

}


void BinocularVisionDlg1::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	//�ֱ��ʵ�ѡ��
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
		MessageBox("��ѡ����Ӧ��ͼ��ֱ��ʣ�");
		break;
	}
	
	//�Ӳ�ڵ�ѡ��
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
		MessageBox("��ѡ����Ӧ���Ӳ�ڣ�");
		break;
	}

	//SAD���ڴ�С��ѡ��
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
		MessageBox("��ѡ����Ӧ��SAD���ڣ�");
		break;
	}

	//���Ӵ��ڵ�ֵ������������
	CBinocularVisionDlg *pFatherDlg = (CBinocularVisionDlg *)GetParent();
	pFatherDlg->ResolutiuonValue = ResolutiuonValue1;
	pFatherDlg->DisparityWindowValue = DisparityWindowValue1;
	pFatherDlg->SADWindowSizeValue = SADWindowSizeValue1;
	CDialogEx::OnOK();
}
