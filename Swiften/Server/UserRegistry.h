/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class String;
	class JID;

	class UserRegistry {
		public:
			virtual ~UserRegistry();

			virtual bool isValidUserPassword(const JID& user, const String& password) const = 0;
	};
}
