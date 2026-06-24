#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QObject>

class MainViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentView READ currentView WRITE setCurrentView NOTIFY currentViewChanged)

public:
    explicit MainViewModel(QObject *parent = nullptr);
    ~MainViewModel() override;

    int currentView() const;
    void setCurrentView(int view);

signals:
    void currentViewChanged(int view);

private:
    int m_currentView;
};

#endif // MAINVIEWMODEL_H