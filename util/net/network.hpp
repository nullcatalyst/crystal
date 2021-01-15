#pragma once

#include <chrono>
#include <thread>

#include "steam/isteamnetworkingutils.h"
#include "steam/steamnetworkingsockets.h"
#include "util/msg/msg.hpp"

namespace util::net {

class Network {
protected:
  ISteamNetworkingSockets* lib_;

public:
  Network() noexcept : lib_(SteamNetworkingSockets()) {}

protected:
  void set_callbacks(SteamNetworkingConfigValue_t& opt) noexcept {
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               connection_status_changed_);
  }

  void run_callbacks() noexcept {
    callback_instance_ = this;
    lib_->RunCallbacks();
    callback_instance_ = nullptr;
  }

  // Callbacks.
  virtual void connection_status_changed(
      const SteamNetConnectionStatusChangedCallback_t* info) noexcept {}
  virtual void message_received(const ISteamNetworkingMessage* message) noexcept {}

private:
  static Network* callback_instance_;

  static void connection_status_changed_(SteamNetConnectionStatusChangedCallback_t* info) {
    callback_instance_->connection_status_changed(info);
  }
};

inline void init() {
#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
  SteamDatagramErrMsg err_msg;
  if (!GameNetworkingSockets_Init(nullptr, err_msg)) {
    msg::fatal("initializing game networking sockets: ", err_msg);
  }
#else
  // Just set something for development, doesn't matter what.
  SteamDatagramClient_SetAppID(570);
  // SteamDatagramClient_SetUniverse( k_EUniverseDev );

  SteamDatagramErrMsg err_msg;
  if (!SteamDatagramClient_Init(true, err_msg)) {
    msg::fatal("initializing steam datagram client: ", err_msg);
  }

  // Disable authentication when running with Steam, for this example, since we're not a real app.
  //
  // Authentication is disabled automatically in the open-source version since we don't have a
  // trusted third party to issue certs.
  SteamNetworkingUtils()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_IP_AllowWithoutAuth,
                                                    1);
#endif

  // SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg,
  //                                                DebugOutput);
}

inline void deinit() {
  // Give connections time to finish up. This is an application layer protocol here, it's not TCP.
  // Note that if you have an application and you need to be more sure about cleanup, you won't be
  // able to do this. You will need to send a message and then either wait for the peer to close
  // the connection, or you can pool the connection to see if any reliable data is pending.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
  GameNetworkingSockets_Kill();
#else
  SteamDatagramClient_Kill();
#endif
}

}  // namespace util::net
