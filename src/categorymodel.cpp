#include "categorymodel.h"
#include <QtDebug>
#include <QUrl>
#include "song.h"

CategoryModel::CategoryModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    Playlist *p = new Playlist("All");
    categories.push_back(p);
}


int CategoryModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return categories.size();
    //qDebug() << Q_FUNC_INFO  << parent.data(Qt::DisplayRole);

    Playlist* root = dynamic_cast<Playlist*>((MyItem*)parent.internalPointer());
    if (!root) return 0;
    qDebug() << Q_FUNC_INFO  << parent.model() << root->getSongs().size();
    return root->getSongs().size();
}

int CategoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex CategoryModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(column);
    qDebug() << Q_FUNC_INFO << row;
    if (!parent.isValid())
       return createIndex(row, 0, categories[row]);

    const std::vector<Song*> &songs = categories[parent.row()]->getSongs();
    if (songs.size() == 0)
        return QModelIndex();
    Song* s = songs[row];
    //qDebug() << Q_FUNC_INFO << row << s->getDataToDisplay();
    return createIndex(row, 0, s);
}

QModelIndex CategoryModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();
//    if (!child.isValid())
//        return createIndex(0, 0, categories[0]);

    qDebug() << Q_FUNC_INFO << "before" << child << child.row();
    MyItem* item = (static_cast< MyItem* >(child.internalPointer()));

    if (item == nullptr)
    {
        qFatal("null!!!!");
        qDebug() << Q_FUNC_INFO << "null!!!!";
        //createIndex(row aktualnej kategorii, 0, aktualna kategoria*)
    }


    qDebug() << Q_FUNC_INFO << "between" << item << item->getDataToDisplay();
    MyItem* par = item->getParent();

    if (par == nullptr)
        return QModelIndex();
     qDebug() << Q_FUNC_INFO << "after" << par << par->getDataToDisplay();

    Playlist* cat = (static_cast<Playlist*>(par));
    int parentRow = getCategoryRow(cat);
    qDebug() << Q_FUNC_INFO << parentRow;
    return createIndex(parentRow, 0, par);
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
   // qDebug() << Q_FUNC_INFO << index << (Qt::ItemDataRole)role ;
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole) return QVariant();
    MyItem* item = static_cast<MyItem*>(index.internalPointer());
    if (item)
        return QVariant(item->getDataToDisplay());

    return QVariant();
}

bool CategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    MyItem* item = static_cast<MyItem*>(index.internalPointer());
    item->setDataToDisplay(value.toString());

    emit dataChanged(index, index);
    return true;
}

void CategoryModel::addCategory(Playlist* cat)
{
    qDebug() << Q_FUNC_INFO;
    beginInsertRows(QModelIndex(), categories.size(), categories.size());
    categories.push_back(cat);
    endInsertRows();
}

std::vector <Playlist*> CategoryModel::getCategories() const
{
    return categories;
}

Playlist* CategoryModel::getCategory(const QModelIndex &index) const
{
    return categories[index.row()];
}

int CategoryModel::getCategoryRow(Playlist* p) const
{
    auto it = find(categories.begin(), categories.end(), p);
    return std::distance (categories.begin(), it);
}

bool CategoryModel::addNewSong(QUrl url)
{
    if (categories.size() == 0)
        return false;

    beginInsertRows(createIndex(0, 0, categories[0]), categories[0]->getSongs().size(), categories[0]->getSongs().size());
    Song* s = new Song(url);
    categories[0]->addToPlaylist(s);
    s->setCategory(categories[0]);
    endInsertRows();

    return true;
}

void CategoryModel::categoryChanged()
{
    beginResetModel();
    endResetModel();
}
