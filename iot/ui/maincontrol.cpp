#include "maincontrol.h"


static int int_temp;
static int int_humid;
//static int parase_data(uint8_t *_protocol, int _len);
maincontrol::maincontrol(QWidget *parent)
    : QWidget(parent),
	  m_ScrollArea(0),
	  m_ScrollWidget(0),
	  m_rcMain(g_RcMain),
	  m_rcTab(QRect(0,0,parent->width(),parent->height()*11/12)),
	  m_rcSan(QRect(0,0,parent->width(),parent->height()*10/12)),
	  m_rcSaWidgetn(m_rcSan),
	  m_rcBt(QRect(parent->width()*4/5,parent->height()*11/12,parent->width()/6,parent->height()/12))
{
	ui.setupUi(this);

	this->setGeometry(m_rcMain);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint);

	// TODO Auto-generated constructor stub
	this->setMouseTracking(true);

	ui.tabWidget->setGeometry(m_rcTab);
	QString strTabStyle;
	strTabStyle.sprintf("QTabBar::tab { height: %dpx; width:%dpx; padding: 0px;}",parent->height()/12,parent->width()/10);
	//strTabStyle +=	"QTabBar::tab:selected { background: lightgray; } ";
	ui.tabWidget->setStyleSheet(strTabStyle);
	m_ScrollArea = (new QScrollArea(ui.tab));
	m_ScrollWidget = (new QWidget(m_ScrollArea));
	m_ScrollArea->setGeometry(m_rcSan);
	m_ScrollArea->setPalette(g_Palette);
	m_ScrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	m_ScrollArea->setWidgetResizable(true);
	m_ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_ScrollWidget->setGeometry(m_rcSaWidgetn);
	m_ScrollArea->setWidget(m_ScrollWidget);
	m_ScrollArea->show();

	ui.pushButton->setGeometry(m_rcBt);

	fm1 = new QFrame(m_ScrollWidget);
	fm1->setObjectName(QString::fromUtf8("fm1"));
	fm1->setGeometry(QRect(0, 0, parent->width(), parent->height()*10/12));
	fm1->setFrameShape(QFrame::StyledPanel);
	fm1->setFrameShadow(QFrame::Raised);


	mainLayout1=new QVBoxLayout();
	mainLayout1->setSpacing(0);
	mainLayout1->setContentsMargins(0,0,0,0);
	layout1=new QHBoxLayout();
	layout1->setSpacing(0);
	layout1->setContentsMargins(0,0,0,0);

	tabchart1 = new mctrtabchart1(fm1);
	tabchart1->setTitle(tr("温度"));
	tabchart1->setLine(8);
	tabchart1->setMax(50);
	mainLayout1->addWidget(tabchart1);

	QLabel *label1 = new QLabel(fm1);
	label1->setGeometry(QRect(0, tabchart1->height(), parent->width(), parent->height()/12));
	mainLayout1->addWidget(label1);

	tabchart2 = new mctrtabchart1(fm1);
	tabchart2->setTitle(tr("湿度"));
	tabchart2->setLine(10);
	tabchart2->setMax(100);
	mainLayout1->addWidget(tabchart2);

	fm1->setLayout(mainLayout1);

//	m_temperature.clear();
//	m_humidity.clear();

    updateTimer1=new QTimer(this);
    updateTimer1->setInterval(200);
    updateTimer2=new QTimer(this);
    updateTimer2->setInterval(300);

//    list = new protocol_list_t();
//    protocol_list_init(list, m_buf, sizeof(m_buf));

    connect(updateTimer1,SIGNAL(timeout()),this,SLOT(get_realdata()));
    connect(updateTimer2,SIGNAL(timeout()),this,SLOT(UpdateData()));
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(btnClick()));

	m_getdata = new getdata();

//	m_comcontrol = new comcontrol("/dev/ttySAC1", 115200);
//	int ret = m_comcontrol->Com_Init();
//	if(ret < 0)
//	{
//		printf("init error\n");
//		return ret;
//	}
//	else
//	{
//		connect(m_comcontrol,SIGNAL(OnComRead()),m_datahandle,SLOT(OnComRead()));
//	}

	updateTimer1->start();
	updateTimer2->start();
}

maincontrol::~maincontrol()
{
	for(int i=0;i<m_vlabeln.size();i++)
	{
		delete m_vlabeln[i];
		m_vlabeln[i] = 0;
	}

	for(int i=0;i<m_vlabelh.size();i++)
	{
		delete m_vlabelh[i];
		m_vlabelh[i] = 0;
	}

	if(m_ScrollWidget != 0)
	{
		delete m_ScrollWidget;
		m_ScrollWidget = 0;
	}

	if(m_ScrollArea != 0)
	{
		delete m_ScrollArea;
		m_ScrollArea = 0;
	}

	if(list != 0)
	{
		delete list;
		list = 0;
	}

	if(m_comcontrol != 0)
	{
		delete m_comcontrol;
		m_comcontrol = 0;
	}
}


