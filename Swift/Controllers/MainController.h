#ifndef SWIFTEN_MainController_H
#define SWIFTEN_MainController_H

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Client/ClientError.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/Error.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Settings/SettingsProvider.h"
#include "Swiften/Elements/CapsInfo.h"
#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Roster/XMPPRoster.h"

namespace Swift {
	class AvatarStorage;
	class Application;
	class Client;
	class ChatWindowFactory;
	class ChatController;
	class EventController;
	class MainWindowFactory;
	class MainWindow;
	class NickResolver;
	class RosterController;
	class XMPPRosterController;
	class DiscoInfoResponder;
	class AvatarManager;
	class LoginWindow;
	class EventLoop;
	class SoftwareVersionResponder;
	class LoginWindowFactory;
	class TreeWidgetFactory;
	class MUCController;
	class PresenceOracle;
	class SystemTray;
	class SystemTrayController;
	class SoundEventController;
	class SoundPlayer;

	class MainController : public MUCRegistry {
		public:
			MainController(ChatWindowFactory* chatWindowFactory, MainWindowFactory *mainWindowFactory, LoginWindowFactory *loginWindowFactory, TreeWidgetFactory* treeWidgetFactory, SettingsProvider *settings, Application* application, SystemTray* systemTray, SoundPlayer* soundPlayer);
			~MainController();


		private:
			void resetClient();

			void handleConnected();
			void handleLoginRequest(const String& username, const String& password, const String& certificateFile, bool remember);
			void handleChatRequest(const String& contact);
			void handleJoinMUCRequest(const JID& muc, const String& nick);
			void handleIncomingPresence(boost::shared_ptr<Presence> presence);
			void handleChatControllerJIDChanged(const JID& from, const JID& to);
			void handleIncomingMessage(boost::shared_ptr<Message> message);
			void handleChangeStatusRequest(StatusShow::Type show, const String &statusText);
			void handleError(const ClientError& error);
			void handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo>, const boost::optional<Error>&);
			void handleEventQueueLengthChange(int count);
			void handleOwnVCardReceived(boost::shared_ptr<VCard> vCard, const boost::optional<Error>& error);
			ChatController* getChatController(const JID &contact);
			void sendPresence(boost::shared_ptr<Presence> presence);
			void logout();
			void signout();

			virtual bool isMUC(const JID& muc) const;
	
		private:	
			void performLoginFromCachedCredentials();
			void setManagersEnabled(bool enabled);
			Client* client_;
			ChatWindowFactory* chatWindowFactory_;
			MainWindowFactory* mainWindowFactory_;
			LoginWindowFactory* loginWindowFactory_;
			TreeWidgetFactory* treeWidgetFactory_;
			SettingsProvider *settings_;
			Application* application_;
			AvatarStorage* avatarStorage_;
			ChatController* chatController_;
			XMPPRosterController* xmppRosterController_;
			RosterController* rosterController_;
			EventController* eventController_;
			LoginWindow* loginWindow_;
			SoftwareVersionResponder* clientVersionResponder_;
			NickResolver* nickResolver_;
			DiscoInfoResponder* discoResponder_;
			boost::shared_ptr<CapsInfo> capsInfo_;
			std::map<JID, MUCController*> mucControllers_;
			std::map<JID, ChatController*> chatControllers_;
			boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
			boost::shared_ptr<XMPPRoster> xmppRoster_;;
			JID jid_;
			PresenceOracle* presenceOracle_;
			SystemTrayController* systemTrayController_;
			SoundEventController* soundEventController_;
			AvatarManager* avatarManager_;
			boost::shared_ptr<Presence> lastSentPresence_;
			String vCardPhotoHash_;
			boost::shared_ptr<Presence> queuedPresence_;
			String password_;
			String certificateFile_;
	};
}
#endif

