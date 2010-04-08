/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class QtTreeWidgetItem;
	class QtContextMenu {
		public:
      virtual ~QtContextMenu();

			virtual void show(QtTreeWidgetItem* item) = 0;
	};
}
