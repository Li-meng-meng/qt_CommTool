#include "AppSettingsViewModel.h"

AppSettingsViewModel::AppSettingsViewModel(QObject *parent)
    : QObject(parent)
    , m_isDarkMode(false)
{
}

AppSettingsViewModel::~AppSettingsViewModel()
{
}

bool AppSettingsViewModel::getIsDarkMode() const
{
    return m_isDarkMode;
}

void AppSettingsViewModel::setIsDarkMode(bool dark)
{
    if (m_isDarkMode != dark) {
        m_isDarkMode = dark;
        emit isDarkModeChanged(m_isDarkMode);
    }
}