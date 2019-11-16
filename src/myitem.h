#ifndef MYITEM_H
#define MYITEM_H

#include <Qstring>

class MyItem
{
public:
    MyItem();
    virtual ~MyItem(){}
    virtual QString getDataToDisplay() const = 0;
    virtual void setDataToDisplay(QString) = 0;
    virtual MyItem * getParent() const = 0;
};

#endif // MYITEM_H
