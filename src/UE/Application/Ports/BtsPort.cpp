#include "BtsPort.hpp"
#include "Messages/IncomingMessage.hpp"
#include "Messages/OutgoingMessage.hpp"

namespace ue
{

    BtsPort::BtsPort(common::ILogger &logger, common::ITransport &transport, common::PhoneNumber phoneNumber)
        : logger(logger, "[BTS-PORT]"),
          transport(transport),
          phoneNumber(phoneNumber)
    {
    }

    void BtsPort::start(IBtsEventsHandler &handler)
    {
        transport.registerMessageCallback([this](BinaryMessage msg)
                                          { handleMessage(msg); });
        transport.registerDisconnectedCallback([this]()
                                               { handleDisconnected(); });
        this->handler = &handler;
    }

    void BtsPort::stop()
    {
        transport.registerMessageCallback(nullptr);
        transport.registerDisconnectedCallback(nullptr);
        handler = nullptr;
    }

    void BtsPort::handleMessage(BinaryMessage msg)
    {
        try
        {
            common::IncomingMessage reader{msg};
            auto msgId = reader.readMessageId();
            auto from = reader.readPhoneNumber();
            auto to = reader.readPhoneNumber();

            switch (msgId)
            {
            case common::MessageId::Sib:
            {
                auto btsId = reader.readBtsId();
                handler->handleSib(btsId);
                break;
            }
            case common::MessageId::AttachResponse:
            {
                bool accept = reader.readNumber<std::uint8_t>() != 0u;
                if (accept)
                    handler->handleAttachAccept();
                else
                    handler->handleAttachReject();
                break;
            }
            case common::MessageId::Sms:
            {
                std::string text = reader.readRemainingText();
                if (handler)
                    handler->handleSmsReceived(from, text);
                break;
            }
            case common::MessageId::UnknownRecipient:
            {
                logger.logInfo("Received UnknownRecipient");

                if (handler)
                    handler->handleUnknownRecipient(to);
                break;
            }

            case common::MessageId::CallRequest:
            {
                logger.logInfo("Received CallRequest from: ", from);
                if (handler)
                    handler->handleCallRequest(from);
                break;
            }
            case common::MessageId::CallDropped:
            {
                logger.logInfo("Received CallDropped from: ", from);
                if (handler)
                    handler->handleCallDropped(from);
                break;
            }
            case common::MessageId::CallAccepted:
            {
                logger.logInfo("Received CallAccepted from: ", from);
                if (handler)
                    handler->handleCallAccept(from);
                break;
            }
            case common::MessageId::CallTalk:
            {
                std::string text = reader.readRemainingText();
                if (handler)
                    handler->handleCallTalk(from, text);
                break;
            }
            case common::MessageId::CallReject:
            {
                logger.logInfo("Received CallReject from: ", from);
                if (handler)
                    handler->handleCallReject(from);
                break;
            }

            default:
                logger.logError("unknow message: ", msgId, ", from: ", from);
            }
        }
        catch (std::exception const &ex)
        {
            logger.logError("handleMessage error: ", ex.what());
        }
    }

    void BtsPort::handleDisconnected()
    {
        logger.logInfo("Transport disconnected");
        if (handler)
            handler->handleDisconnected();
    }

    void BtsPort::sendAttachRequest(common::BtsId btsId)
    {
        logger.logDebug("sendAttachRequest: ", btsId);
        common::OutgoingMessage msg{common::MessageId::AttachRequest,
                                    phoneNumber,
                                    common::PhoneNumber{}};
        msg.writeBtsId(btsId);
        transport.sendMessage(msg.getMessage());
    }
    void BtsPort::sendSms(common::PhoneNumber to, const std::string &text)
    {
        logger.logInfo("Sending SMS to: ", to);
        common::OutgoingMessage msg{common::MessageId::Sms,
                                    phoneNumber,
                                    to};
        msg.writeText(text);
        transport.sendMessage(msg.getMessage());
    }

    void BtsPort::sendCallAccept(common::PhoneNumber to)
    {
        logger.logInfo("Sending CallAccept to: ", to);
        common::OutgoingMessage msg{common::MessageId::CallAccepted, phoneNumber, to};
        transport.sendMessage(msg.getMessage());
    }

    void BtsPort::sendCallReject(common::PhoneNumber to)
    {
        logger.logInfo("Sending CallReject to: ", to);
        common::OutgoingMessage msg{common::MessageId::CallReject, phoneNumber, to};
        transport.sendMessage(msg.getMessage());
    }

    void BtsPort::sendCallDropped(common::PhoneNumber to)
    {
        logger.logInfo("Sending CallDropped to: ", to);
        common::OutgoingMessage msg{common::MessageId::CallDropped, phoneNumber, to};
        transport.sendMessage(msg.getMessage());
    }

    void BtsPort::sendCallRequest(common::PhoneNumber to)
    {
        logger.logInfo("Sending CallRequest to: ", to);
        common::OutgoingMessage msg{common::MessageId::CallRequest, phoneNumber, to};
        transport.sendMessage(msg.getMessage());
    }

    void BtsPort::sendCallTalk(common::PhoneNumber to, const std::string &text)
    {
        logger.logInfo("Sending CallTalk to: ", to);
        common::OutgoingMessage msg{common::MessageId::CallTalk, phoneNumber, to};
        msg.writeText(text);
        transport.sendMessage(msg.getMessage());
    }

}
