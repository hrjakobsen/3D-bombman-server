//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_ASIO_HAS_MOVE 1

#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include <thread>
#include <time.h>
#include <chrono>

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
	virtual ~chat_participant() {}
	virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------
using namespace std;
class chat_room
{
public:
	template <typename T> string tostr(const T& t) { ostringstream os; os << t; return os.str(); }
	int Online = -1;
	int OnlineBeforeStart = 10;

	string BoxOpps = "";

	void BoxOpInit () {
		if (BoxOpps == "") {
			string Comma = "";
			string Numbers[12] = { "0", "1", "2", "3", "4", "0", "0", "0", "0", "0", "0", "0"};
			srand(time(NULL));
			for (int i = 0; i < 250; i++) {
				BoxOpps += Comma + Numbers[rand() % 12];
				Comma = ";";
			}
		}
	}

	chat_message CTS(std::string str) {
		std::string MsgToSend = str;
		char line[chat_message::max_body_length + 1];
		for (int i = 0; i < MsgToSend.length(); i++) {
			line[i] = MsgToSend[i];
		}
		chat_message msg;
		msg.body_length(MsgToSend.length());
		std::memcpy(msg.body(), line, msg.body_length());
		msg.encode_header();
		return msg;
	}
	


	string DP1;
	string DP2;
	string DP3;
	string DP4;



	void join(chat_participant_ptr participant)
	{
		participants_.insert(participant);
		//for (auto msg : recent_msgs_)
			//participant->deliver(msg);
		Online++;
		BoxOpInit();
		if (Online == 1) {
			participant->deliver(CTS("P1;" + BoxOpps));
		} else if (Online == 2) {
			participant->deliver(CTS("P2;" + BoxOpps));
		} else if (Online == 3) {
			participant->deliver(CTS("P3;" + BoxOpps));
		} else if (Online == 4) {
			participant->deliver(CTS("P4;" + BoxOpps));
		} else if (Online == 0) {
			participant->deliver(CTS("P0;" + BoxOpps));
		}
	}

	void leave(chat_participant_ptr participant)
	{
		participants_.erase(participant);
		Online--;
	}

	void deliver(const chat_message& msg)
	{
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
			recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->deliver(msg);
	}
	void C() {
		DP1 = "";
		DP2 = "";
		DP3 = "";
		DP4 = "";
	}
	void Done() {
		if (Online == 1 && DP1 != "") {
			//deliver(CTS("P2"));
			deliver(CTS(DP1));
			C();
		}
		else if (Online == 2 && DP1 != "" && DP2 != "") {
			deliver(CTS(DP1 + ";;;" + DP2));
			C();
		}
		else if (Online == 3 && DP1 != "" && DP2 != "" && DP3 != "") {
			deliver(CTS(DP1 + ";;;" + DP2 + ";;;" + DP3));
			C();
		}
		else if (Online == 4 && DP1 != "" && DP2 != "" && DP3 != "" && DP4 != "") {
			deliver(CTS(DP1 + ";;;" + DP2 + ";;;" + DP3 + ";;;" + DP4));
			C();
		}
	}

