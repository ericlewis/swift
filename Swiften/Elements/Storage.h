/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class Storage : public Payload {
		public:
			struct Conference {
				Conference() : autoJoin(false) {}

				String name;
				JID jid;
				bool autoJoin;
				String nick;
				String password;
			};

			Storage() {
			}

			const std::vector<Conference>& getConferences() const {
				return conferences;
			}

			void addConference(const Conference& conference) {
				conferences.push_back(conference);
			}

		private:
			std::vector<Conference> conferences;
	};
}
