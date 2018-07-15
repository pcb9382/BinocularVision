#pragma once
#include "resource.h"
#include "BinocularVisionDlg.h"

// BinocularVisionDlg1 对话框

class BinocularVisionDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(BinocularVisionDlg1)

public:
	BinocularVisionDlg1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~BinocularVisionDlg1();

// 对话框数据
	enum { IDD = IDD_BINOCULARVISION_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
