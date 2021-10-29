#ifndef COMMUNICATOR_HEADER_HPP
#define COMMUNICATOR_HEADER_HPP 1
#pragma once

#include "mqtt/async_client.h"
#include "mqtt/topic.h"

#include <iostream>

// TODO: maybe use a flat_hash_map for topics
/**
 * @class Communicator
 * @brief Implements the communication functionality via MQTT protocol
 */
class Communicator
{
private:
    // TODO: add setters and getters for these
    const std::string _brokerAddress{"tcp://localhost:1883"};

    // Quality of Service for MQTT comm;
    // 0 means messages are received at most once
    // 1 means messages are received at least once
    // 2 means messages are received exactly once
    /*const*/ int  _qoS     = 1;
    /*const*/ bool _noLocal = true; // we don't want to receive our own messages
    /*const*/ bool _retainData = false; // neither to retain data

    /* options on how to connect to server */
    mqtt::connect_options _connOpts;

    /* async so that operations run in the background */
    mqtt::async_client _client;

    std::unordered_map<std::string, mqtt::topic> _topics;

public:
    Communicator() = default;

    Communicator(const std::string& brokerAddress)
        : _brokerAddress(brokerAddress), _client(brokerAddress, ""), _topics()
    {
    }

    mqtt::async_client& GetClient()
    {
        return _client;
    }

    mqtt::connect_options& GetConnectOptions()
    {
        return _connOpts;
    }

    /* passing 'last will and testament' message (last message ever) to add it */
    void SetDefaultOptions(const char* lwtMessage)
    {
        auto lwt = mqtt::message("info", lwtMessage, _qoS, _retainData);
        _connOpts.set_mqtt_version(MQTTVERSION_5);
        _connOpts.set_keep_alive_interval(20);
        _connOpts.set_clean_start(true);
        _connOpts.set_will_message(std::move(lwt));
        _connOpts.set_automatic_reconnect(true); // maybe required for IoT devices
    }

    void Connect()
    {
        _client.set_connected_handler([](const std::string&) {
            std::cout << "Connection to broker successful!" << std::endl;
        });

        _client.set_connection_lost_handler([](const std::string&) {
            std::cout << "Connection was lost!" << std::endl;
            exit(2);
        });

        try
        {
            _client.connect(_connOpts)->wait();
        }
        catch (const mqtt::exception& ex)
        {
            std::cerr << "Cannot connect. Reason: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Some error" << std::endl;
        }
    }

    void AddTopic(const std::string& topicName)
    {
        if (topicName.empty())
            return;

        auto it = _topics.find(topicName);
        if (it == _topics.end())
        {
            it = _topics
                     .emplace(topicName, mqtt::topic(_client, topicName, _noLocal))
                     .first;
        }

        try
        {
            auto subOpts = mqtt::subscribe_options(_noLocal);
            it->second.subscribe(subOpts)->wait();
        }
        catch (const mqtt::exception& ex)
        {
            std::cerr << "Cannot subscribe. Reason: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Some error" << std::endl;
        }
    }

    // TODO: this would make sense if and only if there was a default topic, else delete it
    void SendMessage()
    {
        // std::this_thread::sleep_for(kPeriod);
        // std::string payload;
        // auto timestamp = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        //                      std::chrono::system_clock::now().time_since_epoch())
        //                      .count();
        // payload += std::to_string(timestamp) + " ";
        // payload += std::to_string(forklift.id) + " ";
        // payload += std::to_string(forklift.x) + " ";
        // payload += std::to_string(forklift.y) + " ";
        // payload += std::to_string(forklift.z);

        // localizationDataTopic.publish(std::move(payload));
    }

    void SendMessage(std::string&& payload, std::string topicName)
    {
        /* Check if client is still connected as this is called separately */
        if (!_client.is_connected())
        {
            return;
        }

        /**
         * We don't want to send messages on any topic
         * We want to enforce authorization, by allowing clients to send
         * messages only on registered topics
         */
        auto it = _topics.find(topicName);
        if (it != _topics.end())
        {
            it->second.publish(std::move(payload));
        }
        else
        {
            throw std::runtime_error("Topic is not registered");
        }
    }

    template <typename DataProcessor>
    void ProcessMessages(std::shared_ptr<DataProcessor> processor)
    {
        _client.start_consuming();

        while (true)
        {
            auto msg = _client.consume_message();
            if (!msg)
                continue;

            if (msg->to_string() == "Sensor Disconnected")
            {
                std::cout << "Exit command received" << std::endl;
                Disconnect();
                break;
            }

            processor->Process(std::move(msg->to_string()));
        }
    }

    void Disconnect(const std::string& msg)
    {
        /* No more data comes */
        for (auto& topic : _topics)
        {
            topic.second.publish(msg);
        }

        Disconnect();
    }

    void Disconnect()
    {
        try
        {
            _client.disconnect()->wait();
            std::cout << "Disconnected" << std::endl;
        }
        catch (const mqtt::exception& ex)
        {
            std::cerr << "Cannot disconnect. Reason: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Some error" << std::endl;
        }
    }
};

#endif /* COMMUNICATOR_HEADER_HPP */