	void SetDP1(string str){
		DP1 = str;
		Done();
	}
	void SetDP2(string str){
		DP2 = str;
		Done();
	}
	void SetDP3(string str){
		DP3 = str;
		Done();
	}
	void SetDP4(string str){
		DP4 = str;
		Done();
	}

private:
	std::set<chat_participant_ptr> participants_;
	enum { max_recent_msgs = 100 };
	chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
	: public chat_participant,
	public std::enable_shared_from_this<chat_session>
{
public:
	string Replacer(string Text, string WhatsToRemove, string WhatsToPutIn) {
		string ReturnString;
		for (int i = 0; i <= Text.length() - 1; i++) {
			if (Text.substr(i, WhatsToRemove.length()) == WhatsToRemove) {
				if (WhatsToPutIn != "") {
					ReturnString = ReturnString + WhatsToPutIn;
					i += (int)WhatsToRemove.length() - 1;
				}
			}
			else {
				ReturnString = ReturnString + Text.substr(i, 1);
			}
		}

		return ReturnString;
	}

	int StringCount(string Text, string WhatToCount) {
		if (Text.length() < WhatToCount.length()) {
			return 0;
		}
		int Count = 0;
		for (int i = 0; i < Text.length() - (WhatToCount.length() - 1); i++) {
			if (Text.substr(i, WhatToCount.length()) == WhatToCount) {
				Count++;
			}
		}
		return Count;
	}

	string* Splitter(string Text, string StringToSplitAt) {
		int Length = StringCount(Text, StringToSplitAt);
		string *NewArray = new string[Length + 1];

		int Counter = 0;
		string RemaningText = Text;
		while (StringCount(RemaningText, StringToSplitAt) != 0) {
			NewArray[Counter] = RemaningText.substr(0, RemaningText.find(StringToSplitAt));
			RemaningText = RemaningText.substr(RemaningText.find(StringToSplitAt) + StringToSplitAt.length(), RemaningText.length() - 1);
			Counter++;
		}
		NewArray[Counter] = RemaningText;

		string* Pointer = NewArray;
		return Pointer;
	}
	std::string CTSS(char *Arr, int Length) {
		std::string ReturnS = "";
		for (int i = 0; i < Length; i++) {
			ReturnS += Arr[i];
		}
		return ReturnS;
	}
	chat_session(tcp::socket socket, chat_room& room)
		: socket_(std::move(socket)),
		room_(room)
	{
	}

	void start()
	{
		room_.join(shared_from_this());
		do_read_header();
	}

	void deliver(const chat_message& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
			cout << "Sendt: \n";
		}
	}

private:
	void do_read_header()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), chat_message::header_length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec && read_msg_.decode_header())
			{
				do_read_body();
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	void do_read_body()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{

				//room_.deliver(read_msg_);
				cout << "Modtaget: " << CTSS(read_msg_.body(), read_msg_.body_length()) << "\n";
				string *Data = Splitter(CTSS(read_msg_.body(), read_msg_.body_length()), ";");
				if (Data[0] == "P1") {
					room_.SetDP1(CTSS(read_msg_.body(), read_msg_.body_length()));
				} else if (Data[0] == "P2") {
					room_.SetDP2(CTSS(read_msg_.body(), read_msg_.body_length()));
				} else if (Data[0] == "P3") {
					room_.SetDP3(CTSS(read_msg_.body(), read_msg_.body_length()));
				} else if (Data[0] == "P4") {
					room_.SetDP4(CTSS(read_msg_.body(), read_msg_.body_length()));
				}
				//std::cout << read_msg_.body() << "\n";
				do_read_header();
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	void do_write()
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_msgs_.front().data(),
			write_msgs_.front().length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				write_msgs_.pop_front();
				if (!write_msgs_.empty())
				{
					do_write();
				}
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	tcp::socket socket_;
	chat_room& room_;
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
	chat_server(boost::asio::io_service& io_service,
		const tcp::endpoint& endpoint)
		: acceptor_(io_service, endpoint),
		socket_(io_service)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<chat_session>(std::move(socket_), room_)->start();
			}

			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
	chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	try
	{
		std::cout << "Port nr:\n";
		int Port;
		std::cin >> Port;

		if (argc < 2)
		{
			//std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			//return 1;
		}

		boost::asio::io_service io_service;

		std::list<chat_server> servers;
		tcp::endpoint endpoint(tcp::v4(), Port);
		servers.emplace_back(io_service, endpoint);
		
		std::cout << "\nServer open at:\nIP: ";

		//std::cout << &endpoint.impl_.data_.v4.sin_addr << "H";
		boost::asio::io_service io_service2;
		tcp::resolver resolver(io_service2);
		tcp::resolver::query query(boost::asio::ip::host_name(), "");
		tcp::resolver::iterator iter = resolver.resolve(query);
		tcp::resolver::iterator end; // End marker.
		bool F = true;
		while (iter != end)
		{
			if (F) {
				F = false;
				*iter++;
			}
			else {
				tcp::endpoint ep = *iter++;
				std::cout << ep.address().to_string() << std::endl;
			}
		}
		std::cout << "PORT: " << Port << "\n";

		
		io_service.run();
		//std::cout << "Server open at:" << boost::asio::ip::tcp::v4() << ":" << Port << "\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}