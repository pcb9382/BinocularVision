
// BinocularVision.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBinocularVisionApp: 
// �йش����ʵ�֣������ BinocularVision.cpp
//

class CBinocularVisionApp : public CWinApp
{
public:
	CBinocularVisionApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBinocularVisionApp theApp;