/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_HostAddressPort_H
#define SWIFTEN_HostAddressPort_H

#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class HostAddressPort {
		public:
			HostAddressPort(const HostAddress& address, int port) : address_(address), port_(port) {
			}

			const HostAddress& getAddress() const {
				return address_;
			}

			int getPort() const {
				return port_;
			}

			bool operator==(const HostAddressPort& o) const {
				return address_ == o.address_ && port_ == o.port_;
			}

		private:
			HostAddress address_;
			int port_;
	};
}

#endif