int maincontrol::addlabel(int i,QString str)
{
	int iSize =0;
	int iWidth = m_rcMain.width()/5;
	int iHeight = m_rcMain.height()/12;

	QLabel *label =new QLabel(m_ScrollWidget);
	label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	label->setText(str);

	//qDebug()<<"addlabel i="<<i<<"  str:"<<str;
	if(i == 0)
	{
		iSize = m_vlabeln.size();
		QRect rcLb(0,iSize*iHeight,iWidth,iHeight);
		label->setGeometry(rcLb);
		if(iSize >= 10)
		{
			m_ScrollWidget->setMinimumHeight((iSize+1)*iHeight);
		}
		label->show();
		m_vlabeln.push_back(label);
		m_vRcn.push_back(rcLb);
		//qDebug()<<"m_vlabeln.push_back"<<iSize<<"  str:"<<str;
	}
	else if(i == 1)
	{
		iSize = m_vlabelh.size();
		QRect rcLb(0,iSize*iHeight,iWidth,iHeight);
		label->setGeometry(rcLb);
		if(iSize >= 10)
		{
			m_ScrollWidget->setMinimumHeight((iSize+1)*iHeight);
		}
		m_vlabelh.push_back(label);
		m_vRch.push_back(rcLb);
	}
	else
	{
		return -1;
	}
	return 0;
}


void maincontrol::mousePressEvent(QMouseEvent *e)
{
	qDebug()<<" maincontrol::mousePressEvent"<<e->pos();
	if(m_rcTab.contains(e->pos()) && ui.tabWidget->currentIndex() == 0)
	{
		m_Point = e->pos();
	}
	return QWidget::mousePressEvent(e);
}


void maincontrol::mouseReleaseEvent(QMouseEvent *e)
{
	qDebug()<<" maincontrol::mouseReleaseEvent"<<e->pos();
	if(m_rcTab.contains(e->pos()) && ui.tabWidget->currentIndex() == 0)
	{
		m_Point = e->pos();
	}
	return QWidget::mouseReleaseEvent(e);
}

void maincontrol::mouseMoveEvent(QMouseEvent *e)
{
	qDebug()<<" maincontrol::mouseMoveEvent"<<e->pos();
	if(ui.tabWidget->currentIndex() == 0)
	{
		QPoint pt(e->pos().x() - m_Point.x(),e->pos().y() - m_Point.y());
		qDebug()<<" m_ScrollArea->scroll"<<pt;
		m_ScrollArea->verticalScrollBar()->setValue(m_ScrollArea->verticalScrollBar()->value() - pt.y());
		m_Point = e->pos();
	}
	return QWidget::mouseMoveEvent(e);
}

int maincontrol::updateUI()
{

}


void maincontrol::btnClick()
{
	addlabel(0,QString("BT"));
}


void maincontrol::UpdateData()
{
    tabchart1->addData(int_temp+40);
    tabchart2->addData(int_humid);
}


//void maincontrol::OnComRead()
//{
//	uint8_t com_initdata[100];
//	int ret = 0;
//
//	ret = m_comcontrol->ComRecv((char *)com_initdata, sizeof(com_initdata));
//	printf("read : %d  \n",ret);
//
//	ret = put_data_to_list(list, (uint8_t *)com_initdata, sizeof(com_initdata));
//	printf("put_data_to_list %d \n",ret);
//}

//int maincontrol::get_realdata()
//{
//	int ret = parase_protocol(list, parase_data);
//	printf("parase_protocol %d \n",ret);
//}
//
//static int parase_data(uint8_t *_protocol, int _len)
//{
//	protocol_stable_t* stable =(protocol_stable_t*)_protocol;
//	//获取序列号，时间，置标志位
//
//	if(stable->command != CMD_REPORTER)  //根据命令情况，转换为不同的数据结构
//	{
//		protocol_reporter_status* report_data = (protocol_reporter_status*)stable;
//		int value = big_to_uint32(&report_data->value);
//	}
//
//	switch(stable->flag)
//	{
//		case FLAG_TEMPERATURE:
//			printf("====温度： %d==== \n", value);
//			//m_temperature.push_back(int_temp);
//			break;
//
//		case FLAG_HUMIDITY:
//			printf("====湿度： %d====\n", value);
//		//	m_humidity.push_back(int_humid);
//			break;
//
//		case FLAG_HUMIDITY:
//
//	}
//		return 0;
//}
