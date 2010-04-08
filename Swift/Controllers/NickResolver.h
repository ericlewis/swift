/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_NickResolver_H
#define SWIFTEN_NickResolver_H

#include <map>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class XMPPRoster;
	class MUCRegistry;
	class NickResolver {
		public:
			NickResolver(boost::shared_ptr<XMPPRoster> xmppRoster);
			String jidToNick(const JID& jid);
			void setMUCRegistry(MUCRegistry* registry);
		private:
			std::map<JID, String> map_;
			boost::shared_ptr<XMPPRoster> xmppRoster_;
			MUCRegistry* mucRegistry_;
	};
}
#endif


