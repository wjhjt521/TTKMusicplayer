#include "musicenhancedwidget.h"
#include "musicsettingmanager.h"
#include "musicconnectionpool.h"
#include "musicuiobject.h"
#include "musickugouuiobject.h"

#include <QLabel>
#include <QMovie>
#include <QMenu>
#include <QWidgetAction>
#include <QButtonGroup>

#define LABEL_BUTTON_WIDGET 116
#define LABEL_BUTTON_HEIGHT 111

MusicEnhancedToolButton::MusicEnhancedToolButton(QWidget *parent)
    : QToolButton(parent)
{
    m_label = new QLabel(this);
    m_label->resize(LABEL_BUTTON_WIDGET, LABEL_BUTTON_HEIGHT);

    m_movie = new QMovie(":/enhance/lb_enter", QByteArray(), this);
    m_label->setMovie(m_movie);
}

MusicEnhancedToolButton::~MusicEnhancedToolButton()
{
    delete m_movie;
    delete m_label;
}

QString MusicEnhancedToolButton::getClassName()
{
    return staticMetaObject.className();
}

void MusicEnhancedToolButton::enterEvent(QEvent *event)
{
    QToolButton::enterEvent(event);
    m_movie->start();
}


MusicEnhancedWidget::MusicEnhancedWidget(QWidget *parent)
    : QToolButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("magic music"));

    initWidget();
    M_CONNECTION_PTR->setValue(getClassName(), this);
}

MusicEnhancedWidget::~MusicEnhancedWidget()
{
    M_CONNECTION_PTR->poolDisConnect(getClassName());
    delete m_caseButton;
    delete m_Button1;
    delete m_Button2;
    delete m_Button3;
    delete m_Button4;
}

QString MusicEnhancedWidget::getClassName()
{
    return staticMetaObject.className();
}

void MusicEnhancedWidget::initWidget()
{
    m_menu = new QMenu(this);
    m_menu->setWindowFlags(m_menu->windowFlags() | Qt::FramelessWindowHint);
    m_menu->setAttribute(Qt::WA_TranslucentBackground);
    m_menu->setStyleSheet("QMenu{ border:none; background:transparent;}");
    QWidgetAction *actionWidget = new QWidgetAction(m_menu);

    QWidget *containWidget = new QWidget(m_menu);
    containWidget->setFixedSize(272, 370);
    containWidget->setObjectName("containWidget");
    containWidget->setStyleSheet("#containWidget{border:none; background:url(':/enhance/lb_background')}");

    QToolButton *labelButton = new QToolButton(containWidget);
    labelButton->setGeometry(80, 20, 126, 40);
    labelButton->setStyleSheet(MusicKuGouUIObject::MKGEnhanceTitle);
    labelButton->setCursor(Qt::PointingHandCursor);

    QToolButton *helpButton = new QToolButton(containWidget);
    helpButton->setGeometry(205, 3, 24, 24);
    helpButton->setStyleSheet(MusicKuGouUIObject::MKGEnhanceHelp);
    helpButton->setCursor(Qt::PointingHandCursor);

    QToolButton *shareButton = new QToolButton(containWidget);
    shareButton->setGeometry(230, 3, 24, 24);
    shareButton->setStyleSheet(MusicKuGouUIObject::MKGEnhanceShare);
    shareButton->setCursor(Qt::PointingHandCursor);

    QToolButton *closeButton = new QToolButton(containWidget);
    closeButton->setGeometry(255, 8, 16, 16);
    closeButton->setStyleSheet(MusicKuGouUIObject::MKGEnhanceClose);
    closeButton->setCursor(Qt::PointingHandCursor);

    m_caseButton = new QToolButton(containWidget);
    m_caseButton->setGeometry(200, 70, 62, 38);
    m_caseButton->setStyleSheet(MusicKuGouUIObject::MKGEnhanceOn);
    m_caseButton->setCursor(Qt::PointingHandCursor);

    m_Button1 = new MusicEnhancedToolButton(containWidget);
    m_Button1->setGeometry(15, 115, LABEL_BUTTON_WIDGET, LABEL_BUTTON_HEIGHT);
    m_Button1->setStyleSheet("background-image:url(':/enhance/lb_3dOff')");
    m_Button1->setCursor(Qt::PointingHandCursor);

    m_Button2 = new MusicEnhancedToolButton(containWidget);
    m_Button2->setGeometry(145, 115, LABEL_BUTTON_WIDGET, LABEL_BUTTON_HEIGHT);
    m_Button2->setStyleSheet("background-image:url(':/enhance/lb_NICAMOff')");
    m_Button2->setCursor(Qt::PointingHandCursor);

    m_Button3 = new MusicEnhancedToolButton(containWidget);
    m_Button3->setGeometry(15, 240, LABEL_BUTTON_WIDGET, LABEL_BUTTON_HEIGHT);
    m_Button3->setStyleSheet("background-image:url(':/enhance/lb_subwooferOff')");
    m_Button3->setCursor(Qt::PointingHandCursor);

    m_Button4 = new MusicEnhancedToolButton(containWidget);
    m_Button4->setGeometry(145, 240, LABEL_BUTTON_WIDGET, LABEL_BUTTON_HEIGHT);
    m_Button4->setStyleSheet("background-image:url(':/enhance/lb_vocalOff')");
    m_Button4->setCursor(Qt::PointingHandCursor);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(m_Button1, 1);
    group->addButton(m_Button2, 2);
    group->addButton(m_Button3, 3);
    group->addButton(m_Button4, 4);
    connect(group, SIGNAL(buttonClicked(int)), SLOT(setEnhancedMusicConfig(int)));

    actionWidget->setDefaultWidget(containWidget);

    m_menu->addAction(actionWidget);
    setMenu(m_menu);
    setPopupMode(QToolButton::InstantPopup);

    m_lastSelectedIndex = M_SETTING_PTR->value(MusicSettingManager::EnhancedMusicChoiced).toInt();
    connect(m_caseButton, SIGNAL(clicked()), SLOT(caseButtonOnAndOff()));
}

