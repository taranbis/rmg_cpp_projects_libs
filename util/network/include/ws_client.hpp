#ifndef _WEB_SOCKET_SERVER_HEADER_HPP_
#define _WEB_SOCKET_SERVER_HEADER_HPP_ 1
#pragma once


	sig<void ()> connected;
	sig<void ()> disconnected;
	sig<void (const QByteArray & data, bool isBinary)> receive;


#endif //! _WEB_SOCKET_SERVER_HEADER_HPP_