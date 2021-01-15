#pragma once

#include <cstdint>

#include "absl/container/flat_hash_map.h"
#include "util/msg/msg.hpp"
#include "util/net/network.hpp"

namespace util::net {

template <typename Handler, typename ClientData>
class Server : public Network {
  [[no_unique_address]] Handler handler_;
  HSteamListenSocket            listen_socket_;
  HSteamNetPollGroup            poll_group_;

  absl::flat_hash_map<HSteamNetConnection, ClientData> clients_;

public:
  template <typename... Args>
  Server(Args&&... args) : handler_(std::forward<Args>(args)...) {}

  void listen(uint16_t port) {
    // Set listen address.
    SteamNetworkingIPAddr local_address;
    local_address.Clear();
    local_address.m_port = port;

    std::array<SteamNetworkingConfigValue_t, 1> opts;
    set_callbacks(opts[0]);
    listen_socket_ = lib_->CreateListenSocketIP(local_address, opts.size(), opts.data());
    if (listen_socket_ == k_HSteamListenSocket_Invalid) {
      msg::fatal("listening on port ", port);
    }

    poll_group_ = lib_->CreatePollGroup();
    if (poll_group_ == k_HSteamNetPollGroup_Invalid) {
      msg::fatal("listening on port ", port);
    }

    msg::debug("server listening on port ", port);
  }

  void poll() {
    for (;;) {
      ISteamNetworkingMessage* incoming_message = nullptr;
      int message_count = lib_->ReceiveMessagesOnPollGroup(poll_group_, &incoming_message, 1);
      if (message_count == 0) {
        break;
      }
      if (message_count < 0) {
        msg::fatal("checking for incoming messages");
      }
      if (message_count != 1 || incoming_message == nullptr) {
        msg::fatal("checking for incoming messages");
      }

      message_received(incoming_message);
      incoming_message->Release();
    }

    run_callbacks();
  }

  void send_unreliable(HSteamNetConnection conn, void* data, size_t byte_length) {
    // TODO: Should this be `k_nSteamNetworkingSend_UnreliableNoNagle`?
    lib_->SendMessageToConnection(conn, data, byte_length, k_nSteamNetworkingSend_Unreliable,
                                  nullptr);
  }

  void send_reliable(HSteamNetConnection conn, void* data, size_t byte_length) {
    lib_->SendMessageToConnection(conn, data, byte_length, k_nSteamNetworkingSend_Reliable,
                                  nullptr);
  }

  void broadcast_unreliable(void* data, size_t byte_length) {
    for (const auto& [conn, client] : clients_) {
      send_unreliable(conn, data, byte_length);
    }
  }

  void broadcast_reliable(void* data, size_t byte_length) {
    for (const auto& [conn, client] : clients_) {
      send_reliable(conn, data, byte_length);
    }
  }

  void broadcast_unreliable(void* data, size_t byte_length, HSteamNetConnection except) {
    for (const auto& [conn, client] : clients_) {
      if (conn != except) {
        send_unreliable(conn, data, byte_length);
      }
    }
  }

  void broadcast_reliable(void* data, size_t byte_length, HSteamNetConnection except) {
    for (const auto& [conn, client] : clients_) {
      if (conn != except) {
        send_reliable(conn, data, byte_length);
      }
    }
  }

protected:
  void connection_status_changed(
      const SteamNetConnectionStatusChangedCallback_t* info) noexcept override {
    switch (info->m_info.m_eState) {
      case k_ESteamNetworkingConnectionState_Connected:
        // We will get a callback immediately after accepting the connection.
        // Since we are the server, we can ignore this, it's not news to us.
        break;

      case k_ESteamNetworkingConnectionState_ClosedByPeer:
      case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
        if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
          auto it = clients_.find(info->m_hConn);
          if (it == clients_.end()) {
            util::msg::fatal("disconnecting client that was never connected");
          }

          // // Select appropriate log messages
          // const char* pszDebugLogAction;
          // if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
          // {
          //   pszDebugLogAction = "problem detected locally";
          //   sprintf(temp, "Alas, %s hath fallen into shadow.  (%s)",
          //           itClient->second.m_sNick.c_str(), pInfo->m_info.m_szEndDebug);
          // } else {
          //   // Note that here we could check the reason code to see if
          //   // it was a "usual" connection or an "unusual" one.
          //   pszDebugLogAction = "closed by peer";
          //   sprintf(temp, "%s hath departed", itClient->second.m_sNick.c_str());
          // }

          // // Spew something to our own log.  Note that because we put their nick
          // // as the connection description, it will show up, along with their
          // // transport-specific data (e.g. their IP address)
          // Printf("Connection %s %s, reason %d: %s\n", info->m_info.m_szConnectionDescription,
          //        pszDebugLogAction, info->m_info.m_eEndReason, info->m_info.m_szEndDebug);

          clients_.erase(it);

          // // Send a message so everybody else knows what happened
          // SendStringToAllClients(temp);
        } else {
          // assert(info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting);
        }

        lib_->CloseConnection(info->m_hConn, 0, nullptr, false);
        break;
      }

      case k_ESteamNetworkingConnectionState_Connecting: {
        if (clients_.find(info->m_hConn) != clients_.end()) {
          util::msg::fatal("connecting to already connected client");
        }

        util::msg::debug("connection request from \"", info->m_info.m_szConnectionDescription,
                         "\"");

        // TODO: Add a way to allow the implementor to decide whether to accept or decline a
        // connection. This will be important for things like server/client versions.
        if (lib_->AcceptConnection(info->m_hConn) != k_EResultOK) {
          // Accepting the connection can fail, like if the remote client tried to connect, but then
          // disconnected, the connection may already be half closed.
          lib_->CloseConnection(info->m_hConn, 0, nullptr, false);
          util::msg::debug("failed to accept connection: connection was already closed");
          break;
        }

        // Assign the poll group
        if (!lib_->SetConnectionPollGroup(info->m_hConn, poll_group_)) {
          lib_->CloseConnection(info->m_hConn, 0, nullptr, false);
          util::msg::debug("failed to add connection to poll group");
          break;
        }

        // // Generate a random nick.  A random temporary nick
        // // is really dumb and not how you would write a real chat server.
        // // You would want them to have some sort of signon message,
        // // and you would keep their client in a state of limbo (connected,
        // // but not logged on) until them.  I'm trying to keep this example
        // // code really simple.
        // char nick[64];
        // sprintf(nick, "BraveWarrior%d", 10000 + (rand() % 100000));

        // // Send them a welcome message
        // sprintf(temp,
        //         "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command "
        //         "'/nick' we shall know thee otherwise.",
        //         nick);
        // SendStringToClient(pInfo->m_hConn, temp);

        // // Also send them a list of everybody who is already connected
        // if (m_mapClients.empty()) {
        //   SendStringToClient(pInfo->m_hConn, "Thou art utterly alone.");
        // } else {
        //   sprintf(temp, "%d companions greet you:", (int)m_mapClients.size());
        //   for (auto& c : m_mapClients) SendStringToClient(pInfo->m_hConn,
        //   c.second.m_sNick.c_str());
        // }

        // // Let everybody else know who they are for now
        // sprintf(temp,
        //         "Hark!  A stranger hath joined this merry host.  For now we shall call them
        //         '%s'", nick);
        // SendStringToAllClients(temp, pInfo->m_hConn);

        clients_.emplace(info->m_hConn, ClientData());
        break;
      }

      default:
        break;
    }
  }

  void message_received(const ISteamNetworkingMessage* message) noexcept override {}
};

}  // namespace util::net
