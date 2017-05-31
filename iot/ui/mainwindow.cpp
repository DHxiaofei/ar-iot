#include "mainwindow.h"
#include "sqlite3.h"

mainwindow::mainwindow(QWidget *parent)
    : QWidget(parent),
	  m_iot(0)
{
	ui.setupUi(this);
	this->setGeometry(QRect(0,0,g_ScreenSize.width(),g_ScreenSize.height()));
	this->setMouseTracking(true);
	sqlite3_initialize();


	/*初始化GLOBAL*/
	g_qrcode = new QrCode();
	g_database = new Database();
	g_Statusbar = new statusbar(this);
	g_Statusbar->show();

	//video_show *m = new video_show(this);
	//m->show();

    iot *m_iot = new iot(this);
    m_iot->show();

//    expertinfo_window *m_expert = new expertinfo_window(this);
//    m_expert->show();
}

mainwindow::~mainwindow()
{

	if(g_Statusbar != 0)
	{
		delete g_Statusbar;
		g_Statusbar = 0;
	}

	if(g_qrcode != 0)
	{
		delete g_qrcode;
		g_qrcode = 0;
	}

	if(m_iot != 0)
	{
		delete m_iot;
		m_iot = 0;
	}
}
