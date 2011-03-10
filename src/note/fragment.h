/*
    Rekollect: A note taking application
    Copyright (C) 2011  Jason Jackson <jacksonje@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QObject>


class Fragment : public QObject
{
    Q_OBJECT
public:
    Fragment();
    Fragment(const Fragment &fragment);
    Fragment &operator=(const Fragment &fragment);

    enum FontSize {
        SMALL, NORMAL, LARGE, HUGE
    };

    bool bold;
    bool italic;
    bool strikeThrough;
    bool highlight;
    FontSize fontSize;
    QString anchorReference;
    QString text;

};

#endif // FRAGMENT_H
