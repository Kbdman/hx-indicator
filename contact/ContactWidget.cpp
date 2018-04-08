#include "ContactWidget.h"
#include "ui_ContactWidget.h"


#include "ContactDataUtil.h"
#include "ContactTreeWidget.h"
#include "ContactSearchWidget.h"
#include "ContactBriefWidget.h"
#include "ContactAddWidget.h"
#include "ContactInfoWidget.h"

#include "wallet.h"

class ContactWidget::ContactWidgetPrivate
{
public:
    ContactWidgetPrivate()
        :contactTreeWidget(new ContactTreeWidget())
        ,contactSearchWidget(new ContactSearchWidget())
        ,contactBriefWidget(new ContactBriefWidget())
        ,contactAddWidget(nullptr)
        ,contactInfoWidget(nullptr)
        ,contactSheet(nullptr)
        ,contactFilePath("")
    {
        if( UBChain::getInstance()->configFile->contains("/settings/chainPath"))
        {
            contactFilePath = UBChain::getInstance()->configFile->value("/settings/chainPath").toString() + "/contacts.dat";
        }
        else
        {
            contactFilePath = UBChain::getInstance()->appDataPath + "/contacts.dat";
        }
    }
public:
    ContactTreeWidget *contactTreeWidget;//联系人编组树
    ContactSearchWidget *contactSearchWidget;//搜索界面
    ContactBriefWidget *contactBriefWidget;//右上角简介界面
    ContactAddWidget *contactAddWidget;//新增界面
    ContactInfoWidget *contactInfoWidget;//联系人显示界面

    std::shared_ptr<ContactSheet> contactSheet;//数据源

    QString contactFilePath;
};

ContactWidget::ContactWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactWidget),
    _p(new ContactWidgetPrivate())
{
    ui->setupUi(this);

    InitWidget();
    InitData();
}

ContactWidget::~ContactWidget()
{
    delete _p;
    delete ui;
}

void ContactWidget::AddNewContactSlots()
{
    if(!_p->contactAddWidget)
    {
        disconnect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,_p->contactTreeWidget,&ContactTreeWidget::AddPersonSlots);
        disconnect(_p->contactTreeWidget,&ContactTreeWidget::GroupModifyFinishedSignal,_p->contactAddWidget,&ContactAddWidget::groupModifySlots);
    }
    _p->contactAddWidget = new ContactAddWidget(_p->contactSheet);
    ui->scrollArea_rightBottom->setWidget(_p->contactAddWidget);
    connect(_p->contactAddWidget,&ContactAddWidget::addContactFinishedSignal,_p->contactTreeWidget,&ContactTreeWidget::AddPersonSlots);
    connect(_p->contactTreeWidget,&ContactTreeWidget::GroupModifyFinishedSignal,_p->contactAddWidget,&ContactAddWidget::groupModifySlots);
}

void ContactWidget::ShowContactInfoSlots(const QString &address)
{
    if(!_p->contactInfoWidget)
    {
        disconnect(_p->contactInfoWidget,&ContactInfoWidget::gotoTransferPage,this,&ContactWidget::gotoTransferPage);
    }
    _p->contactInfoWidget = new ContactInfoWidget();
    _p->contactInfoWidget->setData(_p->contactSheet->findPerson(address));
    ui->scrollArea_rightBottom->setWidget(_p->contactInfoWidget);
    connect(_p->contactInfoWidget,&ContactInfoWidget::gotoTransferPage,this,&ContactWidget::gotoTransferPage);
    connect(_p->contactTreeWidget,&ContactTreeWidget::PersonModifyFinishedSignal,_p->contactInfoWidget,&ContactInfoWidget::PersonModifyFinishedSlots);
}

void ContactWidget::InitStyle()
{
    //setWindowFlags(Qt::FramelessWindowHint);
    //
    //setAutoFillBackground(true);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor(40,46,66));
    //setPalette(palette);

    setStyleSheet("QWidget{background-color:rgb(40,46,66);border:none;color:white;}");
    //setStyleSheet("QWidget{background-color:rgb(40,46,66);color:white;}");
    //setStyleSheet("QWidget{background-color:rgb(24,28,45);border:none;}");
}

void ContactWidget::InitWidget()
{
    InitStyle();

    ui->scrollArea_leftBottom->setWidget(_p->contactTreeWidget);
    ui->scrollArea_leftTop->setWidget(_p->contactSearchWidget);
    ui->scrollArea_rightTop->setWidget(_p->contactBriefWidget);

    connect(_p->contactTreeWidget,&ContactTreeWidget::ShowContactPerson,this,&ContactWidget::ShowContactInfoSlots);
    connect(_p->contactSearchWidget,&ContactSearchWidget::searchSignal,_p->contactTreeWidget,&ContactTreeWidget::QueryPersonSlots);
    connect(_p->contactBriefWidget,&ContactBriefWidget::addNewContactSignal,this,&ContactWidget::AddNewContactSlots);
}

void ContactWidget::InitData()
{
    if(!ContactDataUtil::readContactSheetFromPath(_p->contactFilePath,_p->contactSheet))
    {//联系人文件解析失败，自己新建一个联系人名单
       _p->contactSheet = std::make_shared<ContactSheet>();
       _p->contactSheet->sheetName = tr("New Sheet");
       std::shared_ptr<ContactGroup> group = std::make_shared<ContactGroup>();
       group->groupName = tr("Default Group");
       group->groupType = ContactGroup::GroupType_Default;
       _p->contactSheet->addGroup(group);
    }
    _p->contactTreeWidget->initContactSheet(_p->contactSheet);
}

void ContactWidget::SaveData()
{
    ContactDataUtil::writeContactSheetToPath(_p->contactFilePath,_p->contactSheet);
}

void ContactWidget::closeEvent(QCloseEvent *event)
{
    SaveData();
    QWidget::closeEvent(event);
}

//void ContactWidget::paintEvent(QPaintEvent *event)
//{
//    //QPainter painter(this);
//    //painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
//    //painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
//    //painter.drawRect(rect());
//}