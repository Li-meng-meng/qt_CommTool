#include "MainViewModel.h"

MainViewModel::MainViewModel(QObject *parent)
    : QObject(parent)
    , m_currentView(0)
{
}

MainViewModel::~MainViewModel()
{
}

int MainViewModel::currentView() const
{
    return m_currentView;
}

void MainViewModel::setCurrentView(int view)
{
    if (m_currentView != view) {
        m_currentView = view;
        emit currentViewChanged(m_currentView);
    }
}