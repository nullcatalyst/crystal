#pragma once

#include <cstdint>

#include "absl/container/flat_hash_map.h"
#include "util/msg/msg.hpp"
#include "util/net/network.hpp"

namespace util::net {

// struct ClientHandlerInterface {
//   void connecting() noexcept;
//   void connected() noexcept;
//   void disconnected() noexcept;
//   void connection_failed() noexcept;
// };

template <typename Handler>
class Client : public Network {
  [[no_unique_address]] Handler handler_;

  HSteamNetConnection connection_;

public:
  template <typename... Args>
  Client(Args&&... args) : handler_(std::forward<Args>(args)...) {}

  [[nodiscard]] constexpr const Handler& handler() const noexcept { return handler_; }
  [[nodiscard]] constexpr Handler&       handler() noexcept { return handler_; }

  Handler* operator->() noexcept { return &handler_; }

  void connect(const std::string& server_address) noexcept {
    // Set listen address.
    SteamNetworkingIPAddr ip_address;
    if (!ip_address.ParseString(server_address.c_str())) {
      msg::fatal("parsing server address \"", server_address, "\"");
    }

    std::array<SteamNetworkingConfigValue_t, 1> opts;
    set_callbacks(opts[0]);
    connection_ = lib_->ConnectByIPAddress(ip_address, opts.size(), opts.data());
    if (connection_ == k_HSteamNetConnection_Invalid) {
      msg::fatal("creating connection");
    }

    msg::debug("using server \"", server_address, "\"");
  }

  void poll() noexcept {
    for (;;) {
      ISteamNetworkingMessage* incoming_message = nullptr;
      int message_count = lib_->ReceiveMessagesOnConnection(connection_, &incoming_message, 1);
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

  void send_unreliable(void* data, size_t byte_length) noexcept {
    // TODO: Should this be `k_nSteamNetworkingSend_UnreliableNoNagle`?
    lib_->SendMessageToConnection(connection_, data, byte_length, k_nSteamNetworkingSend_Unreliable,
                                  nullptr);
  }

  void send_reliable(void* data, size_t byte_length) noexcept {
    lib_->SendMessageToConnection(connection_, data, byte_length, k_nSteamNetworkingSend_Reliable,
                                  nullptr);
  }

protected:
  void connection_status_changed(
      const SteamNetConnectionStatusChangedCallback_t* info) noexcept override {
    switch (info->m_info.m_eState) {
      case k_ESteamNetworkingConnectionState_Connecting:
        handler_.connecting();
        break;

      case k_ESteamNetworkingConnectionState_Connected:
        handler_.connected();
        break;

      case k_ESteamNetworkingConnectionState_ClosedByPeer:
        handler_.disconnected();
        lib_->CloseConnection(info->m_hConn, 0, nullptr, false);
        connection_ = k_HSteamNetConnection_Invalid;
        break;

      case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
          handler_.connection_failed();
        }
        lib_->CloseConnection(info->m_hConn, 0, nullptr, false);
        connection_ = k_HSteamNetConnection_Invalid;
        break;

      default:
        break;
    }
  }

  void message_received(const ISteamNetworkingMessage* message) noexcept override {}
};

}  // namespace util::net