void MusicEnhancedWidget::setEnhancedMusicConfig(int type)
{
    setObjectName("EnhancedWidget");
    QString style = MusicKuGouUIObject::MKGBtnMagic;
    switch(type)
    {
        case 0: style += "#EnhancedWidget{ margin-left: -2px; }"; break;
        case 1: style += "#EnhancedWidget{ margin-left: -46px; }"; break;
        case 2: style += "#EnhancedWidget{ margin-left: -178px; }"; break;
        case 3: style += "#EnhancedWidget{ margin-left: -90px; }"; break;
        case 4: style += "#EnhancedWidget{ margin-left: -134px; }"; break;
    }
    setStyleSheet( style );

    QString prfix = QString("background-image:url(':/enhance/lb_%1')");
    m_caseButton->setStyleSheet(type ? MusicKuGouUIObject::MKGEnhanceOn :
                                       MusicKuGouUIObject::MKGEnhanceOff);
    m_Button1->setStyleSheet(prfix.arg(type == 1 ? "3dOn" : "3dOff"));
    m_Button2->setStyleSheet(prfix.arg(type == 2 ? "NICAMOn" : "NICAMOff"));
    m_Button3->setStyleSheet(prfix.arg(type == 3 ? "subwooferOn" : "subwooferOff"));
    m_Button4->setStyleSheet(prfix.arg(type == 4 ? "vocalOn" : "vocalOff"));

    m_lastSelectedIndex = (type == 0) ? m_lastSelectedIndex : type;
    M_SETTING_PTR->setValue(MusicSettingManager::EqualizerEnableChoiced, 0);
    M_SETTING_PTR->setValue(MusicSettingManager::EnhancedMusicChoiced, type);
    emit enhancedMusicChanged(type);
}

void MusicEnhancedWidget::caseButtonOnAndOff()
{
    setEnhancedMusicConfig( m_caseButton->styleSheet().contains(":/enhance/btn_magic_off_normal") ?
                            m_lastSelectedIndex : 0);
}
