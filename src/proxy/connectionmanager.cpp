/*
 * Copyright (C) 2015 Fanout, Inc.
 *
 * This file is part of Pushpin.
 *
 * Pushpin is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Pushpin is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "connectionmanager.h"

#include <assert.h>
#include <QHash>
#include "uuidutil.h"

class ConnectionManager::Private
{
public:
	class Item
	{
	public:
		QPair<QByteArray, QByteArray> rid;
		WebSocket *sock;
		QByteArray cid;

		Item() :
			sock(0)
		{
		}
	};

	QHash<WebSocket*, Item*> itemsBySock;

	Private()
	{
	}

	~Private()
	{
		clearItemsBySock();
	}

	void clearItemsBySock()
	{
		QHashIterator<WebSocket*, Item*> it(itemsBySock);
		while(it.hasNext())
		{
			it.next();
			delete it.value();
		}
		itemsBySock.clear();
	}
};

ConnectionManager::ConnectionManager()
{
	d = new Private;
}

ConnectionManager::~ConnectionManager()
{
	delete d;
}

QByteArray ConnectionManager::addConnection(WebSocket *sock)
{
	assert(!d->itemsBySock.contains(sock));

	Private::Item *i = new Private::Item;
	i->sock = sock;
	i->cid = UuidUtil::createUuid();
	d->itemsBySock[i->sock] = i;

	return i->cid;
}

QByteArray ConnectionManager::getConnection(WebSocket *sock)
{
	Private::Item *i = d->itemsBySock.value(sock);
	if(!i)
		return QByteArray();

	return i->cid;
}

void ConnectionManager::removeConnection(WebSocket *sock)
{
	Private::Item *i = d->itemsBySock.value(sock);
	assert(i);
	d->itemsBySock.remove(sock);
	delete i;
}
