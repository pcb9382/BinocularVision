#pragma once
#include "resource.h"
#include "BinocularVisionDlg.h"

// BinocularVisionDlg1 �Ի���

class BinocularVisionDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(BinocularVisionDlg1)

public:
	BinocularVisionDlg1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~BinocularVisionDlg1();

// �Ի�������
	enum { IDD = IDD_BINOCULARVISION_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int m_Resolutiuon;
	int m_DisparityWindow;
	int m_SADWindowSize;
	afx_msg void OnBnClickedOk();

	int ResolutiuonValue1;
	int DisparityWindowValue1;
	int SADWindowSizeValue1;

};
