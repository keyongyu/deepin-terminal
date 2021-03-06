#include "remotemanagementpanel.h"
#include "serverconfigitem.h"
#include "shortcutmanager.h"
#include "service.h"

#include <DLog>

RemoteManagementPanel::RemoteManagementPanel(QWidget *parent) : CommonPanel(parent)
{
    initUI();
}

void RemoteManagementPanel::refreshPanel()
{
    clearSearchInfo();
    //--added byq qinyaning(nyq) to solve the show when not exist the server-config.conf--//
//    ServerConfigManager::instance()->initServerConfig();
    //---------------------------//
    m_listWidget->refreshAllDatas();
    refreshSearchState();
}

void RemoteManagementPanel::refreshSearchState()
{
    if (m_listWidget->count() >= 2) {
        /************************ Add by m000743 sunchengxi 2020-04-22:搜索显示异常 Begin************************/
        m_searchEdit->clearEdit();
        /************************ Add by m000743 sunchengxi 2020-04-22:搜索显示异常  End ************************/
        m_searchEdit->show();
    } else {
        m_searchEdit->hide();
    }
}

void RemoteManagementPanel::showCurSearchResult()
{
    QString strTxt = m_searchEdit->text();
    if (strTxt.isEmpty())
        return;
    emit showSearchPanel(strTxt);
}

void RemoteManagementPanel::showAddServerConfigDlg()
{
    // 弹窗显示
    Service::instance()->setIsDialogShow(window(), true);

    ServerConfigOptDlg *dlg = new ServerConfigOptDlg(ServerConfigOptDlg::SCT_ADD, nullptr, this);
    connect(dlg, &ServerConfigOptDlg::finished, this, [ = ](int result) {
        // 弹窗隐藏或消失
        Service::instance()->setIsDialogShow(window(), false);
        if (result == QDialog::Accepted) {
            QModelIndex index = m_listWidget->currentIndex(dlg->getServerName());
            m_listWidget->scrollTo(index);
        }
    });
    dlg->show();
}

void RemoteManagementPanel::initUI()
{
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);

    m_searchEdit = new DSearchEdit(this);
    m_listWidget = new ServerConfigList(this);
    m_pushButton = new DPushButton(this);

    m_searchEdit->setClearButtonEnabled(true);

    m_listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    m_pushButton->setFixedHeight(36);
    m_pushButton->setText(tr("Add Server"));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addSpacing(10);
    hlayout->addWidget(m_searchEdit);
    hlayout->addSpacing(10);
    hlayout->setSpacing(0);
    hlayout->setMargin(0);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_pushButton);
    btnLayout->addSpacing(10);
    btnLayout->setSpacing(0);
    btnLayout->setMargin(0);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addSpacing(10);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_listWidget);
    vlayout->addLayout(btnLayout);
    vlayout->addSpacing(12);
    vlayout->setMargin(0);
    vlayout->setSpacing(10);
    setLayout(vlayout);

    connect(m_searchEdit, &DSearchEdit::returnPressed, this, &RemoteManagementPanel::showCurSearchResult);
    connect(m_pushButton, &DPushButton::clicked, this, &RemoteManagementPanel::showAddServerConfigDlg);
    connect(m_listWidget, &ServerConfigList::itemClicked, this, &RemoteManagementPanel::listItemClicked);
    connect(m_listWidget, &ServerConfigList::groupClicked, this, &RemoteManagementPanel::showServerConfigGroupPanel);
    connect(m_listWidget, &ServerConfigList::listItemCountChange, this, &RemoteManagementPanel::refreshSearchState);
    connect(ServerConfigManager::instance(), &ServerConfigManager::refreshList, this, [ = ]() {
        if (m_isShow) {
            refreshPanel();
        }
    });
}

void RemoteManagementPanel::listItemClicked(ServerConfig *curItemServer)
{
    if (nullptr != curItemServer) {
        emit doConnectServer(curItemServer);
    } else {
        qDebug() << "remote item from panel is null";
    }
}
