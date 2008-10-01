#include "AlgMonitor.h"
#include "MantidUI.h"
#include "MantidAPI/AlgorithmManager.h"

#include <QtGui>

#include <iostream>

using namespace std;

QMutex AlgorithmMonitor::s_mutex;

AlgorithmMonitor::AlgorithmMonitor(MantidUI *m):m_mantidUI(m),m_nRunning(0),
m_finishedObserver(*this, &AlgorithmMonitor::handleAlgorithmFinishedNotification),
m_progressObserver(*this, &AlgorithmMonitor::handleAlgorithmProgressNotification),
m_errorObserver(*this, &AlgorithmMonitor::handleAlgorithmErrorNotification)
{}

void AlgorithmMonitor::add(Algorithm *alg)
{
    lock();
    alg->notificationCenter.addObserver(m_finishedObserver);
    alg->notificationCenter.addObserver(m_errorObserver);
    alg->notificationCenter.addObserver(m_progressObserver);
    m_algorithms.push_back(alg);
    unlock();
    emit countChanged(count());
}

void AlgorithmMonitor::remove(const Algorithm *alg)
{
    int i = -1;//m_algorithms.indexOf(alg);
    lock();
    for(int j=0;j<m_algorithms.size();j++)
        if (m_algorithms[j] == alg)
        {
            i = j;
            break;
        }
    if (i < 0) return;
    m_algorithms.remove(i);
    unlock();
    emit countChanged(count());
}

void AlgorithmMonitor::run()
{
    //exec();
}

void AlgorithmMonitor::update()
{
}

void AlgorithmMonitor::handleAlgorithmFinishedNotification(const Poco::AutoPtr<Algorithm::FinishedNotification>& pNf)
{
    remove(pNf->algorithm());
}

void AlgorithmMonitor::handleAlgorithmProgressNotification(const Poco::AutoPtr<Algorithm::ProgressNotification>& pNf)
{
    if (m_monitorDlg) emit needUpdateProgress(pNf->algorithm(),int(pNf->progress*100));
        //m_monitorDlg->updateProgress(pNf->algorithm(),int(pNf->progress*100));
}

void AlgorithmMonitor::handleAlgorithmErrorNotification(const Poco::AutoPtr<Algorithm::ErrorNotification>& pNf)
{
    remove(pNf->algorithm());
}

void AlgorithmMonitor::showDialog()
{
    m_monitorDlg = new MonitorDlg(m_mantidUI->appWindow(),this);
    m_monitorDlg->exec();
    m_monitorDlg = 0;
}

void AlgorithmMonitor::cancel(Algorithm *alg)
{
    int i = m_algorithms.indexOf(alg);
    if (i >= 0) alg->cancel();
}

//-----------------------------------------------------------------------------------------------//
MonitorDlg::MonitorDlg(QWidget *parent,AlgorithmMonitor *algMonitor):QDialog(parent),m_algMonitor(algMonitor)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_tree = 0;
    update(0);
    connect(algMonitor,SIGNAL(countChanged(int)),this,SLOT(update(int)));
    connect(algMonitor,SIGNAL(needUpdateProgress(const Algorithm*,int)),SLOT(updateProgress(const Algorithm*,int)));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *closeButton = new QPushButton("Close");
    connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_tree);
    layout->addLayout(buttonLayout);
	setLayout(layout);
	setWindowTitle("Mantid - Algorithm progress");
    resize(500,300);
}

MonitorDlg::~MonitorDlg()
{
}

void MonitorDlg::update(int n)
{
    if (!m_tree)
    {
        m_tree = new QTreeWidget;
        m_tree->setColumnCount(3);
        m_tree->setSelectionMode(QAbstractItemView::NoSelection);
        QStringList hList;
        hList<<"Algorithm"<<""<<"Progress";
        m_tree->setHeaderLabels(hList);
    }
    else
        m_tree->clear();
    m_algMonitor->lock();
    for(int i=0;i<m_algMonitor->count();i++)
    {
        m_algorithms << m_algMonitor->algorithms()[i];
        QStringList iList;
        iList<<QString::fromStdString(m_algMonitor->algorithms()[i]->name());
        QTreeWidgetItem *algItem = new QTreeWidgetItem(iList);
        m_tree->addTopLevelItem(algItem);
        QProgressBar *algProgress = new QProgressBar;
        AlgButton *cancelButton = new AlgButton("Cancel",m_algMonitor->algorithms()[i]);
        m_tree->setItemWidget(algItem,2,algProgress);
        m_tree->setItemWidget(algItem,1,cancelButton);
        algItem->addChild(new QTreeWidgetItem(QStringList("Properties:")));
        connect(cancelButton,SIGNAL(clicked(Algorithm *)),m_algMonitor,SLOT(cancel(Algorithm *)));
    }
    m_algMonitor->unlock();
}

void MonitorDlg::updateProgress(const Algorithm* alg,int p)
{
    int i = m_algorithms.indexOf(alg);
    if (i >= 0)
    {
        QTreeWidgetItem *item = m_tree->topLevelItem(i);
        if (item)
        {
            QProgressBar *algProgress = static_cast<QProgressBar*>(m_tree->itemWidget(item,2));
            algProgress->setValue(p);
        }
    }
}
