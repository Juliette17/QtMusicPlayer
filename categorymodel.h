#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QAbstractItemModel>
#include "playlist.h"

class CategoryModel : public QAbstractItemModel
{
public:
    explicit CategoryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

    void addCategory(Playlist* cat);
    std::vector <Playlist*> getCategories() const;
    Playlist* getCategory(const QModelIndex &index) const;
    int getCategoryRow(Playlist*) const;
    bool addNewSong(QUrl);
    void categoryChanged();

private:
    std::vector<Playlist*> categories;

};

#endif // CATEGORYMODEL_H
