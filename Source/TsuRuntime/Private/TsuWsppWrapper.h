#pragma once

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#pragma warning(push)
#pragma warning(disable: 4191)
#endif // PLATFORM_WINDOWS

THIRD_PARTY_INCLUDES_START
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"
#include <thread>
THIRD_PARTY_INCLUDES_END

#if PLATFORM_WINDOWS
#pragma warning(pop)
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif // PLATFORM_WINDOWS

namespace wspp = ::websocketpp;

using wspp_server = wspp::server<wspp::config::asio>;
using wspp_message_ptr = wspp_server::message_ptr;
using wspp_connection_ptr = wspp_server::connection_ptr;